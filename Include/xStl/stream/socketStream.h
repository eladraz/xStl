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

#ifndef __TBA_STL_STREAM_SOCKET_H
#define __TBA_STL_STREAM_SOCKET_H

/*
 * socketstream.h
 *
 * Declare the basic stream for socket's  stream class. Implement the brekley
 * socket functionality and the WSA socket classes.
 *
 * Compiled on Windows platforms and UNIX's
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/list.h"
#include "xStl/data/endian.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/socketException.h"
#include "xStl/stream/socketAddr.h"

#if defined(XSTL_WINDOWS) || defined (XSTL_CE)
    /*
     * Windows version
     * You must include the ws2_32.lib
     */
    #include <Winsock.h>
#else
#ifdef XSTL_LINUX
    typedef int SOCKET;

    #undef __USE_MISC
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>

    #ifndef INVALID_SOCKET
    #define INVALID_SOCKET (-1)
    #endif

    #define SOCKET_ERROR (-1)

    typedef struct timeval TIMEVAL;
#else
    // Unknown version
    #error "Unknown platform for files!\n"
#endif
#endif


/*
 * class socketInitSingleton
 *
 * Initialize the TCP stack on the constructor (Valid for Windows program). This
 * class is a singleton so there isn't any duplication calls for the
 * initialization
 *
 * The class should init the operation-system "sockets" library at the first
 * call to the constructor. "atexit" the function should free the library. The
 * class contains expand to init each TCP connection individually using the
 * "init" function
 */
class socketInitSingleton
{
public:
    // Public functions

    /*
     * Destructor. Free up the socket library. For windows this function
     * call to the WSACleanup().
     */
    ~socketInitSingleton();

    /*
     * Return a pointer to a constructor singleton in the system.
     */
    static socketInitSingleton& GetHandle();

    /*
     * Init a new call to the sockets library.
     */
    static void init();

private:
    /*
     * Private constructor. Only one object can be constructed at run time.
     */
    socketInitSingleton();

    /*
     * Prevent copy-constructor and operator =
     */
    socketInitSingleton(const socketInitSingleton& other);
    socketInitSingleton& operator = (const socketInitSingleton& other);

#ifdef XSTL_WINDOWS
    // The init data.
    WSADATA data;
#endif
};


/*
 * Default time-out definitions.
 */
extern const TIMEVAL INFINITIE_TIMEOUT;       // 0xFFFFFFFF seconds
extern const TIMEVAL DEFAULT_READ_TIMEOUT;    // 1 second timeout
extern const TIMEVAL DEFAULT_WRITE_TIMEOUT;   // 1 second timeout


/*
 * class cSocketStream
 *
 * Provide simple stream interface to all protocols under the Berkley sockets
 * and Winsock API's. The implementation of the stream is that the read and
 * the write function will not be blocking but wait for a specific timeout
 * given in the constructor.
 */
class cSocketStream : public basicIO
{
public:
    /*
     * Constructor. Construct an empty socket object with initialize timeout
     * as arguments.
     *
     * readTimout - How many seconds/usec to be wait until the reading operation
     *              will return.
     * writeTimeout - How many seconds/usec to be wait until the writing
     *                operation will return.
     */
    cSocketStream(const TIMEVAL readTimeout = DEFAULT_READ_TIMEOUT,
                  const TIMEVAL writeTimeout = DEFAULT_WRITE_TIMEOUT);

    /*
     * Virtual destructor. close the connection if exist.
     */
    virtual ~cSocketStream();

    /*
     * Socket will be in network order bits (Little-endian)
     */
    littleEndianImpl;


    // Socket class functions

    /*
     * Init the socket object into a protocol. The default arguments creates a
     * TCP socket.
     *
     * protocol     - The protocol to be used.
     * type         - Type of using STREAM/DATAGRAM
     * af           - Which network to be used.
     * disableNagle - True in order to disables the Nagle algorithm (TCP_NODELAY)
     *
     * Throws cSocketException (Create) exception in case of failure.
     */
    void create(int protocol = IPPROTO_TCP,
                int type = SOCK_STREAM,
                int af = AF_INET,
                bool disableNagle = false);

    /*
     * Close the created socket.
     */
    void close();

    /*
     * Establish connection to a peer by it's address.
     *
     * name - The SOCKADDR struct for the connection description.
     * size - The length of the address.
     *
     * addr - The cSockAddr struct which describe the connection.
     *
     * Throws cSocketException (Connect) in case of error.
     */
    void connect(LPSOCKADDR name, unsigned int size);
    void connect(const cSockAddr& addr);

    /*
     * Associate address to this socket pre-connection.
     *
     * name - The SOCKADDR struct for the connection description.
     * size - The length of the address.
     *
     * addr - The cSockAddr struct which describe the connection.
     *
     * Throws cSocketException (Bind) in case of error.
     */
    void bind(LPSOCKADDR name, unsigned int size);
    void bind(const cSockAddr& addr);

    /*
     * Prepare the socket for incoming transmissions.
     *
     * backlog - Defines the maximum available connection simultaneous.
     *
     * Throws cSocketException (Listen) in case of error.
     */
    void listen(unsigned int backlog = SOMAXCONN);

    /*
     * Wait for new connection to arrive to the connection table, Return the new
     * connection socket in the "newSocket" variable
     * and the remote address into the "addr" variable with the size
     * limitation.
     *
     * name - The SOCKADDR struct for the connection description.
     * size - The length of the address.
     *
     * addr - The cSockAddr struct which describe the connection.
     *
     * Throws cSocketException (Accept) in case of error.
     */
    void accept(cSocketStream& newSocket, LPSOCKADDR addr, int size);
    void accept(cSocketStream& newSocket, cSockAddr& addr);

    // Timeout related issues
    void setReadTimeout (TIMEVAL timeout) { m_readTimeout  = timeout; };
    void setWriteTimeout(TIMEVAL timeout) { m_writeTimeout = timeout; };
    TIMEVAL getReadTimeout()  { return m_readTimeout;  };
    TIMEVAL getWriteTimeout() { return m_writeTimeout; };

    /*
     * Return the handler for the socket. This function should be used in case
     * the programmer should call to a platform independed function.
     */
    SOCKET getHandler() const;


    /* Should handle DATAGRAM messages */

    // These functions are driven from basicIO

    /*
     * Send the data into the socket. The function try to send the data for a
     * period time, as defined in the "write-timeout-counter" and return.
     * In case of unexpected close. The function throws exception.
     *
     * Throws cSocketException(Init,Write) for error.
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Read a data from a socket. The function try to read from the socket data
     * until exception was caught or timeout has reached.
     *
     * Return the number of bytes read from the device.
     * Throws cSocketException(Init,Read) for error.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Seek only forward, read bytes into a temporary buffer.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /*
     * Flush the data. Empty implementation in the sockets.
     */
    virtual void flush();

    /*
     * Return true if the connection chopped.
     */
    virtual bool isEOS();

    // These function are not implemented:

    /*
     * Not implement throw exception in debug mode. In release the function
     * always return 0.
     */
    virtual uint getPointer() const;

    /*
     * Not implement.
     * Maybe the function should return the number of bytes waiting in the queue.
     */
    virtual uint length() const;

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     * This value stand on the MTU/4
     */
    virtual uint getPipeReadBestRequest() const { return 256; }

private:
    /* Protected members and functions */
    SOCKET m_handle;
    bool   m_isInit;
    bool   m_isClosed;

    // The timeout for the reading/writing data.
    TIMEVAL m_readTimeout;
    TIMEVAL m_writeTimeout;

    // Prevent copy constructor and operator =
    cSocketStream(const cSocketStream& other);
    cSocketStream & operator = (const cSocketStream& other);

protected:
    // Only seeking forward is allowed.
    virtual void initSeek();
};

#endif // __TBA_STL_STREAM_SOCKET_H
