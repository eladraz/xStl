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

/**
 * @addtogroup xstl_parser
 * @{
 */

/**
 * @file braces.h
 * @brief The braces module 'eats' up braces until the current braces block
 *    ends.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/list.h"
#include "xStl/exceptions.h"

/**
 * @brief When the braces has error, the exception is stored in this format
 */
class BracesException : public cException
{
public:
    /**
     * @brief Exception constructor.
     *
     * @param found The character which was found
     * @param expected The character which was expected to be found
     * @param file The source file where the exception is thrown.
     * @param line The line in the source file where the exception is thrown.
     */
    BracesException(char * file, uint32 line,
                     character found, character expected);

    /**
     * @brief The character which was found
     */
    character m_found;
    /**
     * @brief The character which was expected to be found
     */
    character m_expected;
};

/**
 * @brief Count down braces.
 */
class Braces {
public:
    /**
     * @brief The different braces types
     */
    struct BracesData {
        /**
         * @brief Start braces character '[','(','{'
         */
        character m_open;
        /**
         * @brief Stop braces character ']',')','}'
         */
        character m_close;
    };

    /**
     * @brief Constructor. Prepare the braces block.
     *
     * @details Throw BracesException if the 'braces' is not open braces
     *
     * @param braces The first brace to be read
     */
    Braces(character braces);

    /**
     * @brief Eats up a character.
     *
     * @param data The character to be eaten
     * @return True if the braces block ended.
     * @throws cException (BracesException) if the braces doesn't match
     */
    bool eatCharacter(character data);

    /**
     * @brief Return true if the character is one of the opening characters
     *
     * @return True if the character is one of the opening characters.
     * @param data The character to test.
     */
    bool isOpenBrace(character data);

    /**
     * @brief Return true if the character is one of the closer characters
     *
     * @return True if the character is one of the closer characters.
     * @param data The character to test.
     */
    bool isCloseBrace(character data);

    /**
     * @brief Return true if the open brace matches the close braces
     *
     * @return True if the open brace matches the close braces.
     * @param open The opening brace character.
     * @param close The closing brace character.
     */
    bool isMatch(character open, character close);

    /**
     * @brief Return the closer character
     *
     * @return The closer character.
     * @param open The opening brace character.
     */
    character getCloseCharacter(character open);

    /**
     * @brief Returns the cached expected closer character
     *
     * @return The cached expected closer character.
     */
    character getNextCloserCharacter();

private:
    /**
     * @brief Link list of the current braces state.
     */
    cList<character> m_state;
    /**
     * @brief Cache of the last braces character
     */
    character m_stateCache;

    /**
     * @brief The number of different braces
     */
    enum { BRACES_COUNT = 3 };
    /**
     * @brief The possible different braces types
     */
    static BracesData m_braces[BRACES_COUNT];
};


/** @} */
#endif // __TBA_XSTL_PARSER_BRACES_H
