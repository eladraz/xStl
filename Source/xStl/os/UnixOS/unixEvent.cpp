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

/*
 * unixEvent.cpp
 *
 * Implementation file for UNIX operating system using POSIX API
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/event.h"
#include "xStl/os/osdef.h"

#if defined(XSTL_LINUX)

/*
 * event.cpp
 *
 * Implementation file
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/os/event.h"

cEvent::cEvent()
{
    // Create reset event
    pthread_mutex_init(&m_handle.mutex, 0);
    pthread_cond_init(&m_handle.cond, 0);
    m_handle.triggered = false;
}

cEvent::cEvent(const cString& eventName, bool shouldCreate)
{
    CHECK_FAIL();
}

cEvent::~cEvent()
{
    pthread_mutex_destroy(&m_handle.mutex);
    pthread_cond_destroy(&m_handle.cond);
}

void cEvent::setEvent()
{
    pthread_mutex_lock(&m_handle.mutex);
    m_handle.triggered = true;
    pthread_cond_signal(&m_handle.cond);
    pthread_mutex_unlock(&m_handle.mutex);
}

void cEvent::resetEvent()
{
    pthread_mutex_lock(&m_handle.mutex);
    m_handle.triggered = false;
    pthread_mutex_unlock(&m_handle.mutex);
}

bool cEvent::isEventSet() const
{
    return m_handle.triggered;
}

void cEvent::wait() const
{
    pthread_mutex_lock((pthread_mutex_t*)&m_handle.mutex);
    while (!m_handle.triggered)
        pthread_cond_wait((pthread_cond_t*)&m_handle.cond, (pthread_mutex_t*)&m_handle.mutex);
    pthread_mutex_unlock((pthread_mutex_t*)&m_handle.mutex);
}

cOSDef::eventHandle cEvent::getHandle()
{
    return m_handle;
}

#endif // WIN32
