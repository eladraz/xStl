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
 * cachestream.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"
#include "xStl/stream/cacheStream.h"

cCacheStream::cCacheStream(cSmartPtr<basicIO> stream,
                           unsigned int readCache  /* = DEFAULT_READ_CACHING */,
                           unsigned int writeCache /* = DEFAULT_WRITE_CACHING*/) :
    filterStream(stream),
    m_readCache(readCache),
    m_writeCache(writeCache)
{
    initSeek();

    m_readPosition = m_readUse = 0;
    m_writePosition = m_writeUse = 0;
}

cCacheStream::~cCacheStream()
{
    flush();
}

void cCacheStream::flush()
{
    m_stream->pipeWrite(m_writeCache.getBuffer(), m_writePosition);

    m_writePosition = 0;
    m_readPosition = 0;
    m_readUse = 0;
    m_writeUse = 0;
    m_stream->flush();
}

uint cCacheStream::length() const
{
    return m_stream->length();
}

uint cCacheStream::getPointer() const
{
    return m_stream->getPointer();
}

void cCacheStream::seek(const int distance, const basicInput::seekMethod method)
{
    flush();
    m_stream->seek(distance, method);
}

bool cCacheStream::isEOS()
{
    return m_stream->isEOS();
}

uint cCacheStream::write(const void *buffer, const uint length)
{
    // Only if the buffer is full, empty it
    if (m_writePosition == m_writeCache.getSize())
    {
        m_stream->pipeWrite(m_writeCache.getBuffer(), m_writePosition);
        m_writePosition = 0;
        // Notice that this transaction should be append!
    }

    // Add the rest of the buffer
    unsigned int written = t_min(length, (uint)(m_writeCache.getSize() -
                                                m_writePosition));
    cOS::memcpy(m_writeCache.getBuffer() + m_writePosition, buffer, written);
    m_writePosition+= written;
    return written;
}

uint cCacheStream::read(void *buffer, const uint length)
{
    // Read more data
    if (m_readPosition == m_readUse)
    {
        // Try to fill up buffer
        m_readUse = m_stream->read(m_readCache.getBuffer(),
                                   m_readCache.getSize());
        m_readPosition = 0;
    }

    // Return back data
    unsigned int lread = t_min(length, (uint)(m_readUse - m_readPosition));
    cOS::memcpy(buffer, m_readCache.getBuffer() + m_readPosition, lread);

    m_readPosition+= lread;

    return lread;
}
