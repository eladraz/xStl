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

#ifndef __TBA_STL_ENCRYPTION_DIGEST_MD5_H
#define __TBA_STL_ENCRYPTION_DIGEST_MD5_H

/**
 * @addtogroup xstl_enc_digest
 * @{
 */


/**
 * @file md5.h
 * @brief Declare the cMD5 class which implements the MD5 one-way hash function.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/endian.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/HashDigest.h"


/**
 * @brief The MD5 message digest.
 *
 * @details Most of the code was taken from C code on the Internet:
 *   Part of the Python Cryptography Toolkit, version 1.0.1
 *   Colin Plumb's original code modified by A.M. Kuchling
 *   Further hacked and adapted to pike by Niels Möller
 *
 * The original code was changed for C++ usage:
 * - Creating base-class (cHashDigest)
 * - Wrap the function in cDigest format.
 */
class cMD5 : public cHashDigest
{
public:
    /**
     * @brief Constructor, copy-constructor and operator =.
     */
    cMD5();
    /**
     * @brief Copy-constructor.
     *
     * @param other The object to copy.
     */
    cMD5(const cMD5& other);
    /**
     * @brief Assignment operator.
     *
     * @param other The object to copy from.
     * @return A reference to this object.
     */
    cMD5& operator = (const cMD5& other);


    /**
     * @brief Virtual destructor
     */
    virtual ~cMD5() {};

    /**
     * @brief Reset the hash setting as this class just constructed.
     */
    virtual void reset();

    /**
     * @brief Clone the hash object.
     *
     * @return New constructed object equal to my object.
     */
    virtual cSmartPtr<cDigest> clone();

    /**
     * @brief Constants for the MD5 algorithm
     */
    enum {
        /**
         * @brief The size of a data block in 32 bit words.
         */
        /**
         * @brief The size of the digest in 32 bit words.
         */
        MD5_DATALEN    = 16,
        /** @brief The size of the digest in 32 bit words. */
        MD5_DIGESTLEN  = 4
    };
    /**
     * @brief Return the digest length in 32 bit words.
     *
     * @return MD5_DIGESTLEN.
     */
    virtual unsigned int getDigestLen() { return MD5_DIGESTLEN; };
    /**
     * @brief Return the data block length in 32 bit words.
     *
     * @return MD5_DATALEN.
     */
    virtual unsigned int getDataLen() { return MD5_DATALEN; };

    /**
     * @brief Little endian
     */
    littleEndianImpl;

private:
    /**
     * @brief MD5 core digest. Perform the transformation over one full block of
     *    16
     *   32-bit words.
     *
     * @param data 16 32bit words.
     */
    virtual void HASHTransform(uint32 *data);
};



/** @} */
#endif // __TBA_STL_ENCRYPTION_DIGEST_MD5_H

