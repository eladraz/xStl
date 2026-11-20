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

#include <errno.h>

/*
 * Map a Windows error code to the errno value reported by tryOpen().
 */
static int windowsErrorToErrno(const DWORD error)
{
    switch (error)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
        return ENOENT;
    case ERROR_ACCESS_DENIED:
    case ERROR_SHARING_VIOLATION:
    case ERROR_LOCK_VIOLATION:
        return EACCES;
    case ERROR_FILE_EXISTS:
    case ERROR_ALREADY_EXISTS:
        return EEXIST;
    case ERROR_TOO_MANY_OPEN_FILES:
        return EMFILE;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
        return ENOMEM;
    case ERROR_INVALID_PARAMETER:
        return EINVAL;
    }
    return EIO;
}

/*
 * Open the file without throwing. Returns INVALID_FILE_HANDLE on failure and
 * stores the Windows error code in lastError.
 */
static cOSDef::fileHandle openFileHandle(const cString& filename,
                                         const uint flags,
                                         DWORD& lastError)
{
    DWORD dwShareMode = 0;
    DWORD dwDesiredAccess = 0;
    DWORD dwCreationDisposition = OPEN_EXISTING;

    /*
     * Append (O_APPEND semantics): FILE_APPEND_DATA without FILE_WRITE_DATA
     * makes every write go to the end of the file, whatever the position.
     * The rights are GENERIC_WRITE minus FILE_WRITE_DATA, listed explicitly
     * (masking the generic bit would not remove FILE_WRITE_DATA).
     */
    const DWORD writeAccess = (flags & cFile::APPEND)
        ? (FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA |
           READ_CONTROL | SYNCHRONIZE)
        : GENERIC_WRITE;

    /* fill in the dwDesiredAccess variable */
    if (flags & cFile::READWRITE)
        dwDesiredAccess = writeAccess | GENERIC_READ;
    else
    {
        if (flags & cFile::READ)
            dwDesiredAccess|= GENERIC_READ;

        if (flags & cFile::WRITE)
            dwDesiredAccess|= writeAccess;
    }

    /* fill in the dwSharedMode variable */
    if (flags & cFile::SHARE_READ)
        dwShareMode|= FILE_SHARE_READ;

    if (flags & cFile::SHARE_WRITE)
        dwShareMode|= FILE_SHARE_WRITE;


    /* fill in the dwCreationDisposition variable */
    if (flags & cFile::CREATE)
    {
        // Append keeps the content, so the file is only created when missing
        dwCreationDisposition = (flags & cFile::APPEND) ? OPEN_ALWAYS
                                                        : CREATE_ALWAYS;
    }

    HANDLE handle = CreateFile(OS_CSTRING(filename.getBuffer()),
                          dwDesiredAccess,
                          dwShareMode,
                          NULL,
                          dwCreationDisposition,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    if (handle == INVALID_FILE_HANDLE)
    {
        lastError = GetLastError();
        return INVALID_FILE_HANDLE;
    }

    return handle;
}

cFile::cFile(const cString& filename,
             const uint flags /* = cFile::READ */) :
    m_handle(INVALID_FILE_HANDLE)
{
    DWORD lastError = 0;
    HANDLE handle = openFileHandle(filename, flags, lastError);

    /* Test for open errors */
    if (handle == INVALID_FILE_HANDLE)
    {
        cString msg = cOS::getLastErrorString() + ": " + filename;
        XSTL_THROW(cFileException, msg.getBuffer());
    }

    openHandle(handle, flags, true);
}

cFile::cFile() :
    m_handle(INVALID_FILE_HANDLE),
    m_isReadable(false),
    m_isWriteable(false),
    m_closeHandle(false)
{
}

bool cFile::tryOpen(const cString& name, const uint flags, int& osError)
{
    osError = 0;
    if (isOpen())
    {
        osError = EBUSY;
        return false;
    }

    DWORD lastError = 0;
    HANDLE handle = openFileHandle(name, flags, lastError);
    if (handle == INVALID_FILE_HANDLE)
    {
        osError = windowsErrorToErrno(lastError);
        return false;
    }

    openHandle(handle, flags, true);
    return true;
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

bool cFile::renameFile(const cString& from, const cString& to)
{
    return (::MoveFileEx(OS_CSTRING(from.getBuffer()),
                         OS_CSTRING(to.getBuffer()),
                         MOVEFILE_REPLACE_EXISTING) == TRUE);
}

bool cFile::isTerminal(StandardStream stream)
{
    DWORD id = STD_INPUT_HANDLE;
    switch (stream)
    {
    case STANDARD_INPUT:
        id = STD_INPUT_HANDLE;
        break;
    case STANDARD_OUTPUT:
        id = STD_OUTPUT_HANDLE;
        break;
    case STANDARD_ERROR:
        id = STD_ERROR_HANDLE;
        break;
    }
    DWORD mode = 0;
    return GetConsoleMode(GetStdHandle(id), &mode) != 0;
}

cFile& cFile::getStandardStream(StandardStream stream)
{
    // The objects are never closing their handles (closeHandle=false)
    static cFile standardInput(GetStdHandle(STD_INPUT_HANDLE),
                               cFile::READ, false);
    static cFile standardOutput(GetStdHandle(STD_OUTPUT_HANDLE),
                                cFile::WRITE, false);
    static cFile standardError(GetStdHandle(STD_ERROR_HANDLE),
                               cFile::WRITE, false);

    switch (stream)
    {
    case STANDARD_INPUT:
        return standardInput;
    case STANDARD_OUTPUT:
        return standardOutput;
    case STANDARD_ERROR:
        return standardError;
    }
    XSTL_THROW(cFileException, XSTL_STRING("Unknown standard stream."));
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
    // Test if the file is opened
    if (isOpen())
    {
        flush();
        // Windows version
        if (m_closeHandle)
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

    DWORD bytesRead;
    if (!ReadFile(m_handle, buffer, length, &bytesRead, NULL))
    {
        /* There was an error */
        XSTL_THROW(cFileException, cOS::getLastErrorString().getBuffer());
    }

    return bytesRead;
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

void cFile::writeAll(const void *buffer, const uint length)
{
    ASSERT(isOpen());
    CHECK(m_isWriteable);

    const uint8 *cursor = static_cast<const uint8 *>(buffer);
    uint remaining = length;
    while (remaining > 0)
    {
        DWORD written = 0;
        if (!WriteFile(m_handle, cursor, remaining, &written, NULL))
        {
            /* There was an error */
            XSTL_THROW(cFileException, cOS::getLastErrorString().getBuffer());
        }
        if (written == 0)
        {
            XSTL_THROW(cFileException, XSTL_STRING("Writing to file failed."));
        }
        cursor += written;
        remaining -= written;
    }
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
