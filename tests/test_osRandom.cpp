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
 * test_osRandom.cpp
 *
 * Checks that the random using the OS random is really a random.
 * The test are very simple.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/data/datastream.h"
#include "xStl/stream/ioStream.h"
#include "tests.h"
#include <stdio.h>

class cOSRandom : public cTestObject
{
    // Print the random over the screen
    void test_screenDump()
    {
        cBuffer rnd;
        cOSRand::generateRandom(512, rnd);
        // Print the random
        DATA data(rnd.begin(), rnd.end());
        cout << data << endl;
    }

    #define RNDVALUE (20)
    void test_random()
    {
        uint histogram[256];
        // Zero memory
        memset(histogram, 0, sizeof(histogram));
        uint i;
        for (i = 0; i < RNDVALUE * 256; i++)
        {
            histogram[cOSRand::rand() & 0xFF]++;
        }

        uint lowPos = 0, highPos = 0;
        uint low = histogram[lowPos];
        uint high = histogram[highPos];
        uint average = low;

        for (i = 1; i < 256; i++)
        {
            average+= histogram[i];

            if (low > histogram[i])
            {
                low = histogram[i];
            }

            if (high < histogram[i])
            {
                high = histogram[i];
            }
        }
        double av = average / 256.0;
        char str[15];
        #ifdef XSTL_WINDOWS
        sprintf_s(str, 15, "%2.10f", av);
        #else
        sprintf(str, "%2.10f", av);
        #endif

        cout << "Average value: " << str << " (" << RNDVALUE << ")" << endl;
        cout << "Low value:     " << (uint32)low << endl;
        cout << "High value:    " << (uint32)high << endl;

        // Let the user understand....
        cOS::sleepSeconds(3);
    }

    // Perform the test
    virtual void test()
    {
        // Start tests
        test_screenDump();
        test_random();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance a test module
cOSRandom gGlobalTestOSRandom;
