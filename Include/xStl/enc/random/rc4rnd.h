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

/*
 * rc4rnd.h
 *
 * The psedu-random-generated class for RC4 mechanizm.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/enc/random.h"
#include "xStl/enc/enc/rc4.h"


/*
 * class cRC4Random
 *
 * The psedu-random-generated class for RC4 mechanizm.
 */
class cRC4Random : public cRandom
{
public:
    /*
     * Init the random mechanizm according to 'seed'. The mechanizm is init
     * the RC4 engine (which limit the seed to 256 bytes only...)
     */
    cRC4Random(const cBuffer& seed);

    /*
     * Generate 'length' bytes of random and put it in 'output'. The 'output'
     * size is changed to 'length' and step over pure random. This function
     * increase the internal random mechanizm.
     * The generated random
     *
     * length - The amount of random to be generate.
     * output - Will be filled with the required psedu-random generation code.
     */
    virtual void generateRandom(uint length, cBuffer& output);

private:

    // The RC4 object to be generate the random from.
    cRC4 m_rc4;
};

#endif // __TBA_STL_ENCRYPTION_RANDOM_RC4_H
