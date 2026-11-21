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
 * unixPosixPath.cpp
 *
 * UNIX implementation of cPosixPath: thin wrappers over the *at() family and
 * getdents64/statx, returning 0 or >= 0 on success and -errno on failure.
 */

// System headers come before xStl/types.h ('uint' macro clash)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/posixPath.h"

// The interface carries the Linux AT_FDCWD (-100); other UNIX systems number it
// differently, so it is mapped to the host value there (identity on Linux)
static int hostDirFd(int64 dirfd)
{
    if (dirfd == -100)
    {
        return AT_FDCWD;
    }
    return (int)dirfd;
}

// The *at() flags also carry the Linux values (AT_SYMLINK_NOFOLLOW 0x100,
// AT_SYMLINK_FOLLOW 0x400, and 0x200 for AT_REMOVEDIR/AT_EACCESS). 'flag200'
// is the host flag that 0x200 stands for in the calling function (or 0).
static int hostAtFlags(int flags, int flag200)
{
#ifdef __linux__
    (void)flag200;
    return flags;
#else
    int host = 0;
    if (flags & 0x100) host |= AT_SYMLINK_NOFOLLOW;
    if (flags & 0x400) host |= AT_SYMLINK_FOLLOW;
    if (flags & 0x200) host |= flag200;
    return host;
#endif
}

// Converts a libc result (-1 with errno set) into the cPosixPath convention
static int64 posixResult(long ret)
{
    if (ret < 0)
    {
        return -(int64)errno;
    }
    return (int64)ret;
}

int64 cPosixPath::fstatAt(int64 dirfd, const char* path, int flags,
                          cPosixStat& stat)
{
    struct stat st;
    memset(&st, 0, sizeof(st));
    int ret = fstatat(hostDirFd(dirfd), path, &st, hostAtFlags(flags, 0));
    if (ret < 0)
    {
        return -(int64)errno;
    }
    stat.dev = (uint64)st.st_dev;
    stat.ino = (uint64)st.st_ino;
    stat.nlink = (uint64)st.st_nlink;
    stat.rdev = (uint64)st.st_rdev;
    stat.mode = (uint32)st.st_mode;
    stat.uid = (uint32)st.st_uid;
    stat.gid = (uint32)st.st_gid;
    stat.size = (int64)st.st_size;
    stat.blksize = (int64)st.st_blksize;
    stat.blocks = (int64)st.st_blocks;
#ifdef __APPLE__
    stat.atimeSec = (int64)st.st_atimespec.tv_sec;
    stat.atimeNsec = (int64)st.st_atimespec.tv_nsec;
    stat.mtimeSec = (int64)st.st_mtimespec.tv_sec;
    stat.mtimeNsec = (int64)st.st_mtimespec.tv_nsec;
    stat.ctimeSec = (int64)st.st_ctimespec.tv_sec;
    stat.ctimeNsec = (int64)st.st_ctimespec.tv_nsec;
#else
    stat.atimeSec = (int64)st.st_atim.tv_sec;
    stat.atimeNsec = (int64)st.st_atim.tv_nsec;
    stat.mtimeSec = (int64)st.st_mtim.tv_sec;
    stat.mtimeNsec = (int64)st.st_mtim.tv_nsec;
    stat.ctimeSec = (int64)st.st_ctim.tv_sec;
    stat.ctimeNsec = (int64)st.st_ctim.tv_nsec;
#endif
    return 0;
}

int64 cPosixPath::statx(int64 dirfd, const char* path, int flags, uint32 mask,
                        void* buf)
{
#ifdef __linux__
    // The raw call: no libc wrapper is needed, and buf is the kernel's struct
    return posixResult(syscall(SYS_statx, hostDirFd(dirfd), path, flags,
                               (unsigned int)mask, buf));
#else
    (void)dirfd; (void)path; (void)flags; (void)mask; (void)buf;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixPath::faccessAt(int64 dirfd, const char* path, int mode, int flags)
{
    return posixResult(faccessat(hostDirFd(dirfd), path, mode, hostAtFlags(flags, AT_EACCESS)));
}

int64 cPosixPath::unlinkAt(int64 dirfd, const char* path, int flags)
{
    return posixResult(unlinkat(hostDirFd(dirfd), path, hostAtFlags(flags, AT_REMOVEDIR)));
}

int64 cPosixPath::mkdirAt(int64 dirfd, const char* path, uint32 mode)
{
    return posixResult(mkdirat(hostDirFd(dirfd), path, (mode_t)mode));
}

int64 cPosixPath::renameAt2(int64 oldDirfd, const char* oldPath, int64 newDirfd,
                            const char* newPath, uint32 flags)
{
#if defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 28))
    return posixResult(renameat2(hostDirFd(oldDirfd), oldPath, hostDirFd(newDirfd),
                                 newPath, (unsigned int)flags));
#elif defined(__linux__)
    return posixResult(syscall(SYS_renameat2, hostDirFd(oldDirfd), oldPath,
                               hostDirFd(newDirfd), newPath, (unsigned int)flags));
#else
    (void)oldDirfd; (void)oldPath; (void)newDirfd; (void)newPath; (void)flags;
    return -(int64)ENOSYS;
#endif
}

int64 cPosixPath::linkAt(int64 oldDirfd, const char* oldPath, int64 newDirfd,
                         const char* newPath, int flags)
{
    return posixResult(linkat(hostDirFd(oldDirfd), oldPath, hostDirFd(newDirfd), newPath,
                              hostAtFlags(flags, 0)));
}

int64 cPosixPath::symlinkAt(const char* target, int64 newDirfd,
                            const char* linkPath)
{
    return posixResult(symlinkat(target, hostDirFd(newDirfd), linkPath));
}

int64 cPosixPath::readlinkAt(int64 dirfd, const char* path, char* buf,
                             uint64 len)
{
    return posixResult((long)readlinkat(hostDirFd(dirfd), path, buf, (size_t)len));
}

int64 cPosixPath::getcwd(char* buf, uint64 len)
{
    // getcwd(NULL, 0) would allocate; callers always pass their own buffer
    if (buf == NULL || len == 0)
    {
        return -(int64)EINVAL;
    }
    if (::getcwd(buf, (size_t)len) == NULL)
    {
        return -(int64)errno;
    }
    return (int64)strlen(buf) + 1;
}

int64 cPosixPath::chdir(const char* path)
{
    return posixResult(::chdir(path));
}

int64 cPosixPath::fchdir(int64 fd)
{
    return posixResult(::fchdir((int)fd));
}

int64 cPosixPath::umask(uint32 mask)
{
    return (int64)::umask((mode_t)mask);
}

int64 cPosixPath::utimensAt(int64 dirfd, const char* path, const int64* times,
                            int flags)
{
    struct timespec ts[2];
    struct timespec* request = NULL;
    if (times != NULL)
    {
        ts[0].tv_sec = (time_t)times[0];
        ts[0].tv_nsec = (long)times[1];
        ts[1].tv_sec = (time_t)times[2];
        ts[1].tv_nsec = (long)times[3];
        request = ts;
    }
    return posixResult(utimensat(hostDirFd(dirfd), path, request, hostAtFlags(flags, 0)));
}

int64 cPosixPath::fchmodAt(int64 dirfd, const char* path, uint32 mode, int flags)
{
    return posixResult(fchmodat(hostDirFd(dirfd), path, (mode_t)mode, hostAtFlags(flags, 0)));
}

int64 cPosixPath::getdents64(int64 fd, void* buf, uint64 len)
{
    // Raw call: glibc only gained a wrapper in 2.30 and the records are the kernel's
#ifdef __linux__
    return posixResult(syscall(SYS_getdents64, (int)fd, buf, (size_t)len));
#else
    (void)fd; (void)buf; (void)len;
    return -(int64)ENOSYS;
#endif
}
