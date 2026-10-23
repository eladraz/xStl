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

/**
 * @addtogroup xstl_core
 * @{
 */

/**
 * @file types.h
 * @brief Contains basic blocks of the data-types.
 *
 * @details Supported by g++ (Red Hat Linux) and Visual C++ compiler (Studio
 *    6.0)
 *   Also supported by the Digital Mars compiler
 *   The xStl code can be compiled to the following platforms:
 *   - Win32 Ring3 application
 *   - UNIX POSIX-based applications
 *   - NT-DDK driver (NT 4.0, 2000, XP)
 *
 * @author Elad Raz
 */

/*
 * Fix the different compilers and environment _DEBUG preprocessor
 */
#ifdef DEBUG
    // The DEBUG preprocessor macro is not used; instead the _DEBUG macro is
    // declared.
    #ifndef _DEBUG
        #define _DEBUG
    #endif
#endif


/*
 * Test for Windows version
 *
 * If the user is using the VC++ compiler then there is a
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

    // Separate Ring0 applications from Ring3 applications
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
    #if defined(__x86_64) || defined(__aarch64__) || defined(__LP64__)
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
 * Usage of extern "C" in C/C++ compilers. This macro is also defined
 * inside the XDK package.
 */
#ifndef EXTERNC
    #ifdef __cplusplus
        /**
         * @brief @brief Declares C linkage in C++ (`extern "C"`); empty when
         *   compiled as C.
         */
        #define EXTERNC extern "C"
    #else
        /**
         * @brief @brief Declares C linkage in C++ (`extern "C"`); empty when
         *   compiled as C.
         */
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

/**
 * @brief Marks a function parameter as intentionally unused.
 *
 * Silences the "unreferenced parameter" compiler warning.
 *
 * @param x The unused parameter.
 */
#define UNUSED_PARAM(x)  ((void)(x))

/** @brief Unsigned 8 bit integer. */
typedef unsigned char   uint8;
/** @brief Unsigned 16 bit integer. */
typedef unsigned short  uint16;
/** @brief Signed 16 bit integer. */
typedef short int16;
/** @brief Signed 8 bit integer. */
typedef char  int8;

#ifndef XSTL_16BIT
    /** @brief Unsigned 32 bit integer. */
    typedef unsigned int  uint32;
    /** @brief Signed 32 bit integer. */
    typedef int           int32;
#else
    /**
     * @brief @brief Unsigned 32 bit integer (`unsigned long` for 16 bit
     *   compilers).
     */
    typedef unsigned long  uint32;
    /** @brief Signed 32 bit integer (`long` for 16 bit compilers). */
    typedef long           int32;
#endif

/** @brief A single byte (same as uint8). */
typedef uint8 byte;

#ifndef XSTL_16BIT
    // Add support to 64bit numbers.
    // The problem is with compilers which don't support 64bit numbers
    // For instance Turbo C++ compiler
    #ifdef XSTL_LINUX
        /** @brief Unsigned 64 bit integer. */
        typedef unsigned long long uint64;
        /** @brief Signed 64 bit integer. */
        typedef long long int64;
    #else
        /** @brief Unsigned 64 bit integer. */
        typedef unsigned __int64 uint64;
        /** @brief Signed 64 bit integer. */
        typedef __int64 int64;
    #endif
#endif

// Make sure NULL is declared
#ifndef NULL
    #ifdef  __cplusplus
        /**
         * @brief @brief The null pointer constant (defined only if the platform
         *   headers did not define it).
         */
        #define NULL    0
    #else
        /**
         * @brief @brief The null pointer constant (defined only if the platform
         *   headers did not define it).
         */
        #define NULL    ((void *)0)
    #endif // __cplusplus
#endif // NULL

/*
 * The array size and the array access element. Used in all access to char*
 * buffers. DOS compilers use 16bit access to arrays and 32bit applications
 * use 32bit pointers.
 * This parameter is defined so the compiler can recognize the operator
 * parameters.
 *
 * In Windows x64 int is a 32bit value, so uint stays 32bit (pointers use
 * addressNumericValue).
 */
#if defined(XSTL_32BIT) || defined(XSTL_16BIT) || defined(_M_X64)
    /**
     * @brief The unsigned integer type used for sizes, counts and array
     *    indexes.
     *
     * A 32 bit `unsigned int` on 32 bit, 16 bit and Windows x64 targets and a
     * 64 bit integer (`uint64`) on other 64 bit targets.
     */
    #define uint unsigned int
#elif defined(XSTL_64BIT)
    /**
     * @brief The unsigned integer type used for sizes, counts and array
     *    indexes.
     *
     * A 32 bit `unsigned int` on 32 bit, 16 bit and Windows x64 targets and a
     * 64 bit integer (`uint64`) on other 64 bit targets.
     */
    #define uint uint64
#endif

/** @brief The largest value of #uint. */
#define MAX_UINT ((uint)-1)

/*
 * The address numeric value is a typedef which handles pointers as numbers.
 * See getNumeric
 * See getPtr
 */
#ifdef XSTL_16BIT
    /**
     * @brief 16 bit uses segment:offset address (24 bit according to the
     *    following
     *   calculation: addr = offset + segment * 0x10)
     */
    typedef uint32 addressNumericValue;
#elif defined(_M_X64)
    /**
     * @brief Windows x64 keeps uint as 32 bit, but pointers are 64 bit
     */
    typedef uint64 addressNumericValue;
#else
    /**
     * @brief An unsigned integer wide enough to hold a pointer (a pointer as a
     *    number).
     *
     * Use getNumeric() and getPtr() to convert between pointers and this type.
     */
    typedef uint addressNumericValue;
#endif

/**
 * @brief The end of physical/virtual memory
 */
#define MAX_PHYSICAL_ADDRESS ((addressNumericValue)(-1))

/**
 * @brief Convert a pointer into its numeric address.
 *
 * @param ptr The pointer to convert.
 * @return The address held by @a ptr, as an addressNumericValue.
 * @see getPtr
 */
addressNumericValue getNumeric(const void* ptr);

/**
 * @brief Convert a numeric address into a pointer.
 *
 * @param address The numeric address.
 * @return The pointer that corresponds to @a address.
 * @see getNumeric
 */
void* getPtr(addressNumericValue address);

// Define the remote-address
#include "xStl/remoteAddress.h"

/*
 * Basic data type
 *
 * Since the Windows header files already include these variables,
 * I will use their declarations.
 */
#ifndef _WINDEF_        // <windef.h> - ring3 stdafx.h include file
    typedef int8    CHAR;       ///< Windows-compatible signed 8 bit character.
    typedef uint8   BYTE;       ///< Windows-compatible unsigned 8 bit integer.
    typedef uint8*  PBYTE;      ///< Pointer to a BYTE.
    typedef uint16  WCHAR;      ///< Windows-compatible 16 bit wide character.
    typedef uint16  WORD;       ///< Windows-compatible unsigned 16 bit integer.
    typedef uint16* LPWORD;     ///< Pointer to a WORD.
    typedef int16   SHORT;      ///< Windows-compatible signed 16 bit integer.
    typedef uint16  USHORT;     ///< Windows-compatible unsigned 16 bit integer.
    typedef uint32  DWORD;      ///< Windows-compatible unsigned 32 bit integer.
    typedef uint32* PDWORD;     ///< Pointer to a DWORD.
    typedef int32   LONG;       ///< Windows-compatible signed 32 bit integer.
    typedef uint32  ULONG;      ///< Windows-compatible unsigned 32 bit integer.
    typedef void *  LPVOID;     ///< Generic pointer.
    typedef void *  PVOID;      ///< Generic pointer.
    typedef void    VOID;       ///< Windows-compatible void.
    typedef uint64  ULONGLONG;  ///< Windows-compatible unsigned 64 bit integer.
    /**
     * @brief Windows-compatible 64 bit integer (declared unsigned here).
     */
    typedef uint64  LONGLONG;
    /**
     * @brief Windows-compatible boolean stored in a BYTE.
     */
    typedef BYTE    BOOLEAN;

    #ifndef GUID_DEFINED    // <ntddk.h>  - ring0 include file
        #ifdef XSTL_LINUX
            /**
             * @brief Windows far-pointer qualifier; empty on this platform.
             */
            #define FAR
        #else
            #define FAR far ///< Windows far-pointer qualifier.
        #endif
        typedef int        BOOL;   ///< Windows-compatible boolean type.
        #define TRUE    (0==0)     ///< Boolean true.
        #define FALSE   (1==0)     ///< Boolean false.
    #endif //GUID_DEFINED

    #define UNALIGNED  ///< Windows unaligned-pointer qualifier; empty here.
    #define NTAPI      ///< Windows NT calling-convention qualifier; empty here.
#endif //_WINDEF_

#ifndef XSTL_16BIT
    #if defined(XSTL_64BIT)
        /**
         * @brief @brief The largest value of a signed `int` (64 bit targets:
         *   2^63 - 1).
         */
        #define MAX_INT (0x7FFFFFFFFFFFFFFFL)
    #elif defined(XSTL_32BIT)
        /**
         * @brief @brief The largest value of a signed `int` (32 bit targets:
         *   2^31 - 1).
         */
        #define MAX_INT (0x7FFFFFFF)
    #endif
#else
    /**
     * @brief @brief The largest value of a signed `int` (16 bit targets: 2^15 -
     *   1).
     */
    #define MAX_INT (0x7FFF)
#endif

/** @brief The largest value of a signed 32 bit integer. */
#define MAX_INT32  (0x7FFFFFFF)
/** @brief The largest value of an unsigned 32 bit integer. */
#define MAX_UINT32 (0xFFFFFFFF)

/**
 * @brief Get the number of elements inside a (statically sized) array.
 *
 * @param n The array.
 */
#define arraysize(n) (sizeof(n)/sizeof(n[0]))

/*
 * The normal counting size. Used in Counter.h
 */
#if (!(defined(XSTL_NTDDK) || defined(XSTL_WINDOWS)))
    /**
     * @brief @brief The counter type used by cCounter (a plain `int` on this
     *   platform).
     */
    typedef int counter_t;
#else
    /**
     * @brief @brief The counter type used by cCounter (a LONG, for the Windows
     *   interlocked functions).
     */
    typedef LONG counter_t;
#endif

/**
 * @brief Remark: you can comment your code by writing: REMARK Author: Elad Raz
 *   instead of:
 *
 * @code
 * // Author: Elad Raz
 * @endcode
 */
#define REMARK    /##/

/*
 * Include the exception handling
 */
#include "xStl/exceptions.h"

/**
 * @brief Return code of a successful operation.
 *
 * Return codes are of type `int`.
 */
#define RC_OK    (0)
/** @brief Return code of a failed operation. */
#define RC_ERROR (1)

/*
 * CHECK, CHECK_MSG macro.
 *
 * Compile in both release and debug version. Make sure that the sentence is
 * true. Otherwise throws an exception.
 */
#ifndef DOT_NET
// Normal C++ application
#ifndef _DEBUG
    /**
     * @brief Unconditionally throw an assertion exception (release build).
     *
     * @throws cException (EXCEPTION_ASSERTION) always.
     */
    #define CHECK_FAIL() XSTL_THROW(cException, NULL, EXCEPTION_ASSERTION);
    /**
     * @brief Throw an assertion exception if the condition is false (release
     *    build).
     *
     * Active in both release and debug builds. In a release build the message
     * is not used.
     *
     * @param n   The condition that must be true.
     * @param msg A message describing the failure (reported through TRACE in
     *    debug builds).
     * @throws cException (EXCEPTION_ASSERTION) if @a n is false.
     */
    #define CHECK_MSG(n,msg) if (!(n)) { XSTL_THROW(cException, NULL, EXCEPTION_ASSERTION); }
    /**
     * @brief Throw an assertion exception if the condition is false.
     *
     * Active in both release and debug builds. In a debug build the failed
     * expression, file and line are traced before the exception is thrown.
     *
     * @param n The condition that must be true.
     * @throws cException (EXCEPTION_ASSERTION) if @a n is false.
     */
    #define CHECK(n) if (!(n)) { CHECK_FAIL(); }
#else
    /**
     * @brief Unconditionally throw an assertion exception (debug build).
     *
     * @throws cException (EXCEPTION_ASSERTION) always.
     */
    #define CHECK_FAIL() XSTL_THROW(cException, NULL, EXCEPTION_ASSERTION);
    /**
     * @brief @brief Throw an assertion exception if the condition is false; the
     *   message is traced in debug builds.
     */
    #define CHECK_MSG(n,msg) if (!(n)) { TRACE(TRACE_MEDIUM, msg); XSTL_THROW(cException, EXCEPTION_ASSERTION); }
    /**
     * @brief @brief Throw an assertion exception if the condition is false; the
     *   failed expression, file and line are traced.
     */
    #define CHECK(n)     if (!(n)) { TRACE(TRACE_VERY_HIGH, XSTL_STRING("CHECK FAILED at ") XSTL_FILE XSTL_STRING("(") XSTL_LINE XSTL_STRING(") : ") XSTL_STRING(#n) XSTL_STRING("\n")); XSTL_THROW(cException, XSTL_STRING(#n), EXCEPTION_ASSERTION); }
#endif
#else
    // Managed code for both debug and release TODO!
    #error TODO! I don't think it's possible
    #define CHECK(n) if (!(n)) { CHECK_FAIL(); }
    #define CHECK_FAIL() { throw 0;}
#endif // DOT_NET
/*
 * A proper use of extern "C" in C/C++ compilers
 */
#ifdef __cplusplus
    #define EXTERNC extern "C"
#else
    #define EXTERNC
#endif

#ifdef XSTL_LINUX
    /**
     * @brief @brief Placed before a packed struct (empty on this platform).
     *   @see XSTL_PACKED
     */
    #define XSTL_PACKED_HEADER
    /**
     * @brief @brief Placed after a packed struct (empty on this platform). @see
     *   XSTL_PACKED
     */
    #define XSTL_PACKED_FOOTER
    /**
     * @brief @brief Attribute that removes the padding between struct members
     *   (GCC `packed`).
     */
    #define XSTL_PACKED __attribute__((__packed__))
#else
    /**
     * @brief @brief Attribute that removes the padding between struct members
     *   (empty here; use XSTL_PACKED_HEADER/FOOTER).
     */
    #define XSTL_PACKED
    /** @brief Placed before a packed struct (empty here). @see XSTL_PACKED */
    #define XSTL_PACKED_HEADER
    /** @brief Placed after a packed struct (empty here). @see XSTL_PACKED */
    #define XSTL_PACKED_FOOTER
#endif



/** @} */
#endif //__TBA_STL_TYPES_H
