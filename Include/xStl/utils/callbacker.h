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

/**
 * @addtogroup xstl_utils
 * @{
 */

/**
 * @file callbacker.h
 * @brief A functor class. Provides callbacks to a class and provides a class
 *   context.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/utils/genericCallbackerFunctor.h"

/**
 * @brief Defines the callbackers interface.
 *
 * @details The usage of the callback functions is as follows: There are several
 *    classes: class A, class B, class C etc. There is a dispatcher class which
 *    wants to invoke messages to every registered classes (such as class A and
 *    class B). The dispatcher class should contain the following function:
 *
 * @code
 * void registerCallback(cCallbackPtr callbackClass);
 * @endcode
 *
 * The class A and B will contain the following code:
 *
 * class A {
 * public:
 *
 * @code
 * // The following code will be called as a result of the dispatcher
 * // event
 * [virtual] void* onDispatcherEvent(void* argument);
 * @endcode
 *
 * @code
 * DECLARE_CALLBACK(A, onDispatcherEvent);
 * @endcode
 *
 * };
 *
 * On the implementation of A the following code should exist
 *
 * // The constructor should register the callback
 * A::A() {
 *
 * @code
 * dispatcher.registerCallback(CALLBACKER(onDispatcherEvent));
 * @endcode
 *
 * }
 */
class cCallback
{
public:
    /**
     * @brief Default destructor. The
     */
    virtual ~cCallback() {};

    /**
     * @brief Perform the callback. The function will be called from the
     *    dispatcher
     *   class to notify that the event occurred.
     *
     * @param argument Allows transferring additional information to the
     *    callback.
     * @return Any information in any format.
     */
    virtual void* call(void* argument) = 0;
};

/// The callbacker reference object
typedef cSmartPtr<cCallback> cCallbackPtr;

/**
 * @brief This macro generates a unique functor that will invoke the 'function'
 *    when
 *   it is called.
 *
 * @param function_name The name of the member function to invoke.
 */
#define CALLBACKER(function_name) \
    FUNC_CALLBACKER(function_name, cCallbackPtr)

/**
 * @brief This macro generates a functor thunk that will generate a call to a
 *   function.
 *
 * @param class_name The class which declares the callback.
 * @param function_name The name of the member function to invoke.
 */
#define DECLARE_CALLBACK(class_name, function_name) \
    FUNC_DECLARE_CALLBACK(class_name, function_name, cCallback, call, void*, void*)


/** @} */
#endif // __TBA_STK_UTILS_CALLBACKER_H
