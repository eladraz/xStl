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
 * test_hmac_sha1.cpp
 *
 * Tests the HMAC_SHA1 class.
 *
 *  key =         "Jefe"
 *  data =        "what do ya want for nothing?"
 *  data_len =    28 bytes
 *  digest =      0x750c783e6ab0b503eaa86e310a5db738
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/sha1.h"
#include "xStl/enc/digest/hmac.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

static const uint  _key1len = 4;
static const char  _key1[] = "Jefe";
static const cBuffer key1((uint8*)_key1, _key1len);

static const char _data1[] = "what do ya want for nothing?";
static const cBuffer data1((uint8*)_data1, 28);

class cTestHmacSHA1 : public cTestObject
{
public:
    /*
     * Instance the HMAC-SHA1
     */
    cDigestPtr createHMACsha1(const cBuffer& key)
    {
        cDigestPtr obj((cDigest*)new cSHA1());
        return cDigestPtr((cDigest*)new cHMAC(obj, key));
    }


    void test_hsha1_length()
    {
        cDigestPtr obj = createHMACsha1(key1);
        TESTS_ASSERT_EQUAL(obj->getDigestLength(), 20);
    }

    void test_hsha1_1()
    {
        cDigestPtr obj = createHMACsha1(key1);
        obj->updateStream(data1);
        cBuffer ret = obj->digest();

        // Compare result

        uint8 result[20] = { 0xEF, 0xFC, 0xDF, 0x6A, 0xE5, 0xEB, 0x2F, 0xA2, 0xD2, 0x74,
                             0x16, 0xD5, 0xF1, 0x84, 0xDF, 0x9C, 0x25, 0x9A, 0x7C, 0x79 };
        TESTS_ASSERT_EQUAL(ret.getSize(), 20);

        for (uint i = 0; i < 20; i++)
        {
            TESTS_ASSERT_EQUAL(result[i], ret[i]);
        }
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_hsha1_length();
        test_hsha1_1();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestHmacSHA1 g_globalTestHMACSha1;
