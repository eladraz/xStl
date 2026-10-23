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

#ifndef __TBA_STL_OS_STREAMMEMORYACCESSER_H
#define __TBA_STL_OS_STREAMMEMORYACCESSER_H

/**
 * @addtogroup xstl_os
 * @{
 */

/**
 * @file streamMemoryAccesser.h
 * @brief A stream memory accesser is a manageable reference-countable memory
 *    stream
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/endian.h"
#include "xStl/data/smartptr.h"
#include "xStl/os/fragmentsDescriptor.h"
#include "xStl/os/virtualMemoryAccesser.h"
#include "xStl/os/threadUnsafeMemoryAccesser.h"

/**
 * @brief A stream memory accesser is a manageable reference-countable memory
 *   stream.
 *
 * @note It's safer to never change the size or the content of the cBuffer data,
 *    however, this option exists. After a change at the 'data' pointer, the
 *    'updateStreamBoundaries' function must be invoked.
 */
class cStreamMemoryAccesser : public cThreadUnsafeMemoryAccesser {
public:
    /**
     * @brief Constructor. Create a thread memory accesser that points to the
     *    content of
     *   the 'data'.
     *
     * @details If the data stream is changed, there must be a notification. See
     *    updateStreamBoundaries.
     *
     * @param data The stream which the thread memory points at.
     */
    cStreamMemoryAccesser(const cBufferPtr& data);

    /**
     * @brief Called when the 'data' cBuffer object was modified outside the
     *    content of
     *   the cVirtualMemoryAccesser.
     *
     * @details This function recalibrates the boundaries of the stream.
     */
    void updateStreamBoundaries();

private:
    // Copy-constructor and operator = are denied by the cThreadUnsafeMemoryAccesser

    /**
     * @brief The reference-countable data object
     */
    cBufferPtr m_data;
};


/** @} */
#endif // __TBA_STL_OS_STREAMMEMORYACCESSER_H
