/*
 * Copyright (c) 2002 - 2004    SPZ.
 *
 * http://www.opensource.org/licenses/mit-license.php
 */

//////////////////////////////////////////////////////////////////////
// Thread.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(THREADS_THREAD_INCLUDED__)
#define THREADS_THREAD_INCLUDED__


#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */


#if !defined (_WIN64) && !defined (_WIN32)
#include <pthread.h>
#else
// "stdafx.h" already included // for HANDLE
#endif



/**
 * Caution: this Thread class is copy constructible and
 * assignable! That's a very questionable design for a
 * class that represents an OS thread.
 *
 * TODO: revisit this design.
 */
class __GCC_DONT_EXPORT Thread  
{
public:
    Thread();

    virtual ~Thread();

    bool start();

    bool join();

    virtual void run() = 0;


private: // static methods

    static void* threadEntryHook(void*);

#if defined (_WIN64) || defined (_WIN32)
    static unsigned int __stdcall threadEntryHookWin(void*);
#endif /* _WIN64 || _WIN32 */


private: // thread handle member

#if !defined (_WIN64) && !defined (_WIN32)
    pthread_t m_hThread;
#else
    HANDLE m_hThread;
#endif /* !(_WIN64) && !(_WIN32) */

};

#endif // !defined(THREADS_THREAD_INCLUDED__)
