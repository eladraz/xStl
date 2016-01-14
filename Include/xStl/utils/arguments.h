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

#ifndef __TBA_STL_ARGUMENTS_H
#define __TBA_STL_ARGUMENTS_H

/*
 * arguments.h
 *
 * Analyze arguments from the user command
 * line.
 */

#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/hash.h"

/*
 * cArguments class
 *
 * Provide easy way to parse command line
 * arguments.
 *
 * Command line arguments
 * Can be:
 *
 *   1. Macro
 *      Start with -name or /name
 *      Macro can be with a string description or without.
 *      for example: -i filename.txt
 *         Create a new macro name i with string 'filename.txt'
 *
 *   2. Strings
 *      A list of strings that become in the start of the
 *      line or in the end.
 *      Input like:
 *        tcpdump -n eth0  [In this program -nlxs 5000 will not be parsed right]
 *
 *
 *  In case of advanced parseing it possibole
 *  to overload this class and modify the
 *  OnXXX() functions.
 */
class cArguments
{
public:
    // Constructors, Deconstructors
    cArguments();
    cArguments(const int argc, const char **argv);
    ~cArguments();

    void Parse(const int argc, const char **argv);

    // Argument information
    cString get_command(const cString& macro) const;
    BOOL    check_macro(const cString& macro) const;

    unsigned int get_string_count() const;
    cString      get_string(unsigned int index) const;


    // Parser helper function
    void add_macro (const cString &macro, const cString& desciption);
    void add_string(const cString &string);


    // Parser commands
    virtual void OnToken (cString& token_name);
    virtual void OnMacro (cString& macro_name, cString &description);
    virtual void OnString(cString& string);

private:
    // Disable copy things
    cArguments(const cArguments& other);
    cArguments & operator = (const cArguments& other);

    cHash<cString,cString> argv_hash;
    cList<cString>         argv_str;
};

/*
 * cArguments::cArguments
 *
 * Defualt constructor, Init the class.
 */
cArguments::cArguments()
{
}

/*
 * cArguments::cArguments(const int argc, const char **argv)
 *
 * Init the class, Parse the data
 * thorw exception in case of an error
 */
cArguments::cArguments(const int argc, const char **argv)
{
    cArguments();
    Parse(argc, argv);
}

/*
 * cArguments::~cArguments
 *
 * Init the class.
 */
cArguments::~cArguments()
{
}

#endif // __TBA_STL_ARGUMENTS_H
