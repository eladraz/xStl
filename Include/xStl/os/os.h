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

/*
 * os.h
 *
 * Declare on independet functions for operation system. The operation-system
 * should give support with the following functions:
 *   - Thead functions.
 *      The sleep function. Puts the thread into sleep mode. Only interruptiable
 *      function can be called from the thread context.
 *
 *   - Global random for the system.
 *      Allow changing the system global random, or accuire random from the system
 *      The random for the system can be an hardware random device and doesn't
 *      gerantuiee the same result at any way (Like a psedu-random-function).
 *      When the system loaded, the global random object is init into a strong
 *      random and can be left untouch (Using the srand() function)
 *
 *   - Time functions
 *      Getting the right time
 *      Getting the date of the year
 *
 *   - Memory operation.
 *      Yes. The 'memcpy()' function. The implementation of the operation system
 *      can use an hardware excelerator and avoid overloading the CPU. The OS can
 *      notify the application when the copying has completed or block the thread.
 *      Also define the memset() function which init memory.
 *
 *   - Memory allocation and deallocation for small amount of memory (heap)
 *      The global operation new and delete should use this functions. The
 *      memory is very fast, usually it is cached in the hardware. Also OS
 *      can provide some kind of security by allocating zero-page memory.
 *      NOTE: this kind of memory is NOT sharable and usually it's not paged
 *
 * TODO:
 *
 *   - Memory allocation and deallocation of large amount of memory (pages)
 *      Programs which wants to allocate big memory should use this functions
 *      Instead of using operator new/delete.
 *      NOTE: this kind of memory is NOT sharable.
 *
 *   - Far memory allocation.
 *      Try to allocate memory which can be share to a number of processor or
 *      computers. Usually this memory can be found at number of processors
 *      memory or even on different computers so this memory is very slow but
 *      can contains large amount of memory.
 *      NOTE: this kind of memory is sharable and slow.
 *
 *   - Files access.
 *      Simple interface for openning files, reading/writing to etc...
 *
 *   - Files enumeration
 *
 *   - Network access
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/osdef.h"

// Forward declation of cString
class cString;

/*
 * class cOS
 *
 * The operation system depended code. The class allows to use operation-system
 * implementation for the previous functions.
 *
 * These are very powerfull functions.
 *
 * All of these function are thread-safe and can be invoke from several of thread
 * without worries of data-loses.
 */
class cOS
{
public:
    //
    // Memory operations
    //

    /*
     * Copy a chunk of memory 'src' with size of 'size' bytes into 'dest'.
     * The copy can be synchounius or asyncyhounius. See the cWaitable for more
     * information about asyncyhounius calls.
     * NOTE:
     *   The function doesn't have any protection about the buffer sizes.
     *   The operation-system or the compiler may padd the buffers with magic
     *   and throw exception incase of overflows...
     *
     * dest - The buffer to copy the memory to.
     * src  - The source buffer where the data will copy from.
     * size - The number of bytes of the buffers.
     */
    static void memcpy(void* dest,
                       const void* src,
                       uint size
                       /*cWaitable wait*/);

    //
    // Memory allocation
    //

    /*
     * Try to allocate a small memory block with size 'size' bytes. The
     * allocated memory is returned as a valid pointer as the PROCESS
     * virtual memory range. The allocated heap is valid and map to the
     * physical memory.
     * Incase of faliure this function thorws exception.
     *
     * size - Number of bytes to allocated
     *
     * Return a pointer to the new allocated block. This pointer should be
     * freed with the 'smallMemoryRealloc()' function.
     *
     * Thorws EXCEPTION_FALID incase of faliure.
     */
    static void* smallMemoryAllocation(uint size);

    /*
     * Try to change the amount of memory allocated by the function
     * 'smallMemoryAllocation()' into new size. The function tries to expand
     * or shrink the number of bytes accuiered by the 'mem' block. Incase the
     * next block is occupied rellocation maybe accuered.
     * Incase of faliure the old block doesn't change but exception occuered.
     * The functions also copies the old block memory if the preserveMemory is
     * 'true'
     *
     * mem            - Pointer to the old allocated block (by the smallMemAloc
     *                  function).
     * newSize        - Number of bytes to allocated
     * preserveMemory - Set to true if the memory block should be preserved.
     *                  Set to false for optimizations.
     *
     * Return a pointer into the new block.
     * Thorws EXCEPTION_FALID incase of faliure or if the 'mem' is pointed into
     * invalid location.
     */
    static void* smallMemoryRealloc(void* mem, uint newSize,
                                    bool preserveMemory = true);

    /*
     * Free the memory allocated by the small-memory allocations functions.
     *
     * mem            - Pointer to the allocated block (by the smallMemAlloc
     *                  function).
     *
     * Thorws EXCEPTION_FALID incase of faliure or if the 'mem' is pointed into
     * invalid location.
     */
    static void smallMemoryFree(void* mem);


    //
    // Threads functions
    //

    /*
     * Puts the thread in a sleeping mode. During this time the thread context
     * is block but interrupts can be invoke in the context of the thread (for
     * example window's APC calls or linux's Singnal functions)
     *
     * seconds - The number of seconds to be stall
     *
     * Throws exception in case of thread/system failure.
     */
    static void sleepSeconds(uint seconds);

    /*
     * Puts the thread in a sleeping mode. During this time the thread context
     * is block but interrupts can be invoke in the context of the thread (for
     * example window's APC calls or linux's Singnal functions)
     * Note the functions doesn't gerantiuee real-time wake-up. The time given
     * is only approximate time. Context-switch or other events can stall the
     * thread wake-up call.
     *
     * miliseconds - The number of milli-seconds to be stall.
     *               1000 miliseconds equals to one second.
     *
     * Throws exception in case of thread/system failure.
     */
    static void sleepMillisecond(uint miliseconds);

    /*
     * Puts the thread in a sleeping mode. During this time the thread context
     * is block but interrupts can be invoke in the context of the thread (for
     * example window's APC calls or linux's Singnal functions)
     * Note the functions doesn't gerantiuee real-time wake-up. The time given
     * is only approximate time. Context-switch or other events can stall the
     * thread wake-up call.
     *
     * microseconds - The number of micro-seconds to be stall.
     *                1,000,000 microseconds equals to one second.
     *
     * Throws exception in case of thread/system failure.
     */
    static void sleepMicroseconds(uint microseconds);

    //
    // OS functions
    //

    /*
     * Returns a string which describes the lase error code that happen in
     * the system.
     * For example, For win32 API application, the function will invoke a call
     * to GetLastError and FormatMessage to build that string.
     */
    static cString getLastErrorString();

    //
    // Times functions
    //

    /*
     * Return a number describing the current system time. It can be either a
     * time or the number of ticks from the beginning of the operating system
     */
    static cOSDef::systemTime getSystemTime();

    /*
     * Calculate a difference interval in two system-times and format it into
     * milliseconds diff
     */
    static uint calculateTimesDiffMilli(cOSDef::systemTime currentTime,
                                        cOSDef::systemTime startTime);

    //
    // Debug
    //

    /*
     * This function trap the debugger if connected, otherwise continue
     * execution.
     * NOTE: This function will trap the deubugger if and only if the program
     *       compiled with _DEBUG macro
     */
    static void debuggerBreak();

    //
    // Files
    //
};

#endif // __TBA_STL_OS_OS_H

