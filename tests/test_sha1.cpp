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
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/sha1.h"
#include "tests.h"

/*
 * SHA1 test vectors according to RFC-3174
 *
 */

#define TEST1   "abc"
#define TEST2a  "abcdbcdecdefdefgefghfghighijhi"
#define TEST2b  "jkijkljklmklmnlmnomnopnopq"
#define TEST2   TEST2a TEST2b

// TODO!
/*
#define TEST3   "a"
#define TEST4a  "01234567012345670123456701234567"
#define TEST4b  "01234567012345670123456701234567"
#define TEST4   TEST4a TEST4b
*/
#define NUM_TEST (2)

char *testarray[NUM_TEST] =
{
    TEST1,
    TEST2

    // TODO!
    //TEST4,
    //TEST3
};
uint8 resultarray[NUM_TEST][20] =
{
    {0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
     0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D},
    {0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
     0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1}

    // TODO!
    /*
    {0xDE, 0xA3, 0x56, 0xA2, 0xCD, 0xDD, 0x90, 0xC7, 0xA7, 0xEC,
     0xED, 0xC5, 0xEB, 0xB5, 0x63, 0x93, 0x4F, 0x46, 0x04, 0x52},
    {0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
     0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F}
     */
};


class cTestSHA1 : public cTestObject
{
public:
    void test_sha1_length()
    {
        cSHA1 test;
        TESTS_ASSERT_EQUAL(test.getDigestLength(), 20);
    }

    void test_sha1()
    {
        for (unsigned int test = 0; test < NUM_TEST; test++)
        {
            cSHA1 sha1;
            sha1.update(testarray[test], (uint32)strlen(testarray[test]));
            cBuffer result = sha1.digest();
            TESTS_ASSERT_EQUAL(result.getSize(), 20);
            for (int i = 0; i < 20; i++)
            {
                TESTS_ASSERT_EQUAL(result[i], resultarray[test][i]);
            }
        }
    }

    void test_sha1_concat()
    {
        cSHA1 sha1;
        sha1.update(TEST2a, (uint32)strlen(TEST2a));
        sha1.update(TEST2b, (uint32)strlen(TEST2b));
        cBuffer result = sha1.digest();
        TESTS_ASSERT_EQUAL(result.getSize(), 20);
        for (int i = 0; i < 20; i++)
        {
            TESTS_ASSERT_EQUAL(result[i], resultarray[1][i]);
        }
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_sha1_length();
        test_sha1();
        test_sha1_concat();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestSHA1 g_globalTestSHA1;
