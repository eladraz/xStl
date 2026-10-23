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

/**
 * @addtogroup xstl_stream
 * @{
 */


/**
 * @file cacheStream.h
 * @brief Definition of caching for streams, which provides read-ahead cache and
 *   lazy-write caches for streams.
 *
 * @details The cCacheStream is the cache interface for both input and output
 *    streams.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"


/*
 * The default caching stream sizes. Define the default queue sizes
 * for the cCacheStream class.
 * A 16kb queue size seems to be the most logical value: it's small in memory
 * and
 * saves 16*1024 calls to read() from the decorated stream.
 */
#ifndef DEFAULT_READ_CACHING
#define DEFAULT_READ_CACHING  (16 * 1024) ///< 16kb
#endif

#ifndef DEFAULT_WRITE_CACHING
#define DEFAULT_WRITE_CACHING (16 * 1024) ///< 16kb
#endif


/**
 * @brief Enable caching to streams.
 */
class cCacheStream : public filterStream
{
public:
    /**
     * @brief Constructor. Build up cache stream over other stream.
     *
     * @param stream The decorated stream class to be used for caching.
     * @param readCache The cache size of the reading queue.
     * @param writeCache The cache size of the writing queue.
     */
    cCacheStream(cSmartPtr<basicIO> stream,
                 unsigned int readCache = DEFAULT_READ_CACHING,
                 unsigned int writeCache = DEFAULT_WRITE_CACHING);
    /**
     * @brief Virtual destructor. Free the queues.
     */
    virtual ~cCacheStream();

    /**
     * @brief The endian for the bit stream is the same as the down stream.
     */
    filterStreamEndianImpl;


    // Derived from the basicIO

    /**
     * @brief Read from the downstream 'length' amount of bytes.
     *
     * @details The function returns the bytes from the read-ahead queue. In
     *    case the queue is empty the function calls read() in order to fill it.
     *
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Write 'length' bytes from buffer into the lazy-write buffer.
     *
     * @details In case the buffer was filled the function sends the block down
     *    to the stream
     *
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void *buffer, const uint length);

    /**
     * @brief Return the number of bytes in the stream.
     *
     * @details This function is valid only if the basic decorated stream
     *    supports the length command.
     *
     * @return The number of bytes in the stream.
     */
    virtual uint length() const;

    /**
     * @brief Return the current position from the beginning of the stream.
     *
     * @details This function depend on the decorated class 'getPointer'
     *    supports
     *
     * @return The current position from the beginning of the stream.
     */
    virtual uint getPointer() const;

    /**
     * @brief Flush out read-ahead cache and write to the storage all lazy-write
     *    buffer
     */
    virtual void flush();

    /**
     * @brief Seek inside the stream.
     *
     * @details See basicInput for more information. This method depends on the
     *    decorated class for seeking methods.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /**
     * @brief Return true if we have reached to the end of the stream.
     *
     * @return True if we have reached to the end of the stream.
     */
    virtual bool isEOS();

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const
                                  { return m_stream->getPipeReadBestRequest(); }

protected:
    /**
     * @brief Init the seeking methods.
     */
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
    /**
     * @brief Prevent copying objects
     */
    cCacheStream();
    cCacheStream(const cCacheStream& other);
    cCacheStream & operator = (const cCacheStream& other);

    /**
     * @brief The caching queues (read and write)
     */
    cBuffer m_readCache;
    cBuffer m_writeCache;

    /**
     * @brief The position inside the caching
     */
    uint m_readPosition;
    uint m_writePosition;

    /**
     * @brief How many bytes there are inside the caching queues.
     */
    uint m_readUse;
    uint m_writeUse;
};



/** @} */
#endif // __TBA_STL_STREAMS_CACHESTREAM_H

