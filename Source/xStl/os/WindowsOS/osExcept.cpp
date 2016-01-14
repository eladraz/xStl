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

#include "xStl/xStlPrecompiled.h"
// For this files, the WIN32 or the _WIN32_WCE macro must be defined.
// Only Windows operating system or Windows CE can compile with this API
#if defined(WIN32) || defined(_WIN32_WCE)

/*
 * W I N D O W S implementation
 *
 * osExcept.cpp
 *
 * Implementation file for the Win32 API.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/except/exception.h"
#include "xStl/os/os.h"
#include "xStl/os/osdef.h"
#include "xStl/os/osExcept.h"
#include <windows.h>

cOsException::cOsException(char * file, uint32 line) :
    cException(file, line, EXCEPTION_OS_ERROR)
{
    init(GetLastError());
}

cOsException::cOsException(char * file, uint32 line, const cOSDef::exceptionCode& exceptionCode) :
    cException(file, line, EXCEPTION_OS_ERROR)
{
    init(exceptionCode);
}

void cOsException::init(const cOSDef::exceptionCode& exceptionCode)
{
    // Get code
    m_exceptionCode = exceptionCode;

    // Format message to ASCII-UNICODE string
    character* lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS, NULL, exceptionCode,
                  0, // Default language
                  (character*)&lpMsgBuf, 0, NULL);

    // Builds cString object
    cString ret(lpMsgBuf);

    // Free resources
    LocalFree(lpMsgBuf);

    ret+= XSTL_STRING(" (");
    ret+= HEXDWORD(exceptionCode);
    ret+= XSTL_STRING(")");

    changeMessageString(ret.getBuffer());
}

#endif // WIN32