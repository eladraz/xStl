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
 * array.inl
 *
 * Implementation code of the cArray class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/except/exception.h"

template <class T>
cArray<T>::cArray(uint numberOfElements /* = 0*/,
                  uint pageSize /* = 0*/)
{
    // Init the array
    initArray(pageSize);

    // Create array if needed
	if (numberOfElements > 0)
	{
		m_size          = numberOfElements;
		m_alocatedArray = pageRound(m_size);
		m_array         = new T[m_alocatedArray];
        if (m_array == NULL)
        {
            XSTL_THROW(cException, EXCEPTION_OUT_OF_MEM);
        }
	}
}

template <class T>
cArray<T>::cArray(const T* staticArray,
                  uint length,
                  uint pageSize /* = 0 */)
{
    ASSERT(staticArray != NULL);

    // Init the array
    initArray(pageSize);

    // Create array if needed
	if (length > 0)
	{
		m_size          = length;
		m_alocatedArray = pageRound(m_size);
		m_array         = new T[m_alocatedArray];
        if (m_array == NULL)
        {
                XSTL_THROW(cException, EXCEPTION_OUT_OF_MEM);
        }

		// Copy the static array into the created array
        // Notice that operator = is in used...
		for (uint i = 0; i < length; i++)
		{
			m_array[i] = staticArray[i];
		}
	}
}

template <class T>
cArray<T>::cArray(const cArray<T>& other)
{
    // Init the array
    initArray(other.m_pageSize);

	// Create a new empty array at a default size and set the data
	copyFrom(other);
}

template <class T>
uint cArray<T>::pageRound(uint bytes) const
{
	if (m_pageSize > 1)
	{
        if ((bytes % m_pageSize) == 0)
        {
            return bytes;
        }
		return m_pageSize * ((bytes / m_pageSize) + 1);
	} else
	{
		return bytes;
	}
}

template <class T>
void cArray<T>::initArray(uint pageSize)
{
    m_size = 0;
	m_array = NULL;

	m_alocatedArray = 0;
	m_pageSize      = pageSize;

}

template <class T>
void cArray<T>::copyFrom(const cArray<T> &other)
{
	freeArray();

	// Create a new empty array at a default size and set the data
	changeSize(other.m_size);

	for (uint index = 0; index < other.m_size; index++)
	{
		m_array[index] = other.m_array[index];
	}
}

template <class T>
cArray<T>& cArray<T>::operator = (const cArray<T>& other)
{
	if (this != &other)
	{
		// If and only if, the class are DIFFRENT then
		// preform the assignment.
		copyFrom(other);

		return *this;
	}

	return *this;
}


template <class T>
cArray<T> cArray<T>::operator + (const cArray<T>& other)
{
	// Create new object, append the data and return it.
	return (cArray<T>(*this) += other);
}
template <class T>
cArray<T> cArray<T>::operator + (const T & object)
{
	// Create new object, append the data and return it.
	return (cArray<T>(*this) += object);
}


template <class T>
cArray<T>& cArray<T>::operator +=(const cArray<T>& other)
{
	append(other);
	return *this;
}
template <class T>
cArray<T>& cArray<T>::operator +=(const T & object)
{
	append(object);
	return *this;
}

template <class T>
cArray<T>::~cArray()
{
	freeArray();
}


template <class T>
void cArray<T>::freeArray()
{
	if (m_array != NULL)
	{
		/* Need to deallocate the memory */
		delete [] m_array;
		m_array = NULL;
	}
	m_size = 0;
	m_alocatedArray = 0;
}

template <class T>
uint cArray<T>::getSize() const
{
	return m_size;
}

template <class T>
uint cArray<T>::getPageSize() const
{
    return m_pageSize;
}

template <class T>
void cArray<T>::setPageSize(uint newPageSize)
{
    m_pageSize = newPageSize;
}

template <class T>
void cArray<T>::swap(cArray<T>& other)
{
    t_swap(m_array, other.m_array);
    t_swap(m_size, other.m_size);
    t_swap(m_alocatedArray, other.m_alocatedArray);
}

template <class T>
T& cArray<T>::operator [] (const uint index)
{
    if (index >= m_size)
    {
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    }
    return m_array[index];
}

template <class T>
const T& cArray<T>::operator [] (const uint index) const
{
    ASSERT(index < m_size);
    if (index >= m_size)
    {
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    }
	return m_array[index];
}


template <class T>
void cArray<T>::changeSize(uint newSize,
                           bool preserveMemory /*= true*/)
{
    // Optimization. When freeing a memory and preserving the memory, the array
    // will remain untouched, until a second allocation will be made
    if ((newSize == 0) && preserveMemory)
    {
        m_size = newSize;
        return;
    }

	if (newSize == 0)
	{
		freeArray();
	} else
	{
		if (preserveMemory)
		{
			uint index     = 0;
			uint new_alloc = pageRound(newSize);
			T *buffer;

			/* Create a new buffer and copy the data to it, if needed */
			if ((uint)t_abs((int)new_alloc - (int)m_alocatedArray) >= m_pageSize)
			{
				/* Need to realloc the buffer */
				buffer = new T[new_alloc];
                if (buffer == NULL)
                {
                        XSTL_THROW(cException, EXCEPTION_OUT_OF_MEM);
                }

				if (m_array != NULL)
				{
					for (index = 0; index < t_min(newSize, m_size); index++)
					{
						buffer[index] = m_array[index];
					}
					delete [] m_array;
				}

				/* Assign the new array */
				m_array = buffer;
				m_size  = newSize;
				m_alocatedArray = new_alloc;
			} else
			{
				/* Just increase the space needed */
				m_size = newSize;
			}
		} else
		{
			/* Free the list and create a new one */
			if (m_array != NULL)
			{
				delete [] m_array;
			}
			m_alocatedArray = pageRound(newSize);
			m_array = new T[m_alocatedArray];
			m_size  = newSize;
            if (m_array == NULL)
            {
                    XSTL_THROW(cException, EXCEPTION_OUT_OF_MEM);
            }
		}
	}
}

template <class T>
T* cArray<T>::getBuffer() const
{
	return m_array;
}

template <class T>
void cArray<T>::append(const T& object)
{
	// First we will increase the array by one.
	changeSize(m_size + 1);

	// Now we will copt the element to end of the array
	m_array[m_size - 1] = object;
}

template <class T>
void cArray<T>::append(const cArray<T>& array)
{
	// Save the rest of the bytes left.
	uint org_size = this->m_size;
	uint add_size = array.m_size;

	// Change the size to the size of both array.
	changeSize(m_size + array.m_size);

	// Copy the rest of the data
	for (uint i = 0; i < add_size; i++)
	{
		m_array[i + org_size] = array.m_array[i];
	}
}

template <class T>
void cArray<T>::copyRange(const cArray<T>& other,
                          uint startLocation,
                          uint count)
{
	/* Reinit array */
	changeSize(count, false);

	/* Copy the data */
	for (uint i = 0; i < count; i++)
	{
		m_array[i] = other.m_array[i + startLocation];
	}
}

/*
 * ostream & operator << (ostream &out, const cArray& object)
 *
 * Covert the array into printrable array characters.
 * This function will be used in the cout function
 * As an example: cout << cArray<int>(5, 10, 1) << endl;
 *
 * Note the cascading will work only if the class T have
 * and basic_ostream warpper.
 */
template <class S, class OSTREAM>
OSTREAM& operator << (OSTREAM &out, const cArray<S>& object)
{
	uint index;

	out << "[";

	for (index = 0; index < object.m_size - 1; index++)
	{
		out << object.m_array[index] << ", ";
	}

	out << object.m_array[object.m_size - 1];

	out << "]";

	return out;
}
