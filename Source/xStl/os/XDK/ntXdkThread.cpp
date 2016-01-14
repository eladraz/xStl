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
 * XDK OS Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/osDef.h"
#include "xStl/os/thread.h"
#include "xStl/os/threadedClass.h"
#include "XDK/kernel.h"

void cThread::execute(ThreadRoutine lpStartAddress, void * args)
{
    if (m_thread != INVALID_THREAD_HANDLE) {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }

    HANDLE hThread;
    NTSTATUS status = PsCreateSystemThread(&hThread,
        THREAD_ALL_ACCESS,
        NULL,
        NULL,
        NULL,
        (PKSTART_ROUTINE)lpStartAddress,
        args);

    if (!NT_SUCCESS(status))
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    ObReferenceObjectByHandle(hThread,
                              THREAD_ALL_ACCESS,
                              NULL,
                              KernelMode,
                              (PVOID*)&m_thread,
                              NULL);
    ZwClose(hThread);
}

void cThread::wait()
{
    if (m_thread == INVALID_THREAD_HANDLE)
        return;

    KeWaitForSingleObject(m_thread, Executive, KernelMode, FALSE, NULL);
}

void cThread::kill()
{
    // TODO! I'm not sure that this function call will work, but I couldn't
    //       find any other documented function!
    ObDereferenceObject(m_thread);
    m_thread = INVALID_THREAD_HANDLE;
}

cThread::~cThread()
{
    if (m_thread != INVALID_THREAD_HANDLE)
        kill();
}

cOSDef::threadHandle cThread::getCurrentThreadHandle()
{
    //return KeGetCurrentThread();

    // this implementation is taken from ReactOS src
    cOSDef::threadHandle thread;
    _asm {
        mov eax, fs:[0x124] // KPCR.PrcbData.CurrentThread
        mov thread, eax
    }
    return thread;
}

void cThread::terminate()
{
    testPageableCode();
    PsTerminateSystemThread(STATUS_SUCCESS);
}

