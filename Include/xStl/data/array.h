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

#ifndef __TBA_STL_ARRAY_H
#define __TBA_STL_ARRAY_H

/*
 * array.h
 *
 * Declaration on the global array memory handler. Provide a safe way to
 * allocate memory array. Handles all the thing that need to do.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/assert.h"

#ifdef XSTL_WINDOWS
// Template classes might not use all the local functions the interface has
// to ofer. Warning C4505 should be over-written for template functions
#pragma warning(push)
#pragma warning(disable:4505)
#endif

/*
 * cArray template class.
 *
 * Contains standard implementation of arraies. This is a template vector for
 * class T. T should have default constructor (due to using operator new[]).
 * operator == should be implement only for reference call to remove().
 *
 * The array also implement a 'paging' memory. The paging allow allocating more
 * memory for the array but when the size of the array is changed within the
 * page size, no memory allocation operation is called.
 *
 * PS cArray can be used for many call to changeSize()
 *    only if you configure the page_size variable right.
 *
 * T must have:
 *    default constructor.
 *    For using remove(T&) T must implement operator ==.
 *
 * TODO: Add to this class copy-on-write ablities (Reference count and unshare
 *       method).
 */
template <class T>
class cArray
{
public:
    // Forward deceleration for class iterator.
    class iterator;

    /*
     * Explicit constructor.
     *
     * Create an array at size 'numberOfElements'. The 'pageSize' determine
     * how many elements should allocated in addition to the needed items.
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    explicit cArray(uint numberOfElements = 0,
                    uint pageSize = 0);

    /*
     * Create array and copy 'staticArray' to the constructor array.
     *
     * staticArray - The source array.
     * length      - The number of elements in the array
     * pageSize    - The size of the paging
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    explicit cArray(const T* staticArray,
                    uint length,
                    uint pageSize = 0);

    /*
     * Copy constructor
     */
    cArray(const cArray<T>& other);

    /*
     * Virtual destructor. Used for cBuffer implementation.
     */
    virtual ~cArray();


    /* Class functions */

    /*
     * Return the buffer of the array. getBuffer() declare as const but return
     * non-const operator. The return type can be NULL if the array is empty.
     *
     * Note:
     * The return buffer is good for the following commands until other command
     * from the cArray class is invoked. Each function of the array can
     * reallocated the buffer pointer.
     */
    T* getBuffer() const;

    /*
     * Return the page size of the array object.
     */
    uint getPageSize() const;

    /*
     * Return the number of elements in the array.
     */
    uint getSize() const;

    /*
     * Replace the array pointers of the current array and 'other'
     *
     * NOTE: This function doesn't replace the pageSize values.
     * NOTE: This class is not thread-safe
     */
    void swap(cArray<T>& other);

    /*
     * Increase or decrease the count of elements in the array. Destroy the
     * previous memory if the preserverMemory is false. Otherwise, if the
     * newSize is smaller then the old one, then the padding will be freed.
     * If the new size is bigger then the old size, the new allocated fields
     * will not be initilazied. if the newSize is equal to zero, then the list
     * will be freed.
     */
    virtual void changeSize(uint newSize, bool preserveMemory = true);

    /*
     * Change the the page size of the array. The effect of this function is
     * on future called to the 'changeSize' function.
     */
    void setPageSize(uint newPageSize);

    /*
     * Like the python's operator [:], but mush less improve. Copy from the
     * array 'count' elements starting from 'startLocation' into 'other' array.
     * 'other' will be reset into 'count' elements array.
     *
     * other         - The output array
     * startLocation - The start location in the array
     * count         - The number of elements to copy.
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    virtual void copyRange(const cArray<T>& other,
                           uint startLocation,
                           uint count);

    /*
     * Insert single object to the end of the array. The array is expand by one
     * element and the 'object' is copyied to the end of the array.
     *
     * object - The object to append
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    virtual void append(const T& object);

    /*
     * Insert new array to the end of the array. The array is expand by one
     * element and the 'object' is copyied to the end of the array.
     *
     * object - The object to append
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    virtual void append(const cArray<T>& array);


    // Todo
    //void insert(const T& object, const iterator& index);
    //void insert(const cArray<T>& array, const iterator& index);

    /*
     * Construct a new iterator at the begining of the array. (Which means that
     * the iterator is pointed to the first element in the array).
     *
     * The function declares as const because sometimes you need a way to scan
     * the array without change it and you don't have any other way to preform
     * this.
     *
     * NOTE: Microsoft visual studio .NET version 7.10 cannot compile the
     *       code of the function if it's found outside the class deceleration.
     */
    iterator begin() const
    {
        return iterator(const_cast<cArray<T> *> (this), 0);
    }

    /*
     * Create a new iterator at the end of the array.
     *
     * Note: You cannot access to the content of the iterator!
     *
     * The function declares as const because sometimes you need a way to scan
     * the array without change it and you don't have any other way to preform
     * this.
     *
     * NOTE: Microsoft visual studio .NET version 7.10 cannot compile the
     *       code of the function if it's found outside the class deceleration.
     */
    iterator end() const
    {
        return iterator(const_cast<cArray<T> *> (this), m_size);
    }

    /*
     * Create a new iterator at a position in the array.
     *
     * Note: You cannot access to the content of the iterator!
     *
     * The function declares as const because sometimes you need a way to scan
     * the array without change it and you don't have any other way to preform
     * this.
     *
     * NOTE: Microsoft visual studio .NET version 7.10 cannot compile the
     *       code of the function if it's found outside the class deceleration.
     */
    iterator at(const uint& index) const
    {
        return iterator(const_cast<cArray<T> *> (this), index);
    }

    // Operators

    /*
     * Return the element of the array in a specific location.
     *
     * Index - The index of the elements.
     *
     * Throws exception when the index is invalid.
     */
    T& operator [] (const uint index);
    const T& operator [] (const uint index) const;

    /*
     * Copy one array to other.
     */
    cArray<T>& operator = (const cArray<T>& other);

    /*
     * Operators which appends data to the array. Anoter array
     * or another element.
     */
    cArray<T>  operator + (const cArray<T>& other);
    cArray<T>& operator +=(const cArray<T>& other);
    cArray<T>  operator + (const T & object);
    cArray<T>& operator +=(const T & object);


    /* Printing operators */
    template <class S, class OSTREAM>
    friend OSTREAM & operator << (OSTREAM &out, const cArray<S> &object);

    friend class iterator;

    /********************************************
     *** Iterator class.
     *   Inline implementation
     *
     * Implements the iterator on an array. Create the basic elements of an
     * iterator according to the basicIterator interface documentation.
     *
     * The only reson this is an inline implementation is becuase the compilers
     * seems to have difficulties handling internal template classes.
     *
     * Author: Elad Raz.
     */
    class iterator
    {
        public:
            /*
             * Copy constructor. Copy the iterator position from other object.
             */
            iterator(const iterator &other)
            {
                this->m_tcArray = other.m_tcArray;
                this->m_index   = other.m_index;
                assertIndex();
            }

            /*
             * Deconstructor.
             */
            ~iterator()
            {
            }

            /*
             * Forward the iterator by one position.
             */
            void next()
            {
                ++m_index;
                assertIndex();
            }

            /*
             * Setting the iterator back to the previous element.
             */
            void prev()
            {
                --m_index;
                assertIndex();
            }

            /*
             * Java-style iterator function. Return true if the iterator
             * is pointed to the last element in the array.
             */
            bool hasNext()
            {
                return (m_index <= m_tcArray->m_size);
            }

            /*
             * Get the content of the iterator
             */
            T& operator *()
            {
                return (*m_tcArray)[m_index];
            }

        /*
         * Get a pointer to the content of the iterator
             *
             * TODO! Check this code EVC telling an error
             *
            T* operator ->()
            {
                return &((*m_tcArray)[m_index]);
            }
            **/

            /*
             * Return the iterator location in the array
             */
            uint getLocation()
            {
                assertIndex();
                return m_index;
            }

            /*
             * Compare the position of the iterator.
             */
            bool operator == (const iterator &other) const
            {
                if ((m_tcArray == other.m_tcArray) &&
                    (m_index   == other.m_index))
                {
                    return TRUE;
                }
                return FALSE;
            }

            /*
             * Assignment operator.
             */
            iterator& operator = (const iterator &other)
            {
                if (*this != other)
                {
                    this->m_tcArray = other.m_tcArray;
                    this->m_index   = other.m_index;
                }
                return *this;
            }

            /*
             * Is the pointer is lower in the index range.
             */
            bool operator < (const iterator &other) const
            {
                if ((m_tcArray == other.m_tcArray) &&
                    (m_index   <  other.m_index))
                {
                    return TRUE;
                }
                return FALSE;
            }

            /*
             * Implement operator >, !=, >=, <=
             */
            MAKE_SIMPLE_OPERATORS(iterator);

            /*
             * Implement the two operators ++ and the two operators --.
             */
            MAKE_INCREASE_OPERATORS(iterator);

            /*
             * Decrease the iterator by 'count' elements
             * TODO! unsigned!
             */
            iterator operator+ (const uint count)
            {
                return iterator(m_tcArray, m_index + count);
            }

            /*
             * Increase the iterator by 'count' elements
             * TODO! unsigned!
             */
            iterator operator- (const uint count)
            {
                return iterator(m_tcArray, m_index - count);
            }

        private:
            /*
             * Only the cArray<T> class should access the internal data of the
             * iterator - including constructing the iteratir.
             */
            friend class cArray<T>;

            /*
             * cArray<T>::iterator::iterator
             *
             * Constructor. Create an array iterator according to the index
             * inside the array.
             */
            iterator(cArray<T> *tcArray, uint index = 0)
            {
                this->m_tcArray = tcArray;
                this->m_index   = index;
                assertIndex();
            }

            // Inside paramters

            // Pointer to the array object.
            cArray<T>  * m_tcArray;

            // Pointer to the position inside the array.
            uint m_index;

            /*
             * Asserts the index which the iterator is pointed at.
             *
             * Since m_index is uint, it cannot be below zero, so if
             * the index go below zero then there will be a warp.
             */
            void assertIndex()
            {
                ASSERT(m_index <= m_tcArray->m_size);
            }
    };

protected:
    /* Private methods */
    friend class cTestArray;

    /*
     * Free the array. free all memory allocated and create empty array.
     */
    void freeArray();

    /*
     * Create empty array with 'pageSize' attribute. Init all members. The array
     * should be free otherwise memory will not be free properly.
     *
     * pageSize - The size of the paging in used.
     */
    void initArray(uint pageSize);

    /*
     * Copy from other cArray class.
     */
    virtual void copyFrom(const cArray<T> &other);

    /*
     * Page around for the array elements, using the setting of the page size.
     *
     * bytes - The number of items to be allocated.
     *
     * return the number of elements need to be allocated in the page boundries.
     */
    uint pageRound(uint bytes) const;

    /*
     * Private members
     */
    // Pointer to the allocated array
    T* m_array;
    // The number of real elements in the array
    uint m_size;
    // The real allocated items in the array
    uint m_alocatedArray;
    // The page size for the allocation.
    uint m_pageSize;
};


// Implement the template function.
#include "xStl/data/array.inl"

// Declare cBuffer and cSArray class.
#include "xStl/data/sarray.h"

// Return the warnning behaviour back to it's original mode
#ifdef XSTL_WINDOWS
#pragma warning(pop)
#endif

#endif // __TBA_STL_ARRAY_H
