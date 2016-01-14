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
* socketstream.cpp
*
* Implementation file.
*
* Author: Elad Raz <e@eladraz.com>
*/
#include "xStl/types.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/socketStream.h"
#include "xStl/stream/traceStream.h"

#ifdef XSTL_LINUX
#include <sys/select.h>
#include <netinet/tcp.h>
#define SO_DONTLINGER   (uint)(~SO_LINGER)
#endif

/*
* The default timeout values.
*/
const TIMEVAL INFINITIE_TIMEOUT = {-1, -1};   // 0xFFFFFFFF seconds
const TIMEVAL DEFAULT_READ_TIMEOUT = {1,0};   // 1 second timeout
const TIMEVAL DEFAULT_WRITE_TIMEOUT = {1,0};  // 1 second timeout

/***
* socketInitSingleton implementation.
*/
socketInitSingleton::~socketInitSingleton()
{
#ifdef XSTL_WINDOWS
    /* Windows cleanup */
    WSACleanup();
#endif
}

socketInitSingleton& socketInitSingleton::GetHandle()
{
    static socketInitSingleton m_handle;
    return m_handle;
}

void socketInitSingleton::init()
{
}

socketInitSingleton::socketInitSingleton()
{
#ifdef XSTL_WINDOWS
    WSAStartup(0x0101, &data);
#endif
}



/***
* cSocketStream implementation.
*/
cSocketStream::cSocketStream(const TIMEVAL readTimeout /* = DEFAULT_READ_TIMEOUT */,
    const TIMEVAL writeTimeout /* = DEFAULT_WRITE_TIMEOUT */)
{
    socketInitSingleton::GetHandle().init();
    m_handle = INVALID_SOCKET;
    m_isInit = false;
    m_isClosed = false;

    m_writeTimeout = writeTimeout;
    m_readTimeout  = readTimeout;

    initSeek();
}

cSocketStream::~cSocketStream()
{
    close();
}


void cSocketStream::initSeek()
{
    canLength        = false;
    canSeekFromBegin = false;
    canSeekForward   = true;
    canSeekBackward  = false;
    canSeekFromEnd   = false;
    canGetPointer    = false;
}

SOCKET cSocketStream::getHandler() const
{
    return m_handle;
};

void cSocketStream::create(int protocol, /* = IPPROTO_TCP, */
    int type,     /* = SOCK_STREAM, */
    int af        /* = AF_INET      */,
    bool disableNagle)
{
    if (m_isInit)
    {
        close();
    }
    m_handle = socket(af, type, protocol);

    if (m_handle == INVALID_SOCKET)
    {
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_CREATION_FAILD);
    }

    if (disableNagle)
    {
        int flags = 1;
        if (setsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY,
                       (const char*)&flags, sizeof(flags)) != 0)
        {
            traceHigh("cSocketStream::create(): TCP_NODELAY failed" << endl);
            // No need to throw exception
        }

        if (setsockopt(m_handle, SOL_SOCKET, SO_DONTLINGER,
                       (const char*)&flags, sizeof(flags)) != 0)
        {
            traceHigh("cSocketStream::create(): SO_DONTLINGER failed" << endl);
        }

        flags = FALSE;
        if (setsockopt(m_handle, SOL_SOCKET, SO_DEBUG,
                       (const char*)&flags, sizeof(flags)) != 0)
        {
            traceHigh("cSocketStream::create(): SO_DEBUG failed" << endl);
        }
    }

#ifdef XSTL_LINUX
    int flags = 1;
    if (setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR,
                   (const char*)&flags, sizeof(flags)) != 0)
    {
        traceHigh("cSocketStream::create(): SO_REUSEADDR failed" << endl);
    }
#endif

    m_isInit = true;
}

void cSocketStream::close()
{
    if (m_isInit)
    {
#if defined(XSTL_WINDOWS) || defined (XSTL_CE)
        // Windows have a special function for closing a socket
        closesocket(m_handle);
#else
#ifdef XSTL_LINUX
        ::shutdown(m_handle, SHUT_RDWR);
        ::close(m_handle);
#else
#error  "Over here!"
#endif
#endif
        m_isInit = false;
    }
}

void cSocketStream::connect(LPSOCKADDR name, unsigned int size)
{
    // Preform the socket call
    int ret = ::connect(m_handle, name, size);

    if (ret != 0)
    {
        // Error at connection
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_CONNECT);
    }
}

void cSocketStream::connect(const cSockAddr& addr)
{
    connect(addr.getAddr(), addr.getSize());
}

void cSocketStream::bind(LPSOCKADDR name, unsigned int size)
{
    // Preform the socket call
    int ret = ::bind(m_handle, name, size);

    if (ret != 0)
    {
        // Error at connection
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_BIND);
    }
}

void cSocketStream::bind(const cSockAddr& addr)
{
    bind(addr.getAddr(), addr.getSize());
}

void cSocketStream::listen(unsigned int backlog /* = SOMAXCONN */)
{
    int ret = ::listen(m_handle, backlog);

    if (ret != 0)
    {
        // Error at connection
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_LISTEN);
    }
}

void cSocketStream::accept(cSocketStream & newSocket, LPSOCKADDR addr, int size)
{
    // Close the previous connection, if exist
    newSocket.close();

#if defined(XSTL_WINDOWS) || defined (XSTL_CE)
    // Windows argument
    SOCKET mRet = ::accept(m_handle, addr, &size);
#else
#ifdef XSTL_LINUX
    // LINUX Parameters
    socklen_t ssize = size;
    SOCKET mRet = ::accept(m_handle, addr, &ssize);
#else
#error "Over here!"
#endif
#endif

    if (mRet == INVALID_SOCKET)
    {
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_ACCEPT);
    }

    newSocket.m_isInit = true;
    newSocket.m_handle = mRet;
}

void cSocketStream::accept(cSocketStream & newSocket, cSockAddr& addr)
{
    accept(newSocket, addr.getAddr(), addr.getSize());
}


uint cSocketStream::length() const
{
    ASSERT(false);
    if (m_isInit)
    {
        fd_set  readfds;
        TIMEVAL timeout = {0,0}; /* Don't wait */

        FD_ZERO(&readfds);
        FD_SET (m_handle, &readfds);

        select((int)(m_handle) + 1, &readfds, NULL, NULL, &timeout);

        if (FD_ISSET(m_handle, &readfds))
        {
            return 1;
        } else
        {
            // Timeout
            return 0;
        }
    } else
    {
        XSTL_THROW(cSocketException, SOKCETEXCEPRION_NOT_INIT);
        return 0;
    }
}

uint cSocketStream::getPointer() const
{
    ASSERT(false);
    return 0;
}

uint cSocketStream::write(const void *buffer, const uint length)
{
    unsigned int written = 0;

    // Check whether the socket is OK
    if (m_isInit)
    {
        // Write FD
        fd_set writefd;
        FD_ZERO(&writefd);
        FD_SET (m_handle, &writefd);

        // Error FD
        fd_set errorfd;
        FD_ZERO(&errorfd);
        FD_SET (m_handle, &errorfd);

        select((int)(m_handle) + 1, NULL, &writefd, &errorfd, &m_writeTimeout);

        if (m_isClosed || FD_ISSET(m_handle, &errorfd))
        {
            // An error occurred or the socket closed
            XSTL_THROW(cSocketException, SOCKETEXCEPTION_WRITE_FAILD);
        }

        // Check for write permissions
        if (!FD_ISSET(m_handle, &writefd))
        {
            // Timeout reached.
            return 0;
        }

        if((written = send(m_handle, (const char *)buffer, length, 0)) == (unsigned int)SOCKET_ERROR)
        {
            XSTL_THROW(cSocketException, SOCKETEXCEPTION_WRITE_FAILD);
        }
        return written;
    } else
    {
        XSTL_THROW(cSocketException, SOKCETEXCEPRION_NOT_INIT);
        return 0;
    }
}

uint cSocketStream::read(void *buffer, const uint length)
{
    unsigned int readed = 0;

    if (!m_isInit)
    {
        XSTL_THROW(cSocketException, SOKCETEXCEPRION_NOT_INIT);
    }

    fd_set readfd ;
    FD_ZERO(&readfd);
    FD_SET (m_handle, &readfd);

    // Error FD
    fd_set errorfd;
    FD_ZERO(&errorfd);
    FD_SET (m_handle, &errorfd);

    select((int)(m_handle) + 1, &readfd, NULL, &errorfd, &m_readTimeout);

    if (m_isClosed || FD_ISSET(m_handle, &errorfd))
    {
        // An error occurred or the socket closed
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_READ_FAILD);
    }

    if (!FD_ISSET(m_handle, &readfd))
    {
        return 0;
    }

    if ((readed = recv(m_handle, (char *)buffer, length, 0)) == (unsigned int)SOCKET_ERROR)
    {
        XSTL_THROW(cSocketException, SOCKETEXCEPTION_READ_FAILD);
    }

    return readed;
}

void cSocketStream::seek(const int distance, const basicInput::seekMethod method)
{
    if (!__canSeek(distance, method))
    {
        XSTL_THROW(cException, EXCEPTION_SEEK_ERROR);
    }

    // Distance is positive and method is IO_SEEK_CUR
    cBuffer temp;
    pipeRead(temp, distance);
}

void cSocketStream::flush()
{
}

bool cSocketStream::isEOS()
{
    fd_set errorfd;
    FD_ZERO(&errorfd);
    FD_SET (m_handle, &errorfd);

    struct timeval zero_time = {0,1};
    select((int)(m_handle) + 1, NULL, NULL, &errorfd, &zero_time);

    if (!FD_ISSET(m_handle, &errorfd))
    {
        // Check whether the socket close gracefully
        if (m_isClosed)
        {
            return true;
        }

        // There isn't any errors.
        return false;
    }

    // An error had occurred
    return true;
}

