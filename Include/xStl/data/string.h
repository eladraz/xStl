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

#ifndef __TBA_STL_STRING_H
#define __TBA_STL_STRING_H

/**
 * @addtogroup xstl_data_strings
 * @{
 */

/**
 * @file string.h
 * @brief Implementation of the 'string' in the xStl system.
 *
 * @details Strings are wrappers for the C 'char*' pointers. The class copies
 *    and manage the memory of the string. The class doesn't only wrap the
 *    standard C string, it also helps with Unicode strings or any other
 *    non-default 'char' string.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/data/array.h"
#include "xStl/data/sarray.h"
#include "xStl/data/list.h"
#include "xStl/data/char.h"
#include "xStl/data/serializedObject.h"
#include "xStl/stream/basicIO.h"

// Forward declaration for the test class
class cTestString;

/**
 * @brief A string of characters, with a NULL terminating character.
 *
 * @details An extension to the cSArray\<character\> class which contains
 *    support for the
 *   string operations. Strings are normal C strings with a character type
 *   extension. Basically a string is an array of characters with a NULL
 *   terminating
 *   character ('\\0'). This class provides simple string operation operators
 *   such as concatenating strings, taking part of a string or calculating the
 *   length of the string.
 *   The length of the string is a private member. The function getBuffer()
 *   returns direct memory access to the string with the ability to
 *   enlarge the memory allocated for the string. After changing the string the
 *   application must call the 'rearrangeStringVector' to fix-up the string
 *   in the memory.
 *   The string class uses the 'page' mechanism of the cArray class to
 *   optimize performance. The page is a simple way to avoid memory
 *   allocation whenever concatenating or deleting part of the string.
 *   See cArray class description for more information.
 *
 * COMPATIBLE NOTE: This object can work with all standard C library functions
 * and with code written for it. See 'getBuffer()' function and
 * 'rearrangeStringVector()' for more information.
 *
 * @note The addition of the NULL terminating character is required to be
 *    compatible with older C functions. The cost of appending the NULL
 *    character is not high and the benefit is larger.
 */
class cString : public cSerializedObject
{
public:
    /**
     * @brief Normal constructors and destructor
     */
    enum {
        /**
         * @brief The default page for the string.
         *
         * @details See page declaration in the cArray class description for
         *    more information.
         */
        DefaultStringPage = 30,
        /**
         * @brief The constructor of cString can translate 'uint' and 'int' to
         *    strings.
         *
         * @details The alphabet of the conversion is the string in the
         *    m_baseStrip static const member. For changing the strip compile
         *    your own members and define the 'XSTL_UNIQUE_STRIP' in the
         *    compilation process
         */
        DefaultStringBase = 10
    };

    /**
     * @brief The default constructor. Wrap a pointer to a string with a cString
     *    object.
     *
     * @details The data of the string is copied to the class memory.
     *
     * Throws out of memory exception.
     *
     * @param string The string to be wrapped. If this argument is NULL the
     *    class creates a new empty string object.
     * @param optMem The page size to be used when handling the string.
     */
    cString(const character* string = NULL,
            uint             optMem = DefaultStringPage);

    #ifdef XSTL_UNICODE
    /**
     * @brief In case we compile the xStl with the UNICODE support (good
     *    choice!) we
     *   added a default ASCII string constructor.
     *
     * @details Throws out of memory exception.
     *
     * @param string The string to be wrapped. If this argument is NULL the
     *    class creates a new empty string object.
     * @param optMem The page size to be used when handling the string.
     */
    cString(const char* string,
            uint        optMem = DefaultStringPage);
    #endif

    /**
     * @brief To allow easy communication with ASCII application the following
     *    function
     *   translates between a unicode string and an ASCII string in a safe way.
     *
     * @note The returned array includes the null-terminating character ('\\0')
     * @return The ASCII string, including the null-terminating character.
     */
    cSArray<char> getASCIIstring() const;

    /**
     * @brief Explicitly returns a unicode-string.
     *
     * @note The returned array includes the null-terminating unichar ('\\0'L)
     * @return The unicode string, including the null-terminating character.
     */
    cSArray<unichar> getUnicodeString() const;

    /**
     * @brief Creates a string object with a single character 'ch'
     *
     * @details Throws out of memory exception.
     *
     * @param ch The character to be inserted.
     * @param optMem The page size to be used when handling the string.
     */
    explicit cString(const character ch,
                     uint            optMem = DefaultStringPage);


    #ifdef XSTL_UNICODE
    /**
     * @brief Creates a string object with a single ASCII character. Available
     *    only when xStl is compiled with UNICODE support.
     *
     * Throws an out of memory exception.
     *
     * @param ch The ASCII character to be inserted.
     * @param optMem The page size to be used when handling the string.
     */
    explicit cString(const char ch,
                     uint       optMem = DefaultStringPage);
    #endif


    /**
     * @brief Creates a string object which represent 'number'
     *
     * @details Throws out of memory exception.
     *
     * @param number The number to be translated.
     * @param base The base for the converter.
     * @param optMem The page size to be used when handling the string.
     */
     explicit cString(const int32 number,
                      uint        base = DefaultStringBase,
                      uint        optMem = DefaultStringPage);

    /**
     * @brief Creates a string object which represent 'number'
     *
     * @details Throws out of memory exception.
     *
     * @param number The number to be translated.
     * @param base The base for the converter.
     * @param optMem The page size to be used when handling the string.
     */
     explicit cString(const uint32 number,
                      uint         base = DefaultStringBase,
                      uint         optMem = DefaultStringPage);

    /**
     * @brief Creates a string object which represent 'number'
     *
     * @details Throws out of memory exception.
     *
     * @param number The number to be translated.
     * @param base The base for the converter.
     * @param optMem The page size to be used when handling the string.
     */
     explicit cString(const int64 number,
                      uint        base = DefaultStringBase,
                      uint        optMem = DefaultStringPage);

    /**
     * @brief Creates a string object which represent 'number'
     *
     * @details Throws out of memory exception.
     *
     * @param number The number to be translated.
     * @param base The base for the converter.
     * @param optMem The page size to be used when handling the string.
     */
     explicit cString(const uint64 number,
                      uint         base = DefaultStringBase,
                      uint         optMem = DefaultStringPage);


    /**
     * @brief Copy-constructor. Copies the memory of another string.
     *
     * @param other The string to copy. The memory of the string is copied.
     */
    cString(const cString& other);

    /**
     * @brief Destructor. Free up class memory.
     */
    ~cString();

    // String functions

    /**
     * @brief Returns the length of the string.
     *
     * @return The length of the string.
     */
    uint length() const;

    /**
     * @brief Retrieve copy of part from the string.
     *
     * @details The partial range starts from 'index' location and has 'size'
     *    length. In case the function reaches the end of the string the
     *    function stops (and doesn't throw an exception).
     *
     * @param index The start location of the string
     * @param size The length for the string.
     * @return The partial (or a part from it) string from this string.
     */
    cString mid(uint index, int size) const;

    /**
     * @brief Retrieve copy of part from the string.
     *
     * @details The partial range starts from 'index1' location and ended
     *    'index2' location. In case the function reaches the end of the string
     *    the function stops (And don't throw exception).
     *
     * @param index1 The start location of the string
     * @param index2 The end location of the string
     * @return The partial (or a part from it) string from this string. The
     *    order of the index must be index1 \< index2, else empty string will be
     *    returned.
     */
    cString part(uint index1, uint index2) const;

    /**
     * @brief Return 'size' characters from the beginning of this string.
     *
     * @details If this string ends before 'size' characters the function return
     *    the entire string (without throwing any exception)
     *
     * @param size The number of characters from the beginning of the string
     * @return 'size' characters from the beginning of this string.
     */
    cString left(int size) const;

    /**
     * @brief Return 'size' characters from the end of this string.
     *
     * @details If this string ends before 'size' characters the function
     *    returns the entire string (without throwing any exception)
     *
     * @param size The number of characters from the end of the string
     * @return 'size' characters from the end of this string.
     */
    cString right(int size) const;

    /**
     * @brief Connect 'other' string to the end of this string
     *
     * @param other The string to concat.
     */
    void concat(const cString & other);

    /**
     * @brief The compare data type for strings.
     */
    enum CompareType {
        /**
         * @brief The string is lower than other
         */
        LowerThan   = -1,
        /**
         * @brief The string is equal to other
         */
        EqualTo     = 0,
        /**
         * @brief The string is greater than other
         */
        GreaterThan = 1
    };

    /**
     * @brief Compare two strings case-sensitively.
     *
     * @details The return value indicates whether the string is less than,
     *    greater than or equal to the 'other' string
     *
     * @param other cString object or character* array.
     * @return CompareType object which describes the comparison.
     */
    CompareType compare(const character* other) const;
    /**
     * @brief Compare two strings.
     *
     * The return value indicates whether the string is less than, greater than
     * or equal to the 'other' string.
     *
     * @param other The string to compare with.
     * @return A CompareType object which describes the comparison.
     */
    CompareType compare(const cString& other) const;

    /**
     * @brief Compare two strings and ignore uppercase.
     *
     * @details The return value indicate whether the string is less than,
     *    greater than or equal to the 'other' string
     *
     * @param other cString object or character* array.
     * @return CompareType object which describes the comparison.
     */
    CompareType icompare(const character* other) const;
    /**
     * @brief Compare two strings and ignore the case.
     *
     * The return value indicates whether the string is less than, greater than
     * or equal to the 'other' string.
     *
     * @param other The string to compare with.
     * @return A CompareType object which describes the comparison.
     */
    CompareType icompare(const cString& other) const;

    /**
     * @brief Makes the string to be all upper-case string.
     *
     * @return This object.
     */
    cString& makeUpper();

    /**
     * @brief Makes the string to be all lower-case string.
     *
     * @return This object.
     */
    cString& makeLower();

    /**
     * @brief Makes the string without all leading and trailing blank characters
     *    at the
     *   beginning and the end of the string.
     *
     * @return This object.
     */
    cString& trim();

    /**
     * @brief Returns a string without all spaces blocks inside the string and
     *    'trim'
     *   the string.
     *
     * @details A space-block is defined as more than one blank character.
     *    Example calling cString(" a b elad hj").trimAllSpaces() will result in
     *    cString("a b elad hj").
     *
     * @return A string without all spaces blocks inside the string and 'trim'
     *    the string.
     */
    cString& trimAllSpaces();

    /**
     * @brief Try to find sub-string inside the string.
     *
     * @details The function returns the location of the string or the length of
     *    the string if the text couldn't be found.
     *
     * @param string The string to look for
     * @param startIndex The starting position for the search, should be lower
     *    than length()
     * @return The position of the string inside the string.
     */
    uint find(const cString& string,
              uint startIndex = 0) const;

    /**
     * @brief Try to find sub-string inside the string starting with the end of
     *    the
     *   current string.
     *
     * @details If the string is empty, then 'find' will return the length of
     *    the string,
     *   which is 0.
     *
     * @param string The string to look for
     * @param startIndex The starting position for the search, should be lower
     *    than length()
     * @return The position of the string inside the string.
     */
    uint rfind(const cString& string,
               uint startIndex = MAX_UINT) const;

    /**
     * @brief Try to find strings which match the 'find' string and replace them
     *    with
     *   'replace' string.
     *
     * @param findString The string to look for
     * @param replaceString The string to substitute in place of 'find'.
     * @return This object.
     */
    cString& replace(const cString& findString,
                     const cString& replaceString);

    /**
     * @brief Splits a string into link-list of strings.
     *
     * @details The strings are separated by the 'divider' string. For example
     *    splitting a string with the "\\n" string will separate the string into
     *    lines. Second example: cString("a=13\&b=5\&c=2").split("\&") will
     *    return: ["a=3","b=5","c=2"]
     *
     * @param divider The string to be matched using 'find' in the string.
     * @return Link-list of string. The link-list should include at least one
     *    string. If the link-list has only one item then it will be equal to
     *    this string.
     */
    cList<cString> split(const cString& divider) const;

    /**
     * @brief Returns a pointer to the string buffer.
     *
     * @details The pointer is a direct pointer to the class object data. Any
     *    change of the returned buffer will cause the string to change. This
     *    function allows compatibility with older code written for libC. The
     *    lockSize allows increasing the length of the returned buffer and
     *    allows expanding the string in memory without the worry about
     *    overflows. After changing (and only changing) the data in the buffer a
     *    call to 'rearrangeStringVector()' is required.
     *
     * See 'rearrangeStringVector()' for more information.
     *
     * @param lockSize The total number of bytes for the string buffer. If this
     *    number is lower than 'length()' the return buffer is with 'length()'
     *    size.
     * @return A pointer to the string buffer.
     */
    character* getBuffer(const uint lockSize = 0);

    /**
     * @brief Return the content of the string.
     *
     * @details This function is provided as compatibility with the older C
     *    code.
     *
     * @return The content of the string.
     */
    const character* getBuffer() const;

    /**
     * @brief Recalculate the buffer needed to store the string, after the call
     *    to
     *   non-const 'getBuffer()' function and changing the length of the string.
     *
     * @details The call to this function is required after changing the buffer
     *    data!
     */
    void rearrangeStringVector();

    /**
     * @brief Duplicate a string the number of times specified in the 'number'
     *    argument.
     *
     * @param object The object to be duplicated
     * @param number The number of times for the duplication
     * @return The duplicated string.
     */
    static cString dup(const cString& object, uint number);

    /**
     * @brief Pad a string to a minimum size of 'number' characters.
     *
     * @details If the string is above 'number' character then the return string
     *    is the same as 'object' otherwise, object is padded with 'padding'
     *    character to be at size of 'number'
     *
     * @param object The string to be padded
     * @param number The number of characters to pad to
     * @param padding The padding character
     * @return A padding string.
     */
    static cString pad(const cString& object,
                       uint number,
                       character padding = XSTL_CHAR(' '));

    // Operators

    /**
     * @brief Assignment operator. Copy the string which is represented by
     *    'other' into
     *   this string object.
     *
     * @details other - cString/string object
     *
     * @param other The string to copy.
     * @return A reference to this string.
     */
    cString& operator = (const cString& other);
    /**
     * @brief Assignment operator. Copy a null-terminated string into this
     *    string object.
     *
     * @param other The string to copy.
     * @return A reference to this string.
     */
    cString& operator = (const character* other);


    /**
     * @brief Return true if the two strings are equal.
     *
     * @details This function is equivalent to the 'compare() == 0' function.
     *
     * @param other The string to compare with.
     * @return True if the strings are equal.
     */
    bool operator == (const cString& other) const;

    /**
     * @brief Return true if this string is alphabetically less than the 'other'
     *    string.
     *
     * @details This means that the comparison is case-insensitive.
     *
     * @return True if this string is alphabetically less than the 'other'
     *    string.
     * @param other The string to compare with.
     */
    bool operator < (const cString& other) const;

    /**
     * @brief Implement operator \>, !=, \>=, \<=
     */
    MAKE_SIMPLE_OPERATORS(cString);

    /**
     * @brief Concatenating operators. Adding string with normal operator + or
     *    +=.
     *
     * @details The strings can be either cString strings or character* strings
     *    or single characters.
     *
     * @param other The string to append.
     * @return A reference to this string.
     */
    cString& operator += (const cString& other);
    /**
     * @brief Return a new string which is this string followed by another
     *    string.
     *
     * @param other The string to append.
     * @return The concatenated string.
     */
    cString  operator +  (const cString& other) const;
    /**
     * @brief Append a null-terminated string to this string.
     *
     * @param string The string to append.
     * @return A reference to this string.
     */
    cString& operator += (const character* string);
    /**
     * @brief Return a new string which is this string followed by a
     *    null-terminated string.
     *
     * @param string The string to append.
     * @return The concatenated string.
     */
    cString  operator +  (const character* string) const;
    /**
     * @brief Return a new string which is this string followed by a character.
     *
     * @param ch The character to append.
     * @return The concatenated string.
     */
    cString  operator +  (const character ch) const;
    /**
     * @brief Append a character to this string.
     *
     * @param ch The character to append.
     * @return A reference to this string.
     */
    cString& operator += (const character ch);

    /**
     * @brief Return a reference to the character in a specific location.
     *
     * @param index The index of the character.
     * @return A reference to the character at @a index.
     */
    character & operator [] (const int index);
    /**
     * @brief Return the character in a specific location.
     *
     * @param index The index of the character.
     * @return The character at @a index.
     */
    character operator [] (const int index) const;

    /**
     * @brief Append a string to this string.
     *
     * @param other The string to append.
     * @return A reference to this string.
     */
    cString& operator << (const cString &other) { concat(other); return *this; }
    /**
     * @brief Append a null-terminated string to this string.
     *
     * @param other The string to append.
     * @return A reference to this string.
     */
    cString& operator << (const character *   other) { concat(cString(other)); return *this; }
    /**
     * @brief Append the decimal representation of a number to this string.
     *
     * @param other The number to append.
     * @return A reference to this string.
     */
    cString& operator << (const int32 other) { concat(cString(other)); return *this; }
    /**
     * @brief Append the decimal representation of a number to this string.
     *
     * @param other The number to append.
     * @return A reference to this string.
     */
    cString& operator << (const uint32 other)
    { concat(cString(other)); return *this; }


    /**
     * @brief The string "-" which indicates that the number is negative
     */
    static const character* m_stringNegativeInteger;
    /**
     * @brief The strip for the character
     */
    static const character* m_baseStrip;

    /**
     * @return True.
     * @see cSerializedObject::isValid
     */
    virtual bool isValid() const;

    /**
     * @brief Read a unicode version of the string
     *
     * @see cSerializedObject::deserialize
     * @param inputStream The stream to read from.
     */
    virtual void deserialize(basicInput& inputStream);

    /**
     * @brief Write a unicode version of the string
     *
     * @see cSerializedObject::serialize
     * @param outputStream The stream to write to.
     */
    virtual void serialize(basicOutput& outputStream) const;

private:
    // The friend string class testing.
    friend class cTestString;

    /**
     * @brief Private function. Used in the constructors.
     *
     * @details Creates an empty string and initializes all members. The
     *    function assumes all members are uninitialized so memory will not be
     *    allocated and therefore will not be freed.
     *
     * @param optMem The page size to be used when handling the string.
     */
    void createEmptyString(uint optMem);

    /**
     * @brief The array of character for the string
     */
    cSArray<character>* m_buffer;

    /**
     * @brief The length of the string
     */
    uint m_stringLength;

    // TODO:
    //static const character* EmptyString;
};


/** @} */
#endif // __TBA_STL_STRING_H

