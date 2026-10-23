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

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file socketStream.h
 * @brief Declare the basic stream for the socket stream class.
 *
 * @details Implements the Berkeley socket functionality and the WSA socket
 *    classes.
 *
 * Compiled on Windows platforms and UNIX platforms
 *
 * @author Elad Raz
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


/**
 * @brief Initialize the TCP stack on the constructor (valid for Windows
 *    programs).
 *
 * @details This class is a singleton so there aren't any duplicate calls for
 *    the initialization
 *
 * The class should init the operating-system "sockets" library at the first
 * call to the constructor. "atexit" the function should free the library. The
 * class contains expand to init each TCP connection individually using the
 * "init" function
 */
class socketInitSingleton
{
public:
    // Public functions

    /**
     * @brief Destructor. Free up the socket library.
     *
     * @details For windows this function call WSACleanup().
     */
    ~socketInitSingleton();

    /**
     * @brief Return a pointer to a constructor singleton in the system.
     *
     * @return A pointer to a constructor singleton in the system.
     */
    static socketInitSingleton& GetHandle();

    /**
     * @brief Init a new call to the sockets library.
     */
    static void init();

private:
    /**
     * @brief Private constructor. Only one object can be constructed at run
     *    time.
     */
    socketInitSingleton();

    /**
     * @brief Prevent copy-constructor and operator =
     */
    socketInitSingleton(const socketInitSingleton& other);
    socketInitSingleton& operator = (const socketInitSingleton& other);

#ifdef XSTL_WINDOWS
    /**
     * @brief The init data.
     */
    WSADATA data;
#endif
};


/**
 * @brief Default time-out definitions.
 */
extern const TIMEVAL INFINITE_TIMEOUT;       ///< 0xFFFFFFFF seconds
extern const TIMEVAL DEFAULT_READ_TIMEOUT;    ///< 1 second timeout
extern const TIMEVAL DEFAULT_WRITE_TIMEOUT;   ///< 1 second timeout


/**
 * @brief Provide simple stream interface to all protocols under the Berkeley
 *   sockets and Winsock API's.
 *
 * @details The implementation of the stream is that the read and the write
 *    function will not be blocking but wait for a specific timeout given in the
 *    constructor.
 */
class cSocketStream : public basicIO
{
public:
    /**
     * @brief Constructor. Construct an empty socket object with initialize
     *    timeout as
     *   arguments.
     *
     * @param readTimeout How many seconds/usec to wait until the reading
     *    operation will return.
     * @param writeTimeout How many seconds/usec to wait until the writing
     *    operation will return.
     */
    cSocketStream(const TIMEVAL readTimeout = DEFAULT_READ_TIMEOUT,
                  const TIMEVAL writeTimeout = DEFAULT_WRITE_TIMEOUT);

    /**
     * @brief Virtual destructor. Close the connection if it exists.
     */
    virtual ~cSocketStream();

    /**
     * @brief Socket will be in network order bits (Little-endian)
     */
    littleEndianImpl;


    // Socket class functions

    /**
     * @brief Init the socket object into a protocol.
     *
     * @details The default arguments creates a TCP socket.
     *
     * Throws cSocketException (Create) exception in case of failure.
     *
     * @param protocol The protocol to be used.
     * @param type Type of using STREAM/DATAGRAM
     * @param af Which network to be used.
     * @param disableNagle True in order to disables the Nagle algorithm
     *    (TCP_NODELAY)
     */
    void create(int protocol = IPPROTO_TCP,
                int type = SOCK_STREAM,
                int af = AF_INET,
                bool disableNagle = false);

    /**
     * @brief Close the created socket.
     */
    void close();

    /**
     * @brief Establish a connection to a peer by its address.
     *
     * @details addr - The cSockAddr struct which describe the connection.
     *
     * Throws cSocketException (Connect) in case of error.
     *
     * @param name The SOCKADDR struct for the connection description.
     * @param size The length of the address.
     */
    void connect(LPSOCKADDR name, unsigned int size);
    /**
     * @brief Establish a connection to a peer by its address.
     *
     * @param addr The cSockAddr struct which describes the connection.
     * @throws cSocketException (Connect) in case of error.
     */
    void connect(const cSockAddr& addr);

    /**
     * @brief Associate address to this socket pre-connection.
     *
     * @details addr - The cSockAddr struct which describe the connection.
     *
     * Throws cSocketException (Bind) in case of error.
     *
     * @param name The SOCKADDR struct for the connection description.
     * @param size The length of the address.
     */
    void bind(LPSOCKADDR name, unsigned int size);
    /**
     * @brief Associate an address to this socket pre-connection.
     *
     * @param addr The cSockAddr struct which describes the connection.
     * @throws cSocketException (Bind) in case of error.
     */
    void bind(const cSockAddr& addr);

    /**
     * @brief Prepare the socket for incoming transmissions.
     *
     * @details Throws cSocketException (Listen) in case of error.
     *
     * @param backlog Defines the maximum number of simultaneous connections.
     */
    void listen(unsigned int backlog = SOMAXCONN);

    /**
     * @brief Wait for a new connection to arrive in the connection table.
     *
     * @details Return the new connection socket in the "newSocket" variable and
     *    the remote address into the "addr" variable with the size limitation.
     *
     * name - The SOCKADDR struct for the connection description.
     *
     * Throws cSocketException (Accept) in case of error.
     *
     * @param size The length of the address.
     * @param addr The cSockAddr struct which describe the connection.
     * @param newSocket Will be filled with the new connection socket.
     */
    void accept(cSocketStream& newSocket, LPSOCKADDR addr, int size);
    /**
     * @brief Wait for a new connection to arrive in the connection table.
     *
     * @param newSocket Will be filled with the new connection socket.
     * @param addr Will be filled with the remote address.
     * @throws cSocketException (Accept) in case of error.
     */
    void accept(cSocketStream& newSocket, cSockAddr& addr);

    /**
     * @brief Timeout related issues
     *
     * @param timeout The new timeout.
     */
    void setReadTimeout (TIMEVAL timeout) { m_readTimeout  = timeout; };
    /**
     * @brief Set the timeout of the write operations.
     *
     * @param timeout The new timeout.
     */
    void setWriteTimeout(TIMEVAL timeout) { m_writeTimeout = timeout; };
    /**
     * @brief Return the timeout of the read operations.
     *
     * @return The timeout of the read operations.
     */
    TIMEVAL getReadTimeout()  { return m_readTimeout;  };
    /**
     * @brief Return the timeout of the write operations.
     *
     * @return The timeout of the write operations.
     */
    TIMEVAL getWriteTimeout() { return m_writeTimeout; };

    /**
     * @brief Return the handler for the socket.
     *
     * @details This function should be used in case the programmer should call
     *    a platform-independent function.
     *
     * @return The handler for the socket.
     */
    SOCKET getHandler() const;


    /* Should handle DATAGRAM messages */

    // These functions are derived from basicIO

    /**
     * @brief Send the data into the socket.
     *
     * @details The function tries to send the data for a period of time, as
     *    defined in the "write-timeout-counter", and returns. In case of an
     *    unexpected close the function throws an exception.
     *
     * Throws cSocketException(Init,Write) for error.
     *
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void *buffer, const uint length);

    /**
     * @brief Read data from a socket.
     *
     * @details The function tries to read data from the socket until an
     *    exception is caught or the timeout is reached.
     *
     * Throws cSocketException(Init,Read) for error.
     *
     * @return The number of bytes read from the device.
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Seek only forward, read bytes into a temporary buffer.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /**
     * @brief Flush the data. Empty implementation in the sockets.
     */
    virtual void flush();

    /**
     * @brief Return true if the connection chopped.
     *
     * @return True if the connection chopped.
     */
    virtual bool isEOS();

    // These functions are not implemented:

    /**
     * @brief Not implemented; throws an exception in debug mode.
     *
     * @details In release the function always returns 0.
     *
     * @return The position of the read/write head, in bytes from the beginning
     *    of the stream.
     */
    virtual uint getPointer() const;

    /**
     * @brief Not implemented. Maybe the function should return the number of
     *    bytes
     *   waiting in the queue.
     *
     * @return The total length of the stream in bytes.
     */
    virtual uint length() const;

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @details This value stands at the MTU/4
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return 256; }

private:
    /**
     * @brief Protected members and functions
     */
    SOCKET m_handle;
    bool   m_isInit;
    bool   m_isClosed;

    /**
     * @brief The timeout for the reading/writing data.
     */
    TIMEVAL m_readTimeout;
    TIMEVAL m_writeTimeout;

    /**
     * @brief Prevent copy constructor and operator =
     */
    cSocketStream(const cSocketStream& other);
    cSocketStream & operator = (const cSocketStream& other);

protected:
    /**
     * @brief Only seeking forward is allowed.
     */
    virtual void initSeek();
};


/** @} */
#endif // __TBA_STL_STREAM_SOCKET_H
