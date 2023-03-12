/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if __attribute__((destructor)) is accepted */
#define ATTRIBUTE_DESTRUCTOR @ATTRIBUTE_DESTRUCTOR@

/* Define to 1 if you have the <arpa/inet.h> header file. */
//#define HAVE_ARPA_INET_H 0

/* Define to 1 if you have the <dlfcn.h> header file. */
//#define HAVE_DLFCN_H 1

/* Have dlopen based dso */
//#define HAVE_DLOPEN 0

/* Define to 1 if you have the <dl.h> header file. */
//#define HAVE_DL_H 0

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `ftime' function. */
//#define HAVE_FTIME 0

/* Define to 1 if you have the `gettimeofday' function. */
//#define HAVE_GETTIMEOFDAY 0

/* Define to 1 if you have the <inttypes.h> header file. */
//#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `isascii' function. */
//#define HAVE_ISASCII 0

/* Define if history library is there (-lhistory) */
//#define HAVE_LIBHISTORY 0

/* Define if readline library is there (-lreadline) */
//#define HAVE_LIBREADLINE 0

/* Define to 1 if you have the `mmap' function. */
//#define HAVE_MMAP 0

/* Define to 1 if you have the `munmap' function. */
//#define HAVE_MUNMAP 0

/* mmap() is no good without munmap() */
#if defined(HAVE_MMAP) && !defined(HAVE_MUNMAP)
#  undef /**/ HAVE_MMAP
#endif

/* Define to 1 if you have the <netdb.h> header file. */
//#define HAVE_NETDB_H 0

/* Define to 1 if you have the <netinet/in.h> header file. */
//#define HAVE_NETINET_IN_H 0

/* Define to 1 if you have the <poll.h> header file. */
//#define HAVE_POLL_H 0

/* Define if <pthread.h> is there */
//#define HAVE_PTHREAD_H 0

/* Define to 1 if you have the `putenv' function. */
//#define HAVE_PUTENV 0

/* Define to 1 if you have the `rand_r' function. */
//#define HAVE_RAND_R 0

/* Have shl_load based dso */
//#define HAVE_SHLLOAD 0

/* Define to 1 if you have the `stat' function. */
//#define HAVE_STAT 0

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
//#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
//#define HAVE_SYS_SELECT_H 0

/* Define to 1 if you have the <sys/socket.h> header file. */
//#define HAVE_SYS_SOCKET_H 0

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
//#define HAVE_SYS_TIME_H 0

/* Define to 1 if you have the <unistd.h> header file. */
//#define HAVE_UNISTD_H 0

/* Whether va_copy() is available */
#define HAVE_VA_COPY 1

/* Define to 1 if you have the <zlib.h> header file. */
#define LIBXML_ZLIB_ENABLED

/* Whether __va_copy() is available */
#define HAVE___VA_COPY 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
//define LT_OBJDIR "@LT_OBJDIR@"

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

/* Support for IPv6 */
#define SUPPORT_IP6 1

/* Define if va_list is an array type */
#define VA_LIST_IS_ARRAY 1

/* Version number of package */
#define VERSION ""

/* Determine what socket length (socklen_t) data type is */
#define XML_SOCKLEN_T int

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
//#define _UINT32_T @_UINT32_T@

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
//#define uint32_t @uint32_t@
