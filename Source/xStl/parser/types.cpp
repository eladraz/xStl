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

#include "xStl/xStlPrecompiled.h"
/*
 * types.cpp
 *
 * Implementation of the Types class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/data/string.h"
#include "xStl/data/char.h"
#include "xStl/parser/types.h"

const uint Types::TYPE_SIZE_VARIABLE = 0xFFFFFFFF;

const lpString Types::m_typeStrings[TYPES_COUNT] = {
    XSTL_STRING("uint8"),
    XSTL_STRING("uint16"),
    XSTL_STRING("uint32"),
    XSTL_STRING("int8"),
    XSTL_STRING("int16"),
    XSTL_STRING("int32"),
    XSTL_STRING("string"),
    XSTL_STRING("void")
};

const uint Types::m_typeSize[TYPES_COUNT] = {
    1,                          // uint8
    2,                          // uint16
    4,                          // uint32
    1,                          // int8
    2,                          // int16
    4,                          // int32
    (uint)TYPE_SIZE_VARIABLE,   // string
    0                           // void
};


Types::Type Types::getType(const cString& typeName)
{
    for (uint i = 0; i < TYPES_COUNT; i++)
    {
        if (typeName.compare(m_typeStrings[i]) == cString::EqualTo)
        {
            // Match
            return (Type)i;
        }
    }

    // Failed to find a type
    CHECK_FAIL();
}

bool Types::isUnsignedIntegerType(Types::Type type)
{
    return (type >= TYPE_UINT8) && (type <= TYPE_UINT32);
}

bool Types::isSignedIntegerType(Types::Type type)
{
    return (type >= TYPE_INT8) && (type <= TYPE_INT32);
}

bool Types::isIntegerType(Types::Type type)
{
    return (type >= TYPE_UINT8) && (type <= TYPE_INT32);
}

bool Types::isStringType(Types::Type type)
{
    return type == TYPE_STRING;
}
