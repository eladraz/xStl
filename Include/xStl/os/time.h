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

#ifndef __TBA_STL_OS_TIME_H
#define __TBA_STL_OS_TIME_H

/*
 * time.h
 *
 * Abstract class for time. The time is a pack struct of the date in the year and
 * the time of the day. The class is wrapped in abstract interface which allows
 * accessing the time and modify it in very simple way.
 * The time can be the english calander time or even the hebrew calander time.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"

/*
 * class cTime
 */
class cTime {
public:
    /*
     * Return the time packed in the current operation system mode. The result
     * buffer can be covert to the 'time_t' struct which changes between OS
     * platform.
     */
    cBuffer getOSTime();

    // Get functions

    /*
     * Return the year. Note that the year is not limit to or from the year 1970
     */
    uint getYear();

    /*
     * Return the mounth of the year
     */
    uint getMounth();

    /*
     * Return the day of the mounth
     */
    uint getDay();

    // Set functions.

    /*
     * Change the date. Change the year and the mounth and the day.
     *
     * newYear   - The new year for the date
     * newMounth - The new mounth for the date
     * newDay    - The new day for the date
     *
     * Return true if the change completed. False is the date is invalid (such as
     * 29 feb. for 3 out of 4 years).
     */
    bool setDate(uint newYear,
                 uint newMounth,
                 uint newDay);

    /*
     * Change the OS date. The format of the date is depened by the
     * implementation.
     *
     * newDate - The OS formated bytes which represent the date.
     *
     * Return the status of the change or the validation of the time packet.
     */
    bool setDate(const cBuffer& newOSDate);
};

#endif // __TBA_STL_OS_TIME_H
