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
 * Implementation of the 'string' in the xStl system. The string are wrappers
 * for the  C 'char*' pointers. The class copies and manage the memory of the
 * string. The class doesn't wrapped only the standard C string it also helps
 * the Unicode string or any other non-default 'char' string.
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
 * An extension to cSArray<character> class which contains support in the
 * strings operations. The string are normal C string with character type
 * extension. Basically a string is array of characters with a NULL terminate
 * character ('\0'). This class provide simple string operation operators
 * such as concating strings, taking part of a string or calculating the
 * length of the string.
 * The length of the string is private member. The function getBuffer()
 * return a direct memory access to the string with the ablitiy to
 * enlarge the memory allocated string. After changing the string the
 * application must call the 'rearrangeStringVector' to fix-up the string
 * in the memory.
 * The string class uses the 'page' mechanizm of the cArray class to
 * optimized permormance. The page is a simple way to avoid memory
 * allocation when-ever concating or deleting part of the string.
 * See cArray class description for more information.
 *
 * NOTE: The addition of the NULL terminate character are requiered to be
 *       compatible with older C functions. The cost of appending the NULL
 *       character is not so hard and the benefit are larger.
 *
 * COMPATIBLE NOTE: This object can work with all standard C library functions
 *                  and with code written for it. See 'getBuffer()' function and
 *                  'rearrangeStringVector()' for more information.
 */
class cString : public cSerializedObject
{
public:
    // Normal constructors and deconstructor
    enum {
        // The default page for the string. See page declartion in the cArray
        // class description for more information.
        DefaultStringPage = 30,
        // The constructor of cString can translate 'uint' and 'int' to
        // strings. The alpha-betic of the converstion is the string in the
        // m_baseStrip static const member. For changing the strip compile
        // your own members and define the 'XSTL_UNIQUE_STRIP' in the
        // compliation process
        DefaultStringBase = 10
    };

    /*
     * The default constructor. Wrap a pointer to a string with a cString
     * object. The data of the string are copied to the class memory.
     *
     * string - The string to be wrapped. If this argument is NULL the class
     *          creates a new empty string object.
     * optMem - The page size to be use when handling the string.
     *
     * Throws out of memory exception.
     */
    cString(const character* string = NULL,
            uint             optMem = DefaultStringPage);

    #ifdef XSTL_UNICODE
    /*
     * Incase we compile the xStl with the UNICODE support (good choice!)
     * we added a default ASCII string constructor.
     *
     * string - The string to be wrapped. If this argument is NULL the class
     *          creates a new empty string object.
     * optMem - The page size to be use when handling the string.
     *
     * Throws out of memory exception.
     */
    cString(const char* string,
            uint        optMem = DefaultStringPage);
    #endif

    /*
     * To allow easy communication with ASCII application the following
     * function translates between unicode string and an ASCII string
     * in a safe case.
     * NOTE: The return array include null-terminate character ('\0')
     */
    cSArray<char> getASCIIstring() const;

    /*
     * Explictly returns a unicode-string.
     *
     * NOTE: The return array include null-terminate unichar ('\0'L)
     */
    cSArray<unichar> getUnicodeString() const;

    /*
     * Creates a string object with a single character 'ch'
     *
     * ch     - The character to be inserted.
     * optMem - The page size to be use when handling the string.
     *
     * Throws out of memory exception.
     */
    explicit cString(const character ch,
                     uint            optMem = DefaultStringPage);


    #ifdef XSTL_UNICODE
    /*
     * Incase we compile the xStl with the UNICODE support (good choice!)
     * we added a default ASCII string constructor.
     *
     * string - The string to be wrapped. If this argument is NULL the class
     *          creates a new empty string object.
     * optMem - The page size to be use when handling the string.
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
     * base   - The base for the convertor.
     * optMem - The page size to be use when handling the string.
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
     * base   - The base for the convertor.
     * optMem - The page size to be use when handling the string.
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
     * base   - The base for the convertor.
     * optMem - The page size to be use when handling the string.
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
     * base   - The base for the convertor.
     * optMem - The page size to be use when handling the string.
     *
     * Throws out of memory exception.
     */
     explicit cString(const uint64 number,
                      uint         base = DefaultStringBase,
                      uint         optMem = DefaultStringPage);


    /*
     * Copy-constructor. Copies the memory of another stting.
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
     * Retervie copy of part from the string. The partial ranage starts from
     * 'index' location and has 'size' length. Incase the function reaches
     * the end of the string the function stops (And don't throw exception).
     *
     * index - The start location of the string
     * size  - The length for the string.
     *
     * Returns the partial (or a part from it) string from this string.
     */
    cString mid(uint index, int size) const;

    /*
     * Retervie copy of part from the string. The partial ranage starts from
     * 'index1' location and ended 'index2' location. Incase the function
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
        // The string is greather than other
        GreaterThan = 1
    };

    /*
     * Compare two strings with case sensetive. The return value indicate
     * whether the string is less than, greater than or equal to the 'other'
     * string
     *
     * other - cString object or character* array.
     *
     * Returns CompareType object which describes the compration.
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
     * Returns CompareType object which describes the compration.
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
     * Makes the string without all leading and lead-out blankes characters for
     * the beginning and the end of the string.
     */
    cString& trim();

    /*
     * Returns a string without all spaces blocks inside the string and 'trim'
     * the string. Space-block defined as more than one blank character.
     * Example calling cString("  a    b elad   hj").trimAllSpaces() will result
     * with cString("a b elad hj").
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
     * replaceString - The string to substitue in place of 'find'.
     */
    cString& replace(const cString& findString,
                     const cString& replaceString);

    /*
     * Splits a string into link-list of strings. The strings are traces by the
     * 'divider' string. For example spliting a string with the "\n" string will
     * seperate the string into lines.
     * Second exmpale: cString("a=13&b=5&c=2").split("&") will returned:
     * ["a=3","b=5","c=2"]
     *
     * divider - The string to be match using 'find' in the string.
     *
     * Returns link-list of string. The link-list should include at least one
     * string. If the link-list has only one item then it will be equal to this
     * string.
     */
    cList<cString> split(const cString& divider) const;

    /*
     * Returns a pointer to the string buffer. The pointer is direct pointer to
     * the class object data. Any change of the returned buffer will cause the
     * string to be change. This function allow compatiblities with older code
     * written to the libC. The lockSize allows to increase the length of the
     * return buffer and allow to expand the string in the memory without the
     * worry for overflows. After changing (And only changing) the data in the
     * buffer a call to 'rearrangeStringVector()' required.
     *
     * lockSize - The total number of bytes for the string buffer. If this
     *            number is lower the 'length()' the return buffer is with
     *            'length()' size.
     *
     * See 'rearrangeStringVector()' for more information.
     */
    character* getBuffer(const uint lockSize = 0);

    /*
     * Return the content of the string. This function is provide as
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
     * Duplicate one string as the number of times mentation in the 'number'
     * argument.
     *
     * object - The object to be duplicate
     * number - The number of times for the duplication
     */
    static cString dup(const cString& object, uint number);

    /*
     * Pad a string to a minimum size of 'number' characters. If the string is
     * above 'number' character then the return string is the same as 'object'
     * otherwise, object is padded with 'padding' character to be at size of
     * 'number'
     *
     * object  - The string to be pad
     * number  - The number of characters to be padding
     * padding - The padding character
     *
     * Return a padding string
     */
    static cString pad(const cString& object,
                       uint number,
                       character padding = XSTL_CHAR(' '));

    // Operators

    /*
     * Assignment operator. Copy the string which reperesnt in the 'other' into
     * this string object.
     *
     * other - cString/string object
     */
    cString& operator = (const cString& other);
    cString& operator = (const character* other);


    /*
     * Return true if the two string are equals. This function is equal to the
     * 'compare() == 0' function.
     */
    bool operator == (const cString& other) const;

    /*
     * Return true if the alpha-beit of this string is less than 'other' string
     * This means that the comparing is incase-sensetive.
     */
    bool operator < (const cString& other) const;

    /*
     * Implement operator >, !=, >=, <=
     */
    MAKE_SIMPLE_OPERATORS(cString);

    /*
     * Concating operators. Adding string with normal operator + or +=. The
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


    // The string "-" which indicate that the number is negative
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
     * Private function. Used at the constructors. Creates empty string and
     * init all members. The functions assumes all member are uninitiliazed
     * so memory will not allocated and therfore will not be free.
     *
     * optMem - The page size to be use when handling the string.
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

