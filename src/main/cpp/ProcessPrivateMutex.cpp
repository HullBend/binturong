/*
 * Copyright (c) 2002 - 2012    SPZ.
 *
 * http://www.opensource.org/licenses/mit-license.php
 */

////////////////////////////////////////////////////////////////////////////
// ProcessPrivateMutex.cpp: implementation of the ProcessPrivateMutex class.
//
////////////////////////////////////////////////////////////////////////////

#include "ProcessPrivateMutex.h"

#include <stdio.h> // sprintf

#include <stdexcept>



#if defined (_WIN64) || defined (_WIN32)
// disable "This function may be unsafe" warning for sprintf()
#pragma warning( disable: 4996 )
// disable the "unreachable code" warning from the catch(...)
// in acquire() when we are **not** compiling with the /EHa flag
#pragma warning( disable: 4702 )
#endif /* (_WIN64) || (_WIN32) */




ProcessPrivateMutex::ProcessPrivateMutex() {
#if defined (_WIN64) || defined (_WIN32)

    // Note: The spincount will be ignored on single-processor machines
    if (!InitializeCriticalSectionAndSpinCount(&m_cs, SPINCOUNT)) {
        // this is unexpected
        unsigned long err = GetLastError();
        const int MAX_LEN = 64;
        char code[MAX_LEN] = {0};
        sprintf(code, "%d", err);
        const char* errMsg = "ProcessPrivateMutex::ProcessPrivateMutex()"
            " failed unexpectedly (low memory situation?) - Error Code: ";
        std::string msg(errMsg);
        msg.append(code);
        throw std::runtime_error(msg);
    }

#else /* POSIX threads */

    int rc = pthread_mutex_init(&m_cs, NULL);

    if (rc != 0) {
        const int MAX_LEN = 64;
        char code[MAX_LEN] = {0};
        sprintf(code, "%d", rc);
        const char* errMsg = "ProcessPrivateMutex::ProcessPrivateMutex()"
            " failed unexpectedly with error code: ";
        std::string msg(errMsg);
        msg.append(code);
        throw std::runtime_error(msg);
    }

#endif /* (_WIN64) || (_WIN32) */
}


void ProcessPrivateMutex::acquire() const {
#if defined (_WIN64) || defined (_WIN32)

    try {
        EnterCriticalSection(&m_cs);
    } catch (...) { // this makes only sense with /EHa
        // On Windows 2000, EXCEPTION_INVALID_HANDLE could happen
        // in very low memory situations. I translate this into a
        // C++ exception here. On XP or later EnterCriticalSection
        // is guaranteed not to fail due to lack of resources.
        const char* pszMsg = "ProcessPrivateMutex::acquire() failed"
            " (maybe with EXCEPTION_INVALID_HANDLE)"
            " - this may be an indicator of a very low memory situation";
        throw std::runtime_error(pszMsg);
    }

#else /* POSIX threads*/

    int rc = pthread_mutex_lock(&m_cs);

    if (rc != 0) {
        const int MAX_LEN = 64;
        char code[MAX_LEN] = {0};
        sprintf(code, "%d", rc);
        const char* errMsg = "ProcessPrivateMutex::acquire() failed with return code ";
        std::string msg(errMsg);
        msg.append(code);
        throw std::runtime_error(msg);
    }

#endif /* (_WIN64) || (_WIN32) */
}
