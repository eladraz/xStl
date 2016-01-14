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
 * hash.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/os/osdef.h"
#include "xStl/data/array.h"
#include "xStl/data/list.h"
#include "xStl/data/hash.h"
#include "xStl/except/exception.h"

uint cHashFunction(const int & index, uint range)
{
    return index % range;
}

uint cHashFunction(const uint32& index, uint range)
{
    return index % range;
}

uint cHashFunction(const float& index, uint range)
{
    // Convert the float bits into number
    // And then preform the module process.
    return *((uint *)(&index)) % range;
}

uint cHashFunction(const char* index, uint range)
{
    uint byte_sum = 0;

    // Sum all the bytes in the string
    // and compute the hash.
    const char* ptr = index;
    while (*ptr != '\0')
    {
        byte_sum+= *(ptr++);
    }

    return cHashFunction(byte_sum, range);
}

uint cHashFunction(const cString& index, uint range)
{
    uint byte_sum = 0;
    for (int i = 0; i < (int)index.length(); i++)
    {
        byte_sum+= (uint)index[i];
    }

    return cHashFunction(byte_sum, range);
}

uint cHashFunction(const cBuffer& index, uint range)
{
    uint byte_sum = 0;
    for (uint i = 0; i < index.getSize(); i++)
    {
        byte_sum+= (uint)index[i];
    }

    return cHashFunction(byte_sum, range);
}

/*
 * When addressNumericValue != uint, implement this function
 *
uint cHashFunction(const addressNumericValue& index, uint range)
{
    return cHashFunction((uint)index, range);
}
*/

#ifndef XSTL_16BIT

uint cHashFunction(const uint64& index, uint range)
{
    return (uint)(index % range);
}

uint cHashFunction(const cOSDef::threadHandle& index, uint range)
{
    return cHashFunction(getNumeric((const void*)index), range);
}
#endif //XSTL_16BIT
