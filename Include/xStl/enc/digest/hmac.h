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

#ifndef __TBA_STL_ENCRYPTION_DIGEST_HMAC_H
#define __TBA_STL_ENCRYPTION_DIGEST_HMAC_H

/**
 * @addtogroup xstl_enc_digest
 * @{
 */


/**
 * @file hmac.h
 * @brief Filter implementation of the hmac (hash by secret) algorithm as RFC
 *    2104
 *   suggests.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/enc/digest.h"


/**
 * @brief Hash data using a secret and another hash algorithm.
 *
 * @details See RFC2104 for more information. The class is initialized with a
 *    key and a smart-pointer to a constructed hash object which will be used
 *    for the hashing.
 *
 * This class is not thread-safe. The hash-object shouldn't be shared within
 * several of different threads.
 */
class cHMAC : public cDigest
{
public:
    /**
     * @brief Constructor. Create HMAC object with a secret ('key').
     *
     * @param hashObj The hash object to be used for the HMAC process.
     * @param key The init secret of the HMAC
     */
    cHMAC(cSmartPtr<cDigest> hashObj, const cBuffer& key);

    /*
     * Copy constructor and operator = will be automatically generated and
     * copy all data members.
     * This is normal behavior.
     */

    /**
     * @brief Reset the hash setting as this class just constructed.
     */
    virtual void reset();

    /**
     * @brief Update the hash using a 'buffer' and its length.
     *
     * @param buffer The bytes to be digest.
     * @param length The length of the buffer.
     */
    virtual void update(const void* buffer, const uint32 length);

    /**
     * @brief Return the current digest of the hash using for all previous call
     *    to
     *   'update' from the last 'reset()'/construction call.
     *
     * @return The current digest of the hash using for all previous call to
     *    'update' from the last 'reset()'/construction call.
     */
    virtual cBuffer digest();

    /**
     * @brief Clone the hash object.
     *
     * @return New constructed object equal to my object.
     */
    virtual cSmartPtr<cDigest> clone();

    /**
     * @brief Return the number of bytes used for the digest.
     *
     * @return The number of bytes used for the digest.
     */
    virtual uint getDigestLength();


private:
    /**
     * @brief The number of bytes for the inner and the outer key.
     */
    enum { KEY_B_LENGTH = 64 };

    /**
     * @brief The secret inner and outer key of the hmac.
     */
    uint8 m_innerKey[KEY_B_LENGTH];
    uint8 m_outerKey[KEY_B_LENGTH];

    /**
     * @brief The hash object to be used.
     */
    cSmartPtr<cDigest> m_hashObj;

    /**
     * @brief The text to be hashed.
     */
    cBuffer m_text;
};



/** @} */
#endif // __TBA_STL_ENCRYPTION_DIGEST_HMAC_H

