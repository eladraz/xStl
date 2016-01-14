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

/*
 * hash.h
 *
 * Interface and implementation of the template "hash" class, which also calls
 * "directory". The class is template for the key value and the data values.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/osdef.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/list.h"
#include "xStl/data/dualElement.h"

#ifdef XSTL_WINDOWS
// Template classes might not use all the local functions the interface has
// to ofer. Warning C4505 should be over-written for template functions
#pragma warning(push)
#pragma warning(disable:4505)
#endif

/*
 * class cHash
 *
 * Implementation of a global hash.
 * The hash using an array at variable length
 * of link-list.
 *
 * To access the hash use:
 *   hash[IndexType] = FiledType;
 *
 * NOTES:
 *   IndexType must have a complete implementation of operator ==
 *   FiledType must have an hash function.
 *
 * Hash function:
 *   The hash function work on the IndexType and return a number
 *   from 0 to "range"
 *
 *   Definition:
 *         uint cHash_functoin(const IndexType& index, uint range)
 *
 * In this file there are regular implementation of hash on:
 *   float, int, char *.
 *   cString, cBuffer is also implemented in the array.h, string.h files.
 *
 * TODO: Add copy-on-write feature.
 */
template <class IndexType, class FiledType>
class cHash
{
public:
    /*
     * The default arrangement of the hash array.
     * The number of cells that points to the link list.
     */
    enum {
        DefaultHashSearch = 256
    };

    /*
     * Constructor. Init the hash to be constructed with
     */
    explicit cHash(uint vectorSize = DefaultHashSearch);

    /*
     * Copy-constructor. Duplicate the handles from 'other'
     */
    cHash(const cHash<IndexType, FiledType>& other);

    /*
     * Destructor, free up the memory allocated by the hash
     */
    ~cHash();

    /* Hash functions */

    /*
     * Add an element to the hash. The element is copied into the link list.
     * If you want to use the same data, you can use the cSmartPtr as a
     * reference counter object
     *
     * index - The new index for the object. Must be unique
     * data  - The data to put in the cell
     *
     * Throws exception if the index exist
     */
    void append(const IndexType& index, const FiledType& data);

    /*
     * Remove a item from the hash
     *
     * index - The element to remove
     *
     * Throws 'EXCEPTION_OUT_OF_RANGE' exception incase the index is invalid
     */
    void remove(const IndexType& index);

    /*
     * Free the hash-data struct, create a new hash.
     */
    void removeAll();

    /*
     * Return a list of keys value.
     *
     * ret - Will be filled with the keys
     */
    void keys(cList<IndexType>& ret) const;

    /*
     * Returns a list of keys value in the function return coed
     */
    cList<IndexType> keys() const;

    /*
     * Return true if there is a key in the hash
     *
     * index - The index to be query
     */
    bool hasKey(const IndexType &index) const;

    // Operators

    /*
     * Operator =. Copies the other hash table to this hash table.
     */
    const cHash<IndexType, FiledType>& operator = (const cHash<IndexType, FiledType>& other);

    /*
     * Return true if the hash tables are equals.
     */
    bool operator == (const cHash<IndexType, FiledType> &other) const;

    /*
     * The map operator. Retrieve index-type and return a reference to the object
     *
     * index - The index for the hash table
     *
     * Throws 'EXCEPTION_OUT_OF_RANGE' exception when the index is invalid
     * Return a reference to the object
     */
    FiledType& operator [] (const IndexType &index);

    /*
     * The map operator. Retrieve index-type and return a const reference to the
     * object. This function is the const operator for the previous function.
     *
     * index - The index for the hash table
     *
     * Throws 'EXCEPTION_OUT_OF_RANGE' exception when the index is invalid
     * Return a const reference to the object.
     */
    const FiledType& operator [] (const IndexType &index) const;

private:
    /*
     * Internal function. Construct the m_hash object.
     */
    void initHash(uint vectorSize);

    /*
     * Internal function. Free m_hash and all it's data. After a call to
     * this function the hash will be invalid until a call to initHash()
     * will be make.
     */
    void freeHash();

    // The element for a single hash-table. Translates index to filed object
    typedef cDualElement<IndexType,FiledType> ElementType;

    // The link-list which store by each node
    typedef cList<ElementType> ListType;

    // The hash table data-struct.
    cArray<ListType>* m_hash;

    // The old vector-size
    uint m_vectorSize;
};

/*
 * cHashFunction() standart data-types implementation
 *
 * See comments at the begining of the class interface.
 */
uint cHashFunction(const int& index, uint range);
uint cHashFunction(const uint32& index, uint range);
uint cHashFunction(const float& index, uint range);
uint cHashFunction(const char* index, uint range);
uint cHashFunction(const cString& index, uint range);
uint cHashFunction(const cBuffer& index, uint range);
uint cHashFunction(const addressNumericValue& index, uint range);
#ifndef XSTL_16BIT
// 16 bit application doesn't have threads
uint cHashFunction(const uint64& index, uint range);
uint cHashFunction(const cOSDef::threadHandle& index, uint range);
#endif // XSTL_16BIT

// Include the implementation of the hash in the template .h file
#include "xStl/data/hash.inl"

#ifdef XSTL_WINDOWS
    // Restore the warning levels
    #pragma warning(pop)
#endif

#endif // __TBA_STL_HASH_H
