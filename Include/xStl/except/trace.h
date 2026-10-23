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

/**
 * @addtogroup xstl_except
 * @{
 */

/**
 * @file trace.h
 * @brief Definition of a trace command in the program.
 *
 * @details The TRACE macro will be shown in the compiled code only if the
 *    _DEBUG macro is defined.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/string.h"

/*
 * The TRACE_LEVEL macro
 *
 * Control whether the message is important or not.
 */
#ifndef TRACE_LEVEL
    /**
     * @brief The trace level of the TRACE macro, used to control whether a
     *    message is important or not. It can be overridden before including the
     *    header.
     */
    #define TRACE_LEVEL (0)
#endif



/**
 * @brief Which implementation will be used for the output messages:
 */
#define TRACE_CERR    (0) ///< - using the cerr object
#define TRACE_MSG     (1) ///< - message box, cross platform
#define TRACE_LOG     (2) ///< - into a log file.



/*
 * TRACE_LOG_FILENAME
 *
 * When the user wants to send the traces into a file, the filename should be
 * stored
 * here.
 *
 * The default filename value is "log.txt"
 */
#ifndef TRACE_LOG_FILENAME
    /**
     * @brief The file name of the trace log when traces are sent into a file.
     *    The default is "log.txt".
     */
    #define TRACE_LOG_FILENAME "log.txt"
#endif


/*
 * Declare the standard output of the trace.
 */
#ifndef TRACE_METHOD
    /**
     * @brief The implementation used for the output of the traces. The default
     *    is TRACE_CERR.
     */
    #define TRACE_METHOD (TRACE_CERR)
#endif

/**
 * @brief Priorities
 */
#define TRACE_VERY_LOW   (0)
/**
 * @brief Trace priority: low.
 */
#define TRACE_LOW        (3)
/**
 * @brief Trace priority: medium.
 */
#define TRACE_MEDIUM     (5)
/**
 * @brief Trace priority: high.
 */
#define TRACE_HIGH       (8)
/**
 * @brief Trace priority: very high.
 */
#define TRACE_VERY_HIGH (10)


#ifndef TRACE_ENTER
/**
 * @brief The end of line sequence used by the traces.
 */
#define TRACE_ENTER "\r\n"
#endif


/*
 * traceMessage()
 *
 * Internal function which implements the TRACE macro. Prints the message to the
 * screen if the trace_level is high enough.
 *
 * The function is cross-platform UNIX and WINDOWS.
 */
#ifdef _DEBUG
    void traceMessage(int trace_level, const cString& message);
#endif

// Remove all previous library definitions for the TRACE macro
#undef TRACE

/*
 * TRACE(int trace_level, char *message) macro
 *
 * This macro will be active only in debug mode.
 * The macro will call the traceMessage() function
 * that will handle the tracing message.
 */
#ifdef _DEBUG
    /**
     * @brief Trace a message (debug builds only; empty in release builds).
     *
     * The macro calls traceMessage(), which prints the message if the level is
     * high enough.
     *
     * @param trace_level The importance of the message. See TRACE_VERY_LOW to
     *    TRACE_VERY_HIGH.
     * @param message The message to print (a cString).
     */
    #define TRACE(trace_level, message)    traceMessage(trace_level, message);
#else
    /**
     * @brief Trace a message (debug builds only; empty in release builds).
     *
     * @param trace_level The importance of the message. See TRACE_VERY_LOW to
     *    TRACE_VERY_HIGH.
     * @param message The message to print (a cString).
     */
    #define TRACE(trace_level, message)
#endif



/** @} */
#endif

