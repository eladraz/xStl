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

#ifndef __TBA_STL_OS_FRAGMENTSDESCRIPTOR_H
#define __TBA_STL_OS_FRAGMENTSDESCRIPTOR_H

/*
 * fragmentsDescriptor.h
 *
 * Defines a fragment memory hole. A fragment memory is a block of memory which+
 * combines which valid and invalid ("Holes") regions.
 * See cVirtualMemoryAccesser for more information.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/list.h"
#include "xStl/data/serializedObject.h"
#include "xStl/stream/basicIO.h"


/*
 * Defines a fragment memory hole. A fragment memory is a block of memory which+
 * combines which valid and invalid ("Holes") regions.
 * See cVirtualMemoryAccesser for more information.
 *
 * NOTE: Encoding of more than 2^16 fragmentations will cause an exception
 *       during serialization...
 */
class cFragmentsDescriptor : public cSerializedObject {
public:
    /*
     * A memory range have a starting point and an end point
     */
    class MemoryRange : public cSerializedObject {
    public:
        // Default constructor
        MemoryRange(const remoteAddressNumericValue& start,
                    const remoteAddressNumericValue& end);
        // Copy-constructor and operator = will auto-generated by the compiler
        // The start of the range
        remoteAddressNumericValue m_start;
        // The end of the range
        remoteAddressNumericValue m_end;

        // See cSerializedObject::isValid
        virtual bool isValid() const;
        // See cSerializedObject::deserialize
        virtual void deserialize(basicInput& inputStream);
        // See cSerializedObject::serialize
        virtual void serialize(basicOutput& outputStream) const;
    };
    // The complete fragmentation list
    typedef cList<MemoryRange> MemoryRangeFragmentation;

    /*
     * Default constructor. Assumes all memory is valid
     */
    cFragmentsDescriptor();

    /*
     * Return true if the memory byte 'address' is valid.
     */
    bool isMemoryValid(const remoteAddressNumericValue& address) const;

    /*
     * Append new INVALID block to the memory table.
     *
     * start - The start address block to exclude
     * end - The end address block to exclude
     */
    void appendFragmentBlock(const remoteAddressNumericValue& start,
                             const remoteAddressNumericValue& end);

    /*
     * Return The fragmentation content of the memory block
     */
    const MemoryRangeFragmentation& getFragmentation() const;

    // See cSerializedObject::isValid
    virtual bool isValid() const;
    // See cSerializedObject::deserialize
    virtual void deserialize(basicInput& inputStream);
    // See cSerializedObject::serialize
    virtual void serialize(basicOutput& outputStream) const;

private:
    // Deny copy-constructor and operator =. These are very expansive operation.
    cFragmentsDescriptor(const cFragmentsDescriptor& other);
    cFragmentsDescriptor& operator = (const cFragmentsDescriptor& other);

    // The memory fragment descriptor
    MemoryRangeFragmentation m_fragments;
};

#endif // __TBA_STL_OS_FRAGMENTSDESCRIPTOR_H
