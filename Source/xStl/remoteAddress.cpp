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
 * remoteAddress.h
 *
 * Implementation file
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/trace.h"
#include "xStl/remoteAddress.h"

remoteAddressNumericValue::remoteAddressNumericValue() :
    m_type(DEFAULT_REMOTE_ADDRESS_ENCODING)
{
    m_union.m_64bitAddress = 0;
}

remoteAddressNumericValue::remoteAddressNumericValue(
                        const maxRemoteUint& address,
                        RemoteAddressEncodingTypes type) :
    m_type(type)
{
    m_union.m_64bitAddress = address;
}

maxRemoteUint remoteAddressNumericValue::getClipedAddress() const
{
    maxRemoteUint ret = m_union.m_64bitAddress;
    switch (m_type)
    {
    case REMOTE_ADDRESS_16BIT: ret&= 0xFFFF; break;
    case REMOTE_ADDRESS_32BIT: ret&= 0xFFFFFFFF; break;
    case REMOTE_ADDRESS_64BIT: break;

    default: CHECK_FAIL();
    }
    return ret;
}

bool remoteAddressNumericValue::isInsideRange(
                    const remoteAddressNumericValue& start,
                    const remoteAddressNumericValue& end) const
{
    return ((*this >= start) && (*this <= end));
}

bool remoteAddressNumericValue::operator == (
    const remoteAddressNumericValue& other) const
{
    return getClipedAddress() == other.getClipedAddress();
}

bool remoteAddressNumericValue::operator < (
    const remoteAddressNumericValue& other) const
{
    return getClipedAddress() < other.getClipedAddress();
}

addressNumericValue remoteAddressNumericValue::getAddressValue() const
{
    // The casting is used to prevent compiler errors.
    // NOTE: There isn't a good way to implement only the targer code
    switch (DEFAULT_REMOTE_ADDRESS_ENCODING)
    {
    case REMOTE_ADDRESS_16BIT:
        CHECK(m_union.m_64bitAddress <= 0xFFFFL);
        return ((addressNumericValue)(m_union.m_16bitAddress));

    case REMOTE_ADDRESS_32BIT:
        CHECK(m_union.m_64bitAddress <= 0xFFFFFFFFL);
        return (addressNumericValue)(m_union.m_32bitAddress);

    case REMOTE_ADDRESS_64BIT:
        // Just return the number
        return (addressNumericValue)(m_union.m_64bitAddress);

    default:
        CHECK_FAIL();
    }
}
