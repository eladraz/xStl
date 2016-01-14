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

#ifndef __TBA_STL_ALIGNMENT_H
#define __TBA_STL_ALIGNMENT_H

/*
 * alignment.h
 *
 * Contain function which align data-structs into memory-size boundaries
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

/*
 * Usage:
 *
 *    alignUpToDword(0xFF0) -> 0xFF0
 *    alignUpToDword(0xFF1) -> 0xFF4
 *    alignUpToDword(0xFF2) -> 0xFF4
 *    alignUpToDword(0xFF3) -> 0xFF4
 */
class Alignment {
public:
    /*
     * Align into 4 bytes boundaries
     */
    #define alignUpToDword(number) alignUp((number), 3)
    #define alignDownToDword(number) alignDown((number), 3)

    /*
     * Align into 8 bytes boundaries
     */
    #define alignUpToQword(number) alignUp((number), 7)
    #define alignDownToQword(number) alignDown((number), 7)

    /*
     * Align a number up into 'base2NumberMinus1'
     */
    static uint alignUp(uint number, uint base2NumberMinus1);

    /*
     * Align a number down into 'base2NumberMinus1'
     */
    static uint alignDown(uint number, uint base2NumberMinus1);

    /*
     * Read 16 bit word by 32bit bus transaction
     *
     * ptr     - The pointer to read from
     * busSize - The bus number of bytes size.
     *           Right now this value is fixed to the current machine size.
     *
     */
    static uint16 readAlignedWord(const uint16* ptr /*, uint busSize = 4*/);
};

#endif // __TBA_STL_ALIGNMENT_H
