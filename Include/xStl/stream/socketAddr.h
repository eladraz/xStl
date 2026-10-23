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

#ifndef __TBA_STL_STREAM_SOCKET_SOCKETADDR_H
#define __TBA_STL_STREAM_SOCKET_SOCKETADDR_H

/**
 * @addtogroup xstl_stream
 * @{
 */


/**
 * @file socketAddr.h
 * @brief Declare the basic class of all socket address.
 *
 * @details Also include the implementation of the TCP/IP protocol stack,
 *    address struct
 *
 * @author Elad Raz
 */
#ifdef __APPLE__
#include <sys/types.h>
#endif

#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/list.h"
#include "xStl/os/mutex.h"
#include "xStl/stream/socketException.h"


#if defined(XSTL_WINDOWS) || defined (XSTL_CE)
    /* Windows socket include files */
    #include <Winsock.h>
#else
#ifdef XSTL_LINUX
    /* UNIX socket include files */
    #undef __USE_MISC
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    typedef struct sockaddr_in SOCKADDR_IN;
#else
    // Unknown version
    #error "Unknown platform for files!\n"
#endif
#endif


/// The socket address structure of the platform.
typedef sockaddr    SOCKADDR;
/// A pointer to a SOCKADDR.
typedef sockaddr * LPSOCKADDR;


/**
 * @brief Contains private delegation of the SOCKADDR struct.
 *
 * @details Implement operator SOCKADDR, LPSOCKADDR for using the class inside
 *    normal operations.
 *
 * Recommended functions that should be implemented for any derived class:
 * - cSockAddr() - Default constructor.
 * - cSockAddr(const cSockAddr\& other) - Copy constructor
 * - cSockAddr(const SOCKADDR\&  other) - Copy constructor from struct
 * - cSockAddr \& operator = (const cSockAddr \&other) - operator =
 * - cSockAddr \& operator = (const SOCKADDR\& other) - operator =
 */
class cSockAddr
{
public:
    /**
     * @brief Virtual destructor;
     */
    virtual ~cSockAddr();

    /**
     * @brief Return a name describing the addressing
     *
     * @return A name describing the addressing.
     */
    virtual cString getName() const = 0;

    /**
     * @brief Return the size of the virtual SOCKADDR struct
     *
     * @return The size of the virtual SOCKADDR struct.
     */
    virtual unsigned int getSize() const = 0;

    /**
     * @brief Return the pointer to the SOCKADDR struct.
     *
     * @details The size of the struct can be queried using the getSize
     *    function.
     *
     * @return The pointer to the SOCKADDR struct.
     */
    virtual LPSOCKADDR getAddr() const = 0;
};


/**
 * @brief Provide simple TCP/IP addressing name.
 *
 * @details Implement the SOCKADDR_IN struct and the cSockAddr interface.
 */
class cSockAddrIn : public SOCKADDR_IN,
                    public cSockAddr
{
public:
    /* Public functions */

    /**
     * @brief Default constructor. Create empty SOCKADDR_IN struct.
     */
    cSockAddrIn();

    /**
     * @brief Copy constructor.
     *
     * @param other The address to copy.
     */
    cSockAddrIn(const cSockAddrIn &other);

    /**
     * @brief Create a new socket according to a dotted IP string.
     *
     * @param name Dotted IP address. 4 decimal numbers separated by '.'
     * @param port The port for the connection.
     * @throws cException (SOCKETEXCEPTION_ADDRESS_ERROR) if the address is
     *    invalid
     */
    cSockAddrIn(const cString & name, WORD port = 0);

    /**
     * @brief Create a new socket address according to host order IP and a port.
     *
     * @param addr Host-endian IP address.
     * @param port The port for the connection.
     */
    cSockAddrIn(DWORD addr, WORD port = 0);

    /**
     * @brief Virtual destructor.
     */
    virtual ~cSockAddrIn();

    // cSockAddr addressing function

    /**
     * @brief Return a name for the connection.
     *
     * @return A name for the connection.
     */
    virtual cString getName() const;

    /**
     * @brief Return the size of the SOCKADDR struct.
     *
     * @return The size of the SOCKADDR struct.
     */
    virtual unsigned int getSize() const;


    // cSockAddrIn functions.

    /**
     * @brief Return the IP address of the connection in host-endian.
     *
     * @return The IP address of the connection in host-endian.
     */
    DWORD getIPAddr() const;

    /**
     * @brief Return the port of the connection in host computer endian.
     *
     * @return The port of the connection in host computer endian.
     */
    WORD getIPPort() const;

    /**
     * @brief Change the connection port for the address object.
     *
     * @param port The new port for the address object.
     */
    void setIPPort(const WORD port);

    /**
     * @brief Perform a DNS query for internet address and return a constructed
     *    address
     *   to it.
     *
     * @details The function locks the call to the "gethostbyname" sys-call with
     *    a mutex, so it is safe to call this class from a multi-threaded
     *    process.
     *
     * Throws
     *
     * @param name DNS name of the connection destination.
     * @param port The port to be attached to the address object.
     * @return The address object of the host.
     */
    static cSockAddrIn getHostByName(const cString& name,
                                     WORD port = 0);

   /**
    * @brief Perform a DNS query for internet address and return a constructed
    *    address
    *   objects for all queried translated addresses.
    *
    * @details The function locks the call to the "gethostbyname" sys-call with
    *    a mutex, so it is safe to call this class from a multi-threaded
    *    process.
    *
    * @param name DNS name of the connection destination.
    * @param lists Will be filled with a list of all translated address objects.
    *    The list will be cleared before using it.
    * @param port The port to be attached to the address object.
    */
    static void getAllHostsName(const cString& name,
                                cList<cSockAddrIn> &lists,
                                WORD port = 0);

    /**
     * @brief Return the pointer to the SOCKADDR_IN struct.
     *
     * @details The size of the struct can be queried using the getSize
     *    function.
     *
     * @return The pointer to the SOCKADDR_IN struct.
     */
    virtual LPSOCKADDR getAddr() const;

    /* Operators */

    /**
     * @brief Assignment operator.
     *
     * @param other The address to copy from.
     * @return A reference to this object.
     */
    cSockAddrIn & operator = (const cSockAddrIn &other);
};


/** @} */
#endif // __TBA_STL_STREAM_SOCKET_SOCKETADDR_H
