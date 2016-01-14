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
 * test_string.cpp
 *
 * Test the cString class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/list.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/stringerStream.h"
#include "xStl/stream/ioStream.h"
#include "xStl/os/os.h"
#include "../../xStl/tests/tests.h"

class cTestStringStream : public cTestObject
{
public:
    void test1()
    {
        uint8 buffer[] = {0xDE, 0xAD, 0xFA, 0xCE, 0x00, 0x00, 0xBE, 0xEF, 0xFA, 0xCE };
        cStringStream stream;

        // Write down the buffer
        stream << "Hello world!" << endl;
        stream << "============" << endl;
        stream << XSTL_STRING("This tests the unicode strings") << endl;
        stream << 13 << "  " << -4 << endl;
        stream << HEXDWORD(0xDEADFACE) << endl;
        stream.pipeWrite(buffer, sizeof(buffer));
        stream << endl;
    }

    // Perform the test
    virtual void test()
    {
        test1();
    };

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestStringStream g_globalStringStream;

