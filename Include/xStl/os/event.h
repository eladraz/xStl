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

#ifndef __TBA_STL_OS_EVENT_H
#define __TBA_STL_OS_EVENT_H

/*
 * event.h
 *
 * Wrapper over operating system event objects
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/os/osdef.h"

/*
 * Event is operating system object which has two states: Reset and set.
 * The operating system was internal mechanism related to context-switch which
 * allow freezing objects until one or more events will be set.
 * Using this interface and 'setEvent/wait' to accomplish that functionality.
 */
class cEvent {
public:
    /*
     * Construct a new event object.
     * The new event state is reset.
     *
     * Throw exception if there aren't enough resources
     */
    cEvent();

    /*
     * Named constructor. Generate new named event or open named event.
     *
     * eventName    - The name of the event
     * shouldCreate - Set to true if the event should be generated.
     *                Set to false if the event should be opened
     *
     * Throw exception if there aren't enough resources
     */
    cEvent(const cString& eventName, bool shouldCreate);

    /*
     * Destructor. Free the resources of the event
     */
    ~cEvent();

    /*
     * Set the event
     */
    void setEvent();

    /*
     * Reset the event
     */
    void resetEvent();

    /*
     * Return true if the event is set.
     * Return false otherwise.
     */
    bool isEventSet() const;

    /*
     * Freeze the current thread until the event will be set. If the event
     * was in reset state, the function returns immediately.
     * After the function returns, the event continue to be set until
     * 'resetEvent' will be called.
     *
     * NOTE: For DDK developming, IRQL must be PASSIVE_LEVEL
     */
    void wait() const;

    /*
     * Returns the OS handle of the event.
     */
    cOSDef::eventHandle getHandle();

private:
    // The eventHandle
    cOSDef::eventHandle m_handle;
};

#endif // __TBA_STL_OS_EVENT_H
