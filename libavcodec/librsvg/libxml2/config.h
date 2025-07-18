#ifndef __LIBXML_WIN32_CONFIG__
#define __LIBXML_WIN32_CONFIG__

/* Avoid silly warnings about "insecure" functions. */
#define _CRT_SECURE_NO_DEPRECATE 1
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS  1
#endif
#define _CRT_NONSTDC_NO_WARNINGS 1


//#define HAVE_SYS_TIMEB_H
//#define HAVE_SYS_TIME_H
//#define HAVE_FTIME

#include <winsock2.h> /* struct timeval */
#include <windows.h>
#include <io.h>
#include <direct.h>

#if defined(_MSC_VER) && _MSC_VER >= 1600
  //#define HAVE_STDINT_H 1
#endif

#define NO_LARGEFILE_SOURCE 1

#if defined(_MSC_VER)
  /*#if _MSC_VER < 1900
    #define snprintf _snprintf
  #endif
  #if _MSC_VER < 1500
    #define vsnprintf(b,c,f,a) _vsnprintf(b,c,f,a)
  #endif*/
#endif

#define XML_SYSCONFDIR "/etc"

#endif /* __LIBXML_WIN32_CONFIG__ */

