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

#ifndef __TBA_STL_OS_MUTEX_H
#define __TBA_STL_OS_MUTEX_H

/*
 * mutex.h
 *
 * Declare the cMutex class, which is a warrper to the mutex POSIX and windows
 * OS in one independence class.
 *
 * In the compliation for linux platform you must compile with the "-lpthread"
 * macro.
 *
 * This file supports the following platfroms:
 *  - Win32 API
 *  - POSIX library (Unix based OS)
 *  - XDK (Windows NT device-driver)
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/lockable.h"
#include "xStl/os/osdef.h"

/*
 * class cMutex
 *
 * An implementation to the lockable class with the 'FastMutex' interface. The
 * FastMutex is a mutex without any safety from deadlocks.
 *
 * The usage of the mutex class should be with the 'Lock' object.
 *
 * Usage:
 *     // The global sync object
 *     cMutex myThreadMutex;
 *
 *     uint32 threadFunction(void* context)
 *     {
 *          // Protect the code...
 *          cLock lock(myThreadMutex);
 *
 *          // The code is locked from exceptions and return codes.
 *     }
 */
class cMutex : public cLockableObject
{
public:
    /*
     * Constructor. Create the mutex object.
     */
    cMutex();

    /*
     * Destructor. Free the mutex lockable.
     */
    virtual ~cMutex();

    /*
     * Try to accuire the mutex
     *
     * Throws exception incase of faliure
     */
    virtual void lock();

    /*
     * Free the mutex.
     *
     * Throws exception incase of faliure
     */
    virtual void unlock();

private:
    /*
     * Deney copy-constructor and operator =
     */
    cMutex(const cMutex& other);
    cMutex& operator = (const cMutex& other);

    // The handle for the mutex
    cOSDef::mutexHandle m_handle;
};

#endif // __TBA_STL_OS_MUTEX_H
