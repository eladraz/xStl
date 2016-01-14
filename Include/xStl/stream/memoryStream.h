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

#ifndef __TBA_STL_STREAMS_MEMORY_STREAM_H
#define __TBA_STL_STREAMS_MEMORY_STREAM_H

/*
 * memorystream.h
 *
 * Declare the cMemoryStream class which implement the basicIO interface overs
 * cBuffer class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/utils/algorithm.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/forkStream.h"

/*
 * The default page size for the memory stream allocation.
 */
#ifndef DEFAULT_STREAM_PAGE_SIZE
#define DEFAULT_STREAM_PAGE_SIZE (32 * 1024)  // 32k
#endif

/*
 * class cMemoryStream
 *
 * Memory stream is a memory file which store inside a 'cBuffer' container.
 * The memory file can increase it's size if the fixedSize is false.
 */
class cMemoryStream : public cForkStream {
public:
    /*
     * Default constructor.
     *
     * initData  - The data to be loaded to the memory stream
     * realData  - The data to be linked as the memory stream
     * pageSize  - The pageSize to be used in the stream
     * initSize  - The number of random bytes to be initialized.
     * fixedSize - True incase the memory-stream has a fixed size and it
     *             cannot be expanded.
     *
     * Throws exception incase there isn't engouh memory.
     */
    cMemoryStream(uint pageSize = DEFAULT_STREAM_PAGE_SIZE,
                  uint initSize = 0,
                  bool fixedSize = false);
    cMemoryStream(const cBuffer& initData,
                  uint pageSize = DEFAULT_STREAM_PAGE_SIZE,
                  bool fixedSize = false);
    cMemoryStream(cBufferPtr& realData,
                  uint pageSize = DEFAULT_STREAM_PAGE_SIZE,
                  bool fixedSize = false);

    // Copy-constructor. Used in the fork API
    cMemoryStream(const cMemoryStream& other);

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
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Write bytes to the stream.
     * Change the bytes of the array. If the file postion reached the end of the
     * array, and m_fixedSize is false then the array size will increase.
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Move to a specific position in the file. Throw exception if failed.
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


    // cMemoryStream functions.

    /*
     * Return pointer to the memory stream.
     */
    cBufferPtr getStream() { return m_data; }

protected:
    // Init the seeking methods.
    // All seek operation are permits
    virtual void initSeek();

private:
    /* Private functions and variables */

    // Deny operator =
    cMemoryStream& operator = (const cMemoryStream& other);

    // Is the mem-file is fixed?
    bool m_fixedSize;

    // The position inside the memory stream
    uint m_filePosition;

    // The stream file.
    cBufferPtr m_data;
};

#endif //__TBA_STL_STREAMS_MEMORY_STREAM_H
