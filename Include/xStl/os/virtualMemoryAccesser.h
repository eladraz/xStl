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

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file virtualMemoryAccesser.h
 * @brief Analyzer classes need to access the complete virtual memory mapping of
 *    an
 *   object.
 *
 * @details The problem is that most of the times the virtual format sits inside
 *    a page-file or a file or other process memory. This interface allows
 *    accessing virtual memory without knowing any information.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/endian.h"
#include "xStl/data/smartptr.h"
#include "xStl/os/fragmentsDescriptor.h"

/**
 * @brief Interface for accessing virtual memory-map.
 *
 * @details The interface allows accessing the bus with default bit size
 *    operations. The interface is used by the stack analyzer, so only default
 *    stack bit size (=uint) operations are allowed.
 */
class cVirtualMemoryAccesser : public cEndian {
public:
    // Memory accessor

    /**
     * @brief Read a block of memory.
     *
     * @details The memory can be fragmented.
     *
     * The function shouldn't throw any exception. However, an implementation of
     * this interface may throw an exception. See cThreadUnsafeMemoryAccesser.
     *
     * @param address The virtual address to be read
     * @param buffer Will be filled with the reading memory.
     * @param length The length for the read operation.
     * @param fragments If the memory is fragmented, will be filled with the
     *    description of the fragment blocks. If this argument is NULL than the
     *    fragmentation information will not be generated.
     * @return True if the entire memory was read. False if one or more bytes in
     *    the memory are missing and it is fragmented. In this case the
     *    'fragments' argument will be filled with which parts of the data are
     *    valid and which parts aren't.
     * @note This function is a synchronized operation.
     */
    virtual bool memread(addressNumericValue address,
                         void* buffer,
                         uint length,
                         cFragmentsDescriptor* fragments = NULL) const = 0;

    /**
     * @brief Performs a write to the virtual memory.
     *
     * @details This memory
     *
     * @param address The virtual address to be written to
     * @param buffer The data to write
     * @param length The length in bytes of 'buffer'
     * @return True if the memory was written completely.
     * @throws cException if the interface is a read-only interface.
     */
    virtual bool write(addressNumericValue address,
                       const void* buffer,
                       uint length) = 0;

    /**
     * @brief Return true if the interface supports the writable functions.
     *
     * @return True if the interface supports the writable functions.
     */
    virtual bool isWritableInterface() const = 0;

    // Numeric accessor. Implemented by a call to 'memread' and 'write'

    /**
     * @brief Deserialize uint (Stack sized variable) from virtual memory map.
     *
     * @details This function cannot fail. If the memory doesn't exist, this
     *   function throws an exception.
     *
     * @param address The location inside the virtual process memory.
     * @return The value read from the memory.
     */
    uint readUint(addressNumericValue address) const;

    /**
     * @brief Serialize a uint to the virtual memory map.
     *
     * @details Some platforms don't support the writable interface. In this
     *    case,
     *   the function may throw a FAILED exception. See isWritableInterface()
     *
     * This function cannot fail. If the memory doesn't exist, this
     * function throws an exception.
     *
     * @param address The location inside the virtual process memory.
     * @param value The data to be written to.
     */
    void writeUint(addressNumericValue address,
                   uint value);

    /**
     * @brief Const byte-read implementation of operator [] over memory address
     *
     * @param address The address of the byte.
     * @return The byte at @a address.
     */
    uint8 operator [] (addressNumericValue address) const;
};

/**
 * @brief The reference-countable object over the memory-accesser.
 */
typedef cSmartPtr<cVirtualMemoryAccesser> cVirtualMemoryAccesserPtr;


/** @} */
#endif // __TBA_STL_OS_VIRTUALMEMORYACCESSER_H
