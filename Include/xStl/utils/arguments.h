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

/**
 * @addtogroup xstl_utils
 * @{
 */

/**
 * @file arguments.h
 * @brief Analyze arguments from the user command line.
 */

#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/hash.h"

/**
 * @brief Parse and query the command line arguments of an application.
 *
 * @details Provide an easy way to parse command line
 *   arguments.
 *
 * Command line arguments can be:
 *
 * 1. Macro: starts with -name or /name.
 *    A macro can be with a string description or without.
 *    For example: `-i filename.txt` creates a new macro named `i` with the
 *    string `filename.txt`.
 *
 * 2. Strings: a list of strings that appear at the start of the
 *    line or at the end.
 *    Input like: `tcpdump -n eth0`
 *    (in this program `-nlxs 5000` will not be parsed right).
 *
 * In case of advanced parsing it is possible
 * to overload this class and modify the
 * OnXXX() functions.
 */
class cArguments
{
public:
    /**
     * @brief Constructors, Destructors
     */
    cArguments();
    /**
     * @brief Constructor. Parse the command line arguments.
     *
     * @param argc The number of arguments.
     * @param argv The arguments array.
     */
    cArguments(const int argc, const char **argv);
    ~cArguments();

    /**
     * @brief Parse the command line arguments.
     *
     * @param argc The number of arguments.
     * @param argv The arguments array.
     */
    void Parse(const int argc, const char **argv);

    /**
     * @brief Return the value which was given to a command-line macro.
     *
     * @param macro The name of the macro.
     * @return The value of the macro.
     */
    cString get_command(const cString& macro) const;
    /**
     * @brief Check whether a macro appeared in the command line.
     *
     * @param macro The name of the macro.
     * @return Non-zero if the macro exists.
     */
    BOOL    check_macro(const cString& macro) const;

    /**
     * @brief Return the number of the string arguments.
     *
     * @return The number of the string arguments.
     */
    unsigned int get_string_count() const;
    /**
     * @brief Return a string argument.
     *
     * @param index The index of the string argument.
     * @return The string argument.
     */
    cString      get_string(unsigned int index) const;


    /**
     * @brief Parser helper function. Add a macro.
     *
     * @param macro The name of the macro.
     * @param description The value of the macro.
     */
    void add_macro (const cString &macro, const cString& description);
    /**
     * @brief Parser helper function. Add a string argument.
     *
     * @param string The string to add.
     */
    void add_string(const cString &string);


    /**
     * @brief Parser command. Called for each token.
     *
     * @param token_name The name of the token.
     */
    virtual void OnToken (cString& token_name);
    /**
     * @brief Parser command. Called for each macro.
     *
     * @param macro_name The name of the macro.
     * @param description The value of the macro.
     */
    virtual void OnMacro (cString& macro_name, cString &description);
    /**
     * @brief Parser command. Called for each string argument.
     *
     * @param string The string argument.
     */
    virtual void OnString(cString& string);

private:
    /**
     * @brief Disable copy things
     */
    cArguments(const cArguments& other);
    cArguments & operator = (const cArguments& other);

    cHash<cString,cString> argv_hash;
    cList<cString>         argv_str;
};

/**
 * @brief Default constructor. Init the class.
 */
cArguments::cArguments()
{
}

/**
 * @brief cArguments::cArguments(const int argc, const char **argv)
 *
 * @details Init the class, Parse the data
 *
 * @throws cException in case of an error
 */
cArguments::cArguments(const int argc, const char **argv)
{
    cArguments();
    Parse(argc, argv);
}

/**
 * @brief cArguments::~cArguments
 *
 * @details Init the class.
 */
cArguments::~cArguments()
{
}


/** @} */
#endif // __TBA_STL_ARGUMENTS_H
