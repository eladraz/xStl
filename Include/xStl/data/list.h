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

#ifndef __TBA_STL_LIST_H
#define __TBA_STL_LIST_H

/*
 * list.h
 *
 * Contains delcaration of link-list module.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/utils/algorithm.h"

#ifdef XSTL_WINDOWS
// Template classes might not use all the local functions the interface has
// to ofer. Warning C4505 should be over-written for template functions
#pragma warning(push)
#pragma warning(disable:4505)
#endif

/*
 * g++ and vc compiler changes
 */
#ifdef XSTL_LINUX
    #define cListT
#else
    #define cListT cList<T>::
#endif

/*
 * template cList class
 *
 * The declartion of the stl "list" class which include implementation of the
 * abstract layer of the list, and the implementation of the iterator class.
 *
 * The link-list copyies the element into internal data-struct.
 *
 * requires from T:
 *  T must have an equal == operator
 *  T should have copy constructor.
 *  T must NOT be a pointer because this class calles the destructor of T only,
 *    and doesn't assume that T is a pointer!
 */
template <class T>
class cList
{
public:
    // Forward declation of the iterator class.
    class iterator;

    /*
     * This is internal class which store information about the link-list.
     * The class store pointers to allow terminate entry in the link-list.
     * The implementation is inside the include file since out-side compilation
     * is carch the system
     */
    class ListNode
    {
    public:
        ListNode(const T* node) :
          information(NULL)
        {
            if (node != NULL)
                information = new T(*node);
        }

        ~ListNode()
        {
            if (information != NULL)
                delete information;
        }

        /* Saves the next and the previous pointers */
        // The content of the entry
        T* information;
        // The next element
        ListNode* next;
        // The previous element
        ListNode* prev;
    };

    /* Constructor and deconstructor of the list */

    /*
     * Constructor. Create empty link-list.
     */
    cList();

    /*
     * Destructor, free all entries in the link-list.
     */
    ~cList();

    /*
     * Copy constructor. Creates a clone of the link-list entries.
     */
    cList(const cList<T> &other);

    /*
     * Create a new link-list with one element 'object'
     */
    cList(const T& object);

    /*
     * Free the link-list memory and create a new list which will be an exact
     * copy of the original link.
     *
     * assignPointer - Incase the variable is true, the function makes the
     *                 internal pointer reference to the pointers of 'other'.
     *
     * Note: if the "assignPointer" flag will be on, WHICH IS
     *       NOT RECOMMANDED then a new link-list will be assigned
     *       to the original data type, and there will be a big probleam
     *       of memory closion in MTA (Multiple Thread Apartment).
     */
    void clone(const cList<T> &other, bool assignPointer = false);


    // Operators of the list
    // You can also use the iterator to do this functions

    /*
     * Append element to the link-list. The element is copy using copy-
     * constructor.
     *
     * node - The object to be added.
     */
    void append(const T& node);

    /*
     * Insert element to the beginning of the link-list. The element is
     * copy using copy-constructor.
     *
     * node - The object to be added.
     */
    void insert(const T& node);

    /*
     * Free the link-list, create an empty list.
     */
    void removeAll();

    /*
     * Determine whether the list has any items or not.
     * returns true if list has no items. false if it has item(s).
     */
    bool isEmpty() const;

    /*
     * Insert element to the link-list before 'position'
     */
    void insert(iterator &position, const T& node);

    /*
     * Remove a node from the list. After the call the iterator will become
     * invalid. Don't use it.
     *
     * position - The iterator position.
     */
    iterator remove(iterator &position);

    /*
     * Try to find an object and return true if it is there
     */
    bool isIn(const T& object);

    /*
     * Try to find an object and remove it from the list
     */
    void remove(const T& object);

    /*
     * Return the length of the link-list.
     */
    uint length() const;

    /*
     * Construct a new iterator that will point out to the start of the array.
     *
     * Note: if the iterator is at the end of the list any attempts to access
     *       it will cause at exception.
     *
     * NOTE: Although the function declare as const, the iterator can be used
     *       for non const operation.
     *
     * NOTE: Microsoft visual studio .NET version 7.10 cannot compile the
     *       code of the function if it's found outside the class deceleration.
     */
    iterator begin() const
    {
        #ifdef XSTL_LINUX
            return typename cList<T>::iterator::iterator(m_first);
        #else
        return cList<T>::iterator::iterator(m_first);
        #endif
    }

    /*
     * Construct a new iterator that will point out to the end of the array.
     * Any attempts to access it will cause at exception.
     *
     * NOTE: Although the function declare as const, the iterator can be used
     *       for non const operation.
     *
     * NOTE: Microsoft visual studio .NET version 7.10 cannot compile the
     *       code of the function if it's found outside the class deceleration.
     */
    iterator end() const
    {
        #ifdef XSTL_LINUX
            return typename cList<T>::iterator::iterator(m_last);
        #else
        return cList<T>::iterator::iterator(m_last);
        #endif
    }

    // Operators

    /*
     * Return true if the two link-list are equals, all internal elements
     * compare with operator == are TRUE.
     * TODO: compare is not ready yet.
     */
    bool operator == (const cList<T> &other);

    /*
     * Return the opposite from operator ==.
     */
    bool operator != (const cList<T> &other);

    /*
     * Copy operator.
     */
    cList<T> & operator = (const cList<T> &other);

    // TODO: To create a normal list.
    // cList<T> operator , (const cList<T> &other, const T& object);

    // Dumping operator
    /*
     * Printing the list data into output stream.
     * Usually it will be to the standart output.
     *
     * Using:
     *    cout << "My list is: " << cList<T>(1, 2, 3) << endl;
     */
    template <class S, class OSTREAM>
    friend OSTREAM & operator << (OSTREAM &out, cList<S> &object);

    /*
     * iterator class
     *
     * Inline interface + definition of the iterator
     * class which access the list as a bidirctional
     * iterator.
     */
    friend class iterator;
    class iterator
    {
    friend class cList<T>;
    public:
        /*
         * cList::iterator::iterator
         *
         * Constructor. Used only in the cList class.
         */
        iterator(ListNode *position)
        {
            m_position = position;
            assertIndex();
        }
        /*
         * cList::iterator::iterator
         *
         * Copy Constructor.
         */
        iterator(const iterator& other)
        {
            m_position = other.m_position;
        }
        /*
         * cList::iterator::~iterator, Deconstructor
         */
        ~iterator()
        {
        }
        /*
         * cList::iterator::next
         *
         * Going to the next element in the list.
         */
        void next()
        {
            m_position = m_position->next;
            assertIndex();
        }
        /*
         * cList::iterator::prev
         *
         * Going to the previous element in the list.
         */
        void prev()
        {
            m_position = m_position->prev;
            assertIndex();
        }

        /*
         * Return whether two iterator points to the same location.
         */
        bool operator == (const iterator &other) const
        {
            if (m_position == other.m_position)
            {
                return true;
            }
            return false;
        }

        /*
         * Copy iterator.
         */
        iterator& operator =  (const iterator &other)
        {
            if (*this != other)
            {
                this->m_position = other.m_position;
            }
            return *this;
        }

        /*
         * Is iterator is less than other.
         */
        bool operator < (const iterator &other) const
        {
            /*
             * Scaning the next of the list to see whether the
             * element is there.
             */
            ListNode *index = m_position;
            while (index != NULL)
            {
                if (index == other.m_position)
                {
                    return true;
                }
                index = index->next;
            }
            return false;
        }

        /*
         * operator *
         *
         * Return a refrence to the data.
         */
        T& operator *()
        {
            return *(m_position->information);
        }

        /*
         * const operator *
         *
         * Return a refrence to the data.
         */
        const T& operator *() const
        {
            return *(m_position->information);
        }


        /*
         * operator ->
         *
         * Return a pointer to the data.
         * TODO! Check this code EVC telling an error
         *
        T* operator ->()
        {
            return m_position->information;
        }
         */

        /*
         * Implement operator >, !=, >=, <=
         */
        MAKE_SIMPLE_OPERATORS(iterator);

        /*
         * Implement the two operators ++ and the two operators --.
         */
        MAKE_INCREASE_OPERATORS(iterator);

        /*
         * Increase the iterator by number of elements.
         */
        iterator operator+ (const uint count)
        {
            ListNode *tmp = m_position;
            for (uint i = 0; i < count; i++)
            {
                tmp = tmp->next;
                assertIndex();
            }
            return iterator(tmp);
        }

        /*
         * Decrease the iterator by number of elements.
         */
        iterator operator- (const uint count)
        {
            ListNode *tmp = m_position;
            for (uint i = 0; i < count; i++)
            {
                tmp = tmp->prev;
                assertIndex();
            }
            return iterator(tmp);
        }
    private:
        /*
         * assertIndex()
         *
         * Throw exception if the index is invalid.
         */
        inline void assertIndex()
        {
            ASSERT(m_position != NULL);
        }
        /* Inline data about the list position */
        ListNode *m_position;
    };

private:
    /*
     * Private member and function
     *
     * The following function are used internal in the class and it's not
     * recommand to use them for something else.
     */

    /*
     * Clear the class memory.
     * Destory the link-list and free the memory.
     */
    void freeList();

    /*
     * Create an empty list
     * Init an empty link-list, create the first pointers.
     */
    void initList();

    // Members

    // Pointer to the first element in the link-list
    ListNode *m_first;

    // Pointer to the last element in the link-list
    ListNode *m_last;
};

// Include implementation. This is nessasary in template classes.
#include "xStl/data/list.inl"

#ifdef XSTL_WINDOWS
    // Return the warnning behaviour back to it's original mode
    #pragma warning(pop)
#endif

#endif // __TBA_STL_LIST_H
