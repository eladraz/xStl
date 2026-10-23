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

#ifndef __TBA_STL_OSEXCEPT_H
#define __TBA_STL_OSEXCEPT_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file osExcept.h
 * @brief The OS exception is a class derived from cException which is triggered
 *    in
 *   response to operating system API failure.
 *
 * @details The exception constructor has the information needed to tell the
 *    developer what happened.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/osdef.h"

/**
 * @brief Used in order to throw an exception which contains the operating
 *    system
 *   failure code.
 *
 * @details Here are a few examples:
 *
 * - xStl source code example. Taken from file.cpp:
 *
 * @code
 * if (CreateFile(...) == INVALID_HANDLE_VALUE)
 *    XSTL_THROW(cOsException());
 * @endcode
 *
 * OS: New OS exception thrown, code 0x2E, Access denied
 *
 * - XDK application:
 *
 * @code
 * NTSTATUS ret = IoCreateDevice(...);
 * if (!NT_SUCCESS(ret))
 *      XSTL_THROW(cOsException(ret));
 * @endcode
 */
class cOsException : public cException {
public:
    /**
     * @brief Default constructor. The ctor should be called in response to API
     *    call.
     *
     * @details The function invokes a call to the operating system and queries
     *    the error code ('GetLastError()' in Windows. 'errno' for all others)
     *
     * For operating systems whose last error code cannot be determined
     * this function constructs a 'Failure' code.
     * For these kinds of operating systems use the predetermined code ctor.
     *
     * @param file The source file where the exception is thrown.
     * @param line The line in the source file where the exception is thrown.
     */
    cOsException(char * file, uint32 line);

    /**
     * @brief Predetermined error code constructor.
     *
     * @param exceptionCode The code to be thrown
     * @param file The source file where the exception is thrown.
     * @param line The line in the source file where the exception is thrown.
     */
    cOsException(char * file, uint32 line, const cOSDef::exceptionCode& exceptionCode);

private:
    /**
     * @brief Called from the constructors.
     *
     * @details Inits the exception code for this class and the base class.
     */
    void init(const cOSDef::exceptionCode& exceptionCode);

    /**
     * @brief The OS exception code
     */
    cOSDef::exceptionCode m_exceptionCode;
};


/**
 * @brief Evaluates provided boolean expression and throws cOsException if it is
 *   false.
 *
 * @param n The boolean expression to evaluate.
 */
#define OS_CHECK(n) if (!(n)) { XSTL_THROW(cOsException); }


/**
 * @brief Same as OS_CHECK, but allows passing exception code to the constructor
 *    of
 *   cOsException.
 *
 * @param n The boolean expression to evaluate.
 * @param code The exception code passed to the cOsException constructor.
 */
#define OS_CHECK_WITH_CODE(n,code) if (!(n)) { XSTL_THROW(cOsException, code); }

/**
 * @brief Throws cOsException with provided exception code.
 *
 * @param code The exception code passed to the cOsException constructor.
 */
#define OS_CHECK_FAIL(code) XSTL_THROW(cOsException, code);


/** @} */
#endif // __TBA_STL_OSEXCEPT_H
