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

#ifndef __TBA_STL_EXCEPTION_H
#define __TBA_STL_EXCEPTION_H

/*
 * exception.h
 *
 * Define basic exception types and implement some of them.
 * This code is good for all operation system platfrom, both Windows and Linux
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/char.h"

/*
 * Here is a list of important exception and there ID.
 */
#define EXCEPTION_UNKWON           (0)
#define EXCEPTION_FAILED           (1)
#define EXCEPTION_OUT_OF_RANGE     (2)
#define EXCEPTION_OUT_OF_MEM       (3)
#define EXCEPTION_FORMAT_ERROR     (4)
#define EXCEPTION_READ_ERROR       (5)
#define EXCEPTION_WRITE_ERROR      (6)
#define EXCEPTION_SEEK_ERROR       (7)
#define EXCEPTION_VIOLATION        (8)
#define EXCEPTION_ASSERTION        (9)
#define EXCEPTION_ITERATOR_ERROR  (10)
#define EXCEPTION_OS_ERROR        (11)

// Extern definition
class cString;

/*
 * cException class
 *
 * Exception handling in the system. When XSTL or modules from it throws
 * exception, the exception is driven or cException exception. XSTL exception
 * contains error-code and a message describing it.
 * Use for exception identifier. Allow to add a single message to the exception
 * This class will be extended to combine severel of strings using cString, so
 * we can add the file name, line number and the function name.
 */
class cException
{
public:
    /*
     * Constructor, create a new exception with an error message.
     */
    cException(char * file, uint32 line, const uint32 msgID = 0);
    cException(char * file, uint32 line, const character* msg = NULL, uint32 msgID = 0);

    /*
     * Copy-constructor
     */
    cException(const cException& other);

    /*
     * Virtual deconstructor. Allow virtual inheritance.
     */
    virtual ~cException();

    /*
     * Return the error ID of the exception.
     */
    virtual uint32 getID();

    /*
     * Return string describing the exception.
     */
    virtual const character* getMessage();

    /*
     * Show the exception on the screen.
     *
     * For Windows application, using the "MessageBoxA" function
     * For non-windows application use "cout" command
     */
    virtual void print();

    /*
     * Clone the current exception. Please note, that you must free the returned pointer, so please pay extra care
     */
    virtual cException* clone() const;

    /*
     * Generate exception
     */
    void whereExceptionOccured(cString& ret);

protected:
    // Functions

    /*
     * Change the message to be pointed another string.
     *
     * msg - The new string that the exception contains.
     */
    void changeMessageString(const character* msg);

    // Members

    // The content of the error message. This cannot be string becuase the
    // cString object uses exceptions.
    character* m_message;
    // Should use operator delete over m_message
    bool m_shouldDelete;
    // The INDEX of the message
    uint32 m_messageNumber;

    char * m_atFile;
    uint32 m_atLine;

private:
    /*
     * Free the memory of the exception
     */
    void free();
};

#endif // __TBA_STL_EXCEPTION_H
