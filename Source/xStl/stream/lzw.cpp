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
 * lzw.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/assert.h"
#include "xStl/except/exception.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"
#include "xStl/stream/bitStream.h"
#include "xStl/stream/memoryStream.h"
#include "xStl/stream/lzw.h"


cLZWcompression::cLZWcompression(cSmartPtr<basicIO> stream,
                                 uint8 bitSize /*= DEFALUT_BIT_COUNT*/) :
    filterStream(stream),
    m_bit_stream(NULL),
    m_bitSize(bitSize),
    m_table((uint)0, 256)
{
    initSeek();

    /* Init the compression table */
    if (m_bitSize > 8)
        XSTL_THROW(cException, EXCEPTION_FORMAT_ERROR);

    m_bit_stream = new cBitStream(m_stream);

    /* The numbers between 0..2^bitSize-1 are refrence */
    /* to the real characters                           */

    /* The number 2^bitSize   is the clear signal   */
    /* The number 2^bitSize+1 is the End signal     */
    m_table.changeSize(0);

    start_opcodes = (1 << bitSize) + 2;
    m_first_write = TRUE;
    m_first_read  = TRUE;

    m_is_eof      = FALSE;

    m_io_bits = bitSize + 1;
}


/*
 * cLZWcompression destructor
 *
 * Flush the device (by calling flush() at close, if needed)
 */
cLZWcompression::~cLZWcompression()
{
    flush();
    delete m_bit_stream;
}

void cLZWcompression::initSeek()
{
    canLength = false;
    canSeekFromBegin = m_stream->canSeekFromBegin;
    canSeekForward   = true;
    canSeekBackward = false;
    canSeekFromEnd = false;
    canGetPointer = false;
}

// Throws exception
uint cLZWcompression::length() const
{
    ASSERT(false);
    return m_stream->length();
}

// Throws exception
uint cLZWcompression::getPointer() const
{
    ASSERT(false);
    return m_stream->getPointer();
}

void cLZWcompression::seek(const int distance, const basicInput::seekMethod method)
{
    if (!__canSeek(distance, method))
    {
        XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
    }

    /* Test whether the method is from the begining of the file */
    if (method == IO_SEEK_SET)
    {
        /* Start from begining of the file */
        cBuffer tempArray;

        flush();
        m_stream->seek(0, IO_SEEK_SET);  // Go to the begining of the file

        // Temporary reading of data
        if (basicInput::pipeRead(tempArray, distance) != (unsigned int)(distance))
        {
            XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
        }

    } else if ((method == IO_SEEK_CUR) && (distance >= 0))
    {
        cBuffer tempArray;

        flush();

        // Temporary reading of data

        if (basicInput::pipeRead(tempArray, distance) != (unsigned int)(distance))
        {
            XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
        }
    } else
    {
        ASSERT_MSG(FALSE, XSTL_STRING("Invalid seeking method reached"));
        XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
    }

    XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
}

bool cLZWcompression::isEOS()
{
    //return m_stream->eof();
    return m_is_eof;
}

void cLZWcompression::initNewStream()
{
    m_bit_stream->writeBits(1 << m_bitSize, (uint8)m_io_bits);

    /* Clear the stream */
    m_io_bits = m_bitSize + 1;
    m_first_write = TRUE;
    m_table.changeSize(0);
}

void cLZWcompression::closeNewStream()
{
    m_bit_stream->writeBits((1 << m_bitSize)+1, (uint8)m_io_bits);
}

uint cLZWcompression::write(const void *buffer, const uint length)
{
    if (length == 0)
        return 0;

    /* Prepare the reading from the buffer */
    cBuffer data((uint8 *)buffer, length);

    cMemoryStream mem(data, 0, TRUE);
    cSmartPtr<basicIO> stream(&mem, SMARTPTR_DESTRUCT_NONE);
    cBitStream    input_bitstream(stream);
    bool          found;

    unsigned int number_of_digits = (length * 8) / m_bitSize;
    unsigned int opcode;
    unsigned int pcode;

    /* Take care to the first read */
    /* The first opcode is a special case */
    if (m_first_write)
    {
        m_write_prefix = m_write_suffix = input_bitstream.readBits((uint8)m_bitSize);
        m_first_write = FALSE;
    }


    /* Write each uint8 */
    for (unsigned int i = 0; i < (number_of_digits - 1); i++)
    {
        /* If the pipe throw exception then we have a probleam */
        opcode = input_bitstream.readBits((uint8)m_bitSize);
        m_write_suffix = opcode;


        /* Test whether we have this kind of a string */
        found = FALSE;
        for (pcode = 0; pcode < m_table.getSize(); pcode++)
        {
            if ((m_table[pcode].prefix == m_write_prefix) && (m_table[pcode].value == m_write_suffix))
            {
                found = TRUE;
                break;
            }
        }

        /* Only if the string is not in the table */
        /* Write-out data and reset the string    */
        if (!found)
        {
            /* Write out the preifx */
            m_bit_stream->writeBits(m_write_prefix, (uint8)m_io_bits);

            /* Add the new string to the table */
            unsigned int lpos = m_table.getSize();
            m_table.changeSize(lpos + 1);

            if ((lpos + start_opcodes) == (unsigned int)(1 << m_io_bits))
                m_io_bits++;

            m_table[lpos].prefix = m_write_prefix;
            m_table[lpos].value  = m_write_suffix;

            m_write_prefix = m_write_suffix;
        } else
        {
            m_write_prefix = pcode + start_opcodes;
        }
    }
    return length;
}

void cLZWcompression::flush()
{
    if (!m_first_write)
    {
        m_bit_stream->writeBits(m_write_prefix, (uint8)m_io_bits);
        m_bit_stream->flush();
        m_first_write = true;
    }
    m_stream->flush();
}

bool cLZWcompression::writeString(unsigned int pcode, cBitStream & stream)
{
    if (pcode >= start_opcodes)
    {
        writeString(m_table[pcode - start_opcodes].prefix, stream);
        XSTL_TRY
        {
            stream.writeBits(m_table[pcode - start_opcodes].value,(uint8) m_bitSize);
        }
        XSTL_CATCH(cException e)
        {
            m_readed_codes.append(m_table[pcode - start_opcodes].value);
            return FALSE;
        }
    } else
    {
        XSTL_TRY
        {
            stream.writeBits(pcode, (uint8)m_bitSize);
        }
        XSTL_CATCH (cException e)
        {
            m_readed_codes.append(pcode);
            return FALSE;
        }
    }

    return TRUE;
}

unsigned int cLZWcompression::copyStream(cBitStream &bits_pipe, cMemoryStream &mem, void *buffer)
{
    unsigned int length = bits_pipe.getDownStream()->getPointer(); /* Its mem stream, and it's possiable */

    /* Copy the data */
    cOS::memcpy((uint8 *)buffer, mem.getStream()->getBuffer(), length);

    return length;
}

uint cLZWcompression::read(void *buffer, const uint length)
{
    if (length == 0)
        return 0;

    bool isTableHandled = false;
    cBuffer ldata(length);

    cMemoryStream mem(ldata, 0, TRUE);
    cSmartPtr<basicIO> stream(&mem, SMARTPTR_DESTRUCT_NONE);
    cBitStream bits_pipe(stream);


    bool          ret = FALSE;
    unsigned int  retv = 0;

    /* Write out all old-information */
    while (m_readed_codes.begin() != m_readed_codes.end())
    {
        unsigned int resolved = *(m_readed_codes.begin());
        XSTL_TRY
        {
            bits_pipe.writeBits(resolved, (uint8)m_bitSize);
        }
        XSTL_CATCH (cException e)
        {
            /* End of length */
            return copyStream(bits_pipe, mem, buffer);
        }
        cList<unsigned int>::iterator start = m_readed_codes.begin();
        m_readed_codes.remove(start);
    }

    /* EOF reached */
    if (m_is_eof)
    {
        return copyStream(bits_pipe, mem, buffer);
    }

    while (TRUE)
    {
        unsigned int next_entry;
        XSTL_TRY
        {
            next_entry = m_bit_stream->readBits((uint8)m_io_bits);
        }
        XSTL_CATCH (cException e)
        {
            if (e.getID() == EXCEPTION_READ_ERROR)
            {
                /* EOF reached */
                m_is_eof = true;
                return copyStream(bits_pipe, mem, buffer);
            } else
                XSTL_RETHROW;
        }
        isTableHandled = false;

        /* Test whether the entry is in the table */
        if (next_entry < start_opcodes)
        {
            if (next_entry == (unsigned int)(1 << m_bitSize))
            {
                /* This is the clear signal */

                /* Delete the table */
                m_table.changeSize(0);
                m_first_read  = TRUE;
                m_is_eof      = FALSE;
                m_io_bits = m_bitSize + 1;
                isTableHandled = TRUE;
            } else
            if (next_entry == (unsigned int)(1 << m_bitSize)+1)
            {
                /* This is the end signal */
                /* EOF reached */
                m_is_eof = TRUE;
                return copyStream(bits_pipe, mem, buffer);
            } else
            {
                /* Store the value of the bytes */
                if (!writeString(next_entry, bits_pipe))
                {
                    retv = copyStream(bits_pipe, mem, buffer);
                    ret  = TRUE;
                }
            }
        } else
        {
            /* Test whether the opcode is in the table */
            if (next_entry >= (start_opcodes + m_table.getSize()))
            {
                /* Add a new entry */
                m_read_suffix = next_entry;
                unsigned int lpos = m_table.getSize(), kpak;
                m_table.changeSize(lpos + 1);
                if ((lpos + start_opcodes + 1) == (unsigned int)(1 << m_io_bits))
                    m_io_bits++;
                m_table[lpos].prefix = m_read_prefix;

                kpak = m_read_suffix;
                while (kpak >= start_opcodes)
                    kpak = m_table[kpak - start_opcodes].prefix;

                m_table[lpos].value  = kpak;

                m_read_prefix = m_read_suffix;
                isTableHandled = TRUE;
            }

            /* The opcode is in the table */
            if (!writeString(next_entry, bits_pipe))
            {
                retv = copyStream(bits_pipe, mem, buffer);
                ret  = TRUE;
            }
        }

        /* Reconstruct the table */
        if (isTableHandled)
        {
            // No addition.
        } else
        if (m_first_read)
        {
            m_first_read = false;
            m_read_suffix = m_read_prefix = next_entry;
        } else
        {
            m_read_suffix = next_entry;

            /* Append the string as new entry */
            unsigned int lpos = m_table.getSize();
            m_table.changeSize(lpos + 1);

            if ((lpos + start_opcodes + 1) == (unsigned int)(1 << m_io_bits))
                m_io_bits++;

            m_table[lpos].prefix = m_read_prefix;

            /* Only the last opcode enters as "suffix" */
            unsigned int kpak = m_read_suffix;
            while (kpak >= start_opcodes)
                kpak = m_table[kpak - start_opcodes].prefix;
            m_table[lpos].value = kpak;
            m_read_prefix = m_read_suffix;
        }

        if (ret)
            return retv;
    }

    return 0;
}
