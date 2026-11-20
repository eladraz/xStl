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

#ifndef __TBA_STL_OS_VIRTUALMEMORY_H
#define __TBA_STL_OS_VIRTUALMEMORY_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file virtualMemory.h
 * @brief Reserves a contiguous range of virtual address space.
 *
 * @details The range is reserved and committed read-write. Pages are
 *    zero-filled and physical memory is only used once a page is touched, so
 *    a large reservation is cheap. Individual pages can later be made
 *    inaccessible, for example to create guard areas that trap on access.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"

/**
 * @brief A reserved, read-write region of virtual memory.
 *
 * @details The region is released when the object is destroyed. The object
 *    cannot be copied. Sizes are rounded up to whole pages.
 */
class cVirtualMemory {
public:
    /**
     * @brief Reserve and commit a read-write region of at least @a size bytes.
     *
     * @param size The requested size in bytes. Rounded up to a page multiple.
     * @throws cException if @a size is zero or the reservation failed.
     */
    explicit cVirtualMemory(uint64 size);

    /**
     * @brief Release the whole region.
     */
    ~cVirtualMemory();

    /**
     * @brief Return the first byte of the region.
     *
     * @return Pointer to the first byte, aligned to the page size.
     */
    uint8* getBase() const;

    /**
     * @brief Return the size of the region in bytes (a page multiple).
     *
     * @return The size of the region.
     */
    uint64 getSize() const;

    /**
     * @brief Make a page-aligned range of the region inaccessible.
     *
     * @details Any read or write of the range raises an access violation.
     *    The range stays reserved. This class has no call to make it
     *    accessible again.
     *
     * @param offset The offset from getBase(). Must be a page multiple.
     * @param length The length in bytes. Must be a page multiple and non-zero.
     * @throws cException if the range is not page-aligned, is empty, or does
     *    not lie inside the region.
     */
    void protectNoAccess(uint64 offset, uint64 length);

    /**
     * @brief Set the protection of a page-aligned range of the region.
     *
     * @details The range becomes no-access (neither flag), read-only (@a read
     *    only), or read-write (@a write, which implies read). Execute is never
     *    granted. Unlike protectNoAccess(), this can make a range accessible
     *    again, so a guard page can be reused.
     *
     * @param offset The offset from getBase(). Must be a page multiple.
     * @param length The length in bytes. Must be a page multiple and non-zero.
     * @param read Allow reads.
     * @param write Allow writes (read-write when set, regardless of @a read).
     * @throws cException if the range is not page-aligned, is empty, or does
     *    not lie inside the region, or if the protection change failed.
     */
    void protect(uint64 offset, uint64 length, bool read, bool write);

    /**
     * @brief Map a file over a page-aligned range of the region (MAP_FIXED).
     *
     * @details The host file descriptor @a fd is mapped at the range, so the
     *    guest reads the file bytes starting at @a fileOffset. A private
     *    mapping (@a shared false) is copy-on-write: writes stay in memory and
     *    the file is not changed. A shared mapping makes writes visible to the
     *    file through the page cache (no msync needed for other readers on
     *    Linux). Bytes past the end of the file fault on access, as on the host.
     *    Unix only.
     *
     * @param offset The offset from getBase(). Must be a page multiple.
     * @param length The length in bytes. Must be a page multiple and non-zero.
     * @param fd An open host file descriptor.
     * @param fileOffset The offset in the file. Must be a page multiple.
     * @param shared True for MAP_SHARED, false for MAP_PRIVATE.
     * @param read Allow reads.
     * @param write Allow writes.
     * @return 0 on success, or -errno when the host refused the mapping
     *    (for example a bad descriptor or a misaligned file offset).
     * @throws cException if the range is not page-aligned, is empty, or does
     *    not lie inside the region, or on Windows.
     */
    int mapFile(uint64 offset,
                uint64 length,
                int fd,
                uint64 fileOffset,
                bool shared,
                bool read,
                bool write);

    /**
     * @brief Replace a page-aligned range with a zero-filled, shared mapping.
     *
     * @details The range becomes an anonymous MAP_SHARED mapping. After a
     *    fork() the parent and the child see each other's writes to it. The
     *    range stays read-write. Unix only.
     *
     * @param offset The offset from getBase(). Must be a page multiple.
     * @param length The length in bytes. Must be a page multiple and non-zero.
     * @throws cException if the range is not page-aligned, is empty, or does
     *    not lie inside the region, if the mapping failed, or on Windows.
     */
    void mapShared(uint64 offset, uint64 length);

    /**
     * @brief Replace a page-aligned range with fresh, zero-filled private pages.
     *
     * @details Undoes the previous contents of the range, including a shared
     *    mapping made by mapShared(). The range stays read-write. Unix only.
     *
     * @param offset The offset from getBase(). Must be a page multiple.
     * @param length The length in bytes. Must be a page multiple and non-zero.
     * @throws cException if the range is not page-aligned, is empty, or does
     *    not lie inside the region, if the mapping failed, or on Windows.
     */
    void resetPrivate(uint64 offset, uint64 length);

    /**
     * @brief Return the size of a memory page on this system.
     *
     * @return The page size in bytes.
     */
    static uint64 getPageSize();

private:
    // Non-copyable
    cVirtualMemory(const cVirtualMemory&);
    cVirtualMemory& operator=(const cVirtualMemory&);

    /**
     * @brief Validate a page-aligned range of the region.
     *
     * @throws cException if the range is not page-aligned, is empty, or does
     *    not lie inside the region.
     */
    void checkRange(uint64 offset, uint64 length) const
    {
        const uint64 pageSize = getPageSize();

        if ((offset % pageSize) != 0 || (length % pageSize) != 0 || length == 0)
        {
            XSTL_THROW(cException, EXCEPTION_FAILED);
        }
        if (offset > m_size || length > m_size - offset)
        {
            XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
        }
    }

    uint8* m_base;  ///< First byte of the reserved region
    uint64 m_size;  ///< Size of the region in bytes (page multiple)
};

/** @} */
#endif // __TBA_STL_OS_VIRTUALMEMORY_H
