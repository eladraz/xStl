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

/**
 * @addtogroup xstl_utils
 * @{
 */

/**
 * @file genericCallbackerFunctor.h
 * @brief Template functor implementation for all callback types.
 *
 * @details See 'xStl/utils/callbacker.h' for examples.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"

/**
 * @brief This macro generate a unique functor that will invoke the 'function'
 *    when
 *   it's called.
 *
 * @details The functor is complicated since it supports many different run-time
 *    prototypes
 *
 * @param function_name The name of the member function to invoke.
 * @param functor_name The type of the functor reference object.
 */
#define FUNC_CALLBACKER(function_name, functor_name) \
    functor_name((new cDispatcherClass##function_name(this)))

/**
 * @brief This macro generate functor thunk that will generate a call to a
 *    function.
 *
 * @param class_name The class which declares the callback.
 * @param function_name The name of the member function to invoke.
 * @param functor_name The base class of the functor.
 */
#define FUNC_DECLARE_CALLBACK_BEGIN(class_name, function_name, functor_name)   \
    class cDispatcherClass##function_name : public functor_name {       \
    public:                                                             \
        /* Default constructor. Setup the instance for the class */     \
        cDispatcherClass##function_name(class_name* instance) :         \
            m_instance(instance)                                        \
        {                                                               \
        }

/**
 * @brief Generate 1 returned parameter block and 1 argument
 *
 * @param function_name The name of the member function to invoke.
 * @param functor_function_name The name of the functor function which is
 *    overridden.
 * @param returned_variable The returned type.
 * @param argument_type The type of the argument.
 */
#define FUNC_DECLARE_CALLBACK_PARAM1(function_name, functor_function_name, returned_variable, argument_type) \
        /* The dispatcher function */                                   \
        virtual returned_variable functor_function_name(argument_type argument) \
        {                                                               \
            return m_instance->function_name(argument);                 \
        }

/**
 * @brief Generate 1 returned parameter block and 2 argument
 *
 * @param function_name The name of the member function to invoke.
 * @param functor_function_name The name of the functor function which is
 *    overridden.
 * @param returned_variable The returned type.
 * @param argument_type1 The type of the first argument.
 * @param argument_type2 The type of the second argument.
 */
#define FUNC_DECLARE_CALLBACK_PARAM2(function_name, functor_function_name, returned_variable, argument_type1, argument_type2) \
    /* The dispatcher function */                                                               \
    virtual returned_variable functor_function_name(argument_type1 arg1, argument_type2 arg2)   \
    {                                                                                           \
        return m_instance->function_name(arg1, arg2);                                           \
    }


/**
 * @brief Generate 1 returned parameter block and 4 argument
 *
 * @param function_name The name of the member function to invoke.
 * @param functor_function_name The name of the functor function which is
 *    overridden.
 * @param returned_variable The returned type.
 * @param at1 The type of the first argument.
 * @param at2 The type of the second argument.
 * @param at3 The type of the third argument.
 * @param at4 The type of the fourth argument.
 */
#define FUNC_DECLARE_CALLBACK_PARAM4(function_name, functor_function_name, returned_variable, at1, at2, at3, at4) \
    /* The dispatcher function */                                                                                 \
    virtual returned_variable functor_function_name(at1 arg1, at2 arg2, at3 arg3, at4 arg4)                       \
    {                                                                                                             \
        return m_instance->function_name(arg1, arg2, arg3, arg4);                                                 \
    }


/**
 * @brief The prolog thunk
 *
 * @param class_name The class which declares the callback.
 * @param function_name The name of the member function to invoke.
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

/**
 * @brief This macro generate functor thunk that will generate a call to a
 *    function.
 *
 * @details See class cCallbacker for more information
 *
 * @param class_name The class which declares the callback.
 * @param function_name The name of the member function to invoke.
 * @param functor_name The base class of the functor.
 * @param functor_function_name The name of the functor function which is
 *    overridden.
 * @param returned_variable The returned type.
 * @param argument_type The type of the argument.
 */
#define FUNC_DECLARE_CALLBACK(class_name, function_name, functor_name, functor_function_name, returned_variable, argument_type)                \
        FUNC_DECLARE_CALLBACK_BEGIN(class_name, function_name, functor_name) \
        FUNC_DECLARE_CALLBACK_PARAM1(function_name, functor_function_name, returned_variable, argument_type)      \
        FUNC_DECLARE_CALLBACK_END(class_name, function_name)



/** @} */
#endif // __TBA_XSTL_UTILS_GENERICCALLBACKERFUNCTOR_H
