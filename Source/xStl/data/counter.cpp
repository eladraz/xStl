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
 * counter.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/lock.h"
#include "xStl/data/counter.h"

cCounter::cCounter(counter_t initValue) :
    m_value(initValue)
    #if (!(defined(XSTL_NTDDK) || defined(XSTL_WINDOWS) || defined(XSTL_CE)))
        ,m_lock()
    #endif
{
}

counter_t cCounter::getValue() const
{
    return m_value;
}

counter_t cCounter::increase()
{
    #if (!(defined(XSTL_NTDDK) || defined(XSTL_WINDOWS) || defined(XSTL_CE)))
        cLock lock(m_lock);
        m_value++;
        return m_value;
    #else
        return InterlockedIncrement((PLONG)&m_value);
    #endif
}

counter_t cCounter::decrease()
{
    #if (!(defined(XSTL_NTDDK) || defined(XSTL_WINDOWS) || defined(XSTL_CE)))
        cLock lock(m_lock);
        m_value--;
        return m_value;
    #else
        return InterlockedDecrement((PLONG)&m_value);
    #endif
}

void cCounter::setValue(const counter_t& newValue)
{
    #if (!(defined(XSTL_NTDDK) || defined(XSTL_WINDOWS) || defined(XSTL_CE)))
        cLock lock(m_lock);
        m_value = newValue;
    #else
        InterlockedExchange((PLONG)&m_value, newValue);
    #endif
}

cCounter& cCounter::operator++()
{
    increase();
    return *this;
}

cCounter& cCounter::operator--()
{
    decrease();
    return *this;
}

