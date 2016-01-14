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

#ifndef __TBA_STL_TRACE_H
#define __TBA_STL_TRACE_H

/*
 * trace.h
 *
 * Definition of a trace command in the program. The TRACE macro will shown in
 * the complied code only if the _DEBUG marco will be defined.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"

/*
 * The TRACE_LEVEL marco
 *
 * Control whether the message is improtant or not.
 */
#ifndef TRACE_LEVEL
    #define TRACE_LEVEL (0)
#endif



/*
 * TRACE_METHOD
 *
 * Which implementation will be the output messages:
 */
#define TRACE_CERR    (0) // - using the cerr object
#define TRACE_MSG     (1) // - message box, cross platform
#define TRACE_LOG     (2) // - into a log file.



/*
 * TRACE_LOG_FILENAME
 *
 * When the user want to send the traces into a file, the filename should stored
 * here.
 *
 * The default filename value is "log.txt"
 */
#ifndef TRACE_LOG_FILENAME
    #define TRACE_LOG_FILENAME "log.txt"
#endif


/*
 * Declare the standart output of the trace.
 */
#ifndef TRACE_METHOD
    #define TRACE_METHOD (TRACE_CERR)
#endif

/*
 * Prioreties
 */
#define TRACE_VERY_LOW   (0)
#define TRACE_LOW        (3)
#define TRACE_MEDIUM     (5)
#define TRACE_HIGH       (8)
#define TRACE_VERY_HIGH (10)


#ifndef TRACE_ENTER
#define TRACE_ENTER "\r\n"
#endif


/*
 * traceMessage()
 *
 * Internal function which implement the TRACE macro. Prints the message to the
 * screen if the trace_level is high enough.
 *
 * The function is cross-platform UNIX and WINDOWS.
 */
#ifdef _DEBUG
    void traceMessage(int trace_level, const cString& message);
#endif

// Remove all previous libraries definitions for the TRACE macro
#undef TRACE

/*
 * TRACE(int trace_level, char *message) macro
 *
 * This macro will be active only in debug mode.
 * The macro will call the traceMessage() function
 * that will handle the tracing message.
 */
#ifdef _DEBUG
    #define TRACE(trace_level, message)    traceMessage(trace_level, message);
#else
    #define TRACE(trace_level, message)
#endif


#endif

