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

#ifndef __TBA_STL_STREAM_PIPESTREAM_H
#define __TBA_STL_STREAM_PIPESTREAM_H

/*
 * pipeStream.h
 *
 * A named pipe implementation.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/osdef.h"
#include "xStl/data/string.h"
#include "xStl/stream/fileStream.h"
#include "xStl/stream/basicIO.h"

/*
 * A pipe has 2 ends, one called server and the other called client. Each side
 * can write/read from the pipe. When the client write, the server can read the
 * information.
 *
 * NOTE: Some pipes are INBOUND or OUTBOUND pipes, which means that only one
 *       side can access the pipe.
 *
 * NOTE: The unnamed piped are ignored here, you have better alternative using
 *       xStl stream classes.
 */
class cPipeStream : public basicIO {
public:
    /*
     * Default constructor. Act as a pipe client. Connect to a named pipe in the
     * system.
     */
    cPipeStream(const cString& pipeName);

    /*
     * Destroy the pipe
     */
    virtual ~cPipeStream();

    /*
     * Server-side constructor. Open the pipe as a server. The pipe is generated
     * as a duplex pipe.
     *
     * Wait and return until a client connected to the port.
     */
    static BasicIOPtr listenOnPipe(const cString& pipeName);

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
     * This value stand on 4kb which is the pageSize in most platform.
     */
    virtual uint getPipeReadBestRequest() const { return 4096; }

protected:
    // Init the seeking methods.
    // All seeking methods are allowed
    virtual void initSeek();

private:
    /*
     * Private server constructor. Used when the client opens a server port.
     * See listenOnPipe.
     */
    cPipeStream(const cOSDef::fileHandle& pipeServerHandle);

    // The pipe handle
    cFileStream m_pipeHandle;
    // Set to true if the pipe is the server side and must be destruct in a
    // special way
    bool m_shouldDestroyServerPipe;
};

#endif // __TBA_STL_STREAM_PIPESTREAM_H
