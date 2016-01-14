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
 * TODO!
 *
 * UNIX implementation
 *
 * unixOS.cpp
 *
 * Implementation file for the Posix API.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/data/sarray.h"
#include "xStl/data/datastream.h"
#include "xStl/enc/random.h"
#include "xStl/os/mutex.h"
#include "xStl/os/os.h"
#include "xStl/stream/traceStream.h"

#undef __USE_MISC
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void cOS::memcpy(void* dest,
                 const void* src,
                 uint size
                 /*cWaitable wait*/)
{
    ::memcpy(dest, src, size);
}

void* cOS::smallMemoryAllocation(uint size)
{
    void* ret = malloc(size);
    if (size != 0)
    {
        CHECK(ret != NULL);
    }
    return ret;
}

void* cOS::smallMemoryRealloc(void* mem, uint newSize,
                              bool preserveMemory)
{
    return realloc(mem, newSize);
}

void cOS::smallMemoryFree(void* mem)
{
    free(mem);
}

void cOS::sleepMillisecond(uint milisecond)
{
    sleepMicroseconds(milisecond * 1000);
}
void cOS::sleepSeconds(uint seconds)
{
    sleep(seconds);
}
void cOS::sleepMicroseconds(uint micro)
{
    struct timespec tm = { 0, micro*1000 };
    while (nanosleep(&tm, &tm));
}

cOSDef::systemTime cOS::getSystemTime()
{
    return time(NULL);
}

uint cOS::calculateTimesDiffMilli(cOSDef::systemTime currentTime,
                                  cOSDef::systemTime startTime)
{
    return (uint)((difftime(currentTime, startTime) * 1000));
}

cString cOS::getLastErrorString()
{
    int errnum = errno;
    char error[1024];
    *error = 0;
    strerror_r(errnum, error, sizeof(error));
    cString ret(error);
    ret+= " (";
    ret+= HEXDWORD(errnum);
    ret+= ")";
    return ret;
}

void cOS::debuggerBreak()
{
    #ifdef _DEBUG
    // TODO!
    #endif
}
