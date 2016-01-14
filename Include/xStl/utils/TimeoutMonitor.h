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

#ifndef __XSTL_UTILS_TIMEOUTMONITOR_H
#define __XSTL_UTILS_TIMEOUTMONITOR_H

#include "xStl/os/os.h"

/**
 * Utility class used to set and monitor a timeout period.
 *
 * For timeout implementation, the monitor uses time support routines
 * provided by xStl's cOS class (getSystemTime and calculateTimesDiffMilli).
 *
 * Author: Sergei Chernov.
 */
class TimeoutMonitor {
public:
    /// Special value for the timeout meaning the timeout never expires.
    static const uint INFINITE_TIMEOUT;

    /**
     * Initializes the timeout monitor and starts the timeout period.
     * timeoutMilliseconds    Timeout period, in milliseconds.
     *                        Can be INFINITE_TIMEOUT, in which case
     *                        the timeout will never expire.
     *
     * TODO: timeoutMilliseconds is uint as in cOS::calculateTimesDiffMilli.
     *         Should it be fixed together with calculateTimesDiffMilli?
     */
    TimeoutMonitor(uint timeoutMilliseconds);

    /**
     * Restarts the timeout period.
     */
    void restart();

    /**
     * Tests whether the timeout has expired.
     * return    'true' if the timeout expired, 'false' otherwise.
     */
    bool isExpired();

    /*
     * Return the number of milliseconds elapsed from the 'restart()' time or
     * constructor time
     */
    uint getElapsedMilliseconds();

private:

    /// When the timeout was started.
    cOSDef::systemTime m_startTime;

    /// Timeout period duration (note the special value INFINITE_TIMEOUT).
    uint m_timeoutMilliseconds;
};

#endif // __XSTL_UTILS_TIMEOUTMONITOR_H
