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
 * pipeStream.cpp
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/fileStream.h"
#include "xStl/stream/pipeStream.h"

#ifndef XSTL_WINDOWS
#error "NOT READY YET!"
#endif

static const char ntobjectPipePrefix[] = "\\\\.\\pipe\\";


cPipeStream::cPipeStream(const cString& pipeName) :
    m_shouldDestroyServerPipe(false),
    m_pipeHandle(cString(ntobjectPipePrefix) + pipeName,
                 cFile::READ | cFile::WRITE |
                 cFile::SHARE_READ | cFile::SHARE_WRITE)
{
    initSeek();
}

cPipeStream::cPipeStream(const cOSDef::fileHandle& pipeServerHandle) :
    m_shouldDestroyServerPipe(true)
{
    m_pipeHandle.openHandle(pipeServerHandle,
        cFile::READ | cFile::WRITE |
        cFile::SHARE_READ | cFile::SHARE_WRITE);
}

cPipeStream::~cPipeStream()
{
    // Always good to flush the buffers
    flush();

    if (m_shouldDestroyServerPipe)
        DisconnectNamedPipe(m_pipeHandle.getHandle()->getHandle());
}

BasicIOPtr cPipeStream::listenOnPipe(const cString& pipeName)
{
    cString namedObject = cString(ntobjectPipePrefix) + pipeName;
    HANDLE pipeHandle = CreateNamedPipe(namedObject.getBuffer(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        4096,
        4096,
        5*60*1000,  // 5 minutes timeout. TODO!
        NULL);

    CHECK(pipeHandle != INVALID_HANDLE_VALUE);

    // Windows error reporting...
    bool ok = ConnectNamedPipe(pipeHandle, NULL) ?
        true : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!ok)
    {
        CloseHandle(pipeHandle);
        CHECK_FAIL();
    }

    return BasicIOPtr(new cPipeStream(pipeHandle));
}

void cPipeStream::initSeek()
{
    // I can't do a thing...
    canLength        = false;
    canSeekFromBegin = false;
    canSeekForward   = false;
    canSeekBackward  = false;
    canSeekFromEnd   = false;
    canGetPointer    = false;
}

uint cPipeStream::getPointer() const
{
    CHECK_FAIL();
}

uint cPipeStream::length() const
{
    CHECK_FAIL();
}

bool cPipeStream::isEOS()
{
    // TODO!
    return false;
}

void cPipeStream::flush()
{
    m_pipeHandle.flush();
}

uint cPipeStream::read(void *buffer, const uint length)
{
    return m_pipeHandle.read(buffer, length);
}

uint cPipeStream::write(const void *buffer, const uint length)
{
    return m_pipeHandle.write(buffer, length);
}

void cPipeStream::seek(const int distance,
                       const basicInput::seekMethod method)
{
    CHECK_FAIL();
}
