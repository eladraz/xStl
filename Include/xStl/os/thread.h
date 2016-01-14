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

#ifndef __TBA_STL_OS_THREAD_H
#define __TBA_STL_OS_THREAD_H

/*
 * thread.h
 *
 * Declare virtual class for thread creation. Implement the POSIX interface for
 * UNIX platforms and the Windows Threads command for Windows threads.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/osdef.h"

/*
 * The thread function receives a single pointer as an argument and return
 * a pointer.
 *
 * NOTE: For some operating system, this function shouldn't be returned. It must
 *       be terminate using operating system calls.
 *       For example the XDK (Windows driver kit) thread must be terminated
 *       using a call to 'PsTerminateSystemThread' function.
 *
 * NOTE: This interface doesn't contain any interface for getting the return
 *       code.
 *
 * NOTE: The routine must invoke a call to cThread::terminate in order to
 *       destroy all threads resources. See cThreadedClass implementation
 */
typedef void*(*ThreadRoutine)(void*);

/*
 * Wraper class for thread's API. Allow creating, waiting and killing a thread.
 *
 * NOTE: Don't use this class for threads in your applications. Use the
 *       cThreadedClass instead.
 *
 *       See cThreadedClass.
 *
 * NOTE: The newly constructed thread doesn't close at the destruction
 *       of this class.
 *
 * Using this class:
 *   cThread newThread;
 *   newThread.execute(func, argc);
 *   // Do some things...
 *   newThread.wait();
 *
 * Implemented over:
 *     Win32 API, Windows NT DDK, POSIX (Unix based applications).
 */
class cThread
{
public:
    /*
     * Default constructor. Create empty thread object.
     */
    cThread();

    /*
     * Destructor. The destructor doesn't free any resources.
     * Notice, that the thread can be running without any wrapper class.
     */
    ~cThread();


    // Main function

    /*
     * Create the new thread
     */
    void execute(ThreadRoutine lpStartAddress, void* args);

    /*
     * Wait until the thread will finish it's execution.
     */
    void wait();

    /*
     * Stop the execution of the thread in the middle
     */
    void kill();

    /*
     * Return the handler of the thread
     */
    cOSDef::threadHandle getHandle() const;

    /*
     * Returns the current thread handle
     */
    static cOSDef::threadHandle getCurrentThreadHandle();

    /*
     * Called in order to notify the operating system for thread termination.
     *
     * This function is implemented per operating system.
     */
    static void terminate();

private:
    // Deny copy-constructor and operator =
    cThread(const cThread& other);
    cThread & operator = (const cThread & other);

    // The handle for the thread
    cOSDef::threadHandle m_thread;
};

#endif // __TBA_STL_OS_THREAD_H

