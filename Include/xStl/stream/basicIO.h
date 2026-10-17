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

#ifndef __TBA_STL_STREAMS_BASIC_IO_H
#define __TBA_STL_STREAMS_BASIC_IO_H

/*
 * basicIO.h
 *
 * Declare an abstract class for all the input/output classes, for example files,
 * sockets, memory files, screen devices, keyboard devices and so on...
 *
 * This file defines two important classes: basicInput and basicOutput. All
 * streaming and filtering in the system should use these interfaces.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/endian.h"
#include "xStl/data/smartptr.h"

// Forward declaration for xStl::data serialized objects
class cString;
// Forward declaration for xStl::data::cArray serialized objects
template <class T>
class cArray;
// Forward declaration for xStl::data::cSArray serialized objects
template <class T>
class cSArray;
typedef cSArray<uint8> cBuffer;

/*
 * basicInput
 *
 * Define the interface for input streams. Input stream can support random access
 * medium (file-memory) or read-once stream (network/devices).  The main function
 * for the class is the 'read' function which the inheriting class should implement.
 * The read function reads bytes from the stream and can read part of the reque-
 * sted  bytes. This class  implements the 'pipeRead'  function which  repeats the read
 * request and should return all the requested bytes from the stream.
 * The class also supports deserialization for all standard types in the compilers.
 * Deserialization is made using the 'cEndian' class. There are default implementations
 * for both little endian and big endian: basicLEInput and basicBEInput.
 *
 * For streams which support some kind of seeking, retrieving the stream length or
 * getting the position inside the stream, there are three interface functions:
 * getPointer(), length() and seek(). Streams which do/don't support these
 * functions should update the public 'can*' variables. The interface implementat-
 * ion of function which isn't supported by a stream is to throw an exception.
 */
class basicInput : public cEndian
{
public:
    // IO seek method
    enum seekMethod {
        // Seek from the beginning of the stream
        IO_SEEK_SET = 0,
        // Seek relative to the current position
        IO_SEEK_CUR = 1,
        // Seek from the end of the stream
        IO_SEEK_END = 2
    };

    /*
     * Virtual destructor.
     */
    virtual ~basicInput();


    // Normal functions

    /*
     * Read raw block of data from the stream. The function shouldn't be blocking
     * for infinite time. The function should return the number of bytes read.
     * The number of bytes read can be different from the requested bytes to read.
     * NOTE:
     *  This function can return 0 bytes read and it will be considered normal
     *  behavior. Only an exception indicates an error. (Zero can indicate timeout)
     *
     * buffer - Buffer to be filled with the raw data
     * length - The size for the read request.
     *
     * Return the number of bytes read.
     * Throws an exception in case of failure.
     */
    virtual uint read(void *buffer, const uint length) = 0;

    /*
     * Return true in case the input stream reached the end and there aren't any
     * data left in the queue. False means that there are more bytes waiting to
     * be read.
     *
     * NOTE: EOS stands for End-Of-Stream
     */
    virtual bool isEOS() = 0;

    /*
     * If supported, return a number of bytes from the beginning of the stream.
     *
     * NOTE: This function is implemented only if the 'canGetPointer' variable is set
     *       to true. Otherwise this function raises an exception.
     * Throws 'UNSUPPORT' exception.
     */
    virtual uint getPointer() const = 0;

    /*
     * Seek inside the stream forward/backward. The function has four modes of
     * operations:
     *  - Seeking forward (just reading bytes)
     *  - Seeking backward (Minus relative the current position)
     *  - Number of bytes from the beginning of the stream
     *  - Number of bytes back relative to the end of the stream
     *
     * distance - Number of bytes to move relative to the 'method' variable.
     * method - The relative operation to move inside the stream.
     *
     * Throws 'UNSUPPORT'/error exception depending on the stream
     */
    virtual void seek(const int distance, const seekMethod method) = 0;

    /*
     * If supported, return the total number of bytes of stream (from the
     * beginning to the end).
     *
     * NOTE: This function is implemented only if the 'canGetPointer' variable is set
     *       to true. Otherwise this function raises an exception.
     * Throws 'UNSUPPORT/error exception depending on the stream
     */
    virtual uint length() const = 0;

    /*
     * Returns how many bytes should be read for best performance in the
     * pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const = 0;


    // Implemented functions.
    // The following functions are implemented and can be used in all

    /*
     * Try to  read 'length' bytes from the stream into the buffer. The reading
     * should succeed. In case EOS is reached the function returns normally with the
     * number of bytes read until the EOS.
     *
     * buffer - The buffer to store the read bytes in. If the buffer is
     *          a cBuffer& then the read bytes will be APPENDED to the end
     *          of the buffer.
     * length - The amount of bytes for the reading.
     *
     * Throws error exception if the read failed
     * Return the number of read bytes.
     */
    uint pipeRead(void *buffer, const uint length);
    uint pipeRead(cBuffer& buffer, const uint length);

    /*
     * Read 'length' bytes and verify all bytes actually read.
     * This function is a shortcut to the following functions:
     *    CHECK(pipeRead(buffer, length) == length);
     *
     * See 'pipeRead'.
     */
    void pipeReadNotEos(void *buffer, const uint length);
    void pipeReadNotEos(cBuffer& buffer, const uint length);

    /*
     * The default value for the number of character to be read from a stream.
     * For different compilation modules this value can be changed by a define
     */
    #ifndef MAX_CHAR
    enum { MAX_CHAR = 0xFFFFFFFFL };
    #endif

    /*
     * Global function which reads a string in different storage modes from a
     * stream.
     *
     * Return a string in the requested format. The function can limit the number
     * of bytes of the string. All the functions throw an exception in case there are
     * more  characters than 'numberOfCharacter'  variables or when a read error has
     * occurred.
     *
     * ASCII formats
     * =============
     * readDosString - Read until '$' character is found.
     * readNullString - Read until \000 character is found.
     * readPascal8String - Read length uint8 and then read the string.
     * readPascal8String - Read length uint16 and then read the string.
     *
     * Unicode formats
     * ===============
     * readNullString - Read until \000 character is found.
     */
    cString readAsciiDosString(const uint numberOfCharacter = MAX_CHAR);
    cString readAsciiNullString(const uint numberOfCharacter = MAX_CHAR);
    cString readAsciiPascal8String(const uint numberOfCharacter = MAX_CHAR);
    cString readAsciiPascal16String(const uint numberOfCharacter = MAX_CHAR);
    cString readUnicodeNullString(const uint numberOfCharacter = MAX_CHAR);
    cString readUnicodePascal8String(const uint numberOfCharacter = MAX_CHAR);
    cString readUnicodePascal16String(const uint numberOfCharacter = MAX_CHAR);

    /*
     * Read a string from the stream where the string is encoded in a fixed
     * size of characters. The format can be either unicode or ascii strings.
     *
     * numberOfCharacter - The fixed size of the string available space
     * unicodeSize       - Number of bytes per unicode string (1 for ASCII, 2 for windows wchar_t, 4 for linux wchar_t)
     *
     * Return the read string.
     * Throw any exception that can occur as a 'write' result.
     * See basicInput::readFixedSizeString
     */
    cString readFixedSizeString(uint numberOfCharacters,
                                int unicodeSize =
                                    #ifdef XSTL_UNICODE
                                        sizeof(unichar)
                                    #else
                                        false
                                    #endif
                                );

    /*
     * Deserialization functions. Deserialize the different types using a call
     * to the 'pipeRead()' function and cEndian interface function for parsing
     * the data as needed.
     */
    void streamReadInt8(int8& byte);
    void streamReadInt16(int16& word);
    void streamReadInt32(int32& dword);
    void streamReadUint8(uint8& byte);
    void streamReadUint16(uint16& word);
    void streamReadUint32(uint32& dword);

    #ifndef XSTL_16BIT
    void streamReadInt64(int64& dword);
    void streamReadUint64(uint64& qword);

    /*
     * Change the remote address decoding type. Used for optimization.
     *
     * shouldEncodeType - The default value is true. This means that the remote-
     *                    address values are encoded as 'type' & 'address'.
     *                    When this value is false, the optimization mode is
     *                    taking place.
     * enforceType      - If and only if the 'shouldEncodeType' is false, this
     *                    argument is the casting format of the encoding
     *
     * NOTE: The default encoding is to read the addressing mode and then to
     *       read a 16/32/64-bit address according to the 'type'
     */
    void changeDecodedAddressType(bool shouldEncodeType,
                                  RemoteAddressEncodingTypes enforceType);

    /*
     * Reads from the stream an encoded address and returns its value.
     * NOTE: This function will not compile for 16 bit application
     *
     * See changeDecodedAddressType() for more information
     * See RemoteAddressEncodingTypes
     */
    remoteAddressNumericValue streamReadRemoteAddress();
    #endif // XSTL_16BIT

    /*
     * Try to read until the end of the stream and append the data into 'rawData'.
     * The function works better on streams with 'canLength' and 'canGetPointer'
     * capabilities.
     */
    void readAllStream(cBuffer& rawData);


    // IO parameters
    // Each class should assign some value to this argument.
    bool canLength;
    bool canSeekFromBegin;
    bool canSeekForward;
    bool canSeekBackward;
    bool canSeekFromEnd;
    bool canGetPointer;

protected:
    /*
     * Checks whether a class can do seeking, according to the seek parameters.
     */
    bool __canSeek(const int distance, const basicInput::seekMethod method);

    /*
     * The declaration of this function was to make sure that the programmer
     * who implements an input stream won't forget to call its initSeek
     * function at the constructor (YES, HE SHOULD DO IT!) and fill all the
     * 'can*' variables.
     */
    virtual void initSeek() = 0;

    /*
     * Default constructor. Initialize the remote-address encoding type.
     *
     * NOTE: The constructor cannot invoke a call to 'initSeek' since the
     *       virtual-table hasn't been constructed yet
     */
    basicInput();

private:
    /*
     * Private function. Read a string until some NULL termination character.
     * The function has two forms for both ASCII and unicode strings
     *
     * ret - Will be filled with the returned string.
     * terminate - The ASCII character which terminates the reading.
     * numberOfCharacter - The maximum number of characters to attempt in the
     *                     reading. Prevents freeze.
     */
    void readAsciiString(cString& ret,
                         uint8 terminate,
                         const uint numberOfCharacter);
    void readUnicodeString(cString& ret,
                           unichar terminate,
                           const uint numberOfCharacter);

    #ifndef XSTL_16BIT
    // The remote-address encoding protocol
    RemoteAddressEncodingTypes m_addressDecodingType;
    // Determine if the remote-address type should be decoded
    bool m_shouldEncodeType;
    #endif // XSTL_16BIT
};

// Reference countable object for input streams
typedef cSmartPtr<basicInput> BasicInputPtr;


/*
 * basicOutput
 *
 * Output stream functions.
 */
class basicOutput : public cEndian
{
public:
    /*
     * Virtual destructor.
     */
    virtual ~basicOutput();

    /*
     * Writes a raw block of data to the stream.  The function shouldn't be
     * blocking for infinite time. The function should return the number of
     * bytes written. The number of bytes written can be different from the
     *  requested bytes to write.
     *
     * NOTE:
     *  This function can return 0 bytes written and it will be considered normal
     *  behavior.  Only an exception indicates an error. (Zero can indicate timeout)
     *
     * buffer - Buffer to be written
     * length - The size for the write request
     *
     * Return the number of bytes written
     * Throws an exception in case of failure
     */
    virtual uint write(const void* buffer, const uint length) = 0;

    /*
     * Flush all buffer (lazy write buffers) to the stream. This function should
     * be called before close() or before opening (in shared streams) the same
     * stream for reading.
     *
     * Throws write-error exception.
     */
    virtual void flush() = 0;

    /*
     * Try to write 'length' bytes from the buffer into the stream. The writing
     * should succeed.
     *
     * buffer - The data to be written
     * length - The amount of bytes for the write operation
     *
     * Throws error exception if the write failed or not all bytes written.
     */
    virtual void pipeWrite(const void* buffer, const uint length);
    virtual void pipeWrite(const cBuffer& buffer, const uint length);

    /*
     * Serialization functions. Serialize the different types using a call to the
     * 'pipeWrite()' function and cEndian interface  function for writing in the
     * right alignment.
     */
    void streamWriteInt8(const int8 byte);
    void streamWriteInt16(const int16 word);
    void streamWriteInt32(const int32 dword);
    void streamWriteUint8(const uint8 byte);
    void streamWriteUint16(const uint16 word);
    void streamWriteUint32(const uint32 dword);

    #ifndef XSTL_16BIT
    void streamWriteInt64(const int64 qword);
    void streamWriteUint64(const uint64 qword);

    /*
     * Change the remote address encoding type. Used for optimization.
     *
     * shouldEncodeType - The default value is true. This means that the remote-
     *                    address values are encoded as 'type' & 'address'.
     *                    When this value is false, the optimization mode is
     *                    taking place.
     * enforceType      - If and only if the 'shouldEncodeType' is false, this
     *                    argument is the casting format of the encoding
     *
     * NOTE: The default encoding is to read the addressing mode and then to
     *       read a 16/32/64-bit address according to the 'type'
     */
    void changeEncodedAddressType(bool shouldEncodeType,
                                  RemoteAddressEncodingTypes enforceType);
    /*
     * Encode to the stream an address value
     *
     * See changeEncodedAddressType() for more information
     * See RemoteAddressEncodingTypes
     */
    void streamWriteRemoteAddress(const remoteAddressNumericValue& address);
    #endif // XSTL_16BIT

    /*
     * Write unicode/ascii strings to the stream.
     *
     * string - The text to be sent.
     * shouldPutZeroTerminator - Whether the terminating NULL character should be
     *                           written
     *
     * Throw any exception that can occur as a 'write' result.
     */
    void writeAsciiNullString(const cString& string,
                              bool shouldPutZeroTerminator = true);
    void writeUnicodeNullString(const cString& string,
                                bool shouldPutZeroTerminator = true);

    /*
     * Write a string to the stream where the written string must have a fixed
     * size of characters. The format can be either unicode or ascii strings.
     *
     * string            - The string to be written, If the length of this
     *                     string is bigger than 'numberOfCharacter', the string
     *                     will be trimmed.
     * numberOfCharacter - The fixed size of the string available space
     * unicodeSize       - Number of bytes per unicode string (1 for ASCII, 2 for windows wchar_t, 4 for linux wchar_t)
     *
     * Throw any exception that can occur as a 'write' result.
     * See basicInput::readFixedSizeString
     */
    void writeFixedSizeString(const cString& string,
                              uint numberOfCharacters,
                              int unicodeSize =
                                #ifdef XSTL_UNICODE
                                    sizeof(unichar)
                                #else
                                    1
                                #endif
                              );

protected:
    /*
     * Default constructor. Initialize the remote-address encoding type.
     *
     * NOTE: The constructor cannot invoke a call to 'initSeek' since the
     *       virtual-table hasn;t been constructed yet
     */
    basicOutput();

private:
    #ifndef XSTL_16BIT
    // The remote-address encoding protocol
    RemoteAddressEncodingTypes m_addressEncodingType;
    // Determine if the remote-address type should be decoded
    bool m_shouldEncodeType;
    #endif
};

// Reference countable object for input streams
typedef cSmartPtr<basicOutput> BasicOutputPtr;


/*
 * basicIO
 *
 * Both input and output streams interface.
 */
class basicIO : public basicInput,
                public basicOutput
{
public:
    /*
     * Perform a soft stream copy. The remaining source stream information will
     * be copied to the destination stream.
     *
     * The method of doing so is as follows: As long as there is information at
     * the source, the method reads it (using the default reading buffer) and channels
     * it to the destination.
     */
    static void copyStream(basicOutput& destination,
                           basicInput& source);
};

// Reference countable object for input streams
typedef cSmartPtr<basicIO> BasicIOPtr;


#endif //__TBA_STL_STREAMS_BASIC_IO_H
