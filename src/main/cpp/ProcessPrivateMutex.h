/*
 * Copyright (c) 2002 - 2012    SPZ.
 *
 * http://www.opensource.org/licenses/mit-license.php
 */

//////////////////////////////////////////////////////////////////////
// ProcessPrivateMutex.h: interface for the ProcessPrivateMutex class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCESSPRIVATE_MUTEX_INCLUDED__
#define __PROCESSPRIVATE_MUTEX_INCLUDED__


#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */


// POSIX threads
#if !defined (_WIN64) && !defined (_WIN32)

#include <stdio.h> // sprintf

#include <stdexcept>

#include <pthread.h>

#endif




class __GCC_DONT_EXPORT ProcessPrivateMutex {
public:

    // constructor
    ProcessPrivateMutex();

    // destructor
    inline ~ProcessPrivateMutex() {

#if defined (_WIN64) || defined (_WIN32)
        DeleteCriticalSection(&m_cs);
#else /* POSIX threads */
        pthread_mutex_destroy(&m_cs);
#endif /* (_WIN64) || (_WIN32) */

    }

    // acquire() method
    void acquire() const;

    // release() method
    inline void release() const {

#if defined (_WIN64) || defined (_WIN32)
        LeaveCriticalSection(&m_cs);
#else /* POSIX threads */
        int rc = pthread_mutex_unlock(&m_cs);
        if (rc != 0) {
            const int MAX_LEN = 64;
            char code[MAX_LEN] = {0};
            sprintf(code, "%d", rc);
            const char* errMsg = "ProcessPrivateMutex::release() failed unexpectedly"
                " with error code: ";
            std::string msg(errMsg);
            msg.append(code);
            throw std::runtime_error(msg);
        }
#endif /* (_WIN64) || (_WIN32) */

    }

private:
    // prevent assignment and copying of a ProcessPrivateMutex
    ProcessPrivateMutex(const ProcessPrivateMutex&);
    ProcessPrivateMutex& operator=(const ProcessPrivateMutex&);

private:

#if defined (_WIN64) || defined (_WIN32)

    // the spincount (only used on SMP systems)
    enum {SPINCOUNT = 4000uL};

    // Windows-specific locking primitive
    mutable CRITICAL_SECTION m_cs;

#else /* POSIX threads */

    mutable pthread_mutex_t m_cs;

#endif /* (_WIN64) || (_WIN32) */

};

#endif // __PROCESSPRIVATE_MUTEX_INCLUDED__
