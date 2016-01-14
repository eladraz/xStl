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

#ifndef __TBA_STL_OS_LOCAKABLE_H
#define __TBA_STL_OS_LOCAKABLE_H

/*
 * lockable.h
 *
 * Defines the cLockableObject interface. This interface is used to declare all
 * lockable objects. Lockable object is an object which accuire a resource or
 * holds down the thread (or code execution) accuring the resource.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

/*
 * class cLockableObject
 *
 * The lock object has two function which used for locking: lock() and unlock()
 * These function throws exception in case of faliure, which terminate the code
 * execution.
 * The implementation of thoses function depends over the driven class
 * implementation code.
 */
class cLockableObject
{
public:
    /*
     * Default destructor. Allows dynamic memory allocaiton.
     */
    virtual ~cLockableObject();

    /*
     * Try to acquire the resource
     *
     * Throws exception incase of failure
     */
    virtual void lock() = 0;

    /*
     * Free the resource.
     *
     * Throws exception incase of failure
     */
    virtual void unlock() = 0;
};

#endif //__TBA_STL_OS_LOCAKABLE_H
