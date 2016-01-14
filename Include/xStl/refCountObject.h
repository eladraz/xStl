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

/*
 * refCountObject.h
 *
 * Contains code which make classes into reference-count modules.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/counter.h"
#include "xStl/os/xstlLockable.h"
#include "xStl/os/lock.h"

// Inside the constructor list, reset all ref-count object generated members
// See REFCOUNTABLE
#define REFCOUNTABLE_INIT m_referenceCounter(NULL)

/*
 * This macro makes a class top be reference-countable over a data-type.
 *
 * Parameters:
 *   DataType   - The type-name of the data which is reference-countable, this
 *                value can be a typedef, struct-name, class-name or pointer to
 *                for sharble-objects
 *
 * The usage is inside the class .H file:
 *  class MyClass {
 *    ...
 *    REFCOUNTABLE(Data*);
 *    REFCOUNTABLE_COPY(MyClass);
 *    REFCOUNT_SHARABLE_APPENDIX(Data*);
 *  };
 *
 *  REFCOUNTABLE_CPP(MyClass);
 *
 * This single line will generated all functions and members needed to makes
 * the class a reference-countable object.
 * But there is still a few works to do in order to link the operation
 * properly.
 *
 * IMPORTANT NOTE: Notice the pointer! For sharable objects the data-type is
 *                 a pointer!
 *
 *
 * The macro generate and handle the following members:
 *    m_object           - Pointer to the reference-countable member
 *    m_referenceCounter - The count of all opened references
 *
 * The macro generate the following functions:
 *   public:
 *     getObject()  - Return const/non-const object
 *
 *   protected:
 *     init(object) - Called by the constructor in order to init the reference
 *                    count.
 *     free()       - Called in order to reduce the reference-count object
 *
 * In the constructor of the class you should invoke a call to the 'init'
 * function:
 *   MyClass::MyClass() :
 *      REFCOUNTABLE_INIT {
 *      init(new Data(4));
 *   }
 *
 * In the destructor you should use the following code instead:
 *   MyClass::~MyClass() {
 *      if (free()) {
 *         delete getObject();
 *      }
 *   }
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

/*
 * Must be called inside the CPP file in order to generate the protective
 * mutex.
 */
#define REFCOUNTABLE_CPP(MyClass) cXstlLockable MyClass::m_refCountLockable;


/*
 * This macro generate copy-constructor and operator = for a reference-
 * countable class. There are two ways to generate this copy methods:
 *  - For class which have only the reference-count object is the
 *    'REFCOUNTABLE_COPY' definitions
 *  - For class which have several of members use the
 *    'REFCOUNTABLE_COPY_MORE' definitions. This definition adds a call
 *    to 'void copy(const ClassName& other)' method
 *
 * Parameters:
 *   ClassName  - The name of the class which the operation is taken place
 *
 * In case the REFCOUNTABLE_COPY_MORE is in used the class should also
 * implement the 'copy()' method:
 *   class MyClass {
 *   private:
 *       void copy(const MyClass& other) {
 *          m_member1 = other.m_member1;
 *          m_member2 = other.m_member2;
 *          strcpy(m_member3, other.m_member3);
 *       }
 *
 *       int m_member1;
 *       OtherClass m_member1;
 *       char m_member1[100];
 *   };
 *
 * NOTE: In the copy method, the reference-countable object is invalid.
 *
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


/*
 * For sharable objects, used this prefix macro in order to generate
 * the operators: (cast), *, ->
 */
#define REFCOUNT_SHARABLE_APPENDIX(DataType)                         \
    public:                                                          \
    inline operator DataType*(void) { return m_object; }             \
    inline DataType& operator*(void) { return *m_object; }           \
    inline DataType* operator->(void) { return m_object; }


#endif // __TBA_XSTL_REFCOUNTOBJECT_H
