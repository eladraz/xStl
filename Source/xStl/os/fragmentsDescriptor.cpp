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
 * fragmentsDescriptor.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/fragmentsDescriptor.h"
#include "xStl/except/trace.h"
#include "xStl/data/list.h"
#include "xStl/stream/basicIO.h"

cFragmentsDescriptor::cFragmentsDescriptor()
{
}

bool cFragmentsDescriptor::isMemoryValid(
                const remoteAddressNumericValue& address) const
{
    MemoryRangeFragmentation::iterator i = m_fragments.begin();
    for (; i != m_fragments.end(); ++i)
    {
        if (address.isInsideRange((*i).m_start, (*i).m_end))
            return false;
    }

    return true;
}

const cFragmentsDescriptor::MemoryRangeFragmentation&
    cFragmentsDescriptor::getFragmentation() const
{
    return m_fragments;
}

void cFragmentsDescriptor::appendFragmentBlock(
                const remoteAddressNumericValue& start,
                const remoteAddressNumericValue& end)
{
    m_fragments.append(MemoryRange(start, end));
}

bool cFragmentsDescriptor::isValid() const
{
    return true;
}

void cFragmentsDescriptor::deserialize(basicInput& inputStream)
{
    uint16 count;
    inputStream.streamReadUint16(count);
    m_fragments.removeAll();

    remoteAddressNumericValue temp(0); // Save some stack memory
    MemoryRange newRange(temp, temp);

    for (uint i = 0; i < count; i++)
    {
        newRange.deserialize(inputStream);
        m_fragments.append(newRange);
    }
}

void cFragmentsDescriptor::serialize(basicOutput& outputStream) const
{
    uint count = m_fragments.length();
    CHECK(count <= 0xFFFF);

    outputStream.streamWriteUint16((uint16)count);
    MemoryRangeFragmentation::iterator i = m_fragments.begin();
    for (; i != m_fragments.end(); ++i)
    {
        (*i).serialize(outputStream);
    }
}

//////////////////////////////////////////////////////////////////////////
// cFragmentsDescriptor::MemoryRange
cFragmentsDescriptor::MemoryRange::MemoryRange(const remoteAddressNumericValue& start,
                                               const remoteAddressNumericValue& end) :
    m_start(start),
    m_end(end)
{
}

bool cFragmentsDescriptor::MemoryRange::isValid() const
{
    return true;
}

void cFragmentsDescriptor::MemoryRange::deserialize(basicInput& inputStream)
{
    m_start = inputStream.streamReadRemoteAddress();
    m_end = inputStream.streamReadRemoteAddress();
}

void cFragmentsDescriptor::MemoryRange::serialize(basicOutput& outputStream) const
{
    outputStream.streamWriteRemoteAddress(m_start);
    outputStream.streamWriteRemoteAddress(m_end);
}
