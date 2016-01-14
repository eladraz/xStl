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

#ifndef __TBA_STL_OS_OSRAND_H
#define __TBA_STL_OS_OSRAND_H

/*
 * osrand.h
 *
 * Declare on independet functions for operation system (random)
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/sarray.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/string.h"
#include "xStl/enc/random.h"
#include "xStl/os/mutex.h"
#include "xStl/os/xstlLockable.h"


/*
 * class cOSRand
 *
 * Random API
 *
 * All of these function are thread-safe and can be invoke from several of thread
 * without worries of data-loses.
 */
class cOSRand
{
public:
    //
    // Global random generator
    //

    /*
     * Reset the mechanism into a a new constructed random object. This function
     * called when the system is loaded into the default random object. See the
     * file 'random.cpp' in the OS xStl directory.
     *
     * newLoading - The new mechanism and the seeding object
     */
    static void srand(const cSmartPtr<cRandom>& newLoading);

    /*
     * Return the global random object used in this system. The return object
     * can be shared to number of task in the system so it's won't be a good
     * recommandiation to use it.
     *
     * TODO: Move to private: code?
     */
    static cSmartPtr<cRandom> getRandom();

    /*
     * Return a random number from the random-generation for the system.
     * The function increase the global random mechanizm by sizeof(uint)
     * bytes.
     *
     * This function is thread-safe call.
     */
    static uint rand();

    /*
     * Generate a buffer of random information using the global-random object.
     *
     * This function is thread-safe call, but the 'output' object should be touch
     * from other threads.
     */
    static void generateRandom(uint length, cBuffer& output);

private:
    // When accessing the global random object it's should be locked.
    static cXstlLockable g_globalRandomMutex;
    // The global random object.
    static cSmartPtr<cRandom> g_globalRandom;
};

#endif // __TBA_STL_OS_OSRAND_H

