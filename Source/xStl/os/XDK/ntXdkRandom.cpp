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
 * DDK implementation
 *
 * random.cpp
 *
 * Implementation file for the windows NT DDK.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/data/sarray.h"
#include "xStl/enc/random.h"
#include "xStl/os/mutex.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"

/*
 * The defualt random class for global system random. This class is using
 * hardware time-stamp counter which count the clocks of the cpu and compute
 * from it a random.
 */
class cDefaultWindowsTimestampRandom : public cRandom
{
public:
    /*
     * Default constructor. Read the
     */
    cDefaultWindowsTimestampRandom() :
        m_loading(readCounter1())
    {
    }

    /*
     * Generate 'length' bytes of random and put it in 'output'. The 'output'
     * size is changed to 'length' and step over pure random. This function
     * increase the internal random mechanizm.
     *
     * length - The amount of random to be generate.
     * output - Will be filled with the required psedu-random generation code.
     */
    virtual void generateRandom(uint length, cBuffer& output)
    {
        // Change the size of the output buffer.
        output.changeSize(length, false);

        // Start filling the buffer
        uint8* buffer = output.getBuffer();

        uint i = 0;
        while (i < length)
        {
            uint type;
            fillUpRandom(&type, sizeof(type), m_loading ^ readCounter());
            uint count;
            fillUpRandom(&count, sizeof(count), type);
            count = (count % (length - i)) + 1;

            // Simple filling
            fillUpRandom(buffer, count, m_loading ^ readCounter());

            // Fill out count byte according to the type.
            cBuffer xor(count);
            uint8* xorData = xor.getBuffer();
            if (type & 1)
            {
                fillUpRandom(xorData, count, readCounter() ^ readCounter1());
            } else
            {
                fillUpRandom(xorData, count, m_loading);
            }

            // Xor out values
            for (uint j = 0; j < count; j++)
            {
                buffer[j]^= xorData[j];
            }

            // Increase counting
            i+= count;
            buffer+= count;

            uint newLoading;
            fillUpRandom(&newLoading, sizeof(newLoading), m_loading ^ readCounter());
            newLoading^= readCounter();
            newLoading^= readCounter1();
            m_loading = newLoading;
        }
    }

private:
    /*
     * Uses the pentium time-stamp instruction 0F 31 in order to generate 64bit
     * counter. The function returns the lower 32bit of the counter.
     */
    uint32 readCounter()
    {
        register uint32 retd;
        _asm {
            rdtsc
            mov retd, eax
        };
        return retd;
    }

    /*
     * Returns the number of tick-count since the windows-operation system was first
     * started. The tick-count in windows is the number of internal context-switch
     * ticks. For all windows version this constant stand over 1000Hz.
     */
    uint32 readCounter1()
    {
        LARGE_INTEGER tickCount;
        KeQueryTickCount(&tickCount);
        return (uint32)tickCount.LowPart;
    }

    /*
     * Using a simple 32bit lcg in order to fill up the buffer with random
     */
    void fillUpRandom(void* buffer, uint length, uint32 seed)
    {
        uint32 lcg = seed;
        uint8* buf = (uint8*)(buffer);
        for (uint i = 0; i < length; i++)
        {
            // Increase lcg
            lcg = lcg*214013L + 2531011L;
            // Evalute from the lower high nibble.
            buf[i] = (uint8)(((lcg >> 16) % 512) & 0xFF);
        }
    }

    // The older loading for the mechanizm
    uint32 m_loading;
};

cSmartPtr<cRandom> cOSRand::getRandom()
{
    if (g_globalRandom.isEmpty())
    {
        // Construct the first instance
        g_globalRandom = cSmartPtr<cRandom>(new cDefaultWindowsTimestampRandom());
    }
    return g_globalRandom;
}

// Init the random mechanizm to empty
cSmartPtr<cRandom> cOSRand::g_globalRandom(NULL);
