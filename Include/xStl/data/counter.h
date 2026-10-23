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

#ifndef __TBA_STL_COUNTER_H
#define __TBA_STL_COUNTER_H

/**
 * @addtogroup xstl_data_pointers
 * @{
 */

/**
 * @file counter.h
 * @brief Multiprocess counting unit. Manage reference counting mainly.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/os/xstlLockable.h"


/*
 * Forward declaration for the tests.
 */
class cTestCounter;

/**
 * @brief A safe counter. A counter which is thread-safe and protected by a
 *    mutex.
 */
class cCounter
{
public:
    /**
     * @brief Constructor. Create a counter object.
     *
     * @param initValue The value for the counter.
     */
    cCounter(counter_t initValue);

    /**
     * @brief Increase the counter reference.
     *
     * @return The elements count.
     */
    counter_t increase();

    /**
     * @brief Decrease the counter reference.
     *
     * @return The elements count.
     */
    counter_t decrease();

    /**
     * @brief Return the current value of the counter
     *
     * @return The current value of the counter.
     */
    counter_t getValue() const;

    /**
     * @brief Changes the value of the counter
     *
     * @param newValue The new value of the counter
     */
    void setValue(const counter_t& newValue);

    /**
     * @brief Call to increase
     *
     * @return A reference to this counter.
     */
    cCounter& operator++();

    /**
     * @brief Call to decrease
     *
     * @return A reference to this counter.
     */
    cCounter& operator--();

private:
    /**
     * @brief The value of the counter
     */
    volatile counter_t m_value;

    // Windows NT kernel-mode and user mode contain built-in functions
    #if (!(defined(XSTL_NTDDK) || defined(XSTL_WINDOWS) || defined(XSTL_CE)))
        /**
         * @brief The lock for the counter
         */
        cXstlLockable m_lock;
    #endif

    /**
     * @brief Prevent copy-constructor and operator =.
     */
    cCounter(const cCounter& other);
    cCounter& operator =(const cCounter& other);

    // Everybody needs some friends.
    friend class cTestCounter;
};



/** @} */
#endif //  __TBA_STL_COUNTER_H

