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

#ifndef __TBA_STL_DATASTREAM_H
#define __TBA_STL_DATASTREAM_H

/**
 * @addtogroup xstl_data_dump
 * @{
 */

/**
 * @file datastream.h
 * @brief Provides a set of tools to dump memory structs to the screen.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to offer. Warning C4505 should be disabled for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif

/*
 * The default number of elements in a row.
 */
#ifndef TTY_MAX_ELEMENTS
/**
 * @brief The default number of elements in a dump row. It can be overridden by
 *    defining it before including the header.
 */
#define TTY_MAX_ELEMENTS (16)
#endif

/*
 * The following functions are helpers only. But they can be good for other
 * implementations.
 */

/**
 * @brief The default dump of non-printable character.
 */
#define DB_DEFAULT_CHAR '.'

/**
 * @brief Return a printable character representing the character.
 *
 * @param ch The character to be queried
 * @param nonPrintable The non-printable value for non-printable characters.
 * @return A printable character representing the character.
 */
uint8 DB(const uint8& ch,
         const uint8 nonPrintable = DB_DEFAULT_CHAR);

/**
 * @brief Return the high nibble of a byte as a hexadecimal character.
 *
 * @param ch The byte.
 * @return The hexadecimal character of the high nibble of @a ch.
 */
character HEXHIGH(const uint8& ch);
/**
 * @brief Return the low nibble of a byte as a hexadecimal character.
 *
 * @param ch The byte.
 * @return The hexadecimal character of the low nibble of @a ch.
 */
character HEXLOW(const uint8& ch);


/**
 * @brief Convert an 8 bit number to its hexadecimal string.
 *
 * @param ch The number to convert.
 * @return The hexadecimal string.
 */
cString HEXBYTE(const uint8& ch);
/**
 * @brief Convert a 16 bit number to its hexadecimal string.
 *
 * @param word The number to convert.
 * @return The hexadecimal string.
 */
cString HEXWORD(const uint16& word);
/**
 * @brief Convert a 32 bit number to its hexadecimal string.
 *
 * @param dword The number to convert.
 * @return The hexadecimal string.
 */
cString HEXDWORD(const uint32& dword);

#ifndef XSTL_16BIT
/**
 * @todo 16 bit compilers don't support 64bit integers.
 * @todo Add uint64 as a class
 * @param qword The number to convert.
 * @return The hexadecimal string.
 */
cString HEXQWORD(const uint64& qword);
#endif

/**
 * @brief Convert a number to the closest hexadecimal value.
 *
 * @details If the number is 8 bit then HEXBYTE will be used, if the number is a
 *    16 bit number then HEXWORD will be used, etc.
 *
 * @param number The number to convert.
 * @return The hexadecimal string.
 */
cString HEXNUMBER(uint number);

/**
 * @brief Convert an address to hex-number.
 *
 * @details If the machine uses 32bit address then the conversions will be to
 *    DWORD otherwise the conversions will be 64bit or 24bit depending on the
 *    machine.
 *
 * @param address The address to convert.
 * @return The hexadecimal string.
 */
cString HEXADDRESS(const addressNumericValue& address);

/**
 * @brief Convert a remote address into hex-number.
 *
 * @details The encoding is taken from the remote address
 *
 * @param address The remote address to convert.
 * @return The hexadecimal string.
 */
cString HEXREMOTEADDRESS(const remoteAddressNumericValue& address);

/**
 * @brief A hexadecimal dump of a range of elements.
 *
 * @details An ostream class wrapper. Use this class to dump information
 *    according to
 *   its hex data.
 *   Use the class as follows:
 *   cStrem object;
 *   cout \<\< DATA(object.begin(), object.end()) \<\< endl;
 *
 * @tparam ITERATOR The type of the iterator over the dumped elements.
 * @tparam INFO The type of the dumped elements.
 */
template <class ITERATOR, class INFO>
class DUMP
{
public:
    /**
     * @brief DUMP class flags
     */
    enum {
        /**
         * @brief Print the address at the beginning of each row.
         */
        DATA_USE_ADDRESS = 1,
        /**
         * @brief Print the address in hexadecimal.
         */
        DATA_ADDRESS_HEX = 2
    };

    /**
     * @brief Constructor. Create a dump object with operator \<\<.
     *
     * @details elementInRow - How many objects to dump in a line.
     *
     * @param begin The iterator to the beginning of the object.
     * @param end The iterator to the end of the object.
     * @param flags How to dump the object. DATA_USE_ADDRESS dump the object
     *    with address DATA_ADDRESS_HEX print the address in hex
     * @param startAddress The address for the first iterator
     * @param segmentName The name of the segment
     * @param elementsInRow How many elements to dump in a row. The default is
     *    TTY_MAX_ELEMENTS.
     */
    DUMP(ITERATOR begin,
         ITERATOR end,
         unsigned int flags = DATA_USE_ADDRESS,
         addressNumericValue startAddress = 0,
         const character* segmentName  = NULL,
         unsigned int elementsInRow = TTY_MAX_ELEMENTS) :
        m_begin(begin),
        m_end  (end),
        m_flags(flags),
        m_elementsInRow(elementsInRow),
        m_startAddress (startAddress),
        m_segmentName  (segmentName)
    {
    }

    /**
     * @brief Copy constructor
     *
     * @param other The dump object to copy.
     */
    DUMP(const DUMP<ITERATOR, INFO> &other) :
        m_begin(other.m_begin),
        m_end  (other.m_end),
        m_flags(other.m_flags),
        m_elementsInRow (other.m_elementsInRow),
        m_startAddress  (other.m_startAddress),
        m_segmentName   (other.m_segmentName)
    {
    }

#if (defined(XSTL_WINDOWS) || defined(XSTL_NTDDK) || defined(XSTL_CE))
    /**
     * @brief For Windows compilers, we don't need to declare any template
     *    friend
     *   function.
     */
    template <class OSTREAM>
    friend OSTREAM& operator << (OSTREAM &out, DUMP<ITERATOR, INFO>& object);

    template <class OSTREAM>
    friend OSTREAM & hexDumpObject(OSTREAM &out, DUMP<ITERATOR, INFO>& object);
#else
    /**
     * @brief For the g++ compiler we need to declare these functions as
     *    template
     *   functions
     */
    template <class OSTREAM, class FW_ITR, class DT>
    friend OSTREAM& operator << (OSTREAM &out, DUMP<FW_ITR, DT>& object);

    template <class OSTREAM, class FW_ITR, class DT>
    friend OSTREAM & hexDumpObject(OSTREAM &out, DUMP<FW_ITR, DT>& object);
#endif

public:
    /**
     * @brief The iterator to the beginning of the object
     */
    ITERATOR m_begin;
    /**
     * @brief The iterator to the end of the object
     */
    ITERATOR m_end;
    /**
     * @brief How to dump the object.
     *
     * @details See DATA_USE_ADDRESS
     */
    unsigned int m_flags;
    /**
     * @brief How many objects to dump in a line
     */
    unsigned int m_elementsInRow;
    /**
     * @brief The address for the first iterator
     */
    addressNumericValue m_startAddress;
    /**
     * @brief The name of the segment
     */
    const character* m_segmentName;
};


/**
 * @brief Print a hex byte of the object.
 *
 * @details The object stores the type information of how to print the object
 *    and where (and how) it is stored in the memory
 *
 * @param out The output stream to be used.
 * @param object The object to be dumped
 * @return The reference object to the dump object.
 *
 * @tparam OSTREAM The type of the output stream.
 * @tparam FW_ITR The type of the forward iterator of the dump object.
 * @tparam DT The type of the dumped elements.
 */
template <class OSTREAM, class FW_ITR, class DT>
OSTREAM & hexDumpObject(OSTREAM &out, DUMP<FW_ITR, DT>& object);

/**
 * @brief operator \<\<
 *
 * @details Uses the hexDumpObject for the dumping.
 *
 * @tparam OSTREAM The type of the output stream.
 * @tparam FW_ITR The type of the forward iterator of the dump object.
 * @tparam DT The type of the dumped elements.
 * @param out The output stream to be used.
 * @param object The object to be dumped.
 * @return The output stream @a out.
 */
template <class OSTREAM, class FW_ITR, class DT>
OSTREAM& operator << (OSTREAM &out, DUMP<FW_ITR, DT>& object);

/**
 * @brief DATA commands
 *
 * @details Since most of the use of the DUMP class will be to dump cBuffer data
 *   then there is a shortcut.
 */
typedef DUMP<const uint8*, uint8>                      DATA8;
/**
 * @brief Dump of the bytes of a cBuffer.
 */
typedef DUMP<cBuffer::iterator, uint8>                 DATA;
/**
 * @brief Dump of the bytes of a cBuffer.
 */
typedef DUMP<cBuffer::iterator, uint8>                 DUMP_BYTE;
/**
 * @brief Dump of an array of 16 bit words.
 */
typedef DUMP<cArray<unsigned short>::iterator, uint16> DUMP_WORD;
/**
 * @brief Dump of an array of 32 bit double-words.
 */
typedef DUMP<cArray<unsigned int>::iterator,   uint32> DUMP_DWORD;

/*
 * Template implementation file.
 */
#include "xStl/data/datastream.inl"

#ifdef XSTL_WINDOWS
    // Restore the warning levels
    #pragma warning(pop)
#endif



/** @} */
#endif
