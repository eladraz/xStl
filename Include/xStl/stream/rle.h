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

#ifndef __TBA_STL_STREAMS_RLE_H
#define __TBA_STL_STREAMS_RLE_H

/**
 * @addtogroup xstl_stream
 * @{
 */


/**
 * @file rle.h
 * @brief Define the cRLEcompression which implements the filterStream
 *    interface.
 *
 * @details The class provides simple run-length encoding.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"


/*
 * The default flag character to be used. According to statistics the character
 * '\253' is
 * not found in text files and rarely found in binary data.
 */
#ifndef RLE_FLAG_CHARACTER
/**
 * @brief The flag character which marks a run in the RLE compression. It can be
 *   overridden before including the header.
 */
#define RLE_FLAG_CHARACTER  (253)
#endif



/**
 * @brief Run-Length-Encoding. The filter-stream finds blocks of the same bytes
 *    and
 *   encodes them as one unit.
 *
 * @details The encoding is made by putting inside the string a flag. After the
 *    flag come the byte to be written and how many times it should repeat. In
 *    case the character itself should be encoded the compression writes out the
 *    flag twice.
 *
 * Encoding scheme:
 * \<FLAG\> \<LENGTH\> \<BYTE\>
 */
class cRLEcompression : public filterStream
{
public:
    /**
     * @brief Constructor. Create an RLE filter stream based on the 'stream'
     *    stream.
     *
     * @details The flag used in the compression is stored in the 'flag'
     *    variable.
     *
     * @param stream The decorated stream class to be used for reading/written.
     * @param flag The 8 bit flag for the RLE compression information. This
     *    character should be the least frequent character in the stream.
     */
    cRLEcompression(const cSmartPtr<basicIO>& stream,
                    BYTE flag = RLE_FLAG_CHARACTER);

    /**
     * @brief Virtual destructor. Flush the information.
     */
    virtual ~cRLEcompression();

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
     * @brief Length, getPointer are invalid in the RLE compression since I
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
    // Inline function
    /**
     * @brief Flush the compression for a single element This is where the real
     *   compression is accomplished.
     */
    void rleWriteFlush();

    /**
     * @brief Flush the information for reading operation Discrete reading.
     */
    void rleReadFlush();

    /**
     * @brief The flag for the compression
     */
    BYTE m_flag;

    /**
     * @brief Prevent copying objects
     */
    cRLEcompression();
    cRLEcompression(const cRLEcompression& other);
    cRLEcompression & operator = (const cRLEcompression& other);

    /**
     * @brief Compression variable for writing
     */
    BYTE         m_w_compression_byte;
    unsigned int m_w_count;

    /**
     * @brief Compression variable for reading
     */
    bool         m_r_is_compress;
    BYTE         m_r_compression_byte;
    unsigned int m_r_count;
};


/** @} */
#endif // __TBA_STL_STREAMS_RLE_H
