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

/*
 * datastream.h
 *
 * Provide set of tools to dump memory structs into the screen.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"

#ifdef XSTL_WINDOWS
    // Template classes might not use all the local functions the interface has
    // to ofer. Warning C4505 should be over-written for template functions
    #pragma warning(push)
    #pragma warning(disable:4505)
#endif

/*
 * The default number of elements in a row.
 */
#ifndef TTY_MAX_ELEMENTS
#define TTY_MAX_ELEMENTS (16)
#endif

/*
 * The following functions are helpers only. But they can be good for other
 * implementation.
 */

/*
 * The default dump of non-printable character.
 */
#define DB_DEFUALT_CHAR '.'

/*
 * Return a printable character represinting the character.
 *
 * ch           - The character to be query
 * nonPrintable - The non-printable value for non-printable characters.
 *
 * Return a printable value of the character.
 */
uint8 DB(const uint8& ch,
         const uint8 nonPrintable = DB_DEFUALT_CHAR);

/*
 * Return the high/low nibble of a character as an hex char.
 */
character HEXHIGH(const uint8& ch);
character HEXLOW(const uint8& ch);


/*
 * Convert a number to hexadecimal. Return a cString.
 */
cString HEXBYTE(const uint8& ch);
cString HEXWORD(const uint16& word);
cString HEXDWORD(const uint32& dword);

#ifndef XSTL_16BIT
// TODO! 16 bit compilers doesn't support 64bit integer.
// TODO! Add uint64 as a class
cString HEXQWORD(const uint64& qword);
#endif

/*
 * Convert a number to the closet hexadecimal value. If the number if 8 bit
 * then HEXBYTE will be in used, if the number is 16 bit number then HEXWORD
 * will be used. etc.
 */
cString HEXNUMBER(uint number);

/*
 * Convert an address to hex-number. If the machine uses 32bit address then the
 * conversions will be to DWORD otherwise the conversions will be 64bit or 24bit
 * depending on the machine.
 */
cString HEXADDRESS(const addressNumericValue& address);

/*
 * Convert a remote address into hex-number. The encoding is taken from the
 * remote address
 */
cString HEXREMOTEADDRESS(const remoteAddressNumericValue& address);

/*
 * DUMP class
 *
 * An ostream class wrraper use this class to dump information according to
 * thier hex data.
 * Using the class as the following:
 *  cStrem object;
 *  cout << DATA(object.begin(), object.end()) << endl;
 */
template <class ITERATOR, class INFO>
class DUMP
{
public:
    /*
     * DUMP class flags
     */
    enum {
        // Print the
        DATA_USE_ADDRESS = 1,
        DATA_ADDRESS_HEX = 2
    };

    /*
     * Constructor. Create a dump object with operator <<.
     *
     * begin - The iterator to the beginning of the object.
     * end   - The iterator to the end of the object.
     * flags - How to dump the object.
     *           DATA_USE_ADDRESS dump the object with address
     *           DATA_ADDRESS_HEX print the address in hex
     * startAddress - The address for the first iterator
     * segmentName  - The name of the segment
     * elementInRow - How many objects to dump in a line.
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

    // Copy constructor
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
    /*
     * For windows compilers, we don't need to declare any template
     * function of friend function.
     */
    template <class OSTREAM>
    friend OSTREAM& operator << (OSTREAM &out, DUMP<ITERATOR, INFO>& object);

    template <class OSTREAM>
    friend OSTREAM & hexDumpObject(OSTREAM &out, DUMP<ITERATOR, INFO>& object);
#else
    /*
     * For g++ compiler we need to declare does function
     * as an template function
     */
    template <class OSTREAM, class FW_ITR, class DT>
    friend OSTREAM& operator << (OSTREAM &out, DUMP<FW_ITR, DT>& object);

    template <class OSTREAM, class FW_ITR, class DT>
    friend OSTREAM & hexDumpObject(OSTREAM &out, DUMP<FW_ITR, DT>& object);
#endif

public:
    // The iterator to the beginning of the object
    ITERATOR m_begin;
    // The iterator to the end of the object
    ITERATOR m_end;
    // How to dump the object. See DATA_USE_ADDRESS
    unsigned int m_flags;
    // How many objects to dump in a line
    unsigned int m_elementsInRow;
    // The address for the first iterator
    addressNumericValue m_startAddress;
    // The name of the segment
    const character* m_segmentName;
};


/*
 * Print an hex byte of the object. The object store the type information
 * of how to print the object and where (and how) it is store in the memory
 *
 * out    - The output stream to be used.
 * object - The object to be dump
 *
 * Returns the reference object to the dump object.
 */
template <class OSTREAM, class FW_ITR, class DT>
OSTREAM & hexDumpObject(OSTREAM &out, DUMP<FW_ITR, DT>& object);

/*
 * operator <<
 *
 * Uses the hexDumpObject for the dumping.
 */
template <class OSTREAM, class FW_ITR, class DT>
OSTREAM& operator << (OSTREAM &out, DUMP<FW_ITR, DT>& object);

/*
 * DATA commands
 *
 * Since must of the use in the DUMP class will be to dump cBuffer data
 * then there is a shortcut.
 */
typedef DUMP<const uint8*, uint8>                      DATA8;
typedef DUMP<cBuffer::iterator, uint8>                 DATA;
typedef DUMP<cBuffer::iterator, uint8>                 DUMP_BYTE;
typedef DUMP<cArray<unsigned short>::iterator, uint16> DUMP_WORD;
typedef DUMP<cArray<unsigned int>::iterator,   uint32> DUMP_DWORD;

/*
 * Template implementation file.
 */
#include "xStl/data/datastream.inl"

#ifdef XSTL_WINDOWS
    // Restore the warning levels
    #pragma warning(pop)
#endif


#endif
