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
 * file.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/assert.h"
#include "xStl/except/trace.h"
#include "xStl/except/exception.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/fileStream.h"
#include "xStl/os/os.h"
#include "xStl/os/file.h"
#include "xStl/os/fileException.h"

cFileStream::cFileStream() :
    m_file(NULL)
{
    initSeek();
}

cFileStream::cFileStream(const cString& filename,
                         const uint flags) :
    m_file(NULL)
{
    initSeek();
    open(filename, flags);
}

cFileStream::cFileStream(const cFilePtr& filePtr) :
    m_file(filePtr)
{
}

void cFileStream::initSeek()
{
    canLength        = true;
    canSeekFromBegin = true;
    canSeekForward   = true;
    canSeekBackward  = true;
    canSeekFromEnd   = true;
    canGetPointer    = true;
}

void cFileStream::open(const cString& filename,
                       const uint flags /* = cFile::FILE_READ*/)
{
    // Init the handle
    m_file = cFilePtr(new cFile(filename, flags));
}

void cFileStream::close()
{
    CHECK(isOpen());
    // Dereference pointer
    m_file = cFilePtr(NULL);
}


void cFileStream::openHandle(const cOSDef::fileHandle& handle,
                             const uint flags, /* = cFileStream::FILE_READ*/
                             bool closeHandle /* = true*/)
{
    // Init the handle
    m_file = cFilePtr(new cFile(handle, flags, closeHandle));
}

bool cFileStream::isOpen() const
{
    if (m_file.getPointer() == NULL)
        return false;

    return m_file->isOpen();
}

uint cFileStream::read(void *buffer, const uint length)
{
    CHECK(isOpen());
    return m_file->read(buffer, length);
}

uint cFileStream::write (const void *buffer, const uint length)
{
    CHECK(isOpen());
    return m_file->write(buffer, length);
}

uint cFileStream::length() const
{
    CHECK(isOpen());
    return m_file->length();
}

void cFileStream::flush()
{
    CHECK(isOpen());
    m_file->flush();
}

bool cFileStream::isEOS()
{
    CHECK(isOpen());

    /* Check whether the length of the file equal the position */
    uint fileLength   = length();
    uint filePosition = getPointer();

    return (fileLength == filePosition);
}

uint cFileStream::getPointer() const
{
    CHECK(isOpen());
    return m_file->getPointer();
}

uint64 cFileStream::getPointer64() const
{
    CHECK(isOpen());
    return m_file->getPointer64();
}

void cFileStream::seek(const int distance, const basicInput::seekMethod method)
{
    CHECK(isOpen());

    /* See whether we can accomplish the seek */
    if (!__canSeek(distance, method))
    {
        XSTL_THROW(cFileException, EXCEPTION_SEEK_ERROR);
    }

    m_file->seek(distance, method);
}

void cFileStream::seek64(const int64 distance, const basicInput::seekMethod method)
{
    CHECK(isOpen());
    m_file->seek64(distance, method);
}

bool cFileStream::isOpenForWrite()
{
    if (!isOpen()) return false;
    return m_file->isOpenForWrite();
}

bool cFileStream::isOpenForRead()
{
    if (!isOpen()) return false;
    return m_file->isOpenForRead();
}

const cFilePtr& cFileStream::getHandle() const
{
    return m_file;
}
