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

// Only Windows operating system or Windows CE can compile with this API
#if defined(WIN32) || defined(_WIN32_WCE)

/*
 * winPosixFile.cpp
 *
 * Windows implementation of cPosixFile. The file-descriptor services are
 * POSIX only: every function throws EXCEPTION_FAILED (unsupported).
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/except/exception.h"
#include "xStl/os/posixFile.h"

#define POSIX_FILE_UNSUPPORTED() XSTL_THROW(cException, EXCEPTION_FAILED)

int64 cPosixFile::read(int fd, void* buffer, uint64 length)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::write(int fd, const void* buffer, uint64 length)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::pread(int fd, void* buffer, uint64 length, int64 offset)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::pwrite(int fd, const void* buffer, uint64 length,
                         int64 offset)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::readv(int fd, const cPosixIoVec* iov, int count)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::writev(int fd, const cPosixIoVec* iov, int count)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::openAt(int dirfd, const char* path, int hostFlags,
                         uint32 mode)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::close(int fd)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::lseek(int fd, int64 offset, int whence)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::dup(int fd)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::dup3(int oldFd, int newFd, int flags)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::pipe2(int fds[2], int flags)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::fcntlInt(int fd, int cmd, int64 arg)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::fcntlLock(int fd, int cmd, void* lockStruct)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::ioctlBuffer(int fd, uint64 request, void* buffer)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::ftruncate(int fd, int64 length)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::fsync(int fd)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::fdatasync(int fd)
{
    POSIX_FILE_UNSUPPORTED();
}

int64 cPosixFile::ppoll(void* pollfds, uint64 count, const int64* timeoutNs)
{
    POSIX_FILE_UNSUPPORTED();
}

#endif // defined(WIN32) || defined(_WIN32_WCE)
