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
 * test_hmac_md5.cpp
 *
 * Tests the HMAC_MD5 class.
 * Test Vectors (Trailing '\0' of a character string not included in test):
 * [Taken from RFC2104]
 *
 *  key =         0x0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b
 *  key_len =     16 bytes
 *  data =        "Hi There"
 *  data_len =    8  bytes
 *  digest =      0x9294727a3638bb1c13f48ef8158bfc9d
 *
 *  key =         "Jefe"
 *  data =        "what do ya want for nothing?"
 *  data_len =    28 bytes
 *  digest =      0x750c783e6ab0b503eaa86e310a5db738
 *
 *  key =         0xAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 *  key_len       16 bytes
 *  data =        0xDDDDDDDDDDDDDDDDDDDD...
 *                ..DDDDDDDDDDDDDDDDDDDD...
 *                ..DDDDDDDDDDDDDDDDDDDD...
 *                ..DDDDDDDDDDDDDDDDDDDD...
 *                ..DDDDDDDDDDDDDDDDDDDD
 *  data_len =    50 bytes
 *  digest =      0x56be34521d144c88dbb8c733f0e8b3f6
 *
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/md5.h"
#include "xStl/enc/digest/hmac.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

// 1
static const uint  _key1len = 16;
static const uint8 _key1[_key1len] = { 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                                       0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
static const cBuffer key1(_key1, _key1len);
static const char _data1[] = "Hi There";
static const cBuffer data1((uint8*)_data1, 8);

// 2
static const uint  _key2len = 4;
static const char  _key2[] = "Jefe";
static const cBuffer key2((uint8*)_key2, _key2len);
static const char _data2[] = "what do ya want for nothing?";
static const cBuffer data2((uint8*)_data2, 28);


class cTestHmacMD5 : public cTestObject
{
public:
    /*
     * Instance the HMAC-MD5
     */
    cDigestPtr createHMACmd5(const cBuffer& key)
    {
        cDigestPtr obj((cDigest*)new cMD5());
        return cDigestPtr((cDigest*)new cHMAC(obj, key));
    }


    void test_md5_length()
    {
        cDigestPtr obj = createHMACmd5(key1);
        TESTS_ASSERT_EQUAL(obj->getDigestLength(), 16);
    }

    void test_md5_1()
    {
        cDigestPtr obj = createHMACmd5(key1);
        obj->updateStream(data1);
        cBuffer ret = obj->digest();

        // Compare result
        uint8 result[] = { 0x92, 0x94, 0x72, 0x7a, 0x36, 0x38, 0xbb, 0x1c,
                           0x13, 0xf4, 0x8e, 0xf8, 0x15, 0x8b, 0xfc, 0x9d};
        TESTS_ASSERT_EQUAL(ret.getSize(), 16);

        for (uint i = 0; i < 16; i++)
        {
            TESTS_ASSERT_EQUAL(result[i], ret[i]);
        }
    }

    void test_md5_2()
    {
        cDigestPtr obj = createHMACmd5(key2);
        obj->updateStream(data2);
        cBuffer ret = obj->digest();

        // Compare result
        uint8 result[] = { 0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03,
                           0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38};
        TESTS_ASSERT_EQUAL(ret.getSize(), 16);

        for (uint i = 0; i < 16; i++)
        {
            TESTS_ASSERT_EQUAL(result[i], ret[i]);
        }
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_md5_length();
        test_md5_1();
        test_md5_2();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestHmacMD5 g_globalTestHMACMD5;
