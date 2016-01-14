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

#ifndef __TBA_XSTL_STREAM_MARGINSTRINGERSTREAM_H
#define __TBA_XSTL_STREAM_MARGINSTRINGERSTREAM_H

/*
 * marginStringerStream.h
 *
 * Delegate a stringer stream and add margin attributes
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/stream/stringerStream.h"

/*
 * Delegate a stringer stream and add margin attributes
 *
 * Usage:
 *    void printComplexStruct(Object& object,
 *                            cMarginStringerStream& output)
 *    {
 *       output << "FieldA: " << blabla... << enld;
 *       output << "FieldC: " << packapacka... << enld;
 *       if (object.hasInnerObject)
 *       {
 *           output.setMargin(output.getMargin() + 4);
 *           printComplexStruct(object.getInnerObject(), output);
 *           output.setMargin(output.getMargin() - 4);
 *       }
 *    }
 *
 * NOTE: This class is not thread-safe
 */
class cMarginStringerStream : public cStringerStream {
public:
    /*
     * Constructor. Delegate a stringer stream and add margin attributes
     *
     * output - The delegated stringer stream
     * margin - The margin settings
     * marginChar - The space character
     */
    cMarginStringerStream(cStringerStream& output,
                          uint margin = 0,
                          character marginChar = XSTL_CHAR(' '));

    /*
     * Return the current margin settings
     */
    uint getMargin() const;

    /*
     * Change the margin settings
     */
    void setMargin(uint newMargin);

    /*
     * Equals to setMargin(getMargin() + marginInc)
     */
    void increase(uint marginInc);

    /*
     * Equals to setMargin(getMargin() - marginDec)
     */
    void decrease(uint marginDec);

private:
    // Deny copy-constructor and operator =
    cMarginStringerStream(const cMarginStringerStream& other);
    cMarginStringerStream& operator = (const cMarginStringerStream& other);

    /*
     * Add a margin to the output string
     */
    virtual void outputString(const cString& string);

    // The delegated stringer stream
    cStringerStream& m_output;
    // The margin settings
    uint m_margin;
    // The space character
    cString m_marginChar;
};

#endif // __TBA_XSTL_STREAM_MARGINSTRINGERSTREAM_H
