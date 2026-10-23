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

#ifndef __TBA_STL_HASH_H
#define __TBA_STL_HASH_H

/**
 * @addtogroup xstl_data_containers
 * @{
 */

/**
 * @file hash.h
 * @brief Interface and implementation of the template "hash" class, which also
 *   calls "directory".
 *
 * @details The class is template for the key value and the data values.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/os/osdef.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/list.h"
#include "xStl/data/dualElement.h"

#ifdef XSTL_WINDOWS
// Template classes might not use all the local functions the interface has
// to offer. Warning C4505 should be disabled for template functions
#pragma warning(push)
#pragma warning(disable:4505)
#endif

/**
 * @brief Implementation of a global hash.
 *
 * @details The hash uses an array of variable length of link-lists.
 *
 * To access the hash use:
 * hash[IndexType] = FieldType;
 *
 * Hash function:
 * The hash function works on the IndexType and returns a number
 * from 0 to "range"
 *
 * Definition:
 *
 * @code
 * uint cHashFunction(const IndexType& index, uint range)
 * @endcode
 *
 * In this file there are regular implementations of hash on:
 * float, int, char *.
 * cString, cBuffer is also implemented in the array.h, string.h files.
 *
 * @note IndexType must have a complete implementation of operator ==. FieldType
 *    must have a hash function.
 * @todo Add copy-on-write feature.
 * @tparam IndexType The type of the keys. It must have a cHashFunction()
 *    overload and operator ==.
 * @tparam FieldType The type of the stored values.
 */
template <class IndexType, class FieldType>
class cHash
{
public:
    /**
     * @brief The default arrangement of the hash array.
     *
     * @details The number of cells that point to the link list.
     */
    enum {
        /**
         * @brief The default number of cells of the hash vector.
         */
        DefaultHashSearch = 256
    };

    /**
     * @brief Constructor. Init the hash to be constructed with
     *
     * @param vectorSize The number of cells of the hash vector. Each cell
     *    points to a link-list of the elements which are mapped to it.
     */
    explicit cHash(uint vectorSize = DefaultHashSearch);

    /**
     * @brief Copy-constructor. Duplicate the handles from 'other'
     *
     * @param other The hash to copy.
     */
    cHash(const cHash<IndexType, FieldType>& other);

    /**
     * @brief Destructor, free up the memory allocated by the hash
     */
    ~cHash();

    /* Hash functions */

    /**
     * @brief Add an element to the hash.
     *
     * @details The element is copied into the link list. If you want to use the
     *    same data, you can use the cSmartPtr as a reference counter object
     *
     * @param index The new index for the object. Must be unique
     * @param data The data to put in the cell
     * @throws cException if the index exists
     */
    void append(const IndexType& index, const FieldType& data);

    /**
     * @brief Remove an item from the hash
     *
     * @param index The element to remove
     * @throws cException (EXCEPTION_OUT_OF_RANGE) in case the index is invalid
     */
    void remove(const IndexType& index);

    /**
     * @brief Free the hash-data struct, create a new hash.
     */
    void removeAll();

    /**
     * @brief Return a list of key values.
     *
     * @param ret Will be filled with the keys
     */
    void keys(cList<IndexType>& ret) const;

    /**
     * @brief Returns a list of key values as the function return value
     *
     * @return A list of key values as the function return value.
     */
    cList<IndexType> keys() const;

    /**
     * @brief Return true if there is a key in the hash
     *
     * @param index The index to be queried
     * @return True if there is a key in the hash.
     */
    bool hasKey(const IndexType &index) const;

    // Operators

    /**
     * @brief Operator =. Copies the other hash table to this hash table.
     *
     * @param other The hash to copy from.
     * @return A reference to this hash.
     */
    const cHash<IndexType, FieldType>&
        operator = (const cHash<IndexType, FieldType>& other);

    /**
     * @brief Return true if the hash tables are equal.
     *
     * @param other The hash to compare with.
     * @return True if the hash tables are equal.
     */
    bool operator == (const cHash<IndexType, FieldType> &other) const;

    /**
     * @brief The map operator. Retrieve index-type and return a reference to
     *    the object
     *
     * @param index The index for the hash table
     * @return A reference to the object.
     * @throws cException (EXCEPTION_OUT_OF_RANGE) when the index is invalid
     */
    FieldType& operator [] (const IndexType &index);

    /**
     * @brief The map operator. Retrieve index-type and return a const reference
     *    to the
     *   object.
     *
     * @details This function is the const operator for the previous function.
     *
     * @param index The index for the hash table
     * @return A const reference to the object.
     * @throws cException (EXCEPTION_OUT_OF_RANGE) when the index is invalid
     */
    const FieldType& operator [] (const IndexType &index) const;

private:
    /**
     * @brief Internal function. Construct the m_hash object.
     */
    void initHash(uint vectorSize);

    /**
     * @brief Internal function. Free m_hash and all its data.
     *
     * @details After a call to this function the hash will be invalid until a
     *    call to initHash() will be made.
     */
    void freeHash();

    /**
     * @brief The element for a single hash-table.
     *
     * @details Translates index to field object
     */
    typedef cDualElement<IndexType,FieldType> ElementType;

    /**
     * @brief The link-list which is stored by each node
     */
    typedef cList<ElementType> ListType;

    /**
     * @brief The hash table data-struct.
     */
    cArray<ListType>* m_hash;

    /**
     * @brief The old vector-size
     */
    uint m_vectorSize;
};

/**
 * @brief Hash function overloads for the standard data types.
 *
 * The functions map an index into a cell number inside `[0, range)`. The
 * integer overloads use `index % range`; the float overload applies the same
 * operation to the bits of the number; the string, buffer and C-string
 * overloads sum the characters (bytes) and apply the modulo operation. See the
 * cHash class for how the hash functions are used. Add an overload of this
 * function to use another type as the index of a cHash.
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const int& index, uint range);
/**
 * @brief Hash function for a 32 bit unsigned number. See cHashFunction(const
 *    int&, uint).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const uint32& index, uint range);
/**
 * @brief Hash function for a floating point number. See cHashFunction(const
 *    int&, uint).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const float& index, uint range);
/**
 * @brief Hash function for a null-terminated ASCII string. See
 *    cHashFunction(const int&, uint).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const char* index, uint range);
/**
 * @brief Hash function for a string. See cHashFunction(const int&, uint).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const cString& index, uint range);
/**
 * @brief Hash function for a buffer. See cHashFunction(const int&, uint).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const cBuffer& index, uint range);
/**
 * @brief Hash function for an address. See cHashFunction(const int&, uint).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const addressNumericValue& index, uint range);
#ifndef XSTL_16BIT
/**
 * @brief Hash function for a 64 bit unsigned number. See cHashFunction(const
 *    int&, uint).
 *
 * The 64 bit and the thread handle overloads are not available for 16 bit
 * applications (XSTL_16BIT).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const uint64& index, uint range);
/**
 * @brief Hash function for a thread handle. See cHashFunction(const int&,
 *    uint).
 *
 * @param index The index to hash.
 * @param range The number of cells in the hash vector.
 * @return The cell number for the index.
 */
uint cHashFunction(const cOSDef::threadHandle& index, uint range);
#endif // XSTL_16BIT

// Include the implementation of the hash in the template .h file
#include "xStl/data/hash.inl"

#ifdef XSTL_WINDOWS
    // Restore the warning levels
    #pragma warning(pop)
#endif


/** @} */
#endif // __TBA_STL_HASH_H
