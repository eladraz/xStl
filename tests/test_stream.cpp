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
 * test_stream.cpp
 *
 * Test the all streams cut out from "basicIO":
 *  - cFileStream: generate temporary file and perfrom test. delete the file at the end
 *  - cMemoryStream: Create memory file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/data/datastream.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"
#include "xStl/stream/memoryStream.h"
#include "xStl/stream/fileStream.h"
#include "xStl/stream/ioStream.h"

#ifdef XSTL_NTDDK
    #define tempfilename "\\??\\C:\\Temp\\TEMP.TMP"
#else
    // All other platforms
    #define tempfilename "TEMP.TMP"
#endif

#define MAX_BUFFER (1000)

class cTestStream : public cTestObject
{
public:
    /*
     * Create cMemoryStream or cFileStream object
     */
    cSmartPtr<basicIO> createStream(int type)
    {
        if (type & 1)
        {
            /* file stream */
            cSmartPtr<basicIO> newFile(new cFileStream());
            cFileStream* ptr = (cFileStream*)newFile.getPointer();
            ptr->open(tempfilename, cFile::CREATE | cFile::WRITE);
            return newFile;
        } else
        {
            /* memoryStream stream */
            return cSmartPtr<basicIO>(new cMemoryStream());
        }
    }

    cBuffer m_oldData;

    cSmartPtr<basicIO> createReadStream(int type)
    {
        if (type & 1)
        {
            /* file stream */
            cSmartPtr<basicIO> newFile(new cFileStream());
            cFileStream* ptr = (cFileStream*)newFile.getPointer();
            ptr->open(tempfilename, cFile::READ);
            return newFile;
        } else
        {
            /* memoryStream stream */
            return cSmartPtr<basicIO>(new cMemoryStream(m_oldData));
        }
    }

    void closeStream(int type, cSmartPtr<basicIO> stream)
    {
        if (!(type & 1))
        {
            m_oldData = *(((cMemoryStream*)(stream.getPointer()))->getStream());
        }
    }

    void test_read_write()
    {
        for (unsigned int i = 0; i < 50; i++)
        {
            uint8 buffer[MAX_BUFFER];
            uint8 read[MAX_BUFFER];
            uint32 size = (cOSRand::rand() % (MAX_BUFFER - 1)) + 1;
            for (uint32 i = 0; i < size; i++)
            {
                buffer[i] = (uint8)cOSRand::rand();
                read[i] = 0;
            }

            int type = cOSRand::rand();

            // Write out to the stream
            {
                cSmartPtr<basicIO> stream = createStream(type);
                stream->pipeWrite(buffer, size);
                closeStream(type, stream);
            }

            // Read from the stream
            {
                cSmartPtr<basicIO> rstream = createReadStream(type);
                rstream->pipeRead(read, size);

                // Test the reading
                for (uint32 j = 0; j < size; j++)
                {
                    TESTS_ASSERT_EQUAL(read[j], buffer[j]);
                }

                TESTS_ASSERT(rstream->isEOS());
            }

            cout << ',';
            cout.flush();
        }
    }

    void test_seek()
    {
        for (unsigned int i = 0; i < 50; i++)
        {
            uint8 buffer[MAX_BUFFER];
            uint32 size = (cOSRand::rand() % (MAX_BUFFER - 2)) + 2;
            for (uint32 i = 0; i < size; i++)
            {
                buffer[i] = (uint8)cOSRand::rand();
            }
            int type = cOSRand::rand();
            // Create the stream
            {
                cSmartPtr<basicIO> stream = createStream(type);
                stream->pipeWrite(buffer, size);
                closeStream(type, stream);
            }

            // Read from the stream with seeking...
            {
                cSmartPtr<basicIO> rstream = createReadStream(type);

                // From begin
                unsigned int idx;
                for (idx = 0; idx < 10; idx++)
                {
                    int positionFromBegin = cOSRand::rand() % size;
                    rstream->seek(positionFromBegin, basicInput::IO_SEEK_SET);
                    uint8 ch;
                    TESTS_ASSERT_EQUAL(rstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(buffer[positionFromBegin], ch);
                }

                // From end
                for (idx = 0; idx < 10; idx++)
                {
                    int positionFromEnd = (cOSRand::rand() % (size - 1)) + 1;
                    positionFromEnd = -positionFromEnd;
                    rstream->seek(positionFromEnd, basicInput::IO_SEEK_END);
                    uint8 ch;
                    TESTS_ASSERT_EQUAL(rstream->pipeRead(&ch, 1), 1);
                    TESTS_ASSERT_EQUAL(buffer[size + positionFromEnd], ch);
                }
            }

            cout << '-';
            cout.flush();
        }
    }

    void test_remote_address()
    {
        // Create memory-stream
        BasicIOPtr stream = createStream(0);
        // Start by normally encode a series of default memory stream
        #define NUMBER_OF_ADDRESSES (100)
        cSArray<remoteAddressNumericValue> m_address(NUMBER_OF_ADDRESSES);
        // Serialize
        for (uint i = 0; i < 100; i++)
        {
            // Randomize
            m_address[i] = remoteAddressNumericValue((addressNumericValue)(cOSRand::rand()));
            // And encode
            stream->streamWriteRemoteAddress(m_address[i]);
        }
        stream->seek(0, basicIO::IO_SEEK_SET);
        // Deserialize
        for (uint i = 0; i < 100; i++)
        {
            remoteAddressNumericValue temp = stream->streamReadRemoteAddress();
            TESTS_ASSERT_EQUAL(temp, m_address[i]);
        }

        // Now test for clipping!
        stream->seek(0, basicIO::IO_SEEK_SET);
        stream->changeEncodedAddressType(false, REMOTE_ADDRESS_64BIT);
        stream->changeDecodedAddressType(false, REMOTE_ADDRESS_64BIT);
        // Serialize
        for (uint i = 0; i < 100; i++)
            stream->streamWriteRemoteAddress(m_address[i]);

        stream->seek(0, basicIO::IO_SEEK_SET);
        // Deserialize
        for (uint i = 0; i < 100; i++)
        {
            remoteAddressNumericValue temp = stream->streamReadRemoteAddress();
            TESTS_ASSERT_EQUAL(temp, m_address[i]);
        }

    }

    // Perform the test
    virtual void test()
    {
        // Init random generator.
        for (uint i = 0; i < 100; i++) cOSRand::rand();

        // Start tests
        test_remote_address();
        test_read_write();
        test_seek();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestStream g_globalTestStream;
