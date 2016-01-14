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
#ifdef __APPLE__
#include <sys/types.h>
#endif

/*
 * test_socket.cpp
 *
 * Test the cSocketStream class. Create TCP connection and transfer data
 * from it and to it.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/os/thread.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/stream/ioStream.h"
#include "xStl/stream/socketAddr.h"
#include "xStl/stream/socketStream.h"
#include "tests.h"

#define LOOPBACK "127.0.0.1"
#define TESTPORT 0x666

class cTestSockets : public cTestObject
{
public:
    // Return code from the thread
    enum RETCODE
    {
        THREAD_OK,
        THREAD_ERROR
    };

    typedef struct ARGUMENTS
    {
        cSocketStream* server;
        uint8* smsg;
        uint32 smsglen;
        uint8* rmsg;
        uint32 rmsglen;
    };

    // Singal between threads;
    static volatile RETCODE m_msg;

    // The server side
    static DWORD serverSide(ARGUMENTS* args)
    {
        XSTL_TRY
        {
            // Get a connection
            cSocketStream newSocket;
            cSockAddrIn   newAddr;
            args->server->accept(newSocket, newAddr);

            // Accept
            cout << 'A';

            // Write to the socket
            newSocket.pipeWrite(args->smsg, args->smsglen);
            cout << 'W';

            // Read from the socket
            cBuffer rst;
            unsigned int read = newSocket.pipeRead(rst, args->rmsglen);
            if (read != args->rmsglen)
            {
                // Message doens't recieve
                cout << endl << "Server: cannot read message back!" << endl;
                cout << "Read only " << (uint32)(read) << " out-of " << args->rmsglen << endl;
                m_msg = THREAD_ERROR;
                return m_msg;
            }

            if (memcmp(rst.getBuffer(), args->rmsg, args->rmsglen))
            {
                cout << endl << "Server: cannot read message back!" << endl;
                cout << "Buffer is different!" << endl;
                m_msg = THREAD_ERROR;
                return m_msg;
            }

            cout << 'R';

            // Close the socket
            newSocket.close();
            cout << 'C';

            m_msg = THREAD_OK;
            return m_msg;
        }
        XSTL_CATCH(...)
        {
            m_msg = THREAD_ERROR;
            return m_msg;
        }
    }

    void test_socket()
    {
        for (unsigned int i = 0; i < 30; i++)
        {
            // Create loopback address
            cSockAddrIn addr(LOOPBACK, TESTPORT);

            // Create two-sides objects.
            cSocketStream side1, side2;
            side1.create();
            side2.create();

            // Prepare server
            cout << 'S';
            side1.bind(addr);
            side1.listen();

            // Preparing client.
            m_msg = THREAD_OK;
            ARGUMENTS args;
            args.server = &side1;

            #define MAX_BUFFER (1024)
            uint8 smsg[MAX_BUFFER];
            uint32 smsglen = cOSRand::rand() % MAX_BUFFER;
            uint32 idx;
            for (idx = 0; idx < smsglen; idx++) { smsg[idx] = (uint8)cOSRand::rand(); }

            uint8 rmsg[MAX_BUFFER];
            uint32 rmsglen = cOSRand::rand() % MAX_BUFFER;
            for (idx = 0; idx < rmsglen; idx++) { rmsg[idx] = (uint8)cOSRand::rand(); }

            args.smsg = smsg;
            args.smsglen = smsglen;
            args.rmsg = rmsg;
            args.rmsglen = rmsglen;

            // Wait for connection
            cThread newThread;
            newThread.execute((ThreadRoutine)serverSide, &args);
            cout << 'E';

            // Create a connection and read data from it
            uint8 lbuf[MAX_BUFFER];
            side2.connect(addr);

            // Read the data and compare it.
            unsigned int read = side2.pipeRead(lbuf, smsglen);
            TESTS_ASSERT_EQUAL(read, smsglen);
            TESTS_ASSERT_EQUAL(memcmp(lbuf, smsg,smsglen), 0);
            TESTS_ASSERT_EQUAL(m_msg, THREAD_OK);

            // Write the data
            side2.pipeWrite(rmsg, rmsglen);
            TESTS_ASSERT_EQUAL(m_msg, THREAD_OK);

            newThread.wait();
            TESTS_ASSERT_EQUAL(m_msg, THREAD_OK);

            // Close the server
            cout << 'D';
            side1.close();
        }
    }

    // Perform the test
    virtual void test()
    {
        // Init random generator.
        for (uint i = 0; i < 100; i++) cOSRand::rand();

        // Start tests
        test_socket();
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// The trh
volatile cTestSockets::RETCODE cTestSockets::m_msg = cTestSockets::THREAD_OK;

// Instance test object
cTestSockets g_globalTestSocket;
