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
 * stringerStream.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/lock.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/exception.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/stringerStream.h"

cStringerStream::cStringerStream(bool shouldWaitToNewLine) :
    m_lineRepository((character*)NULL, SCREEN_STRING_OPTIMIZER),
    m_shouldWaitToNewLine(shouldWaitToNewLine),
    m_base(DEFAULT_BASE)
{
}

uint cStringerStream::write(const void* buffer, const uint length)
{
    // TODO! Add uint16 dump and uint32 dump...
    uint8* buf = (uint8*) buffer;

    // Due to EVC compiler and gcc-4.0 compiler, moving the DATA8 as argument causes the compiler to mis-match the template
    // ;(
    DATA8 dump(buf, buf + length);
    hexDumpObject(*this, dump);

    return length;
}

bool cStringerStream::flushBuffer(cString& ret)
{
    cLock lock(m_lock);
    ret = m_lineRepository;
    m_lineRepository = cString();
    return ret.length() > 0;
}

bool cStringerStream::flushEndlBuffer(cString& ret)
{
    cLock lock(m_lock);
    const cString els(endl); // was static, remove due to atexit() dependencies...
    uint index = m_lineRepository.find(els);
    if (index == m_lineRepository.length())
        return false;

    uint llen = cChar::getStrlen(endl);
    // A new line was detected, from 0 until index.
    ret = m_lineRepository.left(index + llen);
    // Delete the outputed line.
    m_lineRepository = m_lineRepository.right(m_lineRepository.length() -
            index -
            llen);
    return true;
}

void cStringerStream::flush()
{
    cString out;
    if (flushBuffer(out))
        outputString(out);
}

cStringerStream& cStringerStream::setBase(uint newBase)
{
    m_base = newBase;
    return *this;
}

uint cStringerStream::getBase() const
{
    return m_base;
}

cStringerStream& cStringerStream::operator << (const cBuffer& rawData)
{
    write(rawData.getBuffer(), rawData.getSize());
    return *this;
}

void cStringerStream::flushNewLines()
{
    cString out;
    while (flushEndlBuffer(out))
        outputString(out);
}

void cStringerStream::addToRepository(const cString& data)
{
    cLock lock(m_lock);
    m_lineRepository+= data;
}

cStringerStream& cStringerStream::operator << (const cString& string)
{
    if (m_shouldWaitToNewLine)
    {
        addToRepository(string);
        flushNewLines();
        return *this;
    }

    outputString(string);
    return *this;
}

cStringerStream& cStringerStream::operator << (const char ch)
{
    return *this << cString((character)ch);
}

#ifdef XSTL_UNICODE
cStringerStream& cStringerStream::operator << (const character ch)
{
    return *this << cString(ch);
}
#endif

/*
cStringerStream& cStringerStream::operator << (const int16 word)
{
    return *this << cString((const int32)word, m_base);
}
*/

cStringerStream& cStringerStream::operator << (const int32 dword)
{
    return *this << cString(dword, m_base);
}

cStringerStream& cStringerStream::operator << (const int64 dword)
{
    return *this << cString(dword, m_base);
}

cStringerStream& cStringerStream::operator << (const uint8 byte)
{
    return *this << cString((const uint32)byte, m_base);
}

cStringerStream& cStringerStream::operator << (const uint16 word)
{
    #ifdef XSTL_UNICODE
        // BUG: If (word=0), and XSTL_UNICODE is defined, it will be replaced
        // to nul character.
        //
        // Why use XSTL_UNICODE when printing uint16 ?
        // Guy

        // character
        return *this << cString((character)word);
    #else
        // number
        return *this << cString((const uint32)word, m_base);
    #endif
}

cStringerStream& cStringerStream::operator << (const uint32 dword)
{
    return *this << cString(dword, m_base);
}

cStringerStream& cStringerStream::operator << (const uint64 dword)
{
    return *this << cString(dword, m_base);
}

cStringerStream& cStringerStream::operator << (const char* asciiString)
{
    return *this << cString(asciiString);
}

#ifdef XSTL_UNICODE
cStringerStream& cStringerStream::operator << (const character* unicodeString)
{
    return *this << cString(unicodeString);
}
#endif // XSTL_UNICODE

// The new line set of characters
#ifdef DOUBLE_NEWLINE
    character* endl = XSTL_STRING("\r\n");
#else
    character* endl = XSTL_STRING("\n");
#endif


cStringStream::cStringStream(uint optimization/* = SCREEN_STRING_OPTIMIZER*/) :
    cStringerStream(false),
    m_data((character*)NULL, optimization)
{
}

const cString& cStringStream::getData() const
{
    return m_data;
}

void cStringStream::clean()
{
    m_data = cString();
}

void cStringStream::outputString(const cString& string)
{
    m_data+= string;
}
