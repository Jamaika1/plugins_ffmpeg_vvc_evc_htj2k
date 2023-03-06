/* create opj_config_private.h for CMake */
//#define OPJ_HAVE_INTTYPES_H 1
#define OPJ_PACKAGE_VERSION "2.5.0"

/* Not used by openjp2*/
#define HAVE_MEMORY_H 1
#define HAVE_STDLIB_H 1
#undef HAVE_STRINGS_H
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#undef HAVE_UNISTD_H

/* find whether or not have <malloc.h> */
#undef OPJ_HAVE_MALLOC_H
/* check if function `aligned_alloc` exists */
#undef OPJ_HAVE_ALIGNED_ALLOC
/* check if function `_aligned_malloc` exists */
#undef OPJ_HAVE__ALIGNED_MALLOC
/* check if function `memalign` exists */
#undef OPJ_HAVE_MEMALIGN
/* check if function `posix_memalign` exists */
#undef OPJ_HAVE_POSIX_MEMALIGN
