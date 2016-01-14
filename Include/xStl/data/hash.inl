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

/*
 * hash.inl
 *
 * Implementation code of the cHash template class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/data/hash.h"

template <class IndexType, class FiledType>
cHash<IndexType, FiledType>::cHash(uint vectorSize /* = DefaultHashSearch*/) :
	m_hash(NULL),
    m_vectorSize(vectorSize)
{
    initHash(vectorSize);
}

template <class IndexType, class FiledType>
void cHash<IndexType, FiledType>::initHash(uint vectorSize)
{
    ASSERT(m_hash == NULL);

	// Create an array of empty lists
	m_hash = new cArray<ListType>(vectorSize);
}

template <class IndexType, class FiledType>
cHash<IndexType, FiledType>::cHash(const cHash<IndexType, FiledType>& other) :
    m_hash(NULL)
{
	// Call operator
	*this = other;
}

template <class IndexType, class FiledType>
cHash<IndexType, FiledType>::~cHash()
{
    freeHash();
}

template <class IndexType, class FiledType>
void cHash<IndexType, FiledType>::freeHash()
{
    ASSERT(m_hash != NULL);
	delete m_hash;
    m_hash = NULL;
}

template <class IndexType, class FiledType>
void cHash<IndexType, FiledType>::append(const IndexType& index, const FiledType& data)
{
    // Test that the object is unique
    CHECK(!hasKey(index));

	// Compute the position of the element in the array
	uint arrayPosition = cHashFunction(index, m_hash->getSize());

	// Object doesn't found
	// Add the element in the end of the list.
	(*m_hash)[arrayPosition].append(cDualElement<IndexType, FiledType>(index, data));
}

template <class IndexType, class FiledType>
bool cHash<IndexType, FiledType>::hasKey(const IndexType& index) const
{
	// Compute the position of the element in the array
	uint arrayPosition = cHashFunction(index, m_hash->getSize());

	// Start searching in the currect link_list
	// Recieve the begining iterator of the list and scan it until the end
	typename cList<ElementType>::iterator i = (*m_hash)[arrayPosition].begin();
	for (;i != (*m_hash)[arrayPosition].end(); i++)
	{
			 if ((*i).m_a == index)
			 {
				 // We found our elemenet, exit the searching
				 break;
			 }
	}

	// Test whether we reach the end of the list
	// e.g. we couldn't find the index in any place
	// of the list.
	if (i == (*m_hash)[arrayPosition].end())
	{
		return false;
	}

	return true;
}

template <class IndexType, class FiledType>
void cHash<IndexType, FiledType>::remove(const IndexType &index)
{
	// Compute the position of the element in the array
	uint arrayPosition = cHashFunction(index, m_hash->getSize());

	// Start searching in the currect link_list
	// Recieve the begining iterator of the list and scan it until the end
	typename cList<ElementType>::iterator i = (*m_hash)[arrayPosition].begin();
	for (; i != (*m_hash)[arrayPosition].end(); i++)
	{
			 if ((*i).m_a == index)
			 {
				 // We found our elemenet, exit the searching
				 break;
			 }
	}

	// Test whether we reach the end of the list
	// e.g. we couldn't find the index in any place
	// of the list.
	if (i == (*m_hash)[arrayPosition].end())
	{
		XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
	}

	/* Remove i from the list */
	(*m_hash)[arrayPosition].remove(i);
}

template <class IndexType, class FiledType>
void cHash<IndexType, FiledType>::removeAll()
{
    freeHash();
    initHash(m_vectorSize);
}

template <class IndexType, class FiledType>
void cHash<IndexType, FiledType>::keys(cList<IndexType>& ret) const
{
	for (uint i = 0; i < m_hash->getSize(); i++)
	{
		for (typename cList<ElementType>::iterator j = (*m_hash)[i].begin();
		     j != (*m_hash)[i].end(); j++)
		{
			ret.append((*j).m_a);
		}
	}
}

template <class IndexType, class FiledType>
cList<IndexType> cHash<IndexType, FiledType>::keys() const
{
	cList<IndexType> ret;
	keys(ret);
	return ret;
}

template <class IndexType, class FiledType>
const cHash<IndexType, FiledType> & cHash<IndexType, FiledType>::operator =
    (const cHash<IndexType, FiledType> &other)
{
	if (this != &other)
	{
		/* Copy the global array */
		if (this->m_hash != NULL)
		{
			freeHash();
		}
		this->m_hash = new cArray<ListType>(*other.m_hash);
        m_vectorSize = this->m_hash->getSize();
	}

	return *this;
}

template <class IndexType, class FiledType>
FiledType & cHash<IndexType, FiledType>::operator [] (const IndexType &index)
{
    const cHash<IndexType, FiledType>* constThis = this;

    // Use the same reference
    return const_cast<FiledType&>((*constThis)[index]);
}

template <class IndexType, class FiledType>
const FiledType & cHash<IndexType, FiledType>::operator [] (const IndexType &index) const
{
    // Compute the position of the element in the array
    uint arrayPosition = cHashFunction(index, m_hash->getSize());

    // Start searching in the currect link_list
    // Recieve the begining iterator of the list and scan it until the end
    typename cList<ElementType>::iterator i = (*m_hash)[arrayPosition].begin();
    for (; i != (*m_hash)[arrayPosition].end(); i++)
    {
        if ((*i).m_a == index)
        {
            // We found our elemenet, exit the searching
            break;
        }
    }

    // Test whether we reach the end of the list
    // e.g. we couldn't find the index in any place
    // of the list.
    if (i == (*m_hash)[arrayPosition].end())
    {
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    }

    // Return the refrence in the list
    return (*i).m_b;
}
