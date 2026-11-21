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

#ifndef __TBA_STL_OS_POSIXPATH_H
#define __TBA_STL_OS_POSIXPATH_H

#include "xStl/types.h"

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * Neutral file status record filled by cPosixPath::fstatAt. Field values are
 * the host's, converted to fixed widths; no host struct layout leaks out.
 */
struct cPosixStat
{
    uint64 dev;
    uint64 ino;
    uint64 nlink;
    uint64 rdev;
    uint32 mode;
    uint32 uid;
    uint32 gid;
    int64 size;
    int64 blksize;
    int64 blocks;
    int64 atimeSec;
    int64 atimeNsec;
    int64 mtimeSec;
    int64 mtimeNsec;
    int64 ctimeSec;
    int64 ctimeNsec;
};

/**
 * POSIX path, directory and metadata services (the *at() family, stat, getdents).
 *
 * Every call returns a value >= 0 on success or -errno on failure, so callers
 * can translate the error without an exception. Descriptors are passed as
 * int64 so that AT_FDCWD (-100) and host fds fit one parameter type.
 *
 * Windows builds compile the same interface; every call throws
 * EXCEPTION_FAILED (the POSIX path model is UNIX-only).
 */
class cPosixPath
{
public:
    /**
     * fstatat(2). With an empty path and AT_EMPTY_PATH in flags this is fstat(2).
     * @return 0 or -errno; stat is filled on success.
     */
    static int64 fstatAt(int64 dirfd, const char* path, int flags,
                         cPosixStat& stat);

    /**
     * statx(2) through the raw system call. buf must point to 256 writable
     * bytes (struct statx). @return 0 or -errno.
     */
    static int64 statx(int64 dirfd, const char* path, int flags, uint32 mask,
                       void* buf);

    /**
     * faccessat(2). @return 0 or -errno.
     */
    static int64 faccessAt(int64 dirfd, const char* path, int mode, int flags);

    /**
     * unlinkat(2); AT_REMOVEDIR in flags removes a directory. @return 0 or -errno.
     */
    static int64 unlinkAt(int64 dirfd, const char* path, int flags);

    /**
     * mkdirat(2). @return 0 or -errno.
     */
    static int64 mkdirAt(int64 dirfd, const char* path, uint32 mode);

    /**
     * renameat2(2) (glibc wrapper when available, else the system call).
     * @return 0 or -errno.
     */
    static int64 renameAt2(int64 oldDirfd, const char* oldPath, int64 newDirfd,
                           const char* newPath, uint32 flags);

    /**
     * linkat(2). @return 0 or -errno.
     */
    static int64 linkAt(int64 oldDirfd, const char* oldPath, int64 newDirfd,
                        const char* newPath, int flags);

    /**
     * symlinkat(2). @return 0 or -errno.
     */
    static int64 symlinkAt(const char* target, int64 newDirfd,
                           const char* linkPath);

    /**
     * readlinkat(2). The link target is written without a NUL terminator.
     * @return number of bytes written or -errno.
     */
    static int64 readlinkAt(int64 dirfd, const char* path, char* buf,
                            uint64 len);

    /**
     * getcwd(3). @return the length including the NUL terminator, or -errno.
     */
    static int64 getcwd(char* buf, uint64 len);

    /**
     * chdir(2). @return 0 or -errno.
     */
    static int64 chdir(const char* path);

    /**
     * fchdir(2). @return 0 or -errno.
     */
    static int64 fchdir(int64 fd);

    /**
     * umask(2). Never fails; @return the previous mask.
     */
    static int64 umask(uint32 mask);

    /**
     * utimensat(2). times is either NULL (set both to now) or four values:
     * atimeSec, atimeNsec, mtimeSec, mtimeNsec. @return 0 or -errno.
     */
    static int64 utimensAt(int64 dirfd, const char* path, const int64* times,
                           int flags);

    /**
     * fchmodat(2). @return 0 or -errno.
     */
    static int64 fchmodAt(int64 dirfd, const char* path, uint32 mode,
                          int flags);

    /**
     * getdents64(2) through the raw system call. Fills buf with kernel
     * linux_dirent64 records. @return bytes written (0 at end) or -errno.
     */
    static int64 getdents64(int64 fd, void* buf, uint64 len);

    /**
     * statfs(2) through the raw system call, so buf receives the kernel's
     * struct statfs (120 bytes on 64-bit targets, the guest layout). buf must
     * point to 120 writable bytes. @return 0 or -errno.
     */
    static int64 statfs(const char* path, void* buf);

    /**
     * fstatfs(2) through the raw system call; buf as for statfs().
     * @return 0 or -errno.
     */
    static int64 fstatfs(int64 fd, void* buf);
};

/**
 * @}
 */

#endif /* __TBA_STL_OS_POSIXPATH_H */
