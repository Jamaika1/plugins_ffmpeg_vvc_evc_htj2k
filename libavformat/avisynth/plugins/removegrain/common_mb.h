#ifndef __COMMON_MB_H__
#define __COMMON_MB_H__

#include "../../avisynth.h"
#include <stdint.h>

#if defined(__clang__)
// Check clang first. clang-cl also defines __MSC_VER
// We set MSVC because they are mostly compatible
#   define CLANG
#if defined(_MSC_VER)
#   define MSVC
#   define MB_FORCEINLINE __attribute__((always_inline))
#else
#   define MB_FORCEINLINE __attribute__((always_inline)) inline
#endif
#elif   defined(_MSC_VER)
#   define MSVC
#   define MSVC_PURE
#   define MB_FORCEINLINE __forceinline
#elif defined(__GNUC__)
#   define GCC
#   define MB_FORCEINLINE __attribute__((always_inline)) inline
#else
#   error Unsupported compiler.
#   define MB_FORCEINLINE inline
#   undef __forceinline
#   define __forceinline inline
#endif

enum class InstructionSet
{
//#ifdef INTEL_INTRINSICS
  AVX2,
  SSE2,
//#endif
  PLAIN_C
};

#ifndef _WIN32
#define OutputDebugString(x)
#endif

#if (defined(GCC) || defined(CLANG)) && !defined(_WIN32)
#include <stdlib.h>
#define _aligned_malloc(size, alignment) aligned_alloc(alignment, size)
#define _aligned_free(ptr) free(ptr)
#endif

#ifndef _WIN32
#include <stdio.h>
#ifdef AVS_POSIX
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif
#include <limits.h>
#endif
#endif

#endif
