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

#ifndef __TBA_STL_OS_VIRTUALMEMORYACCESSER_H
#define __TBA_STL_OS_VIRTUALMEMORYACCESSER_H

/*
 * virtualMemoryAccesser.h
 *
 * Analyzer classes needs to access to complete virtual memory mapping of an
 * object. The problem is that most of the times the virtual format sits inside
 * a page-file or a file or other process memory. This interface allow accessing
 * virtual memory without knowing any information.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/endian.h"
#include "xStl/data/smartptr.h"
#include "xStl/os/fragmentsDescriptor.h"

/*
 * Interface for accessing virtual memory-map. The interface allow accessing the
 * bus with default bit size operations. The interface is inused by the stack
 * analyzer, so only default stack bit size (=uint) operations are allowed.
 */
class cVirtualMemoryAccesser : public cEndian {
public:
    // Memory accessor

    /*
     * Read a block of memory. The memory can be fragment.
     * NOTE: This function is a synchronized operation.
     *
     * address   - The virtual address to be read
     * buffer    - Will be filled with the reading memory.
     * length    - The length for the read operation.
     * fragments - If the memory is fragmented, will be filled with the
     *             description of the fragment blocks.
     *             If this argument is NULL than the fragmentation information
     *             will not be generated.
     *
     * Return true if the entire memory was read. False is one or more bytes
     * in the memory are missing and being fragment. In this case the
     * 'fragments' argument will be filled with the which parts of the data is
     * valid and which parts aren't.
     *
     * The function shouldn't throw any exception. However implementation to
     * this interface may throw exception. See cThreadUnsafeMemoryAccesser.
     */
    virtual bool memread(addressNumericValue address,
                         void* buffer,
                         uint length,
                         cFragmentsDescriptor* fragments = NULL) const = 0;

    /*
     * Performs a write to the virtual memory. This memory
     *
     * address   - The virtual address to be written to
     * buffer    - The data to write
     * length    - The length in bytes of 'buffer'
     *
     * Throw exception if the interface is a read-only interface.
     * Return true if the memory was written completely.
     * Return false if one or more bytes within the memory-range couldn't be
     * written.
     */
    virtual bool write(addressNumericValue address,
                       const void* buffer,
                       uint length) = 0;

    /*
     * Return true if the interface supports the writables functions.
     */
    virtual bool isWritableInterface() const = 0;

    // Numeric accessor. Implements by a call to 'memread' and 'write'

    /*
     * Deserialize uint (Stack sized variable) from virtual memory map.
     *
     * address - The location inside the virtual process memory.
     *
     * This function cannot be failed. If the memory doesn't exist, this
     * function throw exception.
     */
    uint readUint(addressNumericValue address) const;

    /*
     * Serialize a uint to the virtual memory map.
     *
     * address - The location inside the virtual process memory.
     * value - The data to be written to.
     *
     * Some platform doesn't supports the writable interface. In this case,
     * the function may throw a FAILED exception. See isWritableInterface()
     *
     * This function cannot be failed. If the memory doesn't exist, this
     * function throw exception.
     */
    void writeUint(addressNumericValue address,
                   uint value);

    /*
     * Const byte-read implementation of operator [] over memory address
     */
    uint8 operator [] (addressNumericValue address) const;
};

// The reference-countable object over the memory-accesser.
typedef cSmartPtr<cVirtualMemoryAccesser> cVirtualMemoryAccesserPtr;

#endif // __TBA_STL_OS_VIRTUALMEMORYACCESSER_H
