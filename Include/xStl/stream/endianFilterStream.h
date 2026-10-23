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

#ifndef __TBA_STL_STREAMS_ENDIAN_FILTER_STREAM_H
#define __TBA_STL_STREAMS_ENDIAN_FILTER_STREAM_H

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file endianFilterStream.h
 * @brief Define a filter stream which changes a stream endians.
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"

/**
 * @brief A filter stream which converts the byte order of the numbers read from
 *    or written to the decorated stream.
 *
 * @details Usage:
 *
 * This example shows how we can use a "baseStream" as big-endian. The sample
 * wraps the baseStream as a non-destructible smart-pointer.
 *
 * @code
 * basicIO& baseStream;
 * cSmartPtr<cEndianFilterStream> endianStream(new cEndianFilterStream(
 *     cSmartPtr<basicIO>(&baseStream, SMARTPTR_DESTRUCT_NONE), false));
 * @endcode
 *
 * Author: Elad Raz \<e\@eladraz.com\>
 */
class cEndianFilterStream : public filterStream
{
public:
    /**
     * @brief Constructor. Create an endian filter stream based on the 'stream'
     *    stream.
     *
     * @param stream The decorated stream class to be used for reading/written.
     * @param isLittleEndian Set to TRUE if little-endian encoding should be
     *    used.
     */
    cEndianFilterStream(const cSmartPtr<basicIO>& stream,
                        bool isLittleEndian);

    /**
     * @brief Virtual destructor.
     */
    virtual ~cEndianFilterStream();

    /**
     * @brief Derived from the basicIO.
     *
     * @details A direct call to the decorated stream implementation
     *
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual uint read(void *buffer, const uint length);
    virtual uint write(const void *buffer, const uint length);
    virtual void flush();
    virtual void seek(const int distance, const basicInput::seekMethod method);
    virtual bool isEOS();
    /**
     * @return The total length of the stream in bytes.
     */
    virtual uint length() const;
    /**
     * @return The position of the read/write head, in bytes from the beginning
     *    of the stream.
     */
    virtual uint getPointer() const;
    virtual uint getPipeReadBestRequest() const;

    /**
     * @brief Derived from cEndian
     *
     * @param buffer The buffer to read the number from.
     * @return The deserialized number.
     */
    virtual uint64 readUint64(const uint8* buffer) const;
    virtual uint32 readUint32(const uint8* buffer) const;
    virtual uint16 readUint16(const uint8* buffer) const;
    virtual void writeUint64(uint8* buffer, uint64 number) const;
    virtual void writeUint32(uint8* buffer, uint32 number) const;
    virtual void writeUint16(uint8* buffer, uint16 number) const;
    virtual bool isLittleEndian() const;

protected:
    /**
     * @brief Init the seeking methods.
     */
    virtual void initSeek();

private:
    /**
     * @brief Deny copy-constructor
     */
    cEndianFilterStream();
    cEndianFilterStream(const cEndianFilterStream& other);
    cEndianFilterStream & operator = (const cEndianFilterStream& other);

    /**
     * @brief Compression variable for writing
     */
    bool m_isLittleEndian;
};


/** @} */
#endif // __TBA_STL_STREAMS_ENDIAN_FILTER_STREAM_H

