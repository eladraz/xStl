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

#ifndef __TBA_STL_THREADEDCLASS_H
#define __TBA_STL_THREADEDCLASS_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file threadedClass.h
 * @brief The C++ way for thread handling.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/os/thread.h"

/**
 * @brief C++ Thread manager. In order to generate a C++ thread inherit from
 *    this
 *   class
 *
 * @details Usage:
 *
 * @code
 *    class cMyThread : public cThreadedClass {
 *    public:
 *          virtual void run() {
 *            // Do some work here...
 *          }
 *    };
 *
 *    // Open 3 threads
 *    cMyThread a, b, c;
 *    a.start();
 *    b.start();
 *    c.start();
 *
 *    // Wait until all threads have executed
 *    a.wait();
 * @endcode
 */
class cThreadedClass {
public:
    /**
     * @brief Virtual empty destructor. This class should be inherited from.
     */
    virtual ~cThreadedClass();

    /**
     * @brief Implement here the code of the newly constructed thread.
     */
    virtual void run() = 0;

    /**
     * @brief Generate the thread.
     *
     * @throws cException if the 'start()' command was already executed.
     */
    void start();

    /**
     * @brief Wait until the thread finishes its execution.
     *
     * @details If the thread wasn't started yet, the function returns
     *    immediately
     */
    void wait();

    /**
     * @brief Return true if the 'run()' function completed normally
     *
     * @return True if the 'run()' function completed normally.
     */
    bool isDone() const;

    /**
     * @brief Return the thread API class.
     *
     * @details This function is implemented for both const and non-const
     *    functions.
     *
     * @return The thread API class.
     */
    cThread& getThreadHandle();
    /**
     * @brief Return the thread API class (const version).
     *
     * @return The thread API class.
     */
    const cThread& getThreadHandle() const;

protected:
    /**
     * @brief Default constructor.
     */
    cThreadedClass();

    /**
     * @brief Deny copy-constructor.
     *
     * @param other Not used.
     */
    cThreadedClass(const cThreadedClass& other);
    /**
     * @brief Deny operator =.
     *
     * @param other Not used.
     * @return Not used.
     */
    cThreadedClass& operator = (const cThreadedClass& other);

    /**
     * @brief The instance of the running thread.
     */
    cThread m_theInstance;
    /**
     * @brief Set to true when the thread is running.
     */
    volatile bool m_isStarted;
    /**
     * @brief Set to true when the thread died.
     */
    volatile bool m_isDone;

private:
    /**
     * @brief Called when the 'start()' function is called.
     *
     * @details IMPLEMENTATION NOTE:
     *   Let's consider the following case (bug case):
     *
     * @code
     * void generateThread() {
     *    MyThread a;
     *    a.start();
     * }
     * @endcode
     *
     * The context of the 'a' variable will be moved to the
     * 'threadedClassThreadFunction' but also a will be terminated by invoking
     * the class destructor. This causes a kind of race condition.
     * The internal implementation should guarantee that this cases will not
     * crash the system.
     *
     * @param context The context
     */
    static void* threadedClassThreadFunction(void* context);

    /**
     * @brief Private premature death.
     */
    volatile bool m_isStarted1;
};


/** @} */
#endif // __TBA_STL_THREADEDCLASS_H
