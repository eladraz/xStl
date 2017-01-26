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

/*
 * file.h
 *
 * Contains file's functions API.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/string.h"
#include "xStl/data/smartptr.h"
#include "xStl/stream/basicIO.h"
#include "xStl/os/osdef.h"

/*
 * The cFile is an API which communicate with files.
 *
 * 2008: New 64 bit seek file operation
 */
class cFile {
public:
    /*
    * Defines the access mode for the file (READ,WRITE,READWRITE),
    * Defines the creation mode (disposition) for the file (CREATE),
    * Defines the sharing mode for the file (SHARE_READ, SHARE_WRITE)
    */
    enum fileMode {
        // Access modes
        READ        =  1, // O_RDONLY
        WRITE       =  2, // O_WRONLY
        READWRITE   =  4, // O_RDWR
        // Creation modes
        CREATE      =  8, // O_CREAT | O_TRUNC
        // Sharing modes
        SHARE_READ  = 16,
        SHARE_WRITE = 32
    };

    /*
     * Construct a file object and open it for the 'filename' file with flags
     * attributes.
     *
     * filename - A string which represent the file over the disk for the current
     *            operation-system:
     *              DOS/WINDOWS: c:\\windows\\notepad.exe
     *              LINUX: /bin/ls
     *            Most often this string will be as a result from calling the
     *            cFilename::getFullPath()
     * flags - The open mode, creation mode and access mode for the file.
     *
     * Throw FAILED exception if the file couldn't be open.
     */
    cFile(const cString& filename,
          const uint flags = cFile::READ);

    /*
     * Open a file according to a file-handle. Gets the attribute for accessing
     * the file-object (Read only/Write only assertion)
     *
     * handle - The handle for the file, according to the platform.
     * flags - The open mode, creation mode and access mode for the file.
     * closeHandle - Is the 'handle' should be close at dtor.
     *
     * Throw FAILED exception if the file couldn't be open.
     */
    cFile(cOSDef::fileHandle handle,
          const uint flags = cFile::READ,
          bool closeHandle = true);

    /*
     * Destructor. Free all resources.
     */
    virtual ~cFile();

    /*
     * Return true if 'filename' exist. False otherwise.
     *
     * filename - The full-path file to test.
     *            NOTE: Some operating-systems will allow relative path also but
     *                  this API doesn't support it
     *
     * The function shall implemented by best result from operating system,
     * without actually open a file (So opened file will also existed) or
     * changing it's modification time.
     */
    static bool isFileExist(const cString& filename);

    /*
     * Delete a file
     *
     * filename - The full-path file to delete.
     *            NOTE: Some operating-systems will allow relative path also but
     *                  this API doesn't support it
     *
     * Return true if the file deleted, false otherwise
     */
    static bool deleteFile(const cString& filename);

    /*
     * Copy-constructor.
     */
    cFile(const cFile& other);

    /*
     * Copy operator.
     */
    cFile& operator = (const cFile& other);

    /*
     * Return an handle to the file.
     */
    const cOSDef::fileHandle& getHandle() const;

    // Driven from the basicIO
    /*
     * Read a buffer from the file.
     *
     * Return the number of bytes read.
     * Throws cFileException in case of error.
     */
    virtual uint read(void *buffer, const uint length);

    /*
     * Write a buffer to the file.
     *
     * Return the number of bytes written.
     * Throws cFileException in case of error.
     */
    uint write(const void *buffer, const uint length);

    /*
     * Move to a specific position in the file. Throw exception if failed.
     * See basicInput::seek for more information.
     */
    void seek(const int distance, const basicInput::seekMethod method);
    void seek64(const int64 distance, const basicInput::seekMethod method);

    /*
     * Return the length of the file.
     */
    uint length() const;

    /*
     * Flush the information of the file.
     */
    void flush();

    /*
     * Returns the position of the write/read head
     */
    uint getPointer() const;
    uint64 getPointer64() const;

    /*
     * Return true if the file is open for write
     */
    bool isOpenForWrite();

    /*
     * Return true if the file is open for read
     */
    bool isOpenForRead();

    /*
     * Return true if the file is opened
     */
    bool isOpen() const;

private:
    /*
     * Private implementation of the constructor.
     */
    void openHandle(const cOSDef::fileHandle& handle,
                    const uint flags = cFile::READ,
                    bool closeHandle = true);

    // The file-handle
    cOSDef::fileHandle m_handle;
    // The open mode of the file
    bool m_isReadable;
    bool m_isWriteable;
    // Should close the handle at dtor
    bool m_closeHandle;
};

/// Pointer to file, used as a reference-countable object
typedef cSmartPtr<cFile> cFilePtr;

#endif // __TBA_XSTL_OS_FILE_H
