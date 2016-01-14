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
 * wildcardMatcher.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/wildcardMatcher.h"

bool cWildcardMatcher::match(const cString& string,
                             const cString& wildcardPattern,
                             bool isCaseSensetive)
{
    // Empty pattern is a special case
    if (wildcardPattern.length() == 0)
        return string.length() == 0;

    // Reads all character until a wildcard character...
    for (uint i = 0; i < wildcardPattern.length(); i++)
    {
        character ch = wildcardPattern[i];
        // Declared for switch '*' stack variables
        cString newString;
        cString secondPattern;
        uint j;
        switch (ch)
        {
        case XSTL_CHAR('?'):
            // Must be a character!
            if (string.length() <= i)
                return false;
            break;
        case XSTL_CHAR('*'):
            // This is a wildcard at the end: 'blablablabla' and 'bla*'
            // Eats everything.
            if (wildcardPattern.length() == (i+1))
                return true;

            // Test the case: 'blabla' and pattern 'blabla*a'
            if (string.length() == i)
                return false;

            // Now the recursive begins...
            // Get the partial strings 'blabla' and 'a'
            newString = string.right(string.length() - i);
            secondPattern = wildcardPattern.right(wildcardPattern.length() - i - 1);
            // Test all string characters
            for (j = 0; j < newString.length(); j++)
            {
                if (match(newString.right(newString.length() - j),
                          secondPattern))
                    return true;
            }
            return false;
            break;
        default:
            // Must be equal
            if (string.length() <= i)
                return false;
            if (isCaseSensetive)
            {
                if (string[i] != ch)
                    return false;
            } else
            {
                if (cChar::getUppercase(string[i]) != cChar::getUppercase(ch))
                    return false;
            }
        }
    }

    // All done!
    return true;
}
