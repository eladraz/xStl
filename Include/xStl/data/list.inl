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
template <class T>
cList<T>::cList()
{
    m_first = NULL;
    m_last = NULL;
    initList();
    ASSERT(m_first != NULL);
    ASSERT(m_last != NULL);
}

template <class T>
cList<T>::~cList()
{
    freeList(); // Do all the needs.
}

template <class T>
cList<T>::cList(const cList<T> &other)
{
    m_first = NULL;
    m_last = NULL;
    initList();
    clone(other);
}

template <class T>
cList<T>::cList(const T& object)
{
    m_first = NULL;
    m_last = NULL;
    initList();
    append(object);
}

template <class T>
void cList<T>::clone(const cList<T> &other, bool assignPointer /* = false */)
{
    freeList();	// Clear the memory.
    if (assignPointer)
    {
        m_first = other.m_first;
        m_last  = other.m_last;
    } else
    {
        /* Start copying data */
        initList();

        cListT iterator index = other.begin();
        while (index != other.end())
        {
            append(*index);
            index++;
        }
    }
}

template <class T>
void cList<T>::append(const T& node)
{
    ASSERT(m_first != NULL);
    ASSERT(m_last != NULL);

    ListNode *tmp;	// The new node which will be entered.

    // Create the new node.
    tmp = new ListNode(&node);

    // See whether the list is empty
    if (m_first == m_last)
    {
        // The list is empty, create the first node.
        tmp->prev = NULL;
        tmp->next = m_last;

        m_first = tmp;
        m_last->prev = tmp;
    } else
    {
        /* Create an end-point node */
        tmp->prev = m_last->prev;
        tmp->next = m_last;
        m_last->prev->next = tmp;
        m_last->prev = tmp;
    }
}

template <class T>
void cList<T>::insert(iterator &position, const T& node)
{
    // Check iterator
    ASSERT(position.m_position != NULL);

    // Check first element
    if (m_first == m_last)
    {
        append(node);
        return;
    }

    ListNode *tmp;	// The new node which will be entered.

    // Create the new node.
    tmp = new ListNode(&node);
    tmp->prev = position.m_position->prev;
    tmp->next = position.m_position;

    // Link the new entry
    position.m_position->prev = tmp;
    if (m_first == position.m_position)
    {
        m_first = tmp;
    } else
    {
        tmp->prev->next = tmp;
    }
}

template <class T>
void cList<T>::insert(const T& node)
{
    ASSERT(m_first != NULL);
    ASSERT(m_last != NULL);

    ListNode *tmp;	// The new node which will be entered.

    // Create the new node.
    tmp = new ListNode(&node);
    tmp->prev = NULL;
    tmp->next = m_first;

    // Link the new entry
    m_first->prev = tmp;
    m_first = tmp;
}

template <class T>
bool cList<T>::isEmpty() const
{
    return (m_first == m_last);
}

template <class T>
void cList<T>::removeAll()
{
    freeList();
    initList();
}

// Removes the object at position and returns the
// iterator immediately after it
template <class T>
typename cList<T>::iterator cList<T>::remove(iterator &position)
{
    ASSERT(m_first != NULL);
    ASSERT(m_last != NULL);

    ListNode *tmp  = position.m_position; // Get the position of the node
    ListNode *prev = tmp->prev;
    ListNode *next = tmp->next;

    /* Delete the node */
    delete tmp;
    next->prev = prev;

    if (prev == NULL)
    {
        /* Delete the m_first */
        if (m_first == m_last)
        {
            XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
        }
        m_first	= next;
    } else
    {
        /* Delete and change order */
        prev->next = next;
    }
    return iterator(next);
}

template <class T>
bool cList<T>::isIn(const T& object)
{
    ListNode *tmp = m_first;	// The look-up object

    /* Go over the link-list */
    while ((tmp != m_last) && (!(*(tmp->information) == object)))
    {
        /* Go to the next node in the list */
        tmp = tmp->next;
    }

    /* Test whether the list EOL was reached */
    if (tmp == m_last)
    {
		return false;
    }
	return true;
}


template <class T>
void cList<T>::remove(const T& object)
{
    ListNode *tmp = m_first;	// The look-up object

    /* Go over the link-list */
    while ((tmp != m_last) && (!(*(tmp->information) == object)))
    {
        /* Go to the next node in the list */
        tmp = tmp->next;
    }

    /* Test whether the list EOL was reached */
    if (tmp == m_last)
    {
        XSTL_THROW(cException, EXCEPTION_OUT_OF_RANGE);
    }

    /* The previous node should be point to the next node */
    ListNode *prev = tmp->prev;
    ListNode *next = tmp->next; // This could never be NULL!! but m_last

    /* Delete the node */
    delete tmp;
    next->prev = prev;

    if (prev == NULL)
    {
        /* Delete the m_first */
        m_first	= next;
    } else
    {
        /* Delete and change order */
        prev->next = next;
    }
}

template <class T>
uint cList<T>::length() const
{
    uint count = 0; // Number of elements

    /* Go over the elements */
    for (cListT iterator i = begin(); i!=end(); i++)
    {
        ++count;
    }

    return count;
}

template <class T>
bool cList<T>::operator == (const cList<T> &other)
{
    return compare(begin(), end(), other.begin(), other.end());
}

template <class T>
bool cList<T>::operator != (const cList<T> &other)
{
    return !(*this == other);
}

template <class T>
cList<T> & cList<T>::operator = (const cList<T> &other)
{
    clone(other);
    return *this;
}

template <class S, class OSTREAM>
OSTREAM & operator << (OSTREAM &out, cList<S> &object)
{
    typename cList<S>::iterator index = object.begin();
    typename cList<S>::iterator last  = object.end();

    out << "[";

    while (index != last)
    {
        if (index + 1 != last) {
            out << *index << ", ";
        }
        else {
            out << *index;
        }
        index++;
    }
    out << "]";

    return out;
}

template <class T>
void cList<T>::initList()
{
    ASSERT(m_first == NULL);
    ASSERT(m_last == NULL);

    /* Setup pointers */
    /* Create an empty node at the end of the list */
    ListNode *end = new ListNode(NULL);

    end->next = NULL; /* It will always be the last element */
    end->prev = NULL; /* And for now it is the only element */

    m_first = end;
    m_last  = end;
}

template <class T>
void cList<T>::freeList()
{
    ASSERT(m_first != NULL);
    ASSERT(m_last != NULL);

    ListNode *temp = m_first; /* For scaning the list */
    ListNode *tdel;           /* To delete a node in the list */

    /* Scanning the link-list from it's begins Until the end */
    while (temp != NULL)
    {
        tdel = temp;
        temp = temp->next;	// Get the next pointer.
        delete tdel;		// Free the memory of the last pointer.
		                    // The deconstructor of T will be called.
    }

    m_first = NULL;
    m_last  = NULL;
}


