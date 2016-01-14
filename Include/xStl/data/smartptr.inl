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
 * smartptr.inl
 *
 * Implementation code of cSmartPtr object.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/os/lock.h"

template<class T>
cSmartPtr<T>::cSmartPtr() :
    m_object(NULL),
    m_refrenceCounter(new cCounter(1)),
    m_destructionType(SMARTPTR_DESTRUCT_DELETE)
{
}

template<class T>
cSmartPtr<T>::cSmartPtr(T* object, DestructionMethod destructionType /* = SMARTPTR_DESTRUCT_DELETE */) :
    m_object(object),
    m_refrenceCounter(NULL),
    m_destructionType(destructionType)
{
    m_refrenceCounter = new cCounter(1);
    ASSERT(m_refrenceCounter != NULL);
}

template<class T>
void cSmartPtr<T>::free()
{
    // Decrease the refrence
    if (decreaseAndTest())
    {
        if (m_object != NULL)
        {
            // Destroy the object
            switch (m_destructionType)
            {
            case SMARTPTR_DESTRUCT_NONE: break;
            case SMARTPTR_DESTRUCT_DELETE:       delete m_object; break;
            case SMARTPTR_DESTRUCT_ARRAY_DELETE: delete[] m_object; break;
            default:
                // ERROR.
                ASSERT(false);
            }
        }

        delete m_refrenceCounter;
    }
    m_refrenceCounter = NULL;
    m_object = NULL;
}

template<class T>
bool cSmartPtr<T>::decreaseAndTest()
{
    // Protected the atomic operation of decreasing and testing
    cLock lock(cSmartPtrLockable::getInstance().globalSmartPtrLock);
    return (m_refrenceCounter->decrease() == 0);
}

template<class T>
cSmartPtr<T>::~cSmartPtr()
{
    free();
}

template<class T>
cSmartPtr<T>::cSmartPtr(const cSmartPtr<T>& other) :
    m_object(NULL)
{
    cLock lock(cSmartPtrLockable::getInstance().globalSmartPtrLock);
    m_refrenceCounter = other.m_refrenceCounter;
    m_destructionType = other.m_destructionType;
    m_object = other.m_object;
    m_refrenceCounter->increase();
}

template<class T>
cSmartPtr<T>& cSmartPtr<T>::operator=(T* other)
{
    free();
    m_object = other;
    m_refrenceCounter = new cCounter(1);
}

template<class T>
cSmartPtr<T>& cSmartPtr<T>::operator=(const cSmartPtr<T>& other)
{
    if (this != &other)
    {
        // Decrease object reference
        free();

        // Increase reference counter. This operation must be atomic!
        // The object can delete here (even if we have a reference)
        // because two thread can decrease without locking and...
        cLock lock(cSmartPtrLockable::getInstance().globalSmartPtrLock);
        m_refrenceCounter = other.m_refrenceCounter;
        m_destructionType = other.m_destructionType;
        m_object          = other.m_object;
        m_refrenceCounter->increase();
    }
    return *this;
}

template<class T>
bool cSmartPtr<T>::operator == (const cSmartPtr<T>& other)
{
    if (m_object == other.m_object)
    {
        ASSERT(m_refrenceCounter == other.m_refrenceCounter);
        return true;
    }
    return false;
}

template<class T>
bool cSmartPtr<T>::operator != (const cSmartPtr<T>& other)
{
    return !(*this == other);
}

template<class T>
DestructionMethod cSmartPtr<T>::getDestructMethod()
{
    return m_destructionType;
}

template<class T>
void cSmartPtr<T>::setDestructMethod(DestructionMethod destructionType)
{
    m_destructionType = destructionType;
}

template<class T>
bool cSmartPtr<T>::isEmpty() const
{
    return (m_object == NULL);
}

/*
template<class T>
inline T* cSmartPtr<T>::getPointer()
{
    return m_object;
}

template<class T>
inline const T* cSmartPtr<T>::getPointer() const
{
    return m_object;
}
*/

template<class T>
const cCounter& cSmartPtr<T>::getCounter() const
{
    return *m_refrenceCounter;
}
