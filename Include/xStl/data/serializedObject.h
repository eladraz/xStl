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

/**
 * @addtogroup xstl_data_serialization
 * @{
 */

/**
 * @file serializedObject.h
 * @brief A serialized object is an object which declares itself as streamable.
 *
 * @details The object can be saved into a stream and loaded later into the
 *    exact same state.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"

// Forward declaration
class basicInput;
class basicOutput;

/**
 * @brief A serialized-object should support the following interface.
 *
 * @details The object must have a default constructor (Class::Class()). If the
 *    object must have data it should have the 'isValid' function as well
 */
class cSerializedObject {
public:
    /**
     * @brief You may inherit from me
     */
    virtual ~cSerializedObject();

    /**
     * @brief Return true if the object is valid or false if the object is
     *    empty.
     *
     * @return True if the object is valid or false if the object is empty.
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Read from a stream the content of the object
     *
     * @param inputStream The stream to read the content from.
     */
    virtual void deserialize(basicInput& inputStream) = 0;

    /**
     * @brief Marshal the class objects into a stream
     *
     * @param outputStream The stream to write the content to.
     */
    virtual void serialize(basicOutput& outputStream) const = 0;
};


/** @} */
#endif // __TBA_STL_DATA_SERIALIZEDOBJECT_H
