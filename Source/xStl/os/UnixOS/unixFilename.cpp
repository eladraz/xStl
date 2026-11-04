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

#include "xStl/xStlPrecompiled.h"
/*
 * UNIX implementation
 *
 * unixFilename.cpp
 *
 * Implementation file for UNIX based systems (Linux, macOS).
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/filename.h"
#include "xStl/os/unixOS/unixFilename.h"
#include "xStl/data/char.h"
#include "xStl/data/list.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/except/exception.h"

#ifndef XSTL_LINUX
    #error "UNIX source file"
#endif

#undef __USE_MISC
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef XSTL_MACOSX
#include <mach-o/dyld.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

cDirectoryFormatParserPtr cFilename::m_directoryParserHandlers
                        [cFilename::OS_DIRECTORY_FORMAT_PARSER_COUNT] = {
    cDirectoryFormatParserPtr(new cUnixDirectoryParser())
};

// The root directory. A cFilename built from it and an absolute path (which
// ignores the base) translates the path: it removes the '.', '..' and repeated
// separators.
static const character gUnixRootDirectory[] = XSTL_STRING("/");

cFilename cFilename::getCurrentDirectory()
{
    char cdir[PATH_MAX + 1];
    if (getcwd(cdir, PATH_MAX) == NULL)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    cFilename root(gUnixRootDirectory,
        cUnixDirectoryParser::FilenameParserPtr(new cUnixDirectoryParser::cUnixFilenameParser()));
    return cFilename(root, cString(cdir), true, true);
}

cFilename cFilename::getApplicationDirectory()
{
    char path[PATH_MAX + 1];
#ifdef XSTL_MACOSX
    char unresolved[PATH_MAX + 1];
    uint32_t size = PATH_MAX;
    if (_NSGetExecutablePath(unresolved, &size) != 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    // _NSGetExecutablePath() may return a path with symbolic links and '..'
    if (realpath(unresolved, path) == NULL)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
#else
    // The kernel resolves the symbolic links of /proc/self/exe
    ssize_t length = readlink("/proc/self/exe", path, PATH_MAX);
    if ((length <= 0) || (length >= PATH_MAX))
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    path[length] = 0;
#endif
    cFilename root(gUnixRootDirectory,
        cUnixDirectoryParser::FilenameParserPtr(new cUnixDirectoryParser::cUnixFilenameParser()));
    return cFilename(root, cString(path)).getDirectoryName();
}

const cFilename& cFilename::getTempDirectory()
{
    static cFilename gRoot(gUnixRootDirectory,
        cUnixDirectoryParser::FilenameParserPtr(new cUnixDirectoryParser::cUnixFilenameParser()));
    // TMPDIR is set on macOS (a per-user directory) and sometimes on Linux
    const char* tmpdir = getenv("TMPDIR");
    if ((tmpdir == NULL) || (tmpdir[0] != '/'))
        tmpdir = "/tmp/";
    static cFilename gTempDir(gRoot, cString(tmpdir), true, true);
    return gTempDir;
}

//////////////////////////////////////////////////////////////////////////
// cUnixDirectoryParser

bool cUnixDirectoryParser::isAbsolutePath(const cString& filename)
{
    return (filename.length() > 0) &&
           (filename[0] == cFilename::m_directorySeparatorUnix);
}

cString cUnixDirectoryParser::translateAbsolutePath(const cString& absolutePath)
{
    ASSERT(isAbsolutePath(absolutePath));
    // Inits the returned path with the root directory
    cString ret(absolutePath.left(1));
    cList<cString> directories;
    uint i = 1;
    // Some stack algorithm
    while (i < absolutePath.length())
    {
        cString newDirectory = cFilename::readDirectory(absolutePath, i);
        // Advance the iterator by the directory name length and the directory
        // separator
        i+= newDirectory.length() + 1;
        // Empty names come from repeated separators ("a//b")
        if ((newDirectory.length() == 0) ||
            cFilename::isCurrentDirectoryMarker(newDirectory))
            continue;

        if (cFilename::isPreviousDirectoryMarker(newDirectory))
        {
            // Remove the last directory if there is one...
            if (directories.begin() != directories.end())
                directories.remove(--directories.end());
        } else
        {
            // UNIX names may contain any character except the separator, so
            // checkRelativeNameValidation() (DOS rules) is not used here.
            directories.append(newDirectory);
        }
    }

    if (directories.begin() == directories.end())
        return ret;

    cList<cString>::iterator j = directories.begin();
    cList<cString>::iterator last = directories.end() - 1;
    for (; j != directories.end(); ++j)
    {
        ret+= (*j);
        if ((j != last) ||
            (cFilename::isDirectorySeparator(
                absolutePath[absolutePath.length()-1])))
            ret+= cFilename::m_directorySeparatorUnix;
    }

    return ret;
}

cUnixDirectoryParser::FilenameParserPtr cUnixDirectoryParser::generateParser()
{
    return FilenameParserPtr(new cUnixFilenameParser());
}

cDirectoryFormatParserPtr
    cUnixDirectoryParser::cUnixFilenameParser::getParent() const
{
    return cDirectoryFormatParserPtr(new cUnixDirectoryParser());
}

cString cUnixDirectoryParser::cUnixFilenameParser::getDatabaseName(
    const cString& absoluteTranslatedPath) const
{
    return cString();
}

cString cUnixDirectoryParser::cUnixFilenameParser::getDirectoryName(
    const cString& absoluteTranslatedPath) const
{
    // Find until the last directory separator
    return absoluteTranslatedPath.left(
        getLastDirectorySeparator(absoluteTranslatedPath) + 1);
}

cString cUnixDirectoryParser::cUnixFilenameParser::getFilename(
    const cString& absoluteTranslatedPath) const
{
    return absoluteTranslatedPath.right(absoluteTranslatedPath.length() -
                    getLastDirectorySeparator(absoluteTranslatedPath) - 1);
}

character cUnixDirectoryParser::cUnixFilenameParser::getDirectorySeparator() const
{
    return cFilename::m_directorySeparatorUnix;
}

uint cUnixDirectoryParser::cUnixFilenameParser::getLastDirectorySeparator(
    const cString& filename) const
{
    ASSERT(filename.length() > 0);
    for (uint i = filename.length(); i > 0; --i)
    {
        if (cFilename::isDirectorySeparator(filename[i - 1]))
            return i - 1;
    }

    // Never should reach here! A translated path starts with '/'.
    CHECK_FAIL();
}
