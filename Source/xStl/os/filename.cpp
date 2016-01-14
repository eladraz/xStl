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
 * filename.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/data/string.h"
#include "xStl/os/filename.h"

const character cFilename::m_directorySeperatorWindows = XSTL_CHAR('\\');
const character cFilename::m_directorySeperatorUnix = XSTL_CHAR('/');
const character cFilename::m_directoryCurrentDirectory[] = XSTL_STRING(".");
const character cFilename::m_directoryPreviousDirectory[] = XSTL_STRING("..");

cFilename::cFilename(const cString& fullpath,
                     const cDirectoryFormatParser::FilenameParserPtr& parser) :
    m_filename(fullpath),
    m_directoryParser(parser)
{
    CHECK(!parser.isEmpty());
}

cFilename::cFilename(const cFilename& basePath,
                     const cString& relativeFilenamePath,
                     bool shouldStepBackward,
                     bool enforceDirectory) :
    m_directoryParser(basePath.m_directoryParser)
{
    // Seperate between two cases:
    //  1. Absolute path
    //  2. Relative path

    // Check absolute path
    uint i;
    for (i = 0; i < OS_DIRECTORY_FORMAT_PARSER_COUNT; i++)
    {
        if (m_directoryParserHandlers[i]->isAbsolutePath(relativeFilenamePath))
        {
            // Construct a new filename handler from it
            m_filename = m_directoryParserHandlers[i]->translateAbsolutePath(
                                                        relativeFilenamePath);
            if (enforceDirectory)
                this->enforceDirectory();

            m_directoryParser = m_directoryParserHandlers[i]->generateParser();
            // Check whether the base path must exist for filename
            if (!shouldStepBackward)
                CHECK(m_filename.left(basePath.m_filename.length()).
                             icompare(basePath.m_filename) == cString::EqualTo);
            // And done!
            return;
        }
    }

    // Append relative path to basePath. Seperate between two factors:
    // New database is included. or not.
    for (i = 0; i < OS_DIRECTORY_FORMAT_PARSER_COUNT; i++)
    {
        // TODO!
    }

    // Test that the nasePath is really a directory
    ASSERT(basePath.getFullPath().length() > 0);
    m_filename = basePath.m_filename + relativeFilenamePath;
    m_filename = m_directoryParser->getParent()->translateAbsolutePath(m_filename);
    if (enforceDirectory)
        this->enforceDirectory();

    // Check whether the base path must exist for filename
    if (!shouldStepBackward)
        CHECK(m_filename.left(basePath.m_filename.length()).
        icompare(basePath.m_filename) == cString::EqualTo);
}

void cFilename::enforceDirectory()
{
    // TODO! Windows and UNIX styles...
    if (!isDirectorySeperator(m_filename[m_filename.length() - 1]))
        m_filename+= m_directorySeperatorWindows;
}

const cString& cFilename::getFullPath() const
{
    return m_filename;
}

cString cFilename::getDatabaseName() const
{
    return m_directoryParser->getDatabaseName(m_filename);
}

cFilename cFilename::getDirectoryName() const
{
    return cFilename(m_directoryParser->getDirectoryName(m_filename),
                     m_directoryParser);
}

cString cFilename::getFilename() const
{
    return m_directoryParser->getFilename(m_filename);
}

bool cFilename::isDirectorySeperator(const character ch)
{
    return (ch == m_directorySeperatorWindows) ||
           (ch == m_directorySeperatorUnix);
}

bool cFilename::isPreviousDirectoryMarker(const cString& directoryName)
{
    return (directoryName.icompare(m_directoryPreviousDirectory) ==
            cString::EqualTo);
}

bool cFilename::isCurrentDirectoryMarker(const cString& directoryName)
{
    return (directoryName.icompare(m_directoryCurrentDirectory) ==
            cString::EqualTo);
}

cString cFilename::readDirectory(const cString& filename,
                                 uint startPosition)
{
    uint end;
    for (end = startPosition;
         (end < filename.length()) && (!isDirectorySeperator(filename[end]));
         end++);
    return filename.part(startPosition, end);
}

bool cFilename::checkRelativeNameValidation(const cString& relativeName)
{
    for (uint i = 0; i < relativeName.length(); i++)
    {
        character ch = relativeName[i];
        if (!(cChar::isLetter(ch) ||
              cChar::isDigit(ch) ||
              cChar::isBraces(ch) ||
              cChar::isSeperator(ch) ||
              isDirectorySeperator(ch) ||
              // All digits except ?, *, |, ", \t and of-course '.'!
              (ch == XSTL_CHAR('~')) || // Long names extention
              (ch == XSTL_CHAR('!')) ||
              (ch == XSTL_CHAR('@')) ||
              (ch == XSTL_CHAR('#')) ||
              (ch == XSTL_CHAR('$')) ||
              (ch == XSTL_CHAR('%')) ||
              (ch == XSTL_CHAR('^')) ||
              (ch == XSTL_CHAR('&')) ||
              (ch == XSTL_CHAR('-')) ||
              (ch == XSTL_CHAR('_')) ||
              (ch == XSTL_CHAR('=')) ||
              (ch == XSTL_CHAR('+')) ||
              (ch == XSTL_CHAR('\'')) ||
              (ch == XSTL_CHAR(' '))))
        {
            // Illegal character
            return false;
        }

    }

    // If we reached here, then all good
    return true;
}
