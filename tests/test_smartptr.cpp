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

/*
 * test_string.cpp
 *
 * Test the cString class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/list.h"
#include "xStl/except/trace.h"
#include "../../xStl/tests/tests.h"


/*
 * Refrence count object test
 */
class A
{
public:
    A()
    {
        //TRACE(TRACE_LOW, "New object constructed");
        m_member = cOSRand::rand();
    }
    ~A()
    {
        //TRACE(TRACE_LOW, "Object destruct");
    }

    int& getMember() { return m_member; }

private:
    int m_member;
};



class cTestSmartPtr : public cTestObject
{
public:

    void test_normal()
    {
        A* a_obj = new A();
        int member = a_obj->getMember();
        cSmartPtr<A> object2(a_obj, SMARTPTR_DESTRUCT_DELETE);
        cSmartPtr<A> object1(object2);

        TESTS_ASSERT_EQUAL(member, object2->getMember());
        TESTS_ASSERT_EQUAL(member, object1->getMember());

        A* obj1 = object1;
        A* obj2 = object2;
        TESTS_ASSERT_EQUAL(obj1, obj2);
    };


    // Perform the test
    virtual void test()
    {
        // Init random generator.
        for (uint i = 0; i < 100; i++) cOSRand::rand();
        test_normal();
    };

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestSmartPtr g_globalTestSmartPtr;

