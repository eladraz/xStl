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
 * test_pmac.cpp
 *
 * Tests the P_MD5 and the P_SHA1 class.
 * The tests vectors are according to tests tasken from SSLeay0.9.6
 *
 *  P_MD5("secret", "level")  D3 1A FC AA
 *  P_SHA("secret", "level")  5D 55 43 2B
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/md5.h"
#include "xStl/enc/digest/sha1.h"
#include "xStl/enc/digest/hmac.h"
#include "xStl/enc/random/pmacRnd.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

static const cBuffer secret((uint8*)"secret", 6);
static const cBuffer seed((uint8*)"seed", 4);

class cTestPfunc : public cTestObject
{
    // Instance P_MD5 object
    cSmartPtr<cPMacRandom> instancePMD5(const cBuffer& secret, const cBuffer& seed)
    {
        // Create HMAC md5 object.
        cSmartPtr<cHMAC> md5(new cHMAC(cSmartPtr<cDigest>((cDigest*)new cMD5()), secret));

        return cSmartPtr<cPMacRandom>(new cPMacRandom(md5, seed));
    }

    // Instance P_SHA object
    cSmartPtr<cPMacRandom> instancePSHA1(const cBuffer& secret, const cBuffer& seed)
    {
        // Create HMAC sha1 object.
        cSmartPtr<cHMAC> sha(new cHMAC(cSmartPtr<cDigest>((cDigest*)new cSHA1()), secret));

        return cSmartPtr<cPMacRandom>(new cPMacRandom(sha, seed));
    }

    void test_pmd5()
    {
        cSmartPtr<cPMacRandom> rnd(instancePMD5(secret, seed));
        cBuffer output;
        rnd->generateRandom(13, output);

        uint8 result[13] = {0xD3, 0x1A, 0xFC, 0xAA, 0xD8, 0x54, 0xFB,
                            0x2A, 0x6F, 0xDB, 0x14, 0x42, 0xF7};

        for (unsigned int i = 0; i < 13; i++)
        {
            TESTS_ASSERT_EQUAL(output[i], result[i]);
        }
    }

    void test_psha1()
    {
        cSmartPtr<cPMacRandom> rnd(instancePSHA1(secret, seed));
        cBuffer output;
        rnd->generateRandom(13, output);

        uint8 result[13] = {0x5D, 0x55, 0x43, 0x2B, 0xDF, 0xE9, 0xB9,
                            0x3A, 0x81, 0x3A, 0xCC, 0x93, 0x2A};

        for (unsigned int i = 0; i < 13; i++)
        {
            TESTS_ASSERT_EQUAL(output[i], result[i]);
        }
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_pmd5();
        test_psha1();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestPfunc g_globalTestPfunc;
