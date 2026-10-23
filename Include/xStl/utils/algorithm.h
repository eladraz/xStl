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

#ifndef __TBA_STL_ALGORITHM_H
#define __TBA_STL_ALGORITHM_H

/**
 * @addtogroup xstl_utils
 * @{
 */

/**
 * @file algorithm.h
 * @brief Provide a set of template algorithms.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/except/assert.h"

/**
 * @brief Return the lower of two values.
 *
 * Since the Visual C++ compiler uses the function names "min" and "max" as
 * reserved words, the names t_min and t_max are used. T must have operator `<`.
 * A copy constructor is not required.
 *
 * @tparam T The type of the values.
 * @param a The first value.
 * @param b The second value.
 * @return The lower of the two values.
 */
template <class T>
const T& t_min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}
/**
 * @brief Return the greater of two values.
 *
 * See t_min. T must have operator `<`.
 *
 * @tparam T The type of the values.
 * @param a The first value.
 * @param b The second value.
 * @return The greater of the two values.
 */
template <class T>
const T& t_max(const T& a, const T& b)
{
    return (a < b) ? b : a;
}


/**
 * @brief Swap between two variables.
 *
 * T must have a copy constructor and an assignment operator (=).
 *
 * @tparam T The type of the variables.
 * @param a The first variable.
 * @param b The second variable.
 */
template <class T>
void t_swap(T& a, T& b)
{
    T c(a);
    a = b;
    b = c;
}

/**
 * @brief Convert a number to its positive value.
 *
 * T must have operator `<`, unary operator `-` and a copy constructor.
 *
 * @tparam T The type of the number.
 * @param a The number.
 * @return The absolute value of @a a.
 */
template <class T>
T t_abs(const T& a)
{
    if (a < 0) return -a;
    return a;
}

/**
 * @brief Scan a range for an element and return an iterator to it.
 *
 * The scan range is from @a begin to @a end, not including @a end. The function
 * uses a forward iterator (which has operator ++) and compares the elements
 * with operator ==. If the element is not found then the @a end iterator is
 * returned.
 *
 * @tparam ITR_FW The type of the forward iterator.
 * @tparam DATA The type of the searched object.
 * @param begin The iterator to the beginning of the range.
 * @param end The iterator to the end of the range.
 * @param object The object to look for.
 * @return An iterator to the first element which equals @a object, or @a end if
 *    it was not found.
 */
template <class ITR_FW, class DATA>
ITR_FW find(ITR_FW begin, ITR_FW end, DATA object)
{
    ITR_FW index = begin;
    while (index != end)
    {
        if (*index == object)
        {
            return index;
        }
        ++index;
    }
    return end;
}

/**
 * @brief Fill a range with an object.
 *
 * The range is from @a begin to @a end, not including @a end. The function uses
 * a forward iterator (which has operator ++).
 *
 * @tparam ITR_FW The type of the forward iterator.
 * @tparam DATA The type of the object.
 * @param begin The iterator to the beginning of the range.
 * @param end The iterator to the end of the range.
 * @param object The object to fill the range with.
 */
template <class ITR_FW, class DATA>
void fill(ITR_FW begin, ITR_FW end, DATA object)
{
    ITR_FW index = begin;
    while (index != end)
    {
        *index = object;
        index++;
    }
}

/**
 * @brief Compare two data structures: the section b1..e1 to the section b2..e2.
 *
 * The number of elements and their order must be the same.
 *
 * @tparam ITR_FW The type of the forward iterator.
 * @param b1 The beginning of the first range.
 * @param e1 The end of the first range.
 * @param b2 The beginning of the second range.
 * @param e2 The end of the second range.
 * @return True if both data-lists are equal.
 * @warning The function is not implemented yet. It contains `ASSERT(false)` and
 *    always returns true.
 */
template <class ITR_FW>
bool compare(ITR_FW b1, ITR_FW e1, ITR_FW b2, ITR_FW e2)
{
    ASSERT(false);
    // TODO!
    return true;
}

/*
    LIST_NODE<T> *src_data;
    LIST_NODE<T> *dst_data;

    src_data = this->m_first;
    dst_data = other.m_first;

    while ((src_data != NULL) && (dst_data != NULL))
    {
        if (!(src_data->information == dst_data->information))
        {
            // Both fields are not equal, return false
            return false;
        }
        // Carry on to the next data link
        src_data = src_data->next;
        dst_data = dst_data->next;
    }

    // In case the lists are not in the same order
    if ((src_data == NULL) && (dst_data == NULL))
    {
        return true;
    }

    return false;
*/

/**
 * @brief Sort a range using a bubble-sort and a bidirectional iterator.
 *
 * Later implementations on other iterators will be welcome. The elements are
 * ordered by operator `>`.
 *
 * @tparam ITR_BD The type of the bidirectional iterator.
 * @param begin The iterator to the beginning of the range.
 * @param end The iterator to the end of the range.
 */
template <class ITR_BD>
void bubbleSort(ITR_BD begin, ITR_BD end)
{
    if (begin == end)
        return;
    ITR_BD last = end - 1;
    ITR_BD prev_j = end;

    if (begin == last)
        return;

    for (ITR_BD i = begin; i != last; i++)
        for (ITR_BD j = last; j != i; j--)
        {
            prev_j = j - 1;
            if (*prev_j > *j)
            {
                t_swap(*j, *prev_j);
            }
        }
}

/**
 * @brief Rotate a number to the left by a fixed amount.
 *
 * Taken from the crypto template library. The amount must be lower than the
 * number of bits of T.
 *
 * @tparam T The type of the number.
 * @param x The number to rotate.
 * @param y The number of bits to rotate by.
 * @return The rotated number.
 */
template <class T> inline T rotlFixed(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x<<y) | (x>>(sizeof(T)*8-y));
}

/**
 * @brief Rotate a number to the right by a fixed amount. See rotlFixed.
 *
 * @tparam T The type of the number.
 * @param x The number to rotate.
 * @param y The number of bits to rotate by.
 * @return The rotated number.
 */
template <class T> inline T rotrFixed(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x>>y) | (x<<(sizeof(T)*8-y));
}

/**
 * @brief Rotate a number to the left by a variable amount. See rotlFixed.
 *
 * @tparam T The type of the number.
 * @param x The number to rotate.
 * @param y The number of bits to rotate by.
 * @return The rotated number.
 */
template <class T> inline T rotlVariable(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x<<y) | (x>>(sizeof(T)*8-y));
}

/**
 * @brief Rotate a number to the right by a variable amount. See rotlFixed.
 *
 * @tparam T The type of the number.
 * @param x The number to rotate.
 * @param y The number of bits to rotate by.
 * @return The rotated number.
 */
template <class T> inline T rotrVariable(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x>>y) | (x<<(sizeof(T)*8-y));
}

/**
 * @brief Rotate a number to the left. The amount is taken modulo the number of
 *    bits of T.
 *
 * @tparam T The type of the number.
 * @param x The number to rotate.
 * @param y The number of bits to rotate by.
 * @return The rotated number.
 */
template <class T> inline T rotlMod(T x, unsigned int y)
{
    y %= sizeof(T)*8;
    return (x<<y) | (x>>(sizeof(T)*8-y));
}

/**
 * @brief Rotate a number to the right. The amount is taken modulo the number of
 *    bits of T.
 *
 * @tparam T The type of the number.
 * @param x The number to rotate.
 * @param y The number of bits to rotate by.
 * @return The rotated number.
 */
template <class T> inline T rotrMod(T x, unsigned int y)
{
    y %= sizeof(T)*8;
    return (x>>y) | (x<<(sizeof(T)*8-y));
}


/** @} */
#endif // __TBA_STL_ALGORITHM_H
