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

#ifndef __TBA_STL_OS_OS_H
#define __TBA_STL_OS_OS_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file os.h
 * @brief Declare operating-system-independent functions.
 *
 * @details The operating system should provide support for the following
 * functions:
 *
 * - Thread functions.<br>
 *   The sleep function. Puts the thread into sleep mode. Only interruptible
 *   function can be called from the thread context.
 *
 * - Global random for the system.<br>
 *   Allow changing the system global random, or acquire random from the system.
 *   The random for the system can be a hardware random device and doesn't
 *   guarantee the same result in any way (like a pseudo-random-function).
 *   When the system is loaded, the global random object is initialized into a
 *   strong
 *   random and can be left untouched (Using the srand() function).
 *
 * - Time functions.<br>
 *   Getting the right time and getting the date of the year.
 *
 * - Memory operation.<br>
 *   The 'memcpy()' function. The implementation of the operating system
 *   can use a hardware accelerator and avoid overloading the CPU. The OS can
 *   notify the application when the copying has completed or block the thread.
 *   Also define the memset() function which init memory.
 *
 * - Memory allocation and deallocation for a small amount of memory (heap).<br>
 *   The global operators new and delete should use these functions. The
 *   memory is very fast, usually it is cached in the hardware. Also OS
 *   can provide some kind of security by allocating zero-page memory.
 *   NOTE: this kind of memory is NOT sharable and usually it's not paged.
 *
 * The following functions are marked as TODO in the original source:
 *
 * - Memory allocation and deallocation of a large amount of memory (pages).<br>
 *   Programs which want to allocate big memory should use these functions
 *   instead of using operator new/delete.
 *   NOTE: this kind of memory is NOT sharable.
 *
 * - Far memory allocation.<br>
 *   Try to allocate memory which can be shared with a number of processors or
 *   computers. Usually this memory can be found in the memory of a number of
 *   processors or even on different computers, so this memory is very slow but
 *   can contain a large amount of memory.
 *   NOTE: this kind of memory is sharable and slow.
 *
 * - Files access.<br>
 *   Simple interface for opening files, reading/writing, etc...
 *
 * - Files enumeration.
 *
 * - Network access.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/os/osdef.h"

// Forward declaration of cString
class cString;

/**
 * @brief The operating-system-dependent code. The class allows using the
 *   operating-system implementation for the previous functions.
 *
 * @details These are very powerful functions.
 *
 * All of these functions are thread-safe and can be invoked from several
 * threads
 * without worrying about data loss.
 */
class cOS
{
public:
    //
    // Memory operations
    //

    /**
     * @brief Copy a chunk of memory 'src' with size of 'size' bytes into
     *    'dest'.
     *
     * @details The copy can be synchronous or asynchronous. See the cWaitable
     *    for more information about asynchronous calls.
     *
     * @param dest The buffer to copy the memory to.
     * @param src The source buffer where the data will be copied from.
     * @param size The number of bytes of the buffers.
     * @note The function doesn't have any protection regarding the buffer
     *    sizes. The operating system or the compiler may pad the buffers with
     *    magic and throw an exception in case of overflows...
     */
    static void memcpy(void* dest,
                       const void* src,
                       uint size
                       /*cWaitable wait*/);

    //
    // Memory allocation
    //

    /**
     * @brief Try to allocate a small memory block of 'size' bytes.
     *
     * @details The allocated memory is returned as a valid pointer in the
     *    PROCESS virtual memory range. The allocated heap is valid and maps to
     *    the physical memory. In case of failure this function throws an
     *    exception.
     *
     * Throws EXCEPTION_FAILED in case of failure.
     *
     * @param size Number of bytes to allocate
     * @return A pointer to the newly allocated block. This pointer should be
     *    freed with the 'smallMemoryRealloc()' function.
     */
    static void* smallMemoryAllocation(uint size);

    /**
     * @brief Try to change the amount of memory allocated by the function
     *   'smallMemoryAllocation()' to a new size.
     *
     * @details The function tries to expand or shrink the number of bytes
     *    acquired by the 'mem' block. In case the next block is occupied
     *    reallocation may occur. In case of failure the old block doesn't
     *    change but an exception occurs. The function also copies the old block
     *    memory if the preserveMemory is 'true'
     *
     * Throws EXCEPTION_FAILED in case of failure or if 'mem' points into an
     * invalid location.
     *
     * @param mem Pointer to the old allocated block (by the smallMemAloc
     *    function).
     * @param newSize Number of bytes to allocate
     * @param preserveMemory Set to true if the memory block should be
     *    preserved. Set to false for optimizations.
     * @return A pointer into the new block.
     */
    static void* smallMemoryRealloc(void* mem, uint newSize,
                                    bool preserveMemory = true);

    /**
     * @brief Free the memory allocated by the small-memory allocation
     *    functions.
     *
     * @details Throws EXCEPTION_FAILED in case of failure or if 'mem' points
     *    into an invalid location.
     *
     * @param mem Pointer to the allocated block (by the smallMemAlloc
     *    function).
     */
    static void smallMemoryFree(void* mem);


    //
    // Threads functions
    //

    /**
     * @brief Puts the thread in a sleeping mode.
     *
     * @details During this time the thread context is blocked but interrupts
     *    can be invoked in the context of the thread (for example Windows' APC
     *    calls or Linux's Signal functions)
     *
     * @param seconds The number of seconds to stall
     * @throws cException in case of thread/system failure.
     */
    static void sleepSeconds(uint seconds);

    /**
     * @brief Puts the thread in a sleeping mode.
     *
     * @details During this time the thread context is blocked but interrupts
     *    can be invoked in the context of the thread (for example Windows' APC
     *    calls or Linux's Signal functions)
     *
     * @param milliseconds The number of milli-seconds to stall. 1000
     *    milliseconds equals one second.
     * @throws cException in case of thread/system failure.
     * @note the function doesn't guarantee real-time wake-up. The time given is
     *    only an approximate time. Context-switch or other events can stall the
     *    thread wake-up call.
     */
    static void sleepMillisecond(uint milliseconds);

    /**
     * @brief Puts the thread in a sleeping mode.
     *
     * @details During this time the thread context is blocked but interrupts
     *    can be invoked in the context of the thread (for example Windows' APC
     *    calls or Linux's Signal functions)
     *
     * @param microseconds The number of micro-seconds to stall. 1,000,000
     *    microseconds equals one second.
     * @throws cException in case of thread/system failure.
     * @note the function doesn't guarantee real-time wake-up. The time given is
     *    only an approximate time. Context-switch or other events can stall the
     *    thread wake-up call.
     */
    static void sleepMicroseconds(uint microseconds);

    //
    // OS functions
    //

    /**
     * @brief Returns a string which describes the last error code that happened
     *    in the
     *   system.
     *
     * @details For example, for a Win32 API application, the function will
     *    invoke a call to GetLastError and FormatMessage to build that string.
     *
     * @return A string which describes the last error code that happened in the
     *    system.
     */
    static cString getLastErrorString();

    //
    // Times functions
    //

    /**
     * @brief Return a number describing the current system time.
     *
     * @details It can be either a time or the number of ticks from the
     *    beginning of the operating system
     *
     * @return A number describing the current system time.
     */
    static cOSDef::systemTime getSystemTime();

    /**
     * @brief Calculate the interval between two system times and format it into
     *    a
     *   milliseconds difference
     *
     * @param currentTime The later system time.
     * @param startTime The earlier system time.
     * @return The interval between the two times in milliseconds.
     */
    static uint calculateTimesDiffMilli(cOSDef::systemTime currentTime,
                                        cOSDef::systemTime startTime);

    //
    // Debug
    //

    /**
     * @brief This function traps the debugger if connected, otherwise it
     *    continues
     *   execution.
     *
     * @note This function will trap the debugger if and only if the program is
     *    compiled with the _DEBUG macro
     */
    static void debuggerBreak();

    //
    // Files
    //
};


/** @} */
#endif // __TBA_STL_OS_OS_H

