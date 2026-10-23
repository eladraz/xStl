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

#ifndef __TBA_STL_ENCRYPTION_RANDOM_PMAC_H
#define __TBA_STL_ENCRYPTION_RANDOM_PMAC_H

/**
 * @addtogroup xstl_enc_random
 * @{
 */

/**
 * @file pmacRnd.h
 * @brief The pseudo-random-generated class for P function as described in RFC
 *    2246
 *   which is the TLS protocol specification.
 *
 * @details The RFC describes the PRF function which uses the P function for a
 *    single pseudo random-generated HMAC function
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/queueFifo.h"
#include "xStl/enc/random.h"
#include "xStl/enc/digest/hmac.h"


/**
 * @brief Defines the P function.
 *
 * @details The P function is defined as follows:
 *
 * @code
 * P_hash(secret, seed) = HMAC_hash(secret, A(1) + seed) +
 *                        HMAC_hash(secret, A(2) + seed) +
 *                        HMAC_hash(secret, A(3) + seed) + ...
 * @endcode
 *
 * Where cPMacRandom gets a pointer to a constructed HMAC object (which has
 * been initialized with a secret) and a seed.
 */
class cPMacRandom : public cRandom
{
public:
    /**
     * @brief Init the random mechanism according to 'seed' and hmac object
     *    which is
     *   used to digest secret+seed and generate random.
     *
     * @param hmac A constructed HMAC object which was initialized with the
     *    secret.
     * @param seed The seed of the random mechanism.
     */
    cPMacRandom(cSmartPtr<cHMAC> hmac,
                const cBuffer& seed);

    /**
     * @brief Generate 'length' bytes of random and put it in 'output'.
     *
     * @details The 'output' size is changed to 'length' and step over pure
     *    random. This function advances the internal random mechanism. The
     *    generated random
     *
     * @param length The amount of random to be generate.
     * @param output Will be filled with the required pseudo-random generation
     *    code.
     */
    virtual void generateRandom(uint length, cBuffer& output);

private:
    /**
     * @brief Deny copy-constructor and operator =
     */
    cPMacRandom(const cPMacRandom& other);
    cPMacRandom& operator = (const cPMacRandom& other);


    /**
     * @brief The HMAC object
     */
    cSmartPtr<cHMAC> m_hmac;

    /**
     * @brief The seed of the object
     */
    cBuffer m_seed;

    /**
     * @brief The A's seed of the object
     */
    cBuffer m_a;

    /**
     * @brief The rest of the random data
     */
    cQueueFifo m_queue;
};


/** @} */
#endif // __TBA_STL_ENCRYPTION_RANDOM_PMAC_H
