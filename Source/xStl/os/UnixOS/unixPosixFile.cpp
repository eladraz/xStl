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
 * unixPosixFile.cpp
 *
 * Unix implementation of cPosixFile: each call wraps one libc call and
 * returns its result, or -errno when it failed.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

// System headers come before xStl/types.h ('uint' macro clash)
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <sys/file.h>
#include <time.h>
#include <errno.h>

#include "xStl/types.h"
#include "xStl/os/posixFile.h"

// A libc result (-1 with errno on failure) as the -errno convention. errno is
// read before anything else runs.
static int64 posixResult(long long result)
{
    return (result < 0) ? -(int64)errno : (int64)result;
}

// Copy a cPosixIoVec array into host iovecs. False when count is out of range.
static bool toHostIovec(const cPosixIoVec* iov, int count,
                        struct iovec* host)
{
    if (count < 0 || count > cPosixFile::MAX_IOVECS)
    {
        return false;
    }
    for (int i = 0; i < count; i++)
    {
        host[i].iov_base = iov[i].base;
        host[i].iov_len = (size_t)iov[i].length;
    }
    return true;
}

int64 cPosixFile::read(int fd, void* buffer, uint64 length)
{
    return posixResult(::read(fd, buffer, (size_t)length));
}

int64 cPosixFile::write(int fd, const void* buffer, uint64 length)
{
    return posixResult(::write(fd, buffer, (size_t)length));
}

int64 cPosixFile::pread(int fd, void* buffer, uint64 length, int64 offset)
{
    return posixResult(::pread(fd, buffer, (size_t)length, (off_t)offset));
}

int64 cPosixFile::pwrite(int fd, const void* buffer, uint64 length,
                         int64 offset)
{
    return posixResult(::pwrite(fd, buffer, (size_t)length, (off_t)offset));
}

int64 cPosixFile::readv(int fd, const cPosixIoVec* iov, int count)
{
    struct iovec host[MAX_IOVECS];
    if (!toHostIovec(iov, count, host))
    {
        return -(int64)EINVAL;
    }
    return posixResult(::readv(fd, host, count));
}

int64 cPosixFile::writev(int fd, const cPosixIoVec* iov, int count)
{
    struct iovec host[MAX_IOVECS];
    if (!toHostIovec(iov, count, host))
    {
        return -(int64)EINVAL;
    }
    return posixResult(::writev(fd, host, count));
}

int64 cPosixFile::openAt(int dirfd, const char* path, int hostFlags,
                         uint32 mode)
{
    return posixResult(::openat(dirfd, path, hostFlags, (mode_t)mode));
}

int64 cPosixFile::close(int fd)
{
    return posixResult(::close(fd));
}

int64 cPosixFile::lseek(int fd, int64 offset, int whence)
{
    return posixResult(::lseek(fd, (off_t)offset, whence));
}

int64 cPosixFile::dup(int fd)
{
    return posixResult(::dup(fd));
}

int64 cPosixFile::dup3(int oldFd, int newFd, int flags)
{
#ifdef __linux__
    return posixResult(::dup3(oldFd, newFd, flags));
#else
    (void)oldFd; (void)newFd; (void)flags;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixFile::pipe2(int fds[2], int flags)
{
#ifdef __linux__
    return posixResult(::pipe2(fds, flags));
#else
    (void)fds; (void)flags;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixFile::fcntlInt(int fd, int cmd, int64 arg)
{
    return posixResult(::fcntl(fd, cmd, (int)arg));
}

int64 cPosixFile::fcntlLock(int fd, int cmd, void* lockStruct)
{
    return posixResult(::fcntl(fd, cmd, (struct flock*)lockStruct));
}

int64 cPosixFile::ioctlBuffer(int fd, uint64 request, void* buffer)
{
    return posixResult(::ioctl(fd, (unsigned long)request, buffer));
}

int64 cPosixFile::ftruncate(int fd, int64 length)
{
    return posixResult(::ftruncate(fd, (off_t)length));
}

int64 cPosixFile::fsync(int fd)
{
    return posixResult(::fsync(fd));
}

int64 cPosixFile::fdatasync(int fd)
{
#ifdef __linux__
    return posixResult(::fdatasync(fd));
#else
    (void)fd;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixFile::ppoll(void* pollfds, uint64 count, const int64* timeoutNs)
{
#ifdef __linux__
    struct timespec timeout;
    struct timespec* timeoutPtr = NULL;
    if (timeoutNs != NULL)
    {
        if (*timeoutNs < 0)
        {
            return -(int64)EINVAL;
        }
        timeout.tv_sec = (time_t)(*timeoutNs / 1000000000LL);
        timeout.tv_nsec = (long)(*timeoutNs % 1000000000LL);
        timeoutPtr = &timeout;
    }
    return posixResult(::ppoll((struct pollfd*)pollfds, (nfds_t)count,
                               timeoutPtr, NULL));
#else
    (void)pollfds; (void)count; (void)timeoutNs;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixFile::fallocate(int fd, int mode, int64 offset, int64 length)
{
#ifdef __linux__
    return posixResult(::fallocate(fd, mode, (off_t)offset, (off_t)length));
#else
    (void)fd; (void)mode; (void)offset; (void)length;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixFile::fadvise(int fd, int64 offset, int64 length, int advice)
{
#ifdef __linux__
    // posix_fadvise returns the error number itself, not -1 with errno
    int ret = posix_fadvise(fd, (off_t)offset, (off_t)length, advice);
    return (ret == 0) ? 0 : -(int64)ret;
#else
    (void)fd; (void)offset; (void)length; (void)advice;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixFile::flock(int fd, int operation)
{
    return posixResult(::flock(fd, operation));
}
