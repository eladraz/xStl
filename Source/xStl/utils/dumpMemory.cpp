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
 * dumpMemory.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/fragmentsDescriptor.h"
#include "xStl/os/virtualMemoryAccesser.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/utils/algorithm.h"
#include "xStl/utils/dumpMemory.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/stringerStream.h"

DumpMemory::DumpMemory(const cVirtualMemoryAccesser& memory,
                       addressNumericValue startAddress,
                       addressNumericValue endAddress,
                       addressNumericValue relocation,
                       uint elementsCount) :
    m_shouldReadMemory(true),
    m_elementsCount(elementsCount),
    m_startAddress(startAddress),
    m_endAddress(endAddress),
    m_relocation(relocation),
    m_memory(&memory),
    m_buffer(NULL),
    m_fragments(NULL),
    m_remoteAddress(0)
{
    if (m_startAddress > m_endAddress)
        t_swap(m_startAddress, m_endAddress);
}

DumpMemory::DumpMemory(const cBuffer& buffer,
                       const cFragmentsDescriptor& fragments,
                       const remoteAddressNumericValue& startAddress,
                       uint elementsCount) :
    m_shouldReadMemory(false),
    m_elementsCount(elementsCount),
    m_memory(NULL),
    m_buffer(&buffer),
    m_fragments(&fragments),
    m_remoteAddress(startAddress)
{
}

void DumpMemory::dumpLine(addressNumericValue pos,
                          uint count,
                          cBuffer& data,
                          cStringerStream& out) const
{
    // Get the memory
    cFragmentsDescriptor fragments;
    m_memory->memread(pos,
                      data.getBuffer(),
                      data.getSize(),
                      &fragments);

    // And dump it
    remoteAddressNumericValue addr(pos - m_relocation);
    internalDumpLine(addr,
                     count, data.getBuffer(), fragments, out);
}

void DumpMemory::internalDumpLine(const remoteAddressNumericValue& pos,
                                  uint count,
                                  const uint8* data,
                                  const cFragmentsDescriptor& fragments,
                                  cStringerStream& out) const
{
    // Output the address
    out << HEXREMOTEADDRESS(pos) << "   ";

    uint j;
    remoteAddressNumericValue addr = pos;
    for (j = 0; j < count; j++, NR_ADDRESS(addr)++)
    {
        if (fragments.isMemoryValid(addr))
            out << HEXBYTE(data[j]);
        else
            out << "??";

        if (j == (m_elementsCount / 2) - 1)
            // Put a break - in the middle.
            out << "-";
        else if ((j & 1) == 1)
            // XX?? YYZZ
            out << " ";
    }

    for (j = count; j < m_elementsCount; j++)
    {
        if ((j & 1) == 1)
            out << "   ";
        else
            out << "  ";
    }

    out << " ";

    addr = pos;
    for (j = 0; j < count; j++, NR_ADDRESS(addr)++)
    {
        if (fragments.isMemoryValid(addr))
        {
            if (!cChar::isPrintable(data[j]))
                out << ".";
            else
                out << (char)(data[j]);
        }
        else
            out << "?";
    }
    out << endl;
}

cStringerStream& operator << (cStringerStream& out,
                              const DumpMemory& memory)
{
    if (memory.m_shouldReadMemory)
    {
        addressNumericValue pos = memory.m_startAddress;
        uint lines = (memory.m_endAddress - memory.m_startAddress) /
                      memory.m_elementsCount;

        cBuffer data(memory.m_elementsCount);
        for (uint i = 0; i < lines; i++)
        {
            memory.dumpLine(pos,
                            memory.m_elementsCount,
                            data,
                            out);
            pos+= memory.m_elementsCount;
        }

        uint left = memory.m_endAddress - pos;
        if (left != 0)
            memory.dumpLine(pos,
                            left,
                            data,
                            out);
    } else
    {
        uint lines = memory.m_buffer->getSize() / memory.m_elementsCount;
        uint8* pos = memory.m_buffer->getBuffer();
        remoteAddressNumericValue addr = memory.m_remoteAddress;
        for (uint i = 0; i < lines; i++)
        {
            memory.internalDumpLine(addr,
                                    memory.m_elementsCount,
                                    pos,
                                    *(memory.m_fragments),
                                    out);

            pos+= memory.m_elementsCount;
            NR_ADDRESS(addr)+= memory.m_elementsCount;
        }

        uint left = getNumeric(memory.m_buffer->getBuffer());
        left+= memory.m_buffer->getSize();  // End pointer
        left-= getNumeric(pos);             // Minus current pointer

        if (left != 0)
            memory.internalDumpLine(addr,
                                    left,
                                    pos,
                                    *(memory.m_fragments),
                                    out);
    }

    return out;
}
