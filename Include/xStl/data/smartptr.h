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

/*
 * smartptr.h
 *
 * Implement the smart-pointer, auto-pointer and smart-array-pointer and a
 * refernce object.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/xstlLockable.h"
#include "xStl/data/counter.h"
#include "xStl/except/assert.h"

/*
 * The object destruction method.
 */
enum DestructionMethod
{
    SMARTPTR_DESTRUCT_NONE          = 0, // The destructor didn't call
    SMARTPTR_DESTRUCT_DELETE        = 1, // The delete operator will be called.
    SMARTPTR_DESTRUCT_ARRAY_DELETE  = 2  // The delete[] operator will be called.
};

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to ofer. Warning C4505 should be over-written for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif


/*
 * cSmartPtr
 *
 * A global implemetation for all smarts pointers and auto-pointers.
 * The constructor gets destruction method for the object.
 *
 * Operator = (T* other) is the most dangerous function to be used because
 * the destruction method is save as the previous setting. So far this
 * method will be private until CR results.
 *
 * This class is multi-threaded and multi-processor enabled.
 */
template<class T>
class cSmartPtr
{
public:
    // Constructors

    /*
     * Default constructor. Create NULL pointer reference.
     */
    cSmartPtr();

    /*
     * Constructor. Wrap a pointer which a smart-pointer reference count object.
     * The default destruction method is 'operator delete'.
     * The object can be NULL.
     *
     * object - The object to be wrap.
     * destructionType - The method of destruction.
     */
    explicit cSmartPtr(T* object,
                       DestructionMethod destructionType = SMARTPTR_DESTRUCT_DELETE);

    /*
     * Copy-constructor. (with refrence increasment)
     */
    cSmartPtr(const cSmartPtr<T>& other);

    /*
     * Destructor. Derefrence the pointer and delete it when 0 reaches.
     */
    ~cSmartPtr();

    // operators

    /*
     * Return pointer reference to the object.
     */
    inline operator T*(void) { return m_object; }
    inline T& operator*(void) { return *m_object; }
    inline T* operator->(void) { return m_object; }

    /*
     * Return pointer reference to the const object.
     */
    inline operator const T*(void) const { return m_object; }
    inline const T& operator*(void) const { return *m_object; }
    inline const T* operator->(void) const { return m_object; }

    /*
     * Copy operator. Dereference the current object and copy the smart-pointer
     * reference by other.
     */
    cSmartPtr<T>& operator=(const cSmartPtr<T>& other);

    /*
     * Return true if the smartptr-objects pointed to the same object or not.
     */
    bool operator == (const cSmartPtr<T>& other);
    bool operator != (const cSmartPtr<T>& other);

    // Destruction method

    /*
     * Return the destruction method used by the smart-pointer.
     */
    DestructionMethod getDestructMethod();

    /*
     * Change the destruction method of the smart-pointer.
     */
    void setDestructMethod(DestructionMethod destructionType);

    /*
     * Return whether the refrence object is NULL which indicate that the object
     * is empty
     */
    bool isEmpty() const;

    /*
     * Return the reference object stored by the smart-pointer.
     */
    inline T* getPointer() { return m_object;};

    /*
     * The const function which return a const reference for the object.
     */
    inline const T* getPointer() const {return m_object;};

    /*
     * Return the counter object for this class. Used for copy-on-write objects
     * implementations which want to know how many objects are reference to.
     *
     * Throw exception if the object is NULL.
     */
    const cCounter& getCounter() const;

private:
    /*
     * Delete the correct reference counter and create new reference.
     * NOTE: The deleted method is the same as the old object!!
     */
    cSmartPtr<T>& operator=(T* other);

    // Release

    /*
     * Decrease refrence by one and release the object according to the
     * destruction method.
     */
    void free();

    /*
     * Decrease the refernce counter and test whether the object should be deleted.
     * Return true if the object should be deleted.
     */
    bool decreaseAndTest();

    // Refrence counter object
    T*                m_object;
    cCounter*         m_refrenceCounter;
    DestructionMethod m_destructionType;
};


/*
 * In order that the cSmartPtr class can be in used in multi-threaded and multi
 * processor safely a global lockable should be in used.
 *
 * Note that this lockable should be destruct at the end of all atexit dtors.
 * Win32 ring3 application not free this memory.
 * The XDK calls to this destructor.
 */
class cSmartPtrLockable {
public:
    /*
     * Return the singleton for the global lockable object.
     */
    static cSmartPtrLockable& getInstance();

    /*
     * The global lockable
     */
    cXstlLockable globalSmartPtrLock;

private:
    // Deny copy-constructor and operator =
    cSmartPtrLockable(const cSmartPtrLockable& other);
    cSmartPtrLockable& operator = (const cSmartPtrLockable& other);

    // The XDK must now how to destructor this class
    friend class cXDKLibCPP;

    /*
     * Free the singleton
     */
    static void unload();

    /*
     * Protect the singleton.
     */
    cSmartPtrLockable();

    /*
     * The singleton object.
     */
    static cSmartPtrLockable* m_singleton;
};

// Implementation call (this is a template function)
#include "xStl/data/smartptr.inl"

#ifdef XSTL_WINDOWS
    // Return the warnning behaviour back to it's original mode
    #pragma warning(pop)
#endif

#endif //__TBA_STL_SMARTPTR_H

