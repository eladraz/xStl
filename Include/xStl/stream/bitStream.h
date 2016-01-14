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


/*
 * bitstream.h
 *
 * Basic bit stream access. Write/Read bits for a stream.
 * This class should use in a chain.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/endian.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"


/*
 * cBitStream
 *
 * Bit access to streams. Read 1/0 and write 1(non-zero)/0 as bytes. For example,
 * written to the stream the following array: [1,0,1,0,1,0,1,0] will be the same
 * as write '0xAA' into the decorated stream.
 *
 * There is no caching for this class. The write/read alignment is 1 byte per
 * operation. Use cCacheStream to enable alignment to the streams.
 */
class cBitStream : public filterStream
{
public:
    /*
     * Constructor. Build a filter stream which translate bits in uint8
     * represination into byte for the down stream.
     *
     * stream - The decorated stream class to be used for reading/written.
     */
    cBitStream(cSmartPtr<basicIO> stream);

    /*
     * Virtual destructor.
     */
    virtual ~cBitStream();

    /*
     * The endian for the bit stream is the same as the down stream.
     */
    filterStreamEndianImpl;

    // Driven from the basicIO

    /*
     * Read 'length' bits from the decorated stream and put them in the buffer
     * as 0 and 1 only.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Write bits in BYTE alignment into the stream.
     * For example: 1,0,1,0,1,1,1,1 will replace into a single byte: 0xF5.
     *
     * 'One' is non-zero number.
     *
     * W A R N I N G
     *   This function cannot work with the read() instruction
     *   in the same time.
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Return the number of bits in the stream. This function is valid only
     * if the basic decorated stream supports the length command.
     */
    virtual uint length() const;

    /*
     * Return the current position of BITS from the beginning of the stream.
     * This function depend on the decorated class 'getPointer' supports
     */
    virtual uint getPointer() const;

    /*
     * Flush out all bits in memory and create a zero padding. Notice that
     * the implementation cannot know what are the flushed padding bits
     */
    virtual void flush();

    /*
     * Seek 'distance' bits from the 'method' position. See basicInput for
     * more information. This method depend on the decorate class for seeking
     * methods.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /*
     * Return true if we have reached to the end of the stream.
     */
    virtual bool isEOS();

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return m_stream->getPipeReadBestRequest(); }


    // Several of bits functions

    /*
     * Read from the stream "numberOfBits" and convert it into a number
     * limited to 32 bit.
     */
    uint32 readBits(BYTE numberOfBits);

    /*
     * Write a number limited to 32-bit into the stream.
     */
    void writeBits(uint32 number, BYTE numberOfBits);

    // The number of bits in a byte.
    enum { BITS_IN_BYTE = 8 };


protected:
    // Init the seeking methods.
    virtual void initSeek();

private:
    /*
     * Init the variables for the bits reading without any internal cache.
     */
    void initVars();

    /* Data of the class */
    BYTE  m_read_bit_position;
    BYTE  m_write_bit_position;
    BYTE  m_write_byte;
    BYTE  m_read_byte;

    /* Prevent copying objects */
    cBitStream();
    cBitStream(const cBitStream& other);
    cBitStream & operator = (const cBitStream& other);
};

#endif //__TBA_STL_STREAMS_BITSTREAM_H

