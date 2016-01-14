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

#ifndef __TBA_STL_ORDEREDLIST_H
#define __TBA_STL_ORDEREDLIST_H

/*
 * orderedList.h
 *
 * Contains delcaration of ordered-list module.
 */
#include "list.h"

/*
 * g++ and vc compiler changes
 */
#ifdef XSTL_LINUX
    #define cListT
#else
    #ifdef XSTL_WINDOWS
        // Template classes might not use all the local functions the interface has
        // to ofer. Warning C4505 should be over-written for template functions
        #pragma warning(push)
        #pragma warning(disable:4505)
    #endif

    #define cListT cList<T>::
#endif

/*
 * template cOrderedList class
 *
 * A template derived from cList, which keeps the items ordered,
 * as long as only add() is used to add items
 *
 * requires from T:
 *  T must have a < operator
 *  T must have an equal == operator
 *  T should have copy constructor.
 *  T must NOT be a pointer because this class calles the destructor of T only,
 *    and doesn't assume that T is a pointer!
 *
 * Author: Ilan Tayari
 */
template <class T>
class cOrderedList : public cList<T>
{
public:
    /*
     * Constructor. Create empty ordered-list.
     */
    cOrderedList();

    /*
     * Copy constructor. Creates a clone of the ordered-list entries.
     */
    cOrderedList(const cOrderedList<T> &other);

    // Operators of the list
    // You can also use the iterator to do this functions

    /*
     * Add element to the ordered-list, at its correct location.
     * The element is copied using copy-constructor.
     *
     * node - The object to be added.
     */
    void add(const T& node);

    /*
     * Returns a reference to the first (lowest) item
     * Throws exception if empty
     */
    const T& first() const;

    /*
     * Returns a reference to the first (lowest) item
     * Throws exception if empty
     */
    T& first();

    /*
     * Removes the first (lowest) item
     * Throws exception if empty
     */
    void pop2null();

    // See cList<T>::isEmpty()
    bool isEmpty() const;
};

// Include implementation. This is nessasary in template classes.
#include "xStl/data/orderedList.inl"

#ifdef XSTL_WINDOWS
    // Return the warnning behaviour back to it's original mode
    #pragma warning(pop)
#endif

#endif // __TBA_STL_ORDEREDLIST_H
