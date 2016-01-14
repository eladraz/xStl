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

#ifndef __TBA_XSTL_OS_SERIALPORT_H
#define __TBA_XSTL_OS_SERIALPORT_H

/*
 * serialPort.h
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

/*
 * class cSerialPort
 */
class cSerialPort {
public:
    /*
     * Baud rates at which the communication device operates
     */
    enum BaudRate {
        BaudRate_110    = 110,
        BaudRate_300    = 300,
        BaudRate_600    = 600,
        BaudRate_1200   = 1200,
        BaudRate_2400   = 2400,
        BaudRate_4800   = 4800,
        BaudRate_9600   = 9600,
        BaudRate_14400  = 14400,
        BaudRate_19200  = 19200,
        BaudRate_38400  = 38400,
        BaudRate_56000  = 56000,
        BaudRate_57600  = 57600,
        BaudRate_115200 = 115200,
        BaudRate_128000 = 128000,
        BaudRate_256000 = 256000
    };

    /*
     * The different method for parity bit
     */
    enum ParityBit {
        NoParity    = 0,
        OddParity   = 1,
        EvenParity  = 2,
        MarkParity  = 3,
        SpaceParity = 4
    };

    /*
     * The different method for stop bit
     */
    enum StopBit {
        SingleStopBit     = 0,
        OneAndHalfStopBit = 1,
        TwoStopBits       = 2,
    };
};

#endif // __TBA_TODO_SerialPort_H
