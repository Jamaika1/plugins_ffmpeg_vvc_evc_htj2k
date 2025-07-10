#define HAVE_VALGRIND 1
// #define HAVE_BYTESWAP_H 1
#define HAVE_INTTYPES_H 1
//#define HAVE_STDINT_H 1
// #define HAVE_SYS_INT_TYPES_H 1
//#define HAVE_UNISTD_H 1
// #define HAVE_UINT128_T 1
#define HAVE_UINT64_T 1

#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX

#define SIZEOF_INT 4
/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4
/* The size of a `long long', as computed by sizeof. */
#ifndef _MSC_VER
#define SIZEOF_LONG_LONG 8
#else /* _MSC_VER */
#define SIZEOF_LONG_LONG 0
#endif /* _MSC_VER */
#define SIZEOF_VOID_P 8
/* The size of a `__int64', as computed by sizeof. */

//#define HAVE_GCC_LEGACY_ATOMICS 1
//#define HAVE_FT_COLR_V1 1
//#define HAVE_FT_SVG_DOCUMENT 1
//#define HAVE_FT_LOAD_NO_SVG 1

//#define HAVE_FT_GET_VAR_DESIGN_COORDINATES 1
//#define HAVE_FT_LIBRARY_SETLCDFILTER 1
//#define HAVE_FT_PALETTE_SELECT 1
//#define HAVE_FT_LOAD_SFNT_TABLE 1
//#define FT_LOAD_COLOR 1
//#define FC_HINT_STYLE "hintstyle"
//#define PIXMAN_HAS_OP_LERP 1
#define HAS_PIXMAN_GLYPHS 1
#define HAS_PIXMAN_r8g8b8_sRGB 1
//#define PIXMAN_HAS_COMPOSITOR 1
//#define PIXMAN_HAS_ATOMIC_OPS 1
#define PIXMAN_TYPE_BGRA 8
#define PNG_SETJMP_SUPPORTED
