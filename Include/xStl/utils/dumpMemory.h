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

#ifndef __TBA_XSTL_UTILS_DUMPMEMORY_H
#define __TBA_XSTL_UTILS_DUMPMEMORY_H

/**
 * @addtogroup xstl_data_dump
 * @{
 */

/**
 * @file dumpMemory.h
 * @brief A module which translates a cVirtualMemoryAccesser into a human
 *    readable
 *   memory dump.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/os/fragmentsDescriptor.h"
#include "xStl/os/virtualMemoryAccesser.h"
#include "xStl/stream/stringerStream.h"

/*
 * The out operator << forward declaration.
 * See class DumpMemory
 */
class DumpMemory;
/**
 * @brief Print a memory dump to a stringer stream.
 *
 * @param out The stream to print to.
 * @param memory The memory dump object.
 * @return The stream @a out.
 */
cStringerStream& operator << (cStringerStream& out,
                              const DumpMemory& memory);


/**
 * @brief Dump a range of memory in a human readable (hexadecimal) format.
 *
 * @details Usage:
 *
 * @code
 * cout << DumpMemory(*local, 0, 100) << endl;
 * @endcode
 */
class DumpMemory {
public:
    /**
     * @brief The default number of elements in a single output line.
     */
    enum {
        /**
         * @brief The default number of elements in a single output line.
         */
        DEFAULT_NUMBER_OF_ELEMENTS_IN_LINE = 16
    };

    /**
     * @brief Default constructor. Setup the parameters to be used.
     *
     * @details See class intro.
     *
     * @param memory The object to be dumped
     * @param startAddress The starting address range for the dump
     * @param endAddress The ending address range for the dump
     * @param relocation The relative addressing for the display.
     * @param elementsCount The number of elements in a single line
     */
    DumpMemory(const cVirtualMemoryAccesser& memory,
               addressNumericValue startAddress,
               addressNumericValue endAddress,
               addressNumericValue relocation = 0,
               uint elementsCount = DEFAULT_NUMBER_OF_ELEMENTS_IN_LINE);

    /**
     * @brief Default constructor for pre-read memory.
     *
     * @details Setup the parameters to be used. See class intro.
     *
     * addressType  - The type of the remote-address
     *
     * @param buffer The read buffer
     * @param fragments The fragmentation of the same block
     * @param startAddress The address of the buffer
     * @param elementsCount The number of elements in a single line
     */
    DumpMemory(const cBuffer& buffer,
               const cFragmentsDescriptor& fragments,
               const remoteAddressNumericValue& startAddress,
               uint elementsCount = DEFAULT_NUMBER_OF_ELEMENTS_IN_LINE);

private:
    /**
     * @brief Deny operator = and copy-constructor
     */
    DumpMemory(const DumpMemory& other);
    DumpMemory& operator = (const DumpMemory& other);

    // The operator << is the reason for this class's existence.
    friend cStringerStream& operator << (cStringerStream& out,
                                         const DumpMemory& memory);

    /**
     * @brief This function dumps a single line to the output stream.
     *
     * @details The format of the trace is as follows: ADDRESS DATA TEXT
     *    00000000 496E 7374 616C 6C20-4661 696C 6564 0D0A Install Failed.. If
     *    the data stops before 'lineCount' then the format is 00000040 726E
     *    616C 2065 7272-6F72 2E rnal error.
     *
     * @param pos The memory address to read the data
     * @param count The number of elements to read
     * @param data A buffer with at least 'count' elements. Used for
     *    optimization. Avoids allocating and freeing the same buffer size over
     *    and over again
     * @param out The stream to write the data to
     */
    void dumpLine(addressNumericValue pos,
                  uint count,
                  cBuffer& data,
                  cStringerStream& out) const;

    /**
     * @see dumpLine
     */
    void internalDumpLine(const remoteAddressNumericValue& pos,
                          uint count,
                          const uint8* data,
                          const cFragmentsDescriptor& fragments,
                          cStringerStream& out) const;

    /**
     * @brief Set to true to indicate which constructor is called
     */
    bool m_shouldReadMemory;
    /**
     * @brief The number of elements in a line
     */
    uint m_elementsCount;

    /**
     * @brief The starting address range
     */
    addressNumericValue m_startAddress;
    /**
     * @brief The ending address range
     */
    addressNumericValue m_endAddress;
    /**
     * @brief The relocation information
     */
    addressNumericValue m_relocation;
    /**
     * @brief The memory object
     */
    const cVirtualMemoryAccesser* m_memory;

    /**
     * @brief The buffer content
     */
    const cBuffer* m_buffer;
    /**
     * @brief The buffer fragments descriptors
     */
    const cFragmentsDescriptor* m_fragments;
    /**
     * @brief The remote start address
     */
    remoteAddressNumericValue m_remoteAddress;
};


/** @} */
#endif // __TBA_XSTL_UTILS_DUMPMEMORY_H
