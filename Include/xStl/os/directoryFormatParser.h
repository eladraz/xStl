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

/*
 * directoryFormatParser.h
 *
 * This file is an helper library function set for the cFilename class.
 * Please see xStl/os/filename for more information
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/smartptr.h"

// Forward deceleration
class cFilename;
class cDirectoryFormatParser;
typedef cSmartPtr<cDirectoryFormatParser> cDirectoryFormatParserPtr;

/*
 * Generic filename type parser.
 */
class cDirectoryFormatParser {
protected:
    friend class cFilename;
    /*
     * Return true if 'filename' contains absolute-path information and this
     * handler controls
     */
    virtual bool isAbsolutePath(const cString& filename) = 0;

    /*
     * Translate an absolutePath into an absolutePath without any
     */
    virtual cString translateAbsolutePath(const cString& absolutePath) = 0;

    // Forward deceleration
    class FilenameParser;
    typedef cSmartPtr<FilenameParser> FilenameParserPtr;

    /*
     * Generate the object manager for filenames translation.
     * See FilenameParser for more information
     */
    virtual FilenameParserPtr generateParser() = 0;

    /*
     * The object manager for filenames translation.
     */
    class FilenameParser {
    protected:
        // Only the filename manager can instance this class
        friend class cFilename;

        /*
         * Returns the parent object
         */
        virtual cDirectoryFormatParserPtr getParent() const = 0;

        /*
         * Returns the database name from an absolute-path
         */
        virtual cString getDatabaseName(const cString& absoluteTranslatedPath) const = 0;

        /*
         * Return the directory of the file. The directory contains the base position
         * of the file, including it's database name.
         */
        virtual cString getDirectoryName(const cString& absoluteTranslatedPath) const = 0;

        /*
         * Returns only the portion of the filename.
         * Returns empty string if the path is a directory
         */
        virtual cString getFilename(const cString& absoluteTranslatedPath) const = 0;
    };
};

#endif // __TBA_STL_OS_DIRECTORYFORMATPARSER_H
