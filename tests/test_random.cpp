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
 * test_random.cpp
 *
 * Tests the random for thier goodnes.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/enc/random.h"
#include "xStl/enc/random/rc4rnd.h"
#include "xStl/enc/random/prf.h"
#include "tests.h"
#include <stdio.h>

class cTestRandom : public cTestObject
{
public:
    /*
     * Create random objects
     */
    cSmartPtr<cRandom> createRndObject(uint type, const cBuffer& seed)
    {
        if (type & 1)
        {
            // Create rc4 rnd.
            cRC4Random* object = new cRC4Random(seed);
            return cSmartPtr<cRandom>((cRandom*)object);
        } else
        {
            // Create PRF object
            cPRF* object = new cPRF(seed, seed);
            return cSmartPtr<cRandom>((cRandom*)object);
        }
    }

    void test_random()
    {
        for (unsigned int i = 0; i < 10; i++)
        {
            // Test seed to be the same.
            char sp[100];
            #ifdef XSTL_WINDOWS
                sprintf_s(sp, 100, "%Xseed%d", i,i);
            #else
                sprintf(sp, "%Xseed%d", i,i);
            #endif
            cBuffer seed((uint8*)sp, (uint)strlen(sp));
            cBuffer o1;
            cBuffer o2;
            cSmartPtr<cRandom> rnd = createRndObject(i, seed);
            cSmartPtr<cRandom> r1 = createRndObject(i, seed);
            rnd->generateRandom(100, o1);
            r1->generateRandom(100, o2);
            TESTS_ASSERT_EQUAL(o1.getSize(), 100);
            TESTS_ASSERT_EQUAL(o1.getSize(), o2.getSize());
            TESTS_ASSERT_EQUAL(memcmp(o1.getBuffer(), o2.getBuffer(), o1.getSize()), 0);

            cSmartPtr<cRandom> r2 = createRndObject(i, seed);
            r2->generateRandom(50, o2);
            TESTS_ASSERT_EQUAL(o2.getSize(), 50);
            TESTS_ASSERT_EQUAL(memcmp(o1.getBuffer(), o2.getBuffer(), o2.getSize()), 0);

            r2->generateRandom(50, o2);
            TESTS_ASSERT_EQUAL(o2.getSize(), 50);
            TESTS_ASSERT_EQUAL(memcmp(o1.getBuffer() + 50, o2.getBuffer(), o2.getSize()), 0);
        }
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_random();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestRandom g_globalTestRandom;


