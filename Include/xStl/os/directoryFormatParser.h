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

#ifndef __TBA_STL_OS_DIRECTORYFORMATPARSER_H
#define __TBA_STL_OS_DIRECTORYFORMATPARSER_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file directoryFormatParser.h
 * @brief This file is a helper library function set for the cFilename class.
 *
 * @details Please see xStl/os/filename for more information
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/smartptr.h"

// Forward declaration
class cFilename;
class cDirectoryFormatParser;
/// A reference counted pointer to a cDirectoryFormatParser.
typedef cSmartPtr<cDirectoryFormatParser> cDirectoryFormatParserPtr;

/**
 * @brief Generic filename type parser.
 */
class cDirectoryFormatParser {
protected:
    friend class cFilename;
    /**
     * @brief Return true if 'filename' contains absolute-path information and
     *    this
     *   handler controls
     *
     * @param filename The filename to test.
     * @return True if 'filename' contains absolute-path information and this
     *    handler controls.
     */
    virtual bool isAbsolutePath(const cString& filename) = 0;

    /**
     * @brief Translate an absolutePath into an absolutePath without any
     *   relative-path markers.
     *
     * @param absolutePath The absolute path to translate.
     * @return The translated path.
     */
    virtual cString translateAbsolutePath(const cString& absolutePath) = 0;

    // Forward declaration
    class FilenameParser;
    /// A reference counted pointer to a FilenameParser.
    typedef cSmartPtr<FilenameParser> FilenameParserPtr;

    /**
     * @brief Generate the object manager for filenames translation.
     *
     * @details See FilenameParser for more information
     *
     * @return The object manager for filenames translation.
     */
    virtual FilenameParserPtr generateParser() = 0;

    /**
     * @brief The object manager for filenames translation.
     */
    class FilenameParser {
    protected:
        // Only the filename manager can instance this class
        friend class cFilename;

        /**
         * @brief Returns the parent object
         *
         * @return The parent object.
         */
        virtual cDirectoryFormatParserPtr getParent() const = 0;

        /**
         * @brief Returns the database name from an absolute-path
         *
         * @param absoluteTranslatedPath The absolute path, after translation.
         * @return The database name from an absolute-path.
         */
        virtual cString getDatabaseName(const cString& absoluteTranslatedPath) const = 0;

        /**
         * @brief Return the directory of the file.
         *
         * @details The directory contains the base position of the file,
         *    including it's database name.
         *
         * @param absoluteTranslatedPath The absolute path, after translation.
         * @return The directory of the file.
         */
        virtual cString getDirectoryName(const cString& absoluteTranslatedPath) const = 0;

        /**
         * @brief Returns only the portion of the filename.
         *
         * @details Returns empty string if the path is a directory
         *
         * @param absoluteTranslatedPath The absolute path, after translation.
         * @return Only the portion of the filename.
         */
        virtual cString getFilename(const cString& absoluteTranslatedPath) const = 0;
    };
};


/** @} */
#endif // __TBA_STL_OS_DIRECTORYFORMATPARSER_H
