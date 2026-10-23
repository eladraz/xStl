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

#ifndef __TBA_XSTL_PARSER_EXCEPT_H
#define __TBA_XSTL_PARSER_EXCEPT_H

/**
 * @addtogroup xstl_parser
 * @{
 */

/**
 * @file except.h
 * @brief The exception module for the IDL compiler.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/exceptions.h"

/**
 * @brief The exception which is thrown on parsing errors, with the file name
 *    and the line number.
 */
class ParserException : public cException
{
public:
    /**
     * @brief Constructor of the IDL exception.
     *
     * @param file The source file where the exception is thrown.
     * @param line The line in the source file where the exception is thrown.
     * @param filename The name of the file which is being parsed.
     * @param lineNumber The line number in the parsed file.
     * @param message The description of the parsing error.
     */
    ParserException(char * file, uint32 line, const cString& filename, uint lineNumber, const cString& message);

    /**
     * @brief Return the message which describes the exception
     *
     * @return The message which describes the exception.
     */
    virtual const character* getMessage();

private:
    /**
     * @brief The exception message
     */
    cString m_message;
};


/** @} */
#endif // __TBA_XSTL_PARSER_EXCEPT_H
