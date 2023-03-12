/* config.h.win32.in.  Manually Edited for Windows Builds.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

#define _GLIB_EXTERN extern
#define _GIO_EXTERN extern
#define _GMODULE_EXTERN extern
#define GLIB_DEPRECATED G_DEPRECATED _GLIB_EXTERN
#define GLIB_DEPRECATED_FOR(f) G_DEPRECATED_FOR(f) _GLIB_EXTERN
#define GLIB_VAR _GLIB_EXTERN
#define GOBJECT_VAR _GLIB_EXTERN
#define GLIB_VERSION_MAX_ALLOWED GLIB_VERSION_2_76
#define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_26
#define GOBJECT_VERSION_MAX_ALLOWED GOBJECT_VERSION_2_76
#define GOBJECT_VERSION_MIN_REQUIRED GOBJECT_VERSION_2_26
#define GIO_VERSION_MAX_ALLOWED GIO_VERSION_2_76
#define GIO_VERSION_MIN_REQUIRED GIO_VERSION_2_26
#define GMODULE_VERSION_MAX_ALLOWED GMODULE_VERSION_2_76
#define GMODULE_VERSION_MIN_REQUIRED GMODULE_VERSION_2_26
#define GLIB_COMPILATION
#define GIO_COMPILATION
#define GOBJECT_COMPILATION

#define ALIGNOF_GUINT32 4
#define ALIGNOF_GUINT64 8
#define ALIGNOF_UNSIGNED_LONG 8
#define HAVE__ALIGNED_MALLOC 1

#define XDG_PREFIX _gio_xdg
#define GLIB_RUNSTATEDIR ""
#define GIO_LAUNCH_DESKTOP ""
#define GDK_PIXBUF_BINARY_VERSION "2.10"
#define HAVE_ROUND 1
#define HAVE_LRINT 1
//#define GDK_PIXBUF_RELOCATABLE 1
#define GDK_PIXBUF_ENABLE_BACKEND 1
#define USE_GMODULE 1

#ifndef G_LIB_STATIC
#define G_LIB_EXPORT __declspec(dllexport)
#else
#define G_LIB_EXPORT extern
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#if !defined (_MSC_VER) || (_MSC_VER >= 1800)
//#define HAVE_INTTYPES_H 1
#endif

/* Define if your <locale.h> file defines LC_MESSAGES. */
/* #undef HAVE_LC_MESSAGES */

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Have the pangoft2 library */
/* #define HAVE_PANGOFT2 1 */

/* Define to 1 if you have the <stdint.h> header file. */
#if !defined (_MSC_VER) || (_MSC_VER >= 1600)
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#ifndef _MSC_VER
#define HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtok_r' function. */
#define HAVE_STRTOK_R 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef _MSC_VER
//#define HAVE_UNISTD_H 1
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "librsvg"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://bugzilla.gnome.org/enter_bug.cgi?product=librsvg"

/* Define to the full name of this package. */
#define PACKAGE_NAME "RSVG"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "RSVG 2.40.20"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "librsvg"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.40.20"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.40.20"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif

/*#if defined(_MSC_VER) && (_MSC_VER >= 1310)
#  define isnan(a) _isnan(a)
#else
#  define isnan(a) (a != a)
#endif*/
