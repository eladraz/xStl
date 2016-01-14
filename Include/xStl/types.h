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

#ifndef __TBA_STL_TYPES_H
#define __TBA_STL_TYPES_H

/*
 * types.h
 *
 * Contains basic blocks of the data-types.
 *
 * Supported by g++ (Red Hat Linux) and Visual C++ compiler (Studio 6.0)
 * Also supported Digital Mars compiler
 * The xStl code can be compiled to the following platforms:
 *   - Win32 Ring3 application
 *   - Unix's Posix base applications
 *   - NT-DDK driver (NT 4.0, 2000, XP)
 *
 * Author: Elad Raz <e@eladraz.com>
 */

/*
 * Fix the different compilers and enviroment _DEBUG preprocessor
 */
#ifdef DEBUG
    // The DEBUG preprocessor is not in used, instead the _DEBUG macro is
    // declared.
    #ifndef _DEBUG
        #define _DEBUG
    #endif
#endif


/*
 * Test for Windows version
 *
 * If the user using VC++ compiler then there is a
 * precompiler code: WIN32/WIN64/_WIN32_WCE/LINUX/TC16
 */
#ifdef _WIN32_WCE
    // Windows CE device-driver
    #define XSTL_32BIT
    #define XSTL_CE
    #ifdef CEDRIVER
        // Windows OS driver
        #define XSTL_CEDRIVER
    #else
        // Normal CE os application
        #define XSTL_CEOS
    #endif

    // TODO!
    #define XSTL_LITTLE_ENDIAN
    #include <windows.h>
#elif defined _M_X64
    // Windows 64 bit version
    #define XSTL_WINDOWS
    #define XSTL_64BIT

    #include <windows.h>

    #define XSTL_LITTLE_ENDIAN
#elif defined _WIN32
    // Windows 32 bit version
    #define XSTL_32BIT

    // Separate Ring0 application to Ring3 applications
    #ifndef _KERNEL
        #define XSTL_WINDOWS
        #include <windows.h>
    #else
        // Compile with the NTDDK
        #define XSTL_NTDDK
        #include "xdk/kernel.h"
    #endif

    // Windows is a little-endian platform
    // TODO!
    #define XSTL_LITTLE_ENDIAN
#elif defined LINUX
    // Linux 32 bit version
    #if defined(__x86_64)
        #define XSTL_64BIT
    #else
        #define XSTL_32BIT
    #endif
    #define XSTL_LINUX      // SHOULD BE XSTL_UNIX_BASED

    #ifdef __APPLE__
        // MACOSX
        #define XSTL_MACOSX
    #endif

    #include <memory.h>
    #include <string.h>

    // Linux is big-endian platform
    // TODO!
    #define XSTL_LITTLE_ENDIAN // XSTL_BIG_ENDIAN
#elif defined TC16
    // 16 bit application.
    // Turbo C++ compiler
    #define XSTL_16BIT
    // TODO!
    #define XSTL_LITTLE_ENDIAN
#else
    #error "Unknown platform. Contact Elad Raz for more information!\n"
#endif

/*
 * Usage of extern "C" in a C/C++ compilers. This macro is also defined
 * inside the XDK package.
 */
#ifndef EXTERNC
    #ifdef __cplusplus
        #define EXTERNC extern "C"
    #else
        #define EXTERNC
    #endif
#endif

#if defined(XSTL_LITTLE_ENDIAN)
    // Little endian
    #define defaultEndianImpl littleEndianImpl
#elif defined(XSTL_BIG_ENDIAN)
    // Big endian
    #define defaultEndianImpl bigEndianImpl
#else
    #error "Unknown endian. Contact Elad Raz for more information!\n"
#endif

#define UNUSED_PARAM(x)  ((void)(x))

/*
 * mechine types
 */
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef short int16;
typedef char  int8;

#ifndef XSTL_16BIT
    // 32bit compilers
    typedef unsigned int  uint32;
    typedef int           int32;
#else
    // 16bit compilers
    typedef unsigned long  uint32;
    typedef long           int32;
#endif

typedef uint8 byte;

#ifndef XSTL_16BIT
    // Add support to 64bit numbers.
    // The problem is with compiler which doesn't supports 64bit numbers
    // For instance Turbo C++ compiler
    #ifdef XSTL_LINUX
        // Linux
        typedef unsigned long long uint64;
        typedef long long int64;
    #else
        // Windows
        typedef unsigned __int64 uint64;
        typedef __int64 int64;
    #endif
#endif

// Make sure NULL is declared
#ifndef NULL
    #ifdef  __cplusplus
        #define NULL    0
    #else
        #define NULL    ((void *)0)
    #endif // __cplusplus
#endif // NULL

/*
 * The array size and the array access element. Used in all access to char*
 * buffers. DOS compilers uses 16bit access to array and 32bit application
 * uses 32bit pointer.
 * This parameter if define so the compiler can recognize the operators
 * parameters.
 *
 * In Windows x64 bit int is a 32bit value so uint is x64 bit value...
 */
#if defined(XSTL_32BIT) || defined(XSTL_16BIT) || defined(_M_X64)
    #define uint unsigned int
#elif defined(XSTL_64BIT)
    #define uint uint64
#endif

#define MAX_UINT ((uint)-1)

/*
 * The address numeric value is a typedef which handle pointers as numbers.
 * See getNumeric
 * See getPtr
 */
#ifdef XSTL_16BIT
    // 16 bit uses segment:offset address (24 bit according to the following
    // calculatation: addr = offset + segment * 0x10)
    typedef uint32 addressNumericValue;
#else
    // 32 bit and 64 bit compilers
    typedef uint addressNumericValue;
#endif

/*
 * The end of physical/virtual memory
 */
#define MAX_PHYSICAL_ADDRESS ((addressNumericValue)(-1))

/*
 * Address translation functions.
 */
addressNumericValue getNumeric(const void* ptr);
void* getPtr(addressNumericValue address);

// Define the remote-address
#include "xStl/remoteAddress.h"

/*
 * Basic data type
 *
 * Since Windows header file already include of this varibles
 * I will use thier declarations.
 */
#ifndef _WINDEF_        // <windef.h> - ring3 stdafx.h include file
    typedef int8    CHAR;
    typedef uint8   BYTE;
    typedef uint8*  PBYTE;
    typedef uint16  WCHAR;
    typedef uint16  WORD;
    typedef uint16* LPWORD;
    typedef int16   SHORT;
    typedef uint16  USHORT;
    typedef uint32  DWORD;
    typedef uint32* PDWORD;
    typedef int32   LONG;
    typedef uint32  ULONG;
    typedef void *  LPVOID;
    typedef void *  PVOID;
    typedef void    VOID;
    typedef uint64  ULONGLONG;
    typedef uint64  LONGLONG;
    typedef BYTE    BOOLEAN;

    #ifndef GUID_DEFINED    // <ntddk.h>  - ring0 include file
        #ifdef XSTL_LINUX
            #define FAR
        #else
            #define FAR far
        #endif
        typedef int        BOOL;
        #define TRUE    (0==0)
        #define FALSE   (1==0)
    #endif //GUID_DEFINED

    #define UNALIGNED
    #define NTAPI
#endif //_WINDEF_

#ifndef XSTL_16BIT
    #if defined(XSTL_64BIT)
        // 64 bit.
        #define MAX_INT (0x7FFFFFFFFFFFFFFFL)
    #elif defined(XSTL_32BIT)
        // 32 bit.
        #define MAX_INT (0x7FFFFFFF)
    #endif
#else
    // 16 bit
    #define MAX_INT (0x7FFF)
#endif

#define MAX_INT32  (0x7FFFFFFF)
#define MAX_UINT32 (0xFFFFFFFF)

/*
 * For getting the number of elements inside an array
 */
#define arraysize(n) (sizeof(n)/sizeof(n[0]))

/*
 * The normal counting size. Used in Counter.h
 */
#if (!(defined(XSTL_NTDDK) || defined(XSTL_WINDOWS)))
    // All platforms
    typedef int counter_t;
#else
    // Windows
    typedef LONG counter_t;
#endif

/*
 * Remark you can remark your code by writting:
 *    REMARK Author: Elad Raz
 * instead of:
 *        // Author: Elad Raz
 */
#define REMARK    /##/

/*
 * Include the exceptions handling
 */
#include "xStl/exceptions.h"

/*
 * Return codes
 *
 * Definitions of normal return code from function.
 * The return type is: int.
 */
#define RC_OK    (TRUE)
#define RC_ERROR (FALSE)


/*
 * CHECK, CHECK_MSG macro.
 *
 * Compile in both release and debug version. Make sure that the sentence is
 * true. Otherwise throws exception.
 */
#ifndef DOT_NET
// Normal C++ application
#ifndef _DEBUG
    // Release version
    #define CHECK_FAIL() XSTL_THROW(cException, NULL, EXCEPTION_ASSERTION);
    #define CHECK_MSG(n,msg) if (!(n)) { XSTL_THROW(cException, NULL, EXCEPTION_ASSERTION); }
    #define CHECK(n) if (!(n)) { CHECK_FAIL(); }
#else
    // Debug version
    #define CHECK_FAIL() XSTL_THROW(cException, NULL, EXCEPTION_ASSERTION);
    #define CHECK_MSG(n,msg) if (!(n)) { TRACE(TRACE_MEDIUM, msg); XSTL_THROW(cException, EXCEPTION_ASSERTION); }
    #define CHECK(n)     if (!(n)) { TRACE(TRACE_VERY_HIGH, XSTL_STRING("CHECK FAILED at ") XSTL_FILE XSTL_STRING("(") XSTL_LINE XSTL_STRING(") : ") XSTL_STRING(#n) XSTL_STRING("\n")); XSTL_THROW(cException, XSTL_STRING(#n), EXCEPTION_ASSERTION); }
#endif
#else
    // Managed code for both debug and release TODO!
    #error TODO! I don't think it's possible
    #define CHECK(n) if (!(n)) { CHECK_FAIL(); }
    #define CHECK_FAIL() { throw 0;}
#endif // DOT_NET
/*
 * A propere use of extern "C" in a C/C++ compilers
 */
#ifdef __cplusplus
    #define EXTERNC extern "C"
#else
    #define EXTERNC
#endif

#ifdef XSTL_LINUX
    #define XSTL_PACKED_HEADER
    #define XSTL_PACKED_FOOTER
    #define XSTL_PACKED __attribute__((__packed__))
#else
    #define XSTL_PACKED
    #define XSTL_PACKED_HEADER
    #define XSTL_PACKED_FOOTER
#endif


#endif //__TBA_STL_TYPES_H
