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

/*
 * osExcept.h
 *
 * The OS exception is a class derived from cException which is triggered in
 * response to operating system API failure. The exception constructor
 * has the information needed to tell the developer what happened.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/osdef.h"

/*
 * Used in order to throw an exception which contains the operating system failure
 * code.
 *
 * Here are a few examples:
 *
 * - xStl source code example. Taken from file.cpp:
 *      if (CreateFile(...) == INVALID_HANDLE_VALUE)
 *         XSTL_THROW(cOsException());
 *
 *   OS: New OS exception thrown, code 0x2E, Access denied
 *
 * - XDK application:
 *     NTSTATUS ret = IoCreateDevice(...);
 *     if (!NT_SUCCESS(ret))
 *          XSTL_THROW(cOsException(ret));
 */
class cOsException : public cException {
public:
    /*
     * Default constructor. The ctor should be called in response to API
     * call. The function invokes a call to the operating system and queries
     * the error code ('GetLastError()' in Windows. 'errno' for all others)
     *
     * For operating systems whose last error code cannot be determined
     * this function constructs a 'Failure' code.
     * For these kinds of operating systems use the predetermined code ctor.
     */
    cOsException(char * file, uint32 line);

    /*
     * Predetermined error code constructor.
     *
     * exceptionCode - The code to be thrown
     */
    cOsException(char * file, uint32 line, const cOSDef::exceptionCode& exceptionCode);

private:
    /*
     * Called from the constructors.
     * Inits the exception code for this class and the base class.
     */
    void init(const cOSDef::exceptionCode& exceptionCode);

    // The OS exception code
    cOSDef::exceptionCode m_exceptionCode;
};


/*
 * Evaluates provided boolean expression and throws cOsException if it is
 * false.
 */
#define OS_CHECK(n) if (!(n)) { XSTL_THROW(cOsException); }


/*
 * Same as OS_CHECK, but allows passing exception code to the constructor
 * of cOsException.
 */
#define OS_CHECK_WITH_CODE(n,code) if (!(n)) { XSTL_THROW(cOsException, code); }

/*
 * Throws cOsException with provided exception code.
 */
#define OS_CHECK_FAIL(code) XSTL_THROW(cOsException, code);

#endif // __TBA_STL_OSEXCEPT_H
