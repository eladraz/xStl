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

/*
 * unixVirtualMemory.cpp
 *
 * UNIX implementation of cVirtualMemory, based on mmap/mprotect.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
// System headers come first: xStl/types.h defines the 'uint' macro, which
// breaks the system headers that are included after it.
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/data/string.h"
#include "xStl/os/os.h"
#include "xStl/os/virtualMemory.h"

#ifndef MAP_NORESERVE
    // Not every UNIX has the flag. Without it the kernel may reserve swap
    // space for the whole mapping, which only matters for huge mappings.
    #define MAP_NORESERVE 0
#endif

#ifndef MAP_ANONYMOUS
    #define MAP_ANONYMOUS MAP_ANON
#endif


cVirtualMemory::cVirtualMemory(uint64 size) :
    m_base(NULL),
    m_size(0)
{
    const uint64 pageSize = getPageSize();
    if (size == 0)
    {
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    }

    // Round up to a page multiple without overflowing
    if (size > ((uint64)-1) - (pageSize - 1))
    {
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    }
    const uint64 rounded = ((size + pageSize - 1) / pageSize) * pageSize;

    // Anonymous mappings are zero-filled, and their pages are only backed by
    // physical memory once they are touched.
    void* mem = mmap(NULL,
                     (size_t)rounded,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                     -1,
                     0);
    if (mem == MAP_FAILED)
    {
        cString msg = cOS::getLastErrorString();
        XSTL_THROW(cException, msg.getBuffer());
    }

    m_base = (uint8*)mem;
    m_size = rounded;
}

cVirtualMemory::~cVirtualMemory()
{
    if (m_base != NULL)
        munmap(m_base, (size_t)m_size);
}

uint8* cVirtualMemory::getBase() const
{
    return m_base;
}

uint64 cVirtualMemory::getSize() const
{
    return m_size;
}

void cVirtualMemory::protectNoAccess(uint64 offset, uint64 length)
{
    checkRange(offset, length);

    if (mprotect(m_base + offset, (size_t)length, PROT_NONE) != 0)
    {
        cString msg = cOS::getLastErrorString();
        XSTL_THROW(cException, msg.getBuffer());
    }
}

// Map the rwx request onto host protection flags. Execute is never granted.
// PROT_WRITE implies PROT_READ, which the host requires for writable pages.
static int getHostProtection(bool read, bool write)
{
    if (write)
        return PROT_READ | PROT_WRITE;
    if (read)
        return PROT_READ;
    return PROT_NONE;
}

void cVirtualMemory::protect(uint64 offset, uint64 length, bool read, bool write)
{
    checkRange(offset, length);

    if (mprotect(m_base + offset, (size_t)length, getHostProtection(read, write)) != 0)
    {
        cString msg = cOS::getLastErrorString();
        XSTL_THROW(cException, msg.getBuffer());
    }
}

int cVirtualMemory::mapFile(uint64 offset,
                            uint64 length,
                            int fd,
                            uint64 fileOffset,
                            bool shared,
                            bool read,
                            bool write)
{
    checkRange(offset, length);

    // MAP_FIXED replaces the reserved pages in place. The file pages are
    // mapped on the host, so the guest sees the file bytes directly.
    void* mem = mmap(m_base + offset,
                     (size_t)length,
                     getHostProtection(read, write),
                     MAP_FIXED | (shared ? MAP_SHARED : MAP_PRIVATE),
                     fd,
                     (off_t)fileOffset);
    if (mem == MAP_FAILED)
    {
        // Report the host error to the caller instead of throwing
        return -errno;
    }
    return 0;
}

void cVirtualMemory::mapShared(uint64 offset, uint64 length)
{
    checkRange(offset, length);

    // MAP_FIXED replaces the pages of the reservation in place. The new
    // anonymous MAP_SHARED pages survive fork(), so parent and child share them.
    void* mem = mmap(m_base + offset,
                     (size_t)length,
                     PROT_READ | PROT_WRITE,
                     MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS,
                     -1,
                     0);
    if (mem == MAP_FAILED)
    {
        cString msg = cOS::getLastErrorString();
        XSTL_THROW(cException, msg.getBuffer());
    }
}

void cVirtualMemory::resetPrivate(uint64 offset, uint64 length)
{
    checkRange(offset, length);

    // A fresh private mapping is zero-filled and drops any shared pages
    void* mem = mmap(m_base + offset,
                     (size_t)length,
                     PROT_READ | PROT_WRITE,
                     MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                     -1,
                     0);
    if (mem == MAP_FAILED)
    {
        cString msg = cOS::getLastErrorString();
        XSTL_THROW(cException, msg.getBuffer());
    }
}

uint64 cVirtualMemory::getPageSize()
{
    return (uint64)sysconf(_SC_PAGESIZE);
}
