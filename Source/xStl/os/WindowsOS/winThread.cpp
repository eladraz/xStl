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

#include "xStl/xStlPrecompiled.h"
// For this files, the WIN32 or the _WIN32_WCE macro must be defined.
// Only Windows operating system or Windows CE can compile with this API
#if defined(WIN32) || defined(_WIN32_WCE)

/*
 * thread.cpp
 *
 * Win32 OS Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/osDef.h"
#include "xStl/os/thread.h"
#include "xStl/os/threadedClass.h"

void cThread::execute(ThreadRoutine lpStartAddress, void * args)
{
    if (m_thread != INVALID_THREAD_HANDLE) {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }

    /* Windows threads implementation */
    m_thread = CreateThread(NULL,
        0,
        (LPTHREAD_START_ROUTINE)lpStartAddress,
        args,
        0,
        NULL);
    if (m_thread == NULL)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
}

void cThread::wait()
{
    if (m_thread == INVALID_THREAD_HANDLE)
        return;

    /* Windows threads implementation */
    WaitForSingleObject(m_thread, INFINITE);
}

void cThread::kill()
{
    /* Windows threads implementation */
    TerminateThread(m_thread, -1); // Exit KILL
    // Wait until the thread exits
    WaitForSingleObject(m_thread, INFINITE);
}

cThread::~cThread()
{
    if (m_thread != INVALID_THREAD_HANDLE)
        CloseHandle(m_thread);
}

cOSDef::threadHandle cThread::getCurrentThreadHandle()
{
    return GetCurrentThread();
}

void cThread::terminate()
{
    // Win32 thread terminates by invoking ret command.
}

#endif // WIN32
