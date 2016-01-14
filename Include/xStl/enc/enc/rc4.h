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

#ifndef __TBA_STL_ENCRYPTION_RC4_H
#define __TBA_STL_ENCRYPTION_RC4_H


/*
 * rc4.h
 *
 * Interface for the psedu Reivest-code-4 (RC4). Since all right reserved for rivest
 * the code will be called cRC4.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/enc/encryption.h"


/*
 * class cRC4
 *
 * Normal implementation.
 */
class cRC4
{
public:
    /*
     * Load a key for the mechanizm. The key is limited to 256 bytes.
     * If the key is smaller than 256 bytes the loading is warp around in cycle
     * to expand the key.
     */
    virtual void loadKey(const cBuffer& key);

    /*
     * Encrypt/Decrypt. Generate a random from the current key and XOR it to
     * the data stream.
     *
     * data - The cipher or the plain information to be convert to plain/cipher.
     */
    virtual void encrypt(cBuffer& data);
    virtual void decrypt(cBuffer& data);

    /*
     * Fills up 'data' (according to it's previous length) with the following
     * stip of the RC4 mechanizm
     *
     * data - Array to be filled with psedu-random RC4 generated strip.
     */
    void strip(cBuffer& data);

    /* Consts for the algorithm */
    enum { RC4KEY_LEN = 256 };

private:
    /*
     * Advance the mechanizm by one.
     */
    uint8 advance();

    /* Members */
    uint8 m_premutation[RC4KEY_LEN];
    uint8 m_i;
    uint8 m_j;
};

#endif // __TBA_STL_ENCRYPTION_RC4_H

