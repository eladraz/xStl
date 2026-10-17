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

/*
 * stringerStream.h
 *
 * The stringer module converts data into strings and outputs them into
 * virtual functions which handle the string.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/smartptr.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/xstlLockable.h"

/*
 * Implements the serialization functions (such as write, operator << etc) to
 * convert raw data into strings.
 * The class adds some abstraction with the function 'outputString()' which
 * should be implemented in the inheriting class. The function
 *
 * NOTE: This class is thread-safe
 */
class cStringerStream : public basicOutput
{
public:
    /*
     * Endian thing...
     */
    defaultEndianImpl;

    /*
     * Constructor. Inits the string output stream.
     *
     * shouldWaitToNewLine - True if the output function (outputString) should
     *                       be called only when a new line arrives.
     */
    cStringerStream(bool shouldWaitToNewLine);

    /*
     * The following functions are implementation of the basicOutput
     * interface. The functions are implemented by converting the stream
     * to a character set.
     *
     * See basicOutput.
     */
    virtual uint write(const void* buffer, const uint length);
    virtual void flush();

    /*
     * Numeric operator <<, convert number into strings, characters into
     * data.
     */
    cStringerStream& operator << (const cBuffer& rawData);
    cStringerStream& operator << (const char ch);
#ifdef XSTL_UNICODE
    cStringerStream& operator << (const character ch);
#endif
    //cStringerStream& operator << (const int16 word);
    cStringerStream& operator << (const int32  dword);
    cStringerStream& operator << (const int64  qword);
    cStringerStream& operator << (const uint8  byte);
    cStringerStream& operator << (const uint16 word);
    cStringerStream& operator << (const uint32 dword);
    cStringerStream& operator << (const uint64 qword);

    /*
     * Change the base for numeric stringer
     */
    cStringerStream& setBase(uint newBase);

    /*
     * Return the current base for the stream
     */
    uint getBase() const;

    /*
     * cString operator
     */
    cStringerStream& operator << (const cString& string);

    /*
     * ASCII string operator
     */
    cStringerStream& operator << (const char* asciiString);

    /*
     * Unicode string operator
     */
    #ifdef XSTL_UNICODE
    cStringerStream& operator << (const character* unicodeString);
    #endif

protected:
    /*
     * Abstract function. Used in the sub-classes in order to 'print' a string
     * to the device.
     *
     * string - The string to be output.
     *
     * NOTE: The outputString function isn't called with any lockable object
     *       acquired. Otherwise a deadlock may occur.
     */
    virtual void outputString(const cString& string) = 0;

    // The screen optimizer
    enum { SCREEN_STRING_OPTIMIZER = 160 };

private:
    /*
     * Called in order to flush out all cache complete lines.
     */
    void flushNewLines();

    /*
     * Clear the output buffer and return a copy-of it.
     * NOTE: This function is thread-safe
     *
     * ret - Will be filled by the information to be output
     *
     * Return true if ret was filled by information to be output
     */
    bool flushBuffer(cString& ret);


    /*
     * Clear the output buffer until 'endl' appears. The information is stored
     * in a local stack variable.
     * NOTE: This function is thread-safe
     *
     * ret - Will be filled by the information to be output
     *
     * Return true if ret was filled by information to be output
     */
    bool flushEndlBuffer(cString& ret);

    /*
     * Safely append data to the line storage repository.
     *
     * data - The information that will be added to the line-repository
     */
    void addToRepository(const cString& data);

    // The portion of the line saved in a cache. Used for waiting for endln.
    cString m_lineRepository;
    // Should the class
    bool m_shouldWaitToNewLine;
    // The base for numeric conversions.
    uint m_base;

    // Protect the stream
    cXstlLockable m_lock;

    // The default numeric base
    enum { DEFAULT_BASE = 10 };
};

// The reference countable object
typedef cSmartPtr<cStringerStream> cStringerStreamPtr;

/*
 * The newline character set
 */
extern character* endl;

/*
 * Simple implementation of the cStringerStream which save the output strings
 * inside a memory of a cString class.
 *
 * NOTE: This class is not thread-safe
 */
class cStringStream : public cStringerStream
{
public:
    /*
     * Constructor. This class shouldn't wait for endln.
     *
     * optimization - The amount of memory to reserve.
     */
    cStringStream(uint optimization = SCREEN_STRING_OPTIMIZER);

    /*
     * Returns the complete stream memory as a string.
     */
    const cString& getData() const;

    /*
     * Deletes the memory held so far.
     */
    void clean();

protected:
    /*
     * Output the string into the m_data member.
     * See cStringerStream::outputString
     */
    virtual void outputString(const cString& string);

private:
    // Deny copy-constructor and operator =
    cStringStream(const cStringStream& other);
    cStringStream& operator = (const cStringStream& other);
    // The string data
    cString m_data;
};

/*
 * The NULL stringer stream is responsible for sending the string message into
 * a void space. The implementation is simple
 */
class cNullStringerStream : public cStringerStream {
public:
    /*
     * Default constructor
     */
    cNullStringerStream() : cStringerStream(false) {};

protected:
    // Deny copy-constructor and operator =
    cNullStringerStream(const cNullStringerStream& other);
    cNullStringerStream& operator = (const cNullStringerStream& other);

    /*
     * See cStringerStream::outputString
     * Ignore the string.
     */
    virtual void outputString(const cString&) {};
};

#endif // __TBA_STL_STREAMS_STRINGER_H
