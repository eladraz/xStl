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

// For these files, the WIN32 macro must be defined...
#ifdef WIN32

#ifndef __TBA_STL_OS_WINOS_WINFILENAME_H
#define __TBA_STL_OS_WINOS_WINFILENAME_H

/**
 * @addtogroup xstl_os_windows
 * @{
 */

/**
 * @file winFilename.h
 * @brief A set of functions which parse the DOS filename structs.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/os/directoryFormatParser.h"
#include "xStl/os/filename.h"

/**
 * @brief A set of functions which parse the DOS filename structs.
 *
 * @details A DOS format is built up from:
 *    \<DRIVE-LETTER\>:\\\<Directory\>\\\<\>
 */
class cDosDirectoryParser : public cDirectoryFormatParser {
protected:
    friend class cFilename;
    /**
     * @param filename The filename to test.
     * @return True if the format of filename starts with 'X:\\'.
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
     * @brief The filenames translation object of the DOS filename format.
     *
     * @see cDirectoryFormatParser::FilenameParser
     */
    class cDosFilenameParser : public cDirectoryFormatParser::FilenameParser {
    public:
        /**
         * @brief Default empty constructor
         */
        cDosFilenameParser() {};

    protected:
        friend class cFilename;

        /**
         * @brief Returns the parent object
         *
         * @return The parent object.
         */
        virtual cDirectoryFormatParserPtr getParent() const;

        /**
         * @brief Returns the database name from an absolute-path
         *
         * @param absoluteTranslatedPath The absolute path, after translation.
         * @return The database name from an absolute-path.
         */
        virtual cString getDatabaseName(const cString& absoluteTranslatedPath) const;

        /**
         * @brief Return the directory of the file.
         *
         * @details The directory contains the base position of the file,
         *    including its database name.
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
#endif // __TBA_STL_OS_WINOS_WINFILENAME_H

#endif // WIN32