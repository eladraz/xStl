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

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file osrand.h
 * @brief Declare operating-system-independent functions (random)
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/sarray.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/string.h"
#include "xStl/enc/random.h"
#include "xStl/os/mutex.h"
#include "xStl/os/xstlLockable.h"


/**
 * @brief Random API
 *
 * @details All of these functions are thread-safe and can be invoked from
 *    several threads
 *   without worrying about data loss.
 */
class cOSRand
{
public:
    //
    // Global random generator
    //

    /**
     * @brief Reset the mechanism into a newly constructed random object.
     *
     * @details This function is called when the system is loaded into the
     *    default random object. See the file 'random.cpp' in the OS xStl
     *    directory.
     *
     * @param newLoading The new mechanism and the seeding object
     */
    static void srand(const cSmartPtr<cRandom>& newLoading);

    /**
     * @brief Return the global random object used in this system.
     *
     * @details The returned object can be shared by a number of tasks in the
     *    system so it won't be a good recommendation to use it.
     *
     * @return The global random object used in this system.
     * @todo Move to private: code?
     */
    static cSmartPtr<cRandom> getRandom();

    /**
     * @brief Return a random number from the random-generation for the system.
     *
     * @details The function advances the global random mechanism by
     *    sizeof(uint) bytes.
     *
     * This function is thread-safe.
     *
     * @return A random number from the random-generation for the system.
     */
    static uint rand();

    /**
     * @brief Generate a buffer of random information using the global-random
     *    object.
     *
     * @details This function is thread-safe, but the 'output' object should not
     *    be touched
     *   from other threads.
     *
     * @param length The number of random bytes to generate.
     * @param output The buffer which receives the random bytes.
     */
    static void generateRandom(uint length, cBuffer& output);

private:
    /**
     * @brief When accessing the global random object it should be locked.
     */
    static cXstlLockable g_globalRandomMutex;
    /**
     * @brief The global random object.
     */
    static cSmartPtr<cRandom> g_globalRandom;
};


/** @} */
#endif // __TBA_STL_OS_OSRAND_H

