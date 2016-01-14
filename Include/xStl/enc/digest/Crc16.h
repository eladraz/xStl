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

#ifndef __STL_ENCRYPTION_DIGEST_CRC16_H
#define __STL_ENCRYPTION_DIGEST_CRC16_H

#include "xStl/types.h"

/**
 * Used to calculate a CCITT 16bit CRC checksum over a data stream.
 * The current implementation favors memory over execution speed (it is
 * slower).
 *
 * The algorithm code itself was found on the Internet. Note that there are
 * several incompatible implementations available (they mainly differ on
 * initialization and treatment of empty messages). If you require absolute
 * conformance to the CCITT standard, you should probably double-check this
 * code. A detailed discussion on this matter is available at
 * http://www.joegeluso.com/software/articles/ccitt.htm.
 *
 * Author: Sergei Chernov
 * Author: Elad Raz <e@eladraz.com>
 */
class CRC16 {
public:
    /// Initializes the CRC calculator.
    CRC16();

    /// Reset the CRC calculator to its initial state.
    void reset();

    /**
     * Update the CRC with one more byte.
     * data        the input byte.
     */
    void update(byte data);

    /**
     * Updates the CRC with array of bytes.
     * data        array contents.
     * length    array length.
     */
    void update(const byte* data, uint length);

    /**
     * Returns the CRC value for the processed stream.
     */
    uint16 getValue();

private:

    /// Current value of the CRC.
    uint16 m_value;
};


#endif // __STL_ENCRYPTION_DIGEST_CRC16_H
