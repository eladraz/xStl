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
 * HashDigest.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/array.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/HashDigest.h"


cHashDigest::cHashDigest()
{
    m_isValid = false;
}

void cHashDigest::reset()
{
    m_isValid = true;
    m_count_l = 0;
    m_count_h = 0;
    m_index = 0;
    // Reset the block and the digest
    m_blockSize = getDataLen() * sizeof(uint32);
    m_block.changeSize(m_blockSize);
    m_digest.changeSize(getDigestLen());
}

cHashDigest::cHashDigest(const cHashDigest& other)
{
    copyFrom(other);
}

cHashDigest& cHashDigest::operator = (const cHashDigest& other)
{
    if (this != &other)
    {
        copyFrom(other);
    }
    return *this;
}

void cHashDigest::copyFrom(const cHashDigest& other)
{
    m_digest = other.m_digest;
    m_block = other.m_block;

    m_count_l = other.m_count_l;
    m_count_h = other.m_count_h;
    m_index = other.m_index;

    m_isValid = other.m_isValid;
    m_blockSize = other.m_blockSize;
}

void cHashDigest::update(const void* buffer, const uint32 length)
{
    // Should I construct the object?
    if (!m_isValid)
        reset();

    uint8* buf = (uint8*)buffer;
    uint32 len = length;
    if (m_index)
    {
        /* Try to fill partial block */
        unsigned int left = m_blockSize - m_index;
        if (len < left)
        {
            cOS::memcpy(m_block.getBuffer() + m_index, buf, len);
            m_index+= len;
            return;
        }
        else
        {
            cOS::memcpy(m_block.getBuffer() + m_index, buf, left);
            HASHBlock(m_block.getBuffer());
            buf+= left;
            len-= left;
        }
    }
    while (len >= m_blockSize)
    {
        HASHBlock(buf);
        buf+= m_blockSize;
        len-= m_blockSize;
    }
    /* This assignment is intended */
    if ((m_index = len))
        /* buf leftovers */
        cOS::memcpy(m_block.getBuffer(), buf, len);
}

cBuffer cHashDigest::digest()
{
    cBuffer ret(getDigestLength());

    // Final the round
    cSmartPtr<cDigest> m_clone = clone();
    cHashDigest* ptr = (cHashDigest*)m_clone.getPointer();
    ptr->HASHFinal();

    uint8* p = ret.getBuffer();

    /* Endian indepented */
    for (unsigned int i = 0; i < getDigestLen(); i++, p+= sizeof(uint32))
    {
        writeUint32(p, ptr->m_digest[i]);
    }

    return ret;
}

uint cHashDigest::getDigestLength()
{
    return getDigestLen() * sizeof(uint32);
}

void cHashDigest::HASHFinal()
{
    cArray<uint32> data(getDataLen());
    unsigned int i;
    unsigned int words;

    i = m_index;
    /*
     * Set the first char of padding to 0x80.  This is safe since there is
     * always at least one byte free
     */
    m_block[i++] = 0x80;

    /* Fill rest of word */
    for( ; i & 3; i++)
        m_block[i] = 0;

    /* i is now a multiple of the word size 4 */
    words = i >> 2;
    for (i = 0; i < words; i++)
        data[i] = readUint32(m_block.getBuffer() + 4*i);

    if (words > (getDataLen()-2))
    {
        /*
         * No room for length in this block. Process it and
         * pad with another one
         */
        for (i = words ; i < getDataLen(); i++)
            data[i] = 0;

        HASHTransform(data.getBuffer());

        for (i = 0; i < (getDataLen()-2); i++)
            data[i] = 0;
    } else
    {
        for (i = words ; i < getDataLen() - 2; i++)
           data[i] = 0;
    }

    /*
     * Theres 512 = 2^9 bits in one block
     * Watch the endian order!
     */
    if (isLittleEndian()) {
        data[getDataLen()-1] = (m_count_h << 9) | (m_count_l >> 23);
        data[getDataLen()-2] = (m_count_l << 9) | (m_index << 3);
    } else {
        data[getDataLen()-2] = (m_count_h << 9) | (m_count_l >> 23);
        data[getDataLen()-1] = (m_count_l << 9) | (m_index << 3);
    }
    HASHTransform(data.getBuffer());
}


void cHashDigest::HASHBlock(const uint8* block)
{
    cArray<uint32> data(getDataLen());
    int i;

    /* Update block count */
    if (!++m_count_l)
        ++m_count_h;

    /* Endian independent conversion */
    for (i = 0; i < 16; i++, block += 4)
    {
        data[i] = readUint32(block);
    }

    HASHTransform(data.getBuffer());
}


