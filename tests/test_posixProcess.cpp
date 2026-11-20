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

// System headers come first: xStl/types.h defines the 'uint' macro.
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#define POSIX_TEST_UNIX (0)
#else
#define POSIX_TEST_UNIX (1)
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#ifdef __linux__
#include <sys/sysinfo.h>
#endif
#include <sys/times.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#endif

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/posixProcess.h"
#include "xStl/data/string.h"
#include "xStl/data/list.h"
#include "tests.h"

// Exit status of the forked child
#define POSIX_TEST_CHILD_STATUS (7)

// Relative sleep used to measure the monotonic clock: 1 ms
#define POSIX_TEST_SLEEP_NS (1000000LL)

class cTestPosixProcess : public cTestObject {
public:
        void test_identity()
    {
        TESTS_ASSERT(cPosixProcess::getPid() > 0);
        TESTS_ASSERT(cPosixProcess::getUid() >= 0);
        TESTS_ASSERT(cPosixProcess::getEuid() >= 0);
        TESTS_ASSERT(cPosixProcess::getGid() >= 0);
        TESTS_ASSERT(cPosixProcess::getEgid() >= 0);
#if POSIX_TEST_UNIX
        TESTS_ASSERT(cPosixProcess::getPid() == (int64)getpid());
        TESTS_ASSERT(cPosixProcess::getPpid() == (int64)getppid());
        TESTS_ASSERT(cPosixProcess::kill(cPosixProcess::getPid(), 0) == 0);
#endif
    }

#if POSIX_TEST_UNIX
        void test_childSeesParent()
    {
        int64 parent = cPosixProcess::getPid();
        int64 pid = cOS::forkProcess();
        TESTS_ASSERT(pid >= 0);
        if (pid == 0)
        {
            cOS::exitImmediately(cPosixProcess::getPpid() == parent ? 0 : 1);
        }

        int rawStatus = 0;
        int osError = -1;
        TESTS_ASSERT(cOS::waitProcess(pid, 0, rawStatus, osError) == pid);
        TESTS_ASSERT(osError == 0);
        TESTS_ASSERT(WIFEXITED(rawStatus));
        TESTS_ASSERT(WEXITSTATUS(rawStatus) == 0);
    }

        void test_ignoredSignalSurvives()
    {
        TESTS_ASSERT(cPosixProcess::setDisposition(
            SIGUSR1, cPosixProcess::DISPOSITION_IGNORE) == 0);
        TESTS_ASSERT(cPosixProcess::kill(cPosixProcess::getPid(), SIGUSR1) == 0);
        // Reached only because the ignored signal did not end the process
        TESTS_ASSERT(cPosixProcess::setDisposition(
            SIGUSR1, cPosixProcess::DISPOSITION_DEFAULT) == 0);
    }

        void test_sigkillDispositionRejected()
    {
        TESTS_ASSERT(cPosixProcess::setDisposition(
            SIGKILL, cPosixProcess::DISPOSITION_IGNORE) == -EINVAL);
    }

        void test_raiseDefaultInChild()
    {
        int64 pid = cOS::forkProcess();
        TESTS_ASSERT(pid >= 0);
        if (pid == 0)
        {
            cPosixProcess::raiseDefault(SIGABRT);
            // Reached only if the default action of SIGABRT did not end it
            cOS::exitImmediately(POSIX_TEST_CHILD_STATUS);
        }

        int rawStatus = 0;
        int osError = -1;
        TESTS_ASSERT(cOS::waitProcess(pid, 0, rawStatus, osError) == pid);
        TESTS_ASSERT(osError == 0);
        TESTS_ASSERT(WIFSIGNALED(rawStatus));
        TESTS_ASSERT(WTERMSIG(rawStatus) == SIGABRT);
    }

        void test_uname()
    {
        cPosixUtsName name;
        TESTS_ASSERT(cPosixProcess::uname(name) == 0);
        TESTS_ASSERT(name.machine[0] != 0);
#if defined(__linux__)
        TESTS_ASSERT(strcmp(name.sysname, "Linux") == 0);
#endif
    }

        void test_rlimit()
    {
        uint64 current = 0;
        uint64 maximum = 0;
        TESTS_ASSERT(cPosixProcess::getRlimit(RLIMIT_NOFILE,
                                              current, maximum) == 0);
        TESTS_ASSERT(current > 0);
        TESTS_ASSERT(current <= maximum);
    }

        void test_hostStructures()
    {
        struct rusage usage;
        TESTS_ASSERT(cPosixProcess::getRusage(RUSAGE_SELF, &usage) == 0);

        struct tms ticks;
        TESTS_ASSERT(cPosixProcess::times(&ticks) >= 0);

#if defined(__linux__)
        struct sysinfo info;
        TESTS_ASSERT(cPosixProcess::sysinfo(&info) == 0);
        TESTS_ASSERT(info.uptime >= 0);
#else
        TESTS_ASSERT(cPosixProcess::sysinfo(NULL) == -ENOSYS);
#endif

        TESTS_ASSERT(cPosixProcess::schedYield() == 0);
    }

        void test_affinity()
    {
        unsigned char mask[128];
        memset(mask, 0, sizeof(mask));
#if defined(__linux__)
        int64 bytes = cPosixProcess::schedGetAffinity(mask, sizeof(mask));
        TESTS_ASSERT(bytes > 0);
#else
        TESTS_ASSERT(cPosixProcess::schedGetAffinity(mask, sizeof(mask)) ==
                     -ENOSYS);
#endif
    }

        void test_monotonicSleep()
    {
        int64 seconds = 0;
        int64 nanoseconds = 0;
        TESTS_ASSERT(cPosixProcess::clockGetRes(CLOCK_MONOTONIC,
                                                seconds, nanoseconds) == 0);
        TESTS_ASSERT(seconds > 0 || nanoseconds > 0);

        TESTS_ASSERT(cPosixProcess::clockGetTime(CLOCK_MONOTONIC,
                                                 seconds, nanoseconds) == 0);
        int64 start = seconds * 1000000000LL + nanoseconds;

#if defined(__linux__)
        TESTS_ASSERT(cPosixProcess::sleep(CLOCK_MONOTONIC, 0, 0,
                                          POSIX_TEST_SLEEP_NS,
                                          NULL, NULL) == 0);
#else
        TESTS_ASSERT(cPosixProcess::sleep(CLOCK_MONOTONIC, 0, 0,
                                          POSIX_TEST_SLEEP_NS,
                                          NULL, NULL) == -ENOSYS);
        return;
#endif

        TESTS_ASSERT(cPosixProcess::clockGetTime(CLOCK_MONOTONIC,
                                                 seconds, nanoseconds) == 0);
        int64 end = seconds * 1000000000LL + nanoseconds;
        TESTS_ASSERT(end - start >= POSIX_TEST_SLEEP_NS);
    }
#else
        void test_unsupportedOnWindows()
    {
        // Every service throws on Windows, as the process services of cOS
        int64 seconds = 0;
        int64 nanoseconds = 0;
        TESTS_EXCEPTION(cPosixProcess::getPid());
        TESTS_EXCEPTION(cPosixProcess::clockGetTime(0, seconds, nanoseconds));
    }
#endif

        void test_random()
    {
        unsigned char buffer[16];
        memset(buffer, 0, sizeof(buffer));
#if POSIX_TEST_UNIX && defined(__linux__)
        TESTS_ASSERT(cPosixProcess::getRandom(buffer, sizeof(buffer), 0) ==
                     (int64)sizeof(buffer));
#elif POSIX_TEST_UNIX
        TESTS_ASSERT(cPosixProcess::getRandom(buffer, sizeof(buffer), 0) ==
                     -ENOSYS);
#else
        TESTS_EXCEPTION(cPosixProcess::getRandom(buffer, sizeof(buffer), 0));
#endif
    }

        void test_environment()
    {
        // The variable stays in the environment for the rest of the run
        TESTS_ASSERT(cOS::setEnvironmentVariable("XSTL_P8", "1", true));

        cList<cString> entries;
        TESTS_ASSERT(cPosixProcess::getEnvironment(entries) == 0);

        bool found = false;
        for (cList<cString>::iterator i = entries.begin();
                                      i != entries.end();
                                      i++)
        {
            if (*i == cString(XSTL_STRING("XSTL_P8=1")))
            {
                found = true;
            }
        }
        TESTS_ASSERT(found);
    }

    // Perform the test
    virtual void test()
    {
#if POSIX_TEST_UNIX
        test_identity();
        test_random();
        test_environment();
        test_childSeesParent();
        test_ignoredSignalSurvives();
        test_sigkillDispositionRejected();
        test_raiseDefaultInChild();
        test_uname();
        test_rlimit();
        test_hostStructures();
        test_affinity();
        test_monotonicSleep();
#else
        test_unsupportedOnWindows();
#endif
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestPosixProcess g_globalTestPosixProcess;
