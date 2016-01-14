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

#ifndef __TBA_STL_OS_OSDEF_H
#define __TBA_STL_OS_OSDEF_H

/*
 * osdef.h
 *
 * Defines global data-structs for the operation-system handles and types.
 * The OS defs should define all types for all platfroms.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

/*
 * Here is a list of types which this header file declares:
 *
 * systemTime  - The system quata time. Different by ticks.
 * mutexHandle - The OS handle for the cMutex implementation.
 *
 * TODO:
 *   fileHandle - The OS handle for the cFile operation class.
 */

#if defined(XSTL_WINDOWS) || defined(XSTL_CE)
    /*
     * Windows implementation
     */
    #include "xStl/os/WindowsOS/winOSdef.h"

#elif defined XSTL_NTDDK
    /*
     * XDK NT driver implementation
     */
    #include "xStl/os/XDK/ddkOSdef.h"

#elif defined XSTL_LINUX
    /*
     * Linux implementation.
    */
    #include "xStl/os/unixOS/unixOSdef.h"

#elif defined XSTL_16BIT
    // 16 bit Turbo C compiler / Digital Mars compiler
    // For know these compiler doesn't support any files or threads(!)
    #include "xStl/os/tc16/tc16OSdef.h"
#else
    // Unknown platfrom
    #error "Unknown platfrom implementation!!!"
#endif

#endif // __TBA_STL_OS_OSDEF_H

