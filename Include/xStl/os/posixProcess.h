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

#ifndef __TBA_STL_OS_POSIX_PROCESS_H
#define __TBA_STL_OS_POSIX_PROCESS_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file posixProcess.h
 * @brief Thin POSIX process, signal, time, random and environment services.
 *
 * @details Unlike cOS, which returns cOS-style results and throws, these
 *    services mirror the host calls: a result is either a non-negative value
 *    or -errno, and the caller decides what to do. Unix only; the Windows
 *    build throws EXCEPTION_FAILED from every function.
 *
 *    The layouts of the buffers taken as 'void*' are the host ones (the
 *    'struct rusage', 'struct tms', 'struct sysinfo' of the build target).
 *    Callers that need the Linux kernel layout of another ISA convert them.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/list.h"

/**
 * @brief Host name information, in the layout of 'struct utsname' (six
 *    fields of 65 bytes, each NUL terminated).
 */
class cPosixUtsName
{
public:
    /** @brief Number of bytes per field, including the terminating NUL. */
    enum { FIELD_SIZE = 65 };

    char sysname[FIELD_SIZE];
    char nodename[FIELD_SIZE];
    char release[FIELD_SIZE];
    char version[FIELD_SIZE];
    char machine[FIELD_SIZE];
    char domainname[FIELD_SIZE];
};

/**
 * @brief Process, signal, clock, random and environment services over the
 *    host POSIX interface. All functions are static and thread-safe, as
 *    their host counterparts.
 */
class cPosixProcess
{
public:
    /** @brief setDisposition() dispositions. */
    enum
    {
        DISPOSITION_DEFAULT = 0,
        DISPOSITION_IGNORE = 1
    };

    /** @brief clockNanosleep() flag: the time is absolute (TIMER_ABSTIME). */
    enum { SLEEP_ABSOLUTE = 1 };

    /** @brief getRandom() flags (GRND_NONBLOCK, GRND_RANDOM). */
    enum
    {
        RANDOM_NONBLOCK = 1,
        RANDOM_RANDOM = 2
    };

    //
    // Identity (never fail)
    //

    /** @brief getpid(). */
    static int64 getPid();

    /** @brief getppid(). */
    static int64 getPpid();

    /** @brief getuid(), geteuid(), getgid(), getegid(). */
    static int64 getUid();
    static int64 getEuid();
    static int64 getGid();
    static int64 getEgid();

    //
    // Signals
    //

    /**
     * @brief kill(pid, sig). Signal 0 only probes the process. A negative
     *    pid addresses a process group, as in kill(2).
     * @return 0, or -errno.
     */
    static int kill(int64 pid, int sig);

    /**
     * @brief Set the host disposition of 'sig' to DISPOSITION_DEFAULT or
     *    DISPOSITION_IGNORE.
     * @return 0, or -errno (EINVAL for SIGKILL and SIGSTOP).
     */
    static int setDisposition(int sig, int disposition);

    /**
     * @brief Restore the default disposition of 'sig', unblock it in the
     *    calling thread and raise it on the process. Returns only when the
     *    default action of the signal is to ignore it.
     * @return 0, or -errno when the setup fails.
     */
    static int raiseDefault(int sig);

    //
    // Host information and limits
    //

    /**
     * @brief uname(2).
     * @return 0, or -errno.
     */
    static int uname(cPosixUtsName& name);

    /**
     * @brief getrlimit(2) for 'resource'.
     * @return 0, or -errno.
     */
    static int getRlimit(int resource, uint64& current, uint64& maximum);

    /**
     * @brief getrusage(2). 'buf' must hold 'struct rusage' of the host.
     * @return 0, or -errno.
     */
    static int getRusage(int who, void* buf);

    /**
     * @brief times(2). 'buf' must hold 'struct tms' of the host.
     * @return The elapsed clock ticks, or -errno.
     */
    static int64 times(void* buf);

    /**
     * @brief sysinfo(2). 'buf' must hold 'struct sysinfo' of the host.
     * @return 0, or -errno.
     */
    static int sysinfo(void* buf);

    /**
     * @brief Host CPU affinity mask (sched_getaffinity(2) for the calling
     *    process). Linux only; other hosts return -ENOSYS.
     * @return The number of mask bytes written, or -errno.
     */
    static int64 schedGetAffinity(void* buf, uint64 length);

    /**
     * @brief sched_yield(2).
     * @return 0, or -errno.
     */
    static int schedYield();

    //
    // Clocks and sleeping
    //

    /**
     * @brief clock_gettime(2).
     * @return 0, or -errno.
     */
    static int clockGetTime(int clockId, int64& seconds, int64& nanoseconds);

    /**
     * @brief clock_getres(2).
     * @return 0, or -errno.
     */
    static int clockGetRes(int clockId, int64& seconds, int64& nanoseconds);

    /**
     * @brief clock_nanosleep(2). With SLEEP_ABSOLUTE the time is absolute.
     *    On an interrupted relative sleep the remaining time is stored when
     *    'remainingSeconds' and 'remainingNanoseconds' are not NULL.
     * @return 0, or -errno (-EINTR when a signal interrupted the sleep).
     */
    static int sleep(int clockId, int flags, int64 seconds,
                     int64 nanoseconds, int64* remainingSeconds,
                     int64* remainingNanoseconds);

    //
    // Random
    //

    /**
     * @brief getrandom(2): fill 'buf' with 'length' random bytes. Linux only;
     *    other hosts return -ENOSYS.
     * @return The number of bytes written, or -errno.
     */
    static int64 getRandom(void* buf, uint64 length, uint flags);

    //
    // Environment
    //

    /**
     * @brief Snapshot of the environment as "NAME=value" strings, in the
     *    order of the host environment. 'out' is cleared first.
     * @return 0, or -errno.
     */
    static int getEnvironment(cList<cString>& out);
};

/** @} */

#endif //__TBA_STL_OS_POSIX_PROCESS_H
