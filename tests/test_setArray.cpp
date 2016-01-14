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
 * test_setArray.cpp
 *
 * Tests the
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/stream/memoryStream.h"
#include "xStl/data/setArray.h"
#include "xStl/../../tests/tests.h"

class TestObjecttest_setArray : public cTestObject
{
public:
    void testSimple()
    {
        // Randomize test length
        uint testSize = 5 + (cOSRand::rand() % 16384);

        cSetArray simple(testSize);
        // Check all elements are cleared
        uint i;
        for (i = 0; i < testSize; ++i)
        {
            TESTS_ASSERT(!simple.isSet(i));
        }

        // Start puting random values
        for (i = 0; i < 10; ++i)
        {
            uint location = cOSRand::rand() % testSize;
            simple.set(location);
            for (uint j = 0; j < testSize; ++j)
            {
                if (j == location)
                {
                    TESTS_ASSERT(simple.isSet(j));
                } else
                {
                    TESTS_ASSERT(!simple.isSet(j));
                }
            }
            simple.clear(location);
        }

        // Test out of scope exception
        TESTS_EXCEPTION(simple.isSet(testSize));
        TESTS_EXCEPTION(simple.isSet(testSize + 5));
        TESTS_EXCEPTION(simple.isSet(testSize + 90));
        TESTS_EXCEPTION(simple.clear(testSize));
        TESTS_EXCEPTION(simple.clear(testSize + 5));
        TESTS_EXCEPTION(simple.clear(testSize + 90));
        TESTS_EXCEPTION(simple.set(testSize));
        TESTS_EXCEPTION(simple.set(testSize + 5));
        TESTS_EXCEPTION(simple.set(testSize + 90));
    }

    void testComplex()
    {
        // Randomize test length
        uint testSize = 5 + (cOSRand::rand() % 2048);
        uint numberOfLocation = cOSRand::rand() % testSize;
        cSetArray   simple(testSize);
        cArray<uint> locations(numberOfLocation);

        uint i;
        // Set locations
        for (i = 0; i < numberOfLocation; i++)
        {
            locations[i] = cOSRand::rand() % testSize;
            simple.set(locations[i]);
        }

        // Test locations
        for (i = 0; i < testSize; i++)
        {
            bool shouldSet = false;
            for (uint j = 0; j < numberOfLocation; j++)
            {
                if (locations[j] == i)
                {
                    shouldSet = true;
                    break;
                }
            }

            TESTS_ASSERT_EQUAL(simple.isSet(i), shouldSet);
        }
    }

    virtual void test()
    {
        uint i;
        for (i = 0; i < 10; ++i) testSimple();
        for (i = 0; i < 30; ++i) testComplex();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
TestObjecttest_setArray g_globalTestSetArray;
