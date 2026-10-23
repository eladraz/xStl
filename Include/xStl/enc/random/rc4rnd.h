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

#ifndef __TBA_STL_ENCRYPTION_RANDOM_RC4_H
#define __TBA_STL_ENCRYPTION_RANDOM_RC4_H

/**
 * @addtogroup xstl_enc_random
 * @{
 */

/**
 * @file rc4rnd.h
 * @brief The pseudo-random-generated class for RC4 mechanism.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/enc/random.h"
#include "xStl/enc/enc/rc4.h"


/**
 * @brief The pseudo-random-generated class for RC4 mechanism.
 */
class cRC4Random : public cRandom
{
public:
    /**
     * @brief Init the random mechanism according to 'seed'.
     *
     * @details The mechanism initializes the RC4 engine (which limits the seed
     *    to 256 bytes only...)
     *
     * @param seed The seed of the random mechanism, limited to 256 bytes.
     */
    cRC4Random(const cBuffer& seed);

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
     * @brief The RC4 object to generate the random from.
     */
    cRC4 m_rc4;
};


/** @} */
#endif // __TBA_STL_ENCRYPTION_RANDOM_RC4_H
