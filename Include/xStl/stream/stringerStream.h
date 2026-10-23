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

#ifndef __TBA_STL_STREAMS_STRINGER_H
#define __TBA_STL_STREAMS_STRINGER_H

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file stringerStream.h
 * @brief The stringer module converts data into strings and outputs them into
 *   virtual functions which handle the string.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/smartptr.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/xstlLockable.h"

/**
 * @brief Implements the serialization functions (such as write, operator \<\<
 *    etc)
 *   to convert raw data into strings.
 *
 * @details The class adds some abstraction with the function 'outputString()'
 *    which should be implemented in the inheriting class. The function
 *
 * @note This class is thread-safe
 */
class cStringerStream : public basicOutput
{
public:
    /**
     * @brief Endian thing...
     */
    defaultEndianImpl;

    /**
     * @brief Constructor. Inits the string output stream.
     *
     * @param shouldWaitToNewLine True if the output function (outputString)
     *    should be called only when a new line arrives.
     */
    cStringerStream(bool shouldWaitToNewLine);

    /**
     * @brief The following functions are implementation of the basicOutput
     *    interface.
     *
     * @details The functions are implemented by converting the stream to a
     *    character set.
     *
     * @see basicOutput
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void* buffer, const uint length);
    virtual void flush();

    /**
     * @brief Numeric operator \<\<, convert number into strings, characters
     *    into data.
     *
     * @param rawData The data to be written as is.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const cBuffer& rawData);
    /**
     * @brief Write a character to the stream.
     *
     * @param ch The character to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const char ch);
#ifdef XSTL_UNICODE
    /**
     * @brief Write a character to the stream.
     *
     * @param ch The character to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const character ch);
#endif
    /**
     * @brief cStringerStream\& operator \<\< (const int16 word);
     *
     * @param dword The number to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const int32  dword);
    /**
     * @brief Write the int64 number as text, in the current base.
     *
     * @param qword The number to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const int64  qword);
    /**
     * @brief Write the uint8 number as text, in the current base.
     *
     * @param byte The number to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const uint8  byte);
    /**
     * @brief Write the uint16 number as text, in the current base.
     *
     * @param word The number to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const uint16 word);
    /**
     * @brief Write the uint32 number as text, in the current base.
     *
     * @param dword The number to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const uint32 dword);
    /**
     * @brief Write the uint64 number as text, in the current base.
     *
     * @param qword The number to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const uint64 qword);

    /**
     * @brief Change the base for numeric stringer
     *
     * @param newBase The new base of the numbers.
     * @return A reference to this stream.
     */
    cStringerStream& setBase(uint newBase);

    /**
     * @brief Return the current base for the stream
     *
     * @return The current base for the stream.
     */
    uint getBase() const;

    /**
     * @brief cString operator
     *
     * @param string The string to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const cString& string);

    /**
     * @brief ASCII string operator
     *
     * @param asciiString The null-terminated ASCII string to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const char* asciiString);

    /*
     * Unicode string operator
     */
    #ifdef XSTL_UNICODE
    /**
     * @brief Write a null-terminated unicode string.
     *
     * @param unicodeString The string to write.
     * @return A reference to this stream.
     */
    cStringerStream& operator << (const character* unicodeString);
    #endif

protected:
    /**
     * @brief Abstract function. Used in the sub-classes in order to 'print' a
     *    string to
     *   the device.
     *
     * @param string The string to be output.
     * @note The outputString function isn't called with any lockable object
     *    acquired. Otherwise a deadlock may occur.
     */
    virtual void outputString(const cString& string) = 0;

    /**
     * @brief The screen optimizer
     */
    enum { SCREEN_STRING_OPTIMIZER = 160 };

private:
    /**
     * @brief Called in order to flush out all cache complete lines.
     */
    void flushNewLines();

    /**
     * @brief Clear the output buffer and return a copy-of it.
     *
     * @param ret Will be filled by the information to be output
     * @return True if ret was filled by information to be output.
     * @note This function is thread-safe
     */
    bool flushBuffer(cString& ret);


    /**
     * @brief Clear the output buffer until 'endl' appears.
     *
     * @details The information is stored in a local stack variable.
     *
     * @param ret Will be filled by the information to be output
     * @return True if ret was filled by information to be output.
     * @note This function is thread-safe
     */
    bool flushEndlBuffer(cString& ret);

    /**
     * @brief Safely append data to the line storage repository.
     *
     * @param data The information that will be added to the line-repository
     */
    void addToRepository(const cString& data);

    /**
     * @brief The portion of the line saved in a cache.
     *
     * @details Used for waiting for endln.
     */
    cString m_lineRepository;
    /**
     * @brief Should the class
     */
    bool m_shouldWaitToNewLine;
    /**
     * @brief The base for numeric conversions.
     */
    uint m_base;

    /**
     * @brief Protect the stream
     */
    cXstlLockable m_lock;

    /**
     * @brief The default numeric base
     */
    enum { DEFAULT_BASE = 10 };
};

/**
 * @brief The reference countable object
 */
typedef cSmartPtr<cStringerStream> cStringerStreamPtr;

/**
 * @brief The newline character set
 */
extern character* endl;

/**
 * @brief Simple implementation of the cStringerStream which save the output
 *    strings
 *   inside a memory of a cString class.
 *
 * @note This class is not thread-safe
 */
class cStringStream : public cStringerStream
{
public:
    /**
     * @brief Constructor. This class shouldn't wait for endln.
     *
     * @param optimization The amount of memory to reserve.
     */
    cStringStream(uint optimization = SCREEN_STRING_OPTIMIZER);

    /**
     * @brief Returns the complete stream memory as a string.
     *
     * @return The complete stream memory as a string.
     */
    const cString& getData() const;

    /**
     * @brief Deletes the memory held so far.
     */
    void clean();

protected:
    /**
     * @brief Output the string into the m_data member.
     *
     * @param string The string to output.
     * @see cStringerStream::outputString
     */
    virtual void outputString(const cString& string);

private:
    /**
     * @brief Deny copy-constructor and operator =
     */
    cStringStream(const cStringStream& other);
    cStringStream& operator = (const cStringStream& other);
    /**
     * @brief The string data
     */
    cString m_data;
};

/**
 * @brief The NULL stringer stream is responsible for sending the string message
 *   into a void space.
 *
 * @details The implementation is simple
 */
class cNullStringerStream : public cStringerStream {
public:
    /**
     * @brief Default constructor
     */
    cNullStringerStream() : cStringerStream(false) {};

protected:
    /**
     * @brief Deny copy-constructor.
     *
     * @param other Not used.
     */
    cNullStringerStream(const cNullStringerStream& other);
    /**
     * @brief Deny operator =.
     *
     * @param other Not used.
     * @return Not used.
     */
    cNullStringerStream& operator = (const cNullStringerStream& other);

    /**
     * @brief Ignore the string.
     *
     * @see cStringerStream::outputString
     */
    virtual void outputString(const cString&) {};
};


/** @} */
#endif // __TBA_STL_STREAMS_STRINGER_H
