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

#include "xStl/xStlPrecompiled.h"
// For this files, the WIN32 or the _WIN32_WCE macro must be defined.
// Only Windows operating system or Windows CE can compile with this API
#if defined(WIN32) || defined(_WIN32_WCE)

/*
 * W I N D O W S implementation
 *
 * windowsOS.cpp
 *
 * Implementation file for the Win32 API.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/sarray.h"
#include "xStl/data/datastream.h"
#include "xStl/except/exception.h"
#include "xStl/utils/algorithm.h"
#include "xStl/os/mutex.h"
#include "xStl/os/os.h"
#include <windows.h>

//
// Memory functions
//

void cOS::memcpy(void* dest,
                 const void* src,
                 uint size
                 /*cWaitable wait*/)
{
    // Used to be libC memcpy function.
    ::MoveMemory(dest, src, size);
}

void* cOS::smallMemoryAllocation(uint size)
{
    return HeapAlloc(GetProcessHeap(), // Use the deafult heap manager
                     HEAP_GENERATE_EXCEPTIONS |    // Thorws exception
                     HEAP_ZERO_MEMORY,  // Default security attributes
                     size);     // The number of bytes to be allocated
}

void* cOS::smallMemoryRealloc(void* mem, uint newSize, bool preserveMemory /* = true*/)
{
    if (preserveMemory)
    {
        // This feature is not implement by microsoft Heap manager.

        // Allocate new chunk of memory
        void* newMemory = smallMemoryAllocation(newSize);
        // Calculate the movement size
        uint size = (uint)HeapSize(GetProcessHeap(), 0, mem);
        size = t_min(newSize, size);
        // Fast copy the memory
        cOS::memcpy(newMemory, mem, size);
        // Free the old memory
        smallMemoryFree(mem);
        // Return the new memory
        return newMemory;
    }

    // Use the HeapReAlloc...
    return HeapReAlloc(GetProcessHeap(), // Use the deafult heap manager
                       HEAP_GENERATE_EXCEPTIONS,     // Throws exception
                       mem,
                       newSize);
}

void cOS::smallMemoryFree(void* mem)
{
    if (HeapFree(GetProcessHeap(), // Use the deafult heap manager
                 0,
                 mem) == 0)
    {
        // Exception
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
}



//
// Thread functions
//
void cOS::sleepSeconds(uint seconds)
{
    ::Sleep(seconds * 1000);
}

void cOS::sleepMillisecond(uint milliseconds)
{
    ::Sleep(milliseconds);
}

void cOS::sleepMicroseconds(uint microseconds)
{
    ::Sleep(microseconds / 1000);
}

cOSDef::systemTime cOS::getSystemTime()
{
    FILETIME localTime;
    #ifdef XSTL_CE
        SYSTEMTIME sysTime;
        GetSystemTime(&sysTime);
        SystemTimeToFileTime(&sysTime, &localTime);
    #else
        // Normal windows operating system
        GetSystemTimeAsFileTime(&localTime);
    #endif

    // Handles the return type
    uint64 ret = localTime.dwHighDateTime;
    ret = ret << 32;
    ret|= localTime.dwLowDateTime;

    return ret;
}

uint cOS::calculateTimesDiffMilli(cOSDef::systemTime currentTime,
                                  cOSDef::systemTime startTime)
{
    // UTC format

    // 100 nanoseconds, nano is 1/1,000,000. micro is is 1/1,000. 10 factor
    // 1000 micro is milli, 1000 factor
    return (uint)((currentTime - startTime) / 10000L);
}

cString cOS::getLastErrorString()
{
    // Query error code
    uint getLastError = GetLastError();

    // Format to ASCII-UNICODE string
    character* lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS, NULL, getLastError,
                  0, // Default language
                  (LPWSTR)(&lpMsgBuf), 0, NULL);

    // Builds cString object
    cString ret(lpMsgBuf);

    // Free resources
    LocalFree(lpMsgBuf);

    ret+= XSTL_STRING(" (");
    ret+= HEXDWORD(getLastError);
    ret+= XSTL_STRING(")");

    return ret;
}

void cOS::debuggerBreak()
{
    #ifdef _DEBUG
    // If the debugger is not connected then SEH exception will be throw
    __try {
        DebugBreak();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    #endif
}

#endif // WIN32 - _WIN32_WCE
