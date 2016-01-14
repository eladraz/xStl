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
 * virtualMemoryAccesser.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/os/fragmentsDescriptor.h"
#include "xStl/os/virtualMemoryAccesser.h"

uint cVirtualMemoryAccesser::readUint(addressNumericValue address) const
{
    // If this change, than the implementation must be changed also
    ASSERT(sizeof(uint) == sizeof(uint32));

    uint8 buf[4];
    if (!memread(address, &buf, sizeof(buf), NULL))
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);

    return readUint32(buf);
}

void cVirtualMemoryAccesser::writeUint(addressNumericValue address,
                                       uint value)
{
    // If this change, than the implementation must be changed also
    ASSERT(sizeof(uint) == sizeof(uint32));

#if defined(XSTL_32BIT)
    uint8 buf[4];
    writeUint32(buf, (uint32)value);
#elif defined(XSTL_64BIT)
    uint8 buf[8];
    writeUint64(buf, (uint64)value);
#else
    #error Please add machine word support
#endif

    if (!write(address, buf, sizeof(buf)))
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
}

uint8 cVirtualMemoryAccesser::operator [] (addressNumericValue address) const
{
    uint8 ret;
    if (!memread(address, &ret, sizeof(ret), NULL))
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    return ret;
}

