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
 * test_alignment.cpp
 *
 * Tests the
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/alignment.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/stream/memoryStream.h"
#include "xStl/../../tests/tests.h"

class TestObjecttest_alignment : public cTestObject
{
public:
    void testAlign()
    {
        // Up
        TESTS_ASSERT_EQUAL(Alignment::alignDownToDword(0x12345600), 0x12345600);
        TESTS_ASSERT_EQUAL(Alignment::alignDownToDword(0x12345601), 0x12345600);
        TESTS_ASSERT_EQUAL(Alignment::alignDownToDword(0x12345602), 0x12345600);
        TESTS_ASSERT_EQUAL(Alignment::alignDownToDword(0x12345603), 0x12345600);
        TESTS_ASSERT_EQUAL(Alignment::alignDownToDword(0x12345604), 0x12345604);
        TESTS_ASSERT_EQUAL(Alignment::alignDownToDword(0x123456FF), 0x123456FC);
        // Down
        TESTS_ASSERT_EQUAL(Alignment::alignUpToDword(0x12345600), 0x12345600);
        TESTS_ASSERT_EQUAL(Alignment::alignUpToDword(0x12345601), 0x12345604);
        TESTS_ASSERT_EQUAL(Alignment::alignUpToDword(0x12345602), 0x12345604);
        TESTS_ASSERT_EQUAL(Alignment::alignUpToDword(0x12345603), 0x12345604);
        TESTS_ASSERT_EQUAL(Alignment::alignUpToDword(0x12345604), 0x12345604);
        TESTS_ASSERT_EQUAL(Alignment::alignUpToDword(0x123456FF), 0x12345700);
    }

    void testMemory()
    {
        uint8 number[4] = {0x12, 0x34, 0x56, 0x78};
        TESTS_ASSERT_EQUAL(Alignment::readAlignedWord((uint16*)(number + 0)), 0x3412);
        TESTS_ASSERT_EQUAL(Alignment::readAlignedWord((uint16*)(number + 1)), 0x5634);
        TESTS_ASSERT_EQUAL(Alignment::readAlignedWord((uint16*)(number + 2)), 0x7856);
    }

    virtual void test()
    {
        testAlign();
        testMemory();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
TestObjecttest_alignment g_globalTestAlignment;
