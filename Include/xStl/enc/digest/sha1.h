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

#ifndef __TBA_STL_ENCRYPTION_DIGEST_SHA1_H
#define __TBA_STL_ENCRYPTION_DIGEST_SHA1_H


/*
 * sha1.h
 *
 * Declare cSHA1 class which implement the SHA-1 (Secure Hash Algorithm) one-
 * way hash function.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/endian.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/HashDigest.h"


/*
 * class cSHA1
 *
 * SHA1 implementation class.
 *
 * Based on SHA code originally posted to sci.crypt by Peter Gutmann
 * in message <30ajo5$oe8@ccu2.auckland.ac.nz>.
 * Modified to test for endianness on creation of SHA objects by AMK.
 * Also, the original specification of SHA was found to have a weakness
 * by NSA/NIST.  This code implements the fixed version of SHA.
 *
 * Here's the first paragraph of Peter Gutmann's posting:
 *
 * The following is my SHA (FIPS 180) code updated to allow use of the "fixed"
 * SHA, thanks to Jim Gillogly and an anonymous contributor for the information on
 * what's changed in the new version.  The fix is a simple change which involves
 * adding a single rotate in the initial expansion function.  It is unknown
 * whether this is an optimal solution to the problem which was discovered in the
 * SHA or whether it's simply a bandaid which fixes the problem with a minimum of
 * effort (for example the reengineering of a great many Capstone chips).
 *
 *
 * The original code was change for C++ usage:
 *  - Creating base-class (cHashDigest)
 *  - Warp the function in cDigest format.
 */
class cSHA1 : public cHashDigest
{
public:
    /*
     * Constructor, copy-constructor and operator =.
     */
    cSHA1();
    cSHA1(const cSHA1& other);
    cSHA1& operator = (const cSHA1& other);


    /* Virtual destructor */
    virtual ~cSHA1() {};

    /*
     * Reset the hash setting as this class just constructed.
     */
    virtual void reset();

    /*
     * Clone the hash object.
     *
     * Return new constructed object equal to my object.
     */
    virtual cSmartPtr<cDigest> clone();

    /*
     * Consts for the MD5 algorithm
     */
    enum {
        SHA1_DATALEN    = 16,
        SHA1_DIGESTLEN  = 5
    };
    virtual unsigned int getDataLen() { return SHA1_DATALEN; };
    virtual unsigned int getDigestLen() { return SHA1_DIGESTLEN; };

    // Big endian impl
    bigEndianImpl;

private:
    /*
     * MD5 core digest.
     * Perform the transformation over one full block of 16 32-bit words.
     *
     * data - 16 32bit words.
     */
    virtual void HASHTransform(uint32 *data);
};


#endif // __TBA_STL_ENCRYPTION_DIGEST_SHA1_H

