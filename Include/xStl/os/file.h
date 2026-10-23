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

#ifndef __TBA_XSTL_OS_FILE_H
#define __TBA_XSTL_OS_FILE_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file file.h
 * @brief Contains the file functions API.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/smartptr.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/osdef.h"

/**
 * @brief The cFile class is an API which communicates with files.
 *
 * @details 2008: New 64 bit seek file operation
 */
class cFile {
public:
    /**
     * @brief Defines the access mode for the file (READ,WRITE,READWRITE),
     *    Defines the
     *   creation mode (disposition) for the file (CREATE), Defines the sharing
     *   mode for the file (SHARE_READ, SHARE_WRITE)
     */
    enum fileMode {
        /**
         * @brief Access modes
         */
        READ        =  1, ///< O_RDONLY
        WRITE       =  2, ///< O_WRONLY
        READWRITE   =  4, ///< O_RDWR
        /**
         * @brief Creation modes
         */
        CREATE      =  8, ///< O_CREAT | O_TRUNC
        /**
         * @brief Sharing modes
         */
        SHARE_READ  = 16,
        /**
         * @brief Allow other handles to open the file for writing.
         */
        SHARE_WRITE = 32
    };

    /**
     * @brief Construct a file object and open it for the 'filename' file with
     *    flags
     *   attributes.
     *
     * @param filename A string which represents the file on the disk for the
     *    current operating system: DOS/WINDOWS: c:\\\\windows\\\\notepad.exe
     *    LINUX: /bin/ls Most often this string will be the result of calling
     *    the cFilename::getFullPath()
     * @param flags The open mode, creation mode and access mode for the file.
     * @throws cException (FAILED) if the file couldn't be opened.
     */
    cFile(const cString& filename,
          const uint flags = cFile::READ);

    /**
     * @brief Open a file according to a file-handle.
     *
     * @details Gets the attributes for accessing the file-object (Read
     *    only/Write only assertion)
     *
     * @param handle The handle for the file, according to the platform.
     * @param flags The open mode, creation mode and access mode for the file.
     * @param closeHandle Whether the 'handle' should be closed at dtor.
     * @throws cException (FAILED) if the file couldn't be opened.
     */
    cFile(cOSDef::fileHandle handle,
          const uint flags = cFile::READ,
          bool closeHandle = true);

    /**
     * @brief Destructor. Free all resources.
     */
    virtual ~cFile();

    /**
     * @brief Return true if 'filename' exists.
     *
     * @details False otherwise.
     *
     * The function shall be implemented to give the best result for the
     * operating system,
     * without actually opening a file (so an opened file will also exist) or
     * changing its modification time.
     *
     * @param filename The full-path file to test. NOTE: Some operating-systems
     *    will allow relative path also but this API doesn't support it
     * @return True if 'filename' exists.
     */
    static bool isFileExist(const cString& filename);

    /**
     * @brief Delete a file
     *
     * @param filename The full-path file to delete. NOTE: Some
     *    operating-systems will allow relative path also but this API doesn't
     *    support it
     * @return True if the file was deleted, false otherwise.
     */
    static bool deleteFile(const cString& filename);

    /**
     * @brief Copy-constructor.
     *
     * @param other The file object to copy.
     */
    cFile(const cFile& other);

    /**
     * @brief Copy operator.
     *
     * @param other The file object to copy from.
     * @return A reference to this object.
     */
    cFile& operator = (const cFile& other);

    /**
     * @brief Return a handle to the file.
     *
     * @return A handle to the file.
     */
    const cOSDef::fileHandle& getHandle() const;

    // Derived from the basicIO
    /**
     * @brief Read a buffer from the file.
     *
     * @details Throws cFileException in case of error.
     *
     * @return The number of bytes read.
     * @param buffer The buffer which receives the data.
     * @param length The number of bytes to read.
     */
    virtual uint read(void *buffer, const uint length);

    /**
     * @brief Write a buffer to the file.
     *
     * @details Throws cFileException in case of error.
     *
     * @return The number of bytes written.
     * @param buffer The data to write.
     * @param length The number of bytes to write.
     */
    uint write(const void *buffer, const uint length);

    /**
     * @brief Move to a specific position in the file.
     *
     * @details Throw an exception if failed. See basicInput::seek for more
     *    information.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     */
    void seek(const int distance, const basicInput::seekMethod method);
    /**
     * @brief Move to a specific position in the file, using a 64 bit distance.
     *
     * @param distance The distance to move, relative to @a method.
     * @param method The reference position of the movement.
     * @throws cException if the seek failed.
     */
    void seek64(const int64 distance, const basicInput::seekMethod method);

    /**
     * @brief Return the length of the file.
     *
     * @return The length of the file.
     */
    uint length() const;

    /**
     * @brief Flush the information of the file.
     */
    void flush();

    /**
     * @brief Returns the position of the write/read head
     *
     * @return The position of the write/read head.
     */
    uint getPointer() const;
    /**
     * @brief Return the position of the write/read head as a 64 bit number.
     *
     * @return The position of the write/read head.
     */
    uint64 getPointer64() const;

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
     * @brief Return true if the file is opened
     *
     * @return True if the file is opened.
     */
    bool isOpen() const;

private:
    /**
     * @brief Private implementation of the constructor.
     */
    void openHandle(const cOSDef::fileHandle& handle,
                    const uint flags = cFile::READ,
                    bool closeHandle = true);

    /**
     * @brief The file-handle
     */
    cOSDef::fileHandle m_handle;
    /**
     * @brief The open mode of the file
     */
    bool m_isReadable;
    bool m_isWriteable;
    /**
     * @brief Should close the handle at dtor
     */
    bool m_closeHandle;
};

/// Pointer to file, used as a reference-countable object
typedef cSmartPtr<cFile> cFilePtr;


/** @} */
#endif // __TBA_XSTL_OS_FILE_H
