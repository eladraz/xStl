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
 * test_osServices.cpp
 *
 * Tests the process-level OS services of cOS:
 *  - getEnvironmentVariable: a variable known to be set is found with a
 *    non-empty value, and an unset variable is reported as missing without
 *    changing the output value.
 *  - getEpochSeconds: within a few seconds of time(NULL).
 *  - getProcessCpuMicroseconds: monotonic, and it advances after a busy loop.
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"
#include <time.h>

// Variables that are set on every supported system
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#define OS_TEST_PRESENT_VARIABLE "SystemRoot"
#else
#define OS_TEST_PRESENT_VARIABLE "HOME"
#endif
#define OS_TEST_MISSING_VARIABLE "XSTL_NO_SUCH_VAR_42"

// Allowed clock difference between cOS::getEpochSeconds() and time(NULL)
#define OS_TEST_EPOCH_SLACK (5)

class cTestOSServices : public cTestObject {
public:
    // Known variable is found, unknown variable is not and keeps the output
    void test_environmentVariable()
    {
        cString value;
        TESTS_ASSERT(cOS::getEnvironmentVariable(
            OS_TEST_PRESENT_VARIABLE, value));
        TESTS_ASSERT(value.length() > 0);

        cString untouched(XSTL_STRING("untouched"));
        TESTS_ASSERT(!cOS::getEnvironmentVariable(
            OS_TEST_MISSING_VARIABLE, untouched));
        TESTS_ASSERT(untouched == cString(XSTL_STRING("untouched")));
    }

    // The epoch seconds agree with the C library clock
    void test_epochSeconds()
    {
        int64 before = (int64)time(NULL);
        int64 epoch = cOS::getEpochSeconds();
        int64 after = (int64)time(NULL);

        TESTS_ASSERT(epoch >= before - OS_TEST_EPOCH_SLACK);
        TESTS_ASSERT(epoch <= after + OS_TEST_EPOCH_SLACK);
    }

    // The process CPU time never decreases and grows after a busy loop. The
    // clock granularity can be coarse, so the loop is repeated (bounded)
    // until the clock moves.
    void test_processCpuTime()
    {
        uint64 start = cOS::getProcessCpuMicroseconds();
        uint64 last = start;
        bool grown = false;

        volatile uint64 sink = 0;
        for (uint round = 0; round < 200 && !grown; round++)
        {
            for (uint i = 0; i < 2000000; i++)
            {
                sink = sink + (i ^ round);
            }
            uint64 now = cOS::getProcessCpuMicroseconds();
            TESTS_ASSERT(now >= last);
            last = now;
            grown = now > start;
        }
        TESTS_ASSERT(grown);
    }

    // Perform the test
    virtual void test()
    {
        test_environmentVariable();
        test_epochSeconds();
        test_processCpuTime();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestOSServices g_globalTestOSServices;
