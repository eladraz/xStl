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
 * test_prf.cpp
 *
 * Test the PRF Psedu-random-function to be compatible with the SSLeay.0.9.6
 * implementation as describes in the RFC2246 documentation
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/enc/random.h"
#include "xStl/enc/random/prf.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

static const cBuffer secret((uint8*)"secret", 6);
static const cBuffer labelseed((uint8*)"labelseed", 9);

class cTestPRF : public cTestObject
{
    cSmartPtr<cPRF> createPRF(const cBuffer& seed, const cBuffer& secret)
    {
        // Create PRF object
        return cSmartPtr<cPRF>(new cPRF(seed, secret));
    }

    void test_prf()
    {
        cSmartPtr<cPRF> prf(createPRF(labelseed, secret));
        cBuffer output;
        prf->generateRandom(10, output);

        uint8 result[10] = {0xB5, 0xBA, 0xF4, 0x72, 0x2B, 0x91, 0x85, 0x1A, 0x88, 0x16};
        for (unsigned int i = 0; i < 10; i++)
        {
            TESTS_ASSERT_EQUAL(output[i], result[i]);
        }
    };


    // Perform the test
    virtual void test()
    {
        // Start tests
        test_prf();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestPRF g_globalTestPRF;
