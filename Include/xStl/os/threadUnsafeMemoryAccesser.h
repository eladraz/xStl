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

#ifndef __TBA_STL_OS_THREADUNSAFEMEMORYACCESSER_H
#define __TBA_STL_OS_THREADUNSAFEMEMORYACCESSER_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file threadUnsafeMemoryAccesser.h
 * @brief Access virtual memory on the current thread on this machine.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/endian.h"
#include "xStl/data/smartptr.h"
#include "xStl/os/fragmentsDescriptor.h"
#include "xStl/os/virtualMemoryAccesser.h"

/**
 * @brief Write/read to/from the current virtual memory map.
 *
 * @note This class doesn't verify pointer validation. Assumes all calls must
 *    succeed.
 */
class cThreadUnsafeMemoryAccesser : public cVirtualMemoryAccesser {
public:
    /**
     * @brief Use the default implementation
     */
    defaultEndianImpl;

    /**
     * @brief Default constructor. Allow the application to block unneeded
     *    access to
     *   different memory blocks.
     *
     * @details When different values are supplied for 'start'..'end' than the
     *    memory location, 0 will be the 'start' and 'end-start' will be the end
     *    memory.
     *
     * An exception will be thrown for any out-of-range access.
     *
     * @param startAddress The starting physical address that can be accessed
     * @param endAddress The ending physical address that can be accessed
     */
    cThreadUnsafeMemoryAccesser(addressNumericValue startAddress = 0,
        addressNumericValue endAddress = MAX_PHYSICAL_ADDRESS);
    /**
     * @brief Change the range [start..end] to [virtual..virtual+size], however
     *    when
     *   accessing 'virtual', the address will be translated to 'start'
     *
     * @see cThreadUnsafeMemoryAccesser::cThreadUnsafeMemoryAccesser
     * @param startAddress The starting physical address that can be accessed.
     * @param endAddress The ending physical address that can be accessed.
     * @param virtualBase The virtual address which is translated to @a
     *    startAddress.
     */
    cThreadUnsafeMemoryAccesser(addressNumericValue startAddress,
        addressNumericValue endAddress,
        addressNumericValue virtualBase);

    /**
     * @brief Normal implementation. See VirtualMemoryAccesser::memread Ignore
     *    any
     *   fragmentation.
     *
     * @param address The address to read from.
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @param fragments Ignored.
     * @return True if the memory was read.
     */
    virtual bool memread(addressNumericValue address,
                         void* buffer,
                         uint length,
                         cFragmentsDescriptor* fragments = NULL) const;

    /**
     * @brief Normal implementation. See VirtualMemoryAccesser::write Use memcpy
     *    in
     *   order to transfer the memory
     *
     * @param address The address to write to.
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return True if the memory was written.
     */
    virtual bool write(addressNumericValue address,
                       const void* buffer,
                       uint length);

    /**
     * @brief Normal implementation. See
     *    VirtualMemoryAccesser::isWritableInterface
     *   Return true.
     *
     * @return True. The interface is writable.
     */
    virtual bool isWritableInterface() const;

protected:
    /**
     * @brief Perform relocation over 'address'.
     *
     * @param address The address to relocate.
     * @return The relocated address.
     * @throws cException if the memory address is outside the context
     */
    addressNumericValue getAddress(addressNumericValue address) const;

    /**
     * @brief The starting physical address that can be accessed
     */
    addressNumericValue m_startAddress;
    /**
     * @brief The ending physical address that can be accessed
     */
    addressNumericValue m_endAddress;
    /**
     * @brief The virtual name of 'startAddress'
     */
    bool m_isVirtualRelocation;
    /**
     * @brief The virtual address which is translated to the starting address.
     */
    addressNumericValue m_virtualBase;
};


/** @} */
#endif // __TBA_STL_OS_THREADUNSAFEMEMORYACCESSER_H
