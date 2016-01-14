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
 * memorystream.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/array.h"
#include "xStl/utils/algorithm.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/memoryStream.h"

cMemoryStream::cMemoryStream(uint pageSize  /* = DEFAULT_STREAM_PAGE_SIZE*/,
                             uint initSize  /* = 0 */,
                             bool fixedSize /* = false */) :
    m_fixedSize(fixedSize),
    m_filePosition(0),
    m_data(new cBuffer(initSize, pageSize))
{
    initSeek();
}

cMemoryStream::cMemoryStream(const cBuffer& initData,
                             uint pageSize    /* = DEFAULT_STREAM_PAGE_SIZE */,
                             bool fixedSize   /* = false */) :
    m_fixedSize(fixedSize),
    m_filePosition(0),
    m_data(new cBuffer(initData.getBuffer(), initData.getSize(), pageSize))
{
    initSeek();
}

cMemoryStream::cMemoryStream(cBufferPtr& realData,
                             uint pageSize    /* = DEFAULT_STREAM_PAGE_SIZE */,
                             bool fixedSize   /* = false */) :
    m_fixedSize(fixedSize),
    m_filePosition(0),
    m_data(realData)
{
    m_data->setPageSize(pageSize);
    initSeek();
}

void cMemoryStream::initSeek()
{
    canLength        = true;
    canSeekFromBegin = true;
    canSeekForward   = true;
    canSeekBackward  = true;
    canSeekFromEnd   = true;
    canGetPointer    = true;
}

uint cMemoryStream::getPointer() const
{
    return m_filePosition;
}

uint cMemoryStream::length() const
{
    return m_data->getSize();
}

bool cMemoryStream::isEOS()
{
    return m_filePosition == m_data->getSize();
}

void cMemoryStream::flush()
{
}

uint cMemoryStream::read(void *buffer, const uint length)
{
    uint sread = t_min(m_data->getSize() - m_filePosition, length);

    cOS::memcpy(buffer, m_data->getBuffer() + m_filePosition, sread);
    m_filePosition+= sread;

    return sread;
}

uint cMemoryStream::write(const void *buffer, const uint length)
{
    uint swrite = t_min(m_data->getSize() - m_filePosition, length);

    /* Normal copy of the data */
    cOS::memcpy(m_data->getBuffer() + m_filePosition, buffer, swrite);
    m_filePosition+= swrite;

    if ((swrite != length) && (m_fixedSize == FALSE))
    {
        /* Increase the array and write the left bytes */
        uint lsize = length - swrite;
        m_data->changeSize(m_data->getSize() + lsize);

        cOS::memcpy(m_data->getBuffer() + m_filePosition, (BYTE *)(buffer) + swrite, lsize);
        m_filePosition+= lsize;

        swrite = length;
    }

    return swrite;
}

void cMemoryStream::seek(const int distance, const basicInput::seekMethod method)
{
    if (!__canSeek(distance, method))
    {
        XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
    }

    switch (method)
    {
    case IO_SEEK_SET:
        if (distance <= 0)
            m_filePosition = 0;
        else
            m_filePosition = t_min((uint)(distance), m_data->getSize());
        break;

    case IO_SEEK_CUR:
        if ((distance < 0) && ((uint)(-distance) > m_filePosition))
            m_filePosition = 0;
        else
        {
            m_filePosition+= distance;
            m_filePosition = t_min(m_filePosition, m_data->getSize());
        }
        break;

    case IO_SEEK_END: // distance is negative, see __canSeek.
        if ((distance < 0) && ((uint)(-distance) > m_data->getSize()))
            m_filePosition = 0;
        else
            m_filePosition = m_data->getSize() + distance;
        break;
    }
}

cMemoryStream::cMemoryStream(const cMemoryStream& other) :
    m_fixedSize(other.m_fixedSize),
    m_filePosition(other.m_filePosition),
    m_data(new cBuffer(*other.m_data))
{
    initSeek();
}

cForkStreamPtr cMemoryStream::fork() const
{
    // Simple as that
    return cForkStreamPtr(new cMemoryStream(*this));
}
