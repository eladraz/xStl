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

#ifndef __TBA_STL_DATA_SERIALIZEDOBJECT_H
#define __TBA_STL_DATA_SERIALIZEDOBJECT_H

/*
 * serializedObject.h
 *
 * A serialized object is an object which declare itself as streamable. The
 * object can saved into a stream and loaded later into the exact same state.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

// Forward deceleration
class basicInput;
class basicOutput;

/*
 * A serialized-object should support the following interface.
 * The object must have a default constructor (Class::Class()). If the object
 * must have data it's should have the 'isValid' function as well
 */
class cSerializedObject {
public:
    /*
     * You may inherit from me
     */
    virtual ~cSerializedObject();

    /*
     * Return true if the object is valid or false if the object is empty.
     */
    virtual bool isValid() const = 0;

    /*
     * Read from a stream the content of the object
     */
    virtual void deserialize(basicInput& inputStream) = 0;

    /*
     * Marshle the class objects into a stream
     */
    virtual void serialize(basicOutput& outputStream) const = 0;
};

#endif // __TBA_STL_DATA_SERIALIZEDOBJECT_H
