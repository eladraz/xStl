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
#include "xStl/data/list.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/data/wildcardMatcher.h"
#include "xStl/except/trace.h"
#include "xStl/os/os.h"
#include "../../xStl/tests/tests.h"


class cTestString : public cTestObject
{
public:
    static const character ABC[];

    void test_constructor()
    {
        static const character EMPTY[] = XSTL_STRING("Empty");
        static const character EMPTYA[] = XSTL_STRING("EmptyA");
        static const character EMPTYAE[] = XSTL_STRING("EmptyAE");

        cString empty;
        cString emptyString(EMPTY);
        cString emptyChar(XSTL_CHAR('E'));
        cString number(10, 16);
        cString copy(number);
        cString cat;
        cat+= empty;
        TESTS_ASSERT_EQUAL(cat, XSTL_STRING(""));
        TESTS_ASSERT_EQUAL(cChar::strcmp(cat.getBuffer(), XSTL_STRING("")), 0);
        cat+= emptyString;
        TESTS_ASSERT_EQUAL(cat, emptyString);
        TESTS_ASSERT_EQUAL(cChar::strcmp(cat.getBuffer(), emptyString.getBuffer()), 0);
        TESTS_ASSERT_EQUAL(cChar::strcmp(cat.getBuffer(), EMPTY), 0);
        cat = cat + number;
        cString result(EMPTYA);
        TESTS_ASSERT_EQUAL(cat, result);
        TESTS_ASSERT_EQUAL(cChar::strcmp(cat.getBuffer(), result.getBuffer()), 0);
        TESTS_ASSERT_EQUAL(cChar::strcmp(cat.getBuffer(), EMPTYA), 0);

        TESTS_ASSERT_EQUAL(copy, number);
        TESTS_ASSERT_EQUAL(cChar::strcmp(copy.getBuffer(), number.getBuffer()), 0);
        TESTS_ASSERT_EQUAL(cChar::strcmp(copy.getBuffer(), XSTL_STRING("A")), 0);

        cat+= emptyChar;
        result = EMPTYAE;
        TESTS_ASSERT_EQUAL(cat, result);
        TESTS_ASSERT_EQUAL(cChar::strcmp(cat.getBuffer(), result.getBuffer()), 0);
        TESTS_ASSERT_EQUAL(cChar::strcmp(cat.getBuffer(), EMPTYAE), 0);
    }

    void test_mid()
    {
        cString abc(ABC);
        cString m(abc.mid(0, abc.length()));
        TESTS_ASSERT_EQUAL(abc, m);
        TESTS_ASSERT_EQUAL(cChar::strcmp(abc.getBuffer(), m.getBuffer()), 0);
        m = abc.mid(0, 0x100);
        TESTS_ASSERT_EQUAL(abc, m);
        TESTS_ASSERT_EQUAL(cChar::strcmp(abc.getBuffer(), m.getBuffer()), 0);
        m = abc.mid(0, 0);
        cString result;
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("")), 0);
        m = abc.mid(3, 0);
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("")), 0);
        m = abc.mid(0x100, 0x100);
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("")), 0);
        m = abc.mid(0, 1);
        TESTS_ASSERT_EQUAL(m, cString(XSTL_STRING("a")));
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("a")), 0);
        m = abc.mid(0, 3);
        TESTS_ASSERT_EQUAL(m, cString(XSTL_STRING("abc")));
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("abc")), 0);
        m = abc.mid(24, 1);
        TESTS_ASSERT_EQUAL(m, cString(XSTL_STRING("y")));
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("y")), 0);
        m = abc.mid(24, 3);
        TESTS_ASSERT_EQUAL(m, cString(XSTL_STRING("yz")));
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("yz")), 0);
    }

    void test_left()
    {
        cString abc(ABC);
        cString m = abc.left(0);
        cString result;
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("")), 0);
        m = abc.left(0x100);
        TESTS_ASSERT_EQUAL(m, abc);
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), abc.getBuffer()), 0);

        m = abc.left(0x1);
        result = XSTL_STRING("a");
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::stricmp(m.getBuffer(), XSTL_STRING("A")), 0);

        m = abc.left(0x4);
        result = XSTL_STRING("abcd");
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::stricmp(m.getBuffer(), XSTL_STRING("ABCD")), 0);
    }

    void test_find()
    {
        cString empty;
        TESTS_ASSERT_EQUAL(empty.find("Bla bla"), 0);
        cString a = XSTL_STRING("A simple string used for find command");
        TESTS_ASSERT_EQUAL(a.find(XSTL_STRING("Not here")), a.length());
        TESTS_ASSERT_EQUAL(a.find(XSTL_STRING("i")), 3);
        TESTS_ASSERT_EQUAL(a.find(XSTL_STRING("im")), 3);
        TESTS_ASSERT_EQUAL(a.find(XSTL_STRING("i"), 4), 12);

        cString shortString = XSTL_STRING("ABC");
        TESTS_ASSERT_EQUAL(shortString.find(XSTL_STRING("Not here")), shortString.length());
        shortString = "c";
        TESTS_ASSERT_EQUAL(shortString.find(XSTL_STRING("Not here")), shortString.length());
        TESTS_ASSERT_EQUAL(shortString.find(XSTL_STRING("CD")), shortString.length());
        TESTS_ASSERT_EQUAL(shortString.find(XSTL_STRING("cd")), shortString.length());
        TESTS_ASSERT_EQUAL(shortString.find(XSTL_STRING("c")), 0);
        TESTS_ASSERT_EQUAL(shortString.find(XSTL_STRING("C")), shortString.length());

        // Reverse find
        TESTS_ASSERT_EQUAL(empty.rfind("Bla bla"), 0);
        // a = XSTL_STRING("A simple string used for find command");
        TESTS_ASSERT_EQUAL(a.rfind(XSTL_STRING("Not here")), a.length());
        TESTS_ASSERT_EQUAL(a.rfind(XSTL_STRING("i")), 26);
        TESTS_ASSERT_EQUAL(a.rfind(XSTL_STRING("im")), 3);
        TESTS_ASSERT_EQUAL(a.rfind(XSTL_STRING("i"), 25), 12);
        TESTS_ASSERT_EQUAL(a.rfind(XSTL_STRING("blabla"), 2), a.length());
        TESTS_ASSERT_EQUAL(a.rfind(XSTL_STRING("blabla"), 0), a.length());
        TESTS_ASSERT_EQUAL(a.rfind(XSTL_STRING("A "), 2), 0);

        shortString = XSTL_STRING("ABC");
        TESTS_ASSERT_EQUAL(shortString.rfind(XSTL_STRING("Not here")), shortString.length());
        shortString = "c";
        TESTS_ASSERT_EQUAL(shortString.rfind(XSTL_STRING("Not here")), shortString.length());
        TESTS_ASSERT_EQUAL(shortString.rfind(XSTL_STRING("CD")), shortString.length());
        TESTS_ASSERT_EQUAL(shortString.rfind(XSTL_STRING("cd")), shortString.length());
        TESTS_ASSERT_EQUAL(shortString.rfind(XSTL_STRING("c")), 0);
        TESTS_ASSERT_EQUAL(shortString.rfind(XSTL_STRING("C")), shortString.length());
    };

    void test_split()
    {
        cString a = XSTL_STRING("1DIV2DIV3DIV4DIV5");
        cList<cString> d = a.split(XSTL_STRING("DIV"));
        TESTS_ASSERT_EQUAL(d.length(), 5);
        unsigned int counter = 0;
        for (cList<cString>::iterator i = d.begin(); i != d.end(); i++)
        {
            cString ob = *i;
            TESTS_ASSERT_EQUAL(ob.length(), 1);
            TESTS_ASSERT_EQUAL(ob[0], (char)('1' + (counter++)));
            TESTS_ASSERT_EQUAL(ob[1], 0);
        }
    }

    void test_operator_LZ()
    {
        cString temp = XSTL_STRING("TEMP");
        TESTS_ASSERT_EQUAL(temp[0], XSTL_CHAR('T'));
        TESTS_ASSERT_EQUAL(temp[1], XSTL_CHAR('E'));
        TESTS_ASSERT_EQUAL(temp[2], XSTL_CHAR('M'));
        TESTS_ASSERT_EQUAL(temp[3], XSTL_CHAR('P'));

        uint32 position = 4;
        character nil = temp[position];
        TESTS_ASSERT_EQUAL(nil, cChar::getNullCharacter());
    }

    void test_right()
    {
        cString abc(ABC);
        cString m = abc.right(0);
        cString result;
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), XSTL_STRING("")), 0);
        m = abc.right(0x100);
        TESTS_ASSERT_EQUAL(m, abc);
        TESTS_ASSERT_EQUAL(cChar::strcmp(m.getBuffer(), abc.getBuffer()), 0);

        m = abc.right(0x1);
        result = XSTL_STRING("z");
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::stricmp(m.getBuffer(), XSTL_STRING("Z")), 0);

        m = abc.right(0x4);
        result = XSTL_STRING("wxyz");
        TESTS_ASSERT_EQUAL(m, result);
        TESTS_ASSERT_EQUAL(cChar::stricmp(m.getBuffer(), XSTL_STRING("WXYZ")), 0);
    }

    void test_wildcard()
    {
        // Complete words
        TESTS_ASSERT(cWildcardMatcher::match("blabla", "blabla"));
        TESTS_ASSERT(cWildcardMatcher::match("blabla", "bLaBlA", false));
        TESTS_ASSERT(!cWildcardMatcher::match("blabla", "blaBla"));
        TESTS_ASSERT(!cWildcardMatcher::match("blabla", ""));
        TESTS_ASSERT(!cWildcardMatcher::match("blabla", "blabla1"));
        TESTS_ASSERT(!cWildcardMatcher::match("blabla", "blaBla1", false));
        // Patterns
        TESTS_ASSERT(cWildcardMatcher::match("blabla", "bla*"));
        TESTS_ASSERT(cWildcardMatcher::match("bla", "bla*"));
        TESTS_ASSERT(cWildcardMatcher::match("blaBLablBLAbl", "bla*"));
        TESTS_ASSERT(cWildcardMatcher::match("BlaBLablBLAbl", "bla*", false));
        TESTS_ASSERT(!cWildcardMatcher::match("bl", "bla*"));
        TESTS_ASSERT(!cWildcardMatcher::match("Bla", "bla*"));
        // Begin patterns
        TESTS_ASSERT(cWildcardMatcher::match("bla", "*bla"));
        TESTS_ASSERT(cWildcardMatcher::match("abla", "*bla"));
        TESTS_ASSERT(cWildcardMatcher::match("aabla", "*bla"));
        TESTS_ASSERT(cWildcardMatcher::match("aaaaaabla", "*bla"));
        TESTS_ASSERT(!cWildcardMatcher::match("aaaaaabl", "*bla"));
        TESTS_ASSERT(!cWildcardMatcher::match("abl", "*bla"));
        TESTS_ASSERT(!cWildcardMatcher::match("bl", "*bla"));
        // Complicate patterns
        TESTS_ASSERT(cWildcardMatcher::match("blaABCDblaABCD1", "bla*bla*1"));
        TESTS_ASSERT(cWildcardMatcher::match("blabla1", "bla*bla*1"));
        TESTS_ASSERT(!cWildcardMatcher::match("blabca1", "bla*bla*1"));
    }

    void test_trim()
    {
        cString temp;
        // Trim test
        temp = "bla bla   bla  blab"; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "bla bla   bla  blab");
        temp = ""; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "");
        temp = "a"; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "a");
        temp = " a"; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "a");
        temp = "    a a"; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "a a");
        temp = "        a          a"; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "a          a");
        temp = "a "; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "a");
        temp = "a a       "; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "a a");
        temp = "a          a        "; temp.trim();
        TESTS_ASSERT_EQUAL(temp, "a          a");

        // Test trim all spaces
        temp = "bla bla bla blab"; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "bla bla bla blab");
        temp = ""; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "");
        temp = "a"; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "a");
        temp = " a"; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "a");
        temp = "    a a"; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "a a");
        temp = "a "; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "a");
        temp = "a a       "; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "a a");

        temp = "        a          a"; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "a a");
        temp = "a          a        "; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "a a");
        temp = "bla bla   bla  blab"; temp.trimAllSpaces();
        TESTS_ASSERT_EQUAL(temp, "bla bla bla blab");
    }

    // Perform the test
    virtual void test()
    {
        test_wildcard();
        test_right();
        test_mid();
        test_constructor();
        test_operator_LZ();
        test_find();
        test_split();
        test_left();
        test_trim();
    };

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

const character cTestString::ABC[] = XSTL_STRING("abcdefghijklmnopqrstuvwxyz");

// Instance test object
cTestString g_globalTestString;

