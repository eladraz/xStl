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

#ifndef __TBA_STL_STREAM_MEMORYACCESSERSTREAM_H
#define __TBA_STL_STREAM_MEMORYACCESSERSTREAM_H

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file memoryAccesserStream.h
 * @brief Wrap a 'cVirtualMemoryAccesserPtr' with a stream interface.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/os/virtualMemoryAccesser.h"
#include "xStl/stream/forkStream.h"

// Forward declaration
class cMemoryAccesserStream;
/**
 * @brief The memory-accesser stream reference counter object
 */
typedef cSmartPtr<cMemoryAccesserStream> cMemoryAccesserStreamPtr;

/**
 * @brief Wrap a 'cVirtualMemoryAccesserPtr' with a stream interface.
 *
 * @see cVirtualMemoryAccesserPtr
 * @see cForkStream
 */
class cMemoryAccesserStream : public cForkStream {
public:
    /**
     * @brief Constructor, creates a memory stream around a
     *    'virtual-memory-accesser'
     *   object.
     *
     * @param memory The virtual memory accesser for the stream
     * @param startAddress The start-address for the 'memory' accesser
     * @param endAddress The end-address for the 'memory' accesser. Also known
     *    as the size of the stream.
     * @param ignoreFragments When read() encounters a fragmented memory region
     *    it usually throws an exception. When set to true read() will ignore
     *    any fragmented memory and will fill them with 0.
     * @param isFix When the stream grew more than expected, the endAddress can
     *    be enlarged. TODO! THIS VARIABLE MUST BE TRUE. THE FUNCTION IS NOT
     *    IMPLEMENTED YET
     */
    cMemoryAccesserStream(const cVirtualMemoryAccesserPtr& memory,
                          uint startAddress,
                          uint endAddress,
                          bool ignoreFragments = false,
                          bool isFix = true);

    /**
     * @brief Memory-files will be treated as the default endian in the system
     */
    defaultEndianImpl;

    // Derived from the basicIO

    /**
     * @brief Retrieve the position of the write/read head
     *
     * @return The position of the read/write head, in bytes from the beginning
     *    of the stream.
     */
    virtual uint getPointer() const;

    /**
     * @brief Return the length of the memory-file.
     *
     * @return The length of the memory-file.
     */
    virtual uint length() const;

    /**
     * @brief Return true if the file pointer reach the end of the file.
     *
     * @return True if the file pointer reach the end of the file.
     */
    virtual bool isEOS();

    /**
     * @brief Flush the information of the file.
     *
     * @details Shouldn't do anything.
     */
    virtual void flush();

    /**
     * @brief Read from the memory array into a buffer.
     *
     * @details If the reading reaches the end of the array then the function
     *    returns the number of bytes read until EOF is reached.
     *
     * @note The fragments region will be filled with zero. There isn't any
     *    notification that the user can get using the streams interface.
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Write bytes to the stream.
     *
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void *buffer, const uint length);

    /**
     * @brief Move to a specific position in the stream.
     *
     * @details Throw an exception if failed. See basicInput::seek for more
     *    information.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @details This value stands at 4kb which is the pageSize in most
     *    platforms.
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return 4096; }

    /**
     * @brief The cForkStream API
     *
     * @return A new instance of the same stream.
     */
    virtual cForkStreamPtr fork() const;

    // cMemoryAccesserStream functions.

    /**
     * @brief Return pointer to the memory stream.
     *
     * @return Pointer to the memory stream.
     */
    bool isStreamWriteable() const;

    /**
     * @brief Returns the memory accesser object
     *
     * @return The memory accesser object.
     */
    const cVirtualMemoryAccesserPtr& getMemoryAccesser() const;

    /**
     * @brief Return the number of bytes relative to 'getMemoryAccesser' this
     *    stream
     *   access started.
     *
     * @return The number of bytes relative to 'getMemoryAccesser' this stream
     *    access started.
     */
    const uint getMemoryAccesserStartAddress() const;

    /**
     * @brief Return the number of bytes relative to 'getMemoryAccesser' this
     *    stream
     *   access ended.
     *
     * @return The number of bytes relative to 'getMemoryAccesser' this stream
     *    access ended.
     */
    const uint getMemoryAccesserEndAddress() const;

    /**
     * @brief Return a new memory stream forked from this stream which access
     *    only the
     *   relative 'start..end' portion of the stream
     *
     * @param virtualStart The starting address from 0 pointer of this stream
     * @param virtualEnd The ending address from 0 pointer of this stream in
     *    bytes
     * @return A new memory stream forked from this stream which access only the
     *    relative 'start..end' portion of the stream.
     */
    cMemoryAccesserStreamPtr forkRegion(uint virtualStart, uint virtualEnd) const;

protected:
    /**
     * @brief Init the seeking methods.
     *
     * @details All seek operations are permitted
     */
    virtual void initSeek();

private:
    /**
     * @brief Copy-constructor. Used in the fork API
     */
    cMemoryAccesserStream(const cMemoryAccesserStream& other);
    /**
     * @brief Deny operator =
     */
    cMemoryAccesserStream& operator = (const cMemoryAccesserStream& other);

    /**
     * @brief The memory which the data will be read from
     */
    cVirtualMemoryAccesserPtr m_data;
    /**
     * @brief The position inside the memory stream, the pointer is inside the
     *   [start..end] range in order to accelerated calculations.
     */
    uint m_filePosition;
    /**
     * @brief The relative address to access the stream
     */
    uint m_startAddress;
    /**
     * @brief The size of the stream (end-start)
     */
    uint m_endAddress;
    /**
     * @brief Should fragmented memory be ignored or should it be notified
     */
    bool m_ignoreFragments;
};


/** @} */
#endif // __TBA_STL_STREAM_MEMORYACCESSERSTREAM_H
