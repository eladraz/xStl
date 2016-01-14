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

#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/except/trace.h"
#include "xStl/except/traceStack.h"

#ifdef I386
// TODO! Add support for 32 bit and other processors!

/***********************************************************
 * IA32 implementation cdecl/stdcall stack parser
 *
 * FFFFFFFF   ~       Arguments       ~
 *            +-----------------------+
 *            |     Return address    |
 *            +-----------------------+
 *            | Previous base-pointer |     <- basePointer
 *            +-----------------------+
 * 00000000   ~      Local stack      ~
 *
 */

void* getPreviousFunctionBase(void* basePointer)
{
    addressNumericValue previousBase = *((addressNumericValue*)(basePointer));
    addressNumericValue myBasePointer = getNumeric(basePointer);
    if (previousBase <= myBasePointer)
    {
        // Stack error
        return NULL;
    }

    if ((previousBase - myBasePointer) > 0x2000)
    {
        // Stack error, local stack bigger than 8kb
        return NULL;
    }

    return getPtr(previousBase);
}

void* getReturnAddress(void* basePointer)
{
    return getPtr(*((addressNumericValue*)(basePointer) + 1));
}

#define getBasePointer(x) { _asm { mov x, ebp }; }

/***********************/

#define MAX_STACK_TRACE (10)

void traceStack()
{
    XSTL_TRY
    {
        // Get the current base-pointer
        void* basePointer;
        getBasePointer(basePointer);

        // Skip the current function. (e.g. assertFunc base)
        basePointer = getPreviousFunctionBase(basePointer);

        uint previousCount = 0;

        while ((basePointer != NULL) && (previousCount < MAX_STACK_TRACE))
        {
            // Query the return address
            void* addr = getReturnAddress(basePointer);
            TRACE(TRACE_VERY_HIGH, cString("  Level: ") + HEXBYTE((uint8)previousCount) +
                                   "  " + HEXDWORD(getNumeric(addr)) + "\n");
            // TODO! Add symbols
            basePointer = getPreviousFunctionBase(basePointer);
            previousCount++;
        }
    }
    XSTL_CATCH_ALL
    {
        TRACE(TRACE_VERY_HIGH, "   error at stack parsing\n");
    }
}

#else // Non Intel 32 bit processors
    void traceStack()
    {
        TRACE(TRACE_VERY_HIGH, "Not ready yet.\n");
    }
#endif // !XSTL_16BIT
