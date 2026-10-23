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

/**
 * @addtogroup xstl_data_serialization
 * @{
 */

/**
 * @file endian.h
 * @brief Interface for endian machines.
 *
 * @details Modules which want to work with streams should provide this
 *    interface where users can select the needed implementation for the endian
 *    (Big/little endian)
 *
 * @author Elad Raz
 */
#include "xStl/types.h"


/**
 * @brief The interface for endian.
 *
 * @details Streaming uint16 and uint32 back and forth into a buffer. Classes
 *    which support different marshaling code (little-endian and big-endian) can
 *    declare themselves as inheriting from the cEndian class. In the
 *    implementation call the marshaling code through the interface. You can see
 *    two kinds of examples of the implementation in the basicInput/basic Output
 *    classes and in the encryption digital hash modules (SHA-1 and MD5)
 */
class cEndian {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~cEndian() {};

    #ifndef XSTL_16BIT
    /**
     * @brief Deserialize uint64 from a stream.
     *
     * @param buffer Pointer to at least 8 bytes memory
     * @note 16 bit compilers cannot work with 64 bit numbers
     * @return The deserialized number.
     */
    virtual uint64 readUint64(const uint8* buffer) const = 0;

    /**
     * @brief Serialize a 64 bit unsigned number into a buffer
     *
     * @param buffer Pointer to at least 4 bytes memory
     * @param number The number to serialize.
     * @note 16 bit compilers cannot work with 64 bit numbers
     */
    virtual void writeUint64(uint8* buffer, uint64 number) const = 0;
    #endif // XSTL_16BIT


    /**
     * @brief Deserialize uint32 from a stream
     *
     * @param buffer Pointer to at least 4 bytes memory
     * @return The deserialized number.
     */
    virtual uint32 readUint32(const uint8* buffer) const = 0;

    /**
     * @brief Serialize a 32 bit unsigned number into a buffer
     *
     * @param buffer Pointer to at least 4 bytes memory
     * @param number The number to serialize.
     */
    virtual void writeUint32(uint8* buffer, uint32 number) const = 0;

    /**
     * @brief Deserialize uint16 from a stream
     *
     * @param buffer Pointer to at least 2 bytes memory
     * @return The deserialized number.
     */
    virtual uint16 readUint16(const uint8* buffer) const = 0;

    /**
     * @brief Serialize a 16 bit unsigned number into a buffer
     *
     * @param buffer Pointer to at least 2 bytes memory
     * @param number The number to serialize.
     */
    virtual void writeUint16(uint8* buffer, uint16 number) const = 0;

    /**
     * @brief Return true if the system is little endian, false for big-endian.
     *
     * @details Little-endian means that the least significant word is first.
     *
     * @return True if the system is little endian, false for big-endian.
     */
    virtual bool isLittleEndian() const = 0;

    /**
     * @brief Read a 16 bit number from the buffer in the requested byte order.
     *
     * @param buffer Pointer to at least 2 bytes memory.
     * @param isLittleEndian True to read the number as little-endian, false for
     *    big-endian.
     * @return The deserialized number.
     */
    static uint16 readUint16(const uint8* buffer, bool isLittleEndian);
    /**
     * @brief Read a 32 bit number from the buffer in the requested byte order.
     *
     * @param buffer Pointer to at least 4 bytes memory.
     * @param isLittleEndian True to read the number as little-endian, false for
     *    big-endian.
     * @return The deserialized number.
     */
    static uint32 readUint32(const uint8* buffer, bool isLittleEndian);
#ifndef XSTL_16BIT
    /**
     * @brief Read a 64 bit number from the buffer in the requested byte order.
     *
     * @param buffer Pointer to at least 8 bytes memory.
     * @param isLittleEndian True to read the number as little-endian, false for
     *    big-endian.
     * @return The deserialized number.
     */
    static uint64 readUint64(const uint8* buffer, bool isLittleEndian);
#endif // XSTL_16BIT

    /**
     * @brief Write a 16 bit number to the buffer in the requested byte order.
     *
     * @param buffer Pointer to at least 2 bytes memory.
     * @param number The number to serialize.
     * @param isLittleEndian True to write the number as little-endian, false
     *    for big-endian.
     */
    static void writeUint16(uint8* buffer, uint16 number, bool isLittleEndian);
    /**
     * @brief Write a 32 bit number to the buffer in the requested byte order.
     *
     * @param buffer Pointer to at least 4 bytes memory.
     * @param number The number to serialize.
     * @param isLittleEndian True to write the number as little-endian, false
     *    for big-endian.
     */
    static void writeUint32(uint8* buffer, uint32 number, bool isLittleEndian);
#ifndef XSTL_16BIT
    /**
     * @brief Write a 64 bit number to the buffer in the requested byte order.
     *
     * @param buffer Pointer to at least 8 bytes memory.
     * @param number The number to serialize.
     * @param isLittleEndian True to write the number as little-endian, false
     *    for big-endian.
     */
    static void writeUint64(uint8* buffer, uint64 number, bool isLittleEndian);
#endif // XSTL_16BIT

};


/**
 * @brief Big-endian implementation.
 *
 * @details Most significant word first (Motorola)
 *   0x1234 will be stored as 12 34
 */
class cBigEndian
{
public:
    /*
     * Function to implement.
     * See cEndian for explanation about these functions.
     */
    #ifndef XSTL_16BIT
    /**
     * @brief Deserialize a big-endian 64 bit number. See cEndian::readUint64.
     *
     * @param buffer Pointer to at least 8 bytes memory.
     * @return The deserialized number.
     */
    static uint64 readUint64(const uint8* buffer);
    /**
     * @brief Serialize a 64 bit number as big-endian. See cEndian::writeUint64.
     *
     * @param buffer Pointer to at least 8 bytes memory.
     * @param number The number to serialize.
     */
    static void writeUint64(uint8* buffer, uint64 number);
    #endif

    /**
     * @brief Deserialize a big-endian 32 bit number. See cEndian::readUint32.
     *
     * @param buffer Pointer to at least 4 bytes memory.
     * @return The deserialized number.
     */
    static uint32 readUint32(const uint8* buffer);
    /**
     * @brief Deserialize a big-endian 16 bit number. See cEndian::readUint16.
     *
     * @param buffer Pointer to at least 2 bytes memory.
     * @return The deserialized number.
     */
    static uint16 readUint16(const uint8* buffer);
    /**
     * @brief Serialize a 32 bit number as big-endian. See cEndian::writeUint32.
     *
     * @param buffer Pointer to at least 4 bytes memory.
     * @param number The number to serialize.
     */
    static void writeUint32(uint8* buffer, uint32 number);
    /**
     * @brief Serialize a 16 bit number as big-endian. See cEndian::writeUint16.
     *
     * @param buffer Pointer to at least 2 bytes memory.
     * @param number The number to serialize.
     */
    static void writeUint16(uint8* buffer, uint16 number);
    /**
     * @brief Return the byte order of this implementation.
     *
     * @return Always false.
     */
    static bool isLittleEndian() { return false; };
};


/*
 * Definition which implements the big endian interface
 */
#ifdef XSTL_16BIT
/**
 * @brief 16 bit compilers
 */
#define bigEndianImpl \
    virtual uint32 readUint32(const uint8* buffer) const { return cBigEndian::readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return cBigEndian::readUint16(buffer); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { cBigEndian::writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { cBigEndian::writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return cBigEndian::isLittleEndian(); }
#else
/**
 * @brief Non 16 bit compilers
 */
#define bigEndianImpl \
    virtual uint64 readUint64(const uint8* buffer) const { return cBigEndian::readUint64(buffer); }; \
    virtual uint32 readUint32(const uint8* buffer) const { return cBigEndian::readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return cBigEndian::readUint16(buffer); }; \
    virtual void writeUint64(uint8* buffer, uint64 number) const { cBigEndian::writeUint64(buffer,number); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { cBigEndian::writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { cBigEndian::writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return cBigEndian::isLittleEndian(); }
#endif


/**
 * @brief Little-endian implementation.
 *
 * @details Least significant word first (Intel)
 *   0x1234 will be stored as 34 12
 */
class cLittleEndian {
public:
    /*
     * Function to implement
     * See cEndian for explanation about these functions.
     */
    #ifndef XSTL_16BIT
    /**
     * @brief Deserialize a little-endian 64 bit number. See
     *    cEndian::readUint64.
     *
     * @param buffer Pointer to at least 8 bytes memory.
     * @return The deserialized number.
     */
    static uint64 readUint64(const uint8* buffer);
    /**
     * @brief Serialize a 64 bit number as little-endian. See
     *    cEndian::writeUint64.
     *
     * @param buffer Pointer to at least 8 bytes memory.
     * @param number The number to serialize.
     */
    static void writeUint64(uint8* buffer, uint64 number);
    #endif // XSTL_16BIT

    /**
     * @brief Deserialize a little-endian 32 bit number. See
     *    cEndian::readUint32.
     *
     * @param buffer Pointer to at least 4 bytes memory.
     * @return The deserialized number.
     */
    static uint32 readUint32(const uint8* buffer);
    /**
     * @brief Deserialize a little-endian 16 bit number. See
     *    cEndian::readUint16.
     *
     * @param buffer Pointer to at least 2 bytes memory.
     * @return The deserialized number.
     */
    static uint16 readUint16(const uint8* buffer);
    /**
     * @brief Serialize a 32 bit number as little-endian. See
     *    cEndian::writeUint32.
     *
     * @param buffer Pointer to at least 4 bytes memory.
     * @param number The number to serialize.
     */
    static void writeUint32(uint8* buffer, uint32 number);
    /**
     * @brief Serialize a 16 bit number as little-endian. See
     *    cEndian::writeUint16.
     *
     * @param buffer Pointer to at least 2 bytes memory.
     * @param number The number to serialize.
     */
    static void writeUint16(uint8* buffer, uint16 number);
    /**
     * @brief Return the byte order of this implementation.
     *
     * @return Always true.
     */
    static bool isLittleEndian() { return true; };
};


/*
 * Definition which implements the little endian interface
 */
#ifdef XSTL_16BIT
/**
 * @brief 16 bit compilers
 */
#define littleEndianImpl \
    virtual uint32 readUint32(const uint8* buffer) const { return cLittleEndian::readUint32(buffer); }; \
    virtual uint16 readUint16(const uint8* buffer) const { return cLittleEndian::readUint16(buffer); }; \
    virtual void writeUint32(uint8* buffer, uint32 number) const { cLittleEndian::writeUint32(buffer,number); }; \
    virtual void writeUint16(uint8* buffer, uint16 number) const { cLittleEndian::writeUint16(buffer,number); }; \
    virtual bool isLittleEndian() const { return cLittleEndian::isLittleEndian(); }
#else
/**
 * @brief Non 16 bit compilers
 */
#define littleEndianImpl \
        virtual uint64 readUint64(const uint8* buffer) const { return cLittleEndian::readUint64(buffer); }; \
        virtual uint32 readUint32(const uint8* buffer) const { return cLittleEndian::readUint32(buffer); }; \
        virtual uint16 readUint16(const uint8* buffer) const { return cLittleEndian::readUint16(buffer); }; \
        virtual void writeUint64(uint8* buffer, uint64 number) const { cLittleEndian::writeUint64(buffer,number); }; \
        virtual void writeUint32(uint8* buffer, uint32 number) const { cLittleEndian::writeUint32(buffer,number); }; \
        virtual void writeUint16(uint8* buffer, uint16 number) const { cLittleEndian::writeUint16(buffer,number); }; \
        virtual bool isLittleEndian() const { return cLittleEndian::isLittleEndian(); }
#endif


/** @} */
#endif //__TBA_STL_ENDIAN_H
