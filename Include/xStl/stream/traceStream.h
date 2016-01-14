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

/*
 * traceStream.h
 *
 * Declaration of the trace stream classes (traceHigh, traceLow, traceMedium).
 *
 * Usage:
 *    traceHigh( << "Hello world!" << endl);
 *
 *    This will be compiled only in debug mode. The strings will not be insert in
 *    release version.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/stream/stringerStream.h"

#ifndef _DEBUG
    // Release
#define traceHigh(x)   do {} while (0)
#define traceMedium(x) do {} while (0)
#define traceLow(x)    do {} while (0)
#else
    // Debug

/*
 * traceStream
 *
 * Output steam directed to the TRACE macro.
 *
 * This class has 3 functions which returns singleton inside the system
 * in order to allow this class to be called from destructors of global
 * objects, the resources allocated by this methods must be destruct in
 * the lib C++ utility.
 * Ring3 application doesn't have a build way method of doing so, The
 * XDK method however calls to the destructors.
 */
class traceStream : public cStringerStream
{
public:
    /*
     * Constructor.
     * This stream uses a cache and waits until new-line is send.
     */
    traceStream(int traceLevel);

    /*
     * Flash the trace
     */
    virtual ~traceStream();

    /*
     * Return a global (singelton) object for the traces
     */
    static traceStream& getTraceHigh();
    static traceStream& getTraceMedium();
    static traceStream& getTraceLow();

    /*
     * Destroy all stack signletons.
     */
    static void cleanUpMemory();

protected:
    /*
     * Output the string into the m_data member.
     * See cStringerStream::outputString
     */
    virtual void outputString(const cString& string);

private:
    // The trace level for this class
    int m_traceLevel;

    // Deny copy-constructor and operator =
    traceStream(const traceStream& other);
    traceStream& operator = (const traceStream& other);

    // The trace objects
    static traceStream* m_singletonHigh;
    static traceStream* m_singletonMedium;
    static traceStream* m_singletonLow;
};


/*
 * Define the traceHigh, traceLow, traceMedium
 */
#define traceHigh(x)   traceStream::getTraceHigh()   << x

// The preprocessor ONLY_HIGHTRACE allow the application to ignore all
// medium and low trace and keep only high-trace commands
#ifdef ONLY_HIGHTRACE
    // Ignore debugging for non-high tracing
    #define traceMedium(x)
    #define traceLow(x)
#else
    // Continue to normal debugging
    #define traceMedium(x) traceStream::getTraceMedium() << x
    #define traceLow(x)    traceStream::getTraceLow()    << x
#endif

#endif // _DEBUG

#endif //__TBA_STL_TRACESTREAM_H
