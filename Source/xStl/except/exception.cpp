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

/*
 * exception.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/except/trace.h"

#include "xStl/stream/ioStream.h"

#if defined XSTL_WINDOWS
#include <windows.h>
#elif defined XSTL_LINUX
#elif defined XSTL_NTDDK
#include "xdk/kernel.h"
#elif defined(XSTL_16BIT) || defined (XSTL_CE)
#else
#error "Unsupoorted platform!";
#endif

const lpString EXCEPTION[] = {XSTL_STRING("Undefiend exception"),
    XSTL_STRING("Operation failed"),
    XSTL_STRING("Out of range"),
    XSTL_STRING("Not engouth memory"),
    XSTL_STRING("Format error"),
    XSTL_STRING("Error while trying to read from stream"),
    XSTL_STRING("Error while trying to write to stream"),
    XSTL_STRING("Seek error"),
    XSTL_STRING("Object access violated"),
    XSTL_STRING("Assertion failed"),
    XSTL_STRING("OS error"),
};

const lpString EXCEPTION_ID_INVALID = XSTL_STRING("The exception ID is invalid");


cException::cException(char * file, uint32 line, const character* msg, uint32 msgID):
    m_message(NULL),
    m_shouldDelete(false),
    m_messageNumber(msgID),
    m_atFile(file),
    m_atLine(line)
{
    if (NULL != msg) {
        changeMessageString(msg);
    }
}

cException::cException(char * file, uint32 line, const uint32 msgID) :
    m_message(NULL),
    m_shouldDelete(false),
    m_messageNumber(msgID),
    m_atFile(file),
    m_atLine(line)
{
    // Copy the exception string.
    if (msgID < (sizeof(EXCEPTION) / sizeof(lpString)))
    {
        m_message = EXCEPTION[msgID];
    } else
    {
        m_message = EXCEPTION_ID_INVALID;
    }
}

cException::cException(const cException& other) :
    m_message(NULL),
    m_shouldDelete(false),
    m_messageNumber(other.m_messageNumber),
    m_atFile(other.m_atFile),
    m_atLine(other.m_atLine)
{
    if (other.m_message != NULL)
    {
        if (other.m_shouldDelete)
            changeMessageString(other.m_message);
        else
            m_message = other.m_message;
    }
}

void cException::free()
{
    if (m_shouldDelete)
    {
        delete m_message;
    }
    m_message = NULL;
}

cException::~cException()
{
    free();
}

uint32 cException::getID()
{
    return m_messageNumber;
}

void cException::changeMessageString(const character* msg)
{
    ASSERT(msg != NULL);

    free();

    // Generate new memory block
    uint len = cChar::getStrlen(msg) + 1;
    m_message = new character[len];
    m_shouldDelete = true;
    cChar::strncpy(m_message, msg, len);
}

const character* cException::getMessage()
{
    return m_message;
}

void cException::whereExceptionOccured(cString& ret)
{
    // XXX: NOT PORTABLE!
    ret = m_atFile;
    ret.concat(":");
    ret+= cString(m_atLine, 10, 10);
}

void cException::print()
{
#ifdef DOT_NET
    // .NET application
  System::Console::WriteLine(getMessage());
#else
    // Normal Win32 application
    cString whereString;
    cString whatString = cString("Exception: message: ") << getMessage() << endl;
    whereExceptionOccured(whereString);
    whereString = cString("Exception: origin: ") << whereString << endl;

    TRACE(TRACE_VERY_HIGH, whatString);
    TRACE(TRACE_VERY_HIGH, whereString);

#ifndef XSTL_NTDDK
    cerr << whatString.getBuffer();
    cerr << whereString.getBuffer();
#endif

#if defined XSTL_WINDOWS
    //::MessageBox(NULL, getMessage(), XSTL_STRING("Exception"), MB_OK | MB_ICONSTOP);
#endif

#endif
}

cException* cException::clone() const
{
    return new cException(m_atFile, m_atLine, m_message, m_messageNumber);
}