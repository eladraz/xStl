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

/**
 * @addtogroup xstl_stream
 * @{
 */

/**
 * @file basicIO.h
 * @brief Declare an abstract class for all the input/output classes, for
 *    example
 *   files, sockets, memory files, screen devices, keyboard devices and so
 *   on...
 *
 * @details This file defines two important classes: basicInput and basicOutput.
 *    All
 *   streaming and filtering in the system should use these interfaces.
 *
 * @author Elad Raz
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
/// A buffer of bytes.
typedef cSArray<uint8> cBuffer;

/**
 * @brief Define the interface for input streams.
 *
 * @details Input stream can support random access medium (file-memory) or
 *    read-once stream (network/devices). The main function for the class is the
 *    'read' function which the inheriting class should implement. The read
 *    function reads bytes from the stream and can read part of the reque- sted
 *    bytes. This class implements the 'pipeRead' function which repeats the
 *    read request and should return all the requested bytes from the stream.
 *    The class also supports deserialization for all standard types in the
 *    compilers. Deserialization is made using the 'cEndian' class. There are
 *    default implementations for both little endian and big endian:
 *    basicLEInput and basicBEInput.
 *
 * For streams which support some kind of seeking, retrieving the stream length
 * or
 * getting the position inside the stream, there are three interface functions:
 * getPointer(), length() and seek(). Streams which do/don't support these
 * functions should update the public 'can*' variables. The interface
 * implementat-
 * ion of function which isn't supported by a stream is to throw an exception.
 */
class basicInput : public cEndian
{
public:
    /**
     * @brief IO seek method
     */
    enum seekMethod {
        /**
         * @brief Seek from the beginning of the stream
         */
        IO_SEEK_SET = 0,
        /**
         * @brief Seek relative to the current position
         */
        IO_SEEK_CUR = 1,
        /**
         * @brief Seek from the end of the stream
         */
        IO_SEEK_END = 2
    };

    /**
     * @brief Virtual destructor.
     */
    virtual ~basicInput();


    // Normal functions

    /**
     * @brief Read raw block of data from the stream.
     *
     * @details The function shouldn't be blocking for infinite time. The
     *    function should return the number of bytes read. The number of bytes
     *    read can be different from the requested bytes to read.
     *
     * @param buffer Buffer to be filled with the raw data
     * @param length The size for the read request.
     * @return The number of bytes read.
     * @throws cException in case of failure.
     * @note This function can return 0 bytes read and it will be considered
     *    normal behavior. Only an exception indicates an error. (Zero can
     *    indicate timeout)
     */
    virtual uint read(void *buffer, const uint length) = 0;

    /**
     * @brief Return true in case the input stream reached the end and there
     *    aren't any
     *   data left in the queue.
     *
     * @details False means that there are more bytes waiting to be read.
     *
     * @return True in case the input stream reached the end and there aren't
     *    any data left in the queue.
     * @note EOS stands for End-Of-Stream
     */
    virtual bool isEOS() = 0;

    /**
     * @brief If supported, return a number of bytes from the beginning of the
     *    stream.
     *
     * @throws cException (UNSUPPORT) .
     * @note This function is implemented only if the 'canGetPointer' variable
     *    is set to true. Otherwise this function raises an exception.
     * @return The position of the read/write head, in bytes from the beginning
     *    of the stream.
     */
    virtual uint getPointer() const = 0;

    /**
     * @brief Seek inside the stream forward/backward.
     *
     * @details The function has four modes of operations: - Seeking forward
     *    (just reading bytes) - Seeking backward (Minus relative the current
     *    position) - Number of bytes from the beginning of the stream - Number
     *    of bytes back relative to the end of the stream
     *
     * Throws 'UNSUPPORT'/error exception depending on the stream
     *
     * @param distance Number of bytes to move relative to the 'method'
     *    variable.
     * @param method The relative operation to move inside the stream.
     */
    virtual void seek(const int distance, const seekMethod method) = 0;

    /**
     * @brief If supported, return the total number of bytes of stream (from the
     *   beginning to the end).
     *
     * @details Throws 'UNSUPPORT/error exception depending on the stream
     *
     * @note This function is implemented only if the 'canGetPointer' variable
     *    is set to true. Otherwise this function raises an exception.
     * @return The total length of the stream in bytes.
     */
    virtual uint length() const = 0;

    /**
     * @brief Returns how many bytes should be read for best performance in the
     *   pipeRead() function in a single call to the read() function.
     *
     * @return How many bytes should be read for best performance in the
     *    pipeRead() function in a single call to the read() function.
     */
    virtual uint getPipeReadBestRequest() const = 0;


    // Implemented functions.
    // The following functions are implemented and can be used in all

    /**
     * @brief Try to read 'length' bytes from the stream into the buffer.
     *
     * @details The reading should succeed. In case EOS is reached the function
     *    returns normally with the number of bytes read until the EOS.
     *
     * @param buffer The buffer to store the read bytes in. If the buffer is a
     *    cBuffer\& then the read bytes will be APPENDED to the end of the
     *    buffer.
     * @param length The amount of bytes for the reading.
     * @return The number of read bytes.
     * @throws cException if the read failed
     */
    uint pipeRead(void *buffer, const uint length);
    /**
     * @brief Try to read @a length bytes from the stream and append them to a
     *    buffer.
     *
     * @param buffer The buffer to which the read bytes are appended.
     * @param length The amount of bytes for the reading.
     * @return The number of read bytes.
     * @throws cException if the read failed.
     */
    uint pipeRead(cBuffer& buffer, const uint length);

    /**
     * @brief Read 'length' bytes and verify all bytes actually read.
     *
     * @details This function is a shortcut to the following functions:
     *    CHECK(pipeRead(buffer, length) == length);
     *
     * See 'pipeRead'.
     *
     * @param buffer The buffer to store the read bytes in.
     * @param length The amount of bytes to read.
     */
    void pipeReadNotEos(void *buffer, const uint length);
    /**
     * @brief Read @a length bytes and append them to a buffer. Verify that all
     *    the bytes were actually read.
     *
     * @param buffer The buffer to which the read bytes are appended.
     * @param length The amount of bytes to read.
     */
    void pipeReadNotEos(cBuffer& buffer, const uint length);

    /*
     * The default value for the number of character to be read from a stream.
     * For different compilation modules this value can be changed by a define
     */
    #ifndef MAX_CHAR
    /**
     * @brief Constants for the string reading functions.
     */
    enum {
        /**
         * @brief The default maximum number of characters to read from a
         *   stream.
         */
        MAX_CHAR = 0xFFFFFFFFL
    };
    #endif

    /**
     * @brief Global function which reads a string in different storage modes
     *    from a
     *   stream.
     *
     * @details **ASCII formats**
     *   readDosString - Read until '$' character is found.
     *   readNullString - Read until \\000 character is found.
     *   readPascal8String - Read length uint8 and then read the string.
     *   readPascal8String - Read length uint16 and then read the string.
     *
     * **Unicode formats**
     * readNullString - Read until \\000 character is found.
     *
     * @return A string in the requested format. The function can limit the
     *    number of bytes of the string. All the functions throw an exception in
     *    case there are more characters than 'numberOfCharacter' variables or
     *    when a read error has occurred.
     * @param numberOfCharacter The maximum number of characters of the string.
     * @throws cException if the string is longer than @a numberOfCharacter or
     *    if a read error occurred.
     */
    cString readAsciiDosString(const uint numberOfCharacter = MAX_CHAR);
    /**
     * @brief Read ascii string terminated by a null character from the stream.
     *
     * @param numberOfCharacter The maximum number of characters of the string.
     * @return The string which was read.
     * @throws cException if the string is longer than @a numberOfCharacter or
     *    if a read error occurred.
     */
    cString readAsciiNullString(const uint numberOfCharacter = MAX_CHAR);
    /**
     * @brief Read ascii string prefixed by an 8 bit length from the stream.
     *
     * @param numberOfCharacter The maximum number of characters of the string.
     * @return The string which was read.
     * @throws cException if the string is longer than @a numberOfCharacter or
     *    if a read error occurred.
     */
    cString readAsciiPascal8String(const uint numberOfCharacter = MAX_CHAR);
    /**
     * @brief Read ascii string prefixed by a 16 bit length from the stream.
     *
     * @param numberOfCharacter The maximum number of characters of the string.
     * @return The string which was read.
     * @throws cException if the string is longer than @a numberOfCharacter or
     *    if a read error occurred.
     */
    cString readAsciiPascal16String(const uint numberOfCharacter = MAX_CHAR);
    /**
     * @brief Read unicode string terminated by a null character from the
     *    stream.
     *
     * @param numberOfCharacter The maximum number of characters of the string.
     * @return The string which was read.
     * @throws cException if the string is longer than @a numberOfCharacter or
     *    if a read error occurred.
     */
    cString readUnicodeNullString(const uint numberOfCharacter = MAX_CHAR);
    /**
     * @brief Read unicode string prefixed by an 8 bit length from the stream.
     *
     * @param numberOfCharacter The maximum number of characters of the string.
     * @return The string which was read.
     * @throws cException if the string is longer than @a numberOfCharacter or
     *    if a read error occurred.
     */
    cString readUnicodePascal8String(const uint numberOfCharacter = MAX_CHAR);
    /**
     * @brief Read unicode string prefixed by a 16 bit length from the stream.
     *
     * @param numberOfCharacter The maximum number of characters of the string.
     * @return The string which was read.
     * @throws cException if the string is longer than @a numberOfCharacter or
     *    if a read error occurred.
     */
    cString readUnicodePascal16String(const uint numberOfCharacter = MAX_CHAR);

    /**
     * @brief Read a string from the stream where the string is encoded in a
     *    fixed size
     *   of characters.
     *
     * @details The format can be either unicode or ascii strings.
     *
     * numberOfCharacter - The fixed size of the string available space
     * unicodeSize       - Number of bytes per unicode string (1 for ASCII, 2 for windows wchar_t, 4 for linux wchar_t)
     *
     * @throws cException (any) that can occur as a 'write' result.
     * @see basicInput::readFixedSizeString
     * @param numberOfCharacters The fixed size of the string available space.
     * @param unicodeSize The number of bytes per character (1 for ASCII, 2 for
     *    Windows wchar_t, 4 for Linux wchar_t).
     * @return The string which was read.
     */
    cString readFixedSizeString(uint numberOfCharacters,
                                int unicodeSize =
                                    #ifdef XSTL_UNICODE
                                        sizeof(unichar)
                                    #else
                                        false
                                    #endif
                                );

    /**
     * @brief Deserialization functions. Deserialize the different types using a
     *    call to
     *   the 'pipeRead()' function and cEndian interface function for parsing
     *   the
     *   data as needed.
     *
     * @param byte Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadInt8(int8& byte);
    /**
     * @brief Deserialize a 16 bit signed number from the stream.
     *
     * @param word Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadInt16(int16& word);
    /**
     * @brief Deserialize a 32 bit signed number from the stream.
     *
     * @param dword Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadInt32(int32& dword);
    /**
     * @brief Deserialize a 8 bit unsigned number from the stream.
     *
     * @param byte Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadUint8(uint8& byte);
    /**
     * @brief Deserialize a 16 bit unsigned number from the stream.
     *
     * @param word Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadUint16(uint16& word);
    /**
     * @brief Deserialize a 32 bit unsigned number from the stream.
     *
     * @param dword Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadUint32(uint32& dword);

    #ifndef XSTL_16BIT
    /**
     * @brief Deserialize a 64 bit signed number from the stream.
     *
     * @param dword Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadInt64(int64& dword);
    /**
     * @brief Deserialize a 64 bit unsigned number from the stream.
     *
     * @param qword Will be filled with the number which was read.
     * @throws cException if the stream could not provide the data.
     */
    void streamReadUint64(uint64& qword);

    /**
     * @brief Change the remote address decoding type.
     *
     * @details Used for optimization.
     *
     * @param shouldEncodeType The default value is true. This means that the
     *    remote- address values are encoded as 'type' \& 'address'. When this
     *    value is false, the optimization mode is taking place.
     * @param enforceType If and only if the 'shouldEncodeType' is false, this
     *    argument is the casting format of the encoding
     * @note The default encoding is to read the addressing mode and then to
     *    read a 16/32/64-bit address according to the 'type'
     */
    void changeDecodedAddressType(bool shouldEncodeType,
                                  RemoteAddressEncodingTypes enforceType);

    /**
     * @brief Reads from the stream an encoded address and returns its value.
     *
     * @details See changeDecodedAddressType() for more information
     *
     * @note This function will not compile for 16 bit application
     * @see RemoteAddressEncodingTypes
     * @return The address which was read.
     */
    remoteAddressNumericValue streamReadRemoteAddress();
    #endif // XSTL_16BIT

    /**
     * @brief Try to read until the end of the stream and append the data into
     *   'rawData'.
     *
     * @details The function works better on streams with 'canLength' and
     *    'canGetPointer' capabilities.
     *
     * @param rawData The buffer to which the data of the stream is appended.
     */
    void readAllStream(cBuffer& rawData);


    /**
     * @brief IO parameters Each class should assign some value to this
     *    argument.
     */
    /**
     * @brief True if the stream supports the length() function.
     */
    bool canLength;
    /**
     * @brief True if the stream can seek relative to the beginning of the
     *   stream.
     */
    bool canSeekFromBegin;
    /**
     * @brief True if the stream can seek forward relative to the current
     *   position.
     */
    bool canSeekForward;
    /**
     * @brief True if the stream can seek backward relative to the current
     *   position.
     */
    bool canSeekBackward;
    /**
     * @brief True if the stream can seek relative to the end of the stream.
     */
    bool canSeekFromEnd;
    /**
     * @brief True if the stream supports the getPointer() function.
     */
    bool canGetPointer;

protected:
    /**
     * @brief Checks whether a class can do seeking, according to the seek
     *    parameters.
     *
     * @param distance The distance of the seek operation.
     * @param method The reference position of the seek operation.
     * @return True if the stream is capable of the requested seek operation.
     */
    bool __canSeek(const int distance, const basicInput::seekMethod method);

    /**
     * @brief The declaration of this function was to make sure that the
     *    programmer who
     *   implements an input stream won't forget to call its initSeek function
     *   at
     *   the constructor (YES, HE SHOULD DO IT!) and fill all the 'can*'
     *   variables.
     */
    virtual void initSeek() = 0;

    /**
     * @brief Default constructor. Initialize the remote-address encoding type.
     *
     * @note The constructor cannot invoke a call to 'initSeek' since the
     *    virtual-table hasn't been constructed yet
     */
    basicInput();

private:
    /**
     * @brief Private function. Read a string until some NULL termination
     *    character.
     *
     * @details The function has two forms for both ASCII and unicode strings
     *
     * @param ret Will be filled with the returned string.
     * @param terminate The ASCII character which terminates the reading.
     * @param numberOfCharacter The maximum number of characters to attempt in
     *    the reading. Prevents freeze.
     */
    void readAsciiString(cString& ret,
                         uint8 terminate,
                         const uint numberOfCharacter);
    void readUnicodeString(cString& ret,
                           unichar terminate,
                           const uint numberOfCharacter);

    #ifndef XSTL_16BIT
    /**
     * @brief The remote-address encoding protocol
     */
    RemoteAddressEncodingTypes m_addressDecodingType;
    /**
     * @brief Determine if the remote-address type should be decoded
     */
    bool m_shouldEncodeType;
    #endif // XSTL_16BIT
};

/**
 * @brief Reference countable object for input streams
 */
typedef cSmartPtr<basicInput> BasicInputPtr;


/**
 * @brief Output stream functions.
 */
class basicOutput : public cEndian
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~basicOutput();

    /**
     * @brief Writes a raw block of data to the stream.
     *
     * @details The function shouldn't be blocking for infinite time. The
     *    function should return the number of bytes written. The number of
     *    bytes written can be different from the requested bytes to write.
     *
     * @param buffer Buffer to be written
     * @param length The size for the write request
     * @return The number of bytes written.
     * @throws cException in case of failure
     * @note This function can return 0 bytes written and it will be considered
     *    normal behavior. Only an exception indicates an error. (Zero can
     *    indicate timeout)
     */
    virtual uint write(const void* buffer, const uint length) = 0;

    /**
     * @brief Flush all buffer (lazy write buffers) to the stream.
     *
     * @details This function should be called before close() or before opening
     *    (in shared streams) the same stream for reading.
     *
     * Throws write-error exception.
     */
    virtual void flush() = 0;

    /**
     * @brief Try to write 'length' bytes from the buffer into the stream.
     *
     * @details The writing should succeed.
     *
     * @param buffer The data to be written
     * @param length The amount of bytes for the write operation
     * @throws cException if the write failed or not all bytes written.
     */
    virtual void pipeWrite(const void* buffer, const uint length);
    /**
     * @brief Write @a length bytes of a buffer to the stream.
     *
     * @param buffer The data to be written.
     * @param length The amount of bytes for the write operation.
     * @throws cException if the write failed or not all bytes written.
     */
    virtual void pipeWrite(const cBuffer& buffer, const uint length);

    /**
     * @brief Serialization functions. Serialize the different types using a
     *    call to the
     *   'pipeWrite()' function and cEndian interface function for writing in
     *   the
     *   right alignment.
     *
     * @param byte The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteInt8(const int8 byte);
    /**
     * @brief Serialize a 16 bit signed number to the stream.
     *
     * @param word The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteInt16(const int16 word);
    /**
     * @brief Serialize a 32 bit signed number to the stream.
     *
     * @param dword The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteInt32(const int32 dword);
    /**
     * @brief Serialize a 8 bit unsigned number to the stream.
     *
     * @param byte The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteUint8(const uint8 byte);
    /**
     * @brief Serialize a 16 bit unsigned number to the stream.
     *
     * @param word The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteUint16(const uint16 word);
    /**
     * @brief Serialize a 32 bit unsigned number to the stream.
     *
     * @param dword The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteUint32(const uint32 dword);

    #ifndef XSTL_16BIT
    /**
     * @brief Serialize a 64 bit signed number to the stream.
     *
     * @param qword The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteInt64(const int64 qword);
    /**
     * @brief Serialize a 64 bit unsigned number to the stream.
     *
     * @param qword The number to write.
     * @throws cException if the stream could not accept the data.
     */
    void streamWriteUint64(const uint64 qword);

    /**
     * @brief Change the remote address encoding type.
     *
     * @details Used for optimization.
     *
     * @param shouldEncodeType The default value is true. This means that the
     *    remote- address values are encoded as 'type' \& 'address'. When this
     *    value is false, the optimization mode is taking place.
     * @param enforceType If and only if the 'shouldEncodeType' is false, this
     *    argument is the casting format of the encoding
     * @note The default encoding is to read the addressing mode and then to
     *    read a 16/32/64-bit address according to the 'type'
     */
    void changeEncodedAddressType(bool shouldEncodeType,
                                  RemoteAddressEncodingTypes enforceType);
    /**
     * @brief Encode to the stream an address value
     *
     * @details See changeEncodedAddressType() for more information
     *
     * @see RemoteAddressEncodingTypes
     * @param address The address to write.
     */
    void streamWriteRemoteAddress(const remoteAddressNumericValue& address);
    #endif // XSTL_16BIT

    /**
     * @brief Write unicode/ascii strings to the stream.
     *
     * @param string The text to be sent.
     * @param shouldPutZeroTerminator Whether the terminating NULL character
     *    should be written
     * @throws cException (any) that can occur as a 'write' result.
     */
    void writeAsciiNullString(const cString& string,
                              bool shouldPutZeroTerminator = true);
    /**
     * @brief Write a unicode string to the stream.
     *
     * @param string The text to be sent.
     * @param shouldPutZeroTerminator Whether the terminating NULL character
     *    should be written.
     * @throws cException (any) that can occur as a write result.
     */
    void writeUnicodeNullString(const cString& string,
                                bool shouldPutZeroTerminator = true);

    /**
     * @brief Write a string to the stream where the written string must have a
     *    fixed
     *   size of characters.
     *
     * @details The format can be either unicode or ascii strings.
     *
     * If the length of the string is bigger than 'numberOfCharacters', the
     * string will be trimmed.
     *
     * @throws cException (any) that can occur as a 'write' result.
     * @see basicInput::readFixedSizeString
     * @param string The string to be written. If its length is bigger than @a
     *    numberOfCharacters, the string is trimmed.
     * @param numberOfCharacters The fixed size of the string available space.
     * @param unicodeSize The number of bytes per character (1 for ASCII, 2 for
     *    Windows wchar_t, 4 for Linux wchar_t).
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
    /**
     * @brief Default constructor. Initialize the remote-address encoding type.
     *
     * @note The constructor cannot invoke a call to 'initSeek' since the
     *    virtual-table hasn;t been constructed yet
     */
    basicOutput();

private:
    #ifndef XSTL_16BIT
    /**
     * @brief The remote-address encoding protocol
     */
    RemoteAddressEncodingTypes m_addressEncodingType;
    /**
     * @brief Determine if the remote-address type should be decoded
     */
    bool m_shouldEncodeType;
    #endif
};

/**
 * @brief Reference countable object for input streams
 */
typedef cSmartPtr<basicOutput> BasicOutputPtr;


/**
 * @brief Both input and output streams interface.
 */
class basicIO : public basicInput,
                public basicOutput
{
public:
    /**
     * @brief Perform a soft stream copy.
     *
     * @details The remaining source stream information will be copied to the
     *    destination stream.
     *
     * The method of doing so is as follows: As long as there is information at
     * the source, the method reads it (using the default reading buffer) and
     * channels
     * it to the destination.
     *
     * @param destination The stream to copy to.
     * @param source The stream to copy from.
     */
    static void copyStream(basicOutput& destination,
                           basicInput& source);
};

/**
 * @brief Reference countable object for input streams
 */
typedef cSmartPtr<basicIO> BasicIOPtr;



/** @} */
#endif //__TBA_STL_STREAMS_BASIC_IO_H
