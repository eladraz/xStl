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
 * hmac.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/array.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/hmac.h"

cHMAC::cHMAC(cSmartPtr<cDigest> hashObj, const cBuffer& key) :
    m_hashObj(hashObj)
{
    // Test that the hash function is coordinate with this version of
    // the HMAC
    ASSERT(getDigestLength() <= KEY_B_LENGTH);

    uint i;
    m_hashObj->reset();

    // Reset the keys
    for (i = 0; i < KEY_B_LENGTH; i++)
    {
        m_outterKey[i] = m_innerKey[i] = 0;
    }

    // If the key is larger than 64 bytes reset it by the hash function.
    if (key.getSize() > KEY_B_LENGTH)
    {
        // Reset the hash
        m_hashObj->updateStream(key);
        cBuffer key = m_hashObj->digest();

        // Copy the keys
        cOS::memcpy(m_outterKey, key.getBuffer(), key.getSize());
        cOS::memcpy(m_innerKey,  key.getBuffer(), key.getSize());
    } else
    {
        // Copy the keys
        cOS::memcpy(m_outterKey, key.getBuffer(), key.getSize());
        cOS::memcpy(m_innerKey,  key.getBuffer(), key.getSize());
    }

    // Xor the keys
    for (i = 0; i < KEY_B_LENGTH; i++)
    {
        m_innerKey[i]  ^= 0x36;
        m_outterKey[i] ^= 0x5c;
    }
}

void cHMAC::reset()
{
    m_hashObj->reset();
    m_text.changeSize(0);
}

void cHMAC::update(const void* buffer, const uint32 length)
{
    // Save the original hash size
    uint orgSize = m_text.getSize();

    // Increase the array by length bytes
    m_text.changeSize(orgSize + length);

    // Copy the data to the buffer
    cOS::memcpy(m_text.getBuffer() + orgSize, buffer, length);
}

cBuffer cHMAC::digest()
{
    // perform inner MD5
    m_hashObj->reset();
    m_hashObj->update(m_innerKey, KEY_B_LENGTH);
    m_hashObj->updateStream(m_text);
    cBuffer inner = m_hashObj->digest();

    // perform outer MD5
    m_hashObj->reset();
    m_hashObj->update(m_outterKey, KEY_B_LENGTH);
    m_hashObj->updateStream(inner);
    return m_hashObj->digest();
}

cSmartPtr<cDigest> cHMAC::clone()
{
    return cSmartPtr<cDigest>((cDigest*)(new cHMAC(*this)));
}

uint cHMAC::getDigestLength()
{
    return m_hashObj->getDigestLength();
}
