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
 * sarray.inl
 *
 * Implementation code of the cSArray class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/except/exception.h"
#include "xStl/utils/algorithm.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/os.h"

template <class T>
cSArray<T>::cSArray(uint numberOfElements /* = 0*/,
                    uint pageSize /* = 0*/) :
    cArray<T>(numberOfElements, pageSize)
{
}

template <class T>
cSArray<T>::cSArray(const T* staticArray,
                    uint length,
                    uint pageSize /* = 0 */) :
    cArray<T>(length, pageSize)
{
    ASSERT(staticArray != NULL);

    // Copy the array using memcpy
    cOS::memcpy(this->m_array, staticArray, this->m_size * sizeof(T));
}

template <class T>
cSArray<T>::cSArray(const cSArray & other)
{
    // Create empty array
    cSArray<T>::initArray(other.m_pageSize);

	// Create a new empty array at a default size and set the data
	cSArray<T>::copyFrom(other);
}

template <class T>
void cSArray<T>::copyFrom(const cArray<T> &other)
{
    /* Fast copy array */
    this->freeArray();
    changeSize(other.getSize());
    cOS::memcpy(this->m_array, other.getBuffer(), this->m_size * sizeof(T));
}

template <class T>
void cSArray<T>::changeSize(uint newSize,
                            bool preserveMemory /*= true*/)
{
	if ((newSize != 0) && (preserveMemory))
    {
	    uint new_alloc = this->pageRound(newSize);
	    T* buffer;

	    /* Create a new buffer and copy the data to it, if needed */
	    if ((uint)t_abs((int)new_alloc - (int)this->m_alocatedArray) >=
                                                             this->m_pageSize)
	    {
		    /* Need to realloc the buffer */
		    buffer = new T[new_alloc];
            if (buffer == NULL)
            {
                    XSTL_THROW(cException, EXCEPTION_OUT_OF_MEM);
            }

		    if (this->m_array != NULL)
		    {
                cOS::memcpy(buffer, this->m_array,
                            t_min(newSize, this->m_size) * sizeof(T));
			    delete [] this->m_array;
		    }

		    /* Assign the new array */
		    this->m_array = buffer;
		    this->m_size  = newSize;
		    this->m_alocatedArray = new_alloc;
	    } else
	    {
		    /* Just increase the space needed */
		    this->m_size = newSize;
	    }
    } else
    {
        cArray<T>::changeSize(newSize, preserveMemory);
    }
}

template <class T>
void cSArray<T>::append(const T& object)
{
	changeSize(this->m_size + 1);
    cOS::memcpy(this->m_array + this->m_size - 1, &object, sizeof(T));
}

template <class T>
void cSArray<T>::append(const cArray<T>& array)
{
	// Save the rest of the bytes left.
	uint org_size = this->m_size;
	uint add_size = array.getSize();

	// Change the size to the size of both array.
	changeSize(this->m_size + array.getSize());

    // Copy the elements
    cOS::memcpy(this->m_array + org_size, array.getBuffer(), sizeof(T)*add_size);
}


template <class T>
void cSArray<T>::copyRange(const cArray<T>& other,
                           uint startLocation,
                           uint count)
{
    changeSize(count, false);
    cOS::memcpy(this->m_array, other.getBuffer() + startLocation, count * sizeof(T));
}


template <class T>
cSArray<T>& cSArray<T>::operator = (const cSArray<T>& other)
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
cSArray<T> cSArray<T>::operator + (const cSArray<T>& other)
{
	// Create new object, append the data and return it.
	return (cSArray<T>(*this) += other);
}
template <class T>
cSArray<T> cSArray<T>::operator + (const T & object)
{
	// Create new object, append the data and return it.
	return (cSArray<T>(*this) += object);
}

template <class T>
cSArray<T>& cSArray<T>::operator +=(const cSArray<T>& other)
{
	append(other);
	return *this;
}
template <class T>
cSArray<T>& cSArray<T>::operator +=(const T & object)
{
	append(object);
	return *this;
}

template <class T>
bool cSArray<T>::operator == (const cSArray<T>& other) const
{
    if (other.m_size != this->m_size)
        return false;

    return memcmp(this->m_array, other.m_array,
                  sizeof(T) * this->m_size) == 0;
}

template <class T>
bool cSArray<T>::operator <  (const cSArray<T>& other) const
{
    if (other.m_size != this->m_size)
        return this->m_size < other.m_size;

    return memcmp(this->m_array, other.m_array,
                  sizeof(T) * this->m_size) < 0;
}

template <class T>
bool cSArray<T>::isValid() const
{
    return true;
}

// gcc work around. gcc can't access internal function of declared class
// in order to overide this behavior we will use global static functions that
// implemens streamReadUint32/streamWriteUint32 and pipeRead/pipeWrite with interface
#ifdef XSTL_LINUX
extern "C" void gccLinuxStreamReadUint32(basicInput& inputStream, uint32& dword);
extern "C" void gccLinuxStreamWriteUint32(basicOutput& outputStream, uint32& dword);
extern "C" uint gccLinuxStreamPipeRead(basicInput& inputStream, void *buffer, const uint length);
extern "C" void gccLinuxStreamPipeWrite(basicOutput& outputStream, void *buffer, const uint length);
#endif

template <class T>
void cSArray<T>::deserialize(basicInput& inputStream)
{
    uint32 count;
#ifdef XSTL_LINUX
    gccLinuxStreamReadUint32(inputStream, count);
#else
    inputStream.streamReadUint32(count);
#endif

    changeSize(count, false);

#ifdef XSTL_LINUX
    gccLinuxStreamPipeRead(inputStream, (void*)this->m_array, sizeof(T) * count);
#else
    inputStream.pipeRead((void*)this->m_array, sizeof(T) * count);
#endif

}

template <class T>
void cSArray<T>::serialize(basicOutput& outputStream) const
{
	uint32 size = (uint32)(this->m_size);
	// 64 bit size protection
	#ifdef XSTL_64BIT
	if (this->m_size > 0xFFFFFFFFL)
	{
		XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
	}
	#endif
#ifdef XSTL_LINUX
    gccLinuxStreamWriteUint32(outputStream, size);
    gccLinuxStreamPipeWrite(outputStream, (void*)this->m_array, sizeof(T) * size);
#else
    outputStream.streamWriteUint32(size);
    outputStream.pipeWrite((void*)this->m_array, sizeof(T) * size);
#endif
}
