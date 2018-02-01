

#include "currentTimeMillis.h"

#include <time.h>       // gettimeofday


#if defined (_WIN64) || defined (_WIN32)

#include "winsock2.h" // for struct timeval (It's weird, I know)

#if defined (_MSC_VER) || defined (_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif /* (_MSC_VER) || (_MSC_EXTENSIONS) */

#else /* Unix or Linux */

#include <sys/time.h>       // gettimeofday

#endif /* (_WIN64) || (_WIN32) */



static int timeOfDay(struct timeval* tv) {

#if !defined (_WIN64) && !defined (_WIN32)
    return gettimeofday(tv, NULL);
#else

    if (tv) {
        FILETIME ft;
        unsigned __int64 tmpres = 0;

        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        // convert file time to unix epoch
        tmpres /= 10;  // convert into microseconds
        tmpres -= DELTA_EPOCH_IN_MICROSECS;

        tv->tv_sec = (long) (tmpres / 1000000UL);
        tv->tv_usec = (long) (tmpres % 1000000UL);
    }

    return 0;

#endif

}

static unsigned long timeval2millis(struct timeval& tv) {
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

unsigned long currentTimeMillis() {
    struct timeval tv = {};
    timeOfDay(&tv);
    return timeval2millis(tv);
}
