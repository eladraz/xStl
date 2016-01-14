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


/*
 * HashDigest.h
 *
 * Declare cHashDigest class which implement the all basic hash concepts such
 * as update, final, digest etc. (The divistion by block)
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/endian.h"
#include "xStl/enc/digest.h"


/*
 * class cMD5
 *
 * MD5 implementation class. Most of the code taken from C code on the internet.
 */
class cHashDigest : public cDigest,
                    public cEndian
{
public:
    /*
     * Constructor, copy-constructor and operator =.
     */
    cHashDigest();
    cHashDigest(const cHashDigest& other);
    cHashDigest& operator = (const cHashDigest& other);


    /* Virtual destructor */
    virtual ~cHashDigest() {};

    /*
     * Reset the hash setting as this class just constructed.
     * The implementation should overwrite this function and reset the defined
     * hash data.
     *
     * IMPORTANT NOTE:
     * In addition the function call the base class reset function AT THE
     * BEGINNING of the implementation.
     */
    virtual void reset();

    /*
     * Update the hash using a 'buffer' and it's length.
     *
     * buffer - The bytes to be digest.
     * length - The length of the buffer.
     */
    virtual void update(const void* buffer, const uint32 length);

    /*
     * Return the current digest of the hash using for all previous call to
     * 'update' from the last 'reset()'/construction call.
     *
     * Return CONST amount of bytes.
     */
    virtual cBuffer digest();

    /*
     * Return the number of bytes used for the digest.
     */
    virtual uint getDigestLength();

    /*
     * Clone the hash object.
     *
     * Return new constructed object equal to my object.
     */
    virtual cSmartPtr<cDigest> clone() = 0;

protected:
    // Copy hash data from other class
    void copyFrom(const cHashDigest& other);

    /*
     * Consts for the Hash algorithm
     */
    virtual unsigned int getDigestLen() = 0;
    virtual unsigned int getDataLen() = 0;

    /*
     * Final the MD5 round. Pad the block and sum up the digest.
     */
    void HASHFinal();

    /*
     * Final one round of the block.
     */
    void HASHBlock(const uint8* block);

    /*
     * Perform the transformation over one full block of DATALEN 32-bit words.
     *
     * data - DATALEN 32bit words.
     */
    virtual void HASHTransform(uint32 *data) = 0;

    // Digest
    cArray<uint32> m_digest;
    // Block count
    uint32 m_count_l;
    uint32 m_count_h;
    // One block buffer
    cBuffer m_block;
    uint32  m_blockSize;

    // index into buffer
    unsigned int m_index;

private:
    // Is the object constructed...
    bool m_isValid;
};


#endif // __TBA_STL_ENCRYPTION_DIGEST_HASH_H

