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

/**
 * @file posixFile.h
 *
 * Thin POSIX file-descriptor services (read, write, open, pipes, fcntl,
 * ioctl, poll, fallocate, fadvise, flock, fchmod, fchown). Used by the Linux system call layer of reforge.
 *
 * Every function returns a value >= 0 on success or -errno on failure (the
 * host errno, unchanged). No function throws on an OS error. EINTR is not
 * retried: the caller sees it, as a native program would.
 *
 * Flags, commands and structures are the host's (O_* from <fcntl.h>,
 * struct flock, struct pollfd, ...). Converting guest encodings to host
 * values is the caller's job.
 *
 * Windows: every function throws cException(EXCEPTION_FAILED) (unsupported).
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#ifndef __TBA_STL_OS_POSIX_FILE_H
#define __TBA_STL_OS_POSIX_FILE_H

#include "xStl/types.h"

/**
 * @brief One element of a scatter/gather list for readv() and writev().
 */
struct cPosixIoVec
{
    void* base;
    uint64 length;
};

/**
 * @brief Static wrappers over the host file-descriptor calls.
 */
class cPosixFile
{
public:
    /** @brief Largest iovec count accepted by readv() and writev(). */
    enum { MAX_IOVECS = 1024 };

    /** @brief read(2). Returns the byte count (0 at end of file). */
    static int64 read(int fd, void* buffer, uint64 length);

    /** @brief write(2). Returns the byte count written. */
    static int64 write(int fd, const void* buffer, uint64 length);

    /** @brief pread(2) at absolute offset 'offset'. */
    static int64 pread(int fd, void* buffer, uint64 length, int64 offset);

    /** @brief pwrite(2) at absolute offset 'offset'. */
    static int64 pwrite(int fd, const void* buffer, uint64 length,
                        int64 offset);

    /** @brief readv(2). Returns -EINVAL when count is outside [0, MAX_IOVECS]. */
    static int64 readv(int fd, const cPosixIoVec* iov, int count);

    /** @brief writev(2). Returns -EINVAL when count is outside [0, MAX_IOVECS]. */
    static int64 writev(int fd, const cPosixIoVec* iov, int count);

    /**
     * @brief openat(2). 'dirfd' is a host fd or AT_FDCWD (host value). Returns
     *    the new fd. 'hostFlags' and 'mode' are host values.
     */
    static int64 openAt(int dirfd, const char* path, int hostFlags,
                        uint32 mode);

    /** @brief close(2). */
    static int64 close(int fd);

    /** @brief lseek(2) with host whence (SEEK_SET, SEEK_CUR, SEEK_END). */
    static int64 lseek(int fd, int64 offset, int whence);

    /** @brief dup(2). */
    static int64 dup(int fd);

    /** @brief dup3(2): duplicate 'oldFd' to 'newFd' with host 'flags'. */
    static int64 dup3(int oldFd, int newFd, int flags);

    /**
     * @brief pipe2(2). Stores the read end in fds[0] and the write end in
     *    fds[1] on success.
     */
    static int64 pipe2(int fds[2], int flags);

    /**
     * @brief fcntl(2) for commands taking an integer argument (F_GETFD,
     *    F_SETFD, F_GETFL, F_SETFL, F_DUPFD, ...). 'arg' is truncated to int.
     */
    static int64 fcntlInt(int fd, int cmd, int64 arg);

    /**
     * @brief fcntl(2) for lock commands. 'lockStruct' points to a host struct
     *    flock.
     */
    static int64 fcntlLock(int fd, int cmd, void* lockStruct);

    /**
     * @brief ioctl(2) with a buffer argument (e.g. TIOCGWINSZ). 'buffer' is
     *    passed through as is.
     */
    static int64 ioctlBuffer(int fd, uint64 request, void* buffer);

    /** @brief ftruncate(2). */
    static int64 ftruncate(int fd, int64 length);

    /** @brief fsync(2). */
    static int64 fsync(int fd);

    /** @brief fdatasync(2). */
    static int64 fdatasync(int fd);

    /**
     * @brief ppoll(2). 'pollfds' points to 'count' host struct pollfd.
     *    'timeoutNs' is nanoseconds; NULL waits forever. A negative timeout
     *    returns -EINVAL. Returns the number of ready descriptors.
     */
    static int64 ppoll(void* pollfds, uint64 count, const int64* timeoutNs);

    /**
     * @brief fallocate(2). 'mode' is the host FALLOC_FL_* mask, 'offset' and
     *    'length' are in bytes. Returns 0 or -errno.
     */
    static int64 fallocate(int fd, int mode, int64 offset, int64 length);

    /**
     * @brief posix_fadvise(3) with the host POSIX_FADV_* 'advice'. Returns 0
     *    or -errno. The libc error number is negated, not left in errno.
     */
    static int64 fadvise(int fd, int64 offset, int64 length, int advice);

    /**
     * @brief flock(2). 'operation' is the host LOCK_SH, LOCK_EX or LOCK_UN,
     *    optionally or-ed with LOCK_NB. Returns 0 or -errno.
     */
    static int64 flock(int fd, int operation);

    /**
     * @brief fchmod(2). 'mode' is the host permission bits (S_I* mask). Returns
     *    0 or -errno.
     */
    static int64 fchmod(int fd, uint32 mode);

    /**
     * @brief fchown(2). 'uid' and 'gid' are host ids truncated to uid_t and
     *    gid_t; -1 leaves that id unchanged. Returns 0 or -errno.
     */
    static int64 fchown(int fd, int64 uid, int64 gid);
};

#endif // __TBA_STL_OS_POSIX_FILE_H
