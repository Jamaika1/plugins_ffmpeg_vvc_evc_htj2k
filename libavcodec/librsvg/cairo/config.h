// #define HAVE_BYTESWAP_H 1
// #define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
// #define HAVE_SYS_INT_TYPES_H 1
//#define HAVE_UNISTD_H 1
// #define HAVE_UINT128_T 1
#define HAVE_UINT64_T 1

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
