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

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file thread.h
 * @brief Declare a virtual class for thread creation.
 *
 * @details Implement the POSIX interface for UNIX platforms and the Windows
 *    Threads command for Windows threads.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/os/osdef.h"

/**
 * @brief The thread function receives a single pointer as an argument and
 *    returns a
 *   pointer.
 *
 * @note For some operating systems, this function shouldn't return. It must be
 *    terminated using operating system calls. For example the XDK (Windows
 *    driver kit) thread must be terminated using a call to
 *    'PsTerminateSystemThread' function.
 * @note This interface doesn't contain any interface for getting the return
 *    code.
 * @note The routine must invoke a call to cThread::terminate in order to
 *    destroy all threads resources. See cThreadedClass implementation
 */
typedef void*(*ThreadRoutine)(void*);

/**
 * @brief Wrapper class for the thread API.
 *
 * @details Allows creating, waiting for and killing a thread.
 *
 * Using this class:
 *
 * @code
 * cThread newThread;
 * newThread.execute(func, argc);
 * // Do some things...
 * newThread.wait();
 * @endcode
 *
 * Implemented over:
 *
 * @code
 * Win32 API, Windows NT DDK, POSIX (Unix based applications).
 * @endcode
 *
 * @note Don't use this class for threads in your applications. Use the
 *    cThreadedClass instead.
 * @see cThreadedClass
 * @note The newly constructed thread doesn't close at the destruction of this
 *    class.
 */
class cThread
{
public:
    /**
     * @brief Default constructor. Create empty thread object.
     */
    cThread();

    /**
     * @brief Destructor. The destructor doesn't free any resources.
     *
     * @details Notice, that the thread can be running without any wrapper
     *    class.
     */
    ~cThread();


    // Main function

    /**
     * @brief Create the new thread
     *
     * @param lpStartAddress The routine which the new thread executes.
     * @param args The argument which is passed to the routine.
     */
    void execute(ThreadRoutine lpStartAddress, void* args);

    /**
     * @brief Wait until the thread finishes its execution.
     */
    void wait();

    /**
     * @brief Stop the execution of the thread in the middle
     */
    void kill();

    /**
     * @brief Return the handler of the thread
     *
     * @return The handler of the thread.
     */
    cOSDef::threadHandle getHandle() const;

    /**
     * @brief Returns the current thread handle
     *
     * @return The current thread handle.
     */
    static cOSDef::threadHandle getCurrentThreadHandle();

    /**
     * @brief Called in order to notify the operating system of thread
     *    termination.
     *
     * @details This function is implemented per operating system.
     */
    static void terminate();

private:
    /**
     * @brief Deny copy-constructor and operator =
     */
    cThread(const cThread& other);
    cThread & operator = (const cThread & other);

    /**
     * @brief The handle for the thread
     */
    cOSDef::threadHandle m_thread;
};


/** @} */
#endif // __TBA_STL_OS_THREAD_H

