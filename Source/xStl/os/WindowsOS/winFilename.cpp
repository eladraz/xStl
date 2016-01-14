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
// For this files, the WIN32 macro must be defined...
#ifdef WIN32

/*
 * W I N D O W S implementation
 *
 * winFilename.cpp
 *
 * Implementation file for the Win32 API.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/filename.h"
#include "xStl/os/WindowsOS/winFilename.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/except/exception.h"
#include <windows.h>

cDirectoryFormatParserPtr cFilename::m_directoryParserHandlers
                        [cFilename::OS_DIRECTORY_FORMAT_PARSER_COUNT] = {
    cDirectoryFormatParserPtr(new cDosDirectoryParser())
};

cFilename cFilename::getCurrentDirectory()
{
    character cdir[MAX_PATH+1];
    if (GetCurrentDirectory(MAX_PATH, cdir) == 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    cString name(cdir);
    if (!isDirectorySeperator(name[name.length() - 1]))
        name+= m_directorySeperatorWindows;

    return cFilename(name,
        cDosDirectoryParser::FilenameParserPtr(new cDosDirectoryParser::cDosFilenameParser()));
}

// This function is global since it used as initializer for the
// 'cFilename::getTempDirectory()' static variable constructor.
cString win32getTempDirectory()
{
    character tdir[MAX_PATH+1];
    if (GetTempPath(MAX_PATH, tdir) == 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    return cString(tdir);
}


const cFilename& cFilename::getTempDirectory()
{
    static cFilename gTempDir(win32getTempDirectory(),
        cDosDirectoryParser::FilenameParserPtr(new cDosDirectoryParser::cDosFilenameParser()));
    return gTempDir;
}

//////////////////////////////////////////////////////////////////////////
// cDosDirectoryParser

bool cDosDirectoryParser::isAbsolutePath(const cString& filename)
{
    if (filename.length() < 3)
        return false;

    if ((cChar::getUppercase(filename[0]) < 'A') ||
        (cChar::getUppercase(filename[0]) > 'Z'))
        return false;

    if ((filename[1] != ':') ||
        (!cFilename::isDirectorySeperator(filename[2])))
        return false;

    return true;
}

cString cDosDirectoryParser::translateAbsolutePath(const cString& absolutePath)
{
    ASSERT(isAbsolutePath(absolutePath));
    // Inits the returned path A:\ string
    cString ret(absolutePath.left(3));
    cList<cString> directories;
    uint i = 3;
    // Some stack algorithm
    while (i < absolutePath.length())
    {
        cString newDirectory = cFilename::readDirectory(absolutePath, i);
        // Add the iterator the directory name and the directory separator
        i+= newDirectory.length() + 1;
        if (!cFilename::isCurrentDirectoryMarker(newDirectory))
        {
            if (cFilename::isPreviousDirectoryMarker(newDirectory))
            {
                // Remove the last directory if there is one...
                if (directories.begin() != directories.end())
                    directories.remove(--directories.end());
            } else
            {
                if (cFilename::checkRelativeNameValidation(newDirectory))
                    directories.append(newDirectory);
                else
                    // Invalid directory name
                    XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
            }
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
            (cFilename::isDirectorySeperator(absolutePath[absolutePath.length()-1])))
            ret+= cFilename::m_directorySeperatorWindows;
    }

    return ret;
}

cDosDirectoryParser::FilenameParserPtr cDosDirectoryParser::generateParser()
{
    return FilenameParserPtr(new cDosFilenameParser());
}

cDirectoryFormatParserPtr
    cDosDirectoryParser::cDosFilenameParser::getParent() const
{
    return cDirectoryFormatParserPtr(new cDosDirectoryParser());
}

cString cDosDirectoryParser::cDosFilenameParser::getDatabaseName(
    const cString& absoluteTranslatedPath) const
{
    ASSERT(absoluteTranslatedPath.length() > 2);
    ASSERT(absoluteTranslatedPath[1] == ':');
    return absoluteTranslatedPath.left(2);
}

cString cDosDirectoryParser::cDosFilenameParser::getDirectoryName(
    const cString& absoluteTranslatedPath) const
{
    // Find until the last directory separator
    return absoluteTranslatedPath.left(
        getLastDirectorySeparator(absoluteTranslatedPath) + 1);
}

cString cDosDirectoryParser::cDosFilenameParser::getFilename(
    const cString& absoluteTranslatedPath) const
{
    return absoluteTranslatedPath.right(absoluteTranslatedPath.length() -
                    getLastDirectorySeparator(absoluteTranslatedPath) - 1);
}

uint cDosDirectoryParser::cDosFilenameParser::getLastDirectorySeparator(
    const cString& filename) const
{
    for (uint i = filename.length() - 1; i >= 2; --i)
    {
        if (cFilename::isDirectorySeperator(filename[i]))
            return i;
    }

    // Never should reach here!
    CHECK_FAIL();
}

#endif // WIN32
