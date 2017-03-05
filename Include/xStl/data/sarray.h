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

#ifndef __TBA_STL_SARRAY_H
#define __TBA_STL_SARRAY_H

/*
 * sarray.h
 *
 * Declaration on the struct-array. Struct array are array which stores objects
 * without copy-construct and can be copied using 'memcpy' command.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/data/array.h"
#include "xStl/data/serializedObject.h"
#include "xStl/data/smartptr.h"

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to ofer. Warning C4505 should be over-written for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif

#ifdef DMCOMP_NOAD
    /*
     * The default C standard (By Digital Mars compiler) forbid extending
     * template class since the base class is independed name
     * Look on the following code:
     *
     * template <class T>
     * class Base {
     * public:
     *      void func();
     *      int m_member;
     *      T* m_object;
     * };
     *
     * template <class T>
     * class Driven : public Base<T> {
     * public:
     *        Driven() :
     *            m_object(NULL),           <=== COMPILER ERROR!
     *            m_member(4)               <=== COMPILER ERROR!
     *        {
     *             func();                  <=== COMPILER ERROR!
     *        }
     * };
     *
     * Anyway, the compiler doesn't recognize base template class on the driven
     * class implementation code.
     * It can't recognize any static members/static const/methods/members of the
     * base class
     *
     * Hence, using the default settings of DMC compiler the struct-array
     * optimization will be disabled.
     * Use dmc.exe -Ad to overide this behaviour.
     *
     * Elad.
     */
    #define cSArray cArray
#else

/*
 * class cSArray
 *
 * Array class. T cannot have copy-constructor or operator =. T must be struct,
 * class with members only. The class contains build-up optimized code, (Since
 * memcpy is used to copy data and not operator =.)
 */
template <class T>
class cSArray : public cArray<T>
                ,public cSerializedObject
{
public:
    /*
     * Explicit default constructor.
     *
     * Create array of 'numberOfElements' elements (uninitialize objects)
     * with page size of 'pageSize'.
     * Notice that the array is limited by the size of the compiler 'int'.
     *
     * numberOfElements - The number of elements in the array
     * pageSize - The page-size for the array. See cArray for paging mechanizm
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    explicit cSArray(uint numberOfElements = 0,
                     uint pageSize = 0);

    /*
     * Create array of 'length' elements and copy to it the 'staticArray' array.
     * The copy is used using memcpy instead of operator =.
     */
    explicit cSArray(const T* staticArray,
                     uint length,
                     uint pageSize = 0);

    /*
     * Copy-constructor.
     */
    cSArray(const cSArray<T> & other);

    /*
     * Improve the changing memory by copying the members using memcpy
     */
    virtual void changeSize(uint newSize,
                            bool preserveMemory = true);

    /*
     * Insert single object to the end of the array. The copy of the new element
     * is by invoking memcpy.
     *
     * object - The object to append
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    virtual void append(const T& object);

    /*
     * Insert new array to the end of the array. The copy method is by invoking
     * 'memcpy' instead of operator =.
     *
     * object - The object to append
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    virtual void append(const cArray<T> & array);


    /*
     * Copy range from the array using memcpy. See cArray::copyRange function
     * for more documentation about this function.
     *
     * other         - The output array
     * startLocation - The start location in the array
     * count         - The number of elements to copy.
     *
     * throws 'EXCEPTION_OUT_OF_MEM' exception.
     */
    virtual void copyRange(const cArray<T> & other,
                           uint startLocation,
                           uint count);

    // Operators

    /*
     * Operator =. Copy from other cSArray
     */
    cSArray<T>& operator = (const cSArray<T>& other);

    /*
     * Operators which appends data to the array. Another array
     * or another element.
     */
    cSArray<T>  operator + (const cSArray<T>& other);
    cSArray<T>& operator +=(const cSArray<T>& other);
    cSArray<T>  operator + (const T& object);
    cSArray<T>& operator +=(const T& object);

    /*
     * Unlike cArray the cSArray elements have a natural operator ==. The array
     * is composed from a stream-structed elements, so we can use memcmp to find
     * whether the arraies are equals, lower etc...
     */
    bool operator == (const cSArray<T>& other) const;
    bool operator <  (const cSArray<T>& other) const;

    /*
     * Using the implementation of both operator == and operator < we can
     * generate all the simple operators only by using this simple macro!
     */
    MAKE_SIMPLE_OPERATORS(cSArray<T>);

    /*
     * See cSerializedObject::isValid
     * Return true
     */
    virtual bool isValid() const;

    /*
     * See cSerializedObject::deserialize
     * Read an array: read a 32bit value for array size, and then write the
     *                elements
     */
    virtual void deserialize(basicInput& inputStream);

    /*
     * See cSerializedObject::serialize
     * Write an array: write a 32bit value for array size and then write the
     *                 elements
     *
     * IMPORTANT NOTE: If the array size is bigger then 32bit value,
     *                 exception will be thorwn
     */
    virtual void serialize(basicOutput& outputStream) const;

protected:
    /*
     * Copy from other cArray class.
     */
    virtual void copyFrom(const cArray<T> &other);
};

// Template implementation must be include inside the header file
#include "xStl/data/sarray.inl"
#endif // DMCOMP

/*
 * Array of uint8 called also cBuffer
 */
typedef cSArray<uint8> cBuffer;

/*
 * A buffer reference-count object.
 */
typedef cSmartPtr<cBuffer> cBufferPtr;


#ifdef XSTL_WINDOWS
    // Return the warnning behaviour back to it's original mode
    #pragma warning(pop)
#endif

#endif // __TBA_STL_SARRAY_H
