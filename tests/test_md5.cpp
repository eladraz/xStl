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
 * test_md5.cpp
 *
 * Test the MD5 digest according to pre-known values.
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
#include "xStl/enc/digest/md5.h"
#include "tests.h"

/*
 * According to RFC 1321 - MD5 test vectors
 *
 * MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
 * MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661
 * MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
 * MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
 * MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
 * MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
 *       d174ab98d277d9f5a5611c2c9f419d9f
 * MD5 ("123456789012345678901234567890123456789012345678901234567890123456
 *       78901234567890") = 57edf4a22be3c955ac49da2e2107b67a
 *
 */
class cTestMD5 : public cTestObject
{
public:
    void test_md5_length()
    {
        cMD5 test;
        TESTS_ASSERT_EQUAL(test.getDigestLength(), 16);
    }

    void test_md5_1()
    {
        cMD5 test;
        test.update("", 0);
        uint8 except[] = {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                          0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};
        cBuffer result = test.digest();
        TESTS_ASSERT_EQUAL(result.getSize(), sizeof(except));
        for (unsigned int i = 0; i < sizeof(except); i++)
        {
            TESTS_ASSERT_EQUAL(result[i], except[i]);
        }
    }

    void test_md5_2()
    {
        cMD5 test;
        test.update("a", 1);
        uint8 except[] = {0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
                          0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61};
        cBuffer result = test.digest();
        TESTS_ASSERT_EQUAL(result.getSize(), sizeof(except));
        for (unsigned int i = 0; i < sizeof(except); i++)
        {
            TESTS_ASSERT_EQUAL(result[i], except[i]);
        }
    }

    void test_md5_3()
    {
        cMD5 test;
        char message[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "abcdefghijklmnopqrstuvwxyz"
                         "0123456789";
        test.update(message, (uint32)strlen(message));
        uint8 except[] = {0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5,
                          0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f};
        cBuffer result = test.digest();
        TESTS_ASSERT_EQUAL(result.getSize(), sizeof(except));
        for (unsigned int i = 0; i < sizeof(except); i++)
        {
            TESTS_ASSERT_EQUAL(result[i], except[i]);
        }
    }

    void test_md5_4()
    {
        cMD5 test;
        char message1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        char message2[] = "abcdefghijklmnopqrstuvwxyz";
        char message3[] = "0123456789";
        test.update(message1, (uint32)strlen(message1));
        test.update(message2, (uint32)strlen(message2));
        test.update(message3, (uint32)strlen(message3));
        uint8 except[] = {0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5,
                          0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f};
        cBuffer result = test.digest();
        TESTS_ASSERT_EQUAL(result.getSize(), sizeof(except));
        for (unsigned int i = 0; i < sizeof(except); i++)
        {
            TESTS_ASSERT_EQUAL(result[i], except[i]);
        }

        test.reset();
        char message[] = "message digest";
        test.update(message, (uint32)strlen(message));
        uint8 except1[] = {0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
                           0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0};
        cBuffer res = test.digest();
        TESTS_ASSERT_EQUAL(res.getSize(), sizeof(except1));
        for (unsigned int j = 0; j < sizeof(except1); j++)
        {
            TESTS_ASSERT_EQUAL(res[j], except1[j]);
        }
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_md5_length();
        test_md5_1();
        test_md5_2();
        test_md5_3();
        test_md5_4();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestMD5 g_globalTestMD5;
