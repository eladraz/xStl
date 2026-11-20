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

#include "xStl/xStlPrecompiled.h"

#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/list.h"
#include "xStl/except/exception.h"
#include "xStl/os/posixProcess.h"

// The POSIX services are unsupported on Windows: every call throws, as
// cOS::forkProcess does.

int64 cPosixProcess::getPid()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::getPpid()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::getUid()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::getEuid()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::getGid()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::getEgid()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::kill(int64 pid, int sig)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::setDisposition(int sig, int disposition)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::raiseDefault(int sig)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::uname(cPosixUtsName& name)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::getRlimit(int resource, uint64& current, uint64& maximum)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::getRusage(int who, void* buf)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::times(void* buf)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::sysinfo(void* buf)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::schedGetAffinity(void* buf, uint64 length)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::schedYield()
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::clockGetTime(int clockId, int64& seconds,
                                int64& nanoseconds)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::clockGetRes(int clockId, int64& seconds,
                               int64& nanoseconds)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::sleep(int clockId, int flags, int64 seconds,
                         int64 nanoseconds, int64* remainingSeconds,
                         int64* remainingNanoseconds)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int64 cPosixProcess::getRandom(void* buf, uint64 length, uint flags)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}

int cPosixProcess::getEnvironment(cList<cString>& out)
{
    XSTL_THROW(cException, EXCEPTION_FAILED);
    return -1;
}
