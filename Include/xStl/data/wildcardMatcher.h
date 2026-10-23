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

#ifndef __TBA_STL_WILDCARDMATCHER_H
#define __TBA_STL_WILDCARDMATCHER_H

/**
 * @addtogroup xstl_data_strings
 * @{
 */

/**
 * @file wildcardMatcher.h
 * @brief Matches a string to a pattern.
 *
 * @details The pattern can contain wildcard characters such as '*' and '?'.
 *
 * @author Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/string.h"

/**
 * @brief Matches a string to a pattern.
 *
 * @details The pattern can contain wildcard characters such as '*' and '?'.
 */
class cWildcardMatcher {
public:
    /**
     * @brief Return true if 'string' is valid by the 'wildcardPattern'
     *
     * @param string The string to be tested
     * @param wildcardPattern The pattern to use
     * @param isCaseSensitive True means that the match is case sensitive,
     *    otherwise is case insensitive
     * @return True if 'string' is valid by the 'wildcardPattern'.
     */
    static bool match(const cString& string,
                      const cString& wildcardPattern,
                      bool isCaseSensitive = true);
};


/** @} */
#endif // __TBA_STL_WILDCARDMATCHER_H
