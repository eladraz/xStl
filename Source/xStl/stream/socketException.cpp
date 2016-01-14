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
/*
 * socket_exception.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/except/assert.h"
#include "xStl/except/exception.h"
#include "xStl/stream/socketException.h"


const lpString SOCKET_EXCEPTION[] = {
        XSTL_STRING("Socket creation faild"),
        XSTL_STRING("Write timeout achived"),
        XSTL_STRING("Read timeout achived"),
        XSTL_STRING("Write to socket faild"),
        XSTL_STRING("Reading from the socket faild"),
        XSTL_STRING("The socket hasn't been created"),
        XSTL_STRING("Error while parsing the address"),
        XSTL_STRING("Connection faild"),
        XSTL_STRING("Bind to address faild"),
        XSTL_STRING("Listen for incomming connection faild"),
        XSTL_STRING("Accepting a new connection faild")
};


const lpString SOCKET_UNKNOWN_EXCEPTION = XSTL_STRING("Socket unknown error");


cSocketException::cSocketException(char * file, uint32 line, const uint32 msgId) :
    cException(file, line, msgId)
{
    // Change the exception ID
    m_messageNumber = msgId;

    // Fixs the exception string
    if (msgId >= SOCKET_EXCEPTION_BASE)
    {
        if ((msgId - SOCKET_EXCEPTION_BASE) <
            (sizeof(SOCKET_EXCEPTION) / sizeof(lpString)))
        {
            character* error = SOCKET_EXCEPTION[(uint)(msgId - SOCKET_EXCEPTION_BASE)];
            m_message = error;

        } else
        {
            m_message = SOCKET_UNKNOWN_EXCEPTION;
        }
    } else
    {
        m_message = SOCKET_UNKNOWN_EXCEPTION;
    }
}
