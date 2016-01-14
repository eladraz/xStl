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

/*
 * algorithm.h
 *
 * Provide a set of templates algorithm.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/except/assert.h"

/*
 * T& t_min (a,b)
 * T& t_max (a,b)
 *
 * Returns the lower/greater between two variables.
 * T must have operator < or >. Copy constructor is not requierd.
 *
 * Since the compiler of the Visual C++ use the functions names "min", "max"
 * as reserved words, I used the name t_min, t_max.
 */
template <class T>
const T& t_min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}
template <class T>
const T& t_max(const T& a, const T& b)
{
    return (a < b) ? b : a;
}


/*
 * t_swap(a,b) template function
 *
 * Swap between the variables.
 * T must have a copy constructor and assignment operator (=)
 */
template <class T>
void t_swap(T& a, T& b)
{
    T c(a);
    a = b;
    b = c;
}

/*
 * T t_abs(const T& a)
 *
 * Covert a number to it's positive value. T must operator <, operator - and
 * copy constructor.
 */
template <class T>
T t_abs(const T& a)
{
    if (a < 0) return -a;
    return a;
}

/*
 * iterator find(begin, end, object)
 *
 * Template function for iterator and data object. Binary scan a range and
 * finds an elemnet and return the iterator index. If the element is not found
 * then find return "end" iterator.
 * The scan range is from "begin" to "end" not including "end".
 *
 * The function uses forward iterator (which has operator ++).
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

/*
 * fill(begin, end, data)
 *
 * Fill data-list with 'data' object. The range of the filling is from
 * "begin" to "end" not including "end"
 *
 * The function uses forward iterator (which has operator ++).
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

/*
 * bool compare(b1,e1, b2,e2)
 *
 * Compare between two data struct. It compare section b1..e1 to b2..e2.
 * Returns true if two data-lists are equals.
 *
 * The number of elements and their order must be the same.
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
            // Both fileds are not equal, return false
            return false;
        }
        // Carry on to the next data link
        src_data = src_data->next;
        dst_data = dst_data->next;
    }

    // In case that the list are not the same in the orders
    if ((src_data == NULL) && (dst_data == NULL))
    {
        return true;
    }

    return false;
*/

/*
 * boubbleSort(begin, end)
 *
 * Sort a struct using bidirectonal iterator. Later implementation on other
 * iterator will be welcome.
 */
template <class ITR_BD>
void boubbleSort(ITR_BD begin, ITR_BD end)
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

/*
 * Template rotation function.
 *
 * Rotate a number to the left or to the right.
 *
 * Taken from crypto template library.
 */
template <class T> inline T rotlFixed(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x<<y) | (x>>(sizeof(T)*8-y));
}

template <class T> inline T rotrFixed(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x>>y) | (x<<(sizeof(T)*8-y));
}

template <class T> inline T rotlVariable(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x<<y) | (x>>(sizeof(T)*8-y));
}

template <class T> inline T rotrVariable(T x, unsigned int y)
{
    ASSERT(y < sizeof(T)*8);
    return (x>>y) | (x<<(sizeof(T)*8-y));
}

template <class T> inline T rotlMod(T x, unsigned int y)
{
    y %= sizeof(T)*8;
    return (x<<y) | (x>>(sizeof(T)*8-y));
}

template <class T> inline T rotrMod(T x, unsigned int y)
{
    y %= sizeof(T)*8;
    return (x>>y) | (x<<(sizeof(T)*8-y));
}

#endif // __TBA_STL_ALGORITHM_H
