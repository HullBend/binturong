/*
 * Copyright (c) 2002 - 2004    SPZ.
 *
 * http://www.opensource.org/licenses/mit-license.php
 */

//////////////////////////////////////////////////////////////////////
// Thread.cpp: implementation of the Thread class.
//
//////////////////////////////////////////////////////////////////////


#include "Thread.h"

#if !defined (_WIN64) && !defined (_WIN32)
// we already have included pthread.h
#else
#include <process.h> // _beginthreadex etc.
#endif




void* Thread::threadEntryHook(void* pSelf)
{
    Thread* pThis = static_cast<Thread*>(pSelf);
    pThis->run();
    return 0;
}


#if defined (_WIN64) || defined (_WIN32)
unsigned int __stdcall Thread::threadEntryHookWin(void* pSelf)
{
    return (unsigned int) threadEntryHook(pSelf);
}
#endif /* _WIN64 || _WIN32 */


Thread::Thread()
  : m_hThread(0)
{
}


Thread::~Thread()
{
}


bool Thread::start()
{
#if !defined (_WIN64) && !defined (_WIN32)

    return pthread_create(&m_hThread, NULL, threadEntryHook, this) == 0;
#else

    unsigned long handle // temporary for the thread pseudo handle
        = _beginthreadex(0, 0, threadEntryHookWin, this, CREATE_SUSPENDED, NULL);

    if (handle == 0uL) {
        // The return value 0 indicates an error.
        return false;
    }

    m_hThread = reinterpret_cast<HANDLE>(handle);

    // ResumeThread returns the previous suspend count as a
    // DWORD or 0xFFFFFFFF on failure.
    if (ResumeThread(m_hThread) == 0xFFFFFFFFuL) {
        _endthreadex(0);
        CloseHandle(m_hThread);
        m_hThread = reinterpret_cast<HANDLE>(0uL);
        return false;
    }
    // If we are here everything should have worked fine
    return true;

#endif /* !(_WIN64) && !(_WIN32) */
}


bool Thread::join() {
    if (m_hThread) {

#if !defined (_WIN64) && !defined (_WIN32)

        return pthread_join(m_hThread, NULL) == 0;
#else

        const DWORD dwCause = WaitForSingleObject(m_hThread, INFINITE);
        if (dwCause == WAIT_OBJECT_0) {
            return true;
        }
#endif /* !(_WIN64) && !(_WIN32) */

    }
    return false;
}
