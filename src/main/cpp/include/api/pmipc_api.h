

#ifndef __PMI_PC_API_H__
#define __PMI_PC_API_H__



#if defined (_WIN64) || defined (_WIN32)


#ifdef PMI_PRCTRL_COMPILE
#   ifndef PMI_PRCTRL_API
#       define PMI_PRCTRL_API __declspec(dllexport)
#   endif /* !defined(PMI_PRCTRL_API) */
#else /* !defined(PMI_PRCTRL_COMPILE) */
#   ifndef PMI_PRCTRL_API
#       define PMI_PRCTRL_API __declspec(dllimport)
#   endif /* !defined(PMI_PRCTRL_API) */
#endif


#else /* non-Windows systems */


#ifndef PMI_PRCTRL_API
#   define PMI_PRCTRL_API extern
#endif /* !defined(PMI_PRCTRL_API) */


#endif /* (_WIN64) || (_WIN32) */



#endif /* __PMI_PC_API_H__ */
