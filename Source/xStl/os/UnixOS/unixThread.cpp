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
 * thread.cpp
 *
 * Linux POSIX implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/osdef.h"
#include "xStl/os/thread.h"

// sys/types.h in some compilers has a difinitions of uint
#undef uint
#include <signal.h>
#include <unistd.h>

cThread::~cThread()
{
}

void cThread::execute(ThreadRoutine lpStartAddress, void * args)
{
    if (m_thread != INVALID_THREAD_HANDLE) {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }

    /* POSIX implementation */
    int retVal = pthread_create(&m_thread, NULL, lpStartAddress, args);
    if (retVal != 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
}

void cThread::wait()
{
    if (m_thread == INVALID_THREAD_HANDLE)
        return;

    /* POSIX implementation */
    pthread_join(m_thread, NULL);
    m_thread = INVALID_THREAD_HANDLE;
}

void cThread::kill()
{
    /* POSIX implementation */
    pthread_cancel(m_thread);
    pthread_join(m_thread, NULL);
    m_thread = INVALID_THREAD_HANDLE;
}

cOSDef::threadHandle cThread::getCurrentThreadHandle()
{
    return pthread_self();
}


void cThread::terminate()
{
    pthread_exit(NULL);
}
