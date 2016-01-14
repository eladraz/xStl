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

#ifndef __TBA_STL_ENCRYPTION_H
#define __TBA_STL_ENCRYPTION_H


/*
 * encryption.h
 *
 * Declare the abstract class cEncryption used for encryption. The class
 * simplify the encryption process by hiding the implementation and the
 * key spesification.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"


/*
 * class cEncryption
 *
 * The encryption class. When first instance the class should setup by providing
 * a cBuffer key. The key used to init the mechanizm. The key can (and should) be
 * result from cKeyGeneration which create random keys and add auxilary layer.
 *
 * Used:
 *   // cEncryption enc;
 *   enc.loadKey(key);
 *   enc.encrypt(data);
 *   // data now contain the encrypt stream
 */
class cEncryption
{
public:
    /*
     * Virtual destructor
     */
    virtual ~cEncryption();

    /*
     * Setup new key for the mechanizm.
     *
     * key - The new bytes for the key.
     *
     * Each implementation is entitle to use some/or all bytes in the key.
     */
    virtual void loadKey(const cBuffer& key) = 0;

    /*
     * Encrypt the data in the stream 'data'. The mechanizm is advanced by the
     * amount of bytes input for the function. In block cipher the data should
     * be alignment to the block boundries. Assertion will be thorw otherwise.
     *
     * data - The bytes to be encrypt.
     */
    virtual void encrypt(cBuffer &data) = 0;

    /*
     * Decrypt the data in the stream 'data'. The mechanizm is advanced by the
     * amount of bytes input for the function. In block cipher the data should
     * be alignment to the block boundries. Assertion will be thorw otherwise.
     *
     * data - The bytes to be decrypt.
     */
    virtual void decrypt(cBuffer &data) = 0;
};


#endif // __TBA_STL_ENCRYPTION_H

