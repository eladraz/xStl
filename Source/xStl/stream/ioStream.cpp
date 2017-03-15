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

#include "xStl/xStlPrecompiled.h"
/*
 * iostream.cpp
 *
 * Implemetation (Both Windows and UNIX) for the output/
 * input con streams.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/except/trace.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/ioStream.h"

#if !(defined (XSTL_NTDDK) || defined(XSTL_16BIT))
    // Windows and linux uses files.
    #include "xStl/stream/fileStream.h"
#endif


#if defined(XSTL_WINDOWS)
    #include <windows.h>
#elif defined(XSTL_LINUX)
    static const char IOSTREAM_COUT_FILENAME[] = "/dev/stdout"; //"CONOUT$";
    static const char IOSTREAM_CERR_FILENAME[] = "/dev/stderr"; //"CONOUT$";
#elif defined(XSTL_NTDDK)
#else
    #include <conio.h>
    #include <stdio.h>
#endif


#ifndef CON_COUT_DEFAULT_BASE
#define CON_COUT_DEFAULT_BASE (10)
#endif


/*
 * conOutStream constructor
 *
 * Fill up the default behavor for the output stream
 */
conOutStream::conOutStream(conOutObjects object)  :
    cStringerStream(true)
    #ifdef XSTL_NTDDK
        ,m_conOut(MAX_CON_MESSAGES)
    #endif
{
    setBase(CON_COUT_DEFAULT_BASE);

    #if defined(XSTL_WINDOWS)
        switch (object)
        {
        case CON_OUT_COUT_OBJECT:
            m_outputFile.openHandle(GetStdHandle(STD_OUTPUT_HANDLE), cFile::WRITE, false);
            break;
        case CON_OUT_CERR_OBJECT:
            m_outputFile.openHandle(GetStdHandle(STD_ERROR_HANDLE), cFile::WRITE, false);
            break;
        }
    #elif defined(XSTL_LINUX)
        // Open std file
        switch (object)
        {
        case CON_OUT_COUT_OBJECT:
            m_outputFile.open(IOSTREAM_COUT_FILENAME, cFile::WRITE);
            break;
        case CON_OUT_CERR_OBJECT:
            m_outputFile.open(IOSTREAM_CERR_FILENAME, cFile::WRITE);
            break;
        }
    #elif defined(XSTL_NTDDK)
    #elif defined(XSTL_16BIT)
    #else
        #error "Unknown platform!"
    #endif
}

conOutStream& conOutStream::getCout()
{
    static conOutStream m_cout(CON_OUT_COUT_OBJECT);
    return m_cout;
}

conOutStream& conOutStream::getCerr()
{
    static conOutStream m_cerr(CON_OUT_CERR_OBJECT);
    return m_cerr;
}

void conOutStream::outputString(const cString& string)
{
    #ifdef XSTL_NTDDK
        // XDK implementation
        m_conOut.addMessage(string);
    #else
        #ifdef XSTL_16BIT
            // 16 bit implementation
            // Unicode is not defined
            printf("%s", string.getBuffer());
        #else
            // Windows and linux implementations
            uint len = string.length();
            #ifndef XSTL_UNICODE
                // The normal copy template
                m_outputFile.write(string.getBuffer(), len);
            #else
                // Covert to ascii
                m_outputFile.write(string.getASCIIstring().getBuffer(), len);
            #endif // XSTL_UNICODE
        #endif // XSTL_16BIT
    #endif // XSTL_NTDDK
}
