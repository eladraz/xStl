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
 * setArray.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/data/setArray.h"

cSetArray::cSetArray(uint initializedSize,
                     bool shouldResetArray,
                     bool resetAsSet) :
    m_length(0)
{
    changeSize(initializedSize, false);
    if (shouldResetArray)
    {
        resetArray();
        if (resetAsSet)
        {
            for (uint i = 0 ; i < initializedSize ; i++)
            {
                set(i);
            }
        }
    }

}

uint cSetArray::getLength() const
{
    return m_length;
}

bool cSetArray::isSet(uint location) const
{
    CHECK(location < m_length);

    uint arrayIndex, bitIndex;
    calculatePosition(location, arrayIndex, bitIndex);

    return ((m_data[arrayIndex] >> bitIndex) & 1) == 1;
}

uint cSetArray::first() const
{
    uint arrayIndex = 0;
    uint arraySize = ((m_length + BITS_COUNT_PER_ELEMENT - 1))
                             >> BITS_COUNT_PER_ELEMENT_SHIFT;

    // Go over dwords, find first nonzero
    while (arrayIndex < arraySize)
    {
        if (m_data[arrayIndex] != 0)
            break;
        arrayIndex++;
    }
    // Don't overflow
    if (arrayIndex >= arraySize)
        return m_length;

    uint32 data = m_data[arrayIndex];

    uint location = arrayIndex * BITS_COUNT_PER_ELEMENT;
    // Find the first nonzero bit
    while ((data & 1) == 0)
    {
        data >>= 1;
        location++;

        // Don't overflow
        if (location >= m_length)
            break;
    }
    return location;
}

bool cSetArray::isSetInOnePlace()
{
    bool found = false;

    for (uint i = 0 ; i < getLength() ; ++i)
    {
        if (isSet(i) && found)
            return false;
        else if (isSet(i))
            found = true;
    }
    return found;
}

void cSetArray::set(uint location)
{
    CHECK(location < m_length);

    uint arrayIndex, bitIndex;
    calculatePosition(location, arrayIndex, bitIndex);

    m_data[arrayIndex]|= (1 << bitIndex);
}

void cSetArray::clear(uint location)
{
    CHECK(location < m_length);

    uint arrayIndex, bitIndex;
    calculatePosition(location, arrayIndex, bitIndex);

    m_data[arrayIndex] = m_data[arrayIndex] & (~(1 << bitIndex));
}

void cSetArray::changeSize(uint newSize,
                           bool saveContent)
{
    uint arraySize = ((newSize + BITS_COUNT_PER_ELEMENT - 1))
                             >> BITS_COUNT_PER_ELEMENT_SHIFT;
    m_data.changeSize(arraySize, saveContent);
    m_length = newSize;
}

void cSetArray::resetArray()
{
    if (m_data.getSize() > 0)
        memset(m_data.getBuffer(), 0,
               m_data.getSize() << BYTES_PER_ELEMENT_SHIFT);
}

void cSetArray::calculatePosition(uint location,
                                  uint& arrayIndex,
                                  uint& bitIndex)
{
    arrayIndex = location >> BITS_COUNT_PER_ELEMENT_SHIFT;
    bitIndex = location - (arrayIndex << BITS_COUNT_PER_ELEMENT_SHIFT);
}

cSetArray* cSetArray::operator&=(const cSetArray& other)
{
    CHECK(getLength() == other.getLength());

    for (uint i = 0 ; i < getLength() ; ++i)
    {
        if ((isSet(i)) && (other.isSet(i)))
        {
            set(i);
        }
        else {
            clear(i);
        }
    }
    return this;
}

cSetArray* cSetArray::operator|=(const cSetArray& other)
{
    CHECK(getLength() == other.getLength());

    for (uint i = 0 ; i < getLength() ; ++i)
    {
        if ((isSet(i)) || (other.isSet(i)))
        {
            set(i);
        }
        else {
            clear(i);
        }
    }
    return this;
}