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

#ifndef __TBA_STL_FILESTREAM_H
#define __TBA_STL_FILESTREAM_H

/*
 * fileStream.h
 *
 * Platfrom independance file-access interface using basicIO streaming
 * capabilities.
 *
 * This file is compiled to both Linux (LIBC) and all Windows platforms.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/endian.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/exception.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/file.h"

/*
 * cFileStream class
 *
 * BasicIO implementation of files. Single interface for reading and writing to
 * files for all platforms.
 */
class cFileStream : public basicIO
{
public:
    /*
     * Default constructor. Create a file object which doesn't connect to any
     * physical file.
     */
    cFileStream();

    /*
     * Files will be treated as the default endian in the system
     */
    defaultEndianImpl;

    /*
     * Most common constructor. Create a stream according to initialize file
     * pointer.
     *
     * filePtr - The file to be used
     *
     * If the pointer is a NULL pointer, the function will act as the default
     * constructor.
     */
    cFileStream(const cFilePtr& filePtr);

    // Special function for the file interface
    /*
     * Construct a file object and open it for the 'filename' file with flags
     * attributes.
     *
     * filename - A string which represent the file over the disk for the current
     *            operation-system:
     *              DOS/WINDOWS: c:\\windows\\notepad.exe
     *              LINUX: /bin/ls
     * flags - The open mode, creation mode and access mode for the file.
     */
    cFileStream(const cString& filename,
                const uint flags = cFile::READ);


    /*
     * Open a file according to a file-handle. Gets the attribute for accessing
     * the file-object (Read only/Write only assertion)
     *
     * handle - The handle for the file, according to the platform.
     * flags - The open mode, creation mode and access mode for the file.
     * closeHandle - Is the 'handle' should be close at dtor.
     */
    void openHandle(const cOSDef::fileHandle& handle,
                    const uint flags = cFile::READ,
                    bool closeHandle = true);

    /*
     * Open the file name 'filename' using 'flags' as the open mode.
     * For windows enviorment, CreateFile() syscall is in used the flags are
     * translating to the coresponed elements.
     * For linux envirment, open() syscall is in used. The flags are translating
     * to the coresponed elements.
     *
     * filename - A string which represent the file over the disk for the current
     *            operation-system:
     *              DOS/WINDOWS: c:\\windows\\notepad.exe
     *              LINUX: /bin/ls
     * flags - The open mode, creation mode and access mode for the file.
     */
    void open(const cString& filename,
              const uint flags = cFile::READ);

    /*
     * Close the file if opened.
     */
    void close();

    /*
     * Return true if the file-handle is valid or false otherwise.
     */
    bool isOpen() const;


    // Driven from the basicIO

    /*
     * Read a buffer from the file. Implement for both windows and linux platform,
     * using ReadFile() for Windows version and read() syscall for Linux platform.
     *
     * Return the number of bytes read.
     * Throws cFileStreamException in case of error.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Write a buffer to the file. Implement for both windows and linux platform,
     * using WriteFile() for Windows version and write() syscall for linux version.
     *
     * Return the number of bytes written.
     * Throws cFileStreamException in case of error.
     */
    virtual uint write(const void *buffer, const uint length);

    /*
     * Move to a specific position in the file. Throw exception if faild.
     * See basicInput::seek for more information.
     */
    virtual void seek(const int distance, const basicInput::seekMethod method);
    void seek64(const int64 distance, const basicInput::seekMethod method);

    /*
     * Return the length of the file.
     */
    virtual uint length() const;

    /*
     * Flush the information of the file.
     */
    virtual void flush();

    /*
     * Returns the position of the write/read head
     */
    virtual uint getPointer() const;
    uint64 getPointer64() const;

    /*
     * Return true if the file pointer reach the end of the file.
     */
    virtual bool isEOS();

    /*
     * Return true if the file is open for write
     */
    bool isOpenForWrite();

    /*
     * Return true if the file is open for read
     */
    bool isOpenForRead();

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     *
     * This value stand on 4kb which is normal 2 cluster size.
     */
    virtual uint getPipeReadBestRequest() const { return 4096; }

    /*
     * Return the handle to the file-pointer
     */
    const cFilePtr& getHandle() const;

protected:
    // Init the seeking methods.
    // All seeking methods are allowed
    virtual void initSeek();

private:
    // The open mode of the file
    cFilePtr m_file;

    // Deny copy-constructor and operator =
    cFileStream(const cFileStream& other);
    cFileStream operator = (const cFileStream& other);
};

#endif // __TBA_STL_FILESTREAM_H
