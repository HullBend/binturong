

#ifndef __ITOA_PORTABLE_H__
#define __ITOA_PORTABLE_H__


#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */


__GCC_DONT_EXPORT char* itoa_portable(int val, char* buf);
__GCC_DONT_EXPORT char* ltoa_portable(long val, char* buf);
__GCC_DONT_EXPORT char* ultoa_portable(unsigned long val, char* buf);

__GCC_DONT_EXPORT char* itoa_portable(int val, char* buf, int base);
__GCC_DONT_EXPORT char* ltoa_portable(long val, char* buf, int base);
__GCC_DONT_EXPORT char* ultoa_portable(unsigned long val, char* buf, int base);


#endif /* __ITOA_PORTABLE_H__ */
