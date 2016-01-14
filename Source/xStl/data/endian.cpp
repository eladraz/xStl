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
#include "xStl/types.h"
#include "xStl/data/endian.h"

/*
 * Little endian implementation
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#ifndef XSTL_16BIT
uint64 cEndian::readUint64(const uint8* buffer, bool isLittleEndian)
{
    if (isLittleEndian)
    {
        return cLittleEndian::readUint64(buffer);
    }
    return cBigEndian::readUint64(buffer);
}

void cEndian::writeUint64(uint8* buffer, uint64 number, bool isLittleEndian)
{
    if (isLittleEndian)
    {
        return cLittleEndian::writeUint64(buffer, number);
    }
    return cBigEndian::writeUint64(buffer, number);
}
#endif // XSTL_16BIT

uint32 cEndian::readUint32(const uint8* buffer, bool isLittleEndian)
{
    if (isLittleEndian)
    {
        return cLittleEndian::readUint32(buffer);
    }
    return cBigEndian::readUint32(buffer);
}

void cEndian::writeUint32(uint8* buffer, uint32 number, bool isLittleEndian)
{
    if (isLittleEndian)
    {
        return cLittleEndian::writeUint32(buffer, number);
    }
    return cBigEndian::writeUint32(buffer, number);
}

uint16 cEndian::readUint16(const uint8* buffer, bool isLittleEndian)
{
    if (isLittleEndian)
    {
        return cLittleEndian::readUint16(buffer);
    }
    return cBigEndian::readUint16(buffer);
}

void cEndian::writeUint16(uint8* buffer, uint16 number, bool isLittleEndian)
{
    if (isLittleEndian)
    {
        return cLittleEndian::writeUint16(buffer, number);
    }
    return cBigEndian::writeUint16(buffer, number);
}

#ifndef XSTL_16BIT
uint64 cLittleEndian::readUint64(const uint8* buffer)
{
    return ((uint64)buffer[7] << 56) |
           ((uint64)buffer[6] << 48) |
           ((uint64)buffer[5] << 40) |
           ((uint64)buffer[4] << 32) |
           ((uint64)buffer[3] << 24) |
           ((uint64)buffer[2] << 16) |
           ((uint64)buffer[1] <<  8) |
           ((uint64)buffer[0]);
}

void cLittleEndian::writeUint64(uint8* buffer, uint64 number)
{
    buffer[7] = (uint8)(((number) >> 56) & 0xff);
    buffer[6] = (uint8)(((number) >> 48) & 0xff);
    buffer[5] = (uint8)(((number) >> 40) & 0xff);
    buffer[4] = (uint8)(((number) >> 32) & 0xff);
    buffer[3] = (uint8)(((number) >> 24) & 0xff);
    buffer[2] = (uint8)(((number) >> 16) & 0xff);
    buffer[1] = (uint8)(((number) >> 8) & 0xff);
    buffer[0] = (uint8)((number) & 0xff);
}
#endif // XSTL_16BIT

uint32 cLittleEndian::readUint32(const uint8* buffer)
{
    return (buffer[3] << 24) |
           (buffer[2] << 16) |
           (buffer[1] <<  8) |
           (buffer[0]);
}

void cLittleEndian::writeUint32(uint8* buffer, uint32 number)
{
    buffer[3] = (uint8)(((number) >> 24) & 0xff);
    buffer[2] = (uint8)(((number) >> 16) & 0xff);
    buffer[1] = (uint8)(((number) >> 8) & 0xff);
    buffer[0] = (uint8)((number) & 0xff);
}


uint16 cLittleEndian::readUint16(const uint8* buffer)
{
    return (buffer[1] <<  8) |
           (buffer[0]);
}

void cLittleEndian::writeUint16(uint8* buffer, uint16 number)
{
    buffer[1] = (uint8)(((number) >> 8) & 0xff);
    buffer[0] = (uint8)((number) & 0xff);
}


#ifndef XSTL_16BIT
/*
 * Big endian implementation
 */
uint64 cBigEndian::readUint64(const uint8* buffer)
{
    return ((uint64)buffer[0] << 56) |
           ((uint64)buffer[1] << 48) |
           ((uint64)buffer[2] << 40) |
           ((uint64)buffer[3] << 32) |
           ((uint64)buffer[4] << 24) |
           ((uint64)buffer[5] << 16) |
           ((uint64)buffer[6] <<  8) |
           ((uint64)buffer[7]);
}

void cBigEndian::writeUint64(uint8* buffer, uint64 number)
{
    buffer[0] = (uint8)(((number) >> 56) & 0xff);
    buffer[1] = (uint8)(((number) >> 48) & 0xff);
    buffer[2] = (uint8)(((number) >> 40) & 0xff);
    buffer[3] = (uint8)(((number) >> 32) & 0xff);
    buffer[4] = (uint8)(((number) >> 24) & 0xff);
    buffer[5] = (uint8)(((number) >> 16) & 0xff);
    buffer[6] = (uint8)(((number) >> 8) & 0xff);
    buffer[7] = (uint8)((number) & 0xff);
}
#endif // XSTL_16BIT

uint32 cBigEndian::readUint32(const uint8* buffer)
{
    return (buffer[0] << 24) |
           (buffer[1] << 16) |
           (buffer[2] <<  8) |
           (buffer[3]);
}

void cBigEndian::writeUint32(uint8* buffer, uint32 number)
{
    buffer[0] = (uint8)(((number) >> 24) & 0xff);
    buffer[1] = (uint8)(((number) >> 16) & 0xff);
    buffer[2] = (uint8)(((number) >> 8) & 0xff);
    buffer[3] = (uint8)((number) & 0xff);
}


uint16 cBigEndian::readUint16(const uint8* buffer)
{
    return (buffer[0] <<  8) |
           (buffer[1]);
}

void cBigEndian::writeUint16(uint8* buffer, uint16 number)
{
    buffer[0] = (uint8)(((number) >> 8) & 0xff);
    buffer[1] = (uint8)((number) & 0xff);
}
