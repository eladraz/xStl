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
 * test_virtualMemory.cpp
 *
 * Tests cVirtualMemory:
 *  - A 1 GiB reservation is zero-filled, and the first and last byte can be
 *    written and read back. Only the touched pages become resident (checked
 *    through /proc/self/statm on Linux).
 *  - Sizes are rounded up to whole pages, and a zero size is rejected.
 *  - protectNoAccess accepts a page-aligned range and rejects misaligned,
 *    empty and out-of-range requests.
 *  - mapShared and resetPrivate reject the same bad ranges on every platform.
 *  - On UNIX, a range made by mapShared is shared with a forked child (the
 *    child writes, the parent sees the byte after waitpid), and resetPrivate
 *    reads back as zero.
 */

// System headers come first: xStl/types.h defines the 'uint' macro, which
// breaks the system headers that are included after it.
#if defined(LINUX)
    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
#endif

#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/virtualMemory.h"
#include "xStl/data/string.h"
#include "tests.h"

#define VM_TEST_SIZE       (1024ULL * 1024ULL * 1024ULL)  // 1 GiB
#define VM_TEST_RSS_LIMIT  (64ULL * 1024ULL * 1024ULL)    // 64 MiB

class cTestVirtualMemory : public cTestObject {
public:
#if defined(XSTL_LINUX)
    // Resident set size of this process in bytes, or 0 if unknown
    static uint64 getResidentBytes()
    {
        FILE* f = fopen("/proc/self/statm", "r");
        if (f == NULL)
            return 0;
        unsigned long long sizePages = 0, residentPages = 0;
        int count = fscanf(f, "%llu %llu", &sizePages, &residentPages);
        fclose(f);
        if (count != 2)
            return 0;
        return residentPages * (uint64)sysconf(_SC_PAGESIZE);
    }
#endif

    // Reserve 1 GiB, check the zero fill, and touch the first and last byte
    void test_largeReservation()
    {
        uint64 pageSize = cVirtualMemory::getPageSize();
        TESTS_ASSERT(pageSize > 0);

        cVirtualMemory mem(VM_TEST_SIZE);
        TESTS_ASSERT(mem.getBase() != NULL);
        TESTS_ASSERT((uint64)mem.getBase() % pageSize == 0);
        TESTS_ASSERT_EQUAL(mem.getSize(), VM_TEST_SIZE);

#if defined(XSTL_LINUX)
        uint64 rssBefore = getResidentBytes();
#endif

        // Fresh memory is zero-filled
        TESTS_ASSERT_EQUAL(mem.getBase()[0], 0);
        TESTS_ASSERT_EQUAL(mem.getBase()[VM_TEST_SIZE - 1], 0);

        mem.getBase()[0] = 0xa5;
        mem.getBase()[VM_TEST_SIZE - 1] = 0x5a;
        TESTS_ASSERT_EQUAL(mem.getBase()[0], 0xa5);
        TESTS_ASSERT_EQUAL(mem.getBase()[VM_TEST_SIZE - 1], 0x5a);
        TESTS_ASSERT_EQUAL(mem.getBase()[pageSize], 0);

#if defined(XSTL_LINUX)
        uint64 rssAfter = getResidentBytes();
        if (rssBefore != 0 && rssAfter != 0)
        {
            TESTS_ASSERT(rssAfter >= rssBefore);
            TESTS_ASSERT(rssAfter - rssBefore < VM_TEST_RSS_LIMIT);
        }
#endif
    }

    // Sizes are rounded to whole pages, and an empty reservation is rejected
    void test_sizes()
    {
        uint64 pageSize = cVirtualMemory::getPageSize();

        cVirtualMemory tiny(1);
        TESTS_ASSERT_EQUAL(tiny.getSize(), pageSize);

        cVirtualMemory exact(pageSize);
        TESTS_ASSERT_EQUAL(exact.getSize(), pageSize);

        TESTS_EXCEPTION(cVirtualMemory zero(0));
    }

    // Argument validation of protectNoAccess
    void test_protectNoAccess()
    {
        uint64 pageSize = cVirtualMemory::getPageSize();
        cVirtualMemory mem(4 * pageSize);

        // Valid: the last page becomes a guard page
        mem.protectNoAccess(3 * pageSize, pageSize);

        // Misaligned offset, misaligned length, empty and out of range
        TESTS_EXCEPTION(mem.protectNoAccess(1, pageSize));
        TESTS_EXCEPTION(mem.protectNoAccess(0, pageSize + 1));
        TESTS_EXCEPTION(mem.protectNoAccess(0, 0));
        TESTS_EXCEPTION(mem.protectNoAccess(2 * pageSize, 3 * pageSize));
        TESTS_EXCEPTION(mem.protectNoAccess(5 * pageSize, pageSize));
    }

    // Argument validation of mapShared and resetPrivate. Both reject bad
    // ranges on every platform, so this runs everywhere.
    void test_sharedArguments()
    {
        uint64 pageSize = cVirtualMemory::getPageSize();
        cVirtualMemory mem(4 * pageSize);

        TESTS_EXCEPTION(mem.mapShared(1, pageSize));
        TESTS_EXCEPTION(mem.mapShared(0, pageSize + 1));
        TESTS_EXCEPTION(mem.mapShared(0, 0));
        TESTS_EXCEPTION(mem.mapShared(2 * pageSize, 3 * pageSize));
        TESTS_EXCEPTION(mem.mapShared(5 * pageSize, pageSize));

        TESTS_EXCEPTION(mem.resetPrivate(1, pageSize));
        TESTS_EXCEPTION(mem.resetPrivate(0, pageSize + 1));
        TESTS_EXCEPTION(mem.resetPrivate(0, 0));
        TESTS_EXCEPTION(mem.resetPrivate(2 * pageSize, 3 * pageSize));
        TESTS_EXCEPTION(mem.resetPrivate(5 * pageSize, pageSize));
    }

#if defined(XSTL_LINUX)
    // The shared page written by a forked child is seen by the parent, and
    // resetPrivate gives back zero-filled memory.
    void test_sharedAcrossFork()
    {
        uint64 pageSize = cVirtualMemory::getPageSize();
        cVirtualMemory mem(2 * pageSize);

        // Page 0 stays private, page 1 becomes shared
        mem.mapShared(pageSize, pageSize);
        uint8* shared = mem.getBase() + pageSize;
        TESTS_ASSERT_EQUAL(shared[0], 0);

        pid_t pid = fork();
        TESTS_ASSERT(pid >= 0);
        if (pid == 0)
        {
            // Child: write through the shared page and exit without running
            // any atexit handlers or flushing the parent's stdio buffers
            shared[0] = 0x7e;
            _exit(0);
        }

        int status = 0;
        TESTS_ASSERT_EQUAL(waitpid(pid, &status, 0), pid);
        TESTS_ASSERT(WIFEXITED(status) && WEXITSTATUS(status) == 0);
        TESTS_ASSERT_EQUAL(shared[0], 0x7e);

        // Dropping the shared mapping gives zero-filled private pages
        mem.resetPrivate(pageSize, pageSize);
        TESTS_ASSERT_EQUAL(shared[0], 0);
        TESTS_ASSERT_EQUAL(shared[pageSize - 1], 0);
    }
#endif

    // protect: none, then read-write again. The byte is writable after the
    // second call, and read-only protection still reads.
    void test_protect()
    {
        uint64 pageSize = cVirtualMemory::getPageSize();
        cVirtualMemory mem(4 * pageSize);

        mem.protect(3 * pageSize, pageSize, false, false);
        mem.protect(3 * pageSize, pageSize, true, true);
        mem.getBase()[3 * pageSize] = 0x3c;
        TESTS_ASSERT_EQUAL(mem.getBase()[3 * pageSize], 0x3c);

        mem.protect(pageSize, pageSize, true, false);
        TESTS_ASSERT_EQUAL(mem.getBase()[pageSize], 0);

        // Argument validation, as for protectNoAccess
        TESTS_EXCEPTION(mem.protect(1, pageSize, true, true));
        TESTS_EXCEPTION(mem.protect(0, pageSize + 1, true, true));
        TESTS_EXCEPTION(mem.protect(0, 0, true, true));
        TESTS_EXCEPTION(mem.protect(2 * pageSize, 3 * pageSize, true, true));
        TESTS_EXCEPTION(mem.protect(5 * pageSize, pageSize, true, true));
    }

#if defined(XSTL_LINUX)
    // mapFile: private mappings are copy-on-write, shared ones write through
    // to the file. Three pages of a temporary file are used.
    void test_mapFile()
    {
        uint64 pageSize = cVirtualMemory::getPageSize();
        const uint64 fileSize = 3 * pageSize;

        char path[] = "/tmp/xstl_vm_XXXXXX";
        int fd = mkstemp(path);
        TESTS_ASSERT(fd >= 0);
        unlink(path);

        // Fill the file with a known pattern
        uint8* pattern = new uint8[(size_t)fileSize];
        for (uint64 i = 0; i < fileSize; i++)
            pattern[i] = (uint8)(i * 7 + 3);
        TESTS_ASSERT_EQUAL((uint64)write(fd, pattern, (size_t)fileSize), fileSize);

        cVirtualMemory mem(6 * pageSize);

        // Private read-write mapping of the whole file
        TESTS_ASSERT_EQUAL(mem.mapFile(0, fileSize, fd, 0, false, true, true), 0);
        for (uint64 i = 0; i < fileSize; i++)
            TESTS_ASSERT_EQUAL(mem.getBase()[i], pattern[i]);

        // A private write does not change the file
        mem.getBase()[0] = 0xee;
        TESTS_ASSERT_EQUAL(mem.getBase()[0], 0xee);
        uint8 byte = 0;
        TESTS_ASSERT_EQUAL(pread(fd, &byte, 1, 0), 1);
        TESTS_ASSERT_EQUAL(byte, pattern[0]);

        // Shared read-write mapping of the same file, in the next three pages
        TESTS_ASSERT_EQUAL(mem.mapFile(3 * pageSize, fileSize, fd, 0, true, true, true), 0);
        TESTS_ASSERT_EQUAL(mem.getBase()[3 * pageSize], pattern[0]);
        mem.getBase()[3 * pageSize] = 0x42;
        TESTS_ASSERT_EQUAL(pread(fd, &byte, 1, 0), 1);
        TESTS_ASSERT_EQUAL(byte, 0x42);

        // Host errors come back as -errno, not as an exception
        TESTS_ASSERT(mem.mapFile(0, pageSize, -1, 0, false, true, false) < 0);
        TESTS_ASSERT(mem.mapFile(0, pageSize, fd, 1, false, true, false) < 0);

        // Bad ranges still throw
        TESTS_EXCEPTION(mem.mapFile(1, pageSize, fd, 0, false, true, true));
        TESTS_EXCEPTION(mem.mapFile(0, 0, fd, 0, false, true, true));
        TESTS_EXCEPTION(mem.mapFile(6 * pageSize, pageSize, fd, 0, false, true, true));

        delete[] pattern;
        close(fd);
    }
#endif

    // Perform the test
    virtual void test()
    {
        test_sizes();
        test_largeReservation();
        test_protectNoAccess();
        test_protect();
        test_sharedArguments();
#if defined(XSTL_LINUX)
        test_sharedAcrossFork();
        test_mapFile();
#endif
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

// Instance test object
cTestVirtualMemory g_globalTestVirtualMemory;
