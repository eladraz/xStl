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

/**
 * @addtogroup xstl_data_containers
 * @{
 */

/**
 * @file sarray.h
 * @brief Declaration of the struct-array.
 *
 * @details A struct array is an array which stores objects without
 *    copy-construction and can be copied using 'memcpy' command.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/operators.h"
#include "xStl/data/array.h"
#include "xStl/data/serializedObject.h"
#include "xStl/data/smartptr.h"

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to offer. Warning C4505 should be disabled for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif

#ifdef DMCOMP_NOAD
    /**
     * @brief The default C standard (By Digital Mars compiler) forbids
     *    extending
     *   template class since the base class is a dependent name.
     *
     * @details Look at the following code:
     *
     * @code
     * template <class T>
     * class Base {
     * public:
     *     void func();
     *     int m_member;
     *     T* m_object;
     * };
     *
     * template <class T>
     * class Driven : public Base<T> {
     * public:
     *     Driven() :
     *         m_object(NULL),           // <=== COMPILER ERROR!
     *         m_member(4)               // <=== COMPILER ERROR!
     *     {
     *         func();                   // <=== COMPILER ERROR!
     *     }
     * };
     * @endcode
     *
     * Anyway, the compiler doesn't recognize the base template class in the
     * derived
     * class implementation code.
     * It can't recognize any static members/static const/methods/members of the
     * base class
     *
     * Hence, using the default settings of DMC compiler the struct-array
     * optimization will be disabled.
     * Use dmc.exe -Ad to override this behaviour.
     *
     * Elad.
     */
    #define cSArray cArray
#else

/**
 * @brief Array class. T cannot have copy-constructor or operator =.
 *
 * @details T must be struct, class with members only. The class contains
 *    build-up optimized code, (Since memcpy is used to copy data and not
 *    operator =.)
 *
 * @tparam T The type of the elements. It must be copyable with memcpy.
 */
template <class T>
class cSArray : public cArray<T>
                ,public cSerializedObject
{
public:
    /**
     * @brief Explicit default constructor.
     *
     * @details Create array of 'numberOfElements' elements (uninitialized
     *    objects)
     *   with page size of 'pageSize'.
     *   Notice that the array is limited by the size of the compiler 'int'.
     *
     * @param numberOfElements The number of elements in the array
     * @param pageSize The page-size for the array. See cArray for paging
     *    mechanism
     * @throws cException (EXCEPTION_OUT_OF_MEM) .
     */
    explicit cSArray(uint numberOfElements = 0,
                     uint pageSize = 0);

    /**
     * @brief Create array of 'length' elements and copy to it the 'staticArray'
     *    array.
     *
     * @details The copy is done using memcpy instead of operator =.
     *
     * @param staticArray The array to copy.
     * @param length The number of elements to copy.
     * @param pageSize The page-size for the array. See cArray for paging
     *    mechanism.
     */
    explicit cSArray(const T* staticArray,
                     uint length,
                     uint pageSize = 0);

    /**
     * @brief Copy-constructor.
     *
     * @param other The array to copy.
     */
    cSArray(const cSArray<T> & other);

    /**
     * @brief Improve the changing memory by copying the members using memcpy
     *
     * @param newSize The new number of elements in the array.
     * @param preserveMemory If true the previous content is preserved.
     */
    virtual void changeSize(uint newSize,
                            bool preserveMemory = true);

    /**
     * @brief Insert single object to the end of the array.
     *
     * @details The copy of the new element is by invoking memcpy.
     *
     * @param object The object to append
     * @throws cException (EXCEPTION_OUT_OF_MEM) .
     */
    virtual void append(const T& object);

    /**
     * @brief Insert a new array to the end of the array.
     *
     * The copy method is by invoking `memcpy` instead of operator =.
     *
     * @param array The array to append.
     * @throws cException (EXCEPTION_OUT_OF_MEM) if there is not enough memory.
     */
    virtual void append(const cArray<T> & array);


    /**
     * @brief Copy range from the array using memcpy.
     *
     * @details See cArray::copyRange function for more documentation about this
     *    function.
     *
     * @param other The output array
     * @param startLocation The start location in the array
     * @param count The number of elements to copy.
     * @throws cException (EXCEPTION_OUT_OF_MEM) .
     */
    virtual void copyRange(const cArray<T> & other,
                           uint startLocation,
                           uint count);

    // Operators

    /**
     * @brief Operator =. Copy from other cSArray
     *
     * @param other The array to copy from.
     * @return A reference to this array.
     */
    cSArray<T>& operator = (const cSArray<T>& other);

    /**
     * @brief Return a new array which contains this array followed by another
     *    array.
     *
     * @param other The array to append.
     * @return The new array.
     */
    cSArray<T>  operator + (const cSArray<T>& other);
    /**
     * @brief Append another array to this array.
     *
     * @param other The array to append.
     * @return A reference to this array.
     */
    cSArray<T>& operator +=(const cSArray<T>& other);
    /**
     * @brief Return a new array which contains this array followed by an
     *    element.
     *
     * @param object The element to append.
     * @return The new array.
     */
    cSArray<T>  operator + (const T& object);
    /**
     * @brief Append an element to this array.
     *
     * @param object The element to append.
     * @return A reference to this array.
     */
    cSArray<T>& operator +=(const T& object);

    /**
     * @brief Unlike cArray the cSArray elements have a natural operator ==.
     *
     * @details The array is composed of stream-structured elements, so we can
     *    use memcmp to find whether the arrays are equal, lower etc...
     *
     * @param other The array to compare with.
     * @return True if the arrays are equal.
     */
    bool operator == (const cSArray<T>& other) const;
    /**
     * @brief Compare the stream representation of the arrays.
     *
     * @param other The array to compare with.
     * @return True if this array is lower than @a other.
     */
    bool operator <  (const cSArray<T>& other) const;

    /**
     * @brief Using the implementation of both operator == and operator \< we
     *    can
     *   generate all the simple operators only by using this simple macro!
     */
    MAKE_SIMPLE_OPERATORS(cSArray<T>);

    /**
     * @return True.
     * @see cSerializedObject::isValid
     */
    virtual bool isValid() const;

    /**
     * @brief Read an array: read a 32 bit value for the array size, and then
     *    read the elements.
     *
     * @param inputStream The stream to read from.
     * @see cSerializedObject::deserialize
     */
    virtual void deserialize(basicInput& inputStream);

    /**
     * @brief Write an array: write a 32 bit value for the array size, and then
     *    write the elements.
     *
     * @param outputStream The stream to write to.
     * @warning If the array size is bigger than a 32 bit value, an exception
     *    will be thrown.
     * @see cSerializedObject::serialize
     */
    virtual void serialize(basicOutput& outputStream) const;

protected:
    /**
     * @brief Copy from other cArray class.
     *
     * @param other The array to copy from.
     */
    virtual void copyFrom(const cArray<T> &other);
};

// Template implementation must be included inside the header file
#include "xStl/data/sarray.inl"
#endif // DMCOMP

/**
 * @brief Array of uint8, also called cBuffer
 */
typedef cSArray<uint8> cBuffer;

/**
 * @brief A buffer reference-count object.
 */
typedef cSmartPtr<cBuffer> cBufferPtr;


#ifdef XSTL_WINDOWS
    // Return the warning behaviour back to its original mode
    #pragma warning(pop)
#endif


/** @} */
#endif // __TBA_STL_SARRAY_H
