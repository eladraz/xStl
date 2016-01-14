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


/*
 * md5.h
 *
 * Declare cMD5 class which implement the MD5 one way hash function.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/endian.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/HashDigest.h"


/*
 * class cMD5
 *
 * MD5 implementation class.
 *
 * Most of the code taken from C code on the internet:
 * Part of the Python Cryptography Toolkit, version 1.0.1
 * Colin Plumb's original code modified by A.M. Kuchling
 * Further hacked and adapted to pike by Niels Möller
 *
 * The original code was change for C++ usage:
 *  - Creating base-class (cHashDigest)
 *  - Warp the function in cDigest format.
 */
class cMD5 : public cHashDigest
{
public:
    /*
     * Constructor, copy-constructor and operator =.
     */
    cMD5();
    cMD5(const cMD5& other);
    cMD5& operator = (const cMD5& other);


    /* Virtual destructor */
    virtual ~cMD5() {};

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
        MD5_DATALEN    = 16,
        MD5_DIGESTLEN  = 4
    };
    virtual unsigned int getDigestLen() { return MD5_DIGESTLEN; };
    virtual unsigned int getDataLen() { return MD5_DATALEN; };

    // Little endian
    littleEndianImpl;

private:
    /*
     * MD5 core digest.
     * Perform the transformation over one full block of 16 32-bit words.
     *
     * data - 16 32bit words.
     */
    virtual void HASHTransform(uint32 *data);
};


#endif // __TBA_STL_ENCRYPTION_DIGEST_MD5_H

