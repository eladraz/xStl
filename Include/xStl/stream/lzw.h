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

#ifndef __TBA_STL_LZWCOMPRESSION_H
#define __TBA_STL_LZWCOMPRESSION_H

/**
 * @addtogroup xstl_stream
 * @{
 */


/**
 * @file lzw.h
 * @brief Implementation of the Lempel-Ziv-Welch compression as defined in the
 *    GIF
 *   image specification.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/sarray.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"
#include "xStl/stream/bitStream.h"
#include "xStl/stream/memoryStream.h"


/**
 * @brief The default bit amount for the compression.
 */
#define DEFAULT_BIT_COUNT (8)


/**
 * @brief An entry of the LZW compression table.
 *
 * @details Data struct of the compression table in the memory.
 */
struct LZW_STRING
{
    /**
     * @brief The index of the prefix string in the compression table.
     */
    unsigned int prefix;
    /**
     * @brief The value (code) which is appended to the prefix.
     */
    unsigned int value;
};



/**
 * @brief The device can encode stream with bit size of 1,2,3,4,5,6,7,8 bits
 *    only.
 *
 * @details If you want to compress 9bit of information, I personally think that
 *    you are
 *   wasting your time since the pattern which will be found and the pattern of
 *   the memory will enlarge the file.
 *
 * The method of the compression is to build a run-time table containing
 * strings of previous strings found in the string. Instead of coding the
 * string itself, the class writes out the index inside the table. The table
 * size is dynamic and increases during run-time.
 * For more information see GIF compression.
 */
class cLZWcompression : public filterStream
{
public:

    /**
     * @brief Constructor. Create LZW stream with default bit-encoding of
     *    'bitSize'
     *   parameter.
     *
     * @param stream The decorated stream class to be used for the compression
     *    or the decompression process.
     * @param bitSize Number of bits in which to seek patterns.
     */
    cLZWcompression(cSmartPtr<basicIO> stream,
                    uint8 bitSize = DEFAULT_BIT_COUNT);
    /**
     * @brief Virtual destructor
     */
    virtual ~cLZWcompression();

    /**
     * @brief The endian for the bit stream is the same as the down stream.
     */
    filterStreamEndianImpl;


    // Derived from the basicIO

    /**
     * @brief Decompress data from the lower stream.
     *
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Encoding the buffer into the compressor.
     *
     * @details W A R N I N G
     *   This function cannot work with the read() instruction
     *   in the same time.
     *
     * Throws "EXCEPTION_WRITE_EXCEPTION" exception.
     *
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void *buffer, const uint length);

    /**
     * @brief Flush out read buffer and write-buffers.
     *
     * @details Init the compression to new stream.
     */
    virtual void flush();

    /**
     * @brief Can seek forward 'distance' amount of bytes.
     *
     * @details All other seeking methods
     *
     * @throws cException . If the down stream can seek to the beginning this
     *    function will be valid from the beginning of the stream.
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /**
     * @brief Return true if we have reached the end of the compression.
     *
     * @return True if we have reached the end of the compression.
     */
    virtual bool isEOS();

    /**
     * @brief Length, getPointer are invalid in the LZW compression since I
     *    don't have
     *   any way to know the size of the compressed data.
     *
     * @return The total length of the stream in bytes.
     */
    virtual uint length() const;
    /**
     * @return The position of the read/write head, in bytes from the beginning
     *    of the stream.
     */
    virtual uint getPointer() const;


    // LZW functions

    /**
     * @brief Call this function at the beginning of the write-out-compression
     *    process.
     *
     * @details The function writes out the 'CLEAR' signal to the stream. The
     *    decompressor expects to find this signal.
     */
    void initNewStream();

    /**
     * @brief Call this function at the end of the compression.
     *
     * @details Write the 'EOI' signal. After the call to this function flush
     *    the data and close all streams.
     */
    void closeNewStream();

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return m_stream->getPipeReadBestRequest(); }

protected:
    /**
     * @brief Init the seeking methods.
     */
    virtual void initSeek();

private:
    /**
     * @brief Prevent copying objects
     */
    cLZWcompression();
    cLZWcompression(const cLZWcompression& other);
    cLZWcompression & operator = (const cLZWcompression& other);


    cBitStream          *m_bit_stream; /* Bit access to the m_stream       */
    unsigned int         m_bitSize;    /* Number of bit in the compression */
    cSArray<LZW_STRING>  m_table;      /* The LZW table                    */
    cList<unsigned int>  m_read_codes;              /* Saved information */

    unsigned int start_opcodes;
    unsigned int m_write_prefix;
    unsigned int m_write_suffix;
    bool         m_first_write;
    bool         m_first_read;
    bool         m_is_eof;

    unsigned int m_io_bits;

    unsigned int m_read_prefix;
    unsigned int m_read_suffix;

    /* String operation */

    /**
     * @brief Write a single string into the pipe.
     *
     * @details Uses recursion to retrieve the string from the 'pcode'.
     */
    bool writeString(unsigned int pcode, cBitStream & stream);

    /**
     * @brief Copy data directly to the memory stream from the bitstream.
     */
    unsigned int copyStream(cBitStream &bits_pipe, cMemoryStream &mem, void *buffer);
};



/** @} */
#endif //__TBA_STL_LZWCOMPRESSION_H

