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
// For this files, the WIN32 or the _WIN32_WCE macro must be defined.
// Only Windows operating system or Windows CE can compile with this API
#if defined(WIN32) || defined(_WIN32_WCE)

/*
 * winVirtualMemory.cpp
 *
 * Windows implementation of cVirtualMemory, based on VirtualAlloc and
 * VirtualProtect.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/os.h"
#include "xStl/os/osdef.h"
#include "xStl/data/char.h"
#include "xStl/os/virtualMemory.h"

#include <windows.h>


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

    // Committed pages are zero-filled and backed by physical memory on first
    // touch, so the reservation itself is cheap.
    LPVOID mem = VirtualAlloc(NULL,
                              (SIZE_T)rounded,
                              MEM_RESERVE | MEM_COMMIT,
                              PAGE_READWRITE);
    if (mem == NULL)
    {
        cString msg = cOS::getLastErrorString();
        XSTL_THROW(cException, msg.getBuffer());
    }

    m_base = (uint8*)mem;
    m_size = rounded;
}

cVirtualMemory::~cVirtualMemory()
{
    // MEM_RELEASE requires the base address of the whole reservation and a
    // size of zero.
    if (m_base != NULL)
        VirtualFree(m_base, 0, MEM_RELEASE);
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
    const uint64 pageSize = getPageSize();

    // Page-aligned, non-empty, and entirely inside the region
    if ((offset % pageSize) != 0 || (length % pageSize) != 0 || length == 0)
    {
        XSTL_THROW(cException, EXCEPTION_FAILED);
    }
    if (offset > m_size || length > m_size - offset)
    {
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    }

    DWORD oldProtect = 0;
    if (!VirtualProtect(m_base + offset, (SIZE_T)length, PAGE_NOACCESS, &oldProtect))
    {
        cString msg = cOS::getLastErrorString();
        XSTL_THROW(cException, msg.getBuffer());
    }
}

void cVirtualMemory::protect(uint64 offset, uint64 length, bool read, bool write)
{
    checkRange(offset, length);

    // Execute is never granted. Write implies read.
    DWORD newProtect = PAGE_NOACCESS;
    if (write)
        newProtect = PAGE_READWRITE;
    else if (read)
        newProtect = PAGE_READONLY;

    DWORD oldProtect = 0;
    if (!VirtualProtect(m_base + offset, (SIZE_T)length, newProtect, &oldProtect))
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
    // File-backed MAP_FIXED mappings exist only on UNIX
    checkRange(offset, length);
    XSTL_THROW(cException, XSTL_STRING("mapFile is not supported on Windows"));
}

void cVirtualMemory::mapShared(uint64 offset, uint64 length)
{
    // Shared anonymous mappings across fork() exist only on UNIX
    checkRange(offset, length);
    XSTL_THROW(cException, XSTL_STRING("mapShared is not supported on Windows"));
}

void cVirtualMemory::resetPrivate(uint64 offset, uint64 length)
{
    // Only UNIX has the shared mapping that this call undoes
    checkRange(offset, length);
    XSTL_THROW(cException, XSTL_STRING("resetPrivate is not supported on Windows"));
}

uint64 cVirtualMemory::getPageSize()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (uint64)info.dwPageSize;
}

#endif // WIN32
