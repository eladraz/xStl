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
 * winFile.cpp
 *
 * Implementation file for the OS::cFile interface.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/os.h"
#include "xStl/os/osdef.h"
#include "xStl/os/file.h"
#include "xStl/os/fileException.h"

cFile::cFile(const cString& filename,
             const uint flags /* = cFile::READ */) :
    m_handle(INVALID_FILE_HANDLE)
{
    DWORD dwShareMode = 0;
    DWORD dwDesiredAccess = 0;
    DWORD dwCreationDisposition = OPEN_EXISTING;

    /* fill in the dwDesiredAccess variable */
    if (flags & READWRITE)
        dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
    else
    {
        if (flags & READ)
            dwDesiredAccess|= GENERIC_READ;

        if (flags & WRITE)
            dwDesiredAccess|= GENERIC_WRITE;
    }

    /* fill in the dwSharedMode variable */
    if (flags & SHARE_READ)
        dwShareMode|= FILE_SHARE_READ;

    if (flags & SHARE_WRITE)
        dwShareMode|= FILE_SHARE_WRITE;


    /* fill in the dwCreationDisposition variable */
    if (flags & CREATE)
        dwCreationDisposition = CREATE_ALWAYS;

    HANDLE handle = CreateFile(OS_CSTRING(filename.getBuffer()),
                          dwDesiredAccess,
                          dwShareMode,
                          NULL,
                          dwCreationDisposition,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

    /* Test for open errors */
    if (handle == INVALID_FILE_HANDLE)
    {
        cString msg = cOS::getLastErrorString() + ": " + filename;
        XSTL_THROW(cFileException, msg.getBuffer());
    }

    openHandle(handle, flags, true);
}

cFile::cFile(cOSDef::fileHandle handle,
             const uint flags /* = cFile::READ */,
             bool closeHandle /* = true */) :
    m_handle(INVALID_FILE_HANDLE)
{
    openHandle(handle, flags, closeHandle);
}

bool cFile::isFileExist(const cString& filename)
{
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(OS_CSTRING(filename.getBuffer()), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return false;
    FindClose(hFind);

    // If filename contains * or ? character this prevent any problems.
    if ((filename.find("*") == filename.length()) &&
        (filename.find("?") == filename.length()))
        return true;
    return false;
}

bool cFile::deleteFile(const cString& filename)
{
    return (::DeleteFile(OS_CSTRING(filename.getBuffer())) == TRUE);
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
    // Test if the is opened
    if (isOpen())
    {
        flush();
        // Windows version
        CloseHandle(m_handle);
        // Reset data (Legacy code)
        m_handle = INVALID_FILE_HANDLE;
        m_isReadable  = FALSE;
        m_isWriteable = FALSE;
    }
}

uint cFile::read(void *buffer, const uint length)
{
    ASSERT(isOpen());
    CHECK(m_isReadable);

    DWORD readed;
    if (!ReadFile(m_handle, buffer, length, &readed, NULL))
    {
        /* There was an error */
        XSTL_THROW(cFileException, cOS::getLastErrorString().getBuffer());
    }

    return readed;
}

uint cFile::write(const void *buffer, const uint length)
{
    ASSERT(isOpen());
    CHECK(m_isWriteable);

    DWORD written;
    if (!WriteFile(m_handle, buffer, length, &written, NULL))
    {
        /* There was an error */
        XSTL_THROW(cFileException, cOS::getLastErrorString().getBuffer());
    }

    return written;
}

uint cFile::length() const
{
    ASSERT(isOpen());

    DWORD fileSize;
    fileSize = GetFileSize(m_handle, NULL);
    // Test for an error
    if (fileSize == INVALID_FILE_SIZE)
    {
        XSTL_THROW(cFileException, XSTL_STRING("Unable to get the file size"));
        return 0;
    }
    return fileSize;
}

void cFile::flush()
{
    ASSERT(isOpen());

    /* WINDOWS version */
    FlushFileBuffers(m_handle);
}

uint cFile::getPointer() const
{
    ASSERT(isOpen());

    // WINDOWS version
    return SetFilePointer(m_handle, 0, NULL, FILE_CURRENT);
}

uint64 cFile::getPointer64() const
{
    ASSERT(isOpen());

    LARGE_INTEGER nothing;
    nothing.QuadPart = 0;
    LARGE_INTEGER filePosition;

    CHECK(FALSE !=
          SetFilePointerEx(m_handle, nothing, &filePosition, FILE_CURRENT));

    return filePosition.QuadPart;
}

void cFile::seek(const int distance, const basicInput::seekMethod method)
{
    ASSERT(isOpen());

    DWORD dwMoveMethod;
    DWORD filePosition;

    /* Translate action */
    if (method == basicInput::IO_SEEK_SET) dwMoveMethod = FILE_BEGIN;
    if (method == basicInput::IO_SEEK_CUR) dwMoveMethod = FILE_CURRENT;
    if (method == basicInput::IO_SEEK_END) dwMoveMethod = FILE_END;

    filePosition = SetFilePointer(m_handle, distance , NULL, dwMoveMethod);
    if (filePosition == INVALID_FILE_SIZE)
    {
        // Cannot seek
        XSTL_THROW(cFileException, EXCEPTION_SEEK_ERROR);
    }
}

void cFile::seek64(const int64 distance, const basicInput::seekMethod method)
{
    ASSERT(isOpen());

    DWORD dwMoveMethod;
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = distance;

    /* Translate action */
    if (method == basicInput::IO_SEEK_SET) dwMoveMethod = FILE_BEGIN;
    if (method == basicInput::IO_SEEK_CUR) dwMoveMethod = FILE_CURRENT;
    if (method == basicInput::IO_SEEK_END) dwMoveMethod = FILE_END;

    CHECK(FALSE !=
         SetFilePointerEx(m_handle, liDistanceToMove, NULL, dwMoveMethod));
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

#endif // WIN32
