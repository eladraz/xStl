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
#ifdef __APPLE__
#include <sys/types.h>
#endif

/*
 * test_callback.cpp
 *
 * Tests the callbacker classes at the xStl_utils
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/smartptr.h"
#include "xStl/utils/callbacker.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "tests.h"

class cTestCallbacker : public cTestObject
{
public:
    // The callbacker
    void* onDispatcherEvent(void* argument)
    {
        TESTS_ASSERT_EQUAL(m_number, getNumeric(argument));
        // Reset the number
        m_number = m_reset;
        m_reset = 0;
        return NULL;
    }

    // Generate a callback thunk
    DECLARE_CALLBACK(cTestCallbacker, onDispatcherEvent);

    // The dispatcher class
    class Dispatcher {
    public:
        Dispatcher() : m_callbackClass(NULL) {};
        // The callbacker registration.
        void registerCallback(cSmartPtr<cCallback> callbackClass) {
            m_callbackClass = callbackClass;
        }
        // Invoke event
        void trigger(void* data) {
            m_callbackClass->call(data);
        };
        cSmartPtr<cCallback> m_callbackClass;
    };

    // Preform the test...
    virtual void test()
    {
        // Reset the dispacther
        m_dispather.registerCallback(CALLBACKER(onDispatcherEvent));

        // Invoke commands
        for (uint i = 0; i < 100; i++)
        {
            uint32 val = cOSRand::rand();
            uint32 nvl = cOSRand::rand();
            m_number = val;
            m_reset = nvl;
            // Call
            m_dispather.trigger(getPtr(m_number));
            // Test
            TESTS_ASSERT_EQUAL(m_reset, 0);
            TESTS_ASSERT_EQUAL(m_number, nvl);
        }
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }

private:
    // The last argument transfered
    uint32 m_number;
    // The reset value
    uint32 m_reset;
    // The dispatcher class
    Dispatcher m_dispather;
};

// Instance a test object
cTestCallbacker gGlobalTestCounter;

