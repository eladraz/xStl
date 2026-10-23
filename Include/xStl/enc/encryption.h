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

/**
 * @addtogroup xstl_enc_cipher
 * @{
 */


/**
 * @file encryption.h
 * @brief Declare the abstract class cEncryption used for encryption.
 *
 * @details The class simplifies the encryption process by hiding the
 *    implementation and the key specification.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/array.h"


/**
 * @brief The encryption class. When first instantiated the class should be set
 *    up
 *   by providing a cBuffer key.
 *
 * @details The key is used to init the mechanism. The key can (and should) be
 *    the result of cKeyGeneration which creates random keys and adds an
 *    auxiliary layer.
 *
 * Used:
 *
 * @code
 * // cEncryption enc;
 * enc.loadKey(key);
 * enc.encrypt(data);
 * // data now contains the encrypted stream
 * @endcode
 */
class cEncryption
{
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~cEncryption();

    /**
     * @brief Setup new key for the mechanism.
     *
     * @details Each implementation is entitled to use some/or all bytes in the
     *    key.
     *
     * @param key The new bytes for the key.
     */
    virtual void loadKey(const cBuffer& key) = 0;

    /**
     * @brief Encrypt the data in the stream 'data'.
     *
     * @details The mechanism is advanced by the amount of bytes input to the
     *    function. In a block cipher the data should be aligned to the block
     *    boundaries. An assertion will be thrown otherwise.
     *
     * @param data The bytes to be encrypted.
     */
    virtual void encrypt(cBuffer &data) = 0;

    /**
     * @brief Decrypt the data in the stream 'data'.
     *
     * @details The mechanism is advanced by the amount of bytes input to the
     *    function. In a block cipher the data should be aligned to the block
     *    boundaries. An assertion will be thrown otherwise.
     *
     * @param data The bytes to be decrypted.
     */
    virtual void decrypt(cBuffer &data) = 0;
};



/** @} */
#endif // __TBA_STL_ENCRYPTION_H

