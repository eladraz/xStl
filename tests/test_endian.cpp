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
 * test_sha1.cpp
 *
 * Test the SHA1 digest according to pre-known values.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/endian.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"


class cTestEndian : public cTestObject
{
public:
    void test_little_endian_32()
    {
        uint8 buffer[4] = {0x12, 0x34, 0x56, 0x78};
        uint32 data = cLittleEndian::readUint32(buffer);
        TESTS_ASSERT_EQUAL(data, 0x78563412);

        cLittleEndian::writeUint32(buffer, 0xDEADFACE);
        TESTS_ASSERT_EQUAL(buffer[3], 0xDE);
        TESTS_ASSERT_EQUAL(buffer[2], 0xAD);
        TESTS_ASSERT_EQUAL(buffer[1], 0xFA);
        TESTS_ASSERT_EQUAL(buffer[0], 0xCE);
    }

    void test_little_endian_16()
    {
        uint8 buffer[2] = {0x12, 0x34};
        uint32 data = cLittleEndian::readUint16(buffer);
        TESTS_ASSERT_EQUAL(data, 0x3412);

        cLittleEndian::writeUint16(buffer, 0xDEAD);
        TESTS_ASSERT_EQUAL(buffer[1], 0xDE);
        TESTS_ASSERT_EQUAL(buffer[0], 0xAD);
    }

    void test_big_endian_32()
    {
        uint8 buffer[4] = {0x12, 0x34, 0x56, 0x78};
        uint32 data = cBigEndian::readUint32(buffer);
        TESTS_ASSERT_EQUAL(data, 0x12345678);

        cBigEndian::writeUint32(buffer, 0xDEADFACE);
        TESTS_ASSERT_EQUAL(buffer[0], 0xDE);
        TESTS_ASSERT_EQUAL(buffer[1], 0xAD);
        TESTS_ASSERT_EQUAL(buffer[2], 0xFA);
        TESTS_ASSERT_EQUAL(buffer[3], 0xCE);
    }

    void test_big_endian_16()
    {
        uint8 buffer[2] = {0x12, 0x34};
        uint32 data = cBigEndian::readUint16(buffer);
        TESTS_ASSERT_EQUAL(data, 0x1234);

        cBigEndian::writeUint16(buffer, 0xDEAD);
        TESTS_ASSERT_EQUAL(buffer[0], 0xDE);
        TESTS_ASSERT_EQUAL(buffer[1], 0xAD);
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_little_endian_32();
        test_little_endian_16();
        test_big_endian_32();
        test_big_endian_16();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestEndian g_globalTestEndian;
