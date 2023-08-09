/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
/* Actually, this version of config.h is manually edited from the above */

#define _GLIB_EXTERN extern
#define _GIO_EXTERN extern
#define _GMODULE_EXTERN extern
#define GLIB_DEPRECATED G_DEPRECATED _GLIB_EXTERN
#define GLIB_DEPRECATED_FOR(f) G_DEPRECATED_FOR(f) _GLIB_EXTERN
#define GLIB_VAR _GLIB_EXTERN
#define GOBJECT_VAR _GLIB_EXTERN
#define GLIB_VERSION_MAX_ALLOWED GLIB_VERSION_2_78
#define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_26
#define GOBJECT_VERSION_MAX_ALLOWED GOBJECT_VERSION_2_78
#define GOBJECT_VERSION_MIN_REQUIRED GOBJECT_VERSION_2_26
#define GIO_VERSION_MAX_ALLOWED GIO_VERSION_2_78
#define GIO_VERSION_MIN_REQUIRED GIO_VERSION_2_26
#define GMODULE_VERSION_MAX_ALLOWED GMODULE_VERSION_2_78
#define GMODULE_VERSION_MIN_REQUIRED GMODULE_VERSION_2_26
#define GLIB_COMPILATION
#define GIO_COMPILATION
#define GOBJECT_COMPILATION
//#define G_DISABLE_ASSERT
//#define G_DISABLE_CHECKS
#define USE_SYSTEM_PRINTF

#define ALIGNOF_GUINT32 4
#define ALIGNOF_GUINT64 8
#define ALIGNOF_UNSIGNED_LONG 8
#define HAVE__ALIGNED_MALLOC 1

#define XDG_PREFIX _gio_xdg
#define GLIB_RUNSTATEDIR "/run/glib"
#define GIO_LAUNCH_DESKTOP "/gio/gio-launch-desktop"
#define GDK_PIXBUF_BINARY_VERSION "2.10"
#define HAVE_ROUND 1
#define HAVE_LRINT 1
//#define GDK_PIXBUF_RELOCATABLE 1
#define GDK_PIXBUF_ENABLE_BACKEND 1
#define USE_GMODULE 1
//#define __GI_SCANNER__ 1
//#define PANGO_DISABLE_DEPRECATED 1

#ifndef G_LIB_STATIC
#define G_LIB_EXPORT __declspec(dllexport)
#else
#define G_LIB_EXPORT extern
#endif

/* Have usable Cairo library and font backend */
#define HAVE_CAIRO 1

/* Whether Cairo can use FreeType for fonts */
#ifndef PANGO_VISUALC_NO_FC
//#define HAVE_CAIRO_FREETYPE 1
#endif

/* Whether Cairo has PDF support */
#define HAVE_CAIRO_PDF 1

/* Whether Cairo has PNG support */
#define HAVE_CAIRO_PNG 1

/* Whether Cairo has PS support */
#define HAVE_CAIRO_PS 1

/* Whether Cairo can use Quartz for fonts */
/* #undef HAVE_CAIRO_QUARTZ */

/* Whether Cairo can use the Win32 GDI for fonts */
#define HAVE_CAIRO_WIN32 1

/* Whether Cairo has Xlib support */
//#define HAVE_CAIRO_XLIB 1

/* Whether CoreText is available on the system */
/* #undef HAVE_CORE_TEXT */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the `flockfile' function. */
/* #undef HAVE_FLOCKFILE */

/* Have FreeType 2 library */
#ifndef PANGO_VISUALC_NO_FC
//#define HAVE_FREETYPE 1
#endif

/* Define to 1 if you have the `getpagesize' function. */
/* #undef HAVE_GETPAGESIZE */

/* Define to 1 if you have the <inttypes.h> header file. */
/*#undef HAVE_INTTYPES_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <stdint.h> header file. */
#if (!defined (_MSC_VER) || (_MSC_VER >= 1600))
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtok_r' function. */
#define HAVE_STRTOK_R 1

/* Define to 1 if you have the `sysconf' function. */
/* #undef HAVE_SYSCONF */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
/* #undef HAVE_SYS_MMAN_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Have Xft library */
/* #undef HAVE_XFT */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "pango"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=pango"

/* Define to the full name of this package. */
#define PACKAGE_NAME "pango"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "pango "

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "pango"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* PANGO binary age */
#define PANGO_BINARY_AGE 5100

/* PANGO interface age */
#define PANGO_INTERFACE_AGE

/* PANGO major version */
#define PANGO_VERSION_MAJOR 1

/* PANGO micro version */
#define PANGO_VERSION_MICRO 0

/* PANGO minor version */
#define PANGO_VERSION_MINOR 51

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.51.0"
