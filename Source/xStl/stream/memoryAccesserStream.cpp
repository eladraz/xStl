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
 * memoryAccesserStream.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/data/array.h"
#include "xStl/utils/algorithm.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/memoryAccesserStream.h"

cMemoryAccesserStream::cMemoryAccesserStream(
        const cVirtualMemoryAccesserPtr& memory,
        uint startAddress,
        uint endAddress,
        bool ignoreFragments,
        bool isFix) :
    m_data(memory),
    m_filePosition(startAddress),
    m_startAddress(startAddress),
    m_endAddress(endAddress),
    m_ignoreFragments(ignoreFragments)
{
    // TODO!
    CHECK(isFix);
    CHECK(m_endAddress >= m_startAddress);
    initSeek();
}

cMemoryAccesserStream::cMemoryAccesserStream(const cMemoryAccesserStream& other) :
    m_data(other.m_data),
    m_filePosition(other.m_filePosition),
    m_startAddress(other.m_startAddress),
    m_endAddress(other.m_endAddress)
{
    initSeek();
}

cForkStreamPtr cMemoryAccesserStream::fork() const
{
    // Simple as that
    return cForkStreamPtr(new cMemoryAccesserStream(*this));
}

void cMemoryAccesserStream::initSeek()
{
    canLength        = true;
    canSeekFromBegin = true;
    canSeekForward   = true;
    canSeekBackward  = true;
    canSeekFromEnd   = true;
    canGetPointer    = true;
}

uint cMemoryAccesserStream::getPointer() const
{
    return m_filePosition - m_startAddress;
}

uint cMemoryAccesserStream::length() const
{
    return m_endAddress - m_startAddress;
}

bool cMemoryAccesserStream::isEOS()
{
    return m_filePosition == m_endAddress;
}

void cMemoryAccesserStream::flush()
{
}

uint cMemoryAccesserStream::read(void *buffer, const uint length)
{
    if (length == 0)
        return 0;

    uint sread = t_min((m_endAddress - m_filePosition), length);

    // Test that we are not at EOS
    CHECK(sread != 0);

    // Zero out all fragment blocks
    if (m_ignoreFragments)
        memset(buffer, 0, length);

    // Read the memory
    bool success = m_data->memread(m_filePosition, buffer, sread, NULL);

    // Check the status
    if (!m_ignoreFragments)
    {
        //CHECK(success);
        if (!success) CHECK_FAIL();
    }

    m_filePosition+= sread;

    return sread;
}

uint cMemoryAccesserStream::write(const void *buffer, const uint length)
{
    CHECK(isStreamWriteable());

    if (length == 0)
        return 0;

    uint swrite = t_min((m_endAddress - m_filePosition), length);

    // Test that we are not at EOS
    CHECK(swrite != 0);

    // Safe write data
    if (!m_data->write(m_filePosition, buffer, swrite))
        CHECK_FAIL();

    m_filePosition+= swrite;
    return swrite;
}

void cMemoryAccesserStream::seek(const int distance,
                                 const basicInput::seekMethod method)
{
    if (!__canSeek(distance, method))
        XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);

    // Relative code, Reused from the cMemoryStream implementation
    m_filePosition-= m_startAddress;

    switch (method)
    {
    case IO_SEEK_SET:
        if (distance <= 0)
            m_filePosition = 0;
        else
            m_filePosition = t_min((uint)(distance), length());
        break;

    case IO_SEEK_CUR:
        if ((distance < 0) && ((uint)(-distance) > m_filePosition))
            m_filePosition = 0;
        else
        {
            m_filePosition+= distance;
            m_filePosition = t_min(m_filePosition, length());
        }
        break;

    case IO_SEEK_END: // distance is negative, see __canSeek.
        if ((distance < 0) && ((uint)(-distance) > length()))
            m_filePosition = 0;
        else
            m_filePosition = length() + distance;
        break;
    }

    // Relocate again
    m_filePosition+= m_startAddress;
}

bool cMemoryAccesserStream::isStreamWriteable() const
{
    return m_data->isWritableInterface();
}

const cVirtualMemoryAccesserPtr&
    cMemoryAccesserStream::getMemoryAccesser() const
{
    return m_data;
}

cMemoryAccesserStreamPtr cMemoryAccesserStream::forkRegion(uint virtualStart,
                                                           uint virtualEnd) const
{
    return cMemoryAccesserStreamPtr(new cMemoryAccesserStream(m_data,
        virtualStart + m_startAddress,
        virtualEnd + m_startAddress));
}

const uint cMemoryAccesserStream::getMemoryAccesserStartAddress() const
{
    return m_startAddress;
}

const uint cMemoryAccesserStream::getMemoryAccesserEndAddress() const
{
    return m_endAddress;
}
