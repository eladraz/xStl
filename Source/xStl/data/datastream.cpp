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
 * datastream.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"

const character* hexUpperStrip = XSTL_STRING("0123456789ABCDEF");


uint8 DB(const uint8& ch,
         const uint8 nonPrintable)
{
    if ((ch < 0x20) || (ch > 0x7F))
    {
        return nonPrintable;
    }
    return ch;
}

character HEXHIGH(const uint8& ch)
{
    return hexUpperStrip[(ch >> 4)];
}

character HEXLOW(const uint8& ch)
{
    return hexUpperStrip[(ch & 0x0F)];
}

cString HEXBYTE(const uint8 &ch)
{
    return cString(hexUpperStrip[(ch >> 4)]) + cString(hexUpperStrip[(ch & 0x0F)]);
}
cString HEXWORD(const uint16 &word)
{
    return HEXBYTE((uint8)(word >> 8)) + HEXBYTE((uint8)(word & 0xFF));
}
cString HEXDWORD(const uint32 &dword)
{
    return HEXWORD((uint16)(dword >> 16)) + HEXWORD((uint16)(dword & 0xFFFF));
}

#ifndef XSTL_16BIT
cString HEXQWORD(const uint64& qword)
{
    return HEXDWORD((uint32)(qword >> 32)) +
           HEXDWORD((uint32)(qword & 0xFFFFFFFF));
}
#endif // XSTL_16BIT

cString HEXNUMBER(uint number)
{
    if (number < 0x100)
        return HEXBYTE((uint8)number);

    if (number < 0x10000)
        return HEXWORD((uint16)number);

    // TODO! 64bit

    return HEXDWORD((uint32)number);
}

cString HEXADDRESS(const addressNumericValue& address)
{
    #ifdef XSTL_16BIT
        // TODO!
        // Seperate segment:offset
        return HEXDWORD(address);
    #else
        // 32 bit applications
        return HEXREMOTEADDRESS(remoteAddressNumericValue(address));
    #endif
}

cString HEXREMOTEADDRESS(const remoteAddressNumericValue& address)
{
    switch (address.m_type)
    {
    case REMOTE_ADDRESS_16BIT: return HEXWORD(address.m_union.m_16bitAddress);
    case REMOTE_ADDRESS_32BIT: return HEXDWORD(address.m_union.m_32bitAddress);
    case REMOTE_ADDRESS_64BIT: return HEXQWORD(address.m_union.m_64bitAddress);
    default:
        ASSERT_FAIL(XSTL_STRING("HEXREMOTEADDRESS: Unknown remote-address type"));
        return "UNKNOWN";
    }
}
