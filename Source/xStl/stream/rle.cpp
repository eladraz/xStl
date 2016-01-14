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
 * rle.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"
#include "xStl/stream/rle.h"


cRLEcompression::cRLEcompression(const cSmartPtr<basicIO>& stream,
                                 BYTE flag /* = RLE_FLAG_CHARACTER*/) :
    filterStream(stream),
    m_flag(flag)
{
    /* Init write operation */
    m_w_compression_byte = 0;
    m_w_count = 0;

    /* Init read operation */
    m_r_is_compress = false;
    m_r_count = 0;

    initSeek();
}

cRLEcompression::~cRLEcompression()
{
    flush();
}

void cRLEcompression::initSeek()
{
    canLength        = false;
    canSeekFromBegin = m_stream->canSeekFromBegin;
    canSeekForward   = true;
    canSeekBackward  = false;
    canSeekFromEnd   = false;
    canGetPointer    = false;
}

uint cRLEcompression::length() const
{
    ASSERT(false);
    return m_stream->length();
}

uint cRLEcompression::getPointer() const
{
    ASSERT(false);
    return m_stream->getPointer();
}

void cRLEcompression::flush()
{
    /* Pipe down flush() request */
    rleWriteFlush();
    m_stream->flush();
/*
 * NOTE
 * We don't need to call the rel_read_flush() becuase the user may be asking
 * for more byte later on, and the reading flush will ignore some bytes in
 * the file.
 */
}

void cRLEcompression::rleWriteFlush()
{
    if (m_w_count < 3)
    {
        if (m_w_compression_byte == m_flag)
        {
            /* Encode the special character  */
            /* Each character become 2 bytes */
            for (unsigned char i = 0; i < m_w_count; i++)
            {
                m_stream->pipeWrite(&m_flag, sizeof(BYTE));
                m_stream->pipeWrite(&m_flag, sizeof(BYTE));
            }
            m_w_count = 0;
        } else
        {
            /* Normal storage */
            for (unsigned char i = 0; i < m_w_count; i++)
            {
                m_stream->pipeWrite(&m_w_compression_byte, sizeof(BYTE));
            }
            m_w_count = 0;
        }
    } else
    {
        /* We have 3 or more character to store */
        /* Compression will do some compression */
        /*
         * Encode compression
         *
         * Write: <FLAG> <LENGTH> <BYTE>
         */
        m_stream->pipeWrite(&m_flag, sizeof(BYTE));

        /* Write the LENGTH (Which mustn't be equal)
         * to the FLAG character */
        if (m_w_count == m_flag)
        {
            /* In case it does equal    */
            /* Write less then the real */
            /* Compression              */
            unsigned char m_count_minus_1 = m_w_count - 1;
            m_stream->pipeWrite(&m_count_minus_1, sizeof(BYTE));
        } else
        {
            m_stream->pipeWrite(&m_w_count, sizeof(BYTE));
        }
        m_stream->pipeWrite(&m_w_compression_byte, sizeof(BYTE));

        if (m_w_count == m_flag)
        {
            /* Write down the left character */
            m_stream->pipeWrite(&m_w_compression_byte, sizeof(BYTE));
            if (m_w_compression_byte == m_flag)
            {
                m_stream->pipeWrite(&m_w_compression_byte, sizeof(BYTE));
            }
        }

        m_w_count = 0;
    }

}

uint cRLEcompression::write(const void *buffer, const uint length)
{
    BYTE         *data = (BYTE *) buffer;  /* information buffer   */
    unsigned int rLen  = length;           /* Return length, UGLY!  */

    while (rLen != 0)
    {
        /* Test wheter we can compress */
        if (*data == m_w_compression_byte)
        {
            /* Save the information */
            m_w_count++;
            if (m_w_count == 255)
            {
                /* Rich compression maximum */
                rleWriteFlush();
            }
        } else
        {
            /* Flush what we have until now */
            rleWriteFlush();

            /* Begin a new state for the compression */
            m_w_compression_byte = *data;
            m_w_count = 1;
        }

        /* We complete the mission for this byte */
        /* Getting the next byte */
        rLen--;
        data++;
    }

    /* Flush the last bytes */
    // rleWriteFlush();

    /* This function handle all the informaion*/
    return length;
}

void cRLEcompression::rleReadFlush()
{
    //m_r_is_compress = false;
}

uint cRLEcompression::read(void *buffer, const uint length)
{
    BYTE         *data = (BYTE *) buffer;  /* information buffer */
    unsigned int  len  = 0;                /* Return length,     */

    while (len != length)
    {
        /* Test wheter we are in compression section */
        if (m_r_is_compress)
        {
            /* Signal that we read a character */
            m_r_count--;
            if (m_r_count == 0)
            {
                /* Read all the information, move to the next byte */
                m_r_is_compress = false;
            }
            *data = m_r_compression_byte;
        } else
        {
            /* We don't have any memory of older compression */
            /* Read the next byte of the compression         */
            BYTE next_character;
            if (m_stream->pipeRead(&next_character, sizeof(BYTE)) != sizeof(BYTE))
            {
                // End of input data
                return len;
            }

            /* Test whether we are in new compression mode */
            if (next_character == m_flag)
            {
                /* Read the length */
                BYTE length;
                if (m_stream->pipeRead(&length, sizeof(BYTE)) != sizeof(BYTE))
                {
                    // Error on compression stream
                    XSTL_THROW(cException, XSTL_STRING("Compression error: end of pipe"));
                }

                /* Test whether this is a flag or not */
                if (length == m_flag)
                {
                    /* It's was only the flag */
                    *data = m_flag;
                } else
                {
                    /* Read the compression character */
                    if (m_stream->pipeRead(&m_r_compression_byte, sizeof(BYTE)) != sizeof(BYTE))
                    {
                        // Error on compression stream
                        XSTL_THROW(cException, XSTL_STRING("Compression error: end of pipe"));
                    }
                    *data = m_r_compression_byte;
                    m_r_count       = length - 1; /* We just read character */
                    m_r_is_compress = true;
                }
            } else
            {
                /* In this case just return the byte */
                *data = next_character;
            }
        }

        /* We complete the mission for this byte */
        /* Getting the next byte */
        len++;
        data++;
    }

    /* This function handle all the informaion*/
    return len;
}

void cRLEcompression::seek(const int distance, const basicInput::seekMethod method)
{
    if (!__canSeek(distance, method))
    {
        XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
    }

    /* Test whether the method is from the begining of the file */
    if (method == IO_SEEK_SET)
    {
        /* Start from begining of the file */
        cBuffer temp_array;

        flush();
        m_stream->seek(0, IO_SEEK_SET);  // Go to the begining of the file

        // Temporary reading of data
        if (basicInput::pipeRead(temp_array, distance) != (unsigned int)(distance))
        {
            XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
        }

    } else if ((method == IO_SEEK_CUR) && (distance >= 0))
    {
        cBuffer temp_array;

        flush();

        // Temporary reading of data
        if (basicInput::pipeRead(temp_array, distance) != (unsigned int)(distance))
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

bool cRLEcompression::isEOS()
{
    return ((!m_r_is_compress) && m_stream->isEOS());
}
