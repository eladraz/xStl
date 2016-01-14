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
 * mutex.cpp
 *
 * Implementation file for UNIX operating system using POSIX API
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/mutex.h"
#include "xStl/os/osdef.h"

#if defined(XSTL_LINUX)

cMutex::cMutex()
{
    // Create the mutex
    pthread_mutexattr_init(&m_handle.m_mutexAttr);
    pthread_mutexattr_settype(&m_handle.m_mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    int rv = pthread_mutex_init(&m_handle.m_mutex, &m_handle.m_mutexAttr);
    if (rv != 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
}

cMutex::~cMutex()
{
    pthread_mutex_destroy(&m_handle.m_mutex);
    pthread_mutexattr_destroy(&m_handle.m_mutexAttr);
}

void cMutex::lock()
{
    if (pthread_mutex_lock(&m_handle.m_mutex) != 0)
    {
        // Mutex locking faild
        XSTL_THROW(cException, EXCEPTION_VIOLATION);
    }
}

void cMutex::unlock()
{
    if (pthread_mutex_unlock(&m_handle.m_mutex) != 0)
    {
        // The mutex handle was faild
        XSTL_THROW(cException, EXCEPTION_VIOLATION);
    }
}

#endif // LINUX
