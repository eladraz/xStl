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

#ifndef __TBA_STL_ENDIAN_H
#define __TBA_STL_ENDIAN_H

/*
 * endian.h
 *
 * Interface for endian mechine. Modules which want to work with streams
 * should be provide this interface where users can select the needed
 * implementation for the endian (Big/little endian)
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"


/*
 * class cEndian
 *
 * The interface for endian. Streaming uint16 and uint32 back and forward into
 * a buffer.
 * Classes which supports different marchling code (little-endian and big-endian)
 * can declare them self as inherit from the cEndian class. In the implementation
 * call to the marcling codes as interface.
 * You can see two kind of example for the implementation in the basicInput/basic
 * Output classes and in the encryption digital hash modules (SHA-1 and MD5)
 */
class cEndian {
public:
    /* Virtual destructor */
    virtual ~cEndian() {};

    #ifndef XSTL_16BIT
    /*
     * Deserialize uint64 from a stream.
     * NOTE: 16 bit compilers cannot work with 64 bit numbers
     *
     * buffer - Pointer to at least 8 bytes memory
     */
    virtual uint64 readUint64(const uint8* buffer) const = 0;

    /*
     * Serialize 64 unsigned bit into a buffer
     * NOTE: 16 bit compilers cannot work with 64 bit numbers
     *
     * buffer - Pointer to at least 4 bytes memory
     * number - The number to serialize.
     */
    virtual void writeUint64(uint8* buffer, uint64 number) const = 0;
    #endif // XSTL_16BIT


    /*
     * Deserialize uint32 from a stream
     *
     * buffer - Pointer to at least 4 bytes memory
     */
    virtual uint32 readUint32(const uint8* buffer) const = 0;

    /*
     * Serialize 32 unsigned bit into a buffer
     *
     * buffer - Pointer to at least 4 bytes memory
     * number - The number to serialize.
     */
    virtual void writeUint32(uint8* buffer, uint32 number) const = 0;

    /*
     * Deserialize uint16 from a stream
     *
     * buffer - Pointer to at least 2 bytes memory
     */
    virtual uint16 readUint16(const uint8* buffer) const = 0;

    /*
     * Serialize 16 unsigned bit into a buffer
     *
     * buffer - Pointer to at least 2 bytes memory
     * number - The number to serialize.
     */
    virtual void writeUint16(uint8* buffer, uint16 number) const = 0;

    /*
     * Return true if the system is little endian, false for big-endian.
     * Little-endian means that the least significant word is first.
     */
    virtual bool isLittleEndian() const = 0;

    /*
     * Read little or big endian value from the buffer
     */
    static uint16 readUint16(const uint8* buffer, bool isLittleEndian);
    static uint32 readUint32(const uint8* buffer, bool isLittleEndian);
#ifndef XSTL_16BIT
    static uint64 readUint64(const uint8* buffer, bool isLittleEndian);
#endif // XSTL_16BIT

    /*
     * Write little or big endian value to the buffer
     */
    static void writeUint16(uint8* buffer, uint16 number, bool isLittleEndian);
    static void writeUint32(uint8* buffer, uint32 number, bool isLittleEndian);
#ifndef XSTL_16BIT
    static void writeUint64(uint8* buffer, uint64 number, bool isLittleEndian);
#endif // XSTL_16BIT

};


/*
 * Big-endian implementation.
 *
 * Most significant word first (Motorolla)
 * 0x1234 will be store as 12 34
 */
class cBigEndian
{
public:
    /*
     * Function to implement.
     * See cEndian for explanation about these functions.
     */
    #ifndef XSTL_16BIT
    // See comment
    static uint64 readUint64(const uint8* buffer);
    static void writeUint64(uint8* buffer, uint64 number);
    #endif

    static uint32 readUint32(const uint8* buffer);
    static uint16 readUint16(const uint8* buffer);
    static void writeUint32(uint8* buffer, uint32 number);
    static void writeUint16(uint8* buffer, uint16 number);
    static bool isLittleEndian() { return false; };
};


/*
 * Definition which implement the big endian interface
 */
#ifdef XSTL_16BIT
// 16 bit compilers
#define bigEndianImpl \
    virtual uint32 readUint32(const uint8* buffer) const { return cBigEndian::readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return cBigEndian::readUint16(buffer); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { cBigEndian::writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { cBigEndian::writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return cBigEndian::isLittleEndian(); }
#else
// Non 16 bit compilers
#define bigEndianImpl \
    virtual uint64 readUint64(const uint8* buffer) const { return cBigEndian::readUint64(buffer); }; \
    virtual uint32 readUint32(const uint8* buffer) const { return cBigEndian::readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return cBigEndian::readUint16(buffer); }; \
    virtual void writeUint64(uint8* buffer, uint64 number) const { cBigEndian::writeUint64(buffer,number); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { cBigEndian::writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { cBigEndian::writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return cBigEndian::isLittleEndian(); }
#endif


/*
 * Little-endian implementation.
 *
 * Least significant word first (Intel)
 * 0x1234 will be store as 34 12
 */
class cLittleEndian {
public:
    /*
     * Function to implement
     * See cEndian for explanation about these functions.
     */
    #ifndef XSTL_16BIT
    // See comment
    static uint64 readUint64(const uint8* buffer);
    static void writeUint64(uint8* buffer, uint64 number);
    #endif // XSTL_16BIT

    static uint32 readUint32(const uint8* buffer);
    static uint16 readUint16(const uint8* buffer);
    static void writeUint32(uint8* buffer, uint32 number);
    static void writeUint16(uint8* buffer, uint16 number);
    static bool isLittleEndian() { return true; };
};


/*
 * Definition which implement the little endian interface
 */
#ifdef XSTL_16BIT
// 16 bit compilers
#define littleEndianImpl \
    virtual uint32 readUint32(const uint8* buffer) const { return cLittleEndian::readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return cLittleEndian::readUint16(buffer); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { cLittleEndian::writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { cLittleEndian::writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return cLittleEndian::isLittleEndian(); }
#else
// Non 16 bit compilers
#define littleEndianImpl \
        virtual uint64 readUint64(const uint8* buffer) const { return cLittleEndian::readUint64(buffer); }; \
        virtual uint32 readUint32(const uint8* buffer) const { return cLittleEndian::readUint32(buffer); }; \
        virtual uint16 readUint16(const uint8* buffer) const { return cLittleEndian::readUint16(buffer); }; \
        virtual void writeUint64(uint8* buffer, uint64 number) const { cLittleEndian::writeUint64(buffer,number); }; \
        virtual void writeUint32(uint8* buffer, uint32 number) const { cLittleEndian::writeUint32(buffer,number); }; \
        virtual void writeUint16(uint8* buffer, uint16 number) const { cLittleEndian::writeUint16(buffer,number); }; \
        virtual bool isLittleEndian() const { return cLittleEndian::isLittleEndian(); }
#endif

#endif //__TBA_STL_ENDIAN_H
