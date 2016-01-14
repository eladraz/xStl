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
 * test_rle
 *
 * Tests the RLE compression.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"
#include "xStl/stream/memoryStream.h"
#include "xStl/stream/rle.h"
#include "xStl/stream/ioStream.h"

class cTestRLE : public cTestObject
{
public:
    void test_compression()
    {
        for (unsigned int i = 0; i < 512; i++)
        {
            rleFlag = (uint8)cOSRand::rand();
            // Create compression.
            cSmartPtr<basicIO> memstream(new cMemoryStream());
            cRLEcompression rle(memstream, rleFlag);

            // Compress block of characters
            uint8 count = (uint8)((cOSRand::rand() % 250) + 3);
            uint8 och = (uint8)cOSRand::rand();

            // Increase the dangerous situation.
            if ((cOSRand::rand() % 5) == 3)
            {
                count = rleFlag;
                if (count < 3) count = 3;
            }
            if ((cOSRand::rand() % 5) == 3)
            {
                och = rleFlag;
            }

            for (unsigned int i = 0; i < count; i++)
            {
                rle.pipeWrite(&och, 1);
            }
            rle.flush();

            // 3 characters only.
            memstream->seek(0, basicInput::IO_SEEK_SET);

            // Read the compression flag.
            if (count != rleFlag)
            {
                TESTS_ASSERT_EQUAL(memstream->length(), 3);
                uint8 ch;
                TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                TESTS_ASSERT_EQUAL(ch, rleFlag);

                // Read the length
                TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                TESTS_ASSERT_EQUAL(ch, count);

                // Read the byte
                TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                TESTS_ASSERT_EQUAL(ch, och);
            } else {
                if (och == rleFlag)
                {
                    TESTS_ASSERT_EQUAL(memstream->length(), 5);
                    uint8 ch;
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, rleFlag);

                    // Read the length
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, count - 1);

                    // Read the byte
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, rleFlag);

                    // The rest byte.
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, rleFlag);
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, rleFlag);
                } else
                {
                    if (memstream->length() != 4)
                    {
                        uint8 gh = 9;
                        gh*=2;
                    }
                    TESTS_ASSERT_EQUAL(memstream->length(), 4);
                    uint8 ch;
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, rleFlag);

                    // Read the length
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, count - 1);

                    // Read the byte
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, och);

                    // The rest character
                    TESTS_ASSERT_EQUAL(memstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(ch, och);
                }
            }

            // Try to read and compare...
            memstream->seek(0, basicInput::IO_SEEK_SET);
            cRLEcompression orle(memstream, rleFlag);
            cBuffer temp;
            TESTS_ASSERT_EQUAL(orle.pipeRead(temp, 3000), count);
            TESTS_ASSERT(orle.isEOS());
            for (unsigned int j = 0; j < count; j++)
            {
                TESTS_ASSERT_EQUAL(temp[j], och);
            }
        }
    };

    virtual void test_compression_block()
    {
        for (unsigned int i = 0; i < 128; i++)
        {
            rleFlag = (uint8)cOSRand::rand();
            // Create compression.
            cSmartPtr<basicIO> memstream(new cMemoryStream());
            cRLEcompression rle(memstream, rleFlag);

            // Compress block of characters
            unsigned int times = cOSRand::rand() % 20;
            cBuffer count(times);
            cBuffer och(times);

            unsigned int j, totalCount = 0;
            for (j = 0; j < times; j++)
            {
                count[j] = (uint8)((cOSRand::rand() % 250) + 3);
                och[j] = (uint8)cOSRand::rand();
                // Increase the dangerous situation.
                if ((cOSRand::rand() % 5) == 3)
                {
                    count[j] = rleFlag;
                    if (count[j] < 3) count[j] = 3;
                }
                if ((cOSRand::rand() % 5) == 3)
                {
                    och[j] = rleFlag;
                }

                for (unsigned int temp = 0; temp < count[j]; temp++)
                {
                    uint8 t = och[j];
                    rle.pipeWrite(&t, 1);
                }
                totalCount+= count[j];
            }
            rle.flush();

            // Read and parse...
            memstream->seek(0, basicInput::IO_SEEK_SET);
            cRLEcompression orle(memstream, rleFlag);
            cBuffer temp;
            TESTS_ASSERT_EQUAL(orle.pipeRead(temp, totalCount), totalCount);
            TESTS_ASSERT(orle.isEOS());
            totalCount = 0;
            unsigned int p = 0;
            for (j = 0; j < temp.getSize(); j++)
            {
                TESTS_ASSERT_EQUAL(temp[j], och[totalCount]);
                p++;
                if (p == count[totalCount])
                {
                    p = 0;
                    totalCount++;
                }
            }
        }
    }

    // Perform the test
    virtual void test()
    {
        // Init random generator.
        for (uint i = 0; i < 100; i++) cOSRand::rand();
        // Start tests
        test_compression();
        test_compression_block();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }

private:
    uint8 rleFlag;
};

cTestRLE g_globalTestRLE;
