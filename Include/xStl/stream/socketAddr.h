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

#ifndef __TBA_STL_STREAM_SOCKET_SOKCETADDR_H
#define __TBA_STL_STREAM_SOCKET_SOKCETADDR_H


/*
 * sockaddr.h
 *
 * Declare the basic class of all socket address. Also include the
 * implementation of the TCP/IP protocol stack, address struct
 *
 * Author: Elad Raz <e@eladraz.com>
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
    /* Windows sokcet include files */
    #include <Winsock.h>
#else
#ifdef XSTL_LINUX
    /* UINX socket include files */
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


typedef sockaddr    SOCKADDR;
typedef sockaddr * LPSOCKADDR;


/*
 * cSockAddr
 *
 * Contains private delegation of the SOCKADDR struct.
 * Implement operator SOCKADDR, LPSOCKADDR for using the class inside normal
 * operations.
 *
 * Recommanded functions that should be implemented for any driven class:
 *   - cSockAddr() - Default constructor.
 *   - cSockAddr(const cSockAddr& other) - Copy constructor
 *   - cSockAddr(const SOCKADDR&  other) - Copy constructor from struct
 *   - cSockAddr & operator = (const cSockAddr &other) - operator =
 *   - cSockAddr & operator = (const SOCKADDR& other) - operator =
 */
class cSockAddr
{
public:
    /*
     * Virtual destructor;
     */
    virtual ~cSockAddr();

    /*
     * Return a name describing the addressing
     */
    virtual cString getName() const = 0;

    /*
     * Return the size of the virtual SOCKADDR struct
     */
    virtual unsigned int getSize() const = 0;

    /*
     * Return the pointer to the SOCKADDR struct. The size of the struct
     * can be queried using the getSize function.
     */
    virtual LPSOCKADDR getAddr() const = 0;
};


/*
 * class cSockAddrIn
 *
 * Provide simple TCP/IP addresing name. Implement the SOCKADDR_IN struct and
 * the cSockAddr interface.
 */
class cSockAddrIn : public SOCKADDR_IN,
                    public cSockAddr
{
public:
    /* Public functions */

    /*
     * Default constructor. Create empty SOCKADDR_IN struct.
     */
    cSockAddrIn();

    /*
     * Copy constructor.
     */
    cSockAddrIn(const cSockAddrIn &other);

    /*
     * Create a new socket according to a dotted IP string.
     *
     * name - Dotted IP address. 4 decimal numbers seperated by '.'
     * port - The port for the connection.
     *
     * Thorws SOCKETEXCEPTION_ADDRESS_ERROR exception if the address is invalid
     */
    cSockAddrIn(const cString & name, WORD port = 0);

    /*
     * Create a new socket address according to host order IP and a port.
     *
     * addr - Host-endian IP address.
     * port - The port for the connection.
     */
    cSockAddrIn(DWORD addr, WORD port = 0);

    /*
     * Virtual destructor.
     */
    virtual ~cSockAddrIn();

    // cSockAddr addressing function

    /*
     * Return a name for the connection.
     */
    virtual cString getName() const;

    /*
     * Return the size of the SOCKADDR struct.
     */
    virtual unsigned int getSize() const;


    // cSockAddrIn functions.

    /*
     * Return the IP address of the connection in host-endian.
     */
    DWORD getIPAddr() const;

    /*
     * Return the port of the connection in host computer endian.
     */
    WORD getIPPort() const;

    /*
     * Change the connection port for the address object.
     *
     * port - The new port for the address object.
     */
    void setIPPort(const WORD port);

    /*
     * Preform a DNS query for internet address and return a constructed address
     * to it. The function locking the call to the "gethostbyname" sys-call with
     * a mutex, so it is save to call to this class from multi-thread process.
     *
     * name - DNS name of the connection destination.
     * port - The port to be attached to the address object.
     *
     * Throws
     */
    static cSockAddrIn getHostByName(const cString& name,
                                     WORD port = 0);

   /*
    * Preform a DNS query for internet address and return a constructed address
    * objects for all queryed translated address. The function locking the call
    * to the "gethostbyname" sys-call with a mutex, so it is save to call to
    * this class from multi-thread process.
    *
    * name - DNS name of the connection destination.
    * lists - Will be filled with a list of all translated address objectes.
    *         The list will be cleared before using it.
    * port - The port to be attached to the address object.
    */
    static void getAllHostsName(const cString& name,
                                cList<cSockAddrIn> &lists,
                                WORD port = 0);

    /*
     * Return the pointer to the SOCKADDR_IN struct. The size of the struct
     * can be queried using the getSize function.
     */
    virtual LPSOCKADDR getAddr() const;

    /* Operators */
    cSockAddrIn & operator = (const cSockAddrIn &other);
};

#endif // __TBA_STL_STREAM_SOCKET_SOKCETADDR_H
