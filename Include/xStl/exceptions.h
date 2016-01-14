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

/*
 * exceptions.h
 *
 * Defines the TRY-CATCH basic blocks which supports exceptions handling.
 * The exceptions handling should be compiled for all platforms.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

/*
 * All platfrom will are using the compiler exception-mechanizm.
 * For kernel driver, see EHlib which operate the exception hadnling.
 */

#include "xStl/except/exception.h"

#define XSTL_TRY                try
#define XSTL_CATCH(n)           catch(n)
#define XSTL_RETHROW            throw

#ifdef XSTL_LINUX
    // GCC has issues with varisadic marcos and c++, plase see http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
    #define XSTL_THROW(klass, ...)  throw klass(__FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define XSTL_THROW(klass, ...)  throw klass(__FILE__, __LINE__, __VA_ARGS__)
#endif

#define XSTL_CATCH_ALL          catch(...)

#endif // __TBA_STL_EXCEPTIONS_H
