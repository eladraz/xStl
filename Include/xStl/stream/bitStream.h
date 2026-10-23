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

#ifndef __TBA_STL_STREAMS_BITSTREAM_H
#define __TBA_STL_STREAMS_BITSTREAM_H

/**
 * @addtogroup xstl_stream
 * @{
 */


/**
 * @file bitStream.h
 * @brief Basic bit stream access.
 *
 * @details Write/Read bits for a stream. This class should be used in a chain.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/endian.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"


/**
 * @brief Bit access to streams.
 *
 * @details Read 1/0 and write 1(non-zero)/0 as bytes. For example, writing the
 *    following array to the stream: [1,0,1,0,1,0,1,0] will be the same as
 *    writing '0xAA' into the decorated stream.
 *
 * There is no caching for this class. The write/read alignment is 1 byte per
 * operation. Use cCacheStream to enable alignment to the streams.
 */
class cBitStream : public filterStream
{
public:
    /**
     * @brief Constructor. Build a filter stream which translates bits in uint8
     *   representation into byte for the down stream.
     *
     * @param stream The decorated stream class to be used for reading/writing.
     */
    cBitStream(cSmartPtr<basicIO> stream);

    /**
     * @brief Virtual destructor.
     */
    virtual ~cBitStream();

    /**
     * @brief The endian for the bit stream is the same as the down stream.
     */
    filterStreamEndianImpl;

    // Derived from the basicIO

    /**
     * @brief Read 'length' bits from the decorated stream and put them in the
     *    buffer as
     *   0 and 1 only.
     *
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Write bits in BYTE alignment into the stream.
     *
     * @details For example: 1,0,1,0,1,1,1,1 will be packed into a single byte:
     *    0xF5.
     *
     * 'One' is non-zero number.
     *
     * W A R N I N G
     * This function cannot work with the read() instruction
     * at the same time.
     *
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void *buffer, const uint length);

    /**
     * @brief Return the number of bits in the stream.
     *
     * @details This function is valid only if the basic decorated stream
     *    supports the length command.
     *
     * @return The number of bits in the stream.
     */
    virtual uint length() const;

    /**
     * @brief Return the current position of BITS from the beginning of the
     *    stream.
     *
     * @details This function depends on the decorated class's 'getPointer'
     *    support
     *
     * @return The current position of BITS from the beginning of the stream.
     */
    virtual uint getPointer() const;

    /**
     * @brief Flush out all bits in memory and create a zero padding.
     *
     * @details Notice that the implementation cannot know what are the flushed
     *    padding bits
     */
    virtual void flush();

    /**
     * @brief Seek 'distance' bits from the 'method' position.
     *
     * @details See basicInput for more information. This method depends on the
     *    decorated class for seeking methods.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /**
     * @brief Return true if we have reached the end of the stream.
     *
     * @return True if we have reached the end of the stream.
     */
    virtual bool isEOS();

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return m_stream->getPipeReadBestRequest(); }


    // Several bit functions

    /**
     * @brief Read from the stream "numberOfBits" and convert it into a number
     *    limited
     *   to 32 bit.
     *
     * @param numberOfBits The number of bits to read, up to 32.
     * @return The number which was read.
     */
    uint32 readBits(BYTE numberOfBits);

    /**
     * @brief Write a number limited to 32-bit into the stream.
     *
     * @param number The number to write.
     * @param numberOfBits The number of bits of @a number to write, up to 32.
     */
    void writeBits(uint32 number, BYTE numberOfBits);

    /**
     * @brief The number of bits in a byte.
     */
    enum {
        BITS_IN_BYTE = 8    ///< The number of bits in a byte.
    };


protected:
    /**
     * @brief Init the seeking methods.
     */
    virtual void initSeek();

private:
    /**
     * @brief Init the variables for the bits reading without any internal
     *    cache.
     */
    void initVars();

    /**
     * @brief Data of the class
     */
    BYTE  m_read_bit_position;
    BYTE  m_write_bit_position;
    BYTE  m_write_byte;
    BYTE  m_read_byte;

    /**
     * @brief Prevent copying objects
     */
    cBitStream();
    cBitStream(const cBitStream& other);
    cBitStream & operator = (const cBitStream& other);
};


/** @} */
#endif //__TBA_STL_STREAMS_BITSTREAM_H

