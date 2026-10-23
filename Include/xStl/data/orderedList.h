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

/**
 * @addtogroup xstl_data_containers
 * @{
 */

/**
 * @file orderedList.h
 * @brief Contains declaration of ordered-list module.
 */
#include "list.h"

/*
 * g++ and vc compiler changes
 */
#ifdef XSTL_LINUX
    /**
     * @brief Qualifier prefix for the nested types of cList in out-of-class
     *    definitions (`cList<T>::`).
     */
    #define cListT
#else
    #ifdef XSTL_WINDOWS
        // Template classes might not use all the local functions the interface has
        // to offer. Warning C4505 should be disabled for template functions
        #pragma warning(push)
        #pragma warning(disable:4505)
    #endif

    /**
     * @brief Qualifier prefix (`cList<T>::`) for the nested types of the base
     *    class in out-of-class definitions.
     */
    #define cListT cList<T>::
#endif

/**
 * @brief A template derived from cList, which keeps the items ordered, as long
 *    as
 *   only add() is used to add items
 *
 * @details requires from T:
 *   T must have a \< operator
 *   T must have an equal == operator
 *   T should have copy constructor.
 *   T must NOT be a pointer because this class calls the destructor of T only,
 *   and doesn't assume that T is a pointer!
 *
 * Author: Ilan Tayari
 *
 * @tparam T The type of the elements. It must implement operator < in addition
 *    to the cList requirements.
 */
template <class T>
class cOrderedList : public cList<T>
{
public:
    /**
     * @brief Constructor. Create empty ordered-list.
     */
    cOrderedList();

    /**
     * @brief Copy constructor. Creates a clone of the ordered-list entries.
     *
     * @param other The list to clone.
     */
    cOrderedList(const cOrderedList<T> &other);

    // Operators of the list
    // You can also use the iterator to do this functions

    /**
     * @brief Add element to the ordered-list, at its correct location.
     *
     * @details The element is copied using copy-constructor.
     *
     * @param node The object to be added.
     */
    void add(const T& node);

    /**
     * @brief Returns a reference to the first (lowest) item
     *
     * @return A reference to the first (lowest) item.
     * @throws cException if empty
     */
    const T& first() const;

    /**
     * @brief Returns a reference to the first (lowest) item
     *
     * @return A reference to the first (lowest) item.
     * @throws cException if empty
     */
    T& first();

    /**
     * @brief Removes the first (lowest) item
     *
     * @throws cException if empty
     */
    void pop2null();

    /**
     * @brief See cList\<T\>::isEmpty()
     *
     * @return True if the list has no items.
     */
    bool isEmpty() const;
};

// Include implementation. This is necessary in template classes.
#include "xStl/data/orderedList.inl"

#ifdef XSTL_WINDOWS
    // Return the warning behaviour back to its original mode
    #pragma warning(pop)
#endif


/** @} */
#endif // __TBA_STL_ORDEREDLIST_H
