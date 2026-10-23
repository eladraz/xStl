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

/**
 * @addtogroup xstl_data_containers
 * @{
 */

/**
 * @file alignment.h
 * @brief Contains functions which align data structures to memory-size
 *    boundaries
 *
 * @author Elad Raz
 */
#include "xStl/types.h"

/**
 * @brief Static helpers that align numbers to power-of-two boundaries and read
 *    words from unaligned addresses.
 *
 * Usage of the alignment macros:
 * @code
 * alignUpToDword(0xFF0) -> 0xFF0
 * alignUpToDword(0xFF1) -> 0xFF4
 * alignUpToDword(0xFF2) -> 0xFF4
 * alignUpToDword(0xFF3) -> 0xFF4
 * @endcode
 */
class Alignment {
public:
    /**
     * @brief Align a number up to a 4-byte (dword) boundary.
     *
     * @param number The number to align.
     */
    #define alignUpToDword(number) alignUp((number), 3)
    /**
     * @brief Align a number down to a 4-byte (dword) boundary.
     *
     * @param number The number to align.
     */
    #define alignDownToDword(number) alignDown((number), 3)

    /**
     * @brief Align a number up to an 8-byte (qword) boundary.
     *
     * @param number The number to align.
     */
    #define alignUpToQword(number) alignUp((number), 7)
    /**
     * @brief Align a number down to an 8-byte (qword) boundary.
     *
     * @param number The number to align.
     */
    #define alignDownToQword(number) alignDown((number), 7)

    /**
     * @brief Align a number up to 'base2NumberMinus1'
     *
     * @param number The number to align.
     * @param base2NumberMinus1 The alignment boundary minus one, which must be
     *    a power of two minus one (3 for 4-byte alignment, 7 for 8-byte
     *    alignment).
     * @return The smallest multiple of the boundary that is not below @a
     *    number.
     */
    static addressNumericValue alignUp(addressNumericValue number,
                                       addressNumericValue base2NumberMinus1);

    /**
     * @brief Align a number down to 'base2NumberMinus1'
     *
     * @param number The number to align.
     * @param base2NumberMinus1 The alignment boundary minus one, which must be
     *    a power of two minus one (3 for 4-byte alignment, 7 for 8-byte
     *    alignment).
     * @return The largest multiple of the boundary that is not above @a number.
     */
    static addressNumericValue alignDown(addressNumericValue number,
                                         addressNumericValue base2NumberMinus1);

    /**
     * @brief Read a 16 bit word by a 32 bit bus transaction.
     *
     * The aligned 32 bit value that contains the word is read in one
     * transaction and the requested word is extracted from it, so @a ptr does
     * not have to be aligned. The bus size is currently fixed to 4 bytes (the
     * size of the machine word).
     *
     * @param ptr The pointer to read from.
     * @return The 16 bit word at @a ptr.
     * @note A word that starts at the last byte of the 32 bit value is not read
     *    completely, because only that one byte is inside the value that is
     *    read.
     */
    static uint16 readAlignedWord(const uint16* ptr /*, uint busSize = 4*/);
};


/** @} */
#endif // __TBA_STL_ALIGNMENT_H
