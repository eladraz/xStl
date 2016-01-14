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

#ifndef __TBA_STL_TESTS_H
#define __TBA_STL_TESTS_H


#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/data/list.h"
#include "xStl/except/exception.h"

/*
 * tests.h
 *
 * Declare the global cTestObject which instance a test object. The object
 * register itself to a test-list which execute and control the execuation.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
class cTestObject
{
public:
    /*
     * Default constructor must be public.
     */
    cTestObject();

    /*
     * Virtual dtor.
     */
    virtual ~cTestObject() {};

    /*
     * This function perform the test for the module. This function may throw
     * exception in-case of failure. The function should call to all sub-tests
     * functions.
     */
    virtual void test() = 0;

    /*
     * Returns the name for the testing-module.
     */
    virtual cString getName() = 0;

    /*
     * Return a random abc string.
     *
     * maxNumberOfDigits - The maximum number of digist which the string can be
     *
     * NOTE: The string cannot be empty.
     */
    static cString randomizeName(uint maxNumberOfDigits);

    /*
     * Is one of the test failed.
     */
    static bool isException;
};

/*
 * The function covert char* to character* without being thread-safe.
 * The only purpuse of this function is to covert __FILE__ macro to uni-code.
 */
character* makeString(char* string);


/*
 * Usefull assertion.
 */

#define TESTS_ASSERT(condition)      if (!(condition)) { XSTL_THROW(cException, XSTL_STRING("Condition asserted")); }
#define TESTS_ASSERT_EQUAL(a,b)      TESTS_ASSERT(((a)==(b)))
#define TESTS_ASSERT_NOT_EQUAL(a,b)  TESTS_ASSERT(((a)!=(b)))
#define TESTS_EXCEPTION(command)     try { command; } catch (cException&) { cTestObject::isException = true; } if (!cTestObject::isException) { XSTL_THROW(cException, XSTL_STRING("Exception asserted")); } cTestObject::isException = false;
#define TESTS_ALL_EXCEPTION(command) try { command; } catch (...) { cTestObject::isException = true; } if (!cTestObject::isException) { XSTL_THROW(cException, XSTL_STRING("Any exception asserted")); } cTestObject::isException = false;

#endif //__TBA_STL_TESTS_H

