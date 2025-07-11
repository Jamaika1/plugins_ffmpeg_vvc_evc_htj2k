//#ifdef INTEL_INTRINSICS

#include "../../avisynth.h"
#include <stdint.h>
#include <algorithm>
#include <vector>

// experimental simd includes for avx2 compiled files
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#include <x86intrin.h>
// x86intrin.h includes header files for whatever instruction
// sets are specified on the compiler command line, such as: xopintrin.h, fma4intrin.h
#else
#include <immintrin.h> // MS version of immintrin.h covers AVX, AVX2 and FMA3
#endif // __GNUC__

#if !defined(__FMA__)
// Assume that all processors that have AVX2 also have FMA3
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
// Prevent error message in g++ when using FMA intrinsics with avx2:
#pragma message "It is recommended to specify also option -mfma when using -mavx2 or higher"
#else
#define __FMA__  1
#endif
#endif
// FMA3 instruction set
#if defined(__FMA__) && (defined(__GNUC__) || defined(__clang__))  && !defined(__INTEL_COMPILER)
#include <fmaintrin.h>
#endif // __FMA__

/*template<int minimum, int maximum>
static AVS_FORCEINLINE int static_clip(float val) {
    if (val > maximum) {
        return maximum;
    }
    if (val < minimum) {
        return minimum;
    }
    return (int)val;
}*/

template<typename pixel_t, int bits_per_pixel>
void weighted_average_avx2(uint8_t *dstp, int dst_pitch, const uint8_t **src_pointers, int *src_pitches, float *weights, int frames_count, int width, int height) {
    // width is row_size
    int mod_width;
    if(sizeof(pixel_t) == 1)
      mod_width = width / 16 * 16;
    else
      mod_width = width / 32 * 32;

    const int sse_size = (sizeof(pixel_t) == 1) ? 16 : 32;

    const int max_pixel_value = (sizeof(pixel_t) == 1) ? 255 : ((1 << bits_per_pixel) - 1);
    __m256i pixel_limit;
    __m128i pixel_limit_128i;
    if (sizeof(pixel_t) == 2 && bits_per_pixel < 16) {
      pixel_limit = _mm256_set1_epi16((int16_t)max_pixel_value);
      pixel_limit_128i = _mm_set1_epi16((int16_t)max_pixel_value);
    }

    __m128i zero128 = _mm_setzero_si128();
    __m256i zero = _mm256_setzero_si256();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < mod_width; x += sse_size) {
            __m256 acc_lo = _mm256_setzero_ps();
            __m256 acc_hi = _mm256_setzero_ps();

            for (int i = 0; i < frames_count; ++i) {
                __m128i src_lo, src_hi;
                __m256i src;
                __m256 src_lo_ps, src_hi_ps;

                if (sizeof(pixel_t) == 1) {
                  __m128i src128 = _mm_load_si128(reinterpret_cast<const __m128i*>(src_pointers[i] + x));
                  src_lo = _mm_unpacklo_epi8(src128, zero128);
                  src_hi = _mm_unpackhi_epi8(src128, zero128);
                  src = _mm256_set_m128i(src_hi, src_lo);
                }
                else {
                  src = _mm256_load_si256(reinterpret_cast<const __m256i*>(src_pointers[i] + x));
                }

                // _mm256_unpacklo_epi16: AVX2
                src_lo_ps = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(src, zero));
                src_hi_ps = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(src, zero));

                auto weight = _mm256_set1_ps(weights[i]);

                auto weighted_lo = _mm256_mul_ps(src_lo_ps, weight);
                auto weighted_hi = _mm256_mul_ps(src_hi_ps, weight);

                acc_lo = _mm256_add_ps(acc_lo, weighted_lo);
                acc_hi = _mm256_add_ps(acc_hi, weighted_hi);
            }
            auto dst_lo = _mm256_cvtps_epi32(acc_lo);
            auto dst_hi = _mm256_cvtps_epi32(acc_hi);

            __m256i dst;
            if (sizeof(pixel_t) == 1) {
              dst = _mm256_packs_epi32(dst_lo, dst_hi); // // not real 256, 2x128 schema
              dst = _mm256_packus_epi16(dst, zero);
              _mm_storel_epi64(reinterpret_cast<__m128i*>(dstp + x), _mm256_extractf128_si256(dst, 0));
              _mm_storel_epi64(reinterpret_cast<__m128i*>(dstp + x + 8), _mm256_extractf128_si256(dst, 1));
            }
            else if (sizeof(pixel_t) == 2) {
              dst = _mm256_packus_epi32(dst_lo, dst_hi); // not real 256, 2x128 schema
              if (sizeof(pixel_t) == 2 && bits_per_pixel < 16)
                dst = _mm256_min_epi16(dst, pixel_limit);
              _mm_store_si128(reinterpret_cast<__m128i*>(dstp + x), _mm256_extractf128_si256(dst, 0));
              _mm_store_si128(reinterpret_cast<__m128i*>(dstp + x + 16), _mm256_extractf128_si256(dst, 1));
            }
        }

        int start = mod_width / sizeof(pixel_t);
        int end = width / sizeof(pixel_t);
        for (int x = start; x < end; ++x) {
            float acc = 0;
            for (int i = 0; i < frames_count; ++i) {
                acc += reinterpret_cast<const pixel_t *>(src_pointers[i])[x] * weights[i];
            }
            reinterpret_cast<pixel_t *>(dstp)[x] = static_clip<0, max_pixel_value>(acc);
        }

        for (int i = 0; i < frames_count; ++i) {
            src_pointers[i] += src_pitches[i];
        }
        dstp += dst_pitch;
    }
    _mm256_zeroupper();
}

// instantiate
template void weighted_average_avx2<uint8_t, 8>(uint8_t *dstp, int dst_pitch, const uint8_t **src_pointers, int *src_pitches, float *weights, int frames_count, int width, int height);
template void weighted_average_avx2<uint16_t, 10>(uint8_t *dstp, int dst_pitch, const uint8_t **src_pointers, int *src_pitches, float *weights, int frames_count, int width, int height);
template void weighted_average_avx2<uint16_t, 12>(uint8_t *dstp, int dst_pitch, const uint8_t **src_pointers, int *src_pitches, float *weights, int frames_count, int width, int height);
template void weighted_average_avx2<uint16_t, 14>(uint8_t *dstp, int dst_pitch, const uint8_t **src_pointers, int *src_pitches, float *weights, int frames_count, int width, int height);
template void weighted_average_avx2<uint16_t, 16>(uint8_t *dstp, int dst_pitch, const uint8_t **src_pointers, int *src_pitches, float *weights, int frames_count, int width, int height);

void weighted_average_f_avx2(uint8_t *dstp, int dst_pitch, const uint8_t **src_pointers, int *src_pitches, float *weights, int frames_count, int width, int height) {
  // width is row_size
  int mod_width = width / 32 * 32;

  const int sse_size = 32;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < mod_width; x += sse_size) {
      __m256 acc = _mm256_setzero_ps();

      for (int i = 0; i < frames_count; ++i) {
        __m256 src = _mm256_load_ps(reinterpret_cast<const float*>(src_pointers[i] + x)); // float  8*32=256 8 pixels at a time
        auto weight = _mm256_set1_ps(weights[i]);
        auto weighted = _mm256_mul_ps(src, weight);
        acc = _mm256_add_ps(acc, weighted);
      }
      _mm256_stream_ps(reinterpret_cast<float*>(dstp + x), acc);
    }

    for (int x = mod_width / 4; x < width / 4; ++x) {
      float acc = 0;
      for (int i = 0; i < frames_count; ++i) {
        acc += reinterpret_cast<const float *>(src_pointers[i])[x] * weights[i];
      }
      reinterpret_cast<float *>(dstp)[x] = acc; // float: no clamping
    }

    for (int i = 0; i < frames_count; ++i) {
      src_pointers[i] += src_pitches[i];
    }
    dstp += dst_pitch;
  }
  _mm256_zeroupper();
}

//#endif
