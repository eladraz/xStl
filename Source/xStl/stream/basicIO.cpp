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

#include "xStl/xStlPrecompiled.h"
/*
 * basicIO.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/exception.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"


/********************************************************************
 *******************************************************
 *****
 **     b a s i c I n p u t    i m p l e m e n t a t i o n
 *
 */

basicInput::basicInput()
    #ifndef XSTL_16BIT
    : m_addressDecodingType(DEFAULT_REMOTE_ADDRESS_ENCODING),
      m_shouldEncodeType(true)
    #endif
{
}

basicInput::~basicInput()
{
}

uint basicInput::pipeRead(void *buffer, const uint length)
{
    uint8 *array = (uint8 *)buffer;      // The array buffer
    uint readed = 0;             // Total readed
    uint readedAtOnce = 0;     // How many uint8 readed in order.
    uint leftSize = length;     // How many bytes to read more.

    /* Reading loop */
    while (leftSize > 0)
    {
        // Try to read
        readedAtOnce = read(array, t_min(leftSize, getPipeReadBestRequest()));

        // Increase counters
        array    += readedAtOnce;
        leftSize -= readedAtOnce;
        readed   += readedAtOnce;

        // Test whether we reached the end of the stream
        if (isEOS())
        {
            return readed;
        }
    }

    return readed;
}

uint basicInput::pipeRead(cBuffer& buffer, const uint length)
{
    uint oldSize  = buffer.getSize(); // Old size of the buffer
    uint readed   = 0;                // The readed bytes.

    /* Prepare the output buffer */
    buffer.changeSize(oldSize + length);

    /* Read the data */
    readed = pipeRead((buffer.getBuffer() + oldSize), length);

    /* Chope the data */
    if (readed < length)
    {
        buffer.changeSize(oldSize + readed);
    }

    return readed;
}

void basicInput::pipeReadNotEos(void *buffer, const uint length)
{
    CHECK(pipeRead(buffer, length) == length);
}

void basicInput::pipeReadNotEos(cBuffer& buffer, const uint length)
{
    CHECK(pipeRead(buffer, length) == length);
}

cString basicInput::readFixedSizeString(uint numberOfCharacters, int unicodeSize)
{
    cBuffer buffer((numberOfCharacters + 1) * unicodeSize);
    uint8* buf = buffer.getBuffer();
    pipeRead(buf, numberOfCharacters * unicodeSize);
    if (unicodeSize > 1)
    {
        buf[numberOfCharacters * unicodeSize] = 0;
        cBuffer ret(sizeof(character) * (numberOfCharacters + 1));
        character* retBuf = (character*)ret.getBuffer();
        cChar::covert2string(retBuf, numberOfCharacters+1, buf, unicodeSize);
        return cString(retBuf);
    } else
    {
        buf[numberOfCharacters] = 0;
        return cString((char*)buf);
    }
}

void basicInput::readAsciiString(cString& ret,
                                 uint8 terminate,
                                 const uint numberOfCharacter)
{
    /* Read single characters */
    uint8 ch = terminate + 1;
    uint left = numberOfCharacter;

    while (ch != terminate)
    {
        if (left == 0)
        {
            TRACE(TRACE_VERY_LOW, "readString: Number of bytes for string exceeded.");
            XSTL_THROW(cException, EXCEPTION_READ_ERROR);
        }

        // Try to read a single character from the stream
        uint read = pipeRead((void *)(&ch), sizeof(ch));
        if (read == 0)
        {
            // EOF reached and the NULL terminate couldn't be found.
            XSTL_THROW(cException, EXCEPTION_READ_ERROR);
        }

        left--;

        // Remove the terminate character
        if (ch != terminate)
            ret+= (char)(ch);
    }
}

void basicInput::readUnicodeString(cString& ret,
                                   unichar terminate,
                                   const uint numberOfCharacter)
{
    /* Read single characters */
    unichar ch = terminate + 1;
    uint left = numberOfCharacter;

    while (ch != terminate)
    {
        if (left == 0)
        {
            TRACE(TRACE_VERY_LOW, "readString: Number of bytes for string exceeded.");
            XSTL_THROW(cException, EXCEPTION_READ_ERROR);
        }

        // Try to read a single character from the stream
        uint read = pipeRead((void *)(&ch), sizeof(ch));
        if (read == 0)
        {
            // EOF reached and the NULL terminate couldn't be found.
            XSTL_THROW(cException, EXCEPTION_READ_ERROR);
        }

        left--;

        // Remove the terminate character
        if (ch != terminate)
        {
            #ifndef XSTL_UNICODE
                ret+= cChar::covert2Ascii(ch);
            #else
                ret+= ch;
            #endif
        }
    }
}

// In order to avoid calling cString(number) constructor.
static character* nil = NULL;

cString basicInput::readUnicodeNullString(const uint numberOfCharacter /* = MAX_CHAR */)
{
    cString ret(nil, t_min(numberOfCharacter, (uint)cString::DefaultStringPage));
    readUnicodeString(ret, 0, numberOfCharacter);
    return ret;
}

cString basicInput::readAsciiNullString(const uint numberOfCharacter /* = MAX_CHAR */)
{
    cString ret(nil, t_min(numberOfCharacter, (uint)cString::DefaultStringPage));
    readAsciiString(ret, 0, numberOfCharacter);
    return ret;
}

cString basicInput::readAsciiDosString(const uint numberOfCharacter /* = MAX_CHAR */)
{
    cString ret(nil, t_min(numberOfCharacter, (uint)cString::DefaultStringPage));
    readAsciiString(ret, '$', numberOfCharacter);
    return ret;
}

cString basicInput::readAsciiPascal8String(const uint numberOfCharacter /* = MAX_CHAR */)
{
    uint8 length;
    uint8 string[256];

    // Read the length
    streamReadUint8(length);

    if (length > numberOfCharacter)
    {
        // Invalid number of characters
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }
    // Read the string bytes
    uint read = pipeRead(string, length);
    if (read != length)
    {
        // EOF reached!
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }
    string[length] = 0;

    return cString((char *)string);
}

cString basicInput::readUnicodePascal8String(const uint numberOfCharacter)
{
    uint8 length;
    streamReadUint8(length);
    return readFixedSizeString(length, 2);
}

cString basicInput::readUnicodePascal16String(const uint numberOfCharacter)
{
    uint16 length;
    streamReadUint16(length);
    return readFixedSizeString(length, 2);
}

cString basicInput::readAsciiPascal16String(const uint numberOfCharacter /* = MAX_CHAR */)
{
    uint16 length;

    // Read the length
    streamReadUint16(length);

    if (length > numberOfCharacter)
    {
        // Invalid number of characters
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }

    // Allocate dynamic memory
    cBuffer string(length + 1);

    // Read all bytes
    uint read = pipeRead(string.getBuffer(), length);
    if (read != length)
    {
        // EOF reached!
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }
    string[length] = 0;

    return cString((char *)string.getBuffer());
}

bool basicInput::__canSeek(const int distance, const basicInput::seekMethod method)
{
    if (method == IO_SEEK_SET)
    {
        /* Check that we can go back    */
        /* And we are in positive value */
        return (canSeekFromBegin & (distance >= 0));
    } else if (method == IO_SEEK_CUR)
    {
        if (distance < 0)
        {
            /* Check if we can go back */
            return canSeekBackward;
        } else
        {
            return canSeekForward;
        }
    } else if (method == IO_SEEK_END)
    {
        return (canSeekFromEnd & (distance <= 0));
    } else
    {
        // TODO! Why should I put the following line in comment??!
        //ASSERT(false, "Seeking method is invalid");

        return FALSE;
    }
}


/* Deserialization operation */

void basicInput::readAllStream(cBuffer& rawData)
{
    if (canLength && canGetPointer)
    {
        uint left = length() - getPointer();
        pipeRead(rawData, left);
    } else
    {
        // Changing the page size into the default reading size.
        uint savePageSize = rawData.getPageSize();
        rawData.setPageSize(getPipeReadBestRequest());

        uint oldBufferSize = rawData.getSize();
        rawData.changeSize(oldBufferSize + getPipeReadBestRequest(), false);

        uint readed = 0;
        uint total  = oldBufferSize;
        uint8* buffer = rawData.getBuffer() + total;

        // Reading loop
        do
        {
            // Read a chunk
            total += (readed = pipeRead(buffer, getPipeReadBestRequest()));

            // Save space for the next reading operation
            if (readed == getPipeReadBestRequest())
            {
                rawData.changeSize(total + getPipeReadBestRequest());
                buffer = rawData.getBuffer() + total;
            } else
            {
                // Restore the page size
                rawData.setPageSize(savePageSize);

                // Chop free space
                rawData.changeSize(total);
            }

        } while (readed == getPipeReadBestRequest()); // Until there isn't error
    }
}


#ifndef XSTL_16BIT
void basicInput::streamReadUint64(uint64& qword)
{
    uint8 buffer[sizeof(uint64)];
    uint readed = pipeRead(&buffer, sizeof(uint64));
    if (readed != sizeof(uint64))
    {
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }
    qword = ((cEndian*)(this))->readUint64(buffer);
}
#endif // XSTL_16BIT

void basicInput::streamReadUint32(uint32& dword)
{
    uint8 buffer[sizeof(uint32)];
    uint readed = pipeRead(&buffer, sizeof(uint32));
    if (readed != sizeof(uint32))
    {
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }
    dword = ((cEndian*)(this))->readUint32(buffer);
}

void basicInput::streamReadUint16(uint16& word)
{
    uint8 buffer[sizeof(uint16)];
    uint readed = pipeRead(&buffer, sizeof(uint16));
    if (readed != sizeof(uint16))
    {
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }
    word = ((cEndian*)(this))->readUint16(buffer);
}

void basicInput::streamReadUint8(uint8& byte)
{
    uint readed = pipeRead(&byte, sizeof(uint8));
    if (readed != sizeof(char))
    {
        XSTL_THROW(cException, EXCEPTION_READ_ERROR);
    }
}

void basicInput::streamReadInt8(int8& byte)
{
    streamReadUint8((uint8&)(byte));
}

void basicInput::streamReadInt16(int16& word)
{
    streamReadUint16((uint16&)(word));
}

void basicInput::streamReadInt32(int32& dword)
{
    streamReadUint32((uint32&)(dword));
}

#ifndef XSTL_16BIT
void basicInput::streamReadInt64(int64& byte)
{
    streamReadUint64((uint64&)(byte));
}

void basicInput::changeDecodedAddressType(bool shouldEncodeType,
                                          RemoteAddressEncodingTypes enforceType)
{
    m_shouldEncodeType = shouldEncodeType;
    m_addressDecodingType = enforceType;
}

remoteAddressNumericValue basicInput::streamReadRemoteAddress()
{
    remoteAddressNumericValue ret(0);
    RemoteAddressEncodingTypes enforceType = m_addressDecodingType;
    if (m_shouldEncodeType)
    {
        // Read the type
        uint8 type;
        streamReadUint8(type);
        enforceType = (RemoteAddressEncodingTypes)(type);
    }

    // Change the type and return
    ret.m_type = enforceType;

    // Read the address
    switch (enforceType)
    {
    case REMOTE_ADDRESS_16BIT:
        streamReadUint16(ret.m_union.m_16bitAddress);
        return ret;
    case REMOTE_ADDRESS_32BIT:
        streamReadUint32(ret.m_union.m_32bitAddress);
        return ret;
    case REMOTE_ADDRESS_64BIT:
        streamReadUint64(ret.m_union.m_64bitAddress);
        return ret;
    default:
        CHECK_FAIL();
    }
}
#endif // XSTL_16BIT


/********************************************************************
 *******************************************************
 *****
 **     b a s i c O u t p u t    i m p l e m e n t a t i o n
 *
 */

basicOutput::basicOutput()
    #ifndef XSTL_16BIT
    : m_addressEncodingType(DEFAULT_REMOTE_ADDRESS_ENCODING),
      m_shouldEncodeType(true)
    #endif
{
}

basicOutput::~basicOutput()
{
}

void basicOutput::pipeWrite(const void *buffer, const uint length)
{
    uint bufferLength   = length;
    uint writtenAtOnce = 0;
    uint8 *array = (uint8 *) buffer;

    /* Writing loop */
    while (bufferLength != 0)
    {
        writtenAtOnce = write(array, bufferLength);

        // Increase buffer position
        array+= writtenAtOnce;
        bufferLength-= writtenAtOnce;
    }
}

void basicOutput::pipeWrite(const cBuffer& buffer, const uint length)
{
    pipeWrite(buffer.getBuffer(), length);
}


#ifndef XSTL_16BIT
void basicOutput::streamWriteUint64(const uint64 qword)
{
    uint8 buffer[sizeof(uint64)];
    ((cEndian*)(this))->writeUint64(buffer, qword);
    pipeWrite(buffer, sizeof(uint64));
}
#endif // XSTL_16BIT


void basicOutput::streamWriteUint32(const uint32 dword)
{
    uint8 buffer[sizeof(uint32)];
    ((cEndian*)(this))->writeUint32(buffer, dword);
    pipeWrite(buffer, sizeof(uint32));
}

void basicOutput::streamWriteUint16(const uint16 word)
{
    uint8 buffer[sizeof(uint16)];
    ((cEndian*)(this))->writeUint16(buffer, word);
    pipeWrite(buffer, sizeof(uint16));
}

void basicOutput::streamWriteUint8(const uint8 byte)
{
    pipeWrite(&byte, sizeof(uint8));
}


#ifndef XSTL_16BIT
void basicOutput::streamWriteInt64(const int64 qword)
{
    streamWriteUint64((uint64)(qword));
}
#endif // XSTL_16BIT

void basicOutput::streamWriteInt32(const int32 dword)
{
    streamWriteUint32((uint32)(dword));
}

void basicOutput::streamWriteInt16(const int16 word)
{
    streamWriteUint16((uint16)(word));
}

void basicOutput::streamWriteInt8(const int8 byte)
{
    streamWriteUint8((uint8)(byte));
}

void basicOutput::writeFixedSizeString(const cString& string,
                                       uint numberOfCharacters,
                                       int unicodeSize)
{
    cBuffer buffer(numberOfCharacters * unicodeSize);
    uint8* buf = buffer.getBuffer();
    memset(buf, 0, buffer.getSize());
    uint count = t_min(string.length(), numberOfCharacters);

    for (uint i = 0; i < count; i++)
    {
        switch (unicodeSize)
        {
        case 1: buf[i] = cChar::covert2Ascii(string[i]); break;
        case 2: ((uint16*)buf)[i] = string[i]; break;
        case 4: ((uint32*)buf)[i] = string[i]; break;
        default:
            CHECK_FAIL();
        }
    }

    pipeWrite(buffer, buffer.getSize());
}

void basicOutput::writeAsciiNullString(const cString& string,
                                       bool shouldPutZeroTerminator)
{
    #ifdef XSTL_UNICODE
        char ch;
        for (uint i = 0; i < string.length(); i++)
        {
            ch = cChar::covert2Ascii(string[i]);
            pipeWrite(&ch, sizeof(ch));
        }
        if (shouldPutZeroTerminator)
        {
            // Write the null terminated character
            ch = 0;
            pipeWrite(&ch, sizeof(ch));
        }
    #else
        // ASCII version, just put it throw
        uint length = string.length();
        if (shouldPutZeroTerminator)
            length++;
        pipeWrite(string.getBuffer(), length * sizeof(character));
    #endif
}

void basicOutput::writeUnicodeNullString(const cString& string,
                                         bool shouldPutZeroTerminator)
{
    #ifdef XSTL_UNICODE
        // UNICODE version, just put it throw
        uint length = string.length();
        if (shouldPutZeroTerminator)
            length++;
        pipeWrite(string.getBuffer(), length * sizeof(character));
    #else
        unichar ch;
        for (uint i = 0; i < string.length(); i++)
        {
            ch = string[i];
            pipeWrite(&ch, sizeof(ch));
        }
        if (shouldPutZeroTerminator)
        {
            // Write the null terminated character
            ch = 0;
            pipeWrite(&ch, sizeof(ch));
        }
    #endif
}

#ifndef XSTL_16BIT
void basicOutput::changeEncodedAddressType(bool shouldEncodeType,
                                           RemoteAddressEncodingTypes enforceType)
{
    m_shouldEncodeType = shouldEncodeType;
    m_addressEncodingType = enforceType;
}

void basicOutput::streamWriteRemoteAddress(const remoteAddressNumericValue& address)
{
    RemoteAddressEncodingTypes enforceType = m_addressEncodingType;
    if (m_shouldEncodeType)
    {
        // Get the type
        enforceType = address.m_type;
        // Write the type
        uint8 type = (uint8)enforceType;
        streamWriteUint8(type);
    }

    switch (enforceType)
    {
    case REMOTE_ADDRESS_16BIT:
            streamWriteUint16(address.m_union.m_16bitAddress); break;
    case REMOTE_ADDRESS_32BIT:
            streamWriteUint32(address.m_union.m_32bitAddress); break;
    case REMOTE_ADDRESS_64BIT:
            streamWriteUint64(address.m_union.m_64bitAddress); break;
    default:
        CHECK_FAIL();
    }
}
#endif // XSTL_16BIT

/********************************************************************
 *******************************************************
 *****
 **     b a s i c I O     i m p l e m e n t a t i o n
 *
 */

void basicIO::copyStream(basicOutput& destination, basicInput& source)
{
    uint read;
    do
    {
        cBuffer data;
        ASSERT(source.isEOS());
        read = source.pipeRead(data, source.getPipeReadBestRequest());
        if (read > 0)
            destination.pipeWrite(data, read);
    } while ((read > 0) && (!source.isEOS()));
}

// gcc work around. gcc can't access internal function of declared class
// in order to overide this behavior we will use global static functions that
// implemens streamReadUint32/streamWriteUint32 and pipeRead/pipeWrite with interface
#ifdef XSTL_LINUX
extern "C" void gccLinuxStreamReadUint32(basicInput& inputStream, uint32& dword)
{
    inputStream.streamReadUint32(dword);
}

extern "C" void gccLinuxStreamWriteUint32(basicOutput& outputStream, uint32& dword)
{
    outputStream.streamWriteUint32(dword);
}

extern "C" uint gccLinuxStreamPipeRead(basicInput& inputStream, void *buffer, const uint length)
{
    return inputStream.pipeRead(buffer, length);
}

extern "C" void gccLinuxStreamPipeWrite(basicOutput& outputStream, void *buffer, const uint length)
{
    outputStream.pipeWrite(buffer, length);
}
#endif
