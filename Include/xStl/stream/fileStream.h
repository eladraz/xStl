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

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file fileStream.h
 * @brief Platform-independent file-access interface using basicIO streaming
 *   capabilities.
 *
 * @details This file is compiled to both Linux (LIBC) and all Windows
 *    platforms.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/endian.h"
#include "xStl/utils/algorithm.h"
#include "xStl/except/exception.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/file.h"

/**
 * @brief A basicIO stream over a file.
 *
 * @details BasicIO implementation of files. Single interface for reading and
 *    writing to
 *   files for all platforms.
 */
class cFileStream : public basicIO
{
public:
    /**
     * @brief Default constructor. Create a file object which doesn't connect to
     *    any
     *   physical file.
     */
    cFileStream();

    /**
     * @brief Files will be treated as the default endian in the system
     */
    defaultEndianImpl;

    /**
     * @brief Most common constructor. Create a stream according to an
     *    initialized file
     *   pointer.
     *
     * @details If the pointer is a NULL pointer, the function will act as the
     *    default
     *   constructor.
     *
     * @param filePtr The file to be used
     */
    cFileStream(const cFilePtr& filePtr);

    // Special function for the file interface
    /**
     * @brief Construct a file object and open it for the 'filename' file with
     *    flags
     *   attributes.
     *
     * @param filename A string which represents the file on the disk for the
     *    current operating system: DOS/WINDOWS: c:\\\\windows\\\\notepad.exe
     *    LINUX: /bin/ls
     * @param flags The open mode, creation mode and access mode for the file.
     */
    cFileStream(const cString& filename,
                const uint flags = cFile::READ);


    /**
     * @brief Open a file according to a file-handle.
     *
     * @details Gets the attribute for accessing the file-object (Read
     *    only/Write only assertion)
     *
     * @param handle The handle for the file, according to the platform.
     * @param flags The open mode, creation mode and access mode for the file.
     * @param closeHandle Is the 'handle' should be close at dtor.
     */
    void openHandle(const cOSDef::fileHandle& handle,
                    const uint flags = cFile::READ,
                    bool closeHandle = true);

    /**
     * @brief Open the file name 'filename' using 'flags' as the open mode.
     *
     * @details For the Windows environment, the CreateFile() syscall is used
     *    and the flags are translated to the corresponding elements. For the
     *    Linux environment, the open() syscall is used. The flags are
     *    translated to the corresponding elements.
     *
     * @param filename A string which represents the file on the disk for the
     *    current operating system: DOS/WINDOWS: c:\\\\windows\\\\notepad.exe
     *    LINUX: /bin/ls
     * @param flags The open mode, creation mode and access mode for the file.
     */
    void open(const cString& filename,
              const uint flags = cFile::READ);

    /**
     * @brief Close the file if opened.
     */
    void close();

    /**
     * @brief Return true if the file-handle is valid or false otherwise.
     *
     * @return True if the file-handle is valid or false otherwise.
     */
    bool isOpen() const;


    // Derived from the basicIO

    /**
     * @brief Read a buffer from the file.
     *
     * @details Implemented for both Windows and Linux platforms, using
     *    ReadFile() for Windows version and read() syscall for Linux platform.
     *
     * Throws cFileStreamException in case of error.
     *
     * @return The number of bytes read.
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Write a buffer to the file.
     *
     * @details Implemented for both Windows and Linux platforms, using
     *    WriteFile() for Windows version and write() syscall for the Linux
     *    version.
     *
     * Throws cFileStreamException in case of error.
     *
     * @return The number of bytes written.
     * @param buffer The data to write.
     * @param length The number of bytes to write.
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
     * @brief Move to a specific position, using a 64 bit distance.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    void seek64(const int64 distance, const basicInput::seekMethod method);

    /**
     * @brief Return the length of the file.
     *
     * @return The length of the file.
     */
    virtual uint length() const;

    /**
     * @brief Flush the information of the file.
     */
    virtual void flush();

    /**
     * @brief Returns the position of the write/read head
     *
     * @return The position of the write/read head.
     */
    virtual uint getPointer() const;
    /**
     * @brief Return the position of the read/write head as a 64 bit number.
     *
     * @return The position of the read/write head, in bytes from the beginning
     *    of the stream.
     */
    uint64 getPointer64() const;

    /**
     * @brief Return true if the file pointer reaches the end of the file.
     *
     * @return True if the file pointer reaches the end of the file.
     */
    virtual bool isEOS();

    /**
     * @brief Return true if the file is open for write
     *
     * @return True if the file is open for write.
     */
    bool isOpenForWrite();

    /**
     * @brief Return true if the file is open for read
     *
     * @return True if the file is open for read.
     */
    bool isOpenForRead();

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @details This value stands at 4kb which is the normal size of 2 clusters.
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const { return 4096; }

    /**
     * @brief Return the handle to the file-pointer
     *
     * @return The handle to the file-pointer.
     */
    const cFilePtr& getHandle() const;

protected:
    /**
     * @brief Init the seeking methods.
     *
     * @details All seeking methods are allowed
     */
    virtual void initSeek();

private:
    /**
     * @brief The open mode of the file
     */
    cFilePtr m_file;

    /**
     * @brief Deny copy-constructor and operator =
     */
    cFileStream(const cFileStream& other);
    cFileStream operator = (const cFileStream& other);
};


/** @} */
#endif // __TBA_STL_FILESTREAM_H
