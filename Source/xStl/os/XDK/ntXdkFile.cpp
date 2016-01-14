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
 * ntXdkFile.cpp
 *
 * Implementation file for the OS::cFile interface for the XDK enviroment
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/data/datastream.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/traceStream.h"
#include "xStl/utils/algorithm.h"
#include "xStl/os/os.h"
#include "xStl/os/osdef.h"
#include "xStl/os/file.h"
#include "xStl/os/fileException.h"
#include "XDK/kernel.h"
#include "XDK/unicodeString.h"

cFile::cFile(const cString& filename,
             const uint flags /* = cFile::READ */) :
    m_handle(INVALID_FILE_HANDLE)
{
    // All file API must works at PASSIVE_LEVEL
    testPageableCode();

    ACCESS_MASK desiredAccess = SYNCHRONIZE;
    DWORD shareMode = 0;
    DWORD creationDisposition = FILE_OPEN;

    // Fills the desiredAccess
    if (flags & READ)
        desiredAccess|= STANDARD_RIGHTS_READ |
                        FILE_READ_DATA |
                        FILE_READ_ATTRIBUTES |
                        FILE_READ_EA;
    if (flags & WRITE)
        desiredAccess|= STANDARD_RIGHTS_WRITE |
                        FILE_WRITE_DATA |
                        FILE_WRITE_ATTRIBUTES |
                        FILE_WRITE_EA;
    if (flags & READWRITE)
        desiredAccess|= FILE_ALL_ACCESS;

    // Fills the shareMode
    if (flags & SHARE_READ)
        shareMode|= FILE_SHARE_READ;
    if (flags & SHARE_WRITE)
        shareMode|= FILE_SHARE_WRITE;

    // Fills the creationDisposition
    if (flags & CREATE)
        creationDisposition = FILE_OVERWRITE_IF;


    // The returned handle
    HANDLE handle;
    IO_STATUS_BLOCK ioStatus;

    // Prepare the filename
    OBJECT_ATTRIBUTES objectAttributes;
    cUnicodeString fileNameUnicodeString(filename);

    // The filename is insensitive
    InitializeObjectAttributes(&objectAttributes,
        fileNameUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);

    NTSTATUS ret = ZwCreateFile(&handle,
        desiredAccess, //  | SYNCHRONIZE
        &objectAttributes,
        &ioStatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        shareMode,
        creationDisposition,
        FILE_SYNCHRONOUS_IO_NONALERT, // No flushing
        NULL,
        0);

    // Test for open errors
    if ((!NT_SUCCESS(ret)) || (handle == INVALID_FILE_HANDLE))
    {
        traceHigh("cFile: ZwCreateFile failed on file " <<
                  filename << "  [" << HEXDWORD(ret) << "]" << endl);
        XSTL_THROW(cFileException, cOS::getLastErrorString().getBuffer());
    }

    traceLow("cFile: ZwCreateFile complete on file: " << filename << endl);

    openHandle(handle, flags, true);
}

cFile::cFile(cOSDef::fileHandle handle,
             const uint flags /* = cFile::READ */,
             bool closeHandle /* = true */) :
    m_handle(INVALID_FILE_HANDLE)
{
    openHandle(handle, flags, closeHandle);
}

bool cFile::isOpen() const
{
    return (m_handle != INVALID_FILE_HANDLE);
}

void cFile::openHandle(const cOSDef::fileHandle& handle,
                       const uint flags /* = cFile::READ*/,
                       bool closeHandle /* = true*/)
{
    ASSERT(!isOpen());
    if (handle == INVALID_FILE_HANDLE)
    {
        XSTL_THROW(cFileException, XSTL_STRING("Unable to open the file."));
    }
    m_handle = handle;
    m_closeHandle = closeHandle;
    m_isReadable = m_isWriteable = false;

    // Exception filter
    if (flags & READ)
        m_isReadable = TRUE;

    if (flags & WRITE)
        m_isWriteable = TRUE;

    if (flags & READWRITE)
    {
        m_isReadable = m_isWriteable = true;
    }
}

cFile::~cFile()
{
    // All file API must works at PASSIVE_LEVEL
    testPageableCode();

    // Test if the is opened
    if (isOpen())
    {
        flush();
        // Windows version
        ZwClose(m_handle);
        // Reset data (Legacy code)
        m_handle = INVALID_FILE_HANDLE;
        m_isReadable  = FALSE;
        m_isWriteable = FALSE;
    }
}

uint cFile::read(void *buffer, const uint length)
{
    // All file API must works at PASSIVE_LEVEL
    testPageableCode();

    ASSERT(isOpen());
    CHECK(m_isReadable);
    CHECK(buffer != NULL);

    IO_STATUS_BLOCK ioStatus;
    NTSTATUS ret;
    if (!NT_SUCCESS(ret = ZwReadFile(m_handle,
        NULL,
        NULL,
        NULL,
        &ioStatus,
        buffer,
        length,
        NULL,
        NULL)))
    {
        // There was an error
        traceHigh("cFile: ZwReadFile failed " << HEXDWORD(ret) << endl);
        XSTL_THROW(cFileException, cOS::getLastErrorString().getBuffer());
    }

    return ioStatus.Information;
}

uint cFile::write(const void *buffer, const uint length)
{
    // All file API must works at PASSIVE_LEVEL
    testPageableCode();

    ASSERT(isOpen());
    CHECK(m_isWriteable);
    CHECK(buffer != NULL);

    NTSTATUS ret;
    IO_STATUS_BLOCK ioStatus;
    if (!NT_SUCCESS(ret = ZwWriteFile(m_handle,
        NULL,
        NULL,
        NULL,
        &ioStatus,
        const_cast<void*>(buffer),
        length,
        NULL,
        NULL)))
    {
        // There was an error
        traceHigh("cFile: ZwWriteFile failed " << HEXDWORD(ret) << endl);
        XSTL_THROW(cFileException, cOS::getLastErrorString().getBuffer());
    }

    return ioStatus.Information;
}

uint cFile::length() const
{
    // All file API must works at PASSIVE_LEVEL
    testPageableCode();

    ASSERT(isOpen());

    IO_STATUS_BLOCK ioStatus;
    FILE_STANDARD_INFORMATION standardInformation;
    CHECK(NT_SUCCESS(ZwQueryInformationFile(m_handle,
                        &ioStatus,
                        &standardInformation,
                        sizeof(standardInformation),
                        FileStandardInformation)));

    // Downcast to 32bit!
    CHECK(standardInformation.EndOfFile.u.HighPart == 0);
    return (uint)(standardInformation.EndOfFile.u.LowPart);
}

void cFile::flush()
{
    ASSERT(isOpen());
    // No flushing!
}

uint cFile::getPointer() const
{
    // All file API must works at PASSIVE_LEVEL
    testPageableCode();

    ASSERT(isOpen());

    ASSERT(isOpen());

    IO_STATUS_BLOCK ioStatus;
    FILE_POSITION_INFORMATION positionInformation;
    CHECK(NT_SUCCESS(ZwQueryInformationFile(m_handle,
                            &ioStatus,
                            &positionInformation,
                            sizeof(positionInformation),
                            FilePositionInformation)));

    // Downcast to 32bit!
    CHECK(positionInformation.CurrentByteOffset.u.HighPart == 0);
    return (uint)(positionInformation.CurrentByteOffset.u.LowPart);
}

uint64 cFile::getPointer64() const
{
    ASSERT(false);
    return 0;
}

void cFile::seek(const int distance, const basicInput::seekMethod method)
{
    // All file API must works at PASSIVE_LEVEL
    testPageableCode();

    ASSERT(isOpen());

    // Translate action
    IO_STATUS_BLOCK ioStatus;
    FILE_POSITION_INFORMATION positionInformation;
    LARGE_INTEGER newPosition;

    if (method == basicInput::IO_SEEK_SET)
    {
        newPosition.QuadPart = distance;
    }
    if (method == basicInput::IO_SEEK_CUR)
    {
        newPosition.QuadPart = getPointer() + distance;
    }
    if (method == basicInput::IO_SEEK_END)
    {
        // TODO!, Fix common programming bug.
        ASSERT(distance <= 0);
        uint mesaure = (uint)t_abs(distance);
        // Seek from end.
        newPosition.QuadPart = length() - mesaure;
    }

    positionInformation.CurrentByteOffset = newPosition;

    CHECK(NT_SUCCESS(ZwSetInformationFile(m_handle,
                            &ioStatus,
                            &positionInformation,
                            sizeof(positionInformation),
                            FilePositionInformation)));
}

void cFile::seek64(const int64 distance, const basicInput::seekMethod method)
{
    ASSERT(false);
}

const cOSDef::fileHandle& cFile::getHandle() const
{
    return m_handle;
}

bool cFile::isOpenForWrite()
{
    return m_isWriteable;
}

bool cFile::isOpenForRead()
{
    return m_isReadable;
}

