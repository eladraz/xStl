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

/*
 * TODO!
 *
 * UNIX implementation
 *
 * unixOS.cpp
 *
 * Implementation file for the Posix API.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/sarray.h"
#include "xStl/data/datastream.h"
#include "xStl/enc/random.h"
#include "xStl/os/mutex.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"

#undef __USE_MISC
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * This class uses the default lcg of libc compiler
 *
 * Author: Elad Raz
 */
class cUnixRandom : public cRandom
{
public:
    cUnixRandom()
    {
        srand(time(NULL));
    }

    virtual void generateRandom(uint length, cBuffer& output)
    {
        // Change the size of the output buffer.
        output.changeSize(length, false);
        // Start filling the buffer
        uint8* buffer = output.getBuffer();
        for (uint i = 0; i < length; i++)
            buffer[i] = (rand() >> 8) & 0xFF;
    }
};

cSmartPtr<cRandom> cOSRand::getRandom()
{
    if (g_globalRandom.isEmpty())
    {
        // Construct the first instance
        g_globalRandom = cSmartPtr<cRandom>(new cUnixRandom());
    }
    return g_globalRandom;
}

// Init the random mechanizm to empty
cSmartPtr<cRandom> cOSRand::g_globalRandom(NULL);

