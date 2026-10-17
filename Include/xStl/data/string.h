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

/*
 * string.h
 *
 * Implementation of the 'string' in the xStl system. Strings are wrappers
 * for the  C 'char*' pointers. The class copies and manage the memory of the
 * string. The class doesn't only wrap the standard C string, it also helps
 * with Unicode strings or any other non-default 'char' string.
 *
 * Author: Elad Raz <e@eladraz.com>
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

/*
 * cString class
 *
 * An extension to the cSArray<character> class which contains support for the
 * string operations. Strings are normal C strings with a character type
 * extension. Basically a string is an array of characters with a NULL terminating
 * character ('\0'). This class provides simple string operation operators
 * such as concatenating strings, taking part of a string or calculating the
 * length of the string.
 * The length of the string is a private member. The function getBuffer()
 * returns direct memory access to the string with the ability to
 * enlarge the memory allocated for the string. After changing the string the
 * application must call the 'rearrangeStringVector' to fix-up the string
 * in the memory.
 * The string class uses the 'page' mechanism of the cArray class to
 * optimize performance. The page is a simple way to avoid memory
 * allocation whenever concatenating or deleting part of the string.
 * See cArray class description for more information.
 *
 * NOTE: The addition of the NULL terminating character is required to be
 *       compatible with older C functions. The cost of appending the NULL
 *       character is not high and the benefit is larger.
 *
 * COMPATIBLE NOTE: This object can work with all standard C library functions
 *                  and with code written for it. See 'getBuffer()' function and
 *                  'rearrangeStringVector()' for more information.
 */
class cString : public cSerializedObject
{
public:
    // Normal constructors and destructor
    enum {
        // The default page for the string. See page declaration in the cArray
        // class description for more information.
        DefaultStringPage = 30,
        // The constructor of cString can translate 'uint' and 'int' to
        // strings. The alphabet of the conversion is the string in the
        // m_baseStrip static const member. For changing the strip compile
        // your own members and define the 'XSTL_UNIQUE_STRIP' in the
        // compilation process
        DefaultStringBase = 10
    };

    /*
     * The default constructor. Wrap a pointer to a string with a cString
     * object. The data of the string is copied to the class memory.
     *
     * string - The string to be wrapped. If this argument is NULL the class
     *          creates a new empty string object.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
    cString(const character* string = NULL,
            uint             optMem = DefaultStringPage);

    #ifdef XSTL_UNICODE
    /*
     * In case we compile the xStl with the UNICODE support (good choice!)
     * we added a default ASCII string constructor.
     *
     * string - The string to be wrapped. If this argument is NULL the class
     *          creates a new empty string object.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
    cString(const char* string,
            uint        optMem = DefaultStringPage);
    #endif

    /*
     * To allow easy communication with ASCII application the following
     * function translates between a unicode string and an ASCII string
     * in a safe way.
     * NOTE: The returned array includes the null-terminating character ('\0')
     */
    cSArray<char> getASCIIstring() const;

    /*
     * Explicitly returns a unicode-string.
     *
     * NOTE: The returned array includes the null-terminating unichar ('\0'L)
     */
    cSArray<unichar> getUnicodeString() const;

    /*
     * Creates a string object with a single character 'ch'
     *
     * ch     - The character to be inserted.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
    explicit cString(const character ch,
                     uint            optMem = DefaultStringPage);


    #ifdef XSTL_UNICODE
    /*
     * In case we compile the xStl with the UNICODE support (good choice!)
     * we added a default ASCII string constructor.
     *
     * string - The string to be wrapped. If this argument is NULL the class
     *          creates a new empty string object.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
    explicit cString(const char ch,
                     uint       optMem = DefaultStringPage);
    #endif


    /*
     * Creates a string object which represent 'number'
     *
     * number - The number to be translated.
     * base   - The base for the converter.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
     explicit cString(const int32 number,
                      uint        base = DefaultStringBase,
                      uint        optMem = DefaultStringPage);

    /*
     * Creates a string object which represent 'number'
     *
     * number - The number to be translated.
     * base   - The base for the converter.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
     explicit cString(const uint32 number,
                      uint         base = DefaultStringBase,
                      uint         optMem = DefaultStringPage);

    /*
     * Creates a string object which represent 'number'
     *
     * number - The number to be translated.
     * base   - The base for the converter.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
     explicit cString(const int64 number,
                      uint        base = DefaultStringBase,
                      uint        optMem = DefaultStringPage);

    /*
     * Creates a string object which represent 'number'
     *
     * number - The number to be translated.
     * base   - The base for the converter.
     * optMem - The page size to be used when handling the string.
     *
     * Throws out of memory exception.
     */
     explicit cString(const uint64 number,
                      uint         base = DefaultStringBase,
                      uint         optMem = DefaultStringPage);


    /*
     * Copy-constructor. Copies the memory of another string.
     */
    cString(const cString& other);

    /*
     * Destructor. Free up class memory.
     */
    ~cString();

    // String functions

    /*
     * Returns the length of the string.
     */
    uint length() const;

    /*
     * Retrieve copy of part from the string. The partial range starts from
     * 'index' location and has 'size' length. In case the function reaches
     * the end of the string the function stops (and doesn't throw an exception).
     *
     * index - The start location of the string
     * size  - The length for the string.
     *
     * Returns the partial (or a part from it) string from this string.
     */
    cString mid(uint index, int size) const;

    /*
     * Retrieve copy of part from the string. The partial range starts from
     * 'index1' location and ended 'index2' location. In case the function
     * reaches the end of the string the function stops (And don't throw
     * exception).
     *
     * index1 - The start location of the string
     * index2 - The end location of the string
     *
     * Returns the partial (or a part from it) string from this string.
     * The order of the index must be index1 < index2, else empty string will
     * be returned
     */
    cString part(uint index1, uint index2) const;

    /*
     * Return 'size' characters from the beginning of this string. If this
     * string ends before 'size' characters the function return the entire
     * string (without throwing any exception)
     *
     * size - The number of characters from the beginning of the string
     *
     * Returns the partial (or a part from it) string from this string.
     */
    cString left(int size) const;

    /*
     * Return 'size' characters from the end of this string. If this string
     * ends before 'size' characters the function returns the entire string
     * (without throwing any exception)
     *
     * size - The number of characters from the end of the string
     *
     * Returns the partial (or a part from it) string from this string.
     */
    cString right(int size) const;

    /*
     * Connect 'other' string to the end of this string
     *
     * other - The string to concat.
     */
    void concat(const cString & other);

    /*
     * The compare data type for strings.
     */
    enum CompareType {
        // The string is lower than other
        LowerThan   = -1,
        // The string is equal to other
        EqualTo     = 0,
        // The string is greater than other
        GreaterThan = 1
    };

    /*
     * Compare two strings case-sensitively. The return value indicates
     * whether the string is less than, greater than or equal to the 'other'
     * string
     *
     * other - cString object or character* array.
     *
     * Returns CompareType object which describes the comparison.
     */
    CompareType compare(const character* other) const;
    CompareType compare(const cString& other) const;

    /*
     * Compare two strings and ignore uppercase. The return value indicate
     * whether the string is less than, greater than or equal to the 'other'
     * string
     *
     * other - cString object or character* array.
     *
     * Returns CompareType object which describes the comparison.
     */
    CompareType icompare(const character* other) const;
    CompareType icompare(const cString& other) const;

    /*
     * Makes the string to be all upper-case string.
     *
     * Return this object
     */
    cString& makeUpper();

    /*
     * Makes the string to be all lower-case string.
     *
     * Return this object
     */
    cString& makeLower();

    /*
     * Makes the string without all leading and trailing blank characters at
     * the beginning and the end of the string.
     */
    cString& trim();

    /*
     * Returns a string without all spaces blocks inside the string and 'trim'
     * the string. A space-block is defined as more than one blank character.
     * Example calling cString("  a    b elad   hj").trimAllSpaces() will result
     * in cString("a b elad hj").
     */
    cString& trimAllSpaces();

    /*
     * Try to find sub-string inside the string. The function returns the location
     * of the string or the length of the string if the text couldn't be found.
     *
     * string     - The string to look for
     * startIndex - The starting position for the search, should be lower than
     *              length()
     *
     * Return the position of the string inside the string
     * Return the length of the string if the string is not present.
     * If the string is empty, then 'find' will return the length of the string,
     * which is 0.
     */
    uint find(const cString& string,
              uint startIndex = 0) const;

    /*
     * Try to find sub-string inside the string starting with the end of the
     * current string.
     *
     * string     - The string to look for
     * startIndex - The starting position for the search, should be lower than
     *              length()
     *
     * Return the position of the string inside the string
     * Return the length of the string if the string is not present.
     *
     * If the string is empty, then 'find' will return the length of the string,
     * which is 0.
     */
    uint rfind(const cString& string,
               uint startIndex = MAX_UINT) const;

    /*
     * Try to find strings which match the 'find' string and replace them with
     * 'replace' string.
     *
     * findString    - The string to look for
     * replaceString - The string to substitute in place of 'find'.
     */
    cString& replace(const cString& findString,
                     const cString& replaceString);

    /*
     * Splits a string into link-list of strings. The strings are separated by the
     * 'divider' string. For example splitting a string with the "\n" string will
     * separate the string into lines.
     * Second example: cString("a=13&b=5&c=2").split("&") will return:
     * ["a=3","b=5","c=2"]
     *
     * divider - The string to be matched using 'find' in the string.
     *
     * Returns link-list of string. The link-list should include at least one
     * string. If the link-list has only one item then it will be equal to this
     * string.
     */
    cList<cString> split(const cString& divider) const;

    /*
     * Returns a pointer to the string buffer. The pointer is a direct pointer to
     * the class object data. Any change of the returned buffer will cause the
     * string to change. This function allows compatibility with older code
     * written for libC. The lockSize allows increasing the length of the
     * returned buffer and allows expanding the string in memory without the
     * worry about overflows. After changing (and only changing) the data in the
     * buffer a call to 'rearrangeStringVector()' is required.
     *
     * lockSize - The total number of bytes for the string buffer. If this
     *            number is lower than 'length()' the return buffer is with
     *            'length()' size.
     *
     * See 'rearrangeStringVector()' for more information.
     */
    character* getBuffer(const uint lockSize = 0);

    /*
     * Return the content of the string. This function is provided as
     * compatibility with the older C code.
     */
    const character* getBuffer() const;

    /*
     * Recalculate the buffer needed to store the string, after the call to
     * non-const 'getBuffer()' function and changing the length of the string.
     *
     * The call to this function is required after changing the buffer data!
     */
    void rearrangeStringVector();

    /*
     * Duplicate a string the number of times specified in the 'number'
     * argument.
     *
     * object - The object to be duplicated
     * number - The number of times for the duplication
     */
    static cString dup(const cString& object, uint number);

    /*
     * Pad a string to a minimum size of 'number' characters. If the string is
     * above 'number' character then the return string is the same as 'object'
     * otherwise, object is padded with 'padding' character to be at size of
     * 'number'
     *
     * object  - The string to be padded
     * number  - The number of characters to pad to
     * padding - The padding character
     *
     * Return a padding string
     */
    static cString pad(const cString& object,
                       uint number,
                       character padding = XSTL_CHAR(' '));

    // Operators

    /*
     * Assignment operator. Copy the string which is represented by 'other' into
     * this string object.
     *
     * other - cString/string object
     */
    cString& operator = (const cString& other);
    cString& operator = (const character* other);


    /*
     * Return true if the two strings are equal. This function is equivalent to the
     * 'compare() == 0' function.
     */
    bool operator == (const cString& other) const;

    /*
     * Return true if this string is alphabetically less than the 'other' string.
     * This means that the comparison is case-insensitive.
     */
    bool operator < (const cString& other) const;

    /*
     * Implement operator >, !=, >=, <=
     */
    MAKE_SIMPLE_OPERATORS(cString);

    /*
     * Concatenating operators. Adding string with normal operator + or +=. The
     * strings can be either cString strings or character* strings or single
     * characters.
     */
    cString& operator += (const cString& other);
    cString  operator +  (const cString& other) const;
    cString& operator += (const character* string);
    cString  operator +  (const character* string) const;
    cString  operator +  (const character ch) const;
    cString& operator += (const character ch);

    character & operator [] (const int index);
    character operator [] (const int index) const;

    cString& operator << (const cString &other) { concat(other); return *this; }
    cString& operator << (const character *   other) { concat(cString(other)); return *this; }
    cString& operator << (const int32 other) { concat(cString(other)); return *this; }
    cString& operator << (const uint32 other)
    { concat(cString(other)); return *this; }


    // The string "-" which indicates that the number is negative
    static const character* m_stringNegativeInteger;
    // The strip for the character
    static const character* m_baseStrip;

    /*
     * See cSerializedObject::isValid
     * Return true
     */
    virtual bool isValid() const;

    /*
     * See cSerializedObject::deserialize
     * Read a unicode version of the string
     */
    virtual void deserialize(basicInput& inputStream);

    /*
     * See cSerializedObject::serialize
     * Write a unicode version of the string
     */
    virtual void serialize(basicOutput& outputStream) const;

private:
    // The friend string class testing.
    friend class cTestString;

    /*
     * Private function. Used in the constructors. Creates an empty string and
     * initializes all members. The function assumes all members are uninitialized
     * so memory will not be allocated and therefore will not be freed.
     *
     * optMem - The page size to be used when handling the string.
     */
    void createEmptyString(uint optMem);

    // The array of character for the string
    cSArray<character>* m_buffer;

    // The length of the string
    uint m_stringLength;

    // TODO:
    //static const character* EmptyString;
};

#endif // __TBA_STL_STRING_H

