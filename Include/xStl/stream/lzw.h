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


/*
 * LZW.h
 *
 * Implementation of the Lempel-Ziv-Welch compression as defines in the GIF
 * image specification.
 *
 * Author: Elad Raz <e@eladraz.com>
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


/*
 * The default bit amount for the compression.
 */
#define DEFALUT_BIT_COUNT (8)


/*
 * struct LZW_STRING
 *
 * Data struct of the compression table in the memory.
 */
struct LZW_STRING
{
    unsigned int prefix;
    unsigned int value;
};



/*
 * cLZWcompression
 *
 * The device can encode stream with bit size of 1,2,3,4,5,6,7,8 bits only.
 *
 * If you want to compress 9bit of information, I presonly think that you are
 * wasting your time since the pattern which will be found and the pattern of
 * the memory will enlarge the file.
 *
 * The method of the compression is to build over run-time table containing
 * string of previous strings found out on the string. Instead of coding the
 * string itself, the class write out the index inside the table. The table
 * size is dynamic and increase during run-time.
 * For more information see GIF compression.
 */
class cLZWcompression : public filterStream
{
public:

    /*
     * Constructor. Create LZW stream with default bit-encoding of
     * 'bitSize' parameter.
     *
     * stream - The decorated stream class to be used for the compression or
     *          the decompression process.
     * bitSize - Number of bits to seek patterns over them.
     */
    cLZWcompression(cSmartPtr<basicIO> stream,
                    uint8 bitSize = DEFALUT_BIT_COUNT);
    /*
     * Virtual destructor
     */
    virtual ~cLZWcompression();

    /*
     * The endian for the bit stream is the same as the down stream.
     */
    filterStreamEndianImpl;


    // Driven from the basicIO

    /*
     * Decompress data from the lower stream.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Encoding the buffer into the compressor.
     *
     * W A R N I N G
     *   This function cannot work with the read() instruction
     *   in the same time.
     *
     * Throws "EXCEPTION_WRITE_EXCEPTION" exception.
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Flush out read buffer and write-buffers. Init the compression to
     * new stream.
     */
    virtual void flush();

    /*
     * Can seek forward 'distance' amount of bytes. All other seeking methods
     * throws exception.
     * If the down stream can seek to the beginng this function will be valid
     * from the beginning of the stream.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /*
     * Return true if we have reached to the end of the compression.
     */
    virtual bool isEOS();

    /*
     * Length, getPointer are invalid in the LZW compression since I don't
     * have any way to know what is the size of the compressed data.
     */
    virtual uint length() const;
    virtual uint getPointer() const;


    // LZW functions

    /*
     * Call this function at the beginng of the write-out-compression process.
     * The function write out the 'CLEAR' signal to the stream.
     * The decompressor is excepting to find this signal.
     */
    void initNewStream();

    /*
     * Call this function at the end of the compression.
     * Write the 'EOI' signal. After the call to this function flush the data
     * and close all streams.
     */
    void closeNewStream();

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return m_stream->getPipeReadBestRequest(); }

protected:
    // Init the seeking methods.
    virtual void initSeek();

private:
    /* Prevent copying objects */
    cLZWcompression();
    cLZWcompression(const cLZWcompression& other);
    cLZWcompression & operator = (const cLZWcompression& other);


    cBitStream          *m_bit_stream; /* Bit access to the m_stream       */
    unsigned int         m_bitSize;    /* Number of bit in the compression */
    cSArray<LZW_STRING>  m_table;      /* The LZW table                    */
    cList<unsigned int>  m_readed_codes;              /* Saved information */

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

    /*
     * Write a single string into the pipe.
     * Uses recursine to retrieve the string from the 'pcode'.
     */
    bool writeString(unsigned int pcode, cBitStream & stream);

    /*
     * Copy data directly to the memory stream from the bitstream.
     */
    unsigned int copyStream(cBitStream &bits_pipe, cMemoryStream &mem, void *buffer);
};


#endif //__TBA_STL_LZWCOMPRESSION_H

