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

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file serialStream.h
 * @brief Handles the serial devices.
 *
 * @details Wrap a basic input-output stream over the serial interface
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/os/serialPort.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/fileStream.h"

/**
 * @brief The serial port has connection attributes:
 *
 * - Baud rate: the number of bits per second which the line supports.
 * - Transition: indicates when and how the parity bit is transmitted, how
 *   many stop bits should be used, and how many bits should
 *   be transmitted for each session.
 */
class cSerialStream : public basicIO {
public:
    /**
     * @brief Default constructor. Open a serial port.
     *
     * @param comport The port to be opened
     * @param baudRate The baud rate for the COM port.
     * @param bitSize The number of bits for a single byte (4-8)
     * @param parity The type for the parity check
     * @param stopBits Terminator between transmission.
     * @throws cException if the serial-stream cannot be instantiated
     */
    cSerialStream(uint comport,
                  cSerialPort::BaudRate baudRate = cSerialPort::BaudRate_115200,
                  uint bitSize                   = 8,
                  cSerialPort::ParityBit parity  = cSerialPort::NoParity,
                  cSerialPort::StopBit stopBits  = cSerialPort::SingleStopBit);

    /**
     * @brief Memory-files will be treated as the default endian in the system
     */
    defaultEndianImpl;

    // Derived from the basicIO

    /**
     * @brief Retrieve the position of the write/read head
     *
     * @return The position of the read/write head, in bytes from the beginning
     *    of the stream.
     */
    virtual uint getPointer() const;

    /**
     * @brief Return the length of the memory-file.
     *
     * @return The length of the memory-file.
     */
    virtual uint length() const;

    /**
     * @brief Return true if the file pointer reach the end of the file.
     *
     * @return True if the file pointer reach the end of the file.
     */
    virtual bool isEOS();

    /**
     * @brief Flush the information of the file.
     *
     * @details Shouldn't do anything.
     */
    virtual void flush();

    /**
     * @brief Read from the memory array into a buffer.
     *
     * @details If the reading reaches the end of the array then the function
     *    returns the number of bytes read until EOF is reached.
     *
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Write bytes to the stream.
     *
     * @details Change the bytes of the array. If the file position reached the
     *    end of the array, and m_fixedSize is false then the array size will
     *    increase.
     *
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual uint write(const void *buffer, const uint length);

    /**
     * @brief Move to a specific position in the file.
     *
     * @details Throw an exception if failed. See basicInput::seek for more
     *    information.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @details This value stands at 1kb which is the responsible value
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return 1024; }

    /**
     * @brief Return true if the line is ringing
     *
     * @return True if the line is ringing.
     * @throws cException if the serial-stream cannot acquire the UART status
     *    register
     */
    bool isRinging();

protected:
    /**
     * @brief Init the seeking methods.
     *
     * @details All seeking methods are allowed
     */
    virtual void initSeek();

private:
    /**
     * @brief The pipe handle
     */
    cFileStream m_serialHandle;
};


/** @} */
#endif // __TBA_STL_STREAM_SERIALSTREAM_H
