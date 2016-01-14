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
 * bitstream.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/data/array.h"
#include "xStl/data/endian.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"
#include "xStl/stream/bitStream.h"

/*
 * cBitStream constructor
 *
 * Create a filter stream which translates bits into
 * the storage device (bytes).
 */
cBitStream::cBitStream(cSmartPtr<basicIO> stream) :
    filterStream(stream)
{
    initSeek();
    initVars();
}

void cBitStream::initVars()
{
    m_read_bit_position = 8;
    m_write_bit_position = 0;
}

void cBitStream::initSeek()
{
    canLength        = m_stream->canLength;
    canSeekFromBegin = m_stream->canSeekFromBegin;
    canSeekForward   = m_stream->canSeekForward;
    canSeekBackward  = m_stream->canSeekBackward;
    canSeekFromEnd   = m_stream->canSeekFromEnd;
    canGetPointer    = m_stream->canGetPointer;
}


cBitStream::~cBitStream()
{
    flush();
}

void cBitStream::flush()
{
    if (m_write_bit_position > 0)
    {
        m_stream->write(&m_write_byte, 1);
        m_write_bit_position = 0;
    }

    m_stream->flush();
}

uint cBitStream::length() const
{
    return (m_stream->length() * BITS_IN_BYTE);
}

uint cBitStream::getPointer() const
{
    return (m_stream->getPointer() * BITS_IN_BYTE);
}

void cBitStream::seek(const int distance, const basicInput::seekMethod method)
{
    if (!__canSeek(distance, method))
    {
        XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
    }

    // Flush the old write operation
    flush();
    int bytesSeek = distance / BITS_IN_BYTE;
    int bitsSeek = distance % BITS_IN_BYTE;

    // Seek to the number of bytes
    initVars();
    m_stream->seek(bytesSeek, method);
    readBits((uint8)bitsSeek);

    // TODO! Test this function realy carfully!!
    //       This function is NOT completed.
}

bool cBitStream::isEOS()
{
    return (m_stream->isEOS() && (m_read_bit_position == 8));
}

uint cBitStream::write(const void *buffer, const uint length)
{
    /* Write each bit sepreatly */
    for (unsigned int i = 0; i < length; i++)
    {
        /* Change the bit */
        BYTE bitmask  = 1 << m_write_bit_position;

        /* Reset or turn on the bit */
        if (((BYTE *)buffer)[i] != 0)
            m_write_byte|= 1 << m_write_bit_position;
        else
            m_write_byte&= (0xFF - bitmask);

        m_write_bit_position++;

        /* Test whether we have a byte 'ready' */
        if (m_write_bit_position == 8)
        {
            m_stream->pipeWrite(&m_write_byte, 1);
            m_write_bit_position = 0;
        }
    }
    return length;
}

uint cBitStream::read(void *buffer, const uint length)
{
    /* Reading loop */
    for (uint i = 0; i < length; i++)
    {
        if (m_read_bit_position == 8)
        {
            uint readed = m_stream->pipeRead(&m_read_byte, 1);

            /* EOS reached */
            if (readed == 0)
                return i;

            m_read_bit_position = 0;
        }

        /* Read a bit */
        ((BYTE *)buffer)[i] = (m_read_byte >> m_read_bit_position) & 1;
        m_read_bit_position++;
    }
    return length;
}

uint32 cBitStream::readBits(BYTE numberOfBits)
{
    ASSERT(numberOfBits <= 32);

    /* Read the bits */
    uint32 readed = 0;
    BYTE         bit;

    for (BYTE i = 0; i < numberOfBits; i++)
    {
        // Read a bit
        unsigned int lread = read(&bit, 1);
        if (lread != 1)
            XSTL_THROW(cException, EXCEPTION_READ_ERROR);

        readed|= bit << i;
    }

    return readed;
}

void cBitStream::writeBits(uint32 number, BYTE numberOfBits)
{
    if (numberOfBits > 32)
        XSTL_THROW(cException, EXCEPTION_FORMAT_ERROR);


    /* Write the bits */
    for (BYTE i = 0; i < numberOfBits; i++)
    {
        BYTE data = (BYTE)(number & 1);
        unsigned int written = write(&data, 1);
        if (written != 1)
            XSTL_THROW(cException, EXCEPTION_WRITE_ERROR);

        number>>= 1;
    }
}

