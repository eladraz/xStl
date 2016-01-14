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

#include "xStl/xStlPrecompiled.h"
/*
 * socketaddr.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/stream/socketAddr.h"
#include "xStl/stream/socketStream.h"
#include "xStl/os/mutex.h"
#include "xStl/os/lock.h"

cSockAddr::~cSockAddr()
{
}

cSockAddrIn::cSockAddrIn()
{
    /* Clear the struct */
    sin_family = AF_INET;
    sin_port = 0;
    memset(&sin_addr, 0, sizeof(sin_addr));
    memset(&sin_zero, 0, sizeof(sin_zero));
}

cSockAddrIn::cSockAddrIn(const cSockAddrIn& other)
{
    // The class is already constructed so this function call is valid.
    *this = other;
}

cSockAddrIn::cSockAddrIn(const cString& name, WORD port /* = 0 */)
{
    sin_family = AF_INET;
    sin_port   = htons(port);

    // Covert to ASCII
    cSArray<char> asciiName = name.getASCIIstring();

    #ifndef XSTL_LINUX
        sin_addr.S_un.S_addr = inet_addr(asciiName.getBuffer());
        if (sin_addr.S_un.S_addr == INADDR_NONE)
    #else
        sin_addr.s_addr = inet_addr(asciiName.getBuffer());
        if (sin_addr.s_addr == INADDR_NONE)
    #endif
    {
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_ADDRESS_ERROR);
    }
    // Free memory
    memset(&sin_zero, 0, sizeof(sin_zero));
}

cSockAddrIn::cSockAddrIn(DWORD addr, WORD port /* = 0 */)
{
    sin_family = AF_INET;
    sin_port  = htons(port);
    #ifndef XSTL_LINUX
        sin_addr.S_un.S_addr = htonl(addr);
    #else
        sin_addr.s_addr = htonl(addr);
    #endif
    memset(&sin_zero, 0, sizeof(sin_zero));
}



/*
 * cSockAddrIn::operator =
 *
 * Copy the class parametrs from one class to the other.
 */
cSockAddrIn & cSockAddrIn::operator = (const cSockAddrIn & other)
{
    if (this != &other)
    {
        /* Copy the struct */
        this->sin_family = other.sin_family;
        this->sin_port   = other.sin_port;
        memcpy(&(this->sin_addr), &(other.sin_addr), sizeof(sin_addr));
        memcpy(&(this->sin_zero), &(other.sin_zero), sizeof(sin_zero));
    }
    return *this;
}

cSockAddrIn::~cSockAddrIn()
{
}

DWORD cSockAddrIn::getIPAddr() const
{
    #if defined(XSTL_WINDOWS) || defined (XSTL_CE)
        return ntohl(this->sin_addr.S_un.S_addr);
    #else
    #ifdef XSTL_LINUX
        return ntohl(this->sin_addr.s_addr);
    #else
        #error "Add platform functionalty here!";
    #endif
    #endif
};

void cSockAddrIn::setIPPort(const WORD port)
{
    this->sin_port = htons(port);
}

WORD cSockAddrIn::getIPPort() const
{
    return ntohs(this->sin_port);
}

cString cSockAddrIn::getName() const
{
    BYTE b1,b2,b3,b4;

    #ifndef XSTL_LINUX
        b1 = sin_addr.S_un.S_un_b.s_b1;
        b2 = sin_addr.S_un.S_un_b.s_b2;
        b3 = sin_addr.S_un.S_un_b.s_b3;
        b4 = sin_addr.S_un.S_un_b.s_b4;
    #else
        b1 = (BYTE)((sin_addr.s_addr & 0x000000FF) >>  0);
        b2 = (BYTE)((sin_addr.s_addr & 0x0000FF00) >>  8);
        b3 = (BYTE)((sin_addr.s_addr & 0x00FF0000) >> 16);
        b4 = (BYTE)((sin_addr.s_addr & 0xFF000000) >> 24);
    #endif

    return cString((int)(b1)) + "." +
           cString((int)(b2)) + "." +
           cString((int)(b3)) + "." +
           cString((int)(b4)) + ":" + cString((int)(ntohs(sin_port)));
}

LPSOCKADDR cSockAddrIn::getAddr() const
{
    return (LPSOCKADDR)((SOCKADDR_IN *)(this));
}

unsigned int cSockAddrIn::getSize() const
{
    return sizeof(SOCKADDR_IN);
}


/*
 * The global mutex to be lock the 'gethostbyname' function
 */
static cMutex g_getHostByNameLock;

cSockAddrIn cSockAddrIn::getHostByName(const cString& name, WORD port /* = 0 */)
{
    socketInitSingleton::GetHandle().init();
    cSockAddrIn ret;

    ret.sin_family = AF_INET;
    ret.sin_port = htons(port);

    // Lock the mutex
    cLock lock(g_getHostByNameLock);

    // Query the address.
    cSArray<char> asciiName = name.getASCIIstring();
    hostent* addr = gethostbyname(asciiName.getBuffer());

    // Return the first address of the hostent struct.
    #if defined(XSTL_WINDOWS) || defined (XSTL_CE)
        // Windows ordering.
        ret.sin_addr.S_un.S_addr = *((DWORD *)(addr->h_addr_list[0]));
    #else
    #ifdef XSTL_LINUX
        // Linux version
        ret.sin_addr.s_addr = *((DWORD *)(addr->h_addr_list[0]));
    #else
        #error "Add platform functionalty here!";
    #endif
    #endif

    return ret;
}

void cSockAddrIn::getAllHostsName(const cString& name,
                                  cList<cSockAddrIn>& lists,
                                  WORD port /* = 0 */)
{
    socketInitSingleton::GetHandle().init();
    cSockAddrIn ret; // The new constructor INET addressed
    ret.sin_family = AF_INET;
    ret.sin_port   = htons(port);

    lists.removeAll();

    // Lock the mutex
    cLock lock(g_getHostByNameLock);

    /* Invoke the 'gethostname' function call */
    /* Recieve a list of all address          */
    cSArray<char> asciiName = name.getASCIIstring();
    hostent * addr = gethostbyname(asciiName.getBuffer());

    for (unsigned int i = 0; ((DWORD *)addr->h_addr_list[i] != NULL) ; i++)
    {
        /* Get the address */
        #ifndef XSTL_LINUX
            ret.sin_addr.S_un.S_addr = *((DWORD *)(addr->h_addr_list[i])); // Return the first address
        #else
            ret.sin_addr.s_addr = *((DWORD *)(addr->h_addr_list[i])); // Return the first address
        #endif

        /* Fill with new valid address */
        lists.append(ret);
    }
}
