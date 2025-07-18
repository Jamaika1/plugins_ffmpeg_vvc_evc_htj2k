#ifndef MISSING_INTEL_INTRINSICS_H_
#define MISSING_INTEL_INTRINSICS_H_

#include <immintrin.h>

#ifndef __OPTIMIZE__
// Old Visual Studio headers lack the bsrli variant
#ifndef _mm_bsrli_si128
  #define _mm_bsrli_si128(a, imm8) _mm_srli_si128((a), (imm8))
#endif

// GCC headers apparently won't have this at all.. sigh
#ifndef _andn_u32
  // VS2015 headers apparently won't have this at all.. sigh
  #ifdef __andn_u32
    #define _andn_u32(x, y) (__andn_u32((x), (y)))
  #else
    #define _andn_u32(x, y) ((~(x)) & (y))
  #endif // __andn_u32
#endif // _andn_u32

// Some Visual Studio headers apparently lack these pseudoinstructions
#if COMPILE_INTEL_AVX2
#ifdef __AVX2__
  #ifndef _mm256_bsrli_epi128
    #define _mm256_bsrli_epi128(a, imm8) _mm256_srli_si256((a), (imm8))
  #endif
#endif
  #ifndef _mm256_insert_epi32
    #define _mm256_insert_epi32(a, i, index) (_mm256_blend_epi32((a), _mm256_set1_epi32(i), (1 << (index))))
  #endif

  #ifndef _mm256_extract_epi32
    #define _mm256_extract_epi32(a, index) (_mm_extract_epi32(_mm256_extracti128_si256((a), (index) >> 2), (index) & 3))
  #endif

  #ifndef _mm256_storeu2_m128i
    #define _mm256_storeu2_m128i(/* __m128i* */ hiaddr, /* __m128i* */ loaddr, /* __m256i */ a) \
    do { __m256i _a = (a); \
        _mm_storeu_si128((__m128i*)(loaddr), _mm256_castsi256_si128(_a)); \
        _mm_storeu_si128((__m128i*)(hiaddr), _mm256_extractf128_si256(_a, 0x1)); \
    } while (0)
  #endif

#endif
#endif // __OPTIMIZE__

#endif

