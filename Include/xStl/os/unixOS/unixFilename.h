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

#ifndef __TBA_STL_OS_UNIXOS_UNIXFILENAME_H
#define __TBA_STL_OS_UNIXOS_UNIXFILENAME_H

/**
 * @addtogroup xstl_os_unix
 * @{
 */

/**
 * @file unixFilename.h
 * @brief A set of functions which parse the UNIX filename structs.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/os/directoryFormatParser.h"
#include "xStl/os/filename.h"

/**
 * @brief A set of functions which parse the UNIX filename structs.
 *
 * @details A UNIX path is built up from: /\<Directory\>/\<Filename\>
 *   There is no database name (drive letter), so getDatabaseName() returns an
 *   empty string. Both '/' and '\\' are treated as directory separators while
 *   parsing (as in the DOS parser), and the translated path always uses '/'.
 */
class cUnixDirectoryParser : public cDirectoryFormatParser {
protected:
    friend class cFilename;
    /**
     * @param filename The filename to test.
     * @return True if the filename starts with '/'.
     * @see cDirectoryFormatParser::isAbsolutePath
     */
    virtual bool isAbsolutePath(const cString& filename);

    /**
     * @brief Replace all the '.' and the '..' directories into their original
     *    meaning
     *
     * @param absolutePath The absolute path to translate.
     * @return The translated path.
     * @see cDirectoryFormatParser::translateAbsolutePath
     */
    virtual cString translateAbsolutePath(const cString& absolutePath);

    /**
     * @return The object manager for filenames translation.
     * @see cDirectoryFormatParser::generateParser
     */
    virtual cDirectoryFormatParser::FilenameParserPtr generateParser();

    /**
     * @brief The filenames translation object of the UNIX filename format.
     *
     * @see cDirectoryFormatParser::FilenameParser
     */
    class cUnixFilenameParser : public cDirectoryFormatParser::FilenameParser {
    public:
        /**
         * @brief Default empty constructor
         */
        cUnixFilenameParser() {};

    protected:
        friend class cFilename;

        /**
         * @brief Returns the parent object
         *
         * @return The parent object.
         */
        virtual cDirectoryFormatParserPtr getParent() const;

        /**
         * @brief UNIX paths have no database name.
         *
         * @param absoluteTranslatedPath The absolute path, after translation.
         * @return An empty string.
         */
        virtual cString getDatabaseName(const cString& absoluteTranslatedPath) const;

        /**
         * @brief Return the directory of the file, ending with '/'.
         *
         * @param absoluteTranslatedPath The absolute path, after translation.
         * @return The directory of the file.
         */
        virtual cString getDirectoryName(const cString& absoluteTranslatedPath) const;

        /**
         * @brief Returns only the portion of the filename.
         *
         * @details Returns empty string if the path is a directory
         *
         * @param absoluteTranslatedPath The absolute path, after translation.
         * @return Only the portion of the filename.
         */
        virtual cString getFilename(const cString& absoluteTranslatedPath) const;

        /**
         * @return The UNIX directory separator ('/').
         */
        virtual character getDirectorySeparator() const;

    private:
        /**
         * @brief Return the position of the last directory separator
         *
         * @return The position of the last directory separator.
         */
        uint getLastDirectorySeparator(const cString& filename) const;
    };
};


/** @} */
#endif // __TBA_STL_OS_UNIXOS_UNIXFILENAME_H
