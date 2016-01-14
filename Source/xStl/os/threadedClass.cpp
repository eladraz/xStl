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
 * ThreadedClass.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/except/exception.h"
#include "xStl/os/threadedClass.h"
#include "xStl/stream/traceStream.h"

cThreadedClass::cThreadedClass() :
    m_isStarted(false),
    m_isDone(false)
{
}

void cThreadedClass::start()
{
    CHECK(!m_isStarted);
    // Execute the thread
    m_isStarted = true;
    m_theInstance.execute((ThreadRoutine)threadedClassThreadFunction, this);
}

void cThreadedClass::wait()
{
    if (!m_isStarted)
        return;
    if (isDone())
        return;
    m_theInstance.wait();
}

bool cThreadedClass::isDone() const
{
    return m_isDone;
}

cThreadedClass::~cThreadedClass()
{
    // Terminate the thread if started
    if ((m_isStarted) && (!isDone())) {
        // Notify developer for premature thread-death.
        ASSERT_MSG(m_isStarted1, XSTL_STRING("Premature thread death"));
        // Wait until the thread will truely executed
        while (!m_isStarted1);
        // Destroy the thread
        m_theInstance.kill();
    }
}

cThread& cThreadedClass::getThreadHandle()
{
    return m_theInstance;
}

const cThread& cThreadedClass::getThreadHandle() const
{
    return m_theInstance;
}

void* cThreadedClass::threadedClassThreadFunction(void* context)
{
    XSTL_TRY
    {
        cThreadedClass* theObject = (cThreadedClass*)context;
        ASSERT(theObject != NULL);

        // Execute the thread
        theObject->m_isStarted1 = true;
        theObject->run();

        // Change the state of the machine.
        theObject->m_isDone = true;
    }
    XSTL_CATCH(cException& e)
    {
        // An exception during thread execution...
        traceHigh("ThreadClass: Exception " << e.getMessage() << "(" <<
                                               e.getID() << ")" << " throwed "
                                            << endl);
    }
    // In Linux pthreads uses force_unwined to cancel threads
    // Catch all exception will causes immediate abortion of the entire program
    // Waiting for a normal fix
    //    - See: http://gcc.gnu.org/ml/gcc/2003-12/msg00743.html
#ifndef XSTL_LINUX
    XSTL_CATCH_ALL
    {
        // An exception during thread execution...
        traceHigh("ThreadClass: Unknown exception throwed 1" << endl);
        cThread::terminate();
        traceHigh("ThreadClass: Unknown exception throwed 2" << endl);
    }
#endif

    // Destroy the thread
    cThread::terminate();
    return NULL;
}

