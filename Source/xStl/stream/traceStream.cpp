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
 * traceStream.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/stream/traceStream.h"

// This file is for debug only!
#ifdef _DEBUG

traceStream* traceStream::m_singletonHigh   = NULL;

traceStream* traceStream::m_singletonMedium = NULL;

traceStream* traceStream::m_singletonLow    = NULL;


traceStream::traceStream(int traceLevel) :
    cStringerStream(true),
    m_traceLevel(traceLevel)
{
}

traceStream::~traceStream()
{
    flush();
}

void traceStream::outputString(const cString& string)
{
    TRACE(m_traceLevel, string);
}

traceStream& traceStream::getTraceHigh()
{
    if (m_singletonHigh == NULL)
    {
        m_singletonHigh = new traceStream(TRACE_VERY_HIGH);
    }
    return *m_singletonHigh;
}

traceStream& traceStream::getTraceMedium()
{
    if (m_singletonMedium == NULL)
    {
        m_singletonMedium = new traceStream(TRACE_MEDIUM);
    }
    return *m_singletonMedium;
}

traceStream& traceStream::getTraceLow()
{
    if (m_singletonLow == NULL)
    {
        m_singletonLow = new traceStream(TRACE_LOW);
    }
    return *m_singletonLow;
}

void traceStream::cleanUpMemory()
{
    if (m_singletonLow != NULL) delete m_singletonLow;
    if (m_singletonHigh != NULL) delete m_singletonHigh;
    if (m_singletonMedium != NULL) delete m_singletonMedium;

    m_singletonLow = NULL;
    m_singletonHigh = NULL;
    m_singletonMedium = NULL;
}

#endif //_DEUBG
