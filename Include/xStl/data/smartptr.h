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

#ifndef __TBA_STL_SMARTPTR_H
#define __TBA_STL_SMARTPTR_H

/**
 * @addtogroup xstl_data_pointers
 * @{
 */

/**
 * @file smartptr.h
 * @brief Implement the smart-pointer, auto-pointer and smart-array-pointer and
 *    a
 *   reference object.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/os/xstlLockable.h"
#include "xStl/data/counter.h"
#include "xStl/except/assert.h"

/**
 * @brief The object destruction method.
 */
enum DestructionMethod
{
    SMARTPTR_DESTRUCT_NONE          = 0, ///< The destructor didn't call
    /**
     * @brief The delete operator will be called.
     */
    SMARTPTR_DESTRUCT_DELETE        = 1,
    /**
     * @brief The delete[] operator will be called.
     */
    SMARTPTR_DESTRUCT_ARRAY_DELETE  = 2
};

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to offer. Warning C4505 should be disabled for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif


/**
 * @brief A global implementation for all smart pointers and auto-pointers.
 *
 * @details The constructor gets destruction method for the object.
 *
 * Operator = (T* other) is the most dangerous function to be used because
 * the destruction method is saved as the previous setting. So far this
 * method will be private until CR results.
 *
 * This class is multi-threaded and multi-processor enabled.
 *
 * @tparam T The type of the object the pointer holds.
 */
template<class T>
class cSmartPtr
{
public:
    // Constructors

    /**
     * @brief Default constructor. Create NULL pointer reference.
     */
    cSmartPtr();

    /**
     * @brief Constructor. Wrap a pointer with a smart-pointer reference count
     *    object.
     *
     * @details The default destruction method is 'operator delete'. The object
     *    can be NULL.
     *
     * @param object The object to be wrapped.
     * @param destructionType The method of destruction.
     */
    explicit cSmartPtr(T* object,
                       DestructionMethod destructionType = SMARTPTR_DESTRUCT_DELETE);

    /**
     * @brief Copy-constructor. (with reference increment)
     *
     * @param other The smart-pointer to copy. The reference counter is
     *    incremented.
     */
    cSmartPtr(const cSmartPtr<T>& other);

    /**
     * @brief Destructor. Dereference the pointer and delete it when the count
     *    reaches
     *   0.
     */
    ~cSmartPtr();

    // operators

    /**
     * @brief Return pointer reference to the object.
     *
     * @return Pointer reference to the object.
     */
    inline operator T*(void) { return m_object; }
    /**
     * @brief Return a reference to the object.
     *
     * @return A reference to the object.
     */
    inline T& operator*(void) { return *m_object; }
    /**
     * @brief Return a pointer to the object.
     *
     * @return A pointer to the object.
     */
    inline T* operator->(void) { return m_object; }

    /**
     * @brief Return pointer reference to the const object.
     *
     * @return Pointer reference to the const object.
     */
    inline operator const T*(void) const { return m_object; }
    /**
     * @brief Return a const reference to the object.
     *
     * @return A const reference to the object.
     */
    inline const T& operator*(void) const { return *m_object; }
    /**
     * @brief Return a const pointer to the object.
     *
     * @return A const pointer to the object.
     */
    inline const T* operator->(void) const { return m_object; }

    /**
     * @brief Copy operator. Dereference the current object and copy the
     *    smart-pointer
     *   reference by other.
     *
     * @param other The smart-pointer to copy from.
     * @return A reference to this smart-pointer.
     */
    cSmartPtr<T>& operator=(const cSmartPtr<T>& other);

    /**
     * @brief Return true if the smartptr-objects pointed to the same object or
     *    not.
     *
     * @param other The smart-pointer to compare with.
     * @return True if both smart-pointers point to the same object.
     */
    bool operator == (const cSmartPtr<T>& other);
    /**
     * @brief Return true if the smart-pointers point to different objects.
     *
     * @param other The smart-pointer to compare with.
     * @return True if the smart-pointers point to different objects.
     */
    bool operator != (const cSmartPtr<T>& other);

    // Destruction method

    /**
     * @brief Return the destruction method used by the smart-pointer.
     *
     * @return The destruction method used by the smart-pointer.
     */
    DestructionMethod getDestructMethod();

    /**
     * @brief Change the destruction method of the smart-pointer.
     *
     * @param destructionType The new destruction method.
     */
    void setDestructMethod(DestructionMethod destructionType);

    /**
     * @brief Return whether the reference object is NULL which indicates that
     *    the
     *   object is empty
     *
     * @return Whether the reference object is NULL which indicates that the
     *    object is empty.
     */
    bool isEmpty() const;

    /**
     * @brief Return the reference object stored by the smart-pointer.
     *
     * @return The reference object stored by the smart-pointer.
     */
    inline T* getPointer() { return m_object;};

    /**
     * @brief The const function which return a const reference for the object.
     *
     * @return A const pointer to the object.
     */
    inline const T* getPointer() const {return m_object;};

    /**
     * @brief Return the counter object for this class.
     *
     * @details Used for copy-on-write objects implementations which want to
     *    know how many objects reference it.
     *
     * @return The counter object for this class.
     * @throws cException if the object is NULL.
     */
    const cCounter& getCounter() const;

private:
    /**
     * @brief Delete the correct reference counter and create new reference.
     *
     * @note The deleted method is the same as the old object!!
     */
    cSmartPtr<T>& operator=(T* other);

    // Release

    /**
     * @brief Decrease reference by one and release the object according to the
     *   destruction method.
     */
    void free();

    /**
     * @brief Decrease the reference counter and test whether the object should
     *    be
     *   deleted.
     *
     * @return True if the object should be deleted.
     */
    bool decreaseAndTest();

    /**
     * @brief Reference counter object
     */
    T*                m_object;
    cCounter*         m_referenceCounter;
    DestructionMethod m_destructionType;
};


/**
 * @brief In order that the cSmartPtr class can be used in multi-threaded and
 *    multi
 *   processor environments safely, a global lockable should be used.
 *
 * @note that this lockable should be destructed at the end of all atexit dtors.
 *    Win32 ring3 applications do not free this memory. The XDK calls this
 *    destructor.
 */
class cSmartPtrLockable {
public:
    /**
     * @brief Return the singleton for the global lockable object.
     *
     * @return The singleton for the global lockable object.
     */
    static cSmartPtrLockable& getInstance();

    /**
     * @brief The global lockable
     */
    cXstlLockable globalSmartPtrLock;

private:
    /**
     * @brief Deny copy-constructor and operator =
     */
    cSmartPtrLockable(const cSmartPtrLockable& other);
    cSmartPtrLockable& operator = (const cSmartPtrLockable& other);

    // The XDK must know how to destruct this class
    friend class cXDKLibCPP;

    /**
     * @brief Free the singleton
     */
    static void unload();

    /**
     * @brief Protect the singleton.
     */
    cSmartPtrLockable();

    /**
     * @brief The singleton object.
     */
    static cSmartPtrLockable* m_singleton;
};

// Implementation call (this is a template function)
#include "xStl/data/smartptr.inl"

#ifdef XSTL_WINDOWS
    // Return the warning behaviour back to its original mode
    #pragma warning(pop)
#endif


/** @} */
#endif //__TBA_STL_SMARTPTR_H

