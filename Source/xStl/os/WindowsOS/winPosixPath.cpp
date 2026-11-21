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
 * winPosixPath.cpp
 *
 * Windows side of cPosixPath. The POSIX path model has no Windows
 * equivalent in xStl, so every call throws EXCEPTION_FAILED.
 */

#include "xStl/xStlPrecompiled.h"
#include "xStl/types.h"
#include "xStl/except/exception.h"
#include "xStl/os/posixPath.h"

// Every entry point reports the unsupported model the same way
static int64 posixUnsupported()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixPath::fstatAt(int64, const char*, int, cPosixStat&)
{
    return posixUnsupported();
}

int64 cPosixPath::statx(int64, const char*, int, uint32, void*)
{
    return posixUnsupported();
}

int64 cPosixPath::faccessAt(int64, const char*, int, int)
{
    return posixUnsupported();
}

int64 cPosixPath::unlinkAt(int64, const char*, int)
{
    return posixUnsupported();
}

int64 cPosixPath::mkdirAt(int64, const char*, uint32)
{
    return posixUnsupported();
}

int64 cPosixPath::renameAt2(int64, const char*, int64, const char*, uint32)
{
    return posixUnsupported();
}

int64 cPosixPath::linkAt(int64, const char*, int64, const char*, int)
{
    return posixUnsupported();
}

int64 cPosixPath::symlinkAt(const char*, int64, const char*)
{
    return posixUnsupported();
}

int64 cPosixPath::readlinkAt(int64, const char*, char*, uint64)
{
    return posixUnsupported();
}

int64 cPosixPath::getcwd(char*, uint64)
{
    return posixUnsupported();
}

int64 cPosixPath::chdir(const char*)
{
    return posixUnsupported();
}

int64 cPosixPath::fchdir(int64)
{
    return posixUnsupported();
}

int64 cPosixPath::umask(uint32)
{
    return posixUnsupported();
}

int64 cPosixPath::utimensAt(int64, const char*, const int64*, int)
{
    return posixUnsupported();
}

int64 cPosixPath::fchmodAt(int64, const char*, uint32, int)
{
    return posixUnsupported();
}

int64 cPosixPath::fchownAt(int64, const char*, int64, int64, int)
{
    return posixUnsupported();
}

int64 cPosixPath::getdents64(int64, void*, uint64)
{
    return posixUnsupported();
}

int64 cPosixPath::statfs(const char*, void*)
{
    return posixUnsupported();
}

int64 cPosixPath::fstatfs(int64, void*)
{
    return posixUnsupported();
}
