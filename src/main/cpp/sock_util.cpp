

#include "sock_util.h"

#ifndef __SUPPRESS_SIGPIPE_H__
#include "SuppressSigPipe.h"
#endif /* __SUPPRESS_SIGPIPE_H__ */


#if defined (_WIN64) || defined (_WIN32)

#include <WS2tcpip.h>
#include <errno.h> // for EAGAIN (not really used in WinSocks, but allows code to be uniform)

#else /* Unix / Linux */

#include <sys/time.h> // struct timeval

#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <netdb.h> // getnameinfo
#include <fcntl.h> // fcntl
#include <unistd.h> // close
#include <errno.h>


#endif /* (_WIN64) || (_WIN32) */


#ifndef __CURRENTTIME_MILLIS_H__
#include "currentTimeMillis.h"
#endif /* __CURRENTTIME_MILLIS_H__ */



#if defined (_WIN64) || defined (_WIN32) && (!defined(__MINGW32__) && !defined(__MINGW64__))
/* disable "conditional expression is constant" warnings for FD_SET macro on Windows */
#pragma warning( disable: 4127 )
#endif /* (_WIN64) || (_WIN32) */



#if defined (_WIN64) || defined (_WIN32)

static const int _E_INTR = WSAEINTR;
static const int _E_WOULDBLOCK = WSAEWOULDBLOCK;
static const int _E_AGAIN = EAGAIN;

static const int _SHUT_WR = SD_SEND;

#else /* Linux / Unix */

static const int _E_INTR = EINTR;
static const int _E_WOULDBLOCK = EWOULDBLOCK;
static const int _E_AGAIN = EAGAIN;

static const int _SHUT_WR = SHUT_WR;

#endif /* (_WIN64) || (_WIN32) */


static void millis2timeval(struct timeval& tv, unsigned long ms) {
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
}


static int socketGetLastError() {
#if defined (_WIN64) || defined (_WIN32)

    return WSAGetLastError();
#else /* Linux / Unix */

    return errno;
#endif /* Linux / Unix */
}



bool socketClose(SOCKET_T sock) {

#if defined (_WIN64) || defined (_WIN32)
    return closesocket(sock) == 0;
#else /* Unix or Linux */
    return close(sock) != -1;
#endif /* (_WIN64) || (_WIN32) */

}


SOCKET_T socketSetDefaultOpts(SOCKET_T sd, bool& success) {
    const int optYes = 1;
    int rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*) &optYes, sizeof(int));
    if (rc != 0) {
        success = false;
        socketClose(sd);
        return INOPERATIVE_SOCKET;
    }
    rc = setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (char*) &optYes, sizeof(int));
    if (rc != 0) {
        success = false;
        socketClose(sd);
        return INOPERATIVE_SOCKET;
    }
    return sd;
}


SOCKET_T socketInitServerSock(unsigned short port, int backlog, /*unsigned long acceptTimeoutMillis,*/ bool& success) {
    struct sockaddr_in srvAddr = {};
    srvAddr.sin_family = AF_INET; // IPv4
    srvAddr.sin_port = htons(port);
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY); // (one of) our IPv4 IP(s)

    SOCKET_T sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == INOPERATIVE_SOCKET) {
        success = false;
        return INOPERATIVE_SOCKET;
    }

    socketSetDefaultOpts(sd, success);
    if (!success) {
        return INOPERATIVE_SOCKET;
    }

#if 0
#if defined (_WIN64) || defined (_WIN32)
    DWORD optRcv = acceptTimeoutMillis;

#else /* Unix or Linux */
    struct timeval optRcv;
    millis2timeval(optRcv, acceptTimeoutMillis);

#endif /* (_WIN64) || (_WIN32) */

    // timeout for accept()
    // TODO: this will most likely **not** work under __hpux !!
    rc = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char*) &optRcv, sizeof(optRcv));
    if (rc != 0) {
        success = false;
        socketClose(sd);
        return INOPERATIVE_SOCKET;
    }
#endif /* 0 */

    int rc = bind(sd, (sockaddr*) &srvAddr, sizeof(srvAddr));
    if (rc != 0) {
        success = false;
        socketClose(sd);
        return INOPERATIVE_SOCKET;
    }

    rc = listen(sd, backlog);
    if (rc != 0) {
        success = false;
        socketClose(sd);
        return INOPERATIVE_SOCKET;
    }

    return sd;
}


/**
 * Son of a bitch HP/UX doesn't facilitate blocking I/O
 * with snd/rcv timeouts (admitted, old Solaris is the same crap),
 * so we have to do everything with non-blocking sockets and either
 * select() or poll().
 *
 * Choosing select() for the moment for compatibility with
 * older Win (pre-Vista) versions that don't have WSAPoll().
 *
 * TODO: Reconsider this decision; maybe switch to the (hopefully)
 * simpler poll() approach since XP / Server 2003 isn't really
 * on our list of to-be-supported systems.
 */
bool socketSetNonBlocking(SOCKET_T sd) {
#if defined (_WIN64) || defined (_WIN32)

    unsigned long nonblocking = 1;
    const int rc = ioctlsocket(sd, FIONBIO, &nonblocking);
    return rc == 0;

#else /* Unix / Linux */

    int flags = fcntl(sd, F_GETFL);
    if (flags < 0) {
        return false;
    }
    const int rc = fcntl(sd, F_SETFL, flags | O_NONBLOCK);
    return rc == 0;

#endif /* (_WIN64) || (_WIN32) */
}


SOCKET_T socketAccept(SOCKET_T listenSd, unsigned long timeoutMillis, std::string& serverIP) {

    socklen_t addressSize = sizeof(struct sockaddr_storage);
    struct sockaddr_storage clientAddr = {};
    char ownIP[INET6_ADDRSTRLEN] = {}; // support for IPv6 if it'll become necessary

    fd_set readSet = {};
    fd_set exceptSet = {};

    struct timeval tv;
    millis2timeval(tv, timeoutMillis);

    int _lastErrno = 0;
    int status = 0;

    do {
        // (re-)start the madness ...
        FD_ZERO(&readSet);
        FD_ZERO(&exceptSet);
        FD_SET(listenSd, &readSet);
        FD_SET(listenSd, &exceptSet);

        status = select(listenSd + 1, &readSet, NULL, &exceptSet, &tv);
        if (status < 0) {
            _lastErrno = socketGetLastError();
            if (_lastErrno != _E_INTR) {
                // error in select()
                return INOPERATIVE_SOCKET;
            }
        } else if (status == 0) {
            // timed out
            return INOPERATIVE_SOCKET;
        } else if (FD_ISSET(listenSd, &readSet)) {
            // connect request by client

            SOCKET_T client = accept(listenSd, (sockaddr*) &clientAddr, &addressSize);
            if (client != INOPERATIVE_SOCKET) {
                // determine own IP address as seen from connected client
                struct sockaddr_storage serverAddr = {}; // allow for IPv6
                int rc = getsockname(client, (sockaddr*) &serverAddr, &addressSize);
                if (rc == 0) {
                    rc = getnameinfo((const sockaddr*) &serverAddr, addressSize, ownIP, sizeof(ownIP), NULL, 0, NI_NUMERICHOST);
                    if (rc == 0) {
                        serverIP.assign(ownIP);
                    }
                }

                return client;
            } else {
                _lastErrno = socketGetLastError();
                status = -1;
            } // client == INOPERATIVE_SOCKET

        } else {
            // unexpected (maybe listenSd is in exceptSet or some other error)
            return INOPERATIVE_SOCKET;
        } // !FD_ISSET(listenSd, &readSet)
    } while (status < 0 && (_lastErrno == _E_INTR || _lastErrno == _E_WOULDBLOCK || _lastErrno == _E_AGAIN));

    return INOPERATIVE_SOCKET;
}


std::vector<unsigned char>* socketReadUntilEOF(SOCKET_T sd, unsigned long timeoutMillis, bool& timedOut, bool& errorOccurred) {

    timedOut = false;
    errorOccurred = false;

    const int bufSize = 2048; // generally quite small, but more than enough for our average request size
    char buf[bufSize] = {};

    std::vector<unsigned char>* vec = new std::vector<unsigned char>();
    vec->reserve(bufSize);

    fd_set readSet = {};
    fd_set exceptSet = {};

    struct timeval tv;
    millis2timeval(tv, timeoutMillis);

    bool exhausted = false;
    const unsigned long start = currentTimeMillis();

    while (!exhausted) {
        int numBytesRead = 0;
        int _lastErrno = 0;
        int status = 0;

        do {
            FD_ZERO(&readSet);
            FD_ZERO(&exceptSet);

            FD_SET(sd, &readSet);
            FD_SET(sd, &exceptSet);
            _lastErrno = 0;

            status = select(sd + 1, &readSet, NULL, &exceptSet, &tv);
            if (status < 0) {
                _lastErrno = socketGetLastError();
                if (_lastErrno == _E_INTR) {
                    // try again
                    const unsigned long end = currentTimeMillis();
                    const unsigned long newTimeout = timeoutMillis - (end - start);
                    millis2timeval(tv, newTimeout);
                    numBytesRead = -1;
                } else {
                    // error in select()
                    errorOccurred = true;
                    return vec;
                }
            } else if (status == 0) {
                // timed out
                timedOut = true;
                return vec;
            } else if (FD_ISSET(sd, &readSet)) {
                // we can read
                numBytesRead = recv(sd, buf, bufSize, 0);
                _lastErrno = socketGetLastError();
            } else {
                // unexpected (maybe sd is in exceptSet or some other error)
                errorOccurred = true;
                return vec;
            }
        } while (numBytesRead < 0 && (_lastErrno == _E_INTR || _lastErrno == _E_WOULDBLOCK || _lastErrno == _E_AGAIN));

        if (numBytesRead < 0) {
            // error in recv()
            errorOccurred = true;
            exhausted = true;
        } else if (numBytesRead == 0) {
            // EOF (socket closed or shutdown output)
            exhausted = true;
        } else {
            vec->insert(vec->end(), buf, buf + numBytesRead);
        }

        if (!exhausted) {
            const unsigned long end = currentTimeMillis();
            const unsigned long newTimeout = timeoutMillis - (end - start);
            millis2timeval(tv, newTimeout);
        }
    } // while (!exhausted)

    return vec;
}


#if 0
/* This is just like the Unix read() system call, except that it will make
   sure that up to bufSize bytes goes through the socket (if there are that many). */
static int
socketReadUntilEOFBlocking(SOCKET_T sd, char* buf, size_t bufSize) {

    size_t bytesReadInTotal = 0;

    while (bytesReadInTotal < bufSize) {
        int numBytesReadThisTime = 0;

        do {
            numBytesReadThisTime = recv(sd, buf, (bufSize - bytesReadInTotal), 0);
        } while ( (numBytesReadThisTime < 0) && (socketGetLastError() == _E_INTR ) );

        if (numBytesReadThisTime < 0) {
            return numBytesReadThisTime;
        } else if (numBytesReadThisTime == 0) {
            return bytesReadInTotal;
        }

        bytesReadInTotal += numBytesReadThisTime;
        buf += numBytesReadThisTime;
    } // while (bytesReadInTotal < bufSize)

    return bufSize;
}


/* This is just like the Unix write() system call, except that it will
   make sure that all data is transmitted. */
static int
socketWriteAllBlocking(SOCKET_T sd, const char* buf, size_t count)
{
    size_t bytesSentInTotal = 0;

    // suppress SIGPIPE
    SuppressSigPipe noSigPipe;

    while (bytesSentInTotal < count) {
        int numBytesSentThisTime = 0;

        do {
            numBytesSentThisTime = send(sd, buf, count - bytesSentInTotal, 0);
        } while ( (numBytesSentThisTime < 0) && (socketGetLastError() == _E_INTR) );

        if (numBytesSentThisTime < 0) {
            // error in send()
            return numBytesSentThisTime;
        } else if (numBytesSentThisTime == 0) {
            // zero bytes sent should never happen on POSIX
            // systems (unless count is zero), but one never knows
            if (bytesSentInTotal == count) {
                break;
            }
        }

        bytesSentInTotal += numBytesSentThisTime;
        buf += numBytesSentThisTime;
    } // while (bytesSentInTotal < count)

    return count;
}
#endif /*0*/


bool socketWriteAll(SOCKET_T sd, const unsigned char* buf, size_t count, unsigned long timeoutMillis, bool& timedOut) {
    timedOut = false;

    if (count == 0) {
        return true;
    }

    size_t bytesSentInTotal = 0;

    fd_set writeSet = {};
    fd_set exceptSet = {};

    struct timeval tv;
    millis2timeval(tv, timeoutMillis);

    // suppress SIGPIPE
    SuppressSigPipe noSigPipe;

    const unsigned long start = currentTimeMillis();

    while (bytesSentInTotal < count) {
        int numBytesSent = 0;
        int _lastErrno = 0;
        int status = 0;

        do {
            FD_ZERO(&writeSet);
            FD_ZERO(&exceptSet);

            FD_SET(sd, &writeSet);
            FD_SET(sd, &exceptSet);
            _lastErrno = 0;

            status = select(sd + 1, NULL, &writeSet, &exceptSet, &tv);
            if (status < 0) {
                _lastErrno = socketGetLastError();
                if (_lastErrno == _E_INTR) {
                    // try again
                    const unsigned long end = currentTimeMillis();
                    const unsigned long newTimeout = timeoutMillis - (end - start);
                    millis2timeval(tv, newTimeout);
                    numBytesSent = -1;
                } else {
                    // error in select()
                    return false;
                }
            } else if (status == 0) {
                // timed out
                timedOut = true;
                return false;
            } else if (FD_ISSET(sd, &writeSet)) {
                // we can send
                numBytesSent = send(sd, (const char*) buf, count - bytesSentInTotal, 0);
                _lastErrno = socketGetLastError();
            } else {
                // unexpected (maybe sd is in exceptSet or some other error)
                return false;
            }
        } while (numBytesSent < 0 && (_lastErrno == _E_INTR || _lastErrno == _E_WOULDBLOCK || _lastErrno == _E_AGAIN));

        if (numBytesSent < 0) {
            // error in send()
            return false;
        } else if (numBytesSent == 0) {
            // zero bytes sent should never happen on POSIX
            // systems (unless count is zero), but one never knows
            if (bytesSentInTotal == count) {
                break;
            }
        }

        bytesSentInTotal += numBytesSent;
        buf += numBytesSent;

        if (bytesSentInTotal < count) {
            const unsigned long end = currentTimeMillis();
            const unsigned long newTimeout = timeoutMillis - (end - start);
            millis2timeval(tv, newTimeout);
        }
    } // while (bytesSentInTotal < count)

    return (bytesSentInTotal == count);
}


bool socketShutdownOutput(SOCKET_T sd) {

    return (shutdown(sd, _SHUT_WR) == 0);
}


bool socketRuntimeShutdown() {

#if defined (_WIN64) || defined (_WIN32)
    return (WSACleanup() == 0);
#else
    // no-op on non-Windows systems
    return true;
#endif
}


bool socketRuntimeInit() {

#if defined (_WIN64) || defined (_WIN32)
    WSADATA wsaData = {};
    return (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
#else
    // no-op on non-Windows systems
    return true;
#endif
}
