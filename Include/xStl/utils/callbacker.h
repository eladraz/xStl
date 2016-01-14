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

#ifndef __TBA_STK_UTILS_CALLBACKER_H
#define __TBA_STK_UTILS_CALLBACKER_H

/*
 * callbacker.h
 *
 * A functor class. Provide callbacks to a class and procides an class context.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/utils/genericCallbackerFunctor.h"

/*
 * class cCallbacker
 *
 * Defines the callbackers interface. The usage of the callback functions is as
 * follows:
 *   There are serveral of classes: class A, class B, class C etc.
 *   There is an dispatcher class which wants to invoke messages to every
 *   register class (such as class A and class B).
 *   The dispatcher class should contain the following function:
 *
 *      void registerCallback(cCallbackPtr callbackClass);
 *
 *   The class A and B will contain the following code:
 *
 *   class A {
 *   public:
 *       // The following code will be called as a result from the dispatcher
 *       // event
 *       [virtual] void* onDispatcherEvent(void* argument);
 *
 *       DECLARE_CALLBACK(A, onDispatcherEvent);
 *   };
 *
 *   On the implementation of A the following code should be exist
 *
 *   // The constructor should register the callback
 *   A::A() {
 *      dispatcher.registerCallback(CALLBACKER(onDispatcherEvent));
 *   }
 */
class cCallback
{
public:
    /*
     * Default destructor. The
     */
    virtual ~cCallback() {};

    /*
     * Perform the callback. The function will be called from the dispatcher
     * class to notify that the event occuered.
     *
     * argument - Allow to transfer to the callback additional information.
     *
     * Return any information in any format
     */
    virtual void* call(void* argument) = 0;
};

/// The callbacker reference object
typedef cSmartPtr<cCallback> cCallbackPtr;

/*
 * This macro generate a unique functor that will invoke the 'function' when
 * it's called.
 */
#define CALLBACKER(function_name) \
    FUNC_CALLBACKER(function_name, cCallbackPtr)

/*
 * This macro generate fanctor thunk that will generate a call to a function.
 */
#define DECLARE_CALLBACK(class_name, function_name) \
    FUNC_DECLARE_CALLBACK(class_name, function_name, cCallback, call, void*, void*)

#endif // __TBA_STK_UTILS_CALLBACKER_H
