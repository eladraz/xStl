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
 * string.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/except/assert.h"
#include "xStl/except/trace.h"
#include "xStl/except/exception.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/list.h"


// The default strip in the xStl project
#ifndef XSTL_UNIQUE_STRIP
    // The string "-" which indicate that the number is negative
    const character* cString::m_stringNegativeInteger = XSTL_STRING("-");
    // The strip for different bases. Uses the 10 digits characters 26 digits of
    // the alpha-beit in the english language and 12 character for Indian character
    const character* cString::m_baseStrip = XSTL_STRING("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()-+");
#endif

cString::cString(const character* string /* = NULL*/,
                 uint             optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    if (string == NULL)
    {
        // Creates an empty string
        createEmptyString(optMem);
    } else
    {
        // Construct a string and copy data
        m_stringLength = cChar::getStrlen(string);
        m_buffer = new cSArray<character>(m_stringLength + 1, optMem);

        // Copy the string
        cOS::memcpy(m_buffer->getBuffer(), string, m_stringLength * sizeof(character));
        // Add the null terminate string
        (*m_buffer)[m_stringLength] = cChar::getNullCharacter();
    }
}

#ifdef XSTL_UNICODE
cString::cString(const char* string,
                 uint        optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    if (string == NULL)
    {
        // Creates an empty string
        createEmptyString(optMem);
    } else
    {
        // Construct a string and copy data
        m_stringLength = (uint)strlen(string);
        m_buffer = new cSArray<character>(m_stringLength + 1, optMem);

        // Copy the string
        for (uint i = 0; i < m_stringLength; i++)
        {
            m_buffer->getBuffer()[i] = (character) string[i];
        }
        // Add the null terminate string
        (*m_buffer)[m_stringLength] = cChar::getNullCharacter();
    }
}

cSArray<char> cString::getASCIIstring() const
{
    uint newLen = length() + 1;
    cSArray<char> ret(newLen);
    cChar::covert2Ascii(ret.getBuffer(), newLen, getBuffer());
    return ret;
}
#else
// XSTL_UNICODE is not defined, Not unicode version of xStl

cSArray<char> cString::getASCIIstring() const
{
    uint newLen = length() + 1;
    cSArray<char> ret(newLen);
    // Copy all data including the null-terminate character
    cOS::memcpy(ret.getBuffer(), getBuffer(), newLen);
    return ret;
}
#endif // XSTL_UNICODE


cSArray<unichar> cString::getUnicodeString() const
{
    uint newLen = length() + 1;
    cSArray<unichar> ret(newLen);

    #ifdef XSTL_UNICODE
        cOS::memcpy(ret.getBuffer(), getBuffer(), newLen * sizeof(unichar));
    #else
        const char* buffer = getBuffer();
        uint length = (newLen - 1);
        for (uint i = 0; i < length; i++)
        {
            ret[i] = buffer[i];
        }
        // Add unicode null-terminate char
        ret[length] = 0;
    #endif

    return ret;
}


cString::cString(const character ch,
                 uint            optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    // Init the object
    createEmptyString(optMem);
    // Create a minidump array
    character buf[2] = {ch, cChar::getNullCharacter()};
    // Use operator = to copy to data (Avoids code-duplication)
    *this = cString(buf, optMem); // Create a cString with one character
}

#ifdef XSTL_UNICODE
cString::cString(const char ch,
                 uint       optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    // This could cause a serious damage
    ASSERT(ch != cChar::getNullCharacter());

    // Init the object
    createEmptyString(optMem);
    // Create a minidump array
    character buf[2] = {(character)ch, cChar::getNullCharacter()};
    // Use operator = to copy to data (Avoids code-duplication)
    *this = cString(buf, optMem); // Create a cString with one character
}
#endif


cString::cString(const int32 number,
                 uint        base   /* = DefaultStringBase*/,
                 uint        optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    // Init the object
    createEmptyString(optMem);

    // Covert the number into positive value
    uint32 nm = t_abs(number);

    /* Check sign */
    if (number < 0)
    {
        concat(m_stringNegativeInteger);
    }

    /* Covert the number into a string */
    cString positiveString(nm, base);
    concat(positiveString);
}

cString::cString(const uint32 number,
                 uint        base   /* = DefaultStringBase*/,
                 uint        optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    ASSERT_MSG(base < cChar::getStrlen(m_baseStrip),
               XSTL_STRING("cString: There isn't engouth strip for conversion operation."));

    // Init members
    createEmptyString(optMem);

    uint digit = 0;  // The digit to convert
    uint x     = 0;  // The left number

    x = number;

    if (number == 0)
    {
        *this = cString(m_baseStrip[0]);
    }

    // Start with converting
    while (x > 0)
    {
        digit = x % base;
        x     = x / base;

        *this = cString(m_baseStrip[digit]) + *this;
    }
}

cString::cString(const int64 number,
                 uint        base   /* = DefaultStringBase*/,
                 uint        optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    // Init the object
    createEmptyString(optMem);

    // Covert the number into positive value
    uint64 nm = t_abs(number);

    /* Check sign */
    if (number < 0)
    {
        concat(m_stringNegativeInteger);
    }

    /* Covert the number into a string */
    cString positiveString(nm, base);
    concat(positiveString);
}

cString::cString(const uint64 number,
                 uint        base   /* = DefaultStringBase*/,
                 uint        optMem /* = DefaultStringPage*/) :
    m_buffer(NULL),
    m_stringLength(0)
{
    ASSERT_MSG(base < cChar::getStrlen(m_baseStrip),
               XSTL_STRING("cString: There isn't engouth strip for conversion operation."));

    // Init members
    createEmptyString(optMem);

    uint digit = 0;  // The digit to convert
    uint64 x   = 0;  // The left number

    x = number;

    if (number == 0)
    {
        *this = cString(m_baseStrip[0]);
    }

    // Start with converting
    while (x > 0)
    {
        digit = x % base;
        x     = x / base;

        *this = cString(m_baseStrip[digit]) + *this;
    }
}



cString::cString(const cString& other) :
    m_buffer(NULL),
    m_stringLength(0)
{
    // Copy the string
    m_buffer = new cSArray<character>(*(other.m_buffer));
    // Copy the string length
    m_stringLength = other.m_stringLength;
}

void cString::createEmptyString(uint optMem)
{
    // Assert memory lost
    ASSERT(m_buffer == NULL);
    // Reset string length
    m_stringLength = 0;
    // Create a buffer to hold the null-terminate character
    m_buffer = new cSArray<character>(1, optMem);
    // Puts the null-terminate character
    (*m_buffer)[0] = cChar::getNullCharacter();
}

cString::~cString()
{
    // Assertion for the usage in the cString object
    ASSERT(m_buffer != NULL);
    ASSERT(cChar::getStrlen(m_buffer->getBuffer()) == m_stringLength);

    // Destory the memory allocated.
    delete m_buffer;
}

uint cString::length() const
{
    // Do some assertion
    ASSERT_MSG((*m_buffer)[m_stringLength] == cChar::getNullCharacter(),
               XSTL_STRING("cString: Null-terminate character couldn't be founded."));
    ASSERT_MSG(cChar::getStrlen(m_buffer->getBuffer()) == m_stringLength,
               XSTL_STRING("cString: String length is invalid"));

    // Return the cached value of the string length
    return m_stringLength;
}

cString cString::mid(uint index, int size) const
{
    // Incase the size is negative...
    if (size <= 0) return cString();

    return part(index, index + size);
}

cString cString::part(uint index1, uint index2) const
{
    cString ret;

    // Check order
    if (index1 > index2)
        return ret;

    if (index1 > length()) index1 = length();
    if (index2 > length()) index2 = length();

    if (index1 == index2)
        return ret;

    // Create the return string
    ASSERT(ret.m_buffer != NULL);
    delete ret.m_buffer;
    ret.m_buffer = new cSArray<character>(m_buffer->getBuffer() + index1, index2 - index1 + 1, m_buffer->getPageSize());
    (*ret.m_buffer)[index2 - index1] = 0;
    ret.rearrangeStringVector();

    return ret;
}

cString cString::left(int size) const
{
    cString ret;

    /* Take care to egde */
    if (size > (int)length())
    {
        size = (int)length();
    }
    if (size <= 0)
    {
        return ret;
    }

    /* Copy the array */
    ASSERT(ret.m_buffer != NULL);
    delete ret.m_buffer;
    ret.m_buffer = new cSArray<character>(m_buffer->getBuffer(), size + 1, m_buffer->getPageSize());
    (*ret.m_buffer)[size] = 0;
    ret.rearrangeStringVector();

    return ret;
}

cString cString::right(int size) const
{
    cString ret;

    if (size > (int)length())
    {
        size = (int)length();
    }
    if (size <= 0)
    {
        return cString();
    }

    /* Create the return string */
    /* Copy the array */
    return cString(m_buffer->getBuffer() + length() - size, m_buffer->getPageSize());
}

void cString::concat(const cString& other)
{
    // Delete all unnessesry output, including NULL terminate character.
    m_buffer->changeSize(length());

    // Expand the string buffer to the right size
    // Copy the information.
    (*m_buffer) += *(other.m_buffer);

    // Delete the other size temporary information.
    rearrangeStringVector();
}

cString::CompareType cString::compare(const character* other) const
{
    // Compare string
    int ret = cChar::strcmp(m_buffer->getBuffer(), other);

    // Translate comparing.
    if (ret < 0)
        return cString::LowerThan;
    if (ret >= 1)
        return cString::GreaterThan;

    return cString::EqualTo;
}

cString::CompareType cString::compare(const cString& other) const
{
    return compare(other.getBuffer());
}

cString::CompareType cString::icompare(const character* other) const
{
    // Compare string
    int ret = cChar::stricmp(m_buffer->getBuffer(), other);

    // Translate comparing.
    if (ret < 0)
        return cString::LowerThan;
    if (ret >= 1)
        return cString::GreaterThan;

    return cString::EqualTo;
}

cString::CompareType cString::icompare(const cString& other) const
{
    return icompare(other.getBuffer());
}

cString& cString::makeUpper()
{
    for (uint i = 0; i < length(); i++)
    {
        (*m_buffer)[i] = cChar::getUppercase((*m_buffer)[i]);
    }

    return *this;
}

cString& cString::makeLower()
{
    for (uint i = 0; i < length(); i++)
    {
        (*m_buffer)[i] = cChar::getLowercase((*m_buffer)[i]);
    }

    return *this;
}

cString& cString::trim()
{
    uint size = length();

    if (size == 0)
        return *this;

    /* Trim all the spaces at the beginning */
    uint index = 0, i;
    while ((index < length()) && (cChar::isBlanket(m_buffer->getBuffer()[index])))
    {
        index++;
    }

    size-=index;

    /* Find the end position */
    i = length() - 1;
    while ((i >= index) && (cChar::isBlanket(m_buffer->getBuffer()[i])))
    {
        i--;
        size--;
    }

    /* Copy the new string */
    if ((index > 0) || (size < length()))
    {
        *this = this->mid(index, (int)size);
    }

    return *this;
}

cString& cString::trimAllSpaces()
{
    // Remove beginng and ending characters.
    trim();

    bool firstSpace = true; // Is this is the first space character
    uint len = length(); // The length of this string
    cString ret;         // A temporary object

    for (uint i = 0; i < len; i++)
    {
        if (cChar::isBlanket((*m_buffer)[i]))
        {
            if (firstSpace)
            {
                firstSpace = false;
                ret+= (*m_buffer)[i];
            }
        } else
        {
            ret+= (*m_buffer)[i];
            firstSpace = true;
        }
    }

    // Copy the result
    return *this = ret;
}

uint cString::find(const cString& string, uint startIndex /* = 0 */) const
{
    uint i  = startIndex;
    uint ln = length();
    uint on = string.length();
    uint len = (ln - on) + 1;

    // Incase of empty string. Always match
    if (on == 0)
    {
        return startIndex;
    }

    // If this string doesn't have at least the number of characters 'string'
    // has, the search will probably failed. Returns the length of this string.
    if (ln < on)
        return ln;

    while (i < len)
    {
        /* Try to find a match */
        if ((*m_buffer)[i] == string[0])
        {
            /* Scan the rest of the string */
            uint j;
            for (j = i + 1;
                 ((j < ln) && (j < i + on) && (string[(int)(j - i)] == (*m_buffer)[j]));
                 j++)
            {
                /* Match is ok */
            }
            if (j == i + string.length())
                return i;
        }
        i++;
    }

    return ln;
}

uint cString::rfind(const cString& string,
                    uint startIndex) const
{
    uint i  = startIndex;
    uint ln = length();
    uint on = string.length();

    // Incase of empty string. Always match
    if (on == 0)
    {
        return startIndex;
    }

    // If the current string is empty, search failed.
    if (ln == 0)
        return 0;

    // If this string doesn't have at least the number of characters 'string'
    // has, the search will probably failed. Returns the length of this string.
    if (ln < on)
        return ln;

    // Set the postion to the end of the string
    if (i >= ln - on)
        i = ln - on;

    bool shouldExit = false;
    do
    {
        /* Try to find a match */
        if (m_buffer->getBuffer()[i] == string.m_buffer->getBuffer()[0])
        {
            /* Scan the reset of the string */
            uint j;
            for (j = i + 1;
                 ((j < ln) && (j < i + on) && (string.m_buffer->getBuffer()[j - i] == m_buffer->getBuffer()[j]));
                 j++)
            {
                /* Match is ok */
            }
            if (j == i + string.length())
                return i;
        }
        // Since i is uint...
        if (i == 0)
            shouldExit = true;
        else
            i--;
    } while (!shouldExit);

    return ln;
}

cString& cString::replace(const cString& findString,
                          const cString& replaceString)
{
    uint pos;
    while ((pos = find(findString)) != length())
    {
        // Replace this string
        *this = left(pos) +
                replaceString +
                right(length() - pos - findString.length());
    }

    return *this;
}

cList<cString> cString::split(const cString& divider) const
{
    cList<cString> ret;

    uint index     = 0;
    uint lastIndex = 0;

    do
    {
        index = find(divider, (uint)(index + 1));
        ret.append(part(lastIndex, (uint)index));
        lastIndex = index + divider.length(); // Ignore the string
    }
    while ((uint)(index) != length());

    return ret;
}

character* cString::getBuffer(const uint lockSize /* = 0*/)
{
    if (lockSize <= m_buffer->getSize())
    {
        return m_buffer->getBuffer();
    } else
    {
        /* Put some more free size in the buffer */
        m_buffer->changeSize(lockSize);
        return m_buffer->getBuffer();
    }
}

const character* cString::getBuffer() const
{
    return m_buffer->getBuffer();
}

void cString::rearrangeStringVector()
{
    // Calculate the length of the new string.
    m_stringLength = cChar::getStrlen(m_buffer->getBuffer());
    // Delete all unnessecry space
    m_buffer->changeSize(m_stringLength + 1);
}

cString cString::dup(const cString& object, uint number)
{
    cString ret;
    for (uint i = 0; i < number; i++)
    {
        ret+= object;
    }

    return ret;
}

cString cString::pad(const cString& object,
                     uint number,
                     character padding)
{
    if (object.length() >= number)
        return object;

    return object + dup(" ", number - object.length());
}

cString & cString::operator = (const cString& other)
{
    if (this != &other)
    {
        /* Copy the string */
        delete m_buffer;
        m_buffer = new cSArray<character>(*other.m_buffer);
        rearrangeStringVector();
    }
    return *this;
}

cString & cString::operator = (const character* other)
{
    // Using the cString assignment operator
    *this = cString(other);
    return *this;
}

bool cString::operator == (const cString& other) const
{
    if (this != &other)
    {
        return (compare(other) == cString::EqualTo);
    }
    return true;
}

bool cString::operator < (const cString& other) const
{
    return (icompare(other) == cString::LowerThan);
}

cString & cString::operator += (const cString& other)
{
    concat(other);
    return *this;
}

cString & cString::operator += (const character* string)
{
    return (*this)+= cString(string);
}

cString & cString::operator += (const character ch)
{
    if (ch == cChar::getNullCharacter())
    {
        // Are you sure?!
        // This could cause a serious damage
        ASSERT_FAIL(XSTL_STRING("cString+\\0 are you sure?!"));
        // Safety for release versions
        return *this;
    }

    // Fast insert character
    m_buffer->changeSize(m_buffer->getSize() + 1, true);
    (*m_buffer)[m_buffer->getSize() - 1] = cChar::getNullCharacter();
    (*m_buffer)[m_buffer->getSize() - 2] = ch;

    // Change string length...
    m_stringLength++;

    return *this;
}

cString cString::operator + (const cString& other) const
{
    return cString(*this) += other;
}

cString cString::operator + (const character* string) const
{
    return *this + cString(string);
}

cString cString::operator + (const character ch) const
{
    if (ch == cChar::getNullCharacter())
    {
        // Are you sure?!
        // This could cause a serious damage
        ASSERT_FAIL(XSTL_STRING("cString+\\0 are you sure?!"));
        // Safety for release versions
        return *this;
    }

    return cString(*this) += ch;
}


character& cString::operator[] (const int index)
{
    ASSERT(index >= 0);
    ASSERT(index <= (int)m_buffer->getSize());
    return (*m_buffer)[index];
}

character cString::operator [] (const int index) const
{
    ASSERT(index >= 0);
    ASSERT(index <= (int)m_buffer->getSize());
    return (*m_buffer)[index];
}

bool cString::isValid() const
{
    return true;
}

void cString::deserialize(basicInput& inputStream)
{
    *this = inputStream.readUnicodeNullString();
}

void cString::serialize(basicOutput& outputStream) const
{
    outputStream.writeUnicodeNullString(*this);
}
