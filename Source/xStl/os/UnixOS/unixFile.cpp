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
 * unixFile.cpp
 *
 * Implementation file for UNIX based system
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#ifdef __APPLE__
#include <sys/types.h>
#endif

#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/os.h"
#include "xStl/os/osdef.h"
#include "xStl/os/file.h"
#include "xStl/os/fileException.h"

#ifndef XSTL_LINUX
    #error "UNIX source file"
#endif

// Linux version
#undef __USE_MISC

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


cFile::cFile(const cString& filename,
             const uint flags /* = cFile::READ */) :
    m_handle(INVALID_FILE_HANDLE)
{
    #ifdef XSTL_MACOSX
        // Files on MAC are automaticly 64bit
        int o_flags = 0;
    #else
        int o_flags = O_LARGEFILE;
    #endif

    if (flags & READ)
        o_flags|= O_RDONLY;

    if (flags & WRITE)
        o_flags|= O_WRONLY;

    if ((flags & READWRITE) == READWRITE)
        o_flags = O_RDWR;

    // fill in the dwSharedMode variable
    //if (flags & SHARE_READ)
    //if (flags & SHARE_WRITE)

    // fill in the dwCreationDisposition variable
    int handle = INVALID_FILE_HANDLE;
    if (flags & CREATE)
    {
        // TODO!!!

        //o_flags = o_flags | O_CREAT | O_TRUNC;
        //mode_t mode = 666;
        //mode_t old = umask(mode);
        //handle = ::open(filename.getASCIIstring().getBuffer(), o_flags, mode);
        //umask(old);

        // Create the file and reopen it. The above method doesn't seems to
        // work on ubuntu
        FILE* file = fopen(filename.getASCIIstring().getBuffer(), "w+");
        if (file != NULL)
        {
            fclose(file);
            handle = ::open(filename.getASCIIstring().getBuffer(), o_flags);
        }
    } else
    {
        handle = ::open(filename.getASCIIstring().getBuffer(), o_flags);
    }


    // Test for open errors
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
    // Check if file exist
    struct stat st;
    if (stat(filename.getASCIIstring().getBuffer(), &st) == 0)
    {
        // Return true on all type of files/devices except directories
        return S_ISDIR(st.st_mode);
    }

    return false;
}

bool cFile::deleteFile(const cString& filename)
{
    return (::unlink(filename.getASCIIstring().getBuffer()) == 0);
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
        // UNIX version
        ::close(m_handle);
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

    DWORD readed = ::read(m_handle, buffer, length);

    // Test for errors
    if (readed == ((DWORD)-1))
    {
        XSTL_THROW(cFileException, XSTL_STRING("Reading from file faild."));
    }

    return readed;
}

uint cFile::write(const void *buffer, const uint length)
{
    ASSERT(isOpen());
    CHECK(m_isWriteable);

    DWORD written = ::write(m_handle, buffer, length);
    if (written == ((DWORD)-1))
    {
        /* There was an error */
        XSTL_THROW(cFileException, XSTL_STRING("Writeing to file faild."));
    }

    return written;
}

uint cFile::length() const
{
    ASSERT(isOpen());


    /* Linux version */
    DWORD fileLength;
    DWORD fileOffset = lseek(m_handle, 0, SEEK_CUR);

    /* Seek to the end */
    if ((fileLength = lseek(m_handle, 0, SEEK_END)) == 0)
    {
        XSTL_THROW(cFileException, XSTL_STRING(
            "Unable to get the file size (cannot set back head location)"));
    }

    /* Seek back to the file location */
    if (lseek(m_handle, fileOffset, SEEK_SET) == -1)
    {
        XSTL_THROW(cFileException,
            XSTL_STRING("Unable to get the file size (cannot set back head location)"));
    }

    return fileLength;
}

void cFile::flush()
{
    ASSERT(isOpen());

    // UNIX version
    fsync(m_handle);
}

uint cFile::getPointer() const
{
    ASSERT(isOpen());

    // UNIX version
    return lseek(m_handle, 0, SEEK_CUR);
}

uint64 cFile::getPointer64() const
{
    ASSERT(isOpen());

    // UNIX version
    #ifdef XSTL_MACOSX
        // MAC has an off_t of 64bit by default
        return lseek(m_handle, 0, SEEK_CUR);
    #else
        return lseek64(m_handle, 0, SEEK_CUR);
    #endif // XSTL_MACOSX
}

void cFile::seek(const int distance, const basicInput::seekMethod method)
{
    ASSERT(isOpen());

    int iMoveMethod;

    if (method == basicInput::IO_SEEK_SET) iMoveMethod = SEEK_SET;
    if (method == basicInput::IO_SEEK_CUR) iMoveMethod = SEEK_CUR;
    if (method == basicInput::IO_SEEK_END) iMoveMethod = SEEK_END;

    off_t filePosition = lseek(m_handle, distance, iMoveMethod);
    if (filePosition == (off_t)-1)
    {
        XSTL_THROW(cFileException, EXCEPTION_SEEK_ERROR);
    }
}

void cFile::seek64(const int64 distance, const basicInput::seekMethod method)
{
    ASSERT(isOpen());

    int iMoveMethod;

    if (method == basicInput::IO_SEEK_SET) iMoveMethod = SEEK_SET;
    if (method == basicInput::IO_SEEK_CUR) iMoveMethod = SEEK_CUR;
    if (method == basicInput::IO_SEEK_END) iMoveMethod = SEEK_END;

#ifdef XSTL_MACOSX
    // MAC has an off_t of 64bit by default
    off_t filePosition = lseek(m_handle, distance, iMoveMethod);
    if (filePosition == ((off_t)-1))
#else
    off64_t filePosition = lseek64(m_handle, distance, iMoveMethod);
    if (filePosition == ((off64_t)-1))
#endif // XSTL_MACOSX

    {
        XSTL_THROW(cFileException, EXCEPTION_SEEK_ERROR);
    }
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
