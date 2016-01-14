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
 * sampleProtocol.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/digest/md5.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "sampleProtocol.h"

const char cSampleProtocol::signature[SIGNATURE_SIZE + 1] =
    "EladRazT.B.A.RaztTech2005";

void cSampleProtocol::testServerSide(basicIO& stream,
                                     cStringerStream& out)
{
    // Wait until a client will connected
    char sig[SIGNATURE_SIZE + 1];
    stream.pipeRead(sig, SIGNATURE_SIZE);
    sig[SIGNATURE_SIZE] = '\0';
    CHECK(strcmp(sig, signature) == 0);

    out << "SERVER: New client was enter..." << endl;

    // Start by sending hello
    cMD5 md5;
    stream.pipeWrite(signature, SIGNATURE_SIZE);
    md5.update(signature, SIGNATURE_SIZE);
    // Make a session ID
    uint32 id = cOSRand::rand();
    md5.update(&id, sizeof(uint32)); // TODO! Endian
    stream.pipeWrite(&id, sizeof(id));
    // Make the number of packets
    uint32 count = (cOSRand::rand() % MAX_PACKET_COUNT) + 1;
    md5.update(&count, sizeof(uint32)); // TODO! Endian
    stream.pipeWrite(&count, sizeof(count));
    // Transmit the hash
    stream.pipeWrite(md5.digest(), md5.getDataLen());

    out << "SERVER: Stage1 handshake completed!" << endl;

    // Read the client response
    md5.reset();
    uint32 numberOfClientPackets;
    uint32 clientID;
    stream.pipeRead(&numberOfClientPackets, sizeof(uint32));
    stream.pipeRead(&clientID, sizeof(uint32));
    CHECK(clientID == id);
    md5.update(&numberOfClientPackets, sizeof(uint32));
    md5.update(&clientID, sizeof(uint32));
    cBuffer digest;
    stream.pipeRead(digest, md5.getDataLen());
    CHECK(digest == md5.digest());

    out << "SERVER: Stage2 handshake completed!" << endl;

    // Start sending server packets
    for (uint i = 0; i < count; i++)
    {
        md5.reset();
        uint32 length = (cOSRand::rand() % MAX_PACKET_LENGTH) + 1;
        cBuffer data;
        cOSRand::generateRandom(length, data);
        md5.update(&length, sizeof(uint32));
        md5.updateStream(data);
        stream.pipeWrite(&length, sizeof(uint32));
        stream.pipeWrite(data, length);
        stream.pipeWrite(md5.digest(), md5.getDataLen());

        uint8 ok;
        stream.streamReadUint8(ok);
        CHECK(ok == (length & 0xFF));

        out << '.';
        out.flush();
    }

    // Start getting client packets
    for (uint j = 0; j < numberOfClientPackets; j++)
    {
        md5.reset();
        uint32 length;
        cBuffer data;
        cBuffer digest;
        stream.pipeRead(&length, sizeof(length));
        stream.pipeRead(data, length);
        stream.pipeRead(digest, md5.getDataLen());
        md5.update(&length, sizeof(length));
        md5.updateStream(data);
        CHECK(digest == md5.digest());
        uint8 ok = (uint8)(length & 0xFF);
        stream.streamWriteUint8(ok);

        out << '/';
        out.flush();
    }

    out << endl;

    // DONE!
}

void cSampleProtocol::testClientSide(basicIO& stream,
                                     cStringerStream& out)
{
    out << "CLIENT: Connecting to server..." << endl;
    stream.pipeWrite(signature, SIGNATURE_SIZE);

    cMD5 md5;
    char sig[SIGNATURE_SIZE + 1];
    stream.pipeRead(sig, SIGNATURE_SIZE);
    sig[SIGNATURE_SIZE] = '\0';
    CHECK(strcmp(sig, signature) == 0);
    md5.update(signature, SIGNATURE_SIZE);
    // Read the ID
    uint32 id;
    stream.pipeRead(&id, sizeof(id));
    md5.update(&id, sizeof(uint32));
    // Read the number of packets
    uint32 count;
    stream.pipeRead(&count, sizeof(count));
    md5.update(&count, sizeof(uint32));
    // Read the hash
    cBuffer digest;
    stream.pipeRead(digest, md5.getDataLen());
    CHECK(digest == md5.digest());

    out << "CLIENT: Stage1 handshake accepted..." << endl;

    // Send handshake response
    md5.reset();
    uint32 numberOfClientPackets = (cOSRand::rand() % MAX_PACKET_COUNT) + 1;
    md5.update(&numberOfClientPackets, sizeof(uint32));
    stream.pipeWrite(&numberOfClientPackets, sizeof(numberOfClientPackets));
    md5.update(&id, sizeof(uint32));
    stream.pipeWrite(&id, sizeof(id));
    stream.pipeWrite(md5.digest(), md5.getDataLen());

    out << "CLIENT: Stage2 handshake sent..." << endl;

    // Start getting server side packets
    for (uint j = 0; j < count; j++)
    {
        md5.reset();
        uint32 length;
        cBuffer data;
        cBuffer digest;
        stream.pipeRead(&length, sizeof(length));
        stream.pipeRead(data, length);
        stream.pipeRead(digest, md5.getDataLen());
        md5.update(&length, sizeof(length));
        md5.updateStream(data);
        CHECK(digest == md5.digest());
        uint8 ok = (uint8)(length & 0xFF);
        stream.streamWriteUint8(ok);

        out << '/';
        out.flush();
    }

    // Start sending server packets
    for (uint i = 0; i < numberOfClientPackets; i++)
    {
        md5.reset();
        uint32 length = (cOSRand::rand() % MAX_PACKET_LENGTH) + 1;
        cBuffer data;
        cOSRand::generateRandom(length, data);
        md5.update(&length, sizeof(uint32));
        md5.updateStream(data);
        stream.pipeWrite(&length, sizeof(uint32));
        stream.pipeWrite(data, length);
        stream.pipeWrite(md5.digest(), md5.getDataLen());

        uint8 ok;
        stream.streamReadUint8(ok);
        CHECK(ok == (length & 0xFF));

        out << '.';
        out.flush();
    }

    // DONE!
}
