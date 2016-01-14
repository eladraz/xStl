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


/*
 * rle.h
 *
 * Define the cRLEcompression which implements the filterStream interface.
 * The class provides simple run-length encoding.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"


/*
 * The default flag character to be used. Over statics the character '\253' is
 * not shown in text files and few times in binary data.
 */
#ifndef RLE_FLAG_CHARACTER
#define RLE_FLAG_CHARACTER  (253)
#endif



/*
 * cRLEcompression
 *
 * Run-Length-Encoding. The filter-stream find out blocks of the same bytes
 * and encode them as one unit. The encoding is made by puting inside the
 * string a flag. After the flag there are the byte to be written and how
 * many times should it repeat. Incase the character itself should be encoded
 * the compression write out the flag twice.
 *
 * Encoding scheme:
 *  <FLAG> <LENGTH> <BYTE>
 */
class cRLEcompression : public filterStream
{
public:
    /*
     * Constructor. Create RLE filter stream base on the 'stream' stream. The
     * flag used in the compression is stored in the 'flag' variable.
     *
     * stream - The decorated stream class to be used for reading/written.
     * flag - The 8 bit flag for the RLE compression information. This
     *        character should be the most unshown character in the
     *        stream.
     */
    cRLEcompression(const cSmartPtr<basicIO>& stream,
                    BYTE flag = RLE_FLAG_CHARACTER);

    /*
     * Virtual destructor. Flush the information.
     */
    virtual ~cRLEcompression();

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
     * Length, getPointer are invalid in the RLE compression since I don't
     * have any way to know what is the size of the compressed data.
     */
    virtual uint length() const;
    virtual uint getPointer() const;

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return m_stream->getPipeReadBestRequest(); }

protected:
    // Init the seeking methods.
    virtual void initSeek();

private:
    // Inline function
    /*
     * Flush the compression for a single element
     * This is where the real compression is
     * accomplish at.
     */
    void rleWriteFlush();

    /*
     * Flush the information for reading operation
     * Descreate reading.
     */
    void rleReadFlush();

    /* The flag for the compression */
    BYTE m_flag;

    /* Prevent copying objects */
    cRLEcompression();
    cRLEcompression(const cRLEcompression& other);
    cRLEcompression & operator = (const cRLEcompression& other);

    /* Compression variable for writing */
    BYTE         m_w_compression_byte;
    unsigned int m_w_count;

    /* Compression variable for reading */
    bool         m_r_is_compress;
    BYTE         m_r_compression_byte;
    unsigned int m_r_count;
};

#endif // __TBA_STL_STREAMS_RLE_H
