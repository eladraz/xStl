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
 * test_array.cpp
 *
 * Test the template cArray class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "../../xStl/tests/tests.h"

#ifdef XSTL_WINDOWS
// We use a "do {} while(false)" on purpose below.
#pragma warning (disable:4127)
#endif

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
class TESTARRAY
{
public:
    TESTARRAY()  { lastAction = CONSTRUCTOR; counstructorCounter++; m_global = lastPutValue; }
    ~TESTARRAY() { lastAction = DESTRUCTOR; destructorCounter++; }
    TESTARRAY(const TESTARRAY&) { lastAction = COPYCONSTRUCTOR; }
    TESTARRAY& operator=(const TESTARRAY&) { lastAction = OPERATOREQUAL; return *this; }

    unsigned int m_global;
};


class cTestArray : public cTestObject
{
private:
    void reset_testarray()
    {
        lastAction = EMPTY;
        counstructorCounter = 0;
        destructorCounter = 0;
    }
    void test_reset_testarray()
    {
        TESTS_ASSERT_EQUAL(counstructorCounter, 0);
        TESTS_ASSERT_EQUAL(destructorCounter, 0);
        TESTS_ASSERT_EQUAL(lastAction, EMPTY);
    }

public:
    // Make sure that all object constructor and destructor are called.
    void test_constructors_array()
    {
        test_reset_testarray();
        for (unsigned int i = 0; i < 200; i++)
        {
            unsigned int nObjects = 0;
            do {
                cArray<TESTARRAY> list;
                TESTS_ASSERT_EQUAL(lastAction, EMPTY);
                nObjects = (cOSRand::rand() % 200) + 1; // At-least one object
                list.changeSize(nObjects);
                TESTS_ASSERT_EQUAL(lastAction, CONSTRUCTOR);
                TESTS_ASSERT_EQUAL(counstructorCounter, nObjects);
            } while (false);
            TESTS_ASSERT_EQUAL(lastAction, DESTRUCTOR);
            TESTS_ASSERT_EQUAL(destructorCounter, nObjects);
            reset_testarray();
        }
        test_reset_testarray();
    };

    // Default constructor test
    void test_assignment()
    {
        test_reset_testarray();
            for (unsigned int i = 0; i < 100; i++)
            {
                lastPutValue = cOSRand::rand();
                cArray<TESTARRAY> list(10);
                for (unsigned int j = 0; j < 10; j++)
                {
                    TESTS_ASSERT_EQUAL(list[j].m_global, lastPutValue);
                }
            }
        reset_testarray();
    };

    // Test the memory of the array
    void test_array_members()
    {
        for (unsigned int i = 0; i < 100; i++)
        {
            uint32 temp[256];
            uint32 len = cOSRand::rand() % 256;

            cArray<uint32> array(len);
            unsigned int j;
            // put value
            for (j = 0; j < len; j++)
            {
                temp[j] = cOSRand::rand();
                array[j] = temp[j];
            }

            // test value
            for (j = 0; j < len; j++)
            {
                TESTS_ASSERT_EQUAL(array[j], temp[j]);
            }

            // Test exception at wrong position.
            TESTS_EXCEPTION(array[len] = 10);
            TESTS_EXCEPTION(j = array[len]);
        }
    }

    // Test the page-round function
    void test_page_round()
    {
        cArray<uint32> array(10, 300);
        TESTS_ASSERT_EQUAL(array.pageRound(0),     0);
        TESTS_ASSERT_EQUAL(array.pageRound(1),   300);
        TESTS_ASSERT_EQUAL(array.pageRound(10),  300);
        TESTS_ASSERT_EQUAL(array.pageRound(299), 300);
        TESTS_ASSERT_EQUAL(array.pageRound(300), 300);
        TESTS_ASSERT_EQUAL(array.pageRound(301), 600);
        TESTS_ASSERT_EQUAL(array.pageRound(310), 600);
        TESTS_ASSERT_EQUAL(array.pageRound(399), 600);
        for (unsigned int i = 0; i < 100; i++)
        {
            unsigned int newPageSize = (cOSRand::rand() % 1024) + 1;
            array.setPageSize(newPageSize);
            unsigned int j;
            TESTS_ASSERT_EQUAL(array.pageRound(0), 0);
            for (j = 1; j < newPageSize; j++)
            {
                TESTS_ASSERT_EQUAL(array.pageRound(j), newPageSize);
            }
            TESTS_ASSERT_EQUAL(array.pageRound(newPageSize), newPageSize);
            TESTS_ASSERT_EQUAL(array.pageRound(newPageSize+1), newPageSize*2);
        }
    }

    void test_swap()
    {
        uint i;
        cArray<uint32> array1(10);
        cArray<uint32> array2(4);
        for (uint x = 0; x < 100; x++)
        {
            TESTS_ASSERT_EQUAL(array1.getSize(), 10);
            TESTS_ASSERT_EQUAL(array2.getSize(), 4);
            for (i = 0; i < 10; i++)
                array1[i] = (x + i) * 2;
            for (i = 0; i < 4; i++)
                array2[i] = (x + i) * 4;

            array1.swap(array2);

            TESTS_ASSERT_EQUAL(array1.getSize(), 4);
            TESTS_ASSERT_EQUAL(array2.getSize(), 10);

            for (i = 0; i < 10; i++)
                TESTS_ASSERT_EQUAL(array2[i], (x + i) * 2);
            for (i = 0; i < 4; i++)
                TESTS_ASSERT_EQUAL(array1[i], (x + i) * 4);

            // Test for pointer collusions...
            for (i = 0; i < 4; i++)
                array1[i] = (x + i) * 6;
            for (i = 0; i < 10; i++)
                array2[i] = (x + i) * 7;

            for (i = 0; i < 4; i++)
                TESTS_ASSERT_EQUAL(array1[i], (x + i) * 6);
            for (i = 0; i < 10; i++)
                TESTS_ASSERT_EQUAL(array2[i], (x + i) * 7);

            array2.swap(array1);
        }
    }

    // Perform the test
    virtual void test()
    {
        // Init random generator.
        for (uint i = 0; i < 100; i++) cOSRand::rand();

        // Start the tests.
        test_constructors_array();
        test_assignment();
        test_array_members();
        test_page_round();
        test_swap();
    };

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};


// Instance test object
cTestArray g_globalTestArray;

