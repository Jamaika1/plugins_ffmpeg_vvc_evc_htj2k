#define FLEXIBLE_ARRAY_MEMBER 1
#define SIZEOF_VOID_P 8
#define ALIGNOF_DOUBLE 8
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
//#define HAVE_SYS_PARAM_H 0
//#define HAVE_DIRENT_H 1
#define HAVE_FCNTL_H 1
#define HAVE_MKSTEMP 1
#define HAVE_RAND 1
#define ENABLE_LIBXML2 1
#define FC_GPERF_SIZE_T unsigned int
#define FC_DEFAULT_FONTS "/usr/share/fonts"
#define FC_FONTPATH "/etc/fonts"
#define FC_CACHEDIR "/var/cache/fontconfig "
#define FC_TEMPLATEDIR "/usr/share/fontconfig/conf.avail"
#define CONFIGDIR "/etc/fonts/conf.d"

#define FT_FREETYPE_H <freetype/freetype.h>
#define FT_ADVANCES_H <freetype/ftadvanc.h>
#define FT_TRUETYPE_TABLES_H <freetype/tttables.h>
#define FT_SFNT_NAMES_H <freetype/ftsnames.h>
#define FT_TRUETYPE_IDS_H <freetype/ttnameid.h>
#define FT_TYPE1_TABLES_H <freetype/t1tables.h>
#if HAVE_FT_GET_X11_FONT_FORMAT
#define FT_XFREE86_H <freetype/ftfntfmt.h>
#endif
#if HAVE_FT_GET_BDF_PROPERTY
#define FT_BDF_H <freetype/ftbdf.h>
#define FT_MODULE_H <freetype/ftmodapi.h>
#endif
#define FT_MULTIPLE_MASTERS_H <freetype/ftmm.h>

