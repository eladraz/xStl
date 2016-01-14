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

// For this files, the WIN32 macro must be defined...
#ifdef WIN32

#ifndef __TBA_STL_OS_WINOS_WINFILENAME_H
#define __TBA_STL_OS_WINOS_WINFILENAME_H

/*
 * winFilename.h
 *
 * A set of function which parse the DOS filename structs.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/directoryFormatParser.h"
#include "xStl/os/filename.h"

/*
 * A set of function which parse the DOS filename structs.
 * A DOS format is build up from:
 *    <DRIVE-LETTER>:\<Directory>\<>
 */
class cDosDirectoryParser : public cDirectoryFormatParser {
protected:
    friend class cFilename;
    /*
     * See cDirectoryFormatParser::isAbsolutePath
     *
     * Return true if the format of filename starts with 'X:\'
     */
    virtual bool isAbsolutePath(const cString& filename);

    /*
     * See cDirectoryFormatParser::translateAbsolutePath
     *
     * Replace all the '.' and the '..' directories into thier original meanning
     */
    virtual cString translateAbsolutePath(const cString& absolutePath);

    /*
     * See cDirectoryFormatParser::generateParser
     */
    virtual cDirectoryFormatParser::FilenameParserPtr generateParser();

    /*
     * See cDirectoryFormatParser::FilenameParser
     */
    class cDosFilenameParser : public cDirectoryFormatParser::FilenameParser {
    public:
        /*
         * Default empty constructor
         */
        cDosFilenameParser() {};

    protected:
        friend class cFilename;

        /*
         * Returns the parent object
         */
        virtual cDirectoryFormatParserPtr getParent() const;

        /*
         * Returns the database name from an absolute-path
         */
        virtual cString getDatabaseName(const cString& absoluteTranslatedPath) const;

        /*
         * Return the directory of the file. The directory contains the base position
         * of the file, including it's database name.
         */
        virtual cString getDirectoryName(const cString& absoluteTranslatedPath) const;

        /*
         * Returns only the portion of the filename.
         * Returns empty string if the path is a directory
         */
        virtual cString getFilename(const cString& absoluteTranslatedPath) const;

    private:
        /*
         * Return the position of the last directory separator
         */
        uint getLastDirectorySeparator(const cString& filename) const;
    };
};

#endif // __TBA_STL_OS_WINOS_WINFILENAME_H

#endif // WIN32