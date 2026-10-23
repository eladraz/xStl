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

/**
 * @addtogroup xstl_core
 * @{
 */

/**
 * @file operators.h
 * @brief Define the MAKE_SIMPLE_OPERATORS macro which creates all standard
 *   comparison operators by invoking calls to operators == and operator \<
 *   only.
 *
 * @details The class also defines the MAKE_INCREASE_OPERATORS which implements
 *    operators ++, -- by calling the next() and prev() functions.
 *
 * @author Elad Raz
 */


/**
 * @brief Generate the comparison operators `!=`, `>`, `>=` and `<=` for a
 *    class.
 *
 * The generated operators are implemented with the operators `==` and `<` of
 * the class, which the class must therefore provide.
 *
 * @param className The name of the class for which the operators are generated.
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

/**
 * @brief Generate the prefix and postfix `++` and `--` operators for a class.
 *
 * The `++` operators call the member function `next()` and the `--` operators
 * call the member function `prev()`, which the class must provide.
 *
 * @param className The name of the class for which the operators are generated.
 * @note The postfix versions return a reference to the object itself (after the
 *    change), not a copy of its previous state.
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



/** @} */
#endif // __TBA_XSTL_OPERATORS_H
