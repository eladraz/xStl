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

#ifndef __TBA_STL_ENCRYPTION_DIGEST_H
#define __TBA_STL_ENCRYPTION_DIGEST_H


/*
 * digest.h
 *
 * Declare the abstract class cDigest used for one way hash function -
 * digital signature the information.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"

/*
 * class cDigest
 *
 * Hash a number of bytes. For using the class you should create the class or call
 * to 'reset()'. Update the hash using one or more calls to 'update()' function
 * giving the hash bytes. At any priod of time a call to 'digest()' return the
 * signature for the hash.
 */
class cDigest
{
public:
    /*
     * Virtual destructor
     */
    virtual ~cDigest();

    /*
     * Reset the hash setting as this class just constructed.
     */
    virtual void reset() = 0;

    /*
     * Update the hash using a 'buffer' and it's length.
     *
     * buffer - The bytes to be digest.
     * length - The length of the buffer.
     */
    virtual void update(const void* buffer, const uint32 length) = 0;

    /*
     * Return the current digest of the hash using for all previous call to
     * 'update' from the last 'reset()'/construction call.
     *
     * Return CONST amount of bytes.
     */
    virtual cBuffer digest() = 0;

    /*
     * Clone the hash object.
     *
     * Return new constructed object equal to my object.
     */
    virtual cSmartPtr<cDigest> clone() = 0;

    /*
     * Return the number of bytes used for the digest.
     */
    virtual uint getDigestLength() = 0;

    /*
     * Update the hash using a stream. The implementation of this function is
     * to call to the virtual 'update(uint8*, uint32)' function.
     *
     * data - The data to be digest in cBuffer format.
     */
    virtual void updateStream(const cBuffer& data);
};

// The digest pointer object
typedef cSmartPtr<cDigest> cDigestPtr;


#endif // __TBA_STL_ENCRYPTION_DIGEST_H

