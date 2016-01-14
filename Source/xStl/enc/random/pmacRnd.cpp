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
 * pmacRnd.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/queueFifo.h"
#include "xStl/except/trace.h"
#include "xStl/enc/random.h"
#include "xStl/enc/digest/hmac.h"
#include "xStl/enc/random/pmacRnd.h"

cPMacRandom::cPMacRandom(cSmartPtr<cHMAC> hmac,
                         const cBuffer& seed) :
    m_hmac(hmac),
    m_seed(seed),
    m_a(seed),
    m_queue(m_hmac->getDigestLength())
{
}

void cPMacRandom::generateRandom(uint length, cBuffer& output)
{
    output.changeSize(length, false);
    uint8* data = output.getBuffer();

    // Try to read from the olded generated random...
    while (length > 0)
    {
        uint rest = m_queue.getQueueOccupyspace();

        if (rest == 0)
        {
            // There isn't data in the queue
            // Generate length bytes of random
            // New blocks are reqiuerd.

            // Hash for the previous A
            //   A(i) = HMAC_hash(secret, A(i-1))
            m_hmac->reset();
            m_hmac->updateStream(m_a);
            m_a = m_hmac->digest();

            // Hash for the new block
            //   HMAC_hash(secret, A(i) + seed)
            m_hmac->reset();
            m_hmac->updateStream(m_a);
            m_hmac->updateStream(m_seed);
            cBuffer ret = m_hmac->digest();
            CHECK(m_queue.write(ret.getBuffer(), ret.getSize()) == ret.getSize());

            // Update queue length
            rest = m_queue.getQueueOccupyspace();
        }

        // Read from the buffer
        if (rest >= length)
        {
            CHECK(m_queue.read(data, length) == length);
            return;
        }
        CHECK(m_queue.read(data, rest) == rest);
        length-= rest;
        data  += rest;
    }
}

