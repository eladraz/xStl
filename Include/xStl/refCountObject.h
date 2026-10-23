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

#ifndef __TBA_XSTL_REFCOUNTOBJECT_H
#define __TBA_XSTL_REFCOUNTOBJECT_H

/**
 * @addtogroup xstl_core
 * @{
 */

/**
 * @file refCountObject.h
 * @brief Contains code which makes classes into reference-count modules.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/counter.h"
#include "xStl/os/xstlLockable.h"
#include "xStl/os/lock.h"

/**
 * @brief Inside the constructor list, reset all ref-count object generated
 *    members
 *
 * @see REFCOUNTABLE
 */
#define REFCOUNTABLE_INIT m_referenceCounter(NULL)

/**
 * @brief Make a class reference-countable over a data type.
 *
 * The macro declares, inside the class, the members and helper functions that
 * let several instances of the class share one object. Usage in the class
 * header:
 *
 * @code
 * class MyClass {
 * ...
 * REFCOUNTABLE(Data*);
 * REFCOUNTABLE_COPY(MyClass);
 * REFCOUNT_SHARABLE_APPENDIX(Data*);
 * };
 *
 * REFCOUNTABLE_CPP(MyClass);   // in the CPP file
 * @endcode
 *
 * The macro generates the following members: `m_object` (the shared object),
 * `m_referenceCounter` (the count of all opened references) and a static
 * protected lock that protects the counter.
 *
 * It also generates the public function `getObject()` (const and non-const
 * versions) and the protected functions `init(object)`, which the constructor
 * calls to start the reference count, and `free()`, which reduces the reference
 * count and returns true when the last reference was released (the caller then
 * deletes the object).
 *
 * In the constructor of the class, call `init()` after REFCOUNTABLE_INIT:
 * @code
 * MyClass::MyClass() : REFCOUNTABLE_INIT {
 *     init(new Data(4));
 * }
 * @endcode
 *
 * In the destructor use the following code instead of deleting the object
 * directly:
 * @code
 * MyClass::~MyClass() {
 *     if (free()) {
 *         delete getObject();
 *     }
 * }
 * @endcode
 *
 * @param DataType The type name of the data which is reference-countable. It
 *    can be a typedef, struct name, class name or a pointer (for sharable
 *    objects).
 * @warning For sharable objects the data type is a pointer.
 */
#define REFCOUNTABLE(DataType)                                          \
    protected:                                                          \
        DataType m_object;                                              \
        cCounter* m_referenceCounter;                                   \
        static cXstlLockable m_refCountLockable;                        \
        void init(DataType object) {                                    \
            m_object = object;                                          \
            m_referenceCounter = new cCounter(1);                       \
        }                                                               \
        bool free() {                                                   \
            cLock lock(m_refCountLockable);                             \
            if (m_referenceCounter == NULL) return false;               \
            bool ret = (m_referenceCounter->decrease() == 0);           \
            if (ret) delete m_referenceCounter;                         \
            return ret;                                                 \
        }                                                               \
    public:                                                             \
        inline const DataType getObject() const {                       \
            ASSERT(m_referenceCounter != NULL);                         \
            return m_object;                                            \
        }                                                               \
        inline DataType getObject() {                                   \
            ASSERT(m_referenceCounter != NULL);                         \
            return m_object;                                            \
        }

/**
 * @brief Must be called inside the CPP file in order to generate the protective
 *   mutex.
 *
 * @param MyClass The name of the class which uses the reference-countable
 *    object.
 */
#define REFCOUNTABLE_CPP(MyClass) cXstlLockable MyClass::m_refCountLockable;


/**
 * @brief Generate a copy-constructor and operator = for a reference-countable
 *    class.
 *
 * Use REFCOUNTABLE_COPY for a class whose only member is the reference-count
 * object. Use REFCOUNTABLE_COPY_MORE for a class that has other members; it
 * additionally calls the class method `void copy(const ClassName& other)` to
 * copy them. For example:
 * @code
 * class MyClass {
 * private:
 *     void copy(const MyClass& other) {
 *         m_member1 = other.m_member1;
 *         m_member2 = other.m_member2;
 *         strcpy(m_member3, other.m_member3);
 *     }
 *
 *     int m_member1;
 *     OtherClass m_member2;
 *     char m_member3[100];
 * };
 * @endcode
 *
 * @param ClassName The name of the class in which the macro is used.
 * @note In the copy method, the reference-countable object is invalid.
 */
#define REFCOUNTABLE_COPY(ClassName)                                 \
    public:                                                          \
    ClassName(const ClassName& other) : REFCOUNTABLE_INIT {          \
        cLock lock(m_refCountLockable);                              \
        m_object = other.m_object;                                   \
        m_referenceCounter = other.m_referenceCounter;               \
        m_referenceCounter->increase();                              \
    }                                                                \
    const ClassName& operator = (const ClassName& other) {           \
        if (this != & other) {                                       \
            free();                                                  \
            cLock lock(m_refCountLockable);                          \
            m_object = other.m_object;                               \
            m_referenceCounter = other.m_referenceCounter;           \
            m_referenceCounter->increase();                          \
        }                                                            \
        return *this;                                                \
    }                                                                \

/**
 * @brief Generate a copy-constructor and operator = which also call the class
 *    method `copy()`.
 *
 * Like REFCOUNTABLE_COPY, but additionally calls `void copy(const ClassName&
 * other)` so that the other members of the class are copied too.
 *
 * @param ClassName The name of the class in which the macro is used.
 * @see REFCOUNTABLE_COPY
 */
#define REFCOUNTABLE_COPY_MORE(ClassName)                            \
    ClassName(const ClassName& other) : REFCOUNTABLE_INIT {          \
        copy(other);                                                 \
        cLock lock(m_refCountLockable);                              \
        m_object = other.m_object;                                   \
        m_referenceCounter = other.m_referenceCounter;               \
        m_referenceCounter->increase();                              \
    }                                                                \
    const ClassName& operator = (const ClassName& other) {           \
        if (this != & other) {                                       \
            free();                                                  \
            copy(other);                                             \
            cLock lock(m_refCountLockable);                          \
            m_object = other.m_object;                               \
            m_referenceCounter = other.m_referenceCounter;           \
            m_referenceCounter->increase();                          \
        }                                                            \
        return *this;                                                \
    }                                                                \


/**
 * @brief For sharable objects, use this prefix macro in order to generate the
 *   operators: (cast), *, -\>
 *
 * @param DataType The type of the shared object.
 */
#define REFCOUNT_SHARABLE_APPENDIX(DataType)                         \
    public:                                                          \
    inline operator DataType*(void) { return m_object; }             \
    inline DataType& operator*(void) { return *m_object; }           \
    inline DataType* operator->(void) { return m_object; }



/** @} */
#endif // __TBA_XSTL_REFCOUNTOBJECT_H
