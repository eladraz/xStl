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

#include "xStl/xStlPrecompiled.h"
/*
 * prf.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/queueFifo.h"
#include "xStl/enc/random.h"
#include "xStl/enc/utils.h"
#include "xStl/enc/digest/md5.h"
#include "xStl/enc/digest/sha1.h"
#include "xStl/enc/digest/hmac.h"
#include "xStl/enc/random/pmacRnd.h"
#include "xStl/enc/random/prf.h"

cPRF::cPRF(const cBuffer& seed, const cBuffer& secret) :
    m_md5(NULL),
    m_sha1(NULL)
{
    // Split the secret into two half. = ceil(L_S / 2);
    uint ls = secret.getSize() / 2;
    uint gs = ls;
    if ((secret.getSize() & 1) == 1) {
        ls++;
    }

    cBuffer s1(secret.getBuffer(),      ls);
    cBuffer s2(secret.getBuffer() + gs, ls);

    // Create HMAC md5 object.
    cSmartPtr<cHMAC> md5(new cHMAC(cSmartPtr<cDigest>((cDigest*)new cMD5()), s1));

    // Create HMAC sha1 object.
    cSmartPtr<cHMAC> sha(new cHMAC(cSmartPtr<cDigest>((cDigest*)new cSHA1()), s2));

    m_md5 = (cSmartPtr<cPMacRandom>)(new cPMacRandom(md5, seed));
    m_sha1 = (cSmartPtr<cPMacRandom>)(new cPMacRandom(sha, seed));
}


void cPRF::generateRandom(uint length, cBuffer& output)
{
    cBuffer temp;
    m_md5->generateRandom(length, output);
    m_sha1->generateRandom(length, temp);

    // XOR two stream
    cEncryptionUtils::_xor(output.getBuffer(), temp.getBuffer(), length);
}
