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
 * test_fileExt.cpp
 *
 * Tests the file extensions of cFile:
 *  - APPEND mode keeps the existing content and writes at the end, also
 *    after a seek back to the start (read/write "a+" mode).
 *  - renameFile moves a file and fails for a missing source.
 *  - tryOpen opens a file without throwing, and reports the OS error code
 *    (ENOENT for a missing file, EBUSY for an object which is already open).
 *  - isTerminal can be called on the standard streams.
 *
 * All the files are created under the temporary directory with random names,
 * and they are removed by a guard object, even when a check fails.
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/file.h"
#include "xStl/os/filename.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

#include <errno.h>

#define FILEEXT_PREFIX "xstl_fileext_"

class cTestFileExt : public cTestObject {
public:
    // Removes the file when the guard goes out of scope
    class cTempFile {
    public:
        cTempFile(const cString& path) : m_path(path) {}
        ~cTempFile() { cFile::deleteFile(m_path); }
        const cString& getPath() const { return m_path; }
    private:
        cString m_path;
    };

    // Return a new unique path in the temporary directory
    cString makePath(const char* suffix)
    {
        cFilename path(cFilename::getTempDirectory(),
                       cString(FILEEXT_PREFIX) + randomizeName(12) +
                       cString(suffix));
        return path.getFullPath();
    }

    // Create (or truncate) the file with the given content
    void writeFile(const cString& path, const char* content, const uint length)
    {
        cFile file(path, cFile::CREATE | cFile::WRITE);
        file.writeAll(content, length);
    }

    // Read the whole file into 'buffer', return the number of bytes read
    uint readFile(const cString& path, uint8* buffer, const uint capacity)
    {
        cFile file(path, cFile::READ);
        uint total = 0;
        while (total < capacity)
        {
            uint got = file.read(buffer + total, capacity - total);
            if (got == 0)
                break;
            total += got;
        }
        return total;
    }

    // Writes to a file opened with APPEND add to its end
    void test_append()
    {
        cTempFile file(makePath(".dat"));
        writeFile(file.getPath(), "ab", 2);

        {
            cFile append(file.getPath(), cFile::WRITE | cFile::APPEND);
            append.writeAll("c", 1);
        }

        // CREATE with APPEND creates only a missing file, no truncation
        {
            cFile append(file.getPath(), cFile::CREATE | cFile::WRITE | cFile::APPEND);
            append.writeAll("d", 1);
        }

        uint8 read[8] = { 0 };
        TESTS_ASSERT_EQUAL(readFile(file.getPath(), read, sizeof(read)), 4);
        TESTS_ASSERT_EQUAL(read[0], (uint8)'a');
        TESTS_ASSERT_EQUAL(read[1], (uint8)'b');
        TESTS_ASSERT_EQUAL(read[2], (uint8)'c');
        TESTS_ASSERT_EQUAL(read[3], (uint8)'d');
    }

    // "a+" (CREATE|READWRITE|APPEND): reads follow seek, writes always append
    void test_appendReadWrite()
    {
        cTempFile file(makePath(".dat"));
        {
            cFile rw(file.getPath(),
                     cFile::CREATE | cFile::READWRITE | cFile::APPEND);
            rw.writeAll("ab", 2);

            // Read back the existing content
            rw.seek(0, basicInput::IO_SEEK_SET);
            uint8 read[2] = { 0 };
            TESTS_ASSERT_EQUAL(rw.read(read, 2), 2);
            TESTS_ASSERT_EQUAL(read[0], (uint8)'a');
            TESTS_ASSERT_EQUAL(read[1], (uint8)'b');

            // Move back to the start: a plain write would overwrite "ab", an
            // append write goes to the end
            rw.seek(0, basicInput::IO_SEEK_SET);
            rw.writeAll("c", 1);
        }

        uint8 content[8] = { 0 };
        TESTS_ASSERT_EQUAL(readFile(file.getPath(), content, sizeof(content)), 3);
        TESTS_ASSERT_EQUAL(content[0], (uint8)'a');
        TESTS_ASSERT_EQUAL(content[1], (uint8)'b');
        TESTS_ASSERT_EQUAL(content[2], (uint8)'c');
    }

    // Rename moves the file, and a missing source is reported as failure
    void test_renameFile()
    {
        cTempFile from(makePath(".from"));
        cTempFile to(makePath(".to"));
        writeFile(from.getPath(), "xyz", 3);

        TESTS_ASSERT(cFile::renameFile(from.getPath(), to.getPath()));
        TESTS_ASSERT(!cFile::isFileExist(from.getPath()));
        TESTS_ASSERT(cFile::isFileExist(to.getPath()));

        uint8 read[3] = { 0 };
        TESTS_ASSERT_EQUAL(readFile(to.getPath(), read, 3), 3);
        TESTS_ASSERT_EQUAL(read[0], (uint8)'x');
        TESTS_ASSERT_EQUAL(read[1], (uint8)'y');
        TESTS_ASSERT_EQUAL(read[2], (uint8)'z');

        // The source is gone now, so a second rename must fail
        TESTS_ASSERT(!cFile::renameFile(from.getPath(), to.getPath()));
    }

    // tryOpen of a missing file returns false and sets errno-style code
    void test_tryOpenMissing()
    {
        cTempFile missing(makePath(".missing"));
        cFile file;
        int osError = 0;
        TESTS_ASSERT(!file.tryOpen(missing.getPath(), cFile::READ, osError));
        TESTS_ASSERT(!file.isOpen());
        TESTS_ASSERT_EQUAL(osError, ENOENT);
    }

    // tryOpen opens an existing file, and refuses to reopen an open object
    void test_tryOpen()
    {
        cTempFile file(makePath(".dat"));
        writeFile(file.getPath(), "q", 1);

        cFile object;
        int osError = -1;
        TESTS_ASSERT(object.tryOpen(file.getPath(), cFile::READ, osError));
        TESTS_ASSERT_EQUAL(osError, 0);
        TESTS_ASSERT(object.isOpen());
        TESTS_ASSERT(object.isOpenForRead());

        uint8 byte = 0;
        TESTS_ASSERT_EQUAL(object.read(&byte, 1), 1);
        TESTS_ASSERT_EQUAL(byte, (uint8)'q');

        osError = 0;
        TESTS_ASSERT(!object.tryOpen(file.getPath(), cFile::READ, osError));
        TESTS_ASSERT_EQUAL(osError, EBUSY);
        TESTS_ASSERT(object.isOpen());
    }

    // The terminal test depends on how the test is run, so only the calls
    // are checked (they must not throw).
    void test_isTerminal()
    {
        cFile::isTerminal(STANDARD_INPUT);
        cFile::isTerminal(STANDARD_OUTPUT);
        cFile::isTerminal(STANDARD_ERROR);
    }

    // Perform the test
    virtual void test()
    {
        test_append();
        test_appendReadWrite();
        test_renameFile();
        test_tryOpenMissing();
        test_tryOpen();
        test_isTerminal();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestFileExt g_globalTestFileExt;
