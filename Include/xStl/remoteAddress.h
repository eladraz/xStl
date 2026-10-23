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

#ifndef __TBA_XSTL_REMOTEADDRESS_H
#define __TBA_XSTL_REMOTEADDRESS_H

/**
 * @addtogroup xstl_core
 * @{
 */

/**
 * @file remoteAddress.h
 * @brief The remote-address is a variant number which can be encoded and
 *    serialized
 *   in several encoding types.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/operators.h"

#ifdef XSTL_16BIT
    // TODO! Fix 16 bit compilers. 64 bit integers are invalid for that
    // compiler,
    //       a class should be used instead
#else
    /**
     * @brief The different ways for remote address stream encoding.
     *
     * @note This struct is not compiled for 16 bit applications since 16 bit
     *    compilers cannot work with 64 bit numbers and remoteAddressValues
     * @see basicInput::streamReadRemoteAddress
     * @see basicInput::changeDecodedAddressType
     * @see basicOutput::streamWriteRemoteAddress
     * @see basicOutput::changeEncodedAddressType
     */
    enum RemoteAddressEncodingTypes {
        /**
         * @brief The remote address is encoded as 16 bit address
         */
        REMOTE_ADDRESS_16BIT = 0,
        /**
         * @brief The remote address is encoded as 32 bit address
         */
        REMOTE_ADDRESS_32BIT = 1,
        /**
         * @brief The remote address is encoded as 64 bit address
         */
        REMOTE_ADDRESS_64BIT = 2,

        /**
         * @brief The maximum address
         */
        REMOTE_ADDRESS_MAX = REMOTE_ADDRESS_64BIT
    };

    //
    // The current machine address encoding
    // TODO! Change for 64bit machines
    //
    // NOTE: This value shouldn't be changed by the user and must be set
    //       according to target compiler settings!
    #ifdef XSTL_64BIT
        /**
         * @brief The remote-address encoding that matches the machine address
         *    size (64 bit targets).
         *
         * Must be set according to the target compiler settings and should not
         * be changed by the user.
         */
        #define DEFAULT_REMOTE_ADDRESS_ENCODING  REMOTE_ADDRESS_64BIT
    #else
        /**
         * @brief The remote-address encoding that matches the machine address
         *    size (32 bit targets).
         *
         * Must be set according to the target compiler settings and should not
         * be changed by the user.
         */
        #define DEFAULT_REMOTE_ADDRESS_ENCODING  REMOTE_ADDRESS_32BIT
    #endif

    /**
     * @brief The highest possible address encoding value for remote address as
     *    a single
     *   uint64 bit number
     */
    typedef uint64 maxRemoteUint;

    /**
     * @brief A remote address, kept as a 64 bit value together with the
     *    encoding type used to serialize it.
     *
     * The address is stored in a union of 64, 32 and 16 bit views (m_union) and
     * m_type tells how many of these bits are significant (the encoding type,
     * see RemoteAddressEncodingTypes). Comparison operators only consider the
     * significant bits.
     */
    struct remoteAddressNumericValue {
        /**
         * @brief Default constructor. Creates the address 0 using
         *    DEFAULT_REMOTE_ADDRESS_ENCODING.
         */
        remoteAddressNumericValue();
        /**
         * @brief Constructor.
         *
         * @param address The address value.
         * @param type The encoding type of the address.
         */
        remoteAddressNumericValue(const maxRemoteUint& address,
                                  RemoteAddressEncodingTypes type =
                                    DEFAULT_REMOTE_ADDRESS_ENCODING);

        // Copy-constructor and operator = will auto generated by the compiler

        /**
         * @brief Return the address clipped according to the bits settings of
         *    m_type
         *
         * @return The address clipped according to the bits settings of m_type.
         */
        maxRemoteUint getClippedAddress() const;

        /**
         * @brief Convert the remote address into the machine address number.
         *
         * The conversion uses the machine encoding,
         * DEFAULT_REMOTE_ADDRESS_ENCODING.
         *
         * @return The address as an addressNumericValue.
         * @throws cException (EXCEPTION_ASSERTION) if the stored address does
         *    not fit in the machine address size (for example a 64 bit address
         *    on a 32 bit machine).
         */
        addressNumericValue getAddressValue() const;

        /**
         * @brief Check whether the address is inside a range.
         *
         * Both ends are compared using the clipped addresses (see
         * getClippedAddress()).
         *
         * @param start The start address of the range.
         * @param end The end address of the range.
         * @return True if this address is within [start..end], false otherwise.
         * @note The implementation does not check that @a start is not greater
         *    than @a end.
         */
        bool isInsideRange(const remoteAddressNumericValue& start,
                           const remoteAddressNumericValue& end) const;

        /**
         * @brief Operator ==. Compare two remote addresses.
         *
         * @note This function is better than NR_ADDRESS(a) == NR_ADDRESS(b)
         *    since it also clips the higher bits, depending on address-type
         * @param other The address to compare with.
         * @return True if the clipped addresses are equal.
         */
        bool operator == (const remoteAddressNumericValue& other) const;

        /**
         * @brief Operator \<. Compare two remote addresses.
         *
         * Compares the clipped addresses. See operator == for comments.
         *
         * @param other The address to compare with.
         * @return True if this clipped address is less than the clipped address
         *    of @a other.
         */
        bool operator < (const remoteAddressNumericValue& other) const;

        // All other comparison operators (generated from == and <)
        /**
         * @brief The comparison operators `!=`, `>`, `>=` and `<=` (generated
         *    by MAKE_SIMPLE_OPERATORS from `==` and `<`).
         */
        MAKE_SIMPLE_OPERATORS(remoteAddressNumericValue);


        // Public members

        // This struct doesn't have any methods since it is kept as
        // straightforward and simple
        // as possible.

        /**
         * @brief All possible address encoding.
         *
         * @note Changing this struct must be done using NR_ADDRESS which change
         *    the 64 bit integer (=maxRemoteInt)
         */
        union {
            /**
             * @brief The address as a 64 bit value (the value that is actually
             *   stored; changed through NR_ADDRESS()).
             */
            uint64 m_64bitAddress;
            uint32 m_32bitAddress;  ///< The 32 bit view of the address storage.
            uint16 m_16bitAddress;  ///< The 16 bit view of the address storage.
        } m_union;  ///< The address storage (64, 32, 16 bit views).

        /**
         * @brief The type of the remote-address.
         *
         * @details Used for encoding and for serialization
         */
        RemoteAddressEncodingTypes m_type;
    };

    /**
     * @brief The maximum value possible for remote-address
     */
    #define MAX_REMOTE_ADDRESS ((maxRemoteUint)(-1))

    /**
     * @brief Set of macros for helping using the remoteAddressNumericValue as a
     *    normal
     *   integer type and commit all operation, such as =, +, - etc...
     *
     * @details Usage:
     *
     * @code
     * remoteAddressNumericValue type;
     * NR_ADDRESS(type) = 0;
     * NR_ADDRESS(type)+= 500;
     * @endcode
     *
     * @param n A remoteAddressNumericValue object.
     */
    #define NR_ADDRESS(n) ((n).m_union.m_64bitAddress)
#endif


/** @} */
#endif // __TBA_XSTL_REMOTEADDRESS_H
