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

#ifndef __TBA_STL_OS_WAITABLE_H
#define __TBA_STL_OS_WAITABLE_H

/*
 * waitable.h
 *
 * Defines the "waitable" objects. Waitable objects are objects which signal
 * a completion of operation.
 * When operation is made in the "background" (which can be hardware, or another
 * thread), a waitable object is generated. When the operation completed the
 * waitable object singals, and the blocking operation can progress.
 * The waitable object is shared for all platform and can be simulated for non-
 * preemptive operation systems.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/osdef.h"

/*
 * class cWaitable
 */
class cWaitable {
public:
    /*
     * Return true if the operation completed.
     */
    bool isSucceeded() const;

    /*
     * Return true if the operation is active and the system and doesn't
     * complete by:
     *   - Normal behaviour (Operation completed, successfully or not)
     *   - Soft cancel (Abnormal termination)
     *   - Timeout riched.
     */
    bool isValid() const;

    /*
     * Return true if the operation timeout was riched, and the operation was
     * canceled.
     */
    bool isTimeout() const;

    /*
     * Returns the number of system-ticks (CurrentTime-StartTime) that take the
     * operation to be completed.
     */
    cOSDef::systemTime getOperationTime();

    /*
     * Cancel the running operation.
     *
     * Return true if the cancel request succeeded.
     * Return false if:
     *    - The operation already completed
     *    - The operation cannot be canceled.
     */
    bool cancel();

    /*
     * Holds the thread until the operation is complete or being aborted.
     */
    void wait();
};

#endif // __TBA_STL_OS_WAITABLE_H
