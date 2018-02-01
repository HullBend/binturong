

#ifndef __SOCK_UTIL_H__
#define __SOCK_UTIL_H__


#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */



#if defined (_WIN64) || defined (_WIN32)
#include <winsock2.h>
#endif /* (_WIN64) || (_WIN32) */



#if defined (_WIN64) || defined (_WIN32)

typedef SOCKET SOCKET_T;

/* This is used instead of -1, since the Windows SOCKET type is unsigned. */
const SOCKET_T INOPERATIVE_SOCKET = INVALID_SOCKET;


#else /* Unix or Linux */

typedef int SOCKET_T;

const SOCKET_T INOPERATIVE_SOCKET = -1;

#endif /* (_WIN64) || (_WIN32) */



__GCC_DONT_EXPORT
bool socketRuntimeInit();

__GCC_DONT_EXPORT
bool socketRuntimeShutdown();

__GCC_DONT_EXPORT
bool socketClose(SOCKET_T sock);

__GCC_DONT_EXPORT
SOCKET_T socketSetDefaultOpts(SOCKET_T sd, bool& success);

__GCC_DONT_EXPORT
bool socketSetNonBlocking(SOCKET_T sd);

__GCC_DONT_EXPORT
bool socketShutdownOutput(SOCKET_T sd);

__GCC_DONT_EXPORT
SOCKET_T socketInitServerSock(unsigned short port, int backlog, bool& success);

__GCC_DONT_EXPORT
SOCKET_T socketAccept(SOCKET_T listenSocket, unsigned long timeoutMillis, std::string& serverIP);

__GCC_DONT_EXPORT
std::vector<unsigned char>* socketReadUntilEOF(SOCKET_T sd, unsigned long timeoutMillis, bool& timedOut, bool& errorOccurred);

__GCC_DONT_EXPORT
bool socketWriteAll(SOCKET_T sd, const unsigned char* buf, size_t count, unsigned long timeoutMillis, bool& timedOut);


#endif /* __SOCK_UTIL_H__ */
