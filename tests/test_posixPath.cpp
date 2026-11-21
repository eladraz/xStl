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
 * test_posixPath.cpp
 *
 * cPosixPath: the *at() family, stat, statx, getdents64 and the cwd/umask
 * calls. Every case works in a fresh directory created under the current
 * working directory and removes it again. Windows throws on every call.
 */

// System headers come first: xStl/types.h defines the 'uint' macro.
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#define POSIX_PATH_TEST_UNIX (0)
#else
#define POSIX_PATH_TEST_UNIX (1)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#ifdef __linux__
#include <sys/vfs.h>
#endif
#endif

#include "xStl/types.h"
#include "xStl/os/posixPath.h"
#include "tests.h"

// AT_FDCWD and the flags the tests pass
#define POSIX_PATH_TEST_CWD (-100)
#define POSIX_PATH_TEST_REMOVEDIR (0x200)
#define POSIX_PATH_TEST_RENAME_NOREPLACE (1)
#define POSIX_PATH_TEST_STATX_SIZE (0x200)
#define POSIX_PATH_TEST_STATX_BASIC (0x7ff)
#define POSIX_PATH_TEST_STATX_BUF (256)
#define POSIX_PATH_TEST_FILE_SIZE (4097)
#define POSIX_PATH_TEST_MTIME (1000000000LL)
#define POSIX_PATH_TEST_STATFS_BYTES (120)

#if POSIX_PATH_TEST_UNIX

// Creates a regular file of the given size with the libc, for the stat cases
static void createFile(const char* name, uint size)
{
    int fd = open(name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    TESTS_ASSERT(fd >= 0);
    char block[256];
    memset(block, 'x', sizeof(block));
    uint left = size;
    while (left > 0)
    {
        uint chunk = left < sizeof(block) ? left : sizeof(block);
        TESTS_ASSERT(write(fd, block, chunk) == (ssize_t)chunk);
        left -= chunk;
    }
    TESTS_ASSERT(close(fd) == 0);
}

// Counts the entries of a directory through getdents64, returning -1 on error
static int countDirEntries(int64 fd, bool& hasDot, bool& hasDotDot,
                           bool& hasA, bool& hasB)
{
    char buf[1024];
    int count = 0;
    hasDot = hasDotDot = hasA = hasB = false;
    for (;;)
    {
        int64 got = cPosixPath::getdents64(fd, buf, sizeof(buf));
        if (got < 0)
        {
            return -1;
        }
        if (got == 0)
        {
            break;
        }
        uint64 off = 0;
        while (off < (uint64)got)
        {
            // struct linux_dirent64: u64 ino, i64 off, u16 reclen, u8 type, name
            const unsigned char* rec = (const unsigned char*)buf + off;
            unsigned short reclen = (unsigned short)(rec[16] | (rec[17] << 8));
            const char* name = (const char*)(rec + 19);
            if (strcmp(name, ".") == 0) hasDot = true;
            else if (strcmp(name, "..") == 0) hasDotDot = true;
            else if (strcmp(name, "a.txt") == 0) hasA = true;
            else if (strcmp(name, "link") == 0) hasB = true;
            count++;
            off += reclen;
        }
    }
    return count;
}

#endif

class cTestPosixPath : public cTestObject {
public:
    void test_directoryAndStat()
    {
#if POSIX_PATH_TEST_UNIX
        TESTS_ASSERT(cPosixPath::mkdirAt(POSIX_PATH_TEST_CWD, "pp_dir", 0755) == 0);
        // A second create reports EEXIST as a negative errno
        TESTS_ASSERT(cPosixPath::mkdirAt(POSIX_PATH_TEST_CWD, "pp_dir", 0755) == -EEXIST);

        cPosixStat st;
        memset(&st, 0, sizeof(st));
        TESTS_ASSERT(cPosixPath::fstatAt(POSIX_PATH_TEST_CWD, "pp_dir", 0, st) == 0);
        TESTS_ASSERT(S_ISDIR(st.mode));
        TESTS_ASSERT(st.nlink >= 2);

        // Missing names report ENOENT
        TESTS_ASSERT(cPosixPath::fstatAt(POSIX_PATH_TEST_CWD, "pp_missing", 0, st) == -ENOENT);

#if defined(__linux__)
        // AT_EMPTY_PATH on an open descriptor is fstat
        int dirfd = open("pp_dir", O_RDONLY | O_DIRECTORY);
        TESTS_ASSERT(dirfd >= 0);
        cPosixStat viaFd;
        TESTS_ASSERT(cPosixPath::fstatAt(dirfd, "", AT_EMPTY_PATH, viaFd) == 0);
        TESTS_ASSERT(viaFd.ino == st.ino);
        TESTS_ASSERT(close(dirfd) == 0);
#endif

        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_dir", POSIX_PATH_TEST_REMOVEDIR) == 0);
        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_dir", POSIX_PATH_TEST_REMOVEDIR) == -ENOENT);
#else
        TESTS_EXCEPTION(cPosixPath::mkdirAt(POSIX_PATH_TEST_CWD, "pp_dir", 0755));
        cPosixStat st;
        TESTS_EXCEPTION(cPosixPath::fstatAt(POSIX_PATH_TEST_CWD, ".", 0, st));
#endif
    }

    void test_fileSizeAndStatx()
    {
#if POSIX_PATH_TEST_UNIX
        createFile("pp_file", POSIX_PATH_TEST_FILE_SIZE);

        cPosixStat st;
        memset(&st, 0, sizeof(st));
        TESTS_ASSERT(cPosixPath::fstatAt(POSIX_PATH_TEST_CWD, "pp_file", 0, st) == 0);
        TESTS_ASSERT(S_ISREG(st.mode));
        TESTS_ASSERT(st.size == POSIX_PATH_TEST_FILE_SIZE);

#if defined(__linux__)
        // statx fills the kernel record; stx_mask is its first u32
        unsigned char sx[POSIX_PATH_TEST_STATX_BUF];
        memset(sx, 0, sizeof(sx));
        TESTS_ASSERT(cPosixPath::statx(POSIX_PATH_TEST_CWD, "pp_file", 0,
                                       POSIX_PATH_TEST_STATX_BASIC, sx) == 0);
        uint32 mask = (uint32)sx[0] | ((uint32)sx[1] << 8) |
                      ((uint32)sx[2] << 16) | ((uint32)sx[3] << 24);
        TESTS_ASSERT((mask & POSIX_PATH_TEST_STATX_SIZE) != 0);
        // stx_size sits at offset 40 (u64)
        uint64 sxSize = 0;
        for (uint i = 0; i < 8; i++)
        {
            sxSize |= (uint64)sx[40 + i] << (8 * i);
        }
        TESTS_ASSERT(sxSize == POSIX_PATH_TEST_FILE_SIZE);
#else
        unsigned char sx[POSIX_PATH_TEST_STATX_BUF];
        TESTS_ASSERT(cPosixPath::statx(POSIX_PATH_TEST_CWD, "pp_file", 0,
                                       POSIX_PATH_TEST_STATX_BASIC, sx) == -ENOSYS);
#endif

        // Times: utimensAt sets the modification time, fstatAt reads it back
        int64 times[4] = { POSIX_PATH_TEST_MTIME, 0,
                           POSIX_PATH_TEST_MTIME, 500 };
        TESTS_ASSERT(cPosixPath::utimensAt(POSIX_PATH_TEST_CWD, "pp_file", times, 0) == 0);
        TESTS_ASSERT(cPosixPath::fstatAt(POSIX_PATH_TEST_CWD, "pp_file", 0, st) == 0);
        TESTS_ASSERT(st.mtimeSec == POSIX_PATH_TEST_MTIME);
        TESTS_ASSERT(st.mtimeNsec == 500);

        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_file", 0) == 0);
#else
        cPosixStat st;
        TESTS_EXCEPTION(cPosixPath::fstatAt(POSIX_PATH_TEST_CWD, "x", 0, st));
#endif
    }

    void test_renameAndLinks()
    {
#if POSIX_PATH_TEST_UNIX
        createFile("pp_a", 1);
        createFile("pp_b", 1);

#if defined(__linux__)
        // RENAME_NOREPLACE refuses an existing target
        TESTS_ASSERT(cPosixPath::renameAt2(POSIX_PATH_TEST_CWD, "pp_a",
                                           POSIX_PATH_TEST_CWD, "pp_b",
                                           POSIX_PATH_TEST_RENAME_NOREPLACE) == -EEXIST);
        TESTS_ASSERT(cPosixPath::renameAt2(POSIX_PATH_TEST_CWD, "pp_a",
                                           POSIX_PATH_TEST_CWD, "pp_c",
                                           POSIX_PATH_TEST_RENAME_NOREPLACE) == 0);
#else
        TESTS_ASSERT(cPosixPath::renameAt2(POSIX_PATH_TEST_CWD, "pp_a",
                                           POSIX_PATH_TEST_CWD, "pp_c",
                                           0) == -ENOSYS);
        TESTS_ASSERT(rename("pp_a", "pp_c") == 0);
#endif

        // Symbolic link: readlinkAt returns the target without a NUL
        TESTS_ASSERT(cPosixPath::symlinkAt("pp_c", POSIX_PATH_TEST_CWD, "pp_link") == 0);
        char target[64];
        int64 n = cPosixPath::readlinkAt(POSIX_PATH_TEST_CWD, "pp_link", target, sizeof(target));
        TESTS_ASSERT(n == 4);
        TESTS_ASSERT(memcmp(target, "pp_c", 4) == 0);

        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_link", 0) == 0);
        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_c", 0) == 0);
        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_b", 0) == 0);
#else
        char buf[16];
        TESTS_EXCEPTION(cPosixPath::readlinkAt(POSIX_PATH_TEST_CWD, "x", buf, sizeof(buf)));
#endif
    }

    void test_directoryListing()
    {
#if POSIX_PATH_TEST_UNIX
        TESTS_ASSERT(cPosixPath::mkdirAt(POSIX_PATH_TEST_CWD, "pp_list", 0755) == 0);
        createFile("pp_list/a.txt", 0);
        TESTS_ASSERT(cPosixPath::symlinkAt("a.txt", POSIX_PATH_TEST_CWD, "pp_list/link") == 0);

#if defined(__linux__)
        int dirfd = open("pp_list", O_RDONLY | O_DIRECTORY);
        TESTS_ASSERT(dirfd >= 0);
        bool hasDot, hasDotDot, hasA, hasB;
        int count = countDirEntries(dirfd, hasDot, hasDotDot, hasA, hasB);
        TESTS_ASSERT(count == 4);
        TESTS_ASSERT(hasDot && hasDotDot && hasA && hasB);
        TESTS_ASSERT(close(dirfd) == 0);
#else
        char buf[64];
        TESTS_ASSERT(cPosixPath::getdents64(0, buf, sizeof(buf)) == -ENOSYS);
#endif

        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_list/link", 0) == 0);
        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_list/a.txt", 0) == 0);
        TESTS_ASSERT(cPosixPath::unlinkAt(POSIX_PATH_TEST_CWD, "pp_list", POSIX_PATH_TEST_REMOVEDIR) == 0);
#else
        char buf[64];
        TESTS_EXCEPTION(cPosixPath::getdents64(0, buf, sizeof(buf)));
#endif
    }

    void test_cwdAndUmask()
    {
#if POSIX_PATH_TEST_UNIX
        char cwd[4096];
        int64 len = cPosixPath::getcwd(cwd, sizeof(cwd));
        TESTS_ASSERT(len > 1);
        TESTS_ASSERT((uint64)len == strlen(cwd) + 1);

        // umask returns the previous mask; restore it afterwards
        int64 old = cPosixPath::umask(0022);
        TESTS_ASSERT(cPosixPath::umask((uint32)old) == 0022);

        TESTS_ASSERT(cPosixPath::chdir("/") == 0);
        TESTS_ASSERT(cPosixPath::chdir(cwd) == 0);
        TESTS_ASSERT(cPosixPath::chdir("pp_no_such_dir") == -ENOENT);
        TESTS_ASSERT(cPosixPath::faccessAt(POSIX_PATH_TEST_CWD, cwd, F_OK, 0) == 0);
        TESTS_ASSERT(cPosixPath::faccessAt(POSIX_PATH_TEST_CWD, "pp_no_such_name", F_OK, 0) == -ENOENT);
#else
        char cwd[16];
        TESTS_EXCEPTION(cPosixPath::getcwd(cwd, sizeof(cwd)));
        TESTS_EXCEPTION(cPosixPath::umask(0022));
#endif
    }

    // statfs and fstatfs fill the kernel's 120-byte record; f_type and f_bsize
    // are compared with the libc struct statfs of this host
    void test_statfs()
    {
#if POSIX_PATH_TEST_UNIX && !defined(__linux__)
        // The raw Linux statfs is not available on other UNIX systems
        unsigned char raw[POSIX_PATH_TEST_STATFS_BYTES];
        TESTS_ASSERT(cPosixPath::statfs(".", raw) == -ENOSYS);
        TESTS_ASSERT(cPosixPath::fstatfs(0, raw) == -ENOSYS);
#elif POSIX_PATH_TEST_UNIX
        unsigned char raw[POSIX_PATH_TEST_STATFS_BYTES];
        struct statfs host;
        memset(&host, 0, sizeof(host));
        TESTS_ASSERT(::statfs(".", &host) == 0);

        // f_type is the first long, f_bsize the second
        memset(raw, 0, sizeof(raw));
        TESTS_ASSERT(cPosixPath::statfs(".", raw) == 0);
        int64 rawType = 0;
        int64 rawBsize = 0;
        memcpy(&rawType, raw, sizeof(rawType));
        memcpy(&rawBsize, raw + sizeof(int64), sizeof(rawBsize));
        TESTS_ASSERT(rawType == (int64)host.f_type);
        TESTS_ASSERT(rawBsize == (int64)host.f_bsize);
        TESTS_ASSERT(rawBsize > 0);

        int fd = open(".", O_RDONLY);
        TESTS_ASSERT(fd >= 0);
        memset(raw, 0, sizeof(raw));
        TESTS_ASSERT(cPosixPath::fstatfs(fd, raw) == 0);
        memcpy(&rawType, raw, sizeof(rawType));
        TESTS_ASSERT(rawType == (int64)host.f_type);
        TESTS_ASSERT(close(fd) == 0);

        TESTS_ASSERT(cPosixPath::statfs("pp_no_such_statfs", raw) == -ENOENT);
        TESTS_ASSERT(cPosixPath::fstatfs(-1, raw) == -EBADF);
#else
        unsigned char raw[POSIX_PATH_TEST_STATFS_BYTES];
        TESTS_EXCEPTION(cPosixPath::statfs(".", raw));
        TESTS_EXCEPTION(cPosixPath::fstatfs(0, raw));
#endif
    }

    // Return the name of the module
    virtual void test()
    {
        test_directoryAndStat();
        test_fileSizeAndStatx();
        test_renameAndLinks();
        test_directoryListing();
        test_cwdAndUmask();
        test_statfs();
    }

    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestPosixPath g_globalTestPosixPath;
