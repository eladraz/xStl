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

#ifndef __TBA_STL_OS_UNIXOS_H
#define __TBA_STL_OS_UNIXOS_H

/*
 * unixOSdef.h
 *
 * Types, structs definitions and consts needed for the UNIXs type based
 * operating system
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "pthread.h"

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool triggered;
} pthread_unix_event_t;

typedef struct {
    pthread_mutex_t m_mutex;
    pthread_mutexattr_t m_mutexAttr;
} pthread_unix_mutex_wrap_t;

/*
 * UNIX implementation. Tests on the following UNIX based OS:
 *    - Ubuntu
 *    - Fedora 6
 */
namespace cOSDef {
    // The threadHandle
    typedef pthread_t threadHandle;
    // The mutexHandle
    typedef pthread_unix_mutex_wrap_t mutexHandle;
    // The eventHandle
    typedef pthread_unix_event_t eventHandle;
    // The systemTime.
    typedef uint32 systemTime;
    // The fileHandle
    typedef int fileHandle;
    // Consts
}; // namespace cOSDef

// Invalid file handle
#define INVALID_FILE_HANDLE   (-1)
#ifndef XSTL_MACOSX
    #define INVALID_THREAD_HANDLE ((pthread_t)(-1))
#else
    #define INVALID_THREAD_HANDLE (NULL)
#endif // XSTL_MACOSX

#endif // __TBA_STL_OS_UNIXOS_H
