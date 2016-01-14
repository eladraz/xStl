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

#ifndef __TBA_XSTL_OPERATORS_H
#define __TBA_XSTL_OPERATORS_H

/*
 * operators.h
 *
 * Define the MAKE_SIMPLE_OPERATORS macro which creates all standard comparing
 * operators by invoking calls to operators == and operator < only.
 * The class also deifne  the MAKE_INCREASE_OPERATORS which implement operator
 * ++, -- by calling to next() and prev() function.
 *
 * Author: Elad Raz <e@eladraz.com>
 */


/*
 * Creates default implementation of operators:
 *    !=   Not equal
 *    >    Bigger
 *    >=   Bigger or equal
 *    <=   Less or equal
 * By dispatching a call to operator == or operator < which should
 * be implement by the class.
 */
#define MAKE_SIMPLE_OPERATORS(className) \
    bool inline operator != (const className &other) const   \
    {                                                        \
        return !(*this == other);                            \
    }                                                        \
    bool inline operator >  (const className &other) const   \
    {                                                        \
        return ((!(*this < other)) && (!(*this == other)));  \
    }                                                        \
    bool inline operator >= (const className &other) const   \
    {                                                        \
        return !(*this < other);                             \
    }                                                        \
    bool inline operator <= (const className &other) const   \
    {                                                        \
        return ((*this < other) || (*this == other));        \
    }

/*
 * Using function 'void next()' and 'void prev()' function implement the
 * increasment operators.
 */
#define MAKE_INCREASE_OPERATORS(className) \
    className& operator++()                \
    {                                      \
        next();                            \
        return *this;                      \
    }                                      \
    className& operator--()                \
    {                                      \
        prev();                            \
        return *this;                      \
    }                                      \
    className& operator++(int)             \
    {                                      \
        className& tmp = *this;            \
        ++*this;                           \
        return tmp;                        \
    }                                      \
    className& operator--(int)             \
    {                                      \
        className& tmp = *this;            \
        --*this;                           \
        return tmp;                        \
    }


#endif // __TBA_XSTL_OPERATORS_H
