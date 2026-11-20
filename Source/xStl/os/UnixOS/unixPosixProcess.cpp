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

// System headers come before xStl/types.h ('uint' macro clash)
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/list.h"
#include "xStl/os/posixProcess.h"

// The host environment block (POSIX environ)
extern char** environ;

// Host errno as the negative result convention of cPosixProcess
#define POSIX_RESULT_ERRNO() (-(int)errno)

// Fills 'field' from a NUL terminated host field, at most FIELD_SIZE - 1 bytes
static void copyUtsField(char* field, const char* source)
{
    strncpy(field, source, cPosixUtsName::FIELD_SIZE - 1);
    field[cPosixUtsName::FIELD_SIZE - 1] = 0;
}

int64 cPosixProcess::getPid()
{
    return (int64)getpid();
}

int64 cPosixProcess::getPpid()
{
    return (int64)getppid();
}

int64 cPosixProcess::getUid()
{
    return (int64)getuid();
}

int64 cPosixProcess::getEuid()
{
    return (int64)geteuid();
}

int64 cPosixProcess::getGid()
{
    return (int64)getgid();
}

int64 cPosixProcess::getEgid()
{
    return (int64)getegid();
}

int cPosixProcess::kill(int64 pid, int sig)
{
    if (::kill((pid_t)pid, sig) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return 0;
}

int cPosixProcess::setDisposition(int sig, int disposition)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_handler = (disposition == DISPOSITION_IGNORE) ? SIG_IGN : SIG_DFL;
    if (sigaction(sig, &action, NULL) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return 0;
}

int cPosixProcess::raiseDefault(int sig)
{
    int result = setDisposition(sig, DISPOSITION_DEFAULT);
    if (result != 0)
    {
        return result;
    }

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    if (sigprocmask(SIG_UNBLOCK, &set, NULL) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    if (raise(sig) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return 0;
}

int cPosixProcess::uname(cPosixUtsName& name)
{
    struct utsname host;
    if (::uname(&host) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    copyUtsField(name.sysname, host.sysname);
    copyUtsField(name.nodename, host.nodename);
    copyUtsField(name.release, host.release);
    copyUtsField(name.version, host.version);
    copyUtsField(name.machine, host.machine);
#ifdef __USE_GNU
    copyUtsField(name.domainname, host.domainname);
#else
    name.domainname[0] = 0;
#endif
    return 0;
}

int cPosixProcess::getRlimit(int resource, uint64& current, uint64& maximum)
{
    struct rlimit limit;
#if defined(__GLIBC__)
    // glibc declares the resource as an enum in C++ builds
    int status = getrlimit((__rlimit_resource_t)resource, &limit);
#else
    int status = getrlimit(resource, &limit);
#endif
    if (status != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    current = (uint64)limit.rlim_cur;
    maximum = (uint64)limit.rlim_max;
    return 0;
}

int cPosixProcess::getRusage(int who, void* buf)
{
    if (getrusage(who, (struct rusage*)buf) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return 0;
}

int64 cPosixProcess::times(void* buf)
{
    clock_t ticks = ::times((struct tms*)buf);
    if (ticks == (clock_t)-1)
    {
        return POSIX_RESULT_ERRNO();
    }
    return (int64)ticks;
}

int cPosixProcess::sysinfo(void* buf)
{
    if (::sysinfo((struct sysinfo*)buf) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return 0;
}

int64 cPosixProcess::schedGetAffinity(void* buf, uint64 length)
{
#ifdef __linux__
    long ret = syscall(SYS_sched_getaffinity, 0, (size_t)length, buf);
    if (ret < 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return (int64)ret;
#else
    (void)buf;
    (void)length;
    return -ENOSYS;
#endif
}

int cPosixProcess::schedYield()
{
    if (sched_yield() != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return 0;
}

int cPosixProcess::clockGetTime(int clockId, int64& seconds,
                                int64& nanoseconds)
{
    struct timespec now;
    if (clock_gettime((clockid_t)clockId, &now) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    seconds = (int64)now.tv_sec;
    nanoseconds = (int64)now.tv_nsec;
    return 0;
}

int cPosixProcess::clockGetRes(int clockId, int64& seconds,
                               int64& nanoseconds)
{
    struct timespec resolution;
    if (clock_getres((clockid_t)clockId, &resolution) != 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    seconds = (int64)resolution.tv_sec;
    nanoseconds = (int64)resolution.tv_nsec;
    return 0;
}

int cPosixProcess::sleep(int clockId, int flags, int64 seconds,
                         int64 nanoseconds, int64* remainingSeconds,
                         int64* remainingNanoseconds)
{
    struct timespec request;
    struct timespec remaining;
    request.tv_sec = (time_t)seconds;
    request.tv_nsec = (long)nanoseconds;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 0;

    int hostFlags = (flags & SLEEP_ABSOLUTE) ? TIMER_ABSTIME : 0;
    // clock_nanosleep returns the error number itself, not -1 with errno
    int error = clock_nanosleep((clockid_t)clockId, hostFlags, &request,
                                &remaining);
    if (error != 0)
    {
        if (remainingSeconds != NULL && remainingNanoseconds != NULL)
        {
            *remainingSeconds = (int64)remaining.tv_sec;
            *remainingNanoseconds = (int64)remaining.tv_nsec;
        }
        return -error;
    }
    return 0;
}

int64 cPosixProcess::getRandom(void* buf, uint64 length, uint flags)
{
#ifdef __linux__
    long ret = syscall(SYS_getrandom, buf, (size_t)length, flags);
    if (ret < 0)
    {
        return POSIX_RESULT_ERRNO();
    }
    return (int64)ret;
#else
    (void)buf;
    (void)length;
    (void)flags;
    return -ENOSYS;
#endif
}

int cPosixProcess::getEnvironment(cList<cString>& out)
{
    out.removeAll();
    if (environ == NULL)
    {
        return 0;
    }
    for (char** entry = environ; *entry != NULL; entry++)
    {
        out.append(cString(*entry));
    }
    return 0;
}
