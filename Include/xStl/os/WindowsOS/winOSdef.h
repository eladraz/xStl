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

// For this files, the WIN32 macro must be defined...
#ifdef WIN32

#ifndef __TBA_STL_OS_WINOS_H
#define __TBA_STL_OS_WINOS_H

/*
 * winOSdef.h
 *
 * Types, structs definitions and consts needed for the windows operating system
 *
 * Author: Elad Raz <e@eladraz.com>
 */


/*
 * Windows implementation.
 *   The following code implements the Win32 API for Windows 9x, NT, 2000
 *   and Windows XP.
 */
#include <windows.h>
#include <winbase.h>

namespace cOSDef {
    // The mutexHandle
    typedef HANDLE mutexHandle;
    // The eventHandle
    typedef HANDLE eventHandle;
    // The fileHandle
    typedef HANDLE fileHandle;
    // The systemTime.
    typedef uint64 systemTime;
    //The kernel device Handle
    typedef HANDLE deviceHandle;
    // The OS exception code type
    typedef DWORD exceptionCode;
    // System-thread
    typedef HANDLE threadHandle;
}; // namespace cOSDef


// Consts
#define INVALID_FILE_HANDLE (INVALID_HANDLE_VALUE)  // Invalid fileHandle
#define INVALID_THREAD_HANDLE (0)                   // Invalid threadHandle

/*
 * Handle reference count object. When the object is
 */
class cWinHandleStorage
{
public:
    /*
     * Constructor. Save the handle for the object
     */
    cWinHandleStorage(HANDLE object) :
        m_object(object)
    {
    }

    /*
     * Destructor. Close the handle.
     */
    ~cWinHandleStorage()
    {
        if (m_object != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_object);
        }
    }

    /*
     * Return the handle for the object
     */
    HANDLE getHandle() const { return m_object; }

private:
    // The handle object.
    HANDLE m_object;
};

#endif // __TBA_STL_OS_WINOS_H

#endif // WIN32
