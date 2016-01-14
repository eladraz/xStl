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
 * tests.cpp
 *
 * Main module for all xStl uni-tests.
 * The uni-tests are small modules which test the normal behavior of the class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/osrand.h"
#include "xStl/data/char.h"
#include "xStl/data/list.h"
#include "xStl/except/trace.h"
#include "xStl/except/exception.h"
#include "xStl/stream/ioStream.h"
#include "tests.h"


/*
 * All the constructed class are register inside a link-list which execute them.
 */
class tests_container{
public:
    static cList<cTestObject*>& get_tests()
    {
        static cList<cTestObject*> g_tests;
        return g_tests;
    }
};


/*
 * cTestObject constructor
 *
 * Register the class to the g_tests class
 */
cTestObject::cTestObject()
{
    tests_container::get_tests().append(this);
}

cString cTestObject::randomizeName(uint maxNumberOfDigits)
{
    CHECK(maxNumberOfDigits > 0);
    uint rnd = (cOSRand::rand() % (maxNumberOfDigits - 1)) + 1;
    cSArray<character> ret(rnd + 1);
    ret[rnd] = 0;
    for (uint i = 0; i < rnd; i++)
    {
        switch (cOSRand::rand() % 4)
        {
        case 0:
            ret[i] = (character)((cOSRand::rand() % 26) + 'a');
            break;
        case 1:
            ret[i] = (character)((cOSRand::rand() % 26) + 'A');
            break;
        case 2:
            ret[i] = (character)((cOSRand::rand() % 10) + '0');
            break;
        case 3:
            switch (cOSRand::rand() % 4)
            {
            case 0: ret[i] = '.'; break;
            case 1: ret[i] = '@'; break;
            case 2: ret[i] = '_'; break;
            case 3: ret[i] = '-'; break;
            }
        }
    }
    return cString(ret.getBuffer());
}


/*
 * main
 *
 * Scan all registered modules and
 */
int main(const int, const char**)
{
    uint32 modules_passed = 0;
    XSTL_TRY
    {
        uint32 total_counts = tests_container::get_tests().length();

        // print logo ;)
        cout << endl;
        cout << "Wellcome to xStl test module" << endl;
        cout << endl;
        cout << endl;
        cout << "xStl written by Elad Raz T.B.A." << endl;
        cout << "The system contains tests for " << total_counts << " modules" << endl;
        cout << "Start tests execuation" << endl << endl;


        // Spawn the tests.
        for (cList<cTestObject*>::iterator i = tests_container::get_tests().begin();
                                           i!= tests_container::get_tests().end();
                                           i++)
        {
            // Capture the test
            XSTL_TRY
            {
                #if defined(XSTL_CE) || defined(XSTL_LINUX)
                    // Windows CE run slower then normal computer, the testing
                    // library should view the progrss to the user.
                    cout << "Testing " << (*i)->getName() << "... ";
                    cout.flush();
                #endif
                (*i)->test();

                #if defined(XSTL_CE) || defined(XSTL_LINUX)
                cout << "OK!" << endl;
                #endif

                // all good...
                modules_passed++;
            }
            XSTL_CATCH (cException& e)
            {
                #if defined(XSTL_CE) || defined(XSTL_LINUX)
                cout << "FAILED!" << endl;
                #endif

                cout << endl << "Exception: " << e.getMessage() << " (" << e.getID() << ')' << endl;
                cout << "Exception: Throw at module " << (*i)->getName() << endl;
            }
            XSTL_CATCH (...)
            {
                #if defined(XSTL_CE) || defined(XSTL_LINUX)
                cout << "FAILED!" << endl;
                #endif

                cout << endl << "Exception: Unknown exception thrown..." << endl;
                cout << "Exception: Throw at module " << (*i)->getName() << endl;
            }
            #if !(defined(XSTL_CE) || defined(XSTL_LINUX))
            cout << '.';
            cout.flush();
            #endif
        }

        cout << endl;

        if (modules_passed == total_counts)
        {
            cout << "OK!" << endl;
            return RC_OK;
        } else
        {
            cout << modules_passed << " out of " << total_counts << " passed ("
                 << ((modules_passed*100)/total_counts) << "%)" << endl;
            return RC_ERROR;
        }
    }
    XSTL_CATCH(...)
    {
        cout << "Unexcpected error at tests module... "  << endl;
        cout << "During test #" << modules_passed<< endl;
        return RC_ERROR;
    }
}

bool cTestObject::isException = false;

#define MAX_STRING (400)
character* makeString(char* string)
{
    static character temp[MAX_STRING];
    uint len = t_min((int)strlen(string), (int)MAX_STRING);
    for (uint i = 0; i < len; i++)
    {
        temp[i] = (character)string[i];
    }
    temp[len] = cChar::getNullCharacter();
    return temp;
}
