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

#ifndef __TBA_STL_IOSTREAM_H
#define __TBA_STL_IOSTREAM_H

/*
 * iostream.h
 *
 * Declaration of the cout/cin/cerr class
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/endian.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/stringerStream.h"

#if defined(XSTL_NTDDK)
    // The repository for the console messages
    #include "XDK/xdkTrace.h"
#elif (defined(XSTL_16BIT) || defined(XSTL_CE))
    // DOS can use files, however BIOS application can't...
    // TODO!
#else
    // Windows and linux platforms uses files as thier console output
    #include "xStl/stream/fileStream.h"
#endif


/*
 * conOutStream
 *
 * Output steam directed to the console output.
 */
class conOutStream : public cStringerStream
{
public:
    /*
     * Constructor. This stream uses a cache and waits until new-line is send.
     */
    conOutStream();

    /*
     * Return a global (Singelton) cout object
     */
    static conOutStream& getCout();

    /*
     * Return a global (Singelton) cerr object
     */
    static conOutStream& getCerr();

protected:
    /*
     * Output the string into the m_data member.
     * See cStringerStream::outputString
     */
    virtual void outputString(const cString& string);

private:
    // Type of out objects.
    enum conOutObjects
    {
        CON_OUT_COUT_OBJECT,
        CON_OUT_CERR_OBJECT
    };

    /*
     * Private constructor. Create con-out object according to the out-object
     * type.
     *
     * object - The type of the out-object
     *
     * Throws exception in case of construction error.
     */
    conOutStream(conOutObjects object);

    #if defined(XSTL_NTDDK)
        // WINDOWS NT+ DEVICE DRIVERS
        // Only the cConsoleDeviceControls can pools out the console.
        // See cConsoleDevice
        friend class cConsoleDeviceControls;
        // Use as a repository for holding the console messages
        cXdkTrace m_conOut;
        // The memory limits
        enum { MAX_CON_MESSAGES = 2000 };

    #elif (defined (XSTL_16BIT) || defined (XSTL_CE))
        // So far the implementation depend on standard libraries which uses
        // the C printf() method.
        // Windows CE are implemented by the ConsoleLibrary. Please review the
        // code on this project.
    #else
        // WINDOWS APP and UNIX application
        // The output stream for windows and linux
        cFileStream m_outputFile;
    #endif
};


/*
 * Define the cout/cerr objects
 */
#define cout conOutStream::getCout()
#define cerr conOutStream::getCerr()

#endif //__TBA_STL_IOSTREAM_H
