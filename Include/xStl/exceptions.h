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

#ifndef __TBA_STL_EXCEPTIONS_H
#define __TBA_STL_EXCEPTIONS_H

/**
 * @addtogroup xstl_core
 * @{
 */

/**
 * @file exceptions.h
 * @brief Defines the basic TRY-CATCH blocks which support exception handling.
 *
 * @details Exception handling should be compiled for all platforms.
 *
 * @author Elad Raz
 */

/*
 * All platforms use the compiler exception mechanism.
 * For kernel driver, see EHlib which operate the exception handling.
 */

#include "xStl/except/exception.h"

/**
 * @brief Begin a protected block (expands to `try`).
 */
#define XSTL_TRY                try
/**
 * @brief Catch an exception (expands to `catch(n)`).
 *
 * @param n The exception declaration to catch, for example `cException& e`.
 */
#define XSTL_CATCH(n)           catch(n)
/**
 * @brief Rethrow the exception that is currently being handled (expands to
 *    `throw`).
 */
#define XSTL_RETHROW            throw

#ifdef XSTL_LINUX
    /**
     * @brief Throw an exception of class @a klass (GCC/Linux variant).
     *
     * Same as the generic variant below, but uses the GCC `##__VA_ARGS__`
     * extension so that the macro can also be used without additional
     * arguments.
     *
     * @param klass The exception class to throw.
     * @param ... Additional constructor arguments of @a klass.
     */
    #define XSTL_THROW(klass, ...)  throw klass(__FILE__, __LINE__, ##__VA_ARGS__)
#else
    /**
     * @brief Throw an exception of class @a klass.
     *
     * The exception object is constructed with the current source file name
     * (`__FILE__`) and line number (`__LINE__`) followed by the extra
     * arguments. The exception classes of the library take these two values as
     * their first two constructor arguments.
     *
     * @param klass The exception class to throw, for example cException.
     * @param ... Additional constructor arguments of @a klass (message and/or
     *    error id).
     */
    #define XSTL_THROW(klass, ...)  throw klass(__FILE__, __LINE__, __VA_ARGS__)
#endif

/**
 * @brief Catch any exception (expands to `catch(...)`).
 */
#define XSTL_CATCH_ALL          catch(...)


/** @} */
#endif // __TBA_STL_EXCEPTIONS_H
