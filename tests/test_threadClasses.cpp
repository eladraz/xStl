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
 * test_threadClasses.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/stream/ioStream.h"
#include "xStl/os/threadedClass.h"
#include "xStl/os/os.h"
#include "../../xStl/tests/tests.h"

class cTestThreadClasses : public cTestObject {
public:
    class MyThreadTesting : public cThreadedClass {
    public:
        // The number of ticks to be counting for
        enum { COUNT_SIZE = 30 };
        // Ctor
        MyThreadTesting(uint id) : m_id(id), m_counter(0) {};
        // The testing uint.
        virtual void run() {
            for (uint i = 0; i < COUNT_SIZE; i++)
            {
                m_counter++;
                cOS::sleepMillisecond(1);
            }

        }
        // The ID
        volatile uint m_id;
        // The countable
        volatile uint m_counter;
    };

    // Premature death. Execute thread
    void testPrematureDeath()
    {
        uint i;
        for (i = 0; i < 100; i++) {
            MyThreadTesting thread(i);
            TESTS_ASSERT_EQUAL(thread.m_id, i);
            TESTS_ASSERT_EQUAL(thread.m_counter, 0);
        }

        bool wasIncrease = false;
        for (i = 0; i < 100; i++) {
            MyThreadTesting thread(i);
            TESTS_ASSERT_EQUAL(thread.m_id, i);
            TESTS_ASSERT_EQUAL(thread.m_counter, 0);
            // Dtor will be called here!
            thread.start();
            TESTS_ASSERT_EQUAL(thread.m_id, i);
            if (thread.m_counter != 0)
                wasIncrease = true;
        }

        if (wasIncrease) {
            cout << " *** ThreadTest: Enabled." << endl;
        }
    }

    #define THREAD_SCHEDULING_NUMBER (30)
    void testThreadSchedule()
    {
        // Allocated threads
        MyThreadTesting* threads[THREAD_SCHEDULING_NUMBER];
        uint i;
        for (i = 0; i < THREAD_SCHEDULING_NUMBER; i++)
        {
            threads[i] = new MyThreadTesting(i);
            threads[i]->start();
        }

        // Wait until the first thread will be terminated...
        threads[0]->getThreadHandle().wait();
        TESTS_ASSERT_EQUAL(threads[0]->m_id, 0);
        TESTS_ASSERT_EQUAL(threads[0]->m_counter, MyThreadTesting::COUNT_SIZE);
        TESTS_ASSERT(threads[0]->isDone());

        // Destroy threads
        bool wasPremature = false;
        for (i = 0; i < THREAD_SCHEDULING_NUMBER; i++)
        {
            if (threads[i]->m_counter != MyThreadTesting::COUNT_SIZE)
                wasPremature = true;

            delete threads[i];
        }

        if (wasPremature) {
            cout << " *** ThreadTest: Was premature." << endl;
        }
    }

    // Perform the test
    virtual void test()
    {
        //testPrematureDeath();
        testThreadSchedule();
    };

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestThreadClasses g_globalThreadedClasses;
