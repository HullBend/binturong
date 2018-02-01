

#ifndef __SUPPRESS_SIGPIPE_H__
#define __SUPPRESS_SIGPIPE_H__


#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */


#if !defined (_WIN64) && !defined (_WIN32)
#include <signal.h>
#endif



/**
 * Poor old HP/UX neither has the SO_NOSIGPIPE socket option,
 * nor does it recognize the MSG_NOSIGNAL flag in send() and
 * relatives.
 *
 * So we have to do it the hard way (unfortunately).
 * What a freaking OS ...
 */
class __GCC_DONT_EXPORT SuppressSigPipe {

public:
    SuppressSigPipe();
    ~SuppressSigPipe();

private:
    void suppress();
    void restore();

private:
    // Prohibit heap allocation (must be stack allocated)
    // Note: these operators shouldn't have any implementation,
    // but we can't manage to link a loadable sl with weird
    // aCC on HP/UX without an implementation?!
    void* operator new(size_t) { throw 0; }
    void operator delete(void*, size_t) { throw 0; }
    void* operator new[](size_t) { throw 0; }
    void operator delete[](void*, size_t) { throw 0; }

    SuppressSigPipe(const SuppressSigPipe&); // copy ctor
    SuppressSigPipe& operator= (const SuppressSigPipe&); // assignment op

private: // data

#if !defined (_WIN64) && !defined (_WIN32)
    sigset_t sigpipe_mask;
#endif

    bool sigpipe_pending;
    bool sigpipe_unblock;
};


#endif /* __SUPPRESS_SIGPIPE_H__ */
