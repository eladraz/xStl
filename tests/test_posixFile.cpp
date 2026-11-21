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
 * test_posixFile.cpp
 *
 * File-descriptor services of cPosixFile (Unix only; Windows throws). Every
 * call returns >= 0 or -errno; the files live under TMPDIR (or /tmp) and are
 * removed by the test.
 */

// System headers come first: xStl/types.h defines the 'uint' macro.
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#define POSIX_FILE_TEST_UNIX (0)
#else
#define POSIX_FILE_TEST_UNIX (1)
// dup3, pipe2, fdatasync and ppoll exist on Linux only; elsewhere they -ENOSYS
#if defined(__linux__)
#define POSIX_FILE_TEST_LINUX (1)
#else
#define POSIX_FILE_TEST_LINUX (0)
#endif
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <sys/file.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/posixFile.h"
#include "tests.h"

#define POSIX_FILE_TEST_PAYLOAD "hello world"
#define POSIX_FILE_TEST_PAYLOAD_LEN (11)

#if POSIX_FILE_TEST_UNIX

namespace {

// A unique path in the temporary directory; the file is removed on scope exit
class cTempPath {
public:
    cTempPath()
    {
        const char* dir = getenv("TMPDIR");
        if (dir == NULL || dir[0] == 0)
        {
            dir = "/tmp";
        }
        snprintf(m_path, sizeof(m_path), "%s/xstl_posixfile_%llu_%d.dat", dir,
                 (unsigned long long)cOS::getMonotonicNanoseconds(),
                 (int)getpid());
    }
    ~cTempPath() { unlink(m_path); }
    const char* get() const { return m_path; }

private:
    char m_path[512];
};

} // namespace

#endif

class cTestPosixFile : public cTestObject {
public:
#if POSIX_FILE_TEST_UNIX
    // Create, write, read back at offsets, truncate, close; EBADF after close
    void test_fileRoundTrip()
    {
        cTempPath path;
        TESTS_ASSERT(cPosixFile::openAt(AT_FDCWD, path.get(), O_RDONLY, 0) ==
                     -ENOENT);

        int64 fd = cPosixFile::openAt(AT_FDCWD, path.get(),
                                      O_CREAT | O_RDWR | O_TRUNC, 0600);
        TESTS_ASSERT(fd >= 0);
        TESTS_ASSERT(cPosixFile::write(fd, POSIX_FILE_TEST_PAYLOAD,
                                       POSIX_FILE_TEST_PAYLOAD_LEN) ==
                     POSIX_FILE_TEST_PAYLOAD_LEN);
        // A write leaves the offset at the end of the data
        TESTS_ASSERT(cPosixFile::lseek(fd, 0, SEEK_CUR) ==
                     POSIX_FILE_TEST_PAYLOAD_LEN);

        char buffer[16];
        memset(buffer, 0, sizeof(buffer));
        TESTS_ASSERT(cPosixFile::pread(fd, buffer, 5, 6) == 5);
        TESTS_ASSERT(memcmp(buffer, "world", 5) == 0);

        TESTS_ASSERT(cPosixFile::lseek(fd, 0, SEEK_SET) == 0);
        TESTS_ASSERT(cPosixFile::read(fd, buffer, 5) == 5);
        TESTS_ASSERT(memcmp(buffer, "hello", 5) == 0);
        TESTS_ASSERT(cPosixFile::lseek(fd, 0, SEEK_END) ==
                     POSIX_FILE_TEST_PAYLOAD_LEN);

        TESTS_ASSERT(cPosixFile::pwrite(fd, "W", 1, 6) == 1);
        TESTS_ASSERT(cPosixFile::ftruncate(fd, 8) == 0);
        TESTS_ASSERT(cPosixFile::fsync(fd) == 0);
#if POSIX_FILE_TEST_LINUX
        TESTS_ASSERT(cPosixFile::fdatasync(fd) == 0);
#else
        TESTS_ASSERT(cPosixFile::fdatasync(fd) == -ENOSYS);
#endif
        TESTS_ASSERT(cPosixFile::lseek(fd, 0, SEEK_END) == 8);

        TESTS_ASSERT(cPosixFile::close(fd) == 0);
        TESTS_ASSERT(cPosixFile::close(fd) == -EBADF);
    }

    // close and read on a descriptor that is not open
    void test_badFileDescriptor()
    {
        char byte = 0;
        TESTS_ASSERT(cPosixFile::close(-1) == -EBADF);
        TESTS_ASSERT(cPosixFile::read(-1, &byte, 1) == -EBADF);
    }

#if POSIX_FILE_TEST_LINUX
    // writev of three iovecs, read back in order; readv scatters the same bytes
    void test_pipeVectors()
    {
        int fds[2] = { -1, -1 };
        TESTS_ASSERT(cPosixFile::pipe2(fds, O_CLOEXEC) == 0);

        char a[] = "ab";
        char b[] = "cde";
        char c[] = "f";
        cPosixIoVec out[3] = { { a, 2 }, { b, 3 }, { c, 1 } };
        TESTS_ASSERT(cPosixFile::writev(fds[1], out, 3) == 6);

        char buffer[8];
        memset(buffer, 0, sizeof(buffer));
        TESTS_ASSERT(cPosixFile::read(fds[0], buffer, 6) == 6);
        TESTS_ASSERT(memcmp(buffer, "abcdef", 6) == 0);

        TESTS_ASSERT(cPosixFile::writev(fds[1], out, 3) == 6);
        char x[2];
        char y[3];
        char z[1];
        cPosixIoVec in[3] = { { x, 2 }, { y, 3 }, { z, 1 } };
        TESTS_ASSERT(cPosixFile::readv(fds[0], in, 3) == 6);
        TESTS_ASSERT(memcmp(x, "ab", 2) == 0);
        TESTS_ASSERT(memcmp(y, "cde", 3) == 0);
        TESTS_ASSERT(z[0] == 'f');

        TESTS_ASSERT(cPosixFile::readv(fds[0], in, -1) == -EINVAL);
        TESTS_ASSERT(cPosixFile::writev(fds[1], out,
                                        cPosixFile::MAX_IOVECS + 1) == -EINVAL);

        TESTS_ASSERT(cPosixFile::close(fds[0]) == 0);
        TESTS_ASSERT(cPosixFile::close(fds[1]) == 0);
    }

    // dup returns a new descriptor for the same pipe
    void test_dup()
    {
        int fds[2] = { -1, -1 };
        TESTS_ASSERT(cPosixFile::pipe2(fds, 0) == 0);

        int64 copy = cPosixFile::dup(fds[1]);
        TESTS_ASSERT(copy >= 0);
        TESTS_ASSERT(copy != fds[1]);
        TESTS_ASSERT(cPosixFile::write(copy, "d", 1) == 1);
        char byte = 0;
        TESTS_ASSERT(cPosixFile::read(fds[0], &byte, 1) == 1);
        TESTS_ASSERT(byte == 'd');
        TESTS_ASSERT(cPosixFile::close(copy) == 0);

        TESTS_ASSERT(cPosixFile::close(fds[0]) == 0);
        TESTS_ASSERT(cPosixFile::close(fds[1]) == 0);
    }

    // dup3 to a free descriptor: O_CLOEXEC becomes FD_CLOEXEC, flags 0 clears it
    void test_dup3CloseOnExec()
    {
        int fds[2] = { -1, -1 };
        TESTS_ASSERT(cPosixFile::pipe2(fds, 0) == 0);

        // Pick a descriptor that is free right now: never clobber a live one
        int64 target = cPosixFile::fcntlInt(fds[1], F_DUPFD, 100);
        TESTS_ASSERT(target >= 100);
        TESTS_ASSERT(target != fds[1]);
        TESTS_ASSERT(cPosixFile::close(target) == 0);

        TESTS_ASSERT(cPosixFile::dup3(fds[1], (int)target, O_CLOEXEC) == target);
        int64 flags = cPosixFile::fcntlInt((int)target, F_GETFD, 0);
        TESTS_ASSERT(flags >= 0);
        TESTS_ASSERT((flags & FD_CLOEXEC) != 0);
        TESTS_ASSERT(cPosixFile::close((int)target) == 0);

        TESTS_ASSERT(cPosixFile::dup3(fds[1], (int)target, 0) == target);
        flags = cPosixFile::fcntlInt((int)target, F_GETFD, 0);
        TESTS_ASSERT(flags >= 0);
        TESTS_ASSERT((flags & FD_CLOEXEC) == 0);
        TESTS_ASSERT(cPosixFile::close((int)target) == 0);

        TESTS_ASSERT(cPosixFile::close(fds[0]) == 0);
        TESTS_ASSERT(cPosixFile::close(fds[1]) == 0);
    }

    // F_GETFL/F_SETFL: O_NONBLOCK on an empty pipe makes read return EAGAIN
    void test_fcntlStatusFlags()
    {
        int fds[2] = { -1, -1 };
        TESTS_ASSERT(cPosixFile::pipe2(fds, 0) == 0);

        int64 flags = cPosixFile::fcntlInt(fds[0], F_GETFL, 0);
        TESTS_ASSERT(flags >= 0);
        TESTS_ASSERT((flags & O_NONBLOCK) == 0);
        TESTS_ASSERT(cPosixFile::fcntlInt(fds[0], F_SETFL,
                                          flags | O_NONBLOCK) == 0);
        flags = cPosixFile::fcntlInt(fds[0], F_GETFL, 0);
        TESTS_ASSERT(flags >= 0);
        TESTS_ASSERT((flags & O_NONBLOCK) != 0);

        char byte = 0;
        TESTS_ASSERT(cPosixFile::read(fds[0], &byte, 1) == -EAGAIN);

        TESTS_ASSERT(cPosixFile::close(fds[0]) == 0);
        TESTS_ASSERT(cPosixFile::close(fds[1]) == 0);
    }

    // F_GETLK and F_SETLK/F_UNLCK on a regular file, no conflicting lock held
    void test_fcntlLock()
    {
        cTempPath path;
        int64 fd = cPosixFile::openAt(AT_FDCWD, path.get(),
                                      O_CREAT | O_RDWR | O_TRUNC, 0600);
        TESTS_ASSERT(fd >= 0);
        TESTS_ASSERT(cPosixFile::write(fd, POSIX_FILE_TEST_PAYLOAD,
                                       POSIX_FILE_TEST_PAYLOAD_LEN) ==
                     POSIX_FILE_TEST_PAYLOAD_LEN);

        struct flock lock;
        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        TESTS_ASSERT(cPosixFile::fcntlLock((int)fd, F_GETLK, &lock) == 0);
        TESTS_ASSERT(lock.l_type == F_UNLCK);

        lock.l_type = F_WRLCK;
        TESTS_ASSERT(cPosixFile::fcntlLock((int)fd, F_SETLK, &lock) == 0);
        lock.l_type = F_UNLCK;
        TESTS_ASSERT(cPosixFile::fcntlLock((int)fd, F_SETLK, &lock) == 0);

        TESTS_ASSERT(cPosixFile::close(fd) == 0);
    }

    // TIOCGWINSZ on a pipe is ENOTTY; ppoll sees data after a write
    void test_ioctlAndPoll()
    {
        int fds[2] = { -1, -1 };
        TESTS_ASSERT(cPosixFile::pipe2(fds, 0) == 0);

        struct winsize size;
        memset(&size, 0, sizeof(size));
        TESTS_ASSERT(cPosixFile::ioctlBuffer(fds[0], TIOCGWINSZ, &size) ==
                     -ENOTTY);

        struct pollfd entry;
        entry.fd = fds[0];
        entry.events = POLLIN;
        entry.revents = 0;
        int64 noWait = 0;
        TESTS_ASSERT(cPosixFile::ppoll(&entry, 1, &noWait) == 0);

        TESTS_ASSERT(cPosixFile::write(fds[1], "x", 1) == 1);
        entry.revents = 0;
        TESTS_ASSERT(cPosixFile::ppoll(&entry, 1, &noWait) == 1);
        TESTS_ASSERT((entry.revents & POLLIN) != 0);

        int64 negative = -1;
        TESTS_ASSERT(cPosixFile::ppoll(&entry, 1, &negative) == -EINVAL);

        TESTS_ASSERT(cPosixFile::close(fds[0]) == 0);
        TESTS_ASSERT(cPosixFile::close(fds[1]) == 0);
    }

#endif
    // flock: an exclusive lock on one open file refuses other open files
    // (EWOULDBLOCK), shared locks coexist, LOCK_UN releases, no operation is EINVAL
    void test_flock()
    {
        cTempPath path;
        int64 fd = cPosixFile::openAt(AT_FDCWD, path.get(),
                                      O_CREAT | O_RDWR | O_TRUNC, 0600);
        TESTS_ASSERT(fd >= 0);
        int64 other = cPosixFile::openAt(AT_FDCWD, path.get(), O_RDWR, 0);
        TESTS_ASSERT(other >= 0);

        TESTS_ASSERT(cPosixFile::flock((int)fd, LOCK_EX | LOCK_NB) == 0);
        TESTS_ASSERT(cPosixFile::flock((int)other, LOCK_SH | LOCK_NB) ==
                     -EWOULDBLOCK);
        TESTS_ASSERT(cPosixFile::flock((int)fd, LOCK_UN) == 0);

        TESTS_ASSERT(cPosixFile::flock((int)other, LOCK_SH | LOCK_NB) == 0);
        TESTS_ASSERT(cPosixFile::flock((int)fd, LOCK_SH | LOCK_NB) == 0);
        TESTS_ASSERT(cPosixFile::flock((int)fd, LOCK_EX | LOCK_NB) ==
                     -EWOULDBLOCK);
        TESTS_ASSERT(cPosixFile::flock((int)other, LOCK_UN) == 0);
        TESTS_ASSERT(cPosixFile::flock((int)fd, LOCK_EX | LOCK_NB) == 0);
        TESTS_ASSERT(cPosixFile::flock((int)fd, LOCK_UN) == 0);

        TESTS_ASSERT(cPosixFile::flock((int)fd, 0) == -EINVAL);
        TESTS_ASSERT(cPosixFile::flock(-1, LOCK_EX) == -EBADF);

        TESTS_ASSERT(cPosixFile::close(other) == 0);
        TESTS_ASSERT(cPosixFile::close(fd) == 0);
    }

#if POSIX_FILE_TEST_LINUX
    // fallocate: mode 0 grows the file to offset+length keeping the data,
    // FALLOC_FL_KEEP_SIZE allocates without changing the size; bad arguments EINVAL
    void test_fallocate()
    {
        cTempPath path;
        int64 fd = cPosixFile::openAt(AT_FDCWD, path.get(),
                                      O_CREAT | O_RDWR | O_TRUNC, 0600);
        TESTS_ASSERT(fd >= 0);
        TESTS_ASSERT(cPosixFile::write(fd, POSIX_FILE_TEST_PAYLOAD,
                                       POSIX_FILE_TEST_PAYLOAD_LEN) ==
                     POSIX_FILE_TEST_PAYLOAD_LEN);

        TESTS_ASSERT(cPosixFile::fallocate((int)fd, 0, 0, 0) == -EINVAL);
        TESTS_ASSERT(cPosixFile::fallocate((int)fd, 0, -1, 16) == -EINVAL);
        TESTS_ASSERT(cPosixFile::fallocate((int)fd, 0, 0, 4096) == 0);
        TESTS_ASSERT(cPosixFile::lseek(fd, 0, SEEK_END) == 4096);

        char buffer[16];
        memset(buffer, 0, sizeof(buffer));
        TESTS_ASSERT(cPosixFile::pread(fd, buffer, 5, 6) == 5);
        TESTS_ASSERT(memcmp(buffer, "world", 5) == 0);

        TESTS_ASSERT(cPosixFile::fallocate((int)fd, FALLOC_FL_KEEP_SIZE, 0,
                                           8192) == 0);
        TESTS_ASSERT(cPosixFile::lseek(fd, 0, SEEK_END) == 4096);

        TESTS_ASSERT(cPosixFile::fallocate(-1, 0, 0, 16) == -EBADF);
        TESTS_ASSERT(cPosixFile::close(fd) == 0);
    }

    // fadvise: valid advice is accepted; bad advice and a bad descriptor are
    // errors; a pipe is ESPIPE
    void test_fadvise()
    {
        cTempPath path;
        int64 fd = cPosixFile::openAt(AT_FDCWD, path.get(),
                                      O_CREAT | O_RDWR | O_TRUNC, 0600);
        TESTS_ASSERT(fd >= 0);
        TESTS_ASSERT(cPosixFile::write(fd, POSIX_FILE_TEST_PAYLOAD,
                                       POSIX_FILE_TEST_PAYLOAD_LEN) ==
                     POSIX_FILE_TEST_PAYLOAD_LEN);
        TESTS_ASSERT(cPosixFile::fadvise((int)fd, 0, 0,
                                         POSIX_FADV_SEQUENTIAL) == 0);
        TESTS_ASSERT(cPosixFile::fadvise((int)fd, 0, 0,
                                         POSIX_FADV_DONTNEED) == 0);
        TESTS_ASSERT(cPosixFile::fadvise((int)fd, 0, 0, 99) == -EINVAL);
        TESTS_ASSERT(cPosixFile::fadvise(-1, 0, 0, POSIX_FADV_NORMAL) ==
                     -EBADF);
        TESTS_ASSERT(cPosixFile::close(fd) == 0);

        int fds[2] = { -1, -1 };
        TESTS_ASSERT(cPosixFile::pipe2(fds, 0) == 0);
        TESTS_ASSERT(cPosixFile::fadvise(fds[0], 0, 0, POSIX_FADV_NORMAL) ==
                     -ESPIPE);
        TESTS_ASSERT(cPosixFile::close(fds[0]) == 0);
        TESTS_ASSERT(cPosixFile::close(fds[1]) == 0);
    }
#endif
#if !POSIX_FILE_TEST_LINUX
    // The Linux-only services report ENOSYS on other UNIX systems
    void test_enosysOnOtherUnix()
    {
        int fds[2] = { -1, -1 };
        int64 timeout = 0;
        char byte = 0;
        TESTS_ASSERT(cPosixFile::pipe2(fds, 0) == -ENOSYS);
        TESTS_ASSERT(cPosixFile::dup3(0, 1, 0) == -ENOSYS);
        TESTS_ASSERT(cPosixFile::ppoll(&byte, 0, &timeout) == -ENOSYS);
        TESTS_ASSERT(cPosixFile::fallocate(0, 0, 0, 1) == -ENOSYS);
        TESTS_ASSERT(cPosixFile::fadvise(0, 0, 0, 0) == -ENOSYS);
    }
#endif
#else
    // Windows has none of these services: every call reports it
    void test_unsupportedOnWindows()
    {
        char byte = 0;
        cPosixIoVec iov = { &byte, 1 };
        int fds[2] = { 0, 0 };
        int64 timeout = 0;
        TESTS_EXCEPTION(cPosixFile::read(0, &byte, 1));
        TESTS_EXCEPTION(cPosixFile::write(1, &byte, 1));
        TESTS_EXCEPTION(cPosixFile::openAt(0, "x", 0, 0));
        TESTS_EXCEPTION(cPosixFile::close(0));
        TESTS_EXCEPTION(cPosixFile::readv(0, &iov, 1));
        TESTS_EXCEPTION(cPosixFile::pipe2(fds, 0));
        TESTS_EXCEPTION(cPosixFile::ppoll(&byte, 0, &timeout));
        TESTS_EXCEPTION(cPosixFile::fallocate(0, 0, 0, 1));
        TESTS_EXCEPTION(cPosixFile::fadvise(0, 0, 0, 0));
        TESTS_EXCEPTION(cPosixFile::flock(0, 0));
    }
#endif

    // Perform the test
    virtual void test()
    {
#if POSIX_FILE_TEST_UNIX
        test_fileRoundTrip();
        test_badFileDescriptor();
#if POSIX_FILE_TEST_LINUX
        test_pipeVectors();
        test_dup();
        test_dup3CloseOnExec();
        test_fcntlStatusFlags();
        test_fcntlLock();
        test_ioctlAndPoll();
#endif
        test_flock();
#if POSIX_FILE_TEST_LINUX
        test_fallocate();
        test_fadvise();
#else
        test_enosysOnOtherUnix();
#endif
#else
        test_unsupportedOnWindows();
#endif
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestPosixFile g_globalTestPosixFile;
