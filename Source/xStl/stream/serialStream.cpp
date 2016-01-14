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
 * serialStream.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/serialPort.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/fileStream.h"
#include "xStl/stream/serialStream.h"

#ifndef XSTL_WINDOWS
#error "NOT READY YET!"
#endif

static const char ntobjectComPrefix[] = "COM";

cSerialStream::cSerialStream(uint comport,
                             cSerialPort::BaudRate baudRate,
                             uint bitSize,
                             cSerialPort::ParityBit parity,
                             cSerialPort::StopBit stopBits) :
    m_serialHandle(cString(ntobjectComPrefix) + cString(comport),
                   cFile::READ | cFile::WRITE)
{
    initSeek();
    // Configure the stream
    cOSDef::fileHandle comHandle = m_serialHandle.getHandle()->getHandle();
    DCB dcb;
    CHECK(GetCommState(comHandle, &dcb));
    dcb.BaudRate = baudRate;
    dcb.ByteSize = bitSize;
    dcb.Parity   = parity;
    dcb.StopBits = stopBits;
    CHECK(SetCommState(comHandle, &dcb));
}

void cSerialStream::initSeek()
{
    // I can't do a thing...
    canLength        = false;
    canSeekFromBegin = false;
    canSeekForward   = false;
    canSeekBackward  = false;
    canSeekFromEnd   = false;
    canGetPointer    = false;
}

uint cSerialStream::getPointer() const
{
    CHECK_FAIL();
}

uint cSerialStream::length() const
{
    CHECK_FAIL();
}

bool cSerialStream::isEOS()
{
    // TODO!
    return false;
}

void cSerialStream::flush()
{
    m_serialHandle.flush();
}

uint cSerialStream::read(void *buffer, const uint length)
{
    return m_serialHandle.read(buffer, length);
}

uint cSerialStream::write(const void *buffer, const uint length)
{
    return m_serialHandle.write(buffer, length);
}

void cSerialStream::seek(const int distance,
                       const basicInput::seekMethod method)
{
    CHECK_FAIL();
}

bool cSerialStream::isRinging()
{
    DWORD mask;
    CHECK(GetCommMask(m_serialHandle.getHandle()->getHandle(),
          &mask));
    return (mask & EV_RING) == EV_RING;
}
