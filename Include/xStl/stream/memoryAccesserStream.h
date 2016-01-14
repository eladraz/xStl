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

/*
 * memoryAccesserStream.h
 *
 * Wrap a 'cVirtualMemoryAccesserPtr' with a stream interface.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/os/virtualMemoryAccesser.h"
#include "xStl/stream/forkStream.h"

// Forward decelration
class cMemoryAccesserStream;
// The memory-accesser stream reference counter object
typedef cSmartPtr<cMemoryAccesserStream> cMemoryAccesserStreamPtr;

/*
 * Wrap a 'cVirtualMemoryAccesserPtr' with a stream interface.
 * See cVirtualMemoryAccesserPtr
 * See cForkStream
 */
class cMemoryAccesserStream : public cForkStream {
public:
    /*
     * Constructor, creates a memory stream around a 'virtual-memory-accesser'
     * object.
     *
     * memory          - The virtual memory accesser for the stream
     * startAddress    - The start-address for the 'memory' accesser
     * endAddress      - The end-address for the 'memory' accesser. Also known
     *                   as size of the stream.
     * ignoreFragments - When read() encounters a fragmented memory region it
     *                   usually throws an exception. When set to true read()
     *                   will ignore any fragmented memory and will fill them
     *                   with 0.
     * isFix           - When the stream grew more than expected, the endAddress
     *                   can be enlarge.
     *                   TODO! THIS VARIABLE MUST BE TRUE. THE FUNCTION IS NOT
     *                         IMPLEMENTED YET
     */
    cMemoryAccesserStream(const cVirtualMemoryAccesserPtr& memory,
                          uint startAddress,
                          uint endAddress,
                          bool ignoreFragments = false,
                          bool isFix = true);

    /*
     * Memory-files will be treated as the default endian in the system
     */
    defaultEndianImpl;

    // Driven from the basicIO

    /*
     * Retrive the position of the write/read head
     */
    virtual uint getPointer() const;

    /*
     * Return the length of the memory-file.
     */
    virtual uint length() const;

    /*
     * Return true if the file pointer reach the end of the file.
     */
    virtual bool isEOS();

    /*
     * Flush the information of the file. Shouldn't do anything.
     */
    virtual void flush();

    /*
     * Read from the memory array into a buffer. If the reading reach the end
     * of the array then the function return the number of bytes readed until
     * EOF reached.
     *
     * NOTE: The fragments region will be filled with zero. There isn't any
     *       notification that the user can get using the streams interface.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Write bytes to the stream.
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Move to a specific position in the stream. Throw exception if failed.
     * See basicInput::seek for more information.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     *
     * This value stand on 4kb which is the pageSize in most platform.
     */
    virtual uint getPipeReadBestRequest() const { return 4096; }

    // The cForkStream API
    virtual cForkStreamPtr fork() const;

    // cMemoryAccesserStream functions.

    /*
     * Return pointer to the memory stream.
     */
    bool isStreamWriteable() const;

    /*
     * Returns the memory accesser object
     */
    const cVirtualMemoryAccesserPtr& getMemoryAccesser() const;

    /*
     * Return the number of bytes relative to 'getMemoryAccesser' this stream
     * access started.
     */
    const uint getMemoryAccesserStartAddress() const;

    /*
     * Return the number of bytes relative to 'getMemoryAccesser' this stream
     * access ended.
     */
    const uint getMemoryAccesserEndAddress() const;

    /*
     * Return a new memory stream forked from this stream which access only
     * the relative 'start..end' portion of the stream
     *
     * virtualStart - The starting address from 0 pointer of this stream
     * virtualEnd - The ending address from 0 pointer of this stream in bytes
     */
    cMemoryAccesserStreamPtr forkRegion(uint virtualStart, uint virtualEnd) const;

protected:
    // Init the seeking methods.
    // All seek operation are permits
    virtual void initSeek();

private:
    // Copy-constructor. Used in the fork API
    cMemoryAccesserStream(const cMemoryAccesserStream& other);
    // Deny operator =
    cMemoryAccesserStream& operator = (const cMemoryAccesserStream& other);

    // The memory which the data will be read from
    cVirtualMemoryAccesserPtr m_data;
    // The position inside the memory stream, the pointer is inside the
    // [start..end] range in order to accelerated calculations.
    uint m_filePosition;
    // The relative address to access the stream
    uint m_startAddress;
    // The size of the stream (end-start)
    uint m_endAddress;
    // Should fragment memory be ignored or should be notify
    bool m_ignoreFragments;
};

#endif // __TBA_STL_STREAM_MEMORYACCESSERSTREAM_H
