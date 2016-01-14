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
 * trace.cpp
 *
 * Implementation file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/os/lock.h"
#include "xStl/os/xstlLockable.h"
#include "xStl/data/char.h"
#include "xStl/data/datastream.h"
#include "xStl/except/trace.h"

#if !(defined(XSTL_WINDOWS) || defined (XSTL_LINUX) || defined (XSTL_16BIT) || defined (XSTL_CE))
    // Microsoft drivers cannot use this code. It's just not good for them.
    #error "NT-DDK has a special trace utility!!"
#endif

#if (TRACE_METHOD == TRACE_COUT)
    #include "xStl/stream/ioStream.h"
#elif (TRACE_METHOD == TRACE_MSG)
    #if !(defined(XSTL_WINDOWS) || defined (XSTL_CE))
        #include "xStl/stream/ioStream.h"
    #endif
#elif (TRACE_METHOD == TRACE_LOG)
    // Output trace into log file
    #include "xStl/string.h"
    #include "xStl/os/file.h"
    static cFile logFile;
#endif

#ifdef XSTL_CE_DBGVIEWTRACE
    // Sending command messages into DLL routine
    #include "xStl/os/ceddk/DbgCom.h"
    static bool gIsFirstTime = true;
    static HINSTANCE hDllInstance = NULL;
    DBGMSG gDbgMessageFunc = NULL;

    bool checkCeModule()
    {
    #ifndef XSTL_CEDRIVER
        return false;
    #else
        if (gIsFirstTime)
        {
            if ((hDllInstance = LoadLibrary(DBGMSG_DLL_NAME)) != NULL)
            {
                gDbgMessageFunc = (DBGMSG)GetProcAddress(hDllInstance,
                                                         DBGMSG_EXPORT_NAME);
                // if (gDbgMessageFunc == NULL)
                //    MessageBox(NULL, L"ERROR PROC", L"XSTL", MB_OK);
            }   // else
                // MessageBox(NULL, L"ERROR LOAD", L"XSTL", MB_OK);
            gIsFirstTime = false;
        }

        return ((hDllInstance != NULL) && (gDbgMessageFunc != NULL));
    #endif
    }

#endif

#ifdef _DEBUG

/*
 * Global locking Mutex class
 *
 * In order to avoid atexit destructor routine from accessing the 'TRACE'
 * facility, and since there aren't any way to geruntee execuation time,
 * there will be here a small memory lost and resource leak.
 */
class traceLockMutex {
public:
    static cLockableObject& getMutexObject()
    {
        if (g_traceMutex == NULL)
            g_traceMutex = new cXstlLockable();

        return *g_traceMutex;
    }

private:
    // The TRACE mutex
    static cXstlLockable* g_traceMutex;
};
cXstlLockable* traceLockMutex::g_traceMutex = NULL;


void traceMessage(int traceLevel, const cString& message)
{
    if (traceLevel >= TRACE_LEVEL)
    {
        // Lock the traces from other threads...
        cLock lock(traceLockMutex::getMutexObject());

        #if (TRACE_METHOD == TRACE_CERR)
            #if (!(defined(XSTL_WINDOWS) || defined (XSTL_CE)))
                cerr << "Trace: " << message << endl;
            #else
                #ifdef XSTL_CE_DBGVIEWTRACE
                // Loading the module
                if (checkCeModule())
                {
                    // Also add a time header
                    uint32 time = (uint32)(cOS::getSystemTime() & 0xFFFFFFFF);
                    cString outmsg(HEXDWORD(time));
                    outmsg+= "    ";
                    outmsg+= message;
                    gDbgMessageFunc(outmsg);
                }
                #endif
                OutputDebugString(OS_CSTRING(message.getBuffer()));
                //cerr << "Trace: " << message;
                //cerr.flush();
            #endif
        #endif
        #if (TRACE_METHOD == TRACE_MSG)
            /* Display a modal message box */
            #ifdef XSTL_WINDOWS
                ::MessageBox(NULL,
                             message.getBuffer(),
                             XSTL_STRING("TRACE"),
                             MB_ICONSTOP | MB_OK);
            #else
                // TODO! SHOULD CHANGE HERE!!
                cout << XSTL_STRING("Trace: ") << message << endl;
            #endif
        #endif
        #if (TRACE_METHOD == TRACE_LOG)
            // TODO! This code hasn't compiled for a while, it's need to be
            //       upgarded
            // Write trace log into file
            if (!logFile.isOpenForWrite())
            {
                // Open file for writing and opened to append data
                XSTL_TRY
                {
                    // Try to open an exsist file
                    logFile.open(TRACE_logFileNAME, cFile::cFile_READ | cFile::cFile_WRITE);
                    logFile.seek(0, IO_SEEK_END);
                }
                XSTL_CATCH(cFileException e)
                {
                    // Try to create the file
                    logFile.open(TRACE_logFileNAME, cFile::cFile_CREATE | cFile::cFile_WRITE);
                }
            }

            // Write the message
            cString output = message;

            output+= TRACE_ENTER;
            logFile << output;
        #endif
    }
}

#endif // _DEBUG
