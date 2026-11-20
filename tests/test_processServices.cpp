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
 * test_processServices.cpp
 *
 * Process (Unix only; Windows throws), environment and clock services of cOS.
 * A forked child only calls exitImmediately or raise, never returning into
 * the harness.
 */

// System headers come first: xStl/types.h defines the 'uint' macro.
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#define PROCESS_TEST_UNIX (0)
#else
#define PROCESS_TEST_UNIX (1)
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#endif

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/string.h"
#include "tests.h"
#include <time.h>

// Exit statuses of the forked children; the sleeper runs for 200 ms
#define PROCESS_TEST_CHILD_STATUS (7)
#define PROCESS_TEST_SLEEPER_STATUS (3)

#define PROCESS_TEST_SLEEP_MS (200)

#define PROCESS_TEST_VARIABLE_PREFIX "XSTL_TEST_PROCESS_VAR_"

// Allowed clock difference between getRealtimeNanoseconds() and time(NULL)
#define PROCESS_TEST_EPOCH_SLACK (5)

// Unique variable name (48 bytes); the variable stays in the environment
static void makeVariableName(char* out)
{
    static const char hex[] = "0123456789abcdef";
    uint64 stamp = cOS::getMonotonicNanoseconds();
    uint n = 0;
    for (; PROCESS_TEST_VARIABLE_PREFIX[n] != 0; n++)
    {
        out[n] = PROCESS_TEST_VARIABLE_PREFIX[n];
    }
    for (uint i = 0; i < 16; i++)
    {
        out[n + i] = hex[(stamp >> (60 - 4 * i)) & 0xF];
    }
    out[n + 16] = 0;
}

class cTestProcessServices : public cTestObject {
public:
        void test_forkAndWait()
    {
#if PROCESS_TEST_UNIX
        int64 pid = cOS::forkProcess();
        TESTS_ASSERT(pid >= 0);
        if (pid == 0)
        {
            cOS::exitImmediately(PROCESS_TEST_CHILD_STATUS);
        }

        int rawStatus = 0;
        int osError = -1;
        TESTS_ASSERT(cOS::waitProcess(pid, 0, rawStatus, osError) == pid);
        TESTS_ASSERT(osError == 0);
        TESTS_ASSERT(WIFEXITED(rawStatus));
        TESTS_ASSERT(WEXITSTATUS(rawStatus) == PROCESS_TEST_CHILD_STATUS);
#else
        // Windows has no fork and no waitProcess: both report it
        int rawStatus = 0;
        int osError = 0;
        TESTS_EXCEPTION(cOS::forkProcess());
        TESTS_EXCEPTION(cOS::waitProcess(-1, 0, rawStatus, osError));
#endif
    }

#if PROCESS_TEST_UNIX
        void test_waitNoHang()
    {
        int64 pid = cOS::forkProcess();
        TESTS_ASSERT(pid >= 0);
        if (pid == 0)
        {
            cOS::sleepMillisecond(PROCESS_TEST_SLEEP_MS);
            cOS::exitImmediately(PROCESS_TEST_SLEEPER_STATUS);
        }

        int rawStatus = 0;
        int osError = -1;
        TESTS_ASSERT(cOS::waitProcess(pid, cOS::WAIT_NO_HANG,
                                      rawStatus, osError) == 0);
        TESTS_ASSERT(osError == 0);

        TESTS_ASSERT(cOS::waitProcess(pid, 0, rawStatus, osError) == pid);
        TESTS_ASSERT(osError == 0);
        TESTS_ASSERT(WIFEXITED(rawStatus));
        TESTS_ASSERT(WEXITSTATUS(rawStatus) == PROCESS_TEST_SLEEPER_STATUS);
    }

        void test_killedBySignal()
    {
        int64 pid = cOS::forkProcess();
        TESTS_ASSERT(pid >= 0);
        if (pid == 0)
        {
            raise(SIGKILL);
            // Reached only if the signal did not kill the child
            cOS::exitImmediately(PROCESS_TEST_CHILD_STATUS);
        }

        int rawStatus = 0;
        int osError = -1;
        TESTS_ASSERT(cOS::waitProcess(pid, 0, rawStatus, osError) == pid);
        TESTS_ASSERT(osError == 0);
        TESTS_ASSERT(WIFSIGNALED(rawStatus));
        TESTS_ASSERT(WTERMSIG(rawStatus) == SIGKILL);
    }

        void test_noChildrenLeft()
    {
        int rawStatus = 0;
        int osError = 0;
        TESTS_ASSERT(cOS::waitProcess(-1, 0, rawStatus, osError) == -1);
        TESTS_ASSERT(osError == ECHILD);
    }
#endif

        void test_setEnvironmentVariable()
    {
        char name[48];
        makeVariableName(name);

        cString value;
        TESTS_ASSERT(cOS::setEnvironmentVariable(name, "first", false));
        TESTS_ASSERT(cOS::getEnvironmentVariable(name, value));
        TESTS_ASSERT(value == cString(XSTL_STRING("first")));

        // Without overwrite the variable is present and keeps its value
        TESTS_ASSERT(cOS::setEnvironmentVariable(name, "second", false));
        TESTS_ASSERT(cOS::getEnvironmentVariable(name, value));
        TESTS_ASSERT(value == cString(XSTL_STRING("first")));

        // With overwrite the value is replaced
        TESTS_ASSERT(cOS::setEnvironmentVariable(name, "second", true));
        TESTS_ASSERT(cOS::getEnvironmentVariable(name, value));
        TESTS_ASSERT(value == cString(XSTL_STRING("second")));
    }

        void test_monotonicClock()
    {
        uint64 start = cOS::getMonotonicNanoseconds();
        uint64 last = start;
        for (uint round = 0; round < 10; round++)
        {
            cOS::sleepMillisecond(1);
            uint64 now = cOS::getMonotonicNanoseconds();
            TESTS_ASSERT(now >= last);
            last = now;
        }
        // Ten 1 ms sleeps: at least 5 ms must have been measured
        TESTS_ASSERT(last - start >= 5000000ULL);
    }

        void test_realtimeClock()
    {
        int64 before = (int64)time(NULL);
        int64 realtime = cOS::getRealtimeNanoseconds();
        int64 after = (int64)time(NULL);

        int64 seconds = realtime / 1000000000LL;
        TESTS_ASSERT(seconds >= before - PROCESS_TEST_EPOCH_SLACK);
        TESTS_ASSERT(seconds <= after + PROCESS_TEST_EPOCH_SLACK);
    }

    // Perform the test
    virtual void test()
    {
#if PROCESS_TEST_UNIX
        test_forkAndWait();
        test_waitNoHang();
        test_killedBySignal();
        test_noChildrenLeft();
#else
        test_forkAndWait();
#endif
        test_setEnvironmentVariable();
        test_monotonicClock();
        test_realtimeClock();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestProcessServices g_globalTestProcessServices;
