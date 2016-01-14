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
 * variable.cpp
 *
 * Implementation of the BasicVariable and the VariableType classes.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/parser/types.h"
#include "xStl/parser/variable.h"

////////////////////////////////
// BasicVariable

const uint32 BasicVariable::EYE_CATCHER = 0xCDCDCDCD;

BasicVariable::BasicVariable() :
    m_type(Types::TYPE_UINT32)
{
    m_data.m_uint32 = EYE_CATCHER;
}

BasicVariable::BasicVariable(Types::Type type) :
    m_type(type)
{
    // Mark uninitiale BasicVariable
    m_data.m_uint32 = EYE_CATCHER;
}

Types::Type BasicVariable::getType() const
{
    return m_type;
}

bool BasicVariable::isInteger() const
{
    return Types::isIntegerType(m_type);
}

bool BasicVariable::isUnsignedInteger() const
{
    return Types::isUnsignedIntegerType(m_type);
}

bool BasicVariable::isSignedInteger() const
{
    return Types::isSignedIntegerType(m_type);
}

bool BasicVariable::isString() const
{
    return Types::isStringType(m_type);
}

const cString& BasicVariable::getString() const
{
    CHECK(isString());
    return m_string;
}

void BasicVariable::setString(const cString& other)
{
    CHECK(isString());
    m_string = other;
}

int BasicVariable::getSignedInteger() const
{
    CHECK(isSignedInteger());
    switch (m_type)
    {
    case Types::TYPE_INT8:  return m_data.m_int8;
    case Types::TYPE_INT16: return m_data.m_int16;
    case Types::TYPE_INT32: return m_data.m_int32;
    default:
        // Shouldn't be riched here!
        ASSERT(false);
        return EYE_CATCHER;
    }
}

void BasicVariable::setSignedInteger(int other)
{
    CHECK(isSignedInteger());

    switch (m_type)
    {
    case Types::TYPE_INT8:
        // Check cast
        CHECK((other >= -128) && (other <= 127));
        m_data.m_int8 = (int8)other;
        return;
    case Types::TYPE_INT16:
        CHECK((other >= -32768) && (other <= 32767));
        m_data.m_int16 = (int16)other;
        return;
    case Types::TYPE_INT32:
        CHECK((other >= (-2147483647 - 1)) && (other <= 2147483647));
        m_data.m_int32 = (int32)other;
        return;
    default:
        // Shouldn't be riched here!
        ASSERT(false);
    }
}

uint BasicVariable::getUnsignedInteger() const
{
    CHECK(isUnsignedInteger());
    switch (m_type)
    {
    case Types::TYPE_UINT8:  return m_data.m_uint8;
    case Types::TYPE_UINT16: return m_data.m_uint16;
    case Types::TYPE_UINT32: return m_data.m_uint32;
    default:
        // Shouldn't be riched here!
        ASSERT(false);
        return EYE_CATCHER;
    }
}

void BasicVariable::setUnsignedInteger(uint other)
{
    CHECK(isUnsignedInteger());

    switch (m_type)
    {
    case Types::TYPE_UINT8:
        // Check cast
        CHECK(other <= 0xFF);
        m_data.m_uint8 = (uint8)other;
        return;
    case Types::TYPE_UINT16:
        CHECK(other <= 0xFFFF);
        m_data.m_uint16 = (uint16)other;
        return;
    case Types::TYPE_UINT32:
        CHECK(other <= 0xFFFFFFFF);
        m_data.m_uint32 = (uint32)other;
        return;
    default:
        // Shouldn't be riched here!
        ASSERT(false);
    }
}

cString BasicVariable::makeString() const
{
    switch (m_type)
    {
    case Types::TYPE_INT8:
    case Types::TYPE_INT16:
    case Types::TYPE_INT32:
        return cString((int32)getSignedInteger(), 10);

    case Types::TYPE_UINT8:
        return cString("0x") + HEXBYTE(m_data.m_uint8);
    case Types::TYPE_UINT16:
        return cString("0x") + HEXWORD(m_data.m_uint16);
    case Types::TYPE_UINT32:
        return cString("0x") + HEXDWORD(m_data.m_uint32);

    case Types::TYPE_STRING:
        return m_string;

    case Types::TYPE_VOID:
        return Types::m_typeStrings[Types::TYPE_VOID];
    default:
        // Error!
        CHECK_FAIL();
    }
}

////////////////////////////////
// VariableType

VariableType::VariableType(const cString& name, const BasicVariable& data) :
    m_name(name),
    m_data(data)
{
}

const cString& VariableType::getName() const
{
    return m_name;
}

const BasicVariable& VariableType::getData() const
{
    return m_data;
}

BasicVariable& VariableType::getData()
{
    return m_data;
}
