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

#ifndef __TBA_STL_CHAR_H
#define __TBA_STL_CHAR_H

/*
 * char.h
 *
 * The default character handler for the operating system. The OS can provide
 * support for the normal 'char*' character which is the ANSI 8bit character  or it
 * can support some other type of character, most likely it will be 16bit chars
 * which provides access to all characters in almost all languages.
 * The char file defines the character set to be used when compiling the xStl
 * package and all applications written for it.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

/*
 * For UNICODE this typedef will be wchar...
 */
#if (defined(XSTL_WINDOWS) || defined(XSTL_CE) || defined(XSTL_LINUX) || defined(XSTL_NTDDK))
    #define unichar wchar_t
#else
    typedef uint16 unichar;
#endif
#ifndef XSTL_UNICODE
    // The characters in the system are normal char*
    typedef char character;
#else
    // UNICODE
    // The characters in the system are wide-char
    typedef unichar character;
#endif // XSTL_UNICODE

// The string typedef
typedef character* lpString;

// The endl character string
extern character* endl;

/*
 * The XSTL_STRING macro converts ASCII strings into the string format declared
 * in the application
 */
#ifndef XSTL_UNICODE
    #define XSTL_STRING(string) string
    #define XSTL_CHAR(ch) ch
#else
    // UNICODE
    #define XSTL_STRING(string) L##string
    #define XSTL_CHAR(ch)       L##ch
#endif

/*
 * The OS_CSTRING converts XSTL strings into operating system const strings.
 * Used for compilers which are sensitive to typedef declarations.
 */
#define XSTL_NUM2STR2(x) #x
#define XSTL_NUM2STR(x) XSTL_NUM2STR2(x)
#define XSTL_WIDEN2(x) L ## x
#define XSTL_WIDEN(x) XSTL_WIDEN2(x)

#ifndef XSTL_UNICODE
    // ASCII
    #define OS_CSTRING(string) (const char*)(string)
    #define XSTL_FILE __FILE__
    #define XSTL_LINE XSTL_NUM2STR(__LINE__)
#else
    // UNICODE
    #define OS_CSTRING(string) (const wchar_t*)(string)
    #define XSTL_FILE XSTL_WIDEN(__FILE__)
    #define XSTL_LINE XSTL_WIDEN(XSTL_NUM2STR(__LINE__))
#endif

/*
 * class cChar
 *
 * Static functions for handling characters
 */
class cChar
{
public:
    /*
     * Returns the null-terminating character for strings.
     */
    static character getNullCharacter();

    /*
     * Returns the string length for 'string'
     *
     * string - The string to calculate the length on
     */
    static uint getStrlen(const character* string);

    /*
     * Returns the uppercase character for a single character.
     *
     * ch - The character to be uppercase
     *
     * The most important thing about this function is that two characters with
     * the same meaning in the language (Such as 'a' and 'A' or in the Hebrew
     * language 'Mem' and the final 'Mem' have the same meaning) will be equal
     * with operator == after a call to this function.
     */
    static character getUppercase(character ch);

    /*
     * Returns the lowercase character for a single character.
     *
     * ch - The character to be uppercase
     *
     * See getUppercase note
     */
    static character getLowercase(character ch);

    /*
     * Return true if the character 'ch' is printable.
     */
    static bool isPrintable(character ch);

    /*
     * Returns true if the character is a blank character. Blank is the space
     * character ' ', the tab character '\t' or any space in all languages.
     */
    static bool isBlank(character ch);

    /*
     * Returns true if the character is '\r' or '\n' character.
     */
    static bool isNewLine(character ch);

    /*
     * Returns true if the character is a digit character (0..9 or other languages)
     */
    static bool isDigit(character ch);

    /*
     * Returns true if the character is one of the braces characters, such as:
     * {, } or [, ] or (,)
     */
    static bool isBraces(character ch);

    /*
     * Returns true if the character is one of the line separator marks, such as:
     * dot('.') comma(,) or semicolon(';')
     */
    static bool isSeparator(character ch);

    /**
     * Try to convert a digit into a number.
     *
     * Throws an exception, only in debug mode, if the character is not a digit!
     */
    static uint getDigit(character ch);

    /*
     * Return true if the character is a letter.
     */
    static bool isLetter(character ch);

    /*
     * The libC 'strcmp' function with the 'character' string. The function
     * acts exactly as the libC function.
     *
     * Compare strings.
     *
     * string1 - Null-terminated strings to compare
     * string2 - Null-terminated strings to compare
     *
     * Returns value:
     * < 0 string1 less than string2
     * 0   string1 identical to string2
     * > 0 string1 greater than string2
     */
    static int strcmp(const character* string1, const character* string2);

    /*
     * The libC 'stricmp' function with the 'character' string. The function
     * acts exactly as the libC function.
     *
     * Compare case-insensitive strings. (Perform an uppercase comparison of strings)
     *
     * string1 - Null-terminated strings to compare
     * string2 - Null-terminated strings to compare
     *
     * Returns value:
     * < 0 string1 less than string2
     * 0   string1 identical to string2
     * > 0 string1 greater than string2
     */
    static int stricmp(const character* string1, const character* string2);

    /*
     * The libC 'strncpy' function with the 'character' string. The function
     * acts exactly as the libC function.
     *
     * This is the libC definition:
     *   Copy characters of one string to another. The strncpy function copies
     *   the initial count characters of strSource to strDest and returns
     *   strDest. If count is less than or equal to the length of strSource, a
     *   null character is not appended automatically to the copied string. If
     *   count is greater than the length of strSource, the destination string
     *   is padded with null characters up to length count. The behavior of
     *   strncpy is undefined if the source and destination strings overlap.
     *
     * IMPORTANT NOTE:
     *   This version of strncpy puts a NULL terminating character at the 'count'
     *   position to avoid buffer-overflows.
     *
     * strDest   - Destination string
     * strSource - Source string
     * count     - Number of characters to be copied
     */
    static void strncpy(character* strDest,
                        const character* strSource,
                        uint count);

    /*
     * Appends one string to the other. The function, similar to 'strncat'
     * appends the 'source' string to the 'destination' string. But this
     * function prevents overflow by taking into account the size of the
     * destination string.
     *
     * strDest   - Destination string
     * strSource - Source string
     * count     - The number of characters of 'strDest'.
     *             NOTE: This is the number of characters and not the number of
     *                   bytes!
     */
    static void strappend(character* strDest,
                          const character* strSource,
                          uint count);

    /*
     * Translate a character to an ASCII character.
     *
     * ch - The xStl character.
     *
     * Return the ASCII value for the character or '?' for non-American
     * characters
     */
    static char convert2Ascii(const unichar ch);

    /*
     * Convert string into normal ASCII (7-bit per character) string.
     * The conversion may lose non-American characters.
     *
     * output - Will be filled with the new ASCII string. The length of the
     *          string is limited by 'outputLength'. In case the string is
     *          bigger than the length of output, the string will be chopped.
     * outputLength - The length in bytes that can be written to 'output'
     * string - The original string
     */
    static void convert2Ascii(char* output,
                             uint  outputLength,
                             const unichar* string);

    /*
     * Convert a unicode string to a format which is used by this xStl
     * compilation.
     *
     * output - Will be filled with unicode string content.
     * count  - Number of characters for 'output'
     * string - The original string
     * stringCharSize - Number of bytes per unicode string (1 for ASCII, 2 for windows wchar_t, 4 for linux wchar_t)
     */
    static void convert2string(character*  output,
                              uint        count,
                              const void* string,
                              int         stringCharSize);
};

#endif // __TBA_STL_CHAR_H

