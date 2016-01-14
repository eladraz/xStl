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

#ifndef __TBA_STL_ASSERT_H
#define __TBA_STL_ASSERT_H

/*
 * assert.h
 *
 * Define default ASSERT function for assertion. The ASSERT function decaler
 * as be platform indepndece, which means it can work on Windows version,
 * Linux version and other future OS.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/char.h"


/*
 * assertFunction(BOOL condition, cString msg)
 *
 * Private inline functoin of the STL, This function shows the assertions and
 * throw exception.
 */
#ifdef _DEBUG
    #ifdef DOT_NET
        // The .NOT assert macro
        void assertFunction(bool condition, unsigned int line,
                            System::String^ file,
                            System::String^ msg);
    #else
        // Normal C++ application
        void assertFunction(bool condition, unsigned int line, const char* file, const character* msg);
    #endif
#endif


// Remove old definitions
#ifdef ASSERT
#undef ASSERT
#endif
#ifdef ASSERT_MSG
#undef ASSERT_MSG
#endif

/*
 * ASSERT(condition)
 * ASSERT_MSG(condition, msg)
 *
 * Define the assert macro.
 */
#ifdef XSTL_WINDOWS
    #pragma warning(disable: 4005) // Redefined macro
#endif

#ifdef _DEBUG
    #ifdef DOT_NET
        // The .NOT assert macro
        #define ASSERT(condition)          assertFunction(condition, __LINE__, __FILE__, XSTL_STRING(#condition) );
        #define ASSERT_MSG(condition, msg) assertFunction(condition, __LINE__, __FILE__, (msg));
        #define ASSERT_FAIL(msg)           assertFunction(false, __LINE__, __FILE__, (msg));
    #else
        // Normal C++ application
        #define ASSERT(condition)          assertFunction(condition, __LINE__, __FILE__, XSTL_STRING(#condition) );
        #define ASSERT_MSG(condition, msg) assertFunction(condition, __LINE__, __FILE__, (msg));
        #define ASSERT_FAIL(msg)           assertFunction(false, __LINE__, __FILE__, (msg));
    #endif // DOT_NET
#else
    #define ASSERT(condition)
    #define ASSERT_MSG(condition, msg)
    #define ASSERT_FAIL(msg)
#endif

#ifdef XSTL_WINDOWS
    #pragma warning(default: 4005)
#endif

#endif // __TBA_STL_ASSERT_H

