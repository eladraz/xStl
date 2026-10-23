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

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file memoryStream.h
 * @brief Declare the cMemoryStream class which implements the basicIO interface
 *   over the cBuffer class.
 *
 * @author Elad Raz
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
#define DEFAULT_STREAM_PAGE_SIZE (32 * 1024)  ///< 32k
#endif

/**
 * @brief Memory stream is a memory file which is stored inside a 'cBuffer'
 *   container.
 *
 * @details The memory file can increase its size if the fixedSize is false.
 */
class cMemoryStream : public cForkStream {
public:
    /**
     * @brief Default constructor.
     *
     * @details initData  - The data to be loaded to the memory stream
     *   realData  - The data to be linked as the memory stream
     *
     * @param pageSize The pageSize to be used in the stream
     * @param initSize The number of random bytes to be initialized.
     * @param fixedSize True in case the memory-stream has a fixed size and it
     *    cannot be expanded.
     * @throws cException in case there isn't enough memory.
     */
    cMemoryStream(uint pageSize = DEFAULT_STREAM_PAGE_SIZE,
                  uint initSize = 0,
                  bool fixedSize = false);
    /**
     * @brief Constructor. Create a stream over a copy of the data.
     *
     * @param initData The initial content of the stream.
     * @param pageSize The pageSize to be used in the stream.
     * @param fixedSize True in case the memory-stream has a fixed size and it
     *    cannot be expanded.
     */
    cMemoryStream(const cBuffer& initData,
                  uint pageSize = DEFAULT_STREAM_PAGE_SIZE,
                  bool fixedSize = false);
    /**
     * @brief Constructor. Create a stream which is linked to the shared data.
     *
     * @param realData The data to be linked as the memory stream.
     * @param pageSize The pageSize to be used in the stream.
     * @param fixedSize True in case the memory-stream has a fixed size and it
     *    cannot be expanded.
     */
    cMemoryStream(cBufferPtr& realData,
                  uint pageSize = DEFAULT_STREAM_PAGE_SIZE,
                  bool fixedSize = false);

    /**
     * @brief Copy-constructor. Used in the fork API
     *
     * @param other The stream to copy.
     */
    cMemoryStream(const cMemoryStream& other);

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
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Write bytes to the stream.
     *
     * @details Change the bytes of the array. If the file position reached the
     *    end of the array, and m_fixedSize is false then the array size will
     *    increase.
     *
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void *buffer, const uint length);

    /**
     * @brief Move to a specific position in the file.
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
     * @details This value stands at 4kb which is the pageSize in most platform.
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


    // cMemoryStream functions.

    /**
     * @brief Return pointer to the memory stream.
     *
     * @return Pointer to the memory stream.
     */
    cBufferPtr getStream() { return m_data; }

protected:
    /**
     * @brief Init the seeking methods.
     *
     * @details All seek operations are permitted
     */
    virtual void initSeek();

private:
    /* Private functions and variables */

    /**
     * @brief Deny operator =
     */
    cMemoryStream& operator = (const cMemoryStream& other);

    /**
     * @brief Is the mem-file fixed?
     */
    bool m_fixedSize;

    /**
     * @brief The position inside the memory stream
     */
    uint m_filePosition;

    /**
     * @brief The stream file.
     */
    cBufferPtr m_data;
};


/** @} */
#endif //__TBA_STL_STREAMS_MEMORY_STREAM_H
