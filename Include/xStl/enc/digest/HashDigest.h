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

#ifndef __TBA_STL_ENCRYPTION_DIGEST_HASH_H
#define __TBA_STL_ENCRYPTION_DIGEST_HASH_H

/**
 * @addtogroup xstl_enc_digest
 * @{
 */


/**
 * @file HashDigest.h
 * @brief Declare the cHashDigest class which implements all the basic hash
 *    concepts
 *   such as update, final, digest etc. (The division by block)
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/endian.h"
#include "xStl/enc/digest.h"


/**
 * @brief MD5 implementation class. Most of the code was taken from C code on
 *    the
 *   Internet.
 */
class cHashDigest : public cDigest,
                    public cEndian
{
public:
    /**
     * @brief Constructor, copy-constructor and operator =.
     */
    cHashDigest();
    /**
     * @brief Copy-constructor.
     *
     * @param other The object to copy.
     */
    cHashDigest(const cHashDigest& other);
    /**
     * @brief Assignment operator.
     *
     * @param other The object to copy from.
     * @return A reference to this object.
     */
    cHashDigest& operator = (const cHashDigest& other);


    /**
     * @brief Virtual destructor
     */
    virtual ~cHashDigest() {};

    /**
     * @brief Reset the hash setting as this class just constructed.
     *
     * @details The implementation should override this function and reset the
     *    defined hash data.
     *
     * @warning In addition the function calls the base class reset function AT
     *    THE BEGINNING of the implementation.
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
     * @brief Return the number of bytes used for the digest.
     *
     * @return The number of bytes used for the digest.
     */
    virtual uint getDigestLength();

    /**
     * @brief Clone the hash object.
     *
     * @return New constructed object equal to my object.
     */
    virtual cSmartPtr<cDigest> clone() = 0;

protected:
    /**
     * @brief Copy hash data from other class
     *
     * @param other The object to copy the hash data from.
     */
    void copyFrom(const cHashDigest& other);

    /**
     * @brief Return the length of the digest in 32 bit words.
     *
     * @return The length of the digest in 32 bit words.
     */
    virtual unsigned int getDigestLen() = 0;
    /**
     * @brief Return the length of a data block in 32 bit words.
     *
     * @return The length of a data block in 32 bit words.
     */
    virtual unsigned int getDataLen() = 0;

    /**
     * @brief Finalize the MD5 round.
     *
     * @details Pad the block and sum up the digest.
     */
    void HASHFinal();

    /**
     * @brief Finalize one round of the block.
     *
     * @param block The block to process.
     */
    void HASHBlock(const uint8* block);

    /**
     * @brief Perform the transformation over one full block of DATALEN 32-bit
     *    words.
     *
     * @param data DATALEN 32bit words.
     */
    virtual void HASHTransform(uint32 *data) = 0;

    /**
     * @brief Digest
     */
    cArray<uint32> m_digest;
    /**
     * @brief Block count
     */
    uint32 m_count_l;   ///< The low part of the block count.
    uint32 m_count_h;   ///< The high part of the block count.
    /**
     * @brief One block buffer
     */
    cBuffer m_block;
    uint32  m_blockSize;    ///< The size of a block in bytes.

    /**
     * @brief index into buffer
     */
    unsigned int m_index;

private:
    /**
     * @brief Is the object constructed...
     */
    bool m_isValid;
};



/** @} */
#endif // __TBA_STL_ENCRYPTION_DIGEST_HASH_H

