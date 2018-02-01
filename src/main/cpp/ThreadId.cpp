
#include "ThreadId.h"

#include <stdio.h> // sprintf

#if defined(__linux)
#include <sys/syscall.h>    // SYS_gettid
#include <unistd.h>         // syscall & getpid
#endif /* __linux */


#if defined (__hpux)
#include <unistd.h>         // getpid
#include <pthread.h>        // pthread_self
#endif /* __hpux */



#if defined (_WIN64) || defined (_WIN32)
#include <process.h> // _getpid

// disable "This function may be unsafe" warnings for older C API functions
#pragma warning( disable: 4996 ) // instead of _CRT_SECURE_NO_WARNINGS, which doesn't work
#endif /* (_WIN64) || (_WIN32) */




const std::string getCurrentThreadId_portable() {
    const int MAX_LEN = 64;
    char id_[MAX_LEN] = {0};

#if defined (_WIN64) || defined (_WIN32)
    sprintf(id_, "%d", GetCurrentThreadId());
    return std::string(id_);
#elif defined (__linux) && defined (__GNUG__)
    sprintf(id_, "%ld", syscall( SYS_gettid ));
    return std::string(id_);
#elif defined (__hpux)
    sprintf(id_, "%d", pthread_self());
    return std::string(id_);
#else
    return std::string("0");
#endif

}


long getCallingProcessId_portable() {
#if defined (_WIN64) || defined (_WIN32)
    // return type is actually int
    return _getpid();
#else /* Linux / Unix */
    // assumes that pid_t is actually an int
    return getpid();
#endif
}
