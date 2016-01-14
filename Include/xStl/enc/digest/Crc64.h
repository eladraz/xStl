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

#ifndef __STL_ENCRYPTION_DIGEST_CRC64_H
#define __STL_ENCRYPTION_DIGEST_CRC64_H

#include "xStl/types.h"
#include "xStl/enc/digest.h"

/**
 * 64 bit CRC implementation
 *
 * Based on "liblzma" implementation (dynamic table building)
 *
 * Author: Lasse Collin
 * Author: Elad Raz <e@eladraz.com>
 */
class CRC64 : public cDigest {
public:
    /// Initializes the CRC calculator.
    CRC64();

    // See cDigest::reset
    virtual void reset();
    // See cDigest::update
    virtual void update(const void* buffer, const uint32 length);
    // See cDigest::digest
    virtual cBuffer digest();
    // See cDigest::getDigestLength - Return 8
    virtual uint getDigestLength();
    // See cDigest::clone
    virtual cSmartPtr<cDigest> clone();

    /**
     * Returns the CRC value for the processed stream.
     */
    uint64 getValue();

private:
    enum { CRC64_NUMBER_OF_TABLES = 4,
           CRC64_PREMUTATION_TABLE_LEN = 256 };

    // Singleton object for CRC 64 tables
    class CRC64Tables {
    public:
        /*
         * Singleton get object. Initiate CRC on first call
         */
        static CRC64Tables& getInstance();

        /*
         * Return the precalculate tables
         */
        const uint64** getTables();

    private:
        // Constructor and destructor. Allocate/free memory
        CRC64Tables();
        ~CRC64Tables();
        // 4k table (of CRC64_PREMUTATION_TABLE_LEN)
        uint64* m_tables[CRC64_NUMBER_OF_TABLES];
    };

    /// Current value of the CRC.
    uint64 m_value;
};


#endif // __STL_ENCRYPTION_DIGEST_CRC16_H
