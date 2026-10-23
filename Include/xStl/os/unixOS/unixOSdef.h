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

/**
 * @addtogroup xstl_os_unix
 * @{
 */

/**
 * @file unixOSdef.h
 * @brief Types, struct definitions and constants needed for the UNIX type based
 *   operating systems
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "pthread.h"

/**
 * @brief The state of an event object on UNIX platforms.
 */
typedef struct {
    pthread_mutex_t mutex;  ///< The mutex which protects the event state.
    /**
     * @brief The condition variable which is signaled when the event is
     *   triggered.
     */
    pthread_cond_t cond;
    bool triggered;         ///< True if the event was triggered.
} pthread_unix_event_t;

/**
 * @brief A pthread mutex with its attributes.
 */
typedef struct {
    pthread_mutex_t m_mutex;            ///< The mutex.
    /**
     * @brief The attributes the mutex was created with.
     */
    pthread_mutexattr_t m_mutexAttr;
} pthread_unix_mutex_wrap_t;

/*
 * UNIX implementation. Tested on the following UNIX based OS:
 *    - Ubuntu
 *    - Fedora 6
 */
namespace cOSDef {
    /**
     * @brief The threadHandle
     */
    typedef pthread_t threadHandle;
    /**
     * @brief The mutexHandle
     */
    typedef pthread_unix_mutex_wrap_t mutexHandle;
    /**
     * @brief The eventHandle
     */
    typedef pthread_unix_event_t eventHandle;
    /**
     * @brief The systemTime.
     */
    typedef uint32 systemTime;
    /**
     * @brief The fileHandle
     */
    typedef int fileHandle;
    // Consts
}; // namespace cOSDef

/**
 * @brief Invalid file handle
 */
#define INVALID_FILE_HANDLE   (-1)
/**
 * @brief Invalid thread handle. `(pthread_t)(-1)`, or NULL on Mac OS X.
 */
#ifndef XSTL_MACOSX
    #define INVALID_THREAD_HANDLE ((pthread_t)(-1))
#else
    #define INVALID_THREAD_HANDLE (NULL)
#endif // XSTL_MACOSX


/** @} */
#endif // __TBA_STL_OS_UNIXOS_H
