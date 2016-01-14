/*
 * Copyright (c) 2008-2016, Integrity Project Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Integrity Project nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 */

/*
 * DDK implementation
 *
 * ntXdkOS.cpp
 *
 * Implementation file for the windows NT DDK.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/data/sarray.h"
#include "xStl/except/exception.h"
#include "xStl/utils/algorithm.h"
#include "xStl/os/mutex.h"
#include "xStl/os/os.h"
#include "xdk/kernel.h"
#include "xdk/utils/processorUtil.h"

//
// Memory functions
//

void cOS::memcpy(void* dest,
                 const void* src,
                 uint size
                 /*cWaitable wait*/)
{
    // reimplementing memcpy to avoid reentry

    // check for overlapping
    if ((src < dest) && ((char *)src + size > dest))
    {
        char * s8 = (char *)src + size - 1;
        char * d8 = (char *)dest + size - 1;

        while (size--)
        {
            *d8-- = *s8--;
        }
    } else {
        char * s8 = (char *)src;
        char * d8 = (char *)dest;

        while (size--)
        {
            *d8++ = *s8++;
        }
    }
}

void* cOS::smallMemoryAllocation(uint size)
{
    void* ret = ExAllocatePool(NonPagedPool, size);

    // Throw exception if the memory cannot be allocated
    if (ret == NULL)
    {
        // Out of memory! Throw exception.
        XSTL_THROW(cException, EXCEPTION_OUT_OF_MEM);
    }

    return ret;
}

void cOS::smallMemoryFree(void* mem)
{
    ExFreePool(mem);
}


//
// Thread functions
//
void cOS::sleepSeconds(uint seconds)
{
    sleepMillisecond(seconds * 1000);
}

void cOS::sleepMillisecond(uint miliseconds)
{
    sleepMicroseconds(miliseconds * 1000);
}

void cOS::sleepMicroseconds(uint microseconds)
{
    // Should we busy wait the process
    bool stall = false;
    uint time = microseconds;

    if (cProcessorUtil::getCurrentIrql() == PASSIVE_LEVEL)
    {
        LARGE_INTEGER waitTime;
        waitTime.QuadPart = microseconds;
        waitTime.QuadPart*= (-10L); // 100 nanoseconds, nano is 1/1,000,000. micro is is 1/1,000.
        NTSTATUS ret = KeDelayExecutionThread(KernelMode, // Prevents recursive behaviour
                                              false, // And cannot be alertable
                                              &waitTime);
        // Test the return code, should we stall the machine?
        if (!NT_SUCCESS(ret))
        {
            stall = true;
        }
    } else
    {
        stall = true;
    }

    if (stall)
    {
        // Dispatch level should be busy wait since context switch are irrelevant
        KeStallExecutionProcessor(time);
    }
}

cOSDef::systemTime cOS::getSystemTime()
{
    LARGE_INTEGER currentTime;

    /*
    // Query the GMT time and adjust it to the local time
    KeQuerySystemTime(&currentTime);
    ExSystemTimeToLocalTime(&currentTime, &currentTime);
    */

    currentTime = KeQueryPerformanceCounter(NULL);

    return currentTime.QuadPart;
}

uint cOS::calculateTimesDiffMilli(cOSDef::systemTime currentTime,
                                  cOSDef::systemTime startTime)
{
    // 100 nanoseconds, nano is 1/1,000,000. micro is is 1/1,000. 10 factor
    // 1000 micro is milli, 1000 factor
    return (uint)((currentTime - startTime) / 10000L);
}

cString cOS::getLastErrorString()
{
    static const cString gMessage(XSTL_STRING("Unknown error occuered."));
    return gMessage;
}

void cOS::debuggerBreak()
{
    // TODO!
}
