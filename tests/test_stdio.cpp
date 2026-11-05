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
 * test_stdio.cpp
 *
 * Tests the standard streams wrappers (cFile::getStandardStream) and the
 * cFile::writeAll API:
 *  - writeAll writes a binary buffer (including 0 and 0xff bytes) to a
 *    temporary file, which is read back and compared byte by byte.
 *  - The standard streams objects have the expected access modes. The test
 *    does not write to the process standard streams.
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/file.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

#define STDIO_TEMP_FILE "test_stdio.tmp"
#define STDIO_BUFFER_SIZE (3000)

class cTestStdio : public cTestObject {
public:
    // Write a binary pattern with writeAll, then read it back
    void test_writeAll()
    {
        uint8 pattern[STDIO_BUFFER_SIZE];
        uint8 read[STDIO_BUFFER_SIZE];
        for (uint i = 0; i < STDIO_BUFFER_SIZE; i++)
        {
            // Every 256 bytes covers 0x00..0xff, with extra NUL and 0xff runs
            if (i >= 1000 && i < 1100)
                pattern[i] = 0;
            else if (i >= 2000 && i < 2100)
                pattern[i] = 0xff;
            else
                pattern[i] = (uint8)(i ^ (i >> 8));
            read[i] = 0;
        }

        // Write the buffer in two parts, the second part is empty for a
        // zero-length call which must be a no-op
        {
            cFile file(cString(STDIO_TEMP_FILE),
                       cFile::CREATE | cFile::WRITE);
            file.writeAll(pattern, 1500);
            file.writeAll(pattern, 0);
            file.writeAll(pattern + 1500, STDIO_BUFFER_SIZE - 1500);
            file.flush();
        }

        // Read the file back and compare
        {
            cFile file(cString(STDIO_TEMP_FILE), cFile::READ);
            TESTS_ASSERT_EQUAL(file.length(), STDIO_BUFFER_SIZE);
            uint total = 0;
            while (total < STDIO_BUFFER_SIZE)
            {
                uint got = file.read(read + total, STDIO_BUFFER_SIZE - total);
                if (got == 0)
                    break;
                total += got;
            }
            TESTS_ASSERT_EQUAL(total, STDIO_BUFFER_SIZE);
            for (uint i = 0; i < STDIO_BUFFER_SIZE; i++)
            {
                TESTS_ASSERT_EQUAL(read[i], pattern[i]);
            }
        }

        cFile::deleteFile(cString(STDIO_TEMP_FILE));
    }

    // Verify the standard streams objects without touching the process streams
    void test_standardStreams()
    {
        cFile& err = cFile::getStandardStream(STANDARD_ERROR);
        TESTS_ASSERT(err.isOpenForWrite());
        TESTS_ASSERT(!err.isOpenForRead());
        TESTS_ASSERT(err.isOpen());

        cFile& out = cFile::getStandardStream(STANDARD_OUTPUT);
        TESTS_ASSERT(out.isOpenForWrite());
        TESTS_ASSERT(!out.isOpenForRead());

        cFile& in = cFile::getStandardStream(STANDARD_INPUT);
        TESTS_ASSERT(in.isOpenForRead());
        TESTS_ASSERT(!in.isOpenForWrite());

        // The same object is returned on every call
        TESTS_ASSERT(&err == &cFile::getStandardStream(STANDARD_ERROR));
    }

    // Perform the test
    virtual void test()
    {
        test_writeAll();
        test_standardStreams();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestStdio g_globalTestStdio;
