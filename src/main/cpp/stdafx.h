/* ---------------------------------------------------------------------- */
/* stdafx.h :                                                             */
/* Include file for standard system include files, or project specific    */
/* include files that are used frequently, but are changed infrequently.  */
/* ---------------------------------------------------------------------- */

#ifndef STDAFX_INCLUDED_
#define STDAFX_INCLUDED_



#ifndef __BINTURONG_API_H__
#include "binturong_api.h"      // #define BINTURONG_API
#endif /* __BINTURONG_API_H__ */



/* common for all OS */
#if defined(BINTURONG_COMPILE) && defined(__cplusplus)

// STL string and STL vector are used often enough,
// thus globally #include both headers here

#include <string>
#include <vector>

#endif /* common for all OS */




#if defined (_WIN64) || defined (_WIN32)

/* Specifies that the minimum required platform is Windows Server 2003 SP1 or Windows XP SP2 */
#ifndef WINVER
#   define WINVER 0x0502
#endif

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0502
#endif

#ifndef _WIN32_WINDOWS
#   define _WIN32_WINDOWS 0x0502
#endif

/* Specifies that the minimum required IE platform is IE 6.0 SP2 */
#ifndef _WIN32_IE
#   define _WIN32_IE 0x0603
#endif



/* Exclude rarely-used stuff from Windows headers */
#define WIN32_LEAN_AND_MEAN

/* Windows Header File(s) */
#include <windows.h>


/* Expand the g++ shared library "hidden" attribute to nothing */
#ifndef __GCC_DONT_EXPORT
#   define __GCC_DONT_EXPORT
#endif /* !defined(__GCC_DONT_EXPORT) */



#else /* Unix or Linux */



#if defined (__linux) && defined (__GNUG__)

#ifndef __GCC_DONT_EXPORT
#   define __GCC_DONT_EXPORT __attribute ((visibility ("hidden")))
#endif /* !defined(__GCC_DONT_EXPORT) */


#else /* Unix or non-g++ Linux */



/* Expand the g++ shared library "hidden" attribute to nothing */
#ifndef __GCC_DONT_EXPORT
#   define __GCC_DONT_EXPORT
#endif /* !defined(__GCC_DONT_EXPORT) */



//#error "This is Unix (or Linux with non-g++)!"
//#error "You should add your standard includes in stdafx.h or remove this error macro line in stdafx.h"



#endif /* (__linux) && (__GNUG__) */


#endif /* WIN32/64 versus Unix/Linux */


#endif /* STDAFX_INCLUDED_ */
