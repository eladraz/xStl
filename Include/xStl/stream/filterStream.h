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

#ifndef __TBA_STL_STREAMS_FILTER_STREAM_H
#define __TBA_STL_STREAMS_FILTER_STREAM_H


/*
 * filterStream.h
 *
 * Define the "filterStream"s objects.
 * The filter_streams provide the interface of the standart basicIO (basicInput,
 * basicOutput) and provide the fileter interface which cascade stream to one
 * other. The class is abstract base class for all the decorators.
 *
 * This class should use as a chain.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/stream/basicIO.h"

/*
 * class filterStreamInput
 *
 * Decorator for basicInput only streams.
 */
class filterStreamInput : public basicInput
{
public:
    /*
     * Constructor
     *
     * inputStream - The input stream which being filtered
     */
    filterStreamInput(const cSmartPtr<basicInput>& inputStream);

    // Virtual destructor. You can inherit from me
    virtual ~filterStreamInput();

    // Return the lower input stream of the filter
    const cSmartPtr<basicInput>& getDownInputStream() const;

protected:
    // The lower-edge stream pointer
    cSmartPtr<basicInput> m_inputStream;
};


/*
 * class filterStreamOutput
 *
 * Decorator for basicOutput only streams.
 */
class filterStreamOutput : public basicOutput
{
public:
    /*
     * Constructor
     *
     * outputStream - The output stream which being filtered
     */
    filterStreamOutput(const cSmartPtr<basicOutput>& outputStream);

    // Virtual destructor. You can inherit from me
    virtual ~filterStreamOutput();

    // Return the lower output stream of the filter
    const cSmartPtr<basicOutput>& getDownOutputStream() const;

protected:
    // The lower-edge stream pointer
    cSmartPtr<basicOutput> m_outputStream;
};


/*
 * filterStream
 *
 * ABC for decorators.
 *
 * NOTE after opening the stream you should use it for reading or writing only.
 *      You cannot combine read and write functions in the same pipe unless you
 *      called the flush() function.
 */
class filterStream : public basicIO
{
public:
    /*
     * Constructor
     *
     * stream - The stream which being filtered
     */
    filterStream(const cSmartPtr<basicIO>& stream);

    // Virtual destructor. You can inherit from me
    virtual ~filterStream();

    // Return the lower stream of the filter
    const cSmartPtr<basicIO>& getDownStream() const;

protected:
    // The lower-edge stream pointer
    cSmartPtr<basicIO> m_stream;
};

/*
 * Take care to the endian of all filters streams. The endian should be the
 * same as the down stream endian implementation.
 */
#ifdef XSTL_16BIT
// 16 bit compilers
#define filterStreamEndianImpl \
    virtual uint32 readUint32(const uint8* buffer) const { return ((cEndian*)((basicInput*)m_stream.getPointer()))->readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return ((cEndian*)((basicInput*)m_stream.getPointer()))->readUint16(buffer); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { ((cEndian*)((basicInput*)m_stream.getPointer()))->writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { ((cEndian*)((basicInput*)m_stream.getPointer()))->writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return ((cEndian*)((basicInput*)m_stream.getPointer()))->isLittleEndian(); }
#else
// 32 bit compilers
#define filterStreamEndianImpl \
    virtual uint64 readUint64(const uint8* buffer) const { return ((cEndian*)((basicInput*)m_stream.getPointer()))->readUint64(buffer); }; \
    virtual uint32 readUint32(const uint8* buffer) const { return ((cEndian*)((basicInput*)m_stream.getPointer()))->readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return ((cEndian*)((basicInput*)m_stream.getPointer()))->readUint16(buffer); }; \
    virtual void writeUint64(uint8* buffer, uint64 number) const { ((cEndian*)((basicInput*)m_stream.getPointer()))->writeUint64(buffer,number); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { ((cEndian*)((basicInput*)m_stream.getPointer()))->writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { ((cEndian*)((basicInput*)m_stream.getPointer()))->writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return ((cEndian*)((basicInput*)m_stream.getPointer()))->isLittleEndian(); }
#endif

#endif // __TBA_STL_STREAMS_FILTER_STREAM_H
