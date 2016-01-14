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
 * test_hash.cpp
 *
 * Test the template cHash class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/hash.h"
#include "xStl/data/string.h"
#include "xStl/os/os.h"
#include "tests.h"


// For the constructor and deconstructor test
// Copy constructor and operator=.
enum ActionList {
    EMPTY,
    CONSTRUCTOR,
    DESTRUCTOR,
    COPYCONSTRUCTOR,
    OPERATOREQUAL
};

static unsigned int lastAction = EMPTY;
static unsigned int counstructorCounter = 0;
static unsigned int destructorCounter = 0;
static unsigned int lastPutValue = 0;
class TESTOBJECT
{
public:
    TESTOBJECT()  { lastAction = CONSTRUCTOR; counstructorCounter++; m_global = lastPutValue; }
    ~TESTOBJECT() { lastAction = DESTRUCTOR; destructorCounter++; }
    TESTOBJECT(const TESTOBJECT& other) { UNUSED_PARAM(other); lastAction = COPYCONSTRUCTOR; }
    TESTOBJECT& operator=(const TESTOBJECT& other) { UNUSED_PARAM(other); lastAction = OPERATOREQUAL; return *this; }

    unsigned int m_global;
};


class cTestHash : public cTestObject
{
private:
    void reset_testhash()
    {
        lastAction = EMPTY;
        counstructorCounter = 0;
        destructorCounter = 0;
    }
    void test_reset_testhash()
    {
        TESTS_ASSERT_EQUAL(counstructorCounter, 0);
        TESTS_ASSERT_EQUAL(destructorCounter, 0);
        TESTS_ASSERT_EQUAL(lastAction, EMPTY);
    }

public:
    // Make sure that all object constructor and destructor are called.
    void test_hash()
    {
        cHash<cString, cString> hash;

        // Test exception for non exist value
        TESTS_EXCEPTION(hash["First"] = "1");
        TESTS_EXCEPTION(hash[""] = "1");
        TESTS_EXCEPTION(hash["Temp"] = "1");

        // Add some values
        hash.append("First", "1");
        hash.append("Second", "2");
        hash.append("Third", "3");

        TESTS_ASSERT(!hash.hasKey("FIRST"));
        TESTS_ASSERT(!hash.hasKey("temp"));
        TESTS_ASSERT(!hash.hasKey("Thrid"));

        TESTS_ASSERT(hash.hasKey("First"));
        TESTS_ASSERT(hash.hasKey("Second"));
        TESTS_ASSERT(hash.hasKey("Third"));

        TESTS_ASSERT_EQUAL(hash["First"], "1");
    };

    // Perform the test
    virtual void test()
    {
        // Start the tests.
        test_hash();
    };

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestHash g_globalTestHash;

