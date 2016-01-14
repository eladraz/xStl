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
 * test_counter.cpp
 *
 * Test the 'counter' activity.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/data/counter.h"
#include "xStl/except/trace.h"
#include "tests.h"


class cTestCounter : public cTestObject
{
public:
    // Perform the test
    virtual void test()
    {
        // Init random
        uint i;
        for (i = 0; i < 100; i++) cOSRand::rand();

        for (i = 0; i < 10; i++)
        {
            int seed = cOSRand::rand();
            cCounter counter(seed);
            TESTS_ASSERT_EQUAL(counter.m_value, seed);
        }

        for (i = 0; i < 10; i++)
        {
            unsigned int seed = cOSRand::rand() % 100;
            cCounter counter(0);

            for (unsigned int j = 0; j < seed; j++)
            {
                ++counter;
                TESTS_ASSERT_EQUAL(counter.m_value, (int)(j + 1));
            }

            for (unsigned int b = seed; b > 0; b--)
            {
                --counter;
                TESTS_ASSERT_EQUAL(counter.m_value, (int)(b - 1));
            }
        }
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestCounter g_globalTestCounter;
