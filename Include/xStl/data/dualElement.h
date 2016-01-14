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

#ifndef __TBA_XSTL_DUALELEMENT_H
#define __TBA_XSTL_DUALELEMENT_H

/*
 * cDualElement
 *
 * Template struct of two elements,since this kind of datastruct is so widely in
 * used, we defined it over here.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
template <class A, class B>
class cDualElement
{
public:
    // Default constructor
    cDualElement()
    {
    }
    // Default init constructor
    cDualElement(const A& a, const B& b) :
        m_a(a),
        m_b(b)
    {
    }
    // Copy constructor and operator = will generated by the compiler

    // Operator ==
    bool operator == (const cDualElement<A,B>& other) const
    {
        return ((other.m_a == m_a) && (other.m_b == m_b));
    }

    // Operator !=
    bool operator != (const cDualElement<A,B>& other) const
    {
        return !((other.m_a == m_a) && (other.m_b == m_b));
    }

    // The first member
    A m_a;
    // The second member
    B m_b;
};

#endif // __TBA_XSTL_DUALELEMENT_H
