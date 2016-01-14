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
 * test_compression.cpp
 *
 * Test the compression filter streams:
 *   LZW
 *   RLE
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
#include "xStl/stream/lzw.h"
#include "xStl/stream/rle.h"
#include "xStl/stream/ioStream.h"


class cTestCompression : public cTestObject
{
public:

    /*
     * Create RLE or LZW stream.
     */
    cSmartPtr<filterStream> createCompressionStream(cSmartPtr<basicIO> memstream, int type)
    {
        if (type & 1)
        {
            /* RLE stream */
            return cSmartPtr<filterStream>(new cRLEcompression(memstream));
        } else
        {
            /* LZW stream */
            return cSmartPtr<filterStream>(new cLZWcompression(memstream));
        }
    }

    cString getCompressionType(int type)
    {
        if (type & 1)
        {
            return "RLE";
        } else
        {
            return "LZW";
        }
    }

    void test_integraty()
    {
        for (unsigned int i = 0; i < 50; i++)
        {
            #define MAX_BUFFER (1000)
            uint8 buffer[MAX_BUFFER];
            uint8 read[MAX_BUFFER];
            uint32 size = (cOSRand::rand() % (MAX_BUFFER - 1)) + 1;
            for (uint32 i = 0; i < size; i++)
            {
                buffer[i] = (uint8)cOSRand::rand();
                read[i] = 0;
            }

            int type = cOSRand::rand();

            cSmartPtr<basicIO> memstream(new cMemoryStream());
            // Compress block
            {
                cSmartPtr<filterStream> compression = createCompressionStream(memstream, type);
                compression->pipeWrite(buffer, size);
                compression->flush();
            }

            // Go to the beginning of the stream...
            memstream->seek(0, basicInput::IO_SEEK_SET);

            // Calculate compression radio...
            #if 0
            char double_buffer[50];
            double random_radio;
            uint32 cmp_size = ((cMemoryStream*)memstream.getPointer())->getStream()->getSize();
            random_radio = ((double)cmp_size * 100) / size;
            _gcvt_s(double_buffer, 50, random_radio, 3);
            TRACE(TRACE_MEDIUM, cString("Random compression radio: ") +  double_buffer + "%  [" + getCompressionType(type) + "]");
            #endif

            cout << getCompressionType(type);

            // Decompress block
            {
                cSmartPtr<filterStream> decompress = createCompressionStream(memstream, type);
                decompress->pipeRead(read, size);
                // Test decompression
                for (uint32 j = 0; j < size; j++)
                {
                    TESTS_ASSERT_EQUAL(read[j], buffer[j]);
                }

                TESTS_ASSERT(decompress->isEOS());
            }
        }
    }

    // Perform the test
    virtual void test()
    {
        // Init random generator.
        for (uint i = 0; i < 100; i++) cOSRand::rand();

        // Start tests
        test_integraty();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestCompression g_globalTestCompression;
