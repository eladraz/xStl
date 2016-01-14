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
 * rc4.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/utils/algorithm.h"
#include "xStl/enc/encryption.h"
#include "xStl/enc/enc/rc4.h"

void cRC4::loadKey(const cBuffer& key)
{
    uint8 index1, index2;
    uint  i;

    // Reset the premutation entries.
    for (i = 0; i < RC4KEY_LEN; i++)
        m_premutation[i] = i;

    m_i = 0;
    m_j = 0;

    index1 = index2 = 0;
    for (i = 0; i < RC4KEY_LEN; i++)
    {
        index1 = i;
        index2 = (uint8)((key[i % key.getSize()] + m_premutation[index1] + index2) & 0xFF);
        t_swap(m_premutation[index1], m_premutation[index2]);
    }
}

void cRC4::encrypt(cBuffer& data)
{
    uint  length = data.getSize();
    uint8* buffer = data.getBuffer();

    for (uint i = 0; i < length; i++)
    {
        (*buffer)^= advance();
        buffer++;
    }
}

void cRC4::decrypt(cBuffer& data)
{
    encrypt(data);
}

void cRC4::strip(cBuffer& data)
{
    uint  length = data.getSize();
    uint8* buffer = data.getBuffer();

    for (uint i = 0; i < length; i++)
    {
        *buffer = advance();
        buffer++;
    }
}

uint8 cRC4::advance()
{
    m_i++;
    m_j = m_premutation[m_i] + m_j;
    t_swap(m_premutation[m_i], m_premutation[m_j]);

    return m_premutation[(m_premutation[m_i] + m_premutation[m_j]) & 0xFF];
}
