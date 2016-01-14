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

#ifndef __TBA_STL_STREAM_SERIALSTREAM_H
#define __TBA_STL_STREAM_SERIALSTREAM_H

/*
 * serialStream.h
 *
 * Handles the serial devices. Wrap a basic input-output stream over the serial
 * interface
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/serialPort.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/fileStream.h"

/*
 * The serial port have a connection attribute:
 *     - Buadrate       The number of bits per second which the line supported
 *     - Transition     Indicate when and how the parity bit is transmits, how
 *                      many stop bit should be used, and how many bits should
 *                      be transmitted for each session.
 */
class cSerialStream : public basicIO {
public:
    /*
     * Default constructor. Open a serial port.
     *
     * comport  - The port to be opened
     * buadRate - The buadrate for com port.
     * bitSize  - The number of bits for a single byte (4-8)
     * parity   - The type for the parity check
     * stopBits - Terminator between transmission.
     *
     * Throw exception if the serial-stream cannot be instances
     */
    cSerialStream(uint comport,
                  cSerialPort::BaudRate baudRate = cSerialPort::BaudRate_115200,
                  uint bitSize                   = 8,
                  cSerialPort::ParityBit parity  = cSerialPort::NoParity,
                  cSerialPort::StopBit stopBits  = cSerialPort::SingleStopBit);

    /*
     * Memory-files will be treated as the default endian in the system
     */
    defaultEndianImpl;

    // Driven from the basicIO

    /*
     * Retrive the position of the write/read head
     */
    virtual uint getPointer() const;

    /*
     * Return the length of the memory-file.
     */
    virtual uint length() const;

    /*
     * Return true if the file pointer reach the end of the file.
     */
    virtual bool isEOS();

    /*
     * Flush the information of the file. Shouldn't do anything.
     */
    virtual void flush();

    /*
     * Read from the memory array into a buffer. If the reading reach the end
     * of the array then the function return the number of bytes readed until
     * EOF reached.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Write bytes to the stream.
     * Change the bytes of the array. If the file postion reached the end of the
     * array, and m_fixedSize is false then the array size will increase.
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Move to a specific position in the file. Throw exception if failed.
     * See basicInput::seek for more information.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     *
     * This value stand on 1kb which is the responsible value
     */
    virtual uint getPipeReadBestRequest() const { return 1024; }

    /*
     * Return true if the line is ringing
     *
     * Throw exception if the serial-stream cannot acquire the UART status
     * register
    */
    bool isRinging();

protected:
    // Init the seeking methods.
    // All seeking methods are allowed
    virtual void initSeek();

private:
    // The pipe handle
    cFileStream m_serialHandle;
};

#endif // __TBA_STL_STREAM_SERIALSTREAM_H
