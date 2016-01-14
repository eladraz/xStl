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

#ifndef __TBA_STL_TEST_SAMPLEPROTOCOL_H
#define __TBA_STL_TEST_SAMPLEPROTOCOL_H

/*
 * sampleProtocol.h
 *
 * A small protocol which test the ablilites of streams to communicate between
 * one anoter;
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/stream/stringerStream.h"
#include "xStl/stream/basicIO.h"

/*
 * Assume that both stream have a data-link layer and a protocol/transfer layer,
 * which means that all bytes will be sent at the right order and gets correctly
 * to the end.
 * Test the stream bytes sending packets across the streams
 *
 * The protocol is as following:
 *    - Handshake
 *    - Data transmission
 *    - Ending
 *
 * The handshake is consist from:
 *    - Sending hello stream
 *    - Sending a number
 *    - Sending how mush packets will be sent
 *    - Hasing the buffer sent so far
 *
 * The data is built from:
 *    - Packet length
 *    - Data
 *    - Hash over the length and the data
 */
class cSampleProtocol {
public:
    /*
     * Tests the server side protocol.
     * Send packets and read information from a client
     */
    static void testServerSide(basicIO& stream,
                               cStringerStream& out);

    /*
     * Tests the client side protocol
     * Read the server packets and response for commands.
     */
    static void testClientSide(basicIO& stream,
                               cStringerStream& out);

private:
    // The maximum number of packets to be test for each communication
    enum { MAX_PACKET_COUNT = 1000 };
    // The maximum number of bytes for each testing packet
    enum { MAX_PACKET_LENGTH = 4096 };

    /*
     * The signature to be sent as identification
     */
    enum { SIGNATURE_SIZE = 25 };
    static const char signature[SIGNATURE_SIZE + 1];
};

#endif // __TBA_STL_TEST_SAMPLEPROTOCOL_H
