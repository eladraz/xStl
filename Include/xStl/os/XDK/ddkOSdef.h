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

#ifndef __TBA_STL_OS_WINOS_H
#define __TBA_STL_OS_WINOS_H

/*
 * ddkOSdef.h
 *
 * Types, structs definitions and consts needed for the windows driver library
 *
 * Author: Elad Raz <e@eladraz.com>
 */

namespace cOSDef {
    /*
     * XDK implementation.
     *   The following code implements the Windows DDK over NT+ driver kit
     */

    // The mutexHandle
    class mutexHandle {
    public:
        mutexHandle() :
            isLocked(false),
            oldIrql(PASSIVE_LEVEL)
        {
            KeInitializeSpinLock(&spinLock);
        }

        // The spin-lock object
        KSPIN_LOCK spinLock;
        // The old IRQL level
        KIRQL oldIrql;
        // Is the spin-lock locked
        bool isLocked;
    };

    // The fileHandle
    typedef HANDLE fileHandle;
    // The systemTime.
    typedef uint64 systemTime;
    // The OS exception code type
    typedef NTSTATUS exceptionCode;
    // System-thread
    typedef PKTHREAD threadHandle;
    // The event handle
    typedef KEVENT eventHandle;
}; // namespace cOSDef

// Consts
#define INVALID_FILE_HANDLE   ((HANDLE)(-1))  // Invalid fileHandle
#define INVALID_THREAD_HANDLE (NULL)          // Invalid threadHandle

#endif // __TBA_STL_OS_WINOS_H
