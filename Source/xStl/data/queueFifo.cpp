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
 * cQueueFifo.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/array.h"
#include "xStl/data/queueFifo.h"

cQueueFifo::cQueueFifo(uint size) :
    m_queue(size),
    m_readPointer(0),
    m_writePointer(0),
    m_isFull(false),
    m_queueSize(size)
{
}

uint cQueueFifo::getFreespace()
{
    //
    // There is two states, when the write pointer is before the read pointer
    // or otherwise:
    //             size
    // ---------------------------------
    //    |                    |
    //  read              write
    //
    //  And the free space is therefore: size - write + read
    //
    // or:
    //
    //             size
    // ---------------------------------
    //    |                    |
    //  write             read
    //
    //  And the free space is therefore: read - write
    //
    if (m_readPointer == m_writePointer)
    {
        if (m_isFull)
            return 0;
        else
            return m_queueSize;
    }
    return (m_readPointer <= m_writePointer) ? m_queueSize - m_writePointer + m_readPointer :
                                               m_readPointer - m_writePointer;
}

uint cQueueFifo::getOccupyspace()
{
    // Consulte the diagram in the 'getQueueFreespace'
    // This is just 'm_queueSize - X'
    // return (m_readPointer <= m_writePointer) ? m_writePointer - m_readPointer :
    //                                            m_queueSize - m_readPointer + m_writePointer;
    return m_queueSize - getFreespace();
}


uint cQueueFifo::getQueueFreespace()
{
    // The interface was thread-safe
    return getFreespace();
}

uint cQueueFifo::getQueueOccupyspace()
{
    // The interface was thread-safe
    return getOccupyspace();
}


uint cQueueFifo::write(const uint8* buffer, uint count)
{
    uint freespace = getFreespace();
    uint8 *queue = m_queue.getBuffer();
    if (count >= freespace)
    {
        count = freespace;
        m_isFull = true;
    }


    if (m_writePointer < m_readPointer)
    {
        cOS::memcpy(queue + m_writePointer, buffer, count);
        m_writePointer+= count;
    } else
    {
        uint count1 = m_queueSize - m_writePointer;
        if (count > count1)
        {
            uint count2 = count - count1;
            cOS::memcpy(queue + m_writePointer, buffer, count1);
            cOS::memcpy(queue, buffer + count1, count2);
            m_writePointer = count2;
        } else
        {
            cOS::memcpy(queue + m_writePointer, buffer, count);
            m_writePointer+= count;
        }
    }

    return count;
}


uint cQueueFifo::read(uint8 *buffer, uint count)
{
    uint occupyspace = getOccupyspace();
    uint8 *queue = m_queue.getBuffer();
    if (count > occupyspace) count = occupyspace;

    if (m_writePointer > m_readPointer)
    {
        cOS::memcpy(buffer, queue + m_readPointer, count);
        m_readPointer+= count;
    } else
    {
        uint count1 = m_queueSize - m_readPointer;
        if (count > count1)
        {
            uint count2 = count - count1;
            cOS::memcpy(buffer,          queue + m_readPointer, count1);
            cOS::memcpy(buffer + count1, queue, count2);
            m_readPointer = count2;
        } else
        {
            cOS::memcpy(buffer, queue + m_readPointer, count);
            m_readPointer+= count;
        }
    }

    if (count > 0)
    {
        m_isFull = false;
    }

    return count;
}
