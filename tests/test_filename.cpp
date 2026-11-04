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
#include "xStl/os/file.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "tests.h"

class cTestFilename : public cTestObject {
public:

#ifdef XSTL_WINDOWS
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

        // '/' is also a separator; the result uses '\\'
        cFilename slashes(tempDirectory, "dir/temp.tmp", true);
        TESTS_ASSERT_EQUAL(slashes.getFullPath().icompare("c:\\temp\\dir\\temp.tmp"), cString::EqualTo);

        // An enforced directory ends with the DOS separator
        cFilename directory(tempDirectory, "dir", true, true);
        TESTS_ASSERT_EQUAL(directory.getFullPath().icompare("c:\\temp\\dir\\"), cString::EqualTo);

        // TESTS_EXCEPTION(cFilename temp(cFilename::getCurrentDirectory(), "D:"));
    }
#endif

#ifdef XSTL_LINUX
    void test_unix_names()
    {
        cFilename root(cFilename::getCurrentDirectory(), "/", true);
        TESTS_ASSERT_EQUAL(root.getFullPath(), cString("/"));
        TESTS_ASSERT_EQUAL(root.getDatabaseName().length(), 0);
        TESTS_ASSERT_EQUAL(root.getFilename().length(), 0);

        cFilename tempDirectory(cFilename::getCurrentDirectory(), "/tmp/", true);
        TESTS_ASSERT_EQUAL(tempDirectory.getFilename().length(), 0);
        TESTS_ASSERT_EQUAL(tempDirectory.getFullPath(), cString("/tmp/"));

        cFilename currentFile(tempDirectory, "temp.tmp", true);
        TESTS_ASSERT_EQUAL(currentFile.getFullPath(), cString("/tmp/temp.tmp"));
        TESTS_ASSERT_EQUAL(currentFile.getFilename(), cString("temp.tmp"));
        TESTS_ASSERT_EQUAL(currentFile.getDirectoryName().getFullPath(),
                           tempDirectory.getFullPath());

        cFilename previousFile(tempDirectory, "../usr/temp.tmp", true);
        TESTS_ASSERT_EQUAL(previousFile.getFullPath(), cString("/usr/temp.tmp"));

        // '..' never goes above the root
        cFilename aboveRoot(tempDirectory, "../../../temp.tmp", true);
        TESTS_ASSERT_EQUAL(aboveRoot.getFullPath(), cString("/temp.tmp"));

        cFilename p1(tempDirectory, "../usr/lib/./../../tmp/temp.tmp", false);
        TESTS_ASSERT_EQUAL(p1.getFullPath(), cString("/tmp/temp.tmp"));
        TESTS_EXCEPTION(cFilename previousFile(tempDirectory, "../usr/temp.tmp", false));

        // Repeated separators are removed, '\\' is also a separator
        cFilename repeated(tempDirectory, "a//b\\c.txt", true);
        TESTS_ASSERT_EQUAL(repeated.getFullPath(), cString("/tmp/a/b/c.txt"));

        // An absolute path ignores the base
        cFilename absolute(tempDirectory, "/usr/./lib/", true);
        TESTS_ASSERT_EQUAL(absolute.getFullPath(), cString("/usr/lib/"));

        // An enforced directory ends with the UNIX separator
        cFilename directory(tempDirectory, "dir", true, true);
        TESTS_ASSERT_EQUAL(directory.getFullPath(), cString("/tmp/dir/"));
        cFilename absoluteDirectory(tempDirectory, "/usr", true, true);
        TESTS_ASSERT_EQUAL(absoluteDirectory.getFullPath(), cString("/usr/"));

        // UNIX names may contain characters that DOS names may not
        cFilename special(tempDirectory, "what?*.txt", true);
        TESTS_ASSERT_EQUAL(special.getFilename(), cString("what?*.txt"));

        // The temporary directory is an absolute directory
        const cString& temp = cFilename::getTempDirectory().getFullPath();
        TESTS_ASSERT(temp.length() > 1);
        TESTS_ASSERT_EQUAL(temp[0], XSTL_CHAR('/'));
        TESTS_ASSERT_EQUAL(temp[temp.length() - 1], XSTL_CHAR('/'));
    }
#endif

    void test_current_directory()
    {
        cFilename current(cFilename::getCurrentDirectory());
        const cString& path = current.getFullPath();
        TESTS_ASSERT(path.length() > 0);
        TESTS_ASSERT(cFilename::isDirectorySeparator(path[path.length() - 1]));
        TESTS_ASSERT_EQUAL(current.getFilename().length(), 0);

        cFilename file(current, "file.txt");
        TESTS_ASSERT_EQUAL(file.getFilename(), cString("file.txt"));
        TESTS_ASSERT_EQUAL(file.getDirectoryName().getFullPath(), path);
    }

    void test_application_directory()
    {
        cFilename application(cFilename::getApplicationDirectory());
        const cString& path = application.getFullPath();
        TESTS_ASSERT(path.length() > 0);
        TESTS_ASSERT(cFilename::isDirectorySeparator(path[path.length() - 1]));
        TESTS_ASSERT_EQUAL(application.getFilename().length(), 0);

        // The test executable is in that directory, whatever the current
        // directory is. Its name depends on the build system: CMake,
        // MSBuild, or an uninstalled libtool build.
        static const char* names[] = {
            "xstl_tests", "xstl_tests.exe", "xStlTests.exe", "lt-xstl_tests"
        };
        bool isFound = false;
        for (uint i = 0; i < sizeof(names) / sizeof(names[0]); i++)
        {
            if (cFile::isFileExist(cFilename(application, names[i]).getFullPath()))
                isFound = true;
        }
        TESTS_ASSERT(isFound);
        TESTS_ASSERT(!cFile::isFileExist(cFilename(application,
                                         "no_such_file.tmp").getFullPath()));
    }

    // Perform the test
    virtual void test()
    {
#ifdef XSTL_WINDOWS
        test_dos_names();
#endif
#ifdef XSTL_LINUX
        test_unix_names();
#endif
        test_current_directory();
        test_application_directory();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestFilename g_globalTestFilename;
