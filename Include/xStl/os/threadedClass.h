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

/*
 * threadedClass.h
 *
 * The C++ way for thread handling.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/thread.h"

/*
 * C++ Thread manager. In order to generate a C++ thread inherit from this class
 *
 * Usage:
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
 *    // Wait until all thread executed
 *    a.wait();
 */
class cThreadedClass {
public:
    /*
     * Virtual empty destructor. This class should be inherit from.
     */
    virtual ~cThreadedClass();

    /*
     * Implement here the code of the newly constructed thread.
     */
    virtual void run() = 0;

    /*
     * Generate the thread.
     * Throw exception if the 'start()' command was already being executed.
     */
    void start();

    /*
     * Wait until the thread will finish it's execution.
     * If the thread wasn't start yet, the function immediatly return
     */
    void wait();

    /*
     * Return true if the 'run()' function completed normally
     */
    bool isDone() const;

    /*
     * Return the thread API class.
     *
     * This function is implemented for both const and non-const functions.
     */
    cThread& getThreadHandle();
    const cThread& getThreadHandle() const;

protected:
    /*
     * Default constructor.
     */
    cThreadedClass();

    // Deny copy-constructor and operator =
    cThreadedClass(const cThreadedClass& other);
    cThreadedClass& operator = (const cThreadedClass& other);

    // The instance of the running thread.
    cThread m_theInstance;
    // Set to true when the thread is running.
    volatile bool m_isStarted;
    // Set to true when the thread died.
    volatile bool m_isDone;

private:
    /*
     * Called when the 'start()' function is called.
     *
     * context - The context
     *
     * IMPLEMENTATION NOTE:
     *   Let's consider the following case (bug case):
     *     void generateThread() {
     *        MyThread a;
     *        a.start();
     *     }
     *   The context of 'a' variable will be move to the
     *   'threadedClassThreadFunction' but also a will be terminated by invoking
     *   the class destructor. This caused some kind of a race-condition.
     *   The internal implementation should guarantee that this cases will not
     *   crash the system.
     */
    static void* threadedClassThreadFunction(void* context);

    // Private premature death.
    volatile bool m_isStarted1;
};

#endif // __TBA_STL_THREADEDCLASS_H
