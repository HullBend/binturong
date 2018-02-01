

#ifndef __BINTURONG_API_H__
#define __BINTURONG_API_H__



#if defined (_WIN64) || defined (_WIN32)


#ifdef BINTURONG_COMPILE
#   ifndef BINTURONG_API
#       define BINTURONG_API __declspec(dllexport)
#   endif /* !defined(BINTURONG_API) */
#else /* !defined(BINTURONG_COMPILE) */
#   ifndef BINTURONG_API
#       define BINTURONG_API __declspec(dllimport)
#   endif /* !defined(BINTURONG_API) */
#endif


#else /* non-Windows systems */


#ifndef BINTURONG_API
#   define BINTURONG_API extern
#endif /* !defined(BINTURONG_API) */


#endif /* (_WIN64) || (_WIN32) */



#endif /* __BINTURONG_API_H__ */
