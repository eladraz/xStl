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

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file serialPort.h
 *
 * @author Elad Raz
 */
#include "xStl/types.h"

/**
 * @brief Definitions of the serial port communication parameters.
 */
class cSerialPort {
public:
    /**
     * @brief Baud rates at which the communication device operates
     */
    enum BaudRate {
        BaudRate_110    = 110,      ///< 110 baud.
        BaudRate_300    = 300,      ///< 300 baud.
        BaudRate_600    = 600,      ///< 600 baud.
        BaudRate_1200   = 1200,     ///< 1200 baud.
        BaudRate_2400   = 2400,     ///< 2400 baud.
        BaudRate_4800   = 4800,     ///< 4800 baud.
        BaudRate_9600   = 9600,     ///< 9600 baud.
        BaudRate_14400  = 14400,    ///< 14400 baud.
        BaudRate_19200  = 19200,    ///< 19200 baud.
        BaudRate_38400  = 38400,    ///< 38400 baud.
        BaudRate_56000  = 56000,    ///< 56000 baud.
        BaudRate_57600  = 57600,    ///< 57600 baud.
        BaudRate_115200 = 115200,   ///< 115200 baud.
        BaudRate_128000 = 128000,   ///< 128000 baud.
        BaudRate_256000 = 256000    ///< 256000 baud.
    };

    /**
     * @brief The different methods for the parity bit
     */
    enum ParityBit {
        NoParity    = 0,    ///< No parity bit.
        OddParity   = 1,    ///< Odd parity.
        EvenParity  = 2,    ///< Even parity.
        MarkParity  = 3,    ///< The parity bit is always 1 (mark).
        SpaceParity = 4     ///< The parity bit is always 0 (space).
    };

    /**
     * @brief The different methods for the stop bit
     */
    enum StopBit {
        SingleStopBit     = 0,  ///< One stop bit.
        OneAndHalfStopBit = 1,  ///< One and a half stop bits.
        TwoStopBits       = 2,  ///< Two stop bits.
    };
};


/** @} */
#endif // __TBA_TODO_SerialPort_H
