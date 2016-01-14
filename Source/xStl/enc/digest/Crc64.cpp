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
#include "xStl/enc/digest/Crc64.h"

#define UINT64_C(x) x##ULL

// TODO! Should be same for CRC32 as well
#ifdef XSTL_BIG_ENDIAN
#    define A(x) ((x) >> 24)
#    define B(x) (((x) >> 16) & 0xFF)
#    define C(x) (((x) >> 8) & 0xFF)
#    define D(x) ((x) & 0xFF)

#    define S8(x) ((x) << 8)
#    define S32(x) ((x) << 32)

#else
#    define A(x) ((x) & 0xFF)
#    define B(x) (((x) >> 8) & 0xFF)
#    define C(x) (((x) >> 16) & 0xFF)
#    define D(x) ((x) >> 24)

#    define S8(x) ((x) >> 8)
#    define S32(x) ((x) >> 32)
#endif

CRC64::CRC64Tables::CRC64Tables()
{
    // Allocate heap memory
    for (uint i = 0; i < CRC64_NUMBER_OF_TABLES; i++)
        m_tables[i] = new uint64[CRC64_PREMUTATION_TABLE_LEN];
    // Calculate tables
    const uint64 poly64 = UINT64_C(0xC96C5795D7870F42);

    for (uint s = 0; s < CRC64_NUMBER_OF_TABLES; ++s)
    {
        for (size_t b = 0; b < CRC64_PREMUTATION_TABLE_LEN; ++b)
        {
            uint64 r = (s == 0) ? b : m_tables[s - 1][b];
            for (uint i = 0; i < 8; ++i)
            {
                if (r & 1)
                    r = (r >> 1) ^ poly64;
                else
                    r >>= 1;
            }
            m_tables[s][b] = r;
        }
    }

#ifdef XSTL_BIG_ENDIAN
    for (uint s = 0; s < CRC64_NUMBER_OF_TABLES; ++s)
        for (uint b = 0; b < CRC64_PREMUTATION_TABLE_LEN; ++b)
            m_tables[s][b] = swap64(lzma_crc64_table[s][b]); // TODO! Implement swap
#endif
}

CRC64::CRC64Tables::~CRC64Tables()
{
    for (uint i = 0; i < CRC64_NUMBER_OF_TABLES; i++)
        delete m_tables[i];
}

CRC64::CRC64Tables& CRC64::CRC64Tables::getInstance()
{
    static CRC64Tables gTables;
    return gTables;
}

const uint64** CRC64::CRC64Tables::getTables()
{
    return (const uint64**)m_tables;
}

CRC64::CRC64()
{
    // Prepare the tables
    CRC64Tables::getInstance();
    reset();
}

uint64 CRC64::getValue()
{
    return m_value;
}

void CRC64::reset()
{
    m_value = 0;
}

void CRC64::update(const void* b, const uint32 l)
{
    uint32 length = l;
    uint8* buffer = (uint8*)b;
    m_value = ~m_value;

#ifdef XSTL_BIG_ENDIAN
    m_value = swap64(m_value);
#endif

    const uint64** gTables = CRC64Tables::getInstance().getTables();

    if (length > 4)
    {
        while (getNumeric(buffer) & 3) {
            m_value = gTables[0][*buffer++ ^ A(m_value)] ^ S8(m_value);
            --length;
        }

        const uint8* limit = buffer + (length & ~(uint)(3));
        length &= (uint)(3);

        // Calculate the m_value64 using the slice-by-four algorithm.
        //
        // In contrast to m_value32 code, this one seems to be fastest
        // with -O3 -fomit-frame-pointer.
        while (buffer < limit)
        {
#ifdef XSTL_BIG_ENDIAN
            const uint32 tmp = (uint32)((m_value >> 32) ^ *(uint32*)(buffer));
#else
            const uint32 tmp = (uint32)(m_value ^ *(uint32*)(buffer));
#endif
            buffer += 4;

            // It is critical for performance, that
            // the m_value variable is XORed between the
            // two table-lookup pairs.
            m_value = gTables[3][A(tmp)]
                    ^ gTables[2][B(tmp)]
                    ^ S32(m_value)
                    ^ gTables[1][C(tmp)]
                    ^ gTables[0][D(tmp)];
        }
    }

    while (length-- != 0)
        m_value = gTables[0][*buffer++ ^ A(m_value)] ^ S8(m_value);

#ifdef XSTL_BIG_ENDIAN
    m_value = swap64(m_value);
#endif

    m_value = ~m_value;
}

cBuffer CRC64::digest()
{
    cBuffer ret(sizeof(uint64));
    cBigEndian::writeUint64(ret.getBuffer(), m_value);
    return ret;
}

uint CRC64::getDigestLength()
{
    return sizeof(uint64);
}

cSmartPtr<cDigest> CRC64::clone()
{
    return cSmartPtr<cDigest>(new CRC64());
}
