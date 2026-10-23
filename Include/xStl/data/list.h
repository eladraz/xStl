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

/**
 * @addtogroup xstl_data_containers
 * @{
 */

/**
 * @file list.h
 * @brief Contains declaration of link-list module.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/utils/algorithm.h"

#ifdef XSTL_WINDOWS
// Template classes might not use all the local functions the interface has
// to offer. Warning C4505 should be disabled for template functions
#pragma warning(push)
#pragma warning(disable:4505)
#endif

/*
 * g++ and vc compiler changes
 */
#ifdef XSTL_LINUX
    /**
     * @brief Qualifier prefix for the nested types of cList in out-of-class
     *    definitions.
     *
     * Empty on Linux (g++), `cList<T>::` on the other platforms.
     */
    #define cListT
#else
    /**
     * @brief Qualifier prefix for the nested types of cList in out-of-class
     *    definitions (`cList<T>::`).
     */
    #define cListT cList<T>::
#endif

/**
 * @brief The declaration of the stl "list" class which includes implementation
 *    of
 *   the abstract layer of the list, and the implementation of the iterator
 *   class.
 *
 * @details The link-list copies the element into internal data-struct.
 *
 * Requirements for T:
 * T must have an equal == operator
 * T should have copy constructor.
 * T must NOT be a pointer because this class calls the destructor of T only,
 * and doesn't assume that T is a pointer!
 *
 * @tparam T The type of the elements. It must implement operator == and should
 *    have a copy constructor. It must not be a pointer type.
 */
template <class T>
class cList
{
public:
    // Forward declaration of the iterator class.
    class iterator;

    /**
     * @brief This is an internal class which stores information about the
     *    link-list.
     *
     * @details The class stores pointers to allow a terminating entry in the
     *    link-list. The implementation is inside the include file since
     *    out-side compilation crashes the system
     */
    class ListNode
    {
    public:
        /**
         * @brief Constructor. Copy the node content to a new entry.
         *
         * @param node The content of the entry. If it is NULL the entry holds
         *    no content (terminating entry).
         */
        ListNode(const T* node) :
          information(NULL)
        {
            if (node != NULL)
                information = new T(*node);
        }

        /**
         * @brief Destructor. Free the content of the entry.
         */
        ~ListNode()
        {
            if (information != NULL)
                delete information;
        }

        /* Saves the next and the previous pointers */
        /**
         * @brief The content of the entry
         */
        T* information;
        /**
         * @brief The next element
         */
        ListNode* next;
        /**
         * @brief The previous element
         */
        ListNode* prev;
    };

    /* Constructor and destructor of the list */

    /**
     * @brief Constructor. Create empty link-list.
     */
    cList();

    /**
     * @brief Destructor, free all entries in the link-list.
     */
    ~cList();

    /**
     * @brief Copy constructor. Creates a clone of the link-list entries.
     *
     * @param other The list to clone.
     */
    cList(const cList<T> &other);

    /**
     * @brief Create a new link-list with one element 'object'
     *
     * @param object The only element of the new list.
     */
    cList(const T& object);

    /**
     * @brief Free the link-list memory and create a new list which will be an
     *    exact
     *   copy of the original link.
     *
     * @param assignPointer In case the variable is true, the function makes the
     *    internal pointer reference to the pointers of 'other'.
     * @note if the "assignPointer" flag is on, WHICH IS NOT RECOMMENDED, then a
     *    new link-list will be assigned to the original data type, and there
     *    will be a big problem of memory collision in MTA (Multiple Thread
     *    Apartment).
     * @param other The list to copy from.
     */
    void clone(const cList<T> &other, bool assignPointer = false);


    // Operators of the list
    // You can also use the iterator to do these functions

    /**
     * @brief Append an element to the link-list.
     *
     * @details The element is copied using copy- constructor.
     *
     * @param node The object to be added.
     */
    void append(const T& node);

    /**
     * @brief Insert an element at the beginning of the link-list.
     *
     * @details The element is copied using the copy-constructor.
     *
     * @param node The object to be added.
     */
    void insert(const T& node);

    /**
     * @brief Free the link-list, create an empty list.
     */
    void removeAll();

    /**
     * @brief Determine whether the list has any items or not.
     *
     * @return True if the list has no items, false if it has item(s).
     */
    bool isEmpty() const;

    /**
     * @brief Insert an element into the link-list before 'position'
     *
     * @param position The element before which the new element is inserted.
     * @param node The object to be added. It is copied using the
     *    copy-constructor.
     */
    void insert(iterator &position, const T& node);

    /**
     * @brief Remove a node from the list.
     *
     * @details After the call the iterator will become invalid. Don't use it.
     *
     * @param position The iterator position.
     * @return An iterator to the element which follows the removed one.
     */
    iterator remove(iterator &position);

    /**
     * @brief Try to find an object and return true if it is there
     *
     * @param object The object to look for.
     * @return True if the object is in the list.
     */
    bool isIn(const T& object);

    /**
     * @brief Try to find an object and remove it from the list
     *
     * @param object The object to remove.
     */
    void remove(const T& object);

    /**
     * @brief Return the length of the link-list.
     *
     * @return The length of the link-list.
     */
    uint length() const;

    /**
     * @brief Construct a new iterator that will point to the start of the list.
     *
     * @note if the iterator is at the end of the list any attempts to access it
     *    will cause an exception.
     * @note Although the function is declared as const, the iterator can be
     *    used for non-const operations.
     * @note Microsoft Visual Studio .NET version 7.10 cannot compile the code
     *    of the function if it's found outside the class declaration.
     * @return An iterator to the first element of the list.
     */
    iterator begin() const
    {
        #ifdef XSTL_LINUX
            return typename cList<T>::iterator::iterator(m_first);
        #else
        return cList<T>::iterator::iterator(m_first);
        #endif
    }

    /**
     * @brief Construct a new iterator that will point to the end of the list.
     *
     * @details Any attempts to access it will cause an exception.
     *
     * @note Although the function is declared as const, the iterator can be
     *    used for non-const operations.
     * @note Microsoft Visual Studio .NET version 7.10 cannot compile the code
     *    of the function if it's found outside the class declaration.
     * @return An iterator which points after the last element of the list.
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

    /**
     * @brief Return true if the two link-list are equal, all internal elements
     *    compare
     *   with operator == are TRUE.
     *
     * @todo compare is not ready yet.
     * @param other The list to compare with.
     * @return True if both lists are equal.
     */
    bool operator == (const cList<T> &other);

    /**
     * @brief Return the opposite of operator ==.
     *
     * @param other The list to compare with.
     * @return True if the lists are not equal.
     */
    bool operator != (const cList<T> &other);

    /**
     * @brief Copy operator.
     *
     * @param other The list to copy from.
     * @return A reference to this list.
     */
    cList<T> & operator = (const cList<T> &other);

    // TODO: To create a normal list.
    // cList<T> operator , (const cList<T> &other, const T& object);

    // Dumping operator
    /**
     * @brief Print the list data to an output stream.
     *
     * @details Usually it will be to the standard output.
     *
     * Using:
     *
     * @code
     * cout << "My list is: " << cList<T>(1, 2, 3) << endl;
     * @endcode
     */
    template <class S, class OSTREAM>
    friend OSTREAM & operator << (OSTREAM &out, cList<S> &object);

    /*
     * iterator class
     *
     * Inline interface + definition of the iterator
     * class which accesses the list as a bidirectional
     * iterator.
     */
    friend class iterator;
    /**
     * @brief A bidirectional iterator over the elements of a cList.
     */
    class iterator
    {
    friend class cList<T>;
    public:
        /**
         * @brief Constructor. Used only in the cList class.
         *
         * @param position The list entry the iterator points to.
         */
        iterator(ListNode *position)
        {
            m_position = position;
            assertIndex();
        }
        /**
         * @brief Copy Constructor.
         *
         * @param other The iterator to copy.
         */
        iterator(const iterator& other)
        {
            m_position = other.m_position;
        }
        /**
         * @brief cList::iterator::~iterator, Destructor
         */
        ~iterator()
        {
        }
        /**
         * @brief Going to the next element in the list.
         */
        void next()
        {
            m_position = m_position->next;
            assertIndex();
        }
        /**
         * @brief Going to the previous element in the list.
         */
        void prev()
        {
            m_position = m_position->prev;
            assertIndex();
        }

        /**
         * @brief Return whether two iterators point to the same location.
         *
         * @param other The iterator to compare with.
         * @return True if both iterators point to the same entry.
         */
        bool operator == (const iterator &other) const
        {
            if (m_position == other.m_position)
            {
                return true;
            }
            return false;
        }

        /**
         * @brief Copy iterator.
         *
         * @param other The iterator to copy from.
         * @return A reference to this iterator.
         */
        iterator& operator =  (const iterator &other)
        {
            if (*this != other)
            {
                this->m_position = other.m_position;
            }
            return *this;
        }

        /**
         * @brief Check whether another iterator is reachable from this iterator
         *    by moving forward.
         *
         * The list is scanned from the position of this iterator to its end.
         *
         * @param other The iterator to look for.
         * @return True if the position of @a other is the position of this
         *    iterator or follows it.
         */
        bool operator < (const iterator &other) const
        {
            /*
             * Scanning the next of the list to see whether the
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

        /**
         * @brief operator *
         *
         * @return A reference to the data.
         */
        T& operator *()
        {
            return *(m_position->information);
        }

        /**
         * @brief const operator *
         *
         * @return A reference to the data.
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

        /**
         * @brief Implement operator \>, !=, \>=, \<=
         */
        MAKE_SIMPLE_OPERATORS(iterator);

        /**
         * @brief Implement the two operators ++ and the two operators --.
         */
        MAKE_INCREASE_OPERATORS(iterator);

        /**
         * @brief Increase the iterator by a number of elements.
         *
         * @param count The number of elements to advance.
         * @return The advanced iterator.
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

        /**
         * @brief Decrease the iterator by a number of elements.
         *
         * @param count The number of elements to move back.
         * @return The iterator which was moved back.
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
        /**
         * @brief assertIndex()
         *
         * @throws cException if the index is invalid.
         */
        inline void assertIndex()
        {
            ASSERT(m_position != NULL);
        }
        /**
         * @brief Inline data about the list position
         */
        ListNode *m_position;
    };

private:
    /*
     * Private member and function
     *
     * The following functions are used internally in the class and it is not
     * recommended to use them for anything else.
     */

    /**
     * @brief Clear the class memory.
     *
     * @details Destroy the link-list and free the memory.
     */
    void freeList();

    /**
     * @brief Create an empty list Init an empty link-list, create the first
     *    pointers.
     */
    void initList();

    // Members

    /**
     * @brief Pointer to the first element in the link-list
     */
    ListNode *m_first;

    /**
     * @brief Pointer to the last element in the link-list
     */
    ListNode *m_last;
};

// Include implementation. This is necessary in template classes.
#include "xStl/data/list.inl"

#ifdef XSTL_WINDOWS
    // Return the warning behaviour back to its original mode
    #pragma warning(pop)
#endif


/** @} */
#endif // __TBA_STL_LIST_H
