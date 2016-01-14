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

#ifndef __TBA_XSTL_PARSER_BRACES_H
#define __TBA_XSTL_PARSER_BRACES_H

/*
 * braces.h
 *
 * The braces module 'eat' up braces until the current braces blocks ends.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/list.h"
#include "xStl/exceptions.h"

/*
 * When the braces has error, the exception is stored in this format
 */
class BracesException : public cException
{
public:
    /*
     * Exception constructor.
     *
     * founded - The character which has founded
     * excpected - The character which has excepected to be founded
     */
    BracesException(char * file, uint32 line, character founded, character excpected);

    // The character which has founded
    character m_founded;
    // The character which has excepected to be founded
    character m_excpected;
};

/*
 * Count down braces.
 */
class Braces {
public:
    // The different braces types
    struct BracesData {
        // Start braces character '[','(','{'
        character m_open;
        // Stop braces character ']',')','}'
        character m_close;
    };

    /*
     * Constructor. Prepare the braces block.
     *
     * braces - The first brace to be read
     *
     * Throw BracesException if the 'braces' is not open braces
     */
    Braces(character braces);

    /*
     * Eats up a character.
     *
     * data - The character to be eaten
     *
     * Return true if the braces block ended.
     * Throws BracesException exception if the braces doesn't match
     */
    bool eatCharacter(character data);

    /*
     * Return true if the character is one of the openning character
     */
    bool isOpenBrace(character data);

    /*
     * Return true if the character is one of the closer character
     */
    bool isCloseBrace(character data);

    /*
     * Return true if the open brace matches the close braces
     */
    bool isMatch(character open, character close);

    /*
     * Return the closer character
     */
    character getCloseCharacter(character open);

    /*
     * Returns the cache excpected closer character
     */
    character getNextCloserCharacer();

private:
    // Link list of the current braces state.
    cList<character> m_state;
    // Cache of the last braces character
    character m_stateCache;

    // The number of different braces
    enum { BRACES_COUNT = 3 };
    // The possiable different braces types
    static BracesData m_braces[BRACES_COUNT];
};

#endif // __TBA_XSTL_PARSER_BRACES_H
