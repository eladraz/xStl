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

#ifndef __TBA_XSTL_UTILS_GENERICCALLBACKERFUNCTOR_H
#define __TBA_XSTL_UTILS_GENERICCALLBACKERFUNCTOR_H

/*
 * genericCallbackerFunctor.h
 *
 * Template functor implementation for all callbacing types.
 * See 'xStl/utils/callbacker.h' for examples.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

/*
 * This macro generate a unique functor that will invoke the 'function' when
 * it's called.
 * The functor is complicated since it's supports many differents run-time
 * poroto-types
 */
#define FUNC_CALLBACKER(function_name, functor_name) \
    functor_name((new cDispatcherClass##function_name(this)))

/*
 * This macro generate functor thunk that will generate a call to a function.
 */
#define FUNC_DECLARE_CALLBACK_BEGIN(class_name, function_name, functor_name)   \
    class cDispatcherClass##function_name : public functor_name {       \
    public:                                                             \
        /* Default constructor. Setup the instance for the class */     \
        cDispatcherClass##function_name(class_name* instance) :         \
            m_instance(instance)                                        \
        {                                                               \
        }

/*
 * Generate 1 returned parameter block and 1 argument
 */
#define FUNC_DECLARE_CALLBACK_PARAM1(function_name, functor_function_name, returned_variable, argument_type) \
        /* The dispatcher function */                                   \
        virtual returned_variable functor_function_name(argument_type argument) \
        {                                                               \
            return m_instance->function_name(argument);                 \
        }

/*
 * Generate 1 returned parameter block and 2 argument
 */
#define FUNC_DECLARE_CALLBACK_PARAM2(function_name, functor_function_name, returned_variable, argument_type1, argument_type2) \
    /* The dispatcher function */                                                               \
    virtual returned_variable functor_function_name(argument_type1 arg1, argument_type2 arg2)   \
    {                                                                                           \
        return m_instance->function_name(arg1, arg2);                                           \
    }


/*
 * Generate 1 returned parameter block and 4 argument
 */
#define FUNC_DECLARE_CALLBACK_PARAM4(function_name, functor_function_name, returned_variable, at1, at2, at3, at4) \
    /* The dispatcher function */                                                                                 \
    virtual returned_variable functor_function_name(at1 arg1, at2 arg2, at3 arg3, at4 arg4)                       \
    {                                                                                                             \
        return m_instance->function_name(arg1, arg2, arg3, arg4);                                                 \
    }


/*
 * The prolog thunk
 */
#define FUNC_DECLARE_CALLBACK_END(class_name, function_name)            \
    private:                                                            \
        /* Deny copy-constructor and operator = */                      \
        cDispatcherClass##function_name                                 \
            (const cDispatcherClass##function_name& other);             \
        cDispatcherClass##function_name& operator=                      \
            (const cDispatcherClass##function_name& other);             \
        /* The instance to invoke the callback to */                    \
        class_name* m_instance;                                         \
};

/*
 * This macro generate functor thunk that will generate a call to a function.
 * See class cCallbacker for more information
 */
#define FUNC_DECLARE_CALLBACK(class_name, function_name, functor_name, functor_function_name, returned_variable, argument_type)                \
        FUNC_DECLARE_CALLBACK_BEGIN(class_name, function_name, functor_name) \
        FUNC_DECLARE_CALLBACK_PARAM1(function_name, functor_function_name, returned_variable, argument_type)      \
        FUNC_DECLARE_CALLBACK_END(class_name, function_name)


#endif // __TBA_XSTL_UTILS_GENERICCALLBACKERFUNCTOR_H
