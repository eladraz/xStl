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

/**
 * @addtogroup xstl_data_strings
 * @{
 */

/**
 * @file char.h
 * @brief The default character handler for the operating system.
 *
 * @details The OS can provide support for the normal 'char*' character which is
 *    the ANSI 8bit character or it can support some other type of character,
 *    most likely it will be 16bit chars which provides access to all characters
 *    in almost all languages. The char file defines the character set to be
 *    used when compiling the xStl package and all applications written for it.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"

/*
 * For UNICODE this typedef will be wchar...
 */
#if (defined(XSTL_WINDOWS) || defined(XSTL_CE) || defined(XSTL_LINUX) || defined(XSTL_NTDDK))
    #define unichar wchar_t
#else
    /**
     * @brief The unicode character type. Defined as `wchar_t` on Windows, CE,
     *    Linux and NTDDK, otherwise a 16 bit unsigned integer.
     */
    typedef uint16 unichar;
#endif
#ifndef XSTL_UNICODE
    /**
     * @brief The characters in the system are normal char*
     */
    typedef char character;
#else
    /**
     * @brief UNICODE The characters in the system are wide-char
     */
    typedef unichar character;
#endif // XSTL_UNICODE

/**
 * @brief The string typedef
 */
typedef character* lpString;

/**
 * @brief The endl character string
 */
extern character* endl;

/*
 * The XSTL_STRING macro converts ASCII strings into the string format declared
 * in the application
 */
#ifndef XSTL_UNICODE
    /**
     * @brief Convert an ASCII string literal into the string format of the
     *    application (narrow when XSTL_UNICODE is not defined).
     *
     * @param string The string literal.
     */
    #define XSTL_STRING(string) string
    /**
     * @brief Convert an ASCII character literal into the character format of
     *    the application (narrow when XSTL_UNICODE is not defined).
     *
     * @param ch The character literal.
     */
    #define XSTL_CHAR(ch) ch
#else
    /**
     * @brief UNICODE
     *
     * @param string The string literal.
     */
    #define XSTL_STRING(string) L##string
    /**
     * @brief Convert an ASCII character literal into the character format of
     *    the application (wide when XSTL_UNICODE is defined).
     *
     * @param ch The character literal.
     */
    #define XSTL_CHAR(ch)       L##ch
#endif

/**
 * @brief Helper for XSTL_NUM2STR. Stringify the macro argument without
 *    expanding it.
 *
 * @param x The token to stringify.
 */
#define XSTL_NUM2STR2(x) #x
/**
 * @brief Stringify the macro argument after macro expansion (for example
 *    `__LINE__`).
 *
 * @param x The token to expand and stringify.
 */
#define XSTL_NUM2STR(x) XSTL_NUM2STR2(x)
/**
 * @brief Helper for XSTL_WIDEN. Prefix the argument with `L`.
 *
 * @param x The string literal to widen.
 */
#define XSTL_WIDEN2(x) L ## x
/**
 * @brief Widen a string literal after macro expansion (for example `__FILE__`).
 *
 * @param x The string literal to widen.
 */
#define XSTL_WIDEN(x) XSTL_WIDEN2(x)

#ifndef XSTL_UNICODE
    /**
     * @brief Convert an XSTL string into an operating system constant string
     *    (narrow build).
     *
     * Used for compilers which are sensitive to typedef declarations.
     *
     * @param string The string to convert.
     */
    #define OS_CSTRING(string) (const char*)(string)
    /**
     * @brief The current source file name as an XSTL string literal.
     */
    #define XSTL_FILE __FILE__
    /**
     * @brief The current source line number as an XSTL string literal.
     */
    #define XSTL_LINE XSTL_NUM2STR(__LINE__)
#else
    /**
     * @brief Convert an XSTL string into an operating system constant string
     *    (wide build).
     *
     * Used for compilers which are sensitive to typedef declarations.
     *
     * @param string The string to convert.
     */
    #define OS_CSTRING(string) (const wchar_t*)(string)
    /**
     * @brief The current source file name as an XSTL string literal.
     */
    #define XSTL_FILE XSTL_WIDEN(__FILE__)
    /**
     * @brief The current source line number as an XSTL string literal.
     */
    #define XSTL_LINE XSTL_WIDEN(XSTL_NUM2STR(__LINE__))
#endif

/**
 * @brief Static functions for handling characters
 */
class cChar
{
public:
    /**
     * @brief Returns the null-terminating character for strings.
     *
     * @return The null-terminating character for strings.
     */
    static character getNullCharacter();

    /**
     * @brief Returns the string length for 'string'
     *
     * @param string The string to calculate the length on
     * @return The string length for 'string'.
     */
    static uint getStrlen(const character* string);

    /**
     * @brief Returns the uppercase character for a single character.
     *
     * @details The most important thing about this function is that two
     *    characters with
     *   the same meaning in the language (Such as 'a' and 'A' or in the Hebrew
     *   language 'Mem' and the final 'Mem' have the same meaning) will be equal
     *   with operator == after a call to this function.
     *
     * @param ch The character to be uppercase
     * @return The uppercase character for a single character.
     */
    static character getUppercase(character ch);

    /**
     * @brief Returns the lowercase character for a single character.
     *
     * @details See getUppercase note
     *
     * @param ch The character to be uppercase
     * @return The lowercase character for a single character.
     */
    static character getLowercase(character ch);

    /**
     * @brief Return true if the character 'ch' is printable.
     *
     * @return True if the character 'ch' is printable.
     * @param ch The character to test.
     */
    static bool isPrintable(character ch);

    /**
     * @brief Returns true if the character is a blank character.
     *
     * @details Blank is the space character ' ', the tab character '\\t' or any
     *    space in all languages.
     *
     * @return True if the character is a blank character.
     * @param ch The character to test.
     */
    static bool isBlank(character ch);

    /**
     * @brief Returns true if the character is '\\r' or '\\n' character.
     *
     * @return True if the character is '\\r' or '\\n' character.
     * @param ch The character to test.
     */
    static bool isNewLine(character ch);

    /**
     * @brief Returns true if the character is a digit character (0..9 or other
     *   languages)
     *
     * @return True if the character is a digit character (0..9 or other
     *    languages).
     * @param ch The character to test.
     */
    static bool isDigit(character ch);

    /**
     * @brief Returns true if the character is one of the braces characters,
     *    such as: {,
     *   } or [, ] or (,)
     *
     * @return True if the character is one of the braces characters, such as:
     *    {, } or [, ] or (,).
     * @param ch The character to test.
     */
    static bool isBraces(character ch);

    /**
     * @brief Returns true if the character is one of the line separator marks,
     *    such as:
     *   dot('.') comma(,) or semicolon(';')
     *
     * @return True if the character is one of the line separator marks, such
     *    as: dot('.') comma(,) or semicolon(';').
     * @param ch The character to test.
     */
    static bool isSeparator(character ch);

    /**
     * @brief Convert a digit character into its numeric value.
     *
     * @param ch The digit character.
     * @return The numeric value of the digit.
     * @throws cException in debug mode only, if the character is not a digit.
     */
    static uint getDigit(character ch);

    /**
     * @brief Return true if the character is a letter.
     *
     * @return True if the character is a letter.
     * @param ch The character to test.
     */
    static bool isLetter(character ch);

    /**
     * @brief The libC 'strcmp' function with the 'character' string.
     *
     * @details The function acts exactly as the libC function.
     *
     * Compare strings.
     *
     * @param string1 Null-terminated strings to compare
     * @param string2 Null-terminated strings to compare
     * @return Value: \< 0 string1 less than string2 0 string1 identical to
     *    string2 \> 0 string1 greater than string2.
     */
    static int strcmp(const character* string1, const character* string2);

    /**
     * @brief The libC 'stricmp' function with the 'character' string.
     *
     * @details The function acts exactly as the libC function.
     *
     * Compare case-insensitive strings. (Perform an uppercase comparison of
     * strings)
     *
     * @param string1 Null-terminated strings to compare
     * @param string2 Null-terminated strings to compare
     * @return Value: \< 0 string1 less than string2 0 string1 identical to
     *    string2 \> 0 string1 greater than string2.
     */
    static int stricmp(const character* string1, const character* string2);

    /**
     * @brief The libC 'strncpy' function with the 'character' string.
     *
     * @details The function acts exactly as the libC function.
     *
     * This is the libC definition:
     * Copy characters of one string to another. The strncpy function copies
     * the initial count characters of strSource to strDest and returns
     * strDest. If count is less than or equal to the length of strSource, a
     * null character is not appended automatically to the copied string. If
     * count is greater than the length of strSource, the destination string
     * is padded with null characters up to length count. The behavior of
     * strncpy is undefined if the source and destination strings overlap.
     *
     * @param strDest Destination string
     * @param strSource Source string
     * @param count Number of characters to be copied
     * @warning This version of strncpy puts a NULL terminating character at the
     *    'count' position to avoid buffer-overflows.
     */
    static void strncpy(character* strDest,
                        const character* strSource,
                        uint count);

    /**
     * @brief Appends one string to the other.
     *
     * @details The function, similar to 'strncat' appends the 'source' string
     *    to the 'destination' string. But this function prevents overflow by
     *    taking into account the size of the destination string.
     *
     * @param strDest Destination string
     * @param strSource Source string
     * @param count The number of characters of 'strDest'. NOTE: This is the
     *    number of characters and not the number of bytes!
     */
    static void strappend(character* strDest,
                          const character* strSource,
                          uint count);

    /**
     * @brief Translate a character to an ASCII character.
     *
     * @param ch The xStl character.
     * @return The ASCII value for the character or '?' for non-American
     *    characters.
     */
    static char convert2Ascii(const unichar ch);

    /**
     * @brief Convert string into normal ASCII (7-bit per character) string.
     *
     * @details The conversion may lose non-American characters.
     *
     * @param output Will be filled with the new ASCII string. The length of the
     *    string is limited by 'outputLength'. In case the string is bigger than
     *    the length of output, the string will be chopped.
     * @param outputLength The length in bytes that can be written to 'output'
     * @param string The original string
     */
    static void convert2Ascii(char* output,
                             uint  outputLength,
                             const unichar* string);

    /**
     * @brief Convert a unicode string to a format which is used by this xStl
     *   compilation.
     *
     * @param output Will be filled with unicode string content.
     * @param count Number of characters for 'output'
     * @param string The original string
     * @param stringCharSize Number of bytes per unicode string (1 for ASCII, 2
     *    for windows wchar_t, 4 for linux wchar_t)
     */
    static void convert2string(character*  output,
                              uint        count,
                              const void* string,
                              int         stringCharSize);
};


/** @} */
#endif // __TBA_STL_CHAR_H

