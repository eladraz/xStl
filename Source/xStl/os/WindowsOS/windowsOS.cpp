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
    return HeapAlloc(GetProcessHeap(), // Use the default heap manager
                     HEAP_GENERATE_EXCEPTIONS |    // Throws an exception
                     HEAP_ZERO_MEMORY,  // Default security attributes
                     size);     // The number of bytes to be allocated
}

void* cOS::smallMemoryRealloc(void* mem, uint newSize, bool preserveMemory /* = true*/)
{
    if (preserveMemory)
    {
        // This feature is not implemented by the Microsoft heap manager.

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
    return HeapReAlloc(GetProcessHeap(), // Use the default heap manager
                       HEAP_GENERATE_EXCEPTIONS,     // Throws an exception
                       mem,
                       newSize);
}

void cOS::smallMemoryFree(void* mem)
{
    if (HeapFree(GetProcessHeap(), // Use the default heap manager
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
        // Normal Windows operating system
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

    // 100 nanoseconds, nano is 1/1,000,000. micro is 1/1,000. 10 factor
    // 1000 micro is milli, 1000 factor
    return (uint)((currentTime - startTime) / 10000L);
}

// Seconds between the FILETIME epoch (1601-01-01) and the Unix epoch
#define XSTL_FILETIME_TO_UNIX_SECONDS (11644473600LL)

int64 cOS::getEpochSeconds()
{
    // getSystemTime() returns FILETIME units (100 ns since 1601-01-01 UTC)
    uint64 fileTime = getSystemTime();
    return (int64)(fileTime / 10000000ULL) - XSTL_FILETIME_TO_UNIX_SECONDS;
}

uint64 cOS::getProcessCpuMicroseconds()
{
#ifdef XSTL_CE
    // Windows CE has no per-process CPU accounting in this API
    XSTL_THROW(cException, EXCEPTION_FAILED);
#else
    FILETIME creation, exitTime, kernel, user;
    if (!GetProcessTimes(GetCurrentProcess(), &creation, &exitTime,
                         &kernel, &user))
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }

    // Both times are in 100 ns units (FILETIME), 10 units per microsecond
    uint64 kernelUnits = ((uint64)kernel.dwHighDateTime << 32) |
                         (uint64)kernel.dwLowDateTime;
    uint64 userUnits = ((uint64)user.dwHighDateTime << 32) |
                       (uint64)user.dwLowDateTime;
    return (kernelUnits + userUnits) / 10ULL;
#endif // XSTL_CE
}

bool cOS::getEnvironmentVariable(const char* name, cString& value)
{
#ifdef XSTL_CE
    // Windows CE has no environment block: every variable is unset
    return false;
#else
    // The ANSI block is used; the name is ASCII by contract
    for (;;)
    {
        // Query the required size (including the terminating NUL)
        DWORD needed = GetEnvironmentVariableA(name, NULL, 0);
        if (needed == 0)
        {
            // Not set (or the name is invalid)
            return false;
        }

        cSArray<char> buffer(needed);
        buffer[0] = 0;
        SetLastError(ERROR_SUCCESS);
        DWORD got = GetEnvironmentVariableA(name, buffer.getBuffer(), needed);
        if (got == 0)
        {
            // 0 is also the length of a variable set to "": only a missing
            // variable reports ERROR_ENVVAR_NOT_FOUND
            DWORD error = GetLastError();
            if (error == ERROR_ENVVAR_NOT_FOUND)
            {
                return false;
            }
            if (error != ERROR_SUCCESS)
            {
                XSTL_THROW(cException, EXCEPTION_FAILED);
            }
            value = cString(buffer.getBuffer());
            return true;
        }
        if (got < needed)
        {
            // 'got' excludes the terminating NUL
            value = cString(buffer.getBuffer());
            return true;
        }
        // The variable grew between the two calls, query again
    }
#endif // XSTL_CE
}

bool cOS::setEnvironmentVariable(const char* name, const char* value,
                                 bool overwrite)
{
#ifdef XSTL_CE
    // Windows CE has no environment block
    return false;
#else
    if (!overwrite)
    {
        // 0 means unset or invalid name (a variable set to "" reports 1)
        if (GetEnvironmentVariableA(name, NULL, 0) != 0)
        {
            return true;
        }
        if (GetLastError() != ERROR_ENVVAR_NOT_FOUND)
        {
            return false;
        }
    }
    return SetEnvironmentVariableA(name, value) != 0;
#endif // XSTL_CE
}

// FILETIME units (100 ns) from 1601-01-01 to 1970-01-01
#define XSTL_FILETIME_TO_UNIX_100NS (116444736000000000LL)

uint64 cOS::getMonotonicNanoseconds()
{
#ifdef XSTL_CE
    // Milliseconds resolution, the best the CE API offers here
    return (uint64)GetTickCount() * 1000000ULL;
#else
    LARGE_INTEGER frequency, counter;
    if (!QueryPerformanceFrequency(&frequency) ||
        !QueryPerformanceCounter(&counter) ||
        frequency.QuadPart <= 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    // Split the division so that the multiplication cannot overflow
    uint64 ticks = (uint64)counter.QuadPart;
    uint64 hertz = (uint64)frequency.QuadPart;
    return (ticks / hertz) * 1000000000ULL +
           (ticks % hertz) * 1000000000ULL / hertz;
#endif // XSTL_CE
}

int64 cOS::getRealtimeNanoseconds()
{
    // getSystemTime() returns FILETIME units (100 ns since 1601-01-01 UTC)
    uint64 fileTime = getSystemTime();
    return ((int64)fileTime - XSTL_FILETIME_TO_UNIX_100NS) * 100;
}

int64 cOS::forkProcess()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cOS::waitProcess(int64 pid, int options, int& rawStatus, int& osError)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

void cOS::exitImmediately(int status)
{
#ifdef XSTL_CE
    ExitProcess((UINT)status);
#else
    TerminateProcess(GetCurrentProcess(), (UINT)status);
#endif
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
    // If the debugger is not connected then SEH exception will be thrown
    __try {
        DebugBreak();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    #endif
}

#endif // WIN32 - _WIN32_WCE
