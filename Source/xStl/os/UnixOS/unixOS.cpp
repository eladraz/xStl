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
// System headers come before xStl/types.h ('uint' macro clash)
#include <sys/types.h>
#include <sys/wait.h>

#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/except/exception.h"
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

void cOS::sleepMillisecond(uint millisecond)
{
    sleepMicroseconds(millisecond * 1000);
}
void cOS::sleepSeconds(uint seconds)
{
    sleep(seconds);
}
void cOS::sleepMicroseconds(uint micro)
{
    struct timespec tm;
    tm.tv_sec = (time_t)(micro / 1000000);
    tm.tv_nsec = (long)((micro % 1000000) * 1000);
    // Resume after signal interruption; any other error aborts the sleep
    while ((nanosleep(&tm, &tm) != 0) && (errno == EINTR));
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

int64 cOS::getEpochSeconds()
{
    return (int64)time(NULL);
}

uint64 cOS::getProcessCpuMicroseconds()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    return (uint64)ts.tv_sec * 1000000ULL + (uint64)(ts.tv_nsec / 1000);
}

bool cOS::getEnvironmentVariable(const char* name, cString& value)
{
    const char* found = getenv(name);
    if (found == NULL)
    {
        return false;
    }
    value = cString(found);
    return true;
}

bool cOS::setEnvironmentVariable(const char* name, const char* value,
                                 bool overwrite)
{
    // An existing variable is kept (and still present) when overwrite is false
    if (!overwrite && getenv(name) != NULL)
    {
        return true;
    }
    return setenv(name, value, 1) == 0;
}

uint64 cOS::getMonotonicNanoseconds()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    return (uint64)ts.tv_sec * 1000000000ULL + (uint64)ts.tv_nsec;
}

int64 cOS::getRealtimeNanoseconds()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    return (int64)ts.tv_sec * 1000000000LL + (int64)ts.tv_nsec;
}

int64 cOS::forkProcess()
{
    pid_t pid = fork();
    return (int64)pid;
}

int64 cOS::waitProcess(int64 pid, int options, int& rawStatus, int& osError)
{
    int flags = (options & WAIT_NO_HANG) ? WNOHANG : 0;
    for (;;)
    {
        int status = 0;
        pid_t reaped = waitpid((pid_t)pid, &status, flags);
        if (reaped < 0 && errno == EINTR)
        {
            continue;
        }
        if (reaped < 0)
        {
            osError = errno;
            return -1;
        }
        osError = 0;
        if (reaped > 0)
        {
            rawStatus = status;
        }
        return (int64)reaped;
    }
}

void cOS::exitImmediately(int status)
{
    _exit(status);
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
