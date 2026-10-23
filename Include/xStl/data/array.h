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

/**
 * @addtogroup xstl_data_containers
 * @{
 */

/**
 * @file array.h
 * @brief Declaration of the global array memory handler.
 *
 * @details Provides a safe way to allocate an array of memory. Handles
 *    everything that needs to be done.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/assert.h"

#ifdef XSTL_WINDOWS
// Template classes might not use all the local functions the interface has
// to offer. Warning C4505 should be disabled for template functions
#pragma warning(push)
#pragma warning(disable:4505)
#endif

/**
 * @brief A dynamic array with a paging memory mechanism.
 *
 * @details Contains standard implementation of arrays. This is a template
 *    vector for
 *   class T. T should have default constructor (due to using operator new[]).
 *   operator == should be implemented only for reference call to remove().
 *
 * The array also implements a 'paging' memory. The paging allows allocating
 * more
 * memory for the array but when the size of the array is changed within the
 * page size, no memory allocation operation is called.
 *
 * PS: cArray can be used for many calls to changeSize()
 * only if you configure the page_size variable correctly.
 *
 * T must have:
 * default constructor.
 * For using remove(T\&) T must implement operator ==.
 *
 * @todo Add to this class copy-on-write abilities (Reference count and unshare
 *    method).
 * @tparam T The type of the array elements. It must have a default constructor
 *    (the memory is allocated with `operator new[]`). For using remove(T&), T
 *    must also implement operator ==.
 */
template <class T>
class cArray
{
public:
    // Forward declaration for class iterator.
    class iterator;

    /**
     * @brief Explicit constructor.
     *
     * @details Create an array at size 'numberOfElements'. The 'pageSize'
     *    determines
     *   how many elements should be allocated in addition to the needed items.
     *
     * @throws cException (EXCEPTION_OUT_OF_MEM) .
     * @param numberOfElements The number of elements of the new array.
     * @param pageSize The page size. How many elements are allocated in
     *    addition to the needed elements.
     */
    explicit cArray(uint numberOfElements = 0,
                    uint pageSize = 0);

    /**
     * @brief Create array and copy 'staticArray' to the constructor array.
     *
     * @param staticArray The source array.
     * @param length The number of elements in the array
     * @param pageSize The size of the paging
     * @throws cException (EXCEPTION_OUT_OF_MEM) .
     */
    explicit cArray(const T* staticArray,
                    uint length,
                    uint pageSize = 0);

    /**
     * @brief Copy constructor
     *
     * @param other The array to copy.
     */
    cArray(const cArray<T>& other);

    /**
     * @brief Virtual destructor. Used for cBuffer implementation.
     */
    virtual ~cArray();


    /* Class functions */

    /**
     * @brief Return the buffer of the array. getBuffer() declare as const but
     *    return
     *   non-const operator.
     *
     * @details The return type can be NULL if the array is empty.
     *
     * @return The buffer of the array. getBuffer() declare as const but return
     *    non-const operator.
     * @note The returned buffer is valid for the following commands until
     *    another command from the cArray class is invoked. Each function of the
     *    array can reallocate the buffer pointer.
     */
    T* getBuffer() const;

    /**
     * @brief Return the page size of the array object.
     *
     * @return The page size of the array object.
     */
    uint getPageSize() const;

    /**
     * @brief Return the number of elements in the array.
     *
     * @return The number of elements in the array.
     */
    uint getSize() const;

    /**
     * @brief Replace the array pointers of the current array and 'other'
     *
     * @note This function doesn't replace the pageSize values.
     * @note This class is not thread-safe
     * @param other The array to swap the content with.
     */
    void swap(cArray<T>& other);

    /**
     * @brief Increase or decrease the count of elements in the array.
     *
     * @details Destroy the previous memory if the preserveMemory is false.
     *    Otherwise, if the newSize is smaller than the old one, then the
     *    padding will be freed. If the new size is bigger than the old size,
     *    the newly allocated fields will not be initialized. If the newSize is
     *    equal to zero, then the list will be freed.
     *
     * @param newSize The new number of elements in the array.
     * @param preserveMemory If true the previous content is preserved. If false
     *    the previous memory is destroyed.
     */
    virtual void changeSize(uint newSize, bool preserveMemory = true);

    /**
     * @brief Change the page size of the array.
     *
     * @details The effect of this function is on future calls to the
     *    'changeSize' function.
     *
     * @param newPageSize The new page size.
     */
    void setPageSize(uint newPageSize);

    /**
     * @brief Like the Python's operator [:], but much less improved.
     *
     * @details Copy from the array 'count' elements starting from
     *    'startLocation' into 'other' array. 'other' will be reset to an array
     *    of 'count' elements.
     *
     * @param other The output array
     * @param startLocation The start location in the array
     * @param count The number of elements to copy.
     * @throws cException (EXCEPTION_OUT_OF_MEM) .
     */
    virtual void copyRange(const cArray<T>& other,
                           uint startLocation,
                           uint count);

    /**
     * @brief Insert a single object at the end of the array.
     *
     * @details The array is expanded by one element and the 'object' is copied
     *    to the end of the array.
     *
     * @param object The object to append
     * @throws cException (EXCEPTION_OUT_OF_MEM) .
     */
    virtual void append(const T& object);

    /**
     * @brief Insert a new array at the end of the array.
     *
     * The array is expanded by the number of elements of @a array and the
     * elements are copied to its end.
     *
     * @param array The array to append.
     * @throws cException (EXCEPTION_OUT_OF_MEM) if there is not enough memory.
     */
    virtual void append(const cArray<T>& array);


    // Todo
    //void insert(const T& object, const iterator& index);
    //void insert(const cArray<T>& array, const iterator& index);

    /**
     * @brief Construct a new iterator at the beginning of the array. (Which
     *    means that
     *   the iterator points to the first element in the array).
     *
     * @details The function is declared as const because sometimes you need a
     *    way to scan
     *   the array without changing it and you don't have any other way to
     *   perform
     *   this.
     *
     * @note Microsoft Visual Studio .NET version 7.10 cannot compile the code
     *    of the function if it's found outside the class declaration.
     * @return An iterator that points to the first element of the array.
     */
    iterator begin() const
    {
        return iterator(const_cast<cArray<T> *> (this), 0);
    }

    /**
     * @brief Create a new iterator at the end of the array.
     *
     * @details The function is declared as const because sometimes you need a
     *    way to scan
     *   the array without changing it and you don't have any other way to
     *   perform
     *   this.
     *
     * @note You cannot access the content of the iterator!
     * @note Microsoft Visual Studio .NET version 7.10 cannot compile the code
     *    of the function if it's found outside the class declaration.
     * @return An iterator that points after the last element of the array.
     */
    iterator end() const
    {
        return iterator(const_cast<cArray<T> *> (this), m_size);
    }

    /**
     * @brief Create a new iterator at a position in the array.
     *
     * @details The function is declared as const because sometimes you need a
     *    way to scan
     *   the array without changing it and you don't have any other way to
     *   perform
     *   this.
     *
     * @note You cannot access the content of the iterator!
     * @note Microsoft Visual Studio .NET version 7.10 cannot compile the code
     *    of the function if it's found outside the class declaration.
     * @param index The position in the array.
     * @return An iterator that points to the element at @a index.
     */
    iterator at(const uint& index) const
    {
        return iterator(const_cast<cArray<T> *> (this), index);
    }

    // Operators

    /**
     * @brief Return the element of the array in a specific location.
     *
     * @param index The index of the elements.
     * @return The element of the array in a specific location.
     * @throws cException when the index is invalid.
     */
    T& operator [] (const uint index);
    /**
     * @brief Return the element of the array in a specific location (const
     *    version).
     *
     * @param index The index of the element.
     * @return The element of the array in the location @a index.
     * @throws cException if the index is invalid.
     */
    const T& operator [] (const uint index) const;

    /**
     * @brief Copy one array to other.
     *
     * @param other The array to copy from.
     * @return A reference to this array.
     */
    cArray<T>& operator = (const cArray<T>& other);

    /**
     * @brief Create a new array that contains this array followed by another
     *    array.
     *
     * @param other The array whose elements are appended.
     * @return The new array.
     */
    cArray<T>  operator + (const cArray<T>& other);
    /**
     * @brief Append the elements of another array to this array.
     *
     * @param other The array whose elements are appended.
     * @return A reference to this array.
     */
    cArray<T>& operator +=(const cArray<T>& other);
    /**
     * @brief Create a new array that contains this array followed by an
     *    element.
     *
     * @param object The element that is appended.
     * @return The new array.
     */
    cArray<T>  operator + (const T & object);
    /**
     * @brief Append an element to this array.
     *
     * @param object The element that is appended.
     * @return A reference to this array.
     */
    cArray<T>& operator +=(const T & object);


    /**
     * @brief Print the array to an output stream.
     *
     * The elements are separated by commas and enclosed in square brackets. The
     * cascading works only if the type of the elements can be written to the
     * stream.
     *
     * @tparam S The type of the array elements.
     * @tparam OSTREAM The type of the output stream.
     * @param out The output stream.
     * @param object The array to print.
     * @return The output stream @a out.
     */
    template <class S, class OSTREAM>
    friend OSTREAM & operator << (OSTREAM &out, const cArray<S> &object);

    friend class iterator;

    /**
     * @brief Iterator over the elements of a cArray.
     *
     * The iterator is an inline implementation, because the compilers seem to
     * have difficulties handling internal template classes.
     */
    class iterator
    {
        public:
            /**
             * @brief Copy constructor. Copy the iterator position from another
             *    object.
             *
             * @param other The iterator to copy.
             */
            iterator(const iterator &other)
            {
                this->m_tcArray = other.m_tcArray;
                this->m_index   = other.m_index;
                assertIndex();
            }

            /**
             * @brief Destructor.
             */
            ~iterator()
            {
            }

            /**
             * @brief Advance the iterator by one position.
             */
            void next()
            {
                ++m_index;
                assertIndex();
            }

            /**
             * @brief Set the iterator back to the previous element.
             */
            void prev()
            {
                --m_index;
                assertIndex();
            }

            /**
             * @brief Java-style iterator function.
             *
             * @return True if the iterator position does not exceed the end of
             *    the array (the implementation tests that the index is not
             *    greater than the size of the array).
             */
            bool hasNext()
            {
                return (m_index <= m_tcArray->m_size);
            }

            /**
             * @brief Get the content of the iterator
             *
             * @return A reference to the element the iterator points to.
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

            /**
             * @brief Return the iterator location in the array
             *
             * @return The iterator location in the array.
             */
            uint getLocation()
            {
                assertIndex();
                return m_index;
            }

            /**
             * @brief Compare the position of the iterator.
             *
             * @param other The iterator to compare with.
             * @return True if both iterators point to the same position of the
             *    same array.
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

            /**
             * @brief Assignment operator.
             *
             * @param other The iterator to copy from.
             * @return A reference to this iterator.
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

            /**
             * @brief Is the pointer lower in the index range.
             *
             * @param other The iterator to compare with.
             * @return True if both iterators belong to the same array and the
             *    index of this iterator is lower than the index of @a other.
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

            /**
             * @brief Implement operator \>, !=, \>=, \<=
             */
            MAKE_SIMPLE_OPERATORS(iterator);

            /**
             * @brief Implement the two operators ++ and the two operators --.
             */
            MAKE_INCREASE_OPERATORS(iterator);

            /**
             * @brief Return an iterator that is advanced by @a count elements.
             *
             * @param count The number of elements to advance.
             * @return The new iterator.
             * @todo unsigned!
             */
            iterator operator+ (const uint count)
            {
                return iterator(m_tcArray, m_index + count);
            }

            /**
             * @brief Return an iterator that is moved back by @a count
             *    elements.
             *
             * @param count The number of elements to move back.
             * @return The new iterator.
             * @todo unsigned!
             */
            iterator operator- (const uint count)
            {
                return iterator(m_tcArray, m_index - count);
            }

        private:
            /*
             * Only the cArray<T> class should access the internal data of the
             * iterator - including constructing the iterator.
             */
            friend class cArray<T>;

            /**
             * @brief Constructor. Create an array iterator according to the
             *    index inside the array.
             *
             * @param tcArray The array to iterate over.
             * @param index The position of the iterator in the array.
             */
            iterator(cArray<T> *tcArray, uint index = 0)
            {
                this->m_tcArray = tcArray;
                this->m_index   = index;
                assertIndex();
            }

            // Inside parameters

            /**
             * @brief Pointer to the array object.
             */
            cArray<T>  * m_tcArray;

            /**
             * @brief Pointer to the position inside the array.
             */
            uint m_index;

            /**
             * @brief Asserts the index which the iterator points at.
             *
             * @details Since m_index is uint, it cannot be below zero, so if
             *   the index goes below zero then it will wrap around.
             */
            void assertIndex()
            {
                ASSERT(m_index <= m_tcArray->m_size);
            }
    };

protected:
    /* Private methods */
    friend class cTestArray;

    /**
     * @brief Free the array: free all allocated memory and create an empty
     *    array.
     */
    void freeArray();

    /**
     * @brief Create an empty array with the 'pageSize' attribute.
     *
     * @details Init all members. The array should be free, otherwise memory
     *    will not be freed properly.
     *
     * @param pageSize The size of the paging in use.
     */
    void initArray(uint pageSize);

    /**
     * @brief Copy from other cArray class.
     *
     * @param other The array to copy from.
     */
    virtual void copyFrom(const cArray<T> &other);

    /**
     * @brief Page-round the array elements, using the page size setting.
     *
     * @param bytes The number of items to be allocated.
     * @return The number of elements that need to be allocated in the page
     *    boundaries.
     */
    uint pageRound(uint bytes) const;

    /*
     * Private members
     */
    /**
     * @brief Pointer to the allocated array
     */
    T* m_array;
    /**
     * @brief The number of real elements in the array
     */
    uint m_size;
    /**
     * @brief The real allocated items in the array
     */
    uint m_allocatedArray;
    /**
     * @brief The page size for the allocation.
     */
    uint m_pageSize;
};


// Implement the template function.
#include "xStl/data/array.inl"

// Declare cBuffer and cSArray class.
#include "xStl/data/sarray.h"

// Return the warning behaviour back to its original mode
#ifdef XSTL_WINDOWS
#pragma warning(pop)
#endif


/** @} */
#endif // __TBA_STL_ARRAY_H
