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
 * md5.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/md5.h"


cMD5::cMD5()
{
}

void cMD5::reset()
{
    cHashDigest::reset();

    m_digest[0] = 0x67452301L;
    m_digest[1] = 0xefcdab89L;
    m_digest[2] = 0x98badcfeL;
    m_digest[3] = 0x10325476L;
}

cMD5::cMD5(const cMD5& other)
{
    cHashDigest::copyFrom(other);
}

cMD5& cMD5::operator = (const cMD5& other)
{
    if (this != &other)
    {
        cHashDigest::copyFrom(other);
    }
    return *this;
}

cSmartPtr<cDigest> cMD5::clone()
{
    return cSmartPtr<cDigest>(new cMD5(*this));
}


/* MD5 functions */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define ROUND(f, w, x, y, z, data, s) \
( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

void cMD5::HASHTransform(uint32 *data)
{
    uint32 a, b, c, d;
    a = m_digest[0];
    b = m_digest[1];
    c = m_digest[2];
    d = m_digest[3];

    ROUND(F1, a, b, c, d, data[ 0] + 0xd76aa478L, 7);
    ROUND(F1, d, a, b, c, data[ 1] + 0xe8c7b756L, 12);
    ROUND(F1, c, d, a, b, data[ 2] + 0x242070dbL, 17);
    ROUND(F1, b, c, d, a, data[ 3] + 0xc1bdceeeL, 22);
    ROUND(F1, a, b, c, d, data[ 4] + 0xf57c0fafL, 7);
    ROUND(F1, d, a, b, c, data[ 5] + 0x4787c62aL, 12);
    ROUND(F1, c, d, a, b, data[ 6] + 0xa8304613L, 17);
    ROUND(F1, b, c, d, a, data[ 7] + 0xfd469501L, 22);
    ROUND(F1, a, b, c, d, data[ 8] + 0x698098d8L, 7);
    ROUND(F1, d, a, b, c, data[ 9] + 0x8b44f7afL, 12);
    ROUND(F1, c, d, a, b, data[10] + 0xffff5bb1L, 17);
    ROUND(F1, b, c, d, a, data[11] + 0x895cd7beL, 22);
    ROUND(F1, a, b, c, d, data[12] + 0x6b901122L, 7);
    ROUND(F1, d, a, b, c, data[13] + 0xfd987193L, 12);
    ROUND(F1, c, d, a, b, data[14] + 0xa679438eL, 17);
    ROUND(F1, b, c, d, a, data[15] + 0x49b40821L, 22);

    ROUND(F2, a, b, c, d, data[ 1] + 0xf61e2562L, 5);
    ROUND(F2, d, a, b, c, data[ 6] + 0xc040b340L, 9);
    ROUND(F2, c, d, a, b, data[11] + 0x265e5a51L, 14);
    ROUND(F2, b, c, d, a, data[ 0] + 0xe9b6c7aaL, 20);
    ROUND(F2, a, b, c, d, data[ 5] + 0xd62f105dL, 5);
    ROUND(F2, d, a, b, c, data[10] + 0x02441453L, 9);
    ROUND(F2, c, d, a, b, data[15] + 0xd8a1e681L, 14);
    ROUND(F2, b, c, d, a, data[ 4] + 0xe7d3fbc8L, 20);
    ROUND(F2, a, b, c, d, data[ 9] + 0x21e1cde6L, 5);
    ROUND(F2, d, a, b, c, data[14] + 0xc33707d6L, 9);
    ROUND(F2, c, d, a, b, data[ 3] + 0xf4d50d87L, 14);
    ROUND(F2, b, c, d, a, data[ 8] + 0x455a14edL, 20);
    ROUND(F2, a, b, c, d, data[13] + 0xa9e3e905L, 5);
    ROUND(F2, d, a, b, c, data[ 2] + 0xfcefa3f8L, 9);
    ROUND(F2, c, d, a, b, data[ 7] + 0x676f02d9L, 14);
    ROUND(F2, b, c, d, a, data[12] + 0x8d2a4c8aL, 20);

    ROUND(F3, a, b, c, d, data[ 5] + 0xfffa3942L, 4);
    ROUND(F3, d, a, b, c, data[ 8] + 0x8771f681L, 11);
    ROUND(F3, c, d, a, b, data[11] + 0x6d9d6122L, 16);
    ROUND(F3, b, c, d, a, data[14] + 0xfde5380cL, 23);
    ROUND(F3, a, b, c, d, data[ 1] + 0xa4beea44L, 4);
    ROUND(F3, d, a, b, c, data[ 4] + 0x4bdecfa9L, 11);
    ROUND(F3, c, d, a, b, data[ 7] + 0xf6bb4b60L, 16);
    ROUND(F3, b, c, d, a, data[10] + 0xbebfbc70L, 23);
    ROUND(F3, a, b, c, d, data[13] + 0x289b7ec6L, 4);
    ROUND(F3, d, a, b, c, data[ 0] + 0xeaa127faL, 11);
    ROUND(F3, c, d, a, b, data[ 3] + 0xd4ef3085L, 16);
    ROUND(F3, b, c, d, a, data[ 6] + 0x04881d05L, 23);
    ROUND(F3, a, b, c, d, data[ 9] + 0xd9d4d039L, 4);
    ROUND(F3, d, a, b, c, data[12] + 0xe6db99e5L, 11);
    ROUND(F3, c, d, a, b, data[15] + 0x1fa27cf8L, 16);
    ROUND(F3, b, c, d, a, data[ 2] + 0xc4ac5665L, 23);

    ROUND(F4, a, b, c, d, data[ 0] + 0xf4292244L, 6);
    ROUND(F4, d, a, b, c, data[ 7] + 0x432aff97L, 10);
    ROUND(F4, c, d, a, b, data[14] + 0xab9423a7L, 15);
    ROUND(F4, b, c, d, a, data[ 5] + 0xfc93a039L, 21);
    ROUND(F4, a, b, c, d, data[12] + 0x655b59c3L, 6);
    ROUND(F4, d, a, b, c, data[ 3] + 0x8f0ccc92L, 10);
    ROUND(F4, c, d, a, b, data[10] + 0xffeff47dL, 15);
    ROUND(F4, b, c, d, a, data[ 1] + 0x85845dd1L, 21);
    ROUND(F4, a, b, c, d, data[ 8] + 0x6fa87e4fL, 6);
    ROUND(F4, d, a, b, c, data[15] + 0xfe2ce6e0L, 10);
    ROUND(F4, c, d, a, b, data[ 6] + 0xa3014314L, 15);
    ROUND(F4, b, c, d, a, data[13] + 0x4e0811a1L, 21);
    ROUND(F4, a, b, c, d, data[ 4] + 0xf7537e82L, 6);
    ROUND(F4, d, a, b, c, data[11] + 0xbd3af235L, 10);
    ROUND(F4, c, d, a, b, data[ 2] + 0x2ad7d2bbL, 15);
    ROUND(F4, b, c, d, a, data[ 9] + 0xeb86d391L, 21);

    m_digest[0] += a;
    m_digest[1] += b;
    m_digest[2] += c;
    m_digest[3] += d;
}

