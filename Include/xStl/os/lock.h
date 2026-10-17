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

#ifndef __TBA_STL_OS_LOCK_H
#define __TBA_STL_OS_LOCK_H

/*
 * lock.h
 *
 * Implement the lockable-objects locking class. The locking class provides easy
 * access to the resource and some safety from exceptions during the locking
 * code.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/lockable.h"

/*
 * class cLock
 *
 * This class allows safety during locking of execution code. Instead of using
 * the lockable-object in the following way:
 *
 *    // (Global) variable
 *    cLockableObject& mutex;
 *
 *    // Execution code
 *    {
 *      mutex.lock()
 *      // Critical section
 *
 *      // Error, exception throws here and the mutex remains acquired.
 *
 *      mutex.unlock()
 *    }
 *
 * Use the cLock object, so even if an exception is thrown the exception handler
 * will call the destructor of the class and the lock will be released:
 *
 *    // (Global) variable
 *    cLockableObject& mutex;
 *
 *    // Execution code
 *    {
 *      cLock lock(mutex)
 *      // Critical section
 *    }
 */
class cLock
{
public:
    /*
     * Constructor. Acquire the resource.
     *
     * An exception can be thrown during the lockable-object resource lock()
     * function.
     */
    cLock(cLockableObject& lockable);

    /*
     * Destructor. Release the lock over the resource.
     *
     * An exception can be thrown during the lockable-object resource unlock()
     * function.
     */
    ~cLock();

    /*
     * This function releases the lock over the resource. Many times the
     * application wants to lock the object for a part of the code, this
     * function allows improving performance by releasing the lock.
     * Usage:
     *
     *  cLockableObject& lockable;
     *
     *  void func()
     *  {
     *      cLock lock(lockable);
     *      .
     *      .  // Do some stuff with the resource
     *      .
     *
     *      lock.unlock();
     *      // Some non-protected operations.
     *  }
     */
    void unlock();

private:
    // The lockable object which is acquired
    cLockableObject& m_lockable;

    // Deny operator = and copy-constructor
    cLock(const cLock& other);
    cLock& operator=(const cLock& other);

    // Whether the lockable is locked and should be released at dtor
    bool m_isLock;
};

#endif // __TBA_STL_OS_LOCK_H
