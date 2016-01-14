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
 * test_filename.cpp
 *
 * Tests the filename parser implementation
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/filename.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

class cTestFilename : public cTestObject {
public:

    void test_dos_names()
    {
        cFilename file(cFilename::getCurrentDirectory(), "c:\\", true);
        TESTS_ASSERT_EQUAL(file.getFullPath().icompare("c:\\"), cString::EqualTo);
        TESTS_ASSERT_EQUAL(file.getDatabaseName().icompare("c:"), cString::EqualTo);
        TESTS_ASSERT_EQUAL(file.getFilename().length(), 0);

        cFilename tempDirectory(cFilename::getCurrentDirectory(), "c:\\temp\\", true);
        TESTS_ASSERT_EQUAL(tempDirectory.getFilename().length(), 0);
        TESTS_ASSERT_EQUAL(tempDirectory.getFullPath().icompare("c:\\temp\\"), cString::EqualTo);

        cFilename currentFile(tempDirectory, "temp.tmp", true);
        TESTS_ASSERT_EQUAL(currentFile.getFullPath().icompare("c:\\temp\\temp.tmp"), cString::EqualTo);
        TESTS_ASSERT_EQUAL(currentFile.getDirectoryName().getFullPath(),
                           tempDirectory.getFullPath());

        cFilename previousFile(tempDirectory, "..\\windows\\temp.tmp", true);
        TESTS_ASSERT_EQUAL(previousFile.getFullPath().icompare("c:\\windows\\temp.tmp"), cString::EqualTo);

        cFilename p1(tempDirectory, "..\\WINDOWS\\SYSTEM32\\Drivers\\..\\..\\..\\Temp\\temp.tmp", false);
        TESTS_ASSERT_EQUAL(p1.getFullPath().icompare("c:\\temp\\temp.tmp"), cString::EqualTo);
        TESTS_EXCEPTION(cFilename previousFile(tempDirectory, "..\\windows\\temp.tmp", false));

        // TESTS_EXCEPTION(cFilename temp(cFilename::getCurrentDirectory(), "D:"));
    }

    // Perform the test
    virtual void test()
    {
        test_dos_names();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestFilename g_globalTestFilename;
