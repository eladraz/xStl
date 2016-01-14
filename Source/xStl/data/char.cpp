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
 * char.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/assert.h"
#include "xStl/data/char.h"
#include "xStl/utils/algorithm.h"
#include <string.h>
#include <wchar.h>

#ifdef XSTL_WINDOWS
    // Disable the deprecated warnning. We are using these API in a safe manner
    #pragma warning(push)
    #pragma warning(disable:4996)
#endif

character cChar::getNullCharacter()
{
    return XSTL_CHAR('\0');
}

/*
 * An independent implementation of wcslen
 */
size_t alt_wcslen(const unichar *string)
{
    const unichar *tempStr = string;
    while (*(tempStr++));
    return (size_t)(tempStr - string - 1);
}

uint cChar::getStrlen(const character* string)
{
    ASSERT(string != NULL);

    #ifndef XSTL_UNICODE
        // Non-unicode
        return (uint)strlen(string);
    #else
        // UNICODE
        return (uint)alt_wcslen((const unichar*)string);
    #endif
}

character cChar::getUppercase(character ch)
{
    if ((ch >= XSTL_CHAR('a')) && (ch <= XSTL_CHAR('z')))
    {
        return ((ch - XSTL_CHAR('a')) + XSTL_CHAR('A'));
    }
    // TODO! Add more languages!
    return ch;
}

character cChar::getLowercase(character ch)
{
    if ((ch >= XSTL_CHAR('A')) && (ch <= XSTL_CHAR('Z')))
    {
        return ((ch - XSTL_CHAR('A')) + XSTL_CHAR('a'));
    }
    // TODO! Add more languages!
    return ch;
}

bool cChar::isBlanket(character ch)
{
    return ((ch == XSTL_CHAR(' ')) || (ch == XSTL_CHAR('\t')));
}

bool cChar::isNewLine(character ch)
{
    return ((ch == XSTL_CHAR('\r')) || (ch == XSTL_CHAR('\n')));
}

bool cChar::isDigit(character ch)
{
    return ((ch >= XSTL_CHAR('0')) && (ch <= XSTL_CHAR('9')));
}

bool cChar::isBraces(character ch)
{
    return (ch == XSTL_CHAR('(')) || (ch == XSTL_CHAR(')')) ||
           (ch == XSTL_CHAR('[')) || (ch == XSTL_CHAR(']')) ||
           (ch == XSTL_CHAR('{')) || (ch == XSTL_CHAR('}'));
}

bool cChar::isSeperator(character ch)
{
    return (ch == XSTL_CHAR('.')) ||
           (ch == XSTL_CHAR(',')) ||
           (ch == XSTL_CHAR(';'));
}

uint cChar::getDigit(character ch)
{
    ASSERT(isDigit(ch));
    return (ch - XSTL_CHAR('0'));
}

bool cChar::isLetter(character ch)
{
    // TODO! Unicode languages
    return (((ch >= XSTL_CHAR('a')) && (ch <= XSTL_CHAR('z'))) ||
             ((ch >= XSTL_CHAR('A')) && (ch <= XSTL_CHAR('Z'))));
}

bool cChar::isPrintable(character ch)
{
    return (ch >= 0x20) && (ch < 0x80);
    // TODO! ADD ALL UNICODE LANGUAGES IN HERE!
}

int cChar::strcmp(const character* string1, const character* string2)
{
    ASSERT(string1 != NULL);
    ASSERT(string2 != NULL);

    #ifndef XSTL_UNICODE
        // Non-unicode
        return ::strcmp(string1, string2);
    #else
        // UNICODE
        return ::wcscmp((const wchar_t*)string1, (const wchar_t*)string2);
    #endif
}

int cChar::stricmp(const character* string1, const character* string2)
{
    ASSERT(string1 != NULL);
    ASSERT(string2 != NULL);

    #ifndef XSTL_UNICODE
        // Non-unicode
        #ifdef XSTL_LINUX
            return ::strcasecmp(string1, string2);
        #else
            return ::_stricmp(string1, string2);
        #endif
    #else
        // UNICODE
        #ifdef XSTL_LINUX
            #ifdef XSTL_MACOSX
                // TODO!
                return ::wcscmp((const wchar_t*)string1, (const wchar_t*)string2);
            #else
                // Most Linuxs
                return wcscasecmp((const wchar_t*)string1, (const wchar_t*)string2);
            #endif // XSTL_MACOSX
        #else
            return ::_wcsicmp((const wchar_t*)string1, (const wchar_t*)string2);
        #endif
    #endif
}

void cChar::strncpy(character* strDest,
                    const character* strSource,
                    uint count)
{
    ASSERT(strDest != NULL);
    ASSERT(strSource != NULL);
    ASSERT(count > 0);

    #ifndef XSTL_UNICODE
        // Non-unicode
        ::strncpy(strDest, strSource, count);
    #else
        // UNICODE
        ::wcsncpy((wchar_t*)strDest, (const wchar_t*)strSource, count);
    #endif

    // Puts NULL character at the end of the string
    strDest[count - 1] = cChar::getNullCharacter();
}

void cChar::strappend(character* strDest,
                      const character* strSource,
                      uint count)
{
    ASSERT(strDest != NULL);
    ASSERT(strSource != NULL);
    ASSERT(count > 0);

    uint destLength = cChar::getStrlen(strDest);

    // Calculate the maximun amount of character that should be copied
    uint toBeCopied = count - destLength - 1;

    #ifndef XSTL_UNICODE
        // Non-unicode
        strncat(strDest, strSource, toBeCopied);
    #else
        // UNICODE
        wcsncat((wchar_t*)strDest, (const wchar_t*)strSource, toBeCopied);
    #endif

    // Puts NULL character at the end of the string
    strDest[count - 1] = cChar::getNullCharacter();
}

char cChar::covert2Ascii(const unichar ch)
{
    // Translate
    if (ch < 0x0080)
    {
        // Known value...
        return (char)(ch);
    } else
    {
        // Unknown value.
        return '?';
    }
}

void cChar::covert2Ascii(char* output,
                         uint  outputLength,
                         const unichar* string)
{
    ASSERT(string != NULL);
    ASSERT(output != NULL);
    ASSERT(outputLength > 0);

    // Unicode
    uint len = t_min((uint)alt_wcslen((const unichar*)string) + 1, outputLength);
    for (uint i = 0; i < len; i++)
    {
        output[i] = covert2Ascii(string[i]);
    }
    output[len - 1] = '\0';
}

void cChar::covert2string(character*  output,
                          uint        count,
                          const void* string,
                          int         stringCharSize)
{
    ASSERT(string != NULL);
    ASSERT(output != NULL);

    if (stringCharSize == sizeof(character))
    {
        cChar::strncpy(output, (character*)string, count);
    }
    else
    {
        for (uint i = 0; i < count; i++)
        {
            switch (stringCharSize)
            {
            case 1: output[i] = ((char*)string)[i]; break;
#ifdef XSTL_UNICODE
            case 2: output[i] = (character)(((uint16*)string)[i]); break;
            case 4: output[i] = (character)(((uint32*)string)[i]); break;
#else
            case 2: output[i] = covert2Ascii((character)(((uint16*)string)[i])); break;
            case 4: output[i] = covert2Ascii((character)(((uint32*)string)[i])); break;
#endif // XSTL_UNICODE
            default:
                CHECK_FAIL();
            }

        }
        output[count - 1] = '\0';
    }
}

#ifdef XSTL_WINDOWS
    // Restore the warning levels
    #pragma warning(pop)
#endif

