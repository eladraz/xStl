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

#ifndef __TBA_STL_STREAMS_CACHESTREAM_H
#define __TBA_STL_STREAMS_CACHESTREAM_H


/*
 * cacheStream.h
 *
 * Defincation of the caching to streams, which provides read-ahead cache and
 * lazy-write chaches for streams. The cCacheStream is the cache interface for
 * both input and output streams.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"


/*
 * The default caching stream sizes. Define what is the default queues sizes
 * for the cCacheStream class.
 * 16kb queue size seems as the most logic value size it's small in memory and
 * saves 16*1024 calls to read() from the decorated stream.
 */
#ifndef DEFAULT_READ_CACHING
#define DEFAULT_READ_CACHING  (16 * 1024) // 16kb
#endif

#ifndef DEFAULT_WRITE_CACHING
#define DEFAULT_WRITE_CACHING (16 * 1024) // 16kb
#endif


/*
 * cCacheStream
 *
 * Enable caching to streams.
 */
class cCacheStream : public filterStream
{
public:
    /*
     * Constructor. Build up cache stream over other stream.
     *
     * stream - The decorated stream class to be used for caching.
     * readCache - The cache size of the reading queue.
     * writecache - The cache size of the writing queue.
     */
    cCacheStream(cSmartPtr<basicIO> stream,
                 unsigned int readCache = DEFAULT_READ_CACHING,
                 unsigned int writeCache = DEFAULT_WRITE_CACHING);
    /*
     * Virtual destructor. Free the queues.
     */
    virtual ~cCacheStream();

    /*
     * The endian for the bit stream is the same as the down stream.
     */
    filterStreamEndianImpl;


    // Driven from the basicIO

    /*
     * Read from the downstream 'length' amount of bytes. The function return
     * the bytes from read-ahead queue. In case the queue is empty the function
     * call to read() in order to fill it.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Write 'length' bytes from buffer into the lazy-write buffer. In case the
     * buffer was filled the function sends the block down to the stream
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Return the number of bytes in the stream. This function is valid only
     * if the basic decorated stream supports the length command.
     */
    virtual uint length() const;

    /*
     * Return the current position from the beginning of the stream.
     * This function depend on the decorated class 'getPointer' supports
     */
    virtual uint getPointer() const;

    /*
     * Flush out read-ahead cache and write to the storage all laze-write buffer
     */
    virtual void flush();

    /*
     * Seek inside the stream. See basicInput for more information.
     * This method depend on the decorate class for seeking methods.
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
    virtual uint getPipeReadBestRequest() const
                                  { return m_stream->getPipeReadBestRequest(); }

protected:
    // Init the seeking methods.
    virtual void initSeek()
    {
        canLength        = m_stream->canLength;
        canSeekFromBegin = m_stream->canSeekFromBegin;
        canSeekForward   = m_stream->canSeekForward;
        canSeekBackward  = m_stream->canSeekBackward;
        canSeekFromEnd   = m_stream->canSeekFromEnd;
        canGetPointer    = m_stream->canGetPointer;
    }


private:
    /* Prevent copying objects */
    cCacheStream();
    cCacheStream(const cCacheStream& other);
    cCacheStream & operator = (const cCacheStream& other);

    // The caching queues (read and write)
    cBuffer m_readCache;
    cBuffer m_writeCache;

    // The position inside the caching
    uint m_readPosition;
    uint m_writePosition;

    // How many bytes there are inside the caching queues.
    uint m_readUse;
    uint m_writeUse;
};


#endif // __TBA_STL_STREAMS_CACHESTREAM_H

