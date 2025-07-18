/* config.h.win32.in Merged from two versions generated by configure for gcc and MSVC.  */
/* config.h.  Generated by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
*/

#include "config_win32.h"

#define XDG_PREFIX _gio_xdg
#define GLIB_RUNSTATEDIR "/usr64/local/glib/var/run"
#define GLIB_LOCALE_DIR "/usr64/local/glib/share/locale"
#define GDK_PIXBUF_LIBDIR "/usr64/local/gdk_pixbuf/lib"
#define GDK_PIXBUF_LOCALEDIR "/usr64/local/gdk_pixbuf/share/locale"
//#define G_LOG_DOMAIN "GLib-GIO"
#define GIO_LAUNCH_DESKTOP "/usr64/local/gio/share/applications"
#define GIO_MODULE_DIR ""
#define LOCALSTATEDIR "/usr64/local/gio/var"
#define GDK_PIXBUF_BINARY_VERSION "2.10"
#define HAVE_ROUND 1
#define HAVE_LRINT 1
//#define GDK_PIXBUF_RELOCATABLE 1
#define GDK_PIXBUF_ENABLE_BACKEND 1
#define GDK_PIXBUF_USE_GIO_MIME 1
#define USE_GMODULE 1
#define ENABLE_VALGRIND 1
//#define IN_LIBASPRINTF 1

#include "glibconfig.h"

/* #undef CRAY_STACKSEG_END */
/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */
/* Whether to disable memory pools */
/* #undef DISABLE_MEM_POOLS */
/* Whether to enable GC friendliness */
/* #undef ENABLE_GC_FRIENDLY */
/* always defined to indicate that i18n is enabled */
#define ENABLE_NLS 1
/* Define the gettext package to be used */
#define GETTEXT_PACKAGE "dummy"
/* Define to the GLIB binary age */
#define GLIB_BINARY_AGE 8501
/* Byte contents of gmutex */
/* #undef GLIB_BYTE_CONTENTS_GMUTEX */
/* Define to the GLIB interface age */
#define GLIB_INTERFACE_AGE 0

/* The size of gmutex, as computed by sizeof. */
/* #undef GLIB_SIZEOF_GMUTEX */
/* The size of system_thread, as computed by sizeof. */
#define GLIB_SIZEOF_SYSTEM_THREAD 4
/* Whether glib was compiled with debugging enabled */
#define G_COMPILED_WITH_DEBUGGING "yes"
/* Have inline keyword */
#define G_HAVE_INLINE 1
/* Have __inline keyword */
#define G_HAVE___INLINE 1
/* Have __inline__ keyword */
#define G_HAVE___INLINE__ 1
/* Source file containing theread implementation */
//#define G_THREAD_SOURCE "gthread-win32.c"
/* A 'va_copy' style function */
#define G_VA_COPY va_copy
/* 'va_lists' cannot be copies as values */
/* #undef G_VA_COPY_AS_ARRAY */
/* Define to 1 if you have `alloca', as a function or macro. */
/* #undef HAVE_ALLOCA 1 */
/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix). */
/* #undef HAVE_ALLOCA_H */
/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1
/* Define to 1 if you have the `bind_textdomain_codeset' function. */
#define HAVE_BIND_TEXTDOMAIN_CODESET 1
/* Define if you have a version of the snprintf function with semantics as
   specified by the ISO C99 standard. */
/* #undef HAVE_C99_SNPRINTF */
/* Define if you have a version of the vsnprintf function with semantics as
   specified by the ISO C99 standard. */
/* #undef HAVE_C99_VSNPRINTF */
/* Have nl_langinfo (CODESET) */
/* #undef HAVE_CODESET */
/* Define to 1 if you have the `dcgettext' function. */
#define HAVE_DCGETTEXT 1
/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1
/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */
/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */
/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1
/* Define to 1 if you have the `getcwd' function. */
/* #undef HAVE_GETCWD */
/* Define to 1 if you have the `getc_unlocked' function. */
/* #undef HAVE_GETC_UNLOCKED */
/* Define if the GNU gettext() function is already present or preinstalled. */
#define HAVE_GETTEXT 1
/* define to use system printf */
/* #undef HAVE_GOOD_PRINTF */
/* define to support printing 64-bit integers with format I64 */
#define HAVE_INT64_AND_I64 1
/* Define if you have the 'intmax_t' type in <stdint.h> or <inttypes.h>. */
#define HAVE_INTMAX_T 1
/* Define to 1 if you have the <inttypes.h> header file. */
//#define HAVE_INTTYPES_H 1
/* Define if <inttypes.h> exists, doesn't clash with <sys/types.h>, and
   declares uintmax_t. */
#define HAVE_INTTYPES_H_WITH_UINTMAX 1
/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
/* #undef HAVE_LANGINFO_CODESET */
/* Define to 1 if you have the <langinfo.h> header file. */
/* #undef HAVE_LANGINFO_H */
/* Define if your <locale.h> file defines LC_MESSAGES. */
/* #undef HAVE_LC_MESSAGES */
/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1
/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1
/* Define to 1 if you have the `localtime_r' function. */
/* #undef HAVE_LOCALTIME_R */
/* Define if you have the 'long double' type. */
#define HAVE_LONG_DOUBLE 1
/* Define if you have the 'long long' type. */
#define HAVE_LONG_LONG 1
/* define if system printf can print long long */
#define HAVE_LONG_LONG_FORMAT 1
/* Define to 1 if you have the `lstat' function. */
/* #undef HAVE_LSTAT */
/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1
/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H
/* Define to 1 if you have the `mkstemp' function. */
/* #undef HAVE_MKSTEMP */
/* Define to 1 if you have the `nanosleep' function. */
/* #undef HAVE_NANOSLEEP */
/* Define to 1 if you have the `nl_langinfo' function. */
/* #undef HAVE_NL_LANGINFO */
/* Have non-POSIX function getpwuid_r */
/* #undef HAVE_NONPOSIX_GETPWUID_R */
/* Define to 1 if you have the `on_exit' function. */
/* #undef HAVE_ON_EXIT */
/* Define to 1 if you have the `poll' function. */
/* #undef HAVE_POLL */
/* Have POSIX function getpwuid_r */
/* #undef HAVE_POSIX_GETPWUID_R */
/* Have function pthread_attr_setstacksize */
/* #undef HAVE_PTHREAD_ATTR_SETSTACKSIZE */
/* Have function pthread_cond_timedwait_relative_np */
#undef HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP
/* Have function pthread_condattr_setclock */
#define HAVE_PTHREAD_CONDATTR_SETCLOCK 1
#define CLOCK_MONOTONIC 1
/* Have function pthread_setname */
#define HAVE_PTHREAD_SETNAME_NP_WITH_TID 1
/* Define to 1 if the system has the type `ptrdiff_t'. */
#define HAVE_PTRDIFF_T 1
/* Define to 1 if you have the <pwd.h> header file. */
/* #undef HAVE_PWD_H */
/* Define to 1 if you have the `readlink' function. */
/* #undef HAVE_READLINK */
/* Define to 1 if you have the <sched.h> header file. */
/* #undef HAVE_SCHED_H */
/* Define to 1 if you have the `setenv' function. */
/* #undef HAVE_SETENV */
/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1
/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1
#define HAVE_STRNLEN 1
#ifdef __DMC__
#define snprintf _snprintf
#endif
/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1
/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1
/* Define if <stdint.h> exists, doesn't clash with <sys/types.h>, and declares
   uintmax_t. */
#undef HAVE_STDINT_H_WITH_UINTMAX
/* Define to 1 if you have the `stpcpy' function. */
/* #undef HAVE_STPCPY */
/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1
/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1
/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */
/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1
/* Have functions strlcpy and strlcat */
/* #undef HAVE_STRLCPY */
/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1
/* Define to 1 if you have the `strsignal' function. */
/* #undef HAVE_STRSIGNAL */
/* Define to 1 if you have the `symlink' function. */
/* #undef HAVE_SYMLINK */
/* Define to 1 if you have the <sys/param.h> header file. */
/* #undef HAVE_SYS_PARAM_H */
/* Define to 1 if you have the <sys/poll.h> header file. */
/* #undef HAVE_SYS_POLL_H */
/* found fd_set in sys/select.h */
/* #undef HAVE_SYS_SELECT_H */
/* Define to 1 if you have the <sys/stat.h> header file. */
/* #undef HAVE_SYS_STAT_H */
/* Define to 1 if you have the <sys/times.h> header file. */
/* #undef HAVE_SYS_TIMES_H */
/* Define to 1 if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */
/* Define to 1 if you have the <sys/types.h> header file. */
/* #undef HAVE_SYS_TYPES_H */
/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */
/* Define if your printf function family supports positional parameters as
   specified by Unix98. */
/* #undef HAVE_UNIX98_PRINTF */
/* Define to 1 if you have the `unsetenv' function. */
/* #undef HAVE_UNSETENV */
/* Define to 1 if you have the <values.h> header file. */
#define HAVE_VALUES_H 1
/* Define to 1 if you have the `vasprintf' function. */
#undef HAVE_VASPRINTF
/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1
/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1
#ifdef __DMC__
#define vsnprintf _vsnprintf
#endif
/* Define to 1 if you have the 'wcslen' function. */
#define HAVE_WCSLEN 1
/* Define if you have the 'wchar_t' type. */
#define HAVE_WCHAR_T 1
/* Define if you have the 'wint_t' type. */
#define HAVE_WINT_T 1
/* Have a working bcopy */
/* #undef HAVE_WORKING_BCOPY */
/* didn't find fd_set */
#define NO_FD_SET 1
/* global 'sys_errlist' not found */
#define NO_SYS_ERRLIST 1
/* global 'sys_siglist' not found */
#define NO_SYS_SIGLIST 1
/* global 'sys_siglist' not declared */
#define NO_SYS_SIGLIST_DECL 1
/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=glib"
/* Define to the full name of this package. */
#define PACKAGE_NAME "glib"
/* Define to the full name and version of this package. */
#define PACKAGE_STRING "glib 2.85.1-25a1768"
/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "glib"
/* Define to the version of this package. */
#define PACKAGE_VERSION "2.85.1-25a1768"
/* Maximum POSIX RT priority */
/* #undef POSIX_MAX_PRIORITY */
/* Minimum POSIX RT priority */
/* #undef POSIX_MIN_PRIORITY */
/* The POSIX RT yield function */
/* #undef POSIX_YIELD_FUNC */
/* whether realloc (NULL,) works */
#define REALLOC_0_WORKS 1
/* Define if you have correct malloc prototypes */
#define SANE_MALLOC_PROTOS 1
/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */
/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
/* Using GNU libiconv */
//#define USE_LIBICONV_GNU 1
/* Using a native implementation of iconv in a separate library */
/* #undef USE_LIBICONV_NATIVE */
/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */
/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef __FILE_OFFSET_BITS */
/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */
/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
/* Define to long or long long if <inttypes.h> and <stdint.h> don't define. */
/* #undef intmax_t */
/* Define to empty if the C compiler doesn't support this keyword. */
/* #undef signed */
/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

#if ((__GNUC__ \
      ? defined __GNUC_STDC_INLINE__ && __GNUC_STDC_INLINE__ \
      : (199901L <= __STDC_VERSION__ \
         && !defined __HP_cc \
         && !(defined __SUNPRO_C && __STDC__))) \
     && !defined __APPLE__)
# define _GL_INLINE inline
# define _GL_EXTERN_INLINE extern inline
#elif (2 < __GNUC__ + (7 <= __GNUC_MINOR__) && !defined __STRICT_ANSI__ \
       && !defined __APPLE__)
# if __GNUC_GNU_INLINE__
   /* __gnu_inline__ suppresses a GCC 4.2 diagnostic.  */
#  define _GL_INLINE extern inline __attribute__ ((__gnu_inline__))
# else
#  define _GL_INLINE extern inline
# endif
# define _GL_EXTERN_INLINE extern
#else
# define _GL_INLINE static _GL_UNUSED
# define _GL_EXTERN_INLINE static _GL_UNUSED
#endif
#if 4 < __GNUC__ + (6 <= __GNUC_MINOR__)
# if defined __GNUC_STDC_INLINE__ && __GNUC_STDC_INLINE__
#  define _GL_INLINE_HEADER_CONST_PRAGMA
# else
#  define _GL_INLINE_HEADER_CONST_PRAGMA \
     _Pragma ("GCC diagnostic ignored \"-Wsuggest-attribute=const\"")
# endif
  /* Suppress GCC's bogus "no previous prototype for 'FOO'"
     and "no previous declaration for 'FOO'"  diagnostics,
     when FOO is an inline function in the header; see
     <http://gcc.gnu.org/bugzilla/show_bug.cgi?id=54113>.  */
# define _GL_INLINE_HEADER_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wmissing-prototypes\"") \
    _Pragma ("GCC diagnostic ignored \"-Wmissing-declarations\"") \
    _GL_INLINE_HEADER_CONST_PRAGMA
# define _GL_INLINE_HEADER_END \
    _Pragma ("GCC diagnostic pop")
#else
# define _GL_INLINE_HEADER_BEGIN
# define _GL_INLINE_HEADER_END
#endif


