
#ifndef __THREAD_ID_INCLUDED__
#define __THREAD_ID_INCLUDED__


#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */



__GCC_DONT_EXPORT const std::string getCurrentThreadId_portable();


__GCC_DONT_EXPORT long getCallingProcessId_portable();


#endif /* __THREAD_ID_INCLUDED__ */

