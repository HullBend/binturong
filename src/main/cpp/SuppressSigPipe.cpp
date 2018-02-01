

#include "SuppressSigPipe.h"


#if !defined (_WIN64) && !defined (_WIN32)
#include <errno.h>
#include <pthread.h>
#endif



//
// See:
//
// http://krokisplace.blogspot.de/2010/02/suppressing-sigpipe-in-library.html
//
// for an explanation of this approach
//



SuppressSigPipe::SuppressSigPipe()
    : sigpipe_pending(false),
    sigpipe_unblock(false)
{
#if !defined (_WIN64) && !defined (_WIN32)
    sigemptyset(&sigpipe_mask);
    sigaddset(&sigpipe_mask, SIGPIPE);
    suppress();
#endif
}


SuppressSigPipe::~SuppressSigPipe()
{
    restore();
}




inline
void SuppressSigPipe::suppress()
{
#if !defined (_WIN64) && !defined (_WIN32)
    /*
      We want to ignore possible SIGPIPE that we can generate on write.
      SIGPIPE is delivered *synchronously* and *only* to the thread
      doing the write. So if it is reported as already pending (which
      means the thread blocks it), then we do nothing. If we generate
      SIGPIPE, it will be merged with the pending one (there's no
      queueing), and that suits us well. If it is not pending, we block
      it in this thread (and we avoid changing signal action, because it
      is per-process).
    */
    sigset_t pending;
    sigpending(&pending);
    sigpipe_pending = sigismember(&pending, SIGPIPE);
    if (!sigpipe_pending) {
        sigset_t blocked;
        pthread_sigmask(SIG_BLOCK, &sigpipe_mask, &blocked);
        /* Maybe it was blocked already ? */
        sigpipe_unblock = !sigismember(&blocked, SIGPIPE);
    }
#endif
}


inline
void SuppressSigPipe::restore()
{
#if !defined (_WIN64) && !defined (_WIN32)
    /*
      If SIGPIPE was pending already we do nothing. Otherwise, if it
      became pending (i.e., we generated it), then we sigwait() it (thus
      clearing pending status). Then we unblock SIGPIPE, but only if it
      were us who blocked it.
    */
    if (!sigpipe_pending) {
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGPIPE)) {
            /*
              Protect ourselves from a situation when SIGPIPE was sent
              by the user to the whole process, and was delivered to
              other thread before we had a chance to wait for it.
            */
            static const struct timespec nowait = {0, 0};
            int rc = 0;
            do {
                rc = sigtimedwait(&sigpipe_mask, NULL, &nowait);
                //int signalNo = 0;
                //rc = sigwait(&sigpipe_mask, &signalNo);
            } while (rc == -1 && errno == EINTR);
        }
        if (sigpipe_unblock) {
            pthread_sigmask(SIG_UNBLOCK, &sigpipe_mask, NULL);
        }
    }
#endif
}
