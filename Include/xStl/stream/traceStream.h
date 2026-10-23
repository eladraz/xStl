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

#ifndef __TBA_STL_TRACESTREAM_H
#define __TBA_STL_TRACESTREAM_H

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file traceStream.h
 * @brief Declaration of the trace stream classes (traceHigh, traceLow,
 *   traceMedium).
 *
 * @details Usage:
 *
 * @code
 *    traceHigh( << "Hello world!" << endl);
 *
 *    This will be compiled only in debug mode. The strings will not be
 *    inserted in
 *    release version.
 * @endcode
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/stream/stringerStream.h"

#ifndef _DEBUG
/**
 * @brief Trace a high priority message (does nothing in release builds).
 *
 * @param x The message to trace.
 */
#define traceHigh(x)   do {} while (0)
/**
 * @brief Trace a medium priority message (does nothing in release builds).
 *
 * @param x The message to trace.
 */
#define traceMedium(x) do {} while (0)
/**
 * @brief Trace a low priority message (does nothing in release builds).
 *
 * @param x The message to trace.
 */
#define traceLow(x)    do {} while (0)
#else
    // Debug

/**
 * @brief Output stream directed to the TRACE macro.
 *
 * @details This class has 3 functions which return singletons inside the system
 *   in order to allow this class to be called from destructors of global
 *   objects, the resources allocated by these methods must be destructed in
 *   the lib C++ utility.
 *   Ring3 applications don't have a built-in way of doing so. The
 *   XDK method however calls the destructors.
 */
class traceStream : public cStringerStream
{
public:
    /**
     * @brief Constructor. This stream uses a cache and waits until new-line is
     *    sent.
     */
    traceStream(int traceLevel);

    /**
     * @brief Flush the trace
     */
    virtual ~traceStream();

    /**
     * @brief Return a global (singleton) object for the traces
     *
     * @return A global (singleton) object for the traces.
     */
    static traceStream& getTraceHigh();
    static traceStream& getTraceMedium();
    static traceStream& getTraceLow();

    /**
     * @brief Destroy all stack singletons.
     */
    static void cleanUpMemory();

protected:
    /**
     * @brief Output the string into the m_data member.
     *
     * @see cStringerStream::outputString
     */
    virtual void outputString(const cString& string);

private:
    /**
     * @brief The trace level for this class
     */
    int m_traceLevel;

    /**
     * @brief Deny copy-constructor and operator =
     */
    traceStream(const traceStream& other);
    traceStream& operator = (const traceStream& other);

    /**
     * @brief The trace objects
     */
    static traceStream* m_singletonHigh;
    static traceStream* m_singletonMedium;
    static traceStream* m_singletonLow;
};


/**
 * @brief Define the traceHigh, traceLow, traceMedium
 */
#define traceHigh(x)   traceStream::getTraceHigh()   << x

// The preprocessor ONLY_HIGHTRACE allow the application to ignore all
// medium and low trace and keep only high-trace commands
#ifdef ONLY_HIGHTRACE
    /**
     * @brief Ignore debugging for non-high tracing
     */
    #define traceMedium(x)
    #define traceLow(x)
#else
    /**
     * @brief Continue to normal debugging
     */
    #define traceMedium(x) traceStream::getTraceMedium() << x
    #define traceLow(x)    traceStream::getTraceLow()    << x
#endif

#endif // _DEBUG


/** @} */
#endif //__TBA_STL_TRACESTREAM_H
