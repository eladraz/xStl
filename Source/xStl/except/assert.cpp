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
/*
 * assert.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/except/traceStack.h"

#ifdef _DEBUG

#ifndef DOT_NET
// Normal C++ application
void assertFunction(bool condition,
                    unsigned int line,
                    const char* file,
                    const character* msg)
{
    // Test whether we should activate ASSERT
    if (!condition)
    {
        // Trace the assertion
        cString message("**********************************************\n");
        message+= "Assertion at file ";
        message+= file;
        message+= " line ";
        message+= cString((uint32)line);
        message+= "  ";
        message+= msg;
        message+= "\n**********************************************\n";

        TRACE(TRACE_VERY_HIGH, message);

        // Trace stack
        traceStack();

        // Trap the debugger
        cOS::debuggerBreak();

        // Terminate the program
        XSTL_THROW(cException, EXCEPTION_ASSERTION);
    }
}
#else
// Manage method
void assertFunction(bool condition, unsigned int line,
                    System::String^ file,
                    System::String^ msg)
{
    if (!condition)
    {
        System::Console::Write("Assertion failed ");
        System::Console::Write(file);
        System::Console::Write(" (");
        System::Console::Write(line);
        System::Console::Write("): ");
        System::Console::WriteLine(msg);
        // TODO! break!
        throw 0;
    }
}
#endif // DOT_NET
#endif // _DEUBG
