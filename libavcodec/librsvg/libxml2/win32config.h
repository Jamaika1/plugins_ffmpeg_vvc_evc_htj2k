#ifndef __LIBXML_WIN32_CONFIG__
#define __LIBXML_WIN32_CONFIG__

/* Avoid silly warnings about "insecure" functions. */
#define _CRT_SECURE_NO_DEPRECATE 1
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS  1
#endif
#define _CRT_NONSTDC_NO_WARNINGS 1


#define HAVE_SYS_TIMEB_H
#define HAVE_SYS_TIME_H
#define HAVE_FTIME
#define HAVE_SYS_STAT_H
#define HAVE_STAT
#define HAVE_FCNTL_H
#include <winsock2.h> /* struct timeval */
#include <windows.h>
#include <io.h>
#include <direct.h>

#if defined(__MINGW32__) || (defined(_MSC_VER) && _MSC_VER >= 1600)
  #define HAVE_STDINT_H
#endif

#if defined(_MSC_VER)
  #if _MSC_VER < 1900
    #define snprintf _snprintf
  #endif
  #if _MSC_VER < 1500
    #define vsnprintf(b,c,f,a) _vsnprintf(b,c,f,a)
  #endif
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "libxml2"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Version number of package */
#define VERSION "2.9.2"

#endif /* __LIBXML_WIN32_CONFIG__ */

