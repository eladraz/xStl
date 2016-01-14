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
 * braces.cpp
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/data/char.h"
#include "xStl/parser/braces.h"

Braces::BracesData Braces::m_braces[BRACES_COUNT];

BracesException::BracesException(char * file, uint32 line, character founded, character excpected) :
    cException(file, line, NULL, 0),
    m_founded(founded),
    m_excpected(excpected)
{
}

Braces::Braces(character braces) :
    m_stateCache(braces)
{
    m_braces[0].m_open = XSTL_CHAR('{');
    m_braces[1].m_open = XSTL_CHAR('[');
    m_braces[2].m_open = XSTL_CHAR('(');

    m_braces[0].m_close = XSTL_CHAR('}');
    m_braces[1].m_close = XSTL_CHAR(']');
    m_braces[2].m_close = XSTL_CHAR(')');

    if (!isOpenBrace(braces))
    {
        XSTL_THROW(BracesException, braces, '}');
    }
    m_state.append(braces);
}

bool Braces::eatCharacter(character data)
{
    // Tests that the list isn't empty
    ASSERT(m_state.begin() != m_state.end());

    if (isOpenBrace(data))
    {
        m_state.append(data);
        m_stateCache = data;
        return false;
    }

    if (isCloseBrace(data))
    {
        if (!isMatch(m_stateCache, data))
        {
            // Error at braces!
            XSTL_THROW(BracesException, data, getNextCloserCharacer());
        }

        // Pop the last braces
        cList<character>::iterator i = m_state.end();
        i.prev();
        ASSERT((*i) == m_stateCache);
        if (m_state.begin() == i)
        {
            // end of the block
            m_state.remove(i);
            return true;
        }

        cList<character>::iterator j = i;
        j.prev();
        m_state.remove(i);
        m_stateCache = *j;
    }

    // Nothing to do
    return false;
}

bool Braces::isOpenBrace(character open)
{
    for (uint i = 0; i < BRACES_COUNT; i++)
    {
        if (m_braces[i].m_open == open)
        {
            return true;
        }
    }
    return false;
}

bool Braces::isCloseBrace(character close)
{
    for (uint i = 0; i < BRACES_COUNT; i++)
    {
        if (m_braces[i].m_close == close)
        {
            return true;
        }
    }
    return false;
}

bool Braces::isMatch(character open, character close)
{
    ASSERT(isOpenBrace(open));
    ASSERT(isCloseBrace(close));

    for (uint i = 0; i < BRACES_COUNT; i++)
    {
        if (m_braces[i].m_open == open)
        {
            if (m_braces[i].m_close == close)
            {
                return true;
            }
            // else
            return false;
        }
    }

    // Cannot reach here
    CHECK(false);
    return false;
}

character Braces::getCloseCharacter(character open)
{
    ASSERT(isOpenBrace(open));

    for (uint i = 0; i < BRACES_COUNT; i++)
    {
        if (m_braces[i].m_open == open)
        {
            return m_braces[i].m_close;
        }
    }

    // Cannot reach here
    CHECK(false);
    return 0;
}

character Braces::getNextCloserCharacer()
{
    // Tests that the list isn't empty
    ASSERT(m_state.begin() != m_state.end());

    return getCloseCharacter(m_stateCache);
}
