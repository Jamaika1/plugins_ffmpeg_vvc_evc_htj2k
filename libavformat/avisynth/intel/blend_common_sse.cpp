// Avisynth v2.5.  Copyright 2002 Ben Rudiak-Gould et al.
// http://avisynth.nl

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
// http://www.gnu.org/copyleft/gpl.html .
//
// Linking Avisynth statically or dynamically with other modules is making a
// combined work based on Avisynth.  Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Avisynth give you
// permission to link Avisynth with independent modules that communicate with
// Avisynth solely through the interfaces defined in avisynth.h, regardless of the license
// terms of these independent modules, and to copy and distribute the
// resulting combined work under terms of your choice, provided that
// every copy of the combined work is accompanied by a complete copy of
// the source code of Avisynth (the version of Avisynth used to produce the
// combined work), being distributed under the terms of the GNU General
// Public License plus this exception.  An independent module is a module
// which is not derived from or based on Avisynth, such as 3rd-party filters,
// import and export plugins, or graphical user interfaces.

// Overlay (c) 2003, 2004 by Klaus Post

#include "../avisynth.h"

#include "blend_common_sse.h"
#include "../blend_common.h"
#include "../internal.h"

// Intrinsics for SSE4.1, SSSE3, SSE3, SSE2, ISSE and MMX
#include <emmintrin.h>
#include <smmintrin.h>
#include <cstdint>

/********************************
 ********* Blend Opaque *********
 ** Use for Lighten and Darken **
 ********************************/

#ifdef X86_32
AVS_FORCEINLINE __m64 overlay_blend_opaque_mmx_core(const __m64& p1, const __m64& p2, const __m64& mask) {
  // return (mask) ? p2 : p1;
  __m64 r1 = _mm_andnot_si64(mask, p1);
  __m64 r2 = _mm_and_si64   (mask, p2);
  return _mm_or_si64(r1, r2);
}
#endif

AVS_FORCEINLINE __m128i overlay_blend_opaque_sse2_core(const __m128i& p1, const __m128i& p2, const __m128i& mask) {
  // return (mask) ? p2 : p1;
  __m128i r1 = _mm_andnot_si128(mask, p1);
  __m128i r2 = _mm_and_si128   (mask, p2);
  return _mm_or_si128(r1, r2);
}

template<typename pixel_t>
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
static AVS_FORCEINLINE void Eightpixels_to_Eightfloats(const pixel_t* src, __m128& src_lo, __m128& src_hi, __m128i& zero) {
  __m128i srci;
  if constexpr (sizeof(pixel_t) == 1) {
    srci = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(src));
    srci = _mm_unpacklo_epi8(srci, zero);
  }
  else {
    srci = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
  }
  src_lo = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(srci));
  src_hi = _mm_cvtepi32_ps(_mm_unpackhi_epi16(srci, zero));
}

template<typename pixel_t>
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
static AVS_FORCEINLINE void Store_Eightpixels(pixel_t* dst, __m128 what_lo, __m128 what_hi, const __m128 rounder) {
  what_lo = _mm_add_ps(what_lo, rounder); // round
  what_hi = _mm_add_ps(what_hi, rounder); // round
  auto si32_lo = _mm_cvttps_epi32(what_lo); // truncate
  auto si32_hi = _mm_cvttps_epi32(what_hi); // truncate
  auto result = _mm_packus_epi32(si32_lo, si32_hi); // 2x4x32bit -> 8x16
  if constexpr (sizeof(pixel_t) == 1) {
    __m128i result64 = _mm_packus_epi16(result, result); // 8x16bit -> 8x8
    _mm_storel_epi64(reinterpret_cast<__m128i*>(dst), result64);
  }
  else {
    /* when mask is 0..1 checked then this is not possible
    if constexpr (bits_per_pixel < 16) { // otherwise no clamp needed
      constexpr int max_pixel_value = (1 << bits_per_pixel) - 1;
      auto max_pixel_value_v = _mm_set1_epi16(static_cast<uint16_t>(max_pixel_value));
      result128 = _mm_min_epu16(result128, max_pixel_value_v);
    }
    */
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), result);
  }
}

template<typename pixel_t>
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse2")))
#endif
static AVS_FORCEINLINE void Eightpixels_to_Eightfloats_sse2(const pixel_t* src, __m128& src_lo, __m128& src_hi, __m128i& zero) {
  __m128i srci;
  if constexpr (sizeof(pixel_t) == 1) {
    srci = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(src));
    srci = _mm_unpacklo_epi8(srci, zero);
  }
  else {
    srci = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
  }
  src_lo = _mm_cvtepi32_ps(_mm_unpacklo_epi16(srci, zero));
  src_hi = _mm_cvtepi32_ps(_mm_unpackhi_epi16(srci, zero));
}

template<typename pixel_t>
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse2")))
#endif
static AVS_FORCEINLINE void Store_Eightpixels_sse2(pixel_t* dst, __m128 what_lo, __m128 what_hi, const __m128 rounder) {
  what_lo = _mm_add_ps(what_lo, rounder); // round
  what_hi = _mm_add_ps(what_hi, rounder); // round
  auto si32_lo = _mm_cvttps_epi32(what_lo); // truncate
  auto si32_hi = _mm_cvttps_epi32(what_hi); // truncate
  if constexpr (sizeof(pixel_t) == 1) {
    auto result = _mm_packs_epi32(si32_lo, si32_hi); // 2x4x32bit -> 8x16
    __m128i result64 = _mm_packus_epi16(result, result); // 8x16bit -> 8x8
    _mm_storel_epi64(reinterpret_cast<__m128i*>(dst), result64);
  }
  else {
    auto result = _MM_PACKUS_EPI32(si32_lo, si32_hi); // 2x4x32bit -> 8x16
      /* when mask is 0..1 checked then this is not possible
    if constexpr (bits_per_pixel < 16) { // otherwise no clamp needed
      constexpr int max_pixel_value = (1 << bits_per_pixel) - 1;
      auto max_pixel_value_v = _mm_set1_epi16(static_cast<uint16_t>(max_pixel_value));
      result128 = _mm_min_epu16(result128, max_pixel_value_v);
    }
    */
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), result);
  }
}



AVS_FORCEINLINE static __m128 overlay_blend_sse_core_new(const __m128& p1_f, const __m128& p2_f, const __m128& factor) {
  /*
  //  p1*(1-mask_f) + p2*mask_f -> p1 + (p2-p1)*mask_f
  constexpr int max_pixel_value = (1 << bits_per_pixel) - 1;
  constexpr float factor = 1.0f / max_pixel_value;
  constexpr float half_rounder = 0.5f;
  const float mask_f = mask * factor;
  const float res = p1 + (p2 - p1) * mask_f;
  int result = (int)(res + 0.5f);
  */
  // rounding not here, but before storage
  auto res = _mm_add_ps(p1_f, _mm_mul_ps(_mm_sub_ps(p2_f, p1_f), factor));
  return res;
}

template<bool has_mask, typename pixel_t>
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void overlay_blend_sse41_uint(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch,
  const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel)
{

  auto rounder = _mm_set1_ps(0.5f);
  const int max_pixel_value = (1 << bits_per_pixel) - 1;
  auto factor = has_mask ? opacity_f / max_pixel_value : opacity_f;
  auto factor_v = _mm_set1_ps(factor);

  const int realwidth = width * sizeof(pixel_t);

  // 8 pixels at a time
  constexpr int bytes_per_cycle = 8 * sizeof(pixel_t);
  int wMod8 = (realwidth / bytes_per_cycle) * bytes_per_cycle;

  auto zero = _mm_setzero_si128();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < wMod8; x += bytes_per_cycle) {
      __m128 unpacked_p1, unpacked_p1_2;
      __m128 unpacked_p2, unpacked_p2_2;
      Eightpixels_to_Eightfloats<pixel_t>((const pixel_t*)(p1 + x), unpacked_p1, unpacked_p1_2, zero); // 8x32
      Eightpixels_to_Eightfloats<pixel_t>((const pixel_t*)(p2 + x), unpacked_p2, unpacked_p2_2, zero); // 8x32

      __m128 result, result_2;
      if constexpr (has_mask) {
        __m128 unpacked_mask, unpacked_mask_2;
        Eightpixels_to_Eightfloats<pixel_t>((const pixel_t*)(mask + x), unpacked_mask, unpacked_mask_2, zero); // 8x32
        unpacked_mask = _mm_mul_ps(unpacked_mask, factor_v);
        unpacked_mask_2 = _mm_mul_ps(unpacked_mask_2, factor_v);
        result = overlay_blend_sse_core_new(unpacked_p1, unpacked_p2, unpacked_mask);
        result_2 = overlay_blend_sse_core_new(unpacked_p1_2, unpacked_p2_2, unpacked_mask_2);
      }
      else {
        result = overlay_blend_sse_core_new(unpacked_p1, unpacked_p2, factor_v);
        result_2 = overlay_blend_sse_core_new(unpacked_p1_2, unpacked_p2_2, factor_v);
      }

      Store_Eightpixels<pixel_t>((pixel_t*)(p1 + x), result, result_2, rounder);
    }

    // Leftover value

    for (int x = wMod8 / sizeof(pixel_t); x < width; x++) {
      const float new_factor = has_mask ? static_cast<float>(reinterpret_cast<const pixel_t*>(mask)[x]) * factor : factor;
      auto result = overlay_blend_c_core_simple(reinterpret_cast<pixel_t*>(p1)[x], reinterpret_cast<const pixel_t*>(p2)[x], new_factor);
      reinterpret_cast<pixel_t*>(p1)[x] = (pixel_t)(result + 0.5f);
    }

    p1 += p1_pitch;
    p2 += p2_pitch;
    if (has_mask)
      mask += mask_pitch;
  }
}

// instantiate
// mask yes/no
template void overlay_blend_sse41_uint<true, uint8_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);
template void overlay_blend_sse41_uint<true, uint16_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);
//--
template void overlay_blend_sse41_uint<false, uint8_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);
template void overlay_blend_sse41_uint<false, uint16_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);


template<bool has_mask, typename pixel_t>
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse2")))
#endif
void overlay_blend_sse2_uint(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch,
  const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel)
{

  auto rounder = _mm_set1_ps(0.5f);
  const int max_pixel_value = (1 << bits_per_pixel) - 1;
  auto factor = has_mask ? opacity_f / max_pixel_value : opacity_f;
  auto factor_v = _mm_set1_ps(factor);

  const int realwidth = width * sizeof(pixel_t);

  // 8 pixels at a time
  constexpr int bytes_per_cycle = 8 * sizeof(pixel_t);
  int wMod8 = (realwidth / bytes_per_cycle) * bytes_per_cycle;

  auto zero = _mm_setzero_si128();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < wMod8; x += bytes_per_cycle) {
      __m128 unpacked_p1, unpacked_p1_2;
      __m128 unpacked_p2, unpacked_p2_2;
      Eightpixels_to_Eightfloats_sse2<pixel_t>((const pixel_t*)(p1 + x), unpacked_p1, unpacked_p1_2, zero); // 8x32
      Eightpixels_to_Eightfloats_sse2<pixel_t>((const pixel_t*)(p2 + x), unpacked_p2, unpacked_p2_2, zero); // 8x32

      __m128 result, result_2;
      if constexpr (has_mask) {
        __m128 unpacked_mask, unpacked_mask_2;
        Eightpixels_to_Eightfloats_sse2<pixel_t>((const pixel_t*)(mask + x), unpacked_mask, unpacked_mask_2, zero); // 8x32
        unpacked_mask = _mm_mul_ps(unpacked_mask, factor_v);
        unpacked_mask_2 = _mm_mul_ps(unpacked_mask_2, factor_v);
        result = overlay_blend_sse_core_new(unpacked_p1, unpacked_p2, unpacked_mask);
        result_2 = overlay_blend_sse_core_new(unpacked_p1_2, unpacked_p2_2, unpacked_mask_2);
      }
      else {
        result = overlay_blend_sse_core_new(unpacked_p1, unpacked_p2, factor_v);
        result_2 = overlay_blend_sse_core_new(unpacked_p1_2, unpacked_p2_2, factor_v);
      }

      Store_Eightpixels_sse2<pixel_t>((pixel_t*)(p1 + x), result, result_2, rounder);
    }

    // Leftover value
    // Working with exact dimension, overlay is possible atop an existing frame at any position
    for (int x = wMod8 / sizeof(pixel_t); x < width; x++) {
      const float new_factor = has_mask ? static_cast<float>(reinterpret_cast<const pixel_t*>(mask)[x]) * factor : factor;
      auto result = overlay_blend_c_core_simple(reinterpret_cast<pixel_t*>(p1)[x], reinterpret_cast<const pixel_t*>(p2)[x], new_factor);
      reinterpret_cast<pixel_t*>(p1)[x] = (pixel_t)(result + 0.5f);
    }

    p1 += p1_pitch;
    p2 += p2_pitch;
    if (has_mask)
      mask += mask_pitch;
  }
}

// instantiate
// mask yes/no
template void overlay_blend_sse2_uint<true, uint8_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);
template void overlay_blend_sse2_uint<true, uint16_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);
//--
template void overlay_blend_sse2_uint<false, uint8_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);
template void overlay_blend_sse2_uint<false, uint16_t>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int opacity, const float opacity_f, const int bits_per_pixel);


template<bool has_mask>
void overlay_blend_sse2_float(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch,
  const int width, const int height, const int /*opacity*/, const float opacity_f, const int bits_per_pixel)
{

  const int realwidth = width * sizeof(float);

  int wMod16 = (realwidth / 16) * 16;
  auto opacity_v = _mm_set1_ps(opacity_f);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < wMod16; x += 16) {
      auto p1_f = _mm_loadu_ps(reinterpret_cast<const float*>(p1 + x));
      auto p2_f = _mm_loadu_ps(reinterpret_cast<const float*>(p2 + x));
      __m128 new_mask;
      if constexpr (has_mask) {
        new_mask = _mm_loadu_ps(reinterpret_cast<const float*>(mask + x));
        new_mask = _mm_mul_ps(new_mask, opacity_v);
      }
      else {
        new_mask = opacity_v;
      }
      auto result = _mm_add_ps(p1_f, _mm_mul_ps(_mm_sub_ps(p2_f, p1_f), new_mask)); // p1*(1-mask) + p2*mask = p1+(p2-p1)*mask

      _mm_storeu_ps(reinterpret_cast<float*>(p1 + x), result);
    }

    // Leftover value
    // Working with exact dimension, overlay is possible atop an existing frame at any position
    for (int x = wMod16 / sizeof(float); x < width; x++) {
      auto new_mask = has_mask ? reinterpret_cast<const float*>(mask)[x] * opacity_f : opacity_f;
      auto p1x = reinterpret_cast<float*>(p1)[x];
      auto p2x = reinterpret_cast<const float*>(p2)[x];
      auto result = p1x + (p2x - p1x) * new_mask; // p1x*(1-new_mask) + p2x*mask
      reinterpret_cast<float*>(p1)[x] = result;
    }


    p1 += p1_pitch;
    p2 += p2_pitch;
    if constexpr (has_mask)
      mask += mask_pitch;
  }
}

// instantiate
template void overlay_blend_sse2_float<false>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int /*opacity*/, const float opacity_f, const int bits_per_pixel);
template void overlay_blend_sse2_float<true>(BYTE* p1, const BYTE* p2, const BYTE* mask,
  const int p1_pitch, const int p2_pitch, const int mask_pitch, const int width, const int height, const int /*opacity*/, const float opacity_f, const int bits_per_pixel);


/***************************************
 ********* Mode: Lighten/Darken ********
 ***************************************/

typedef __m128i (OverlaySseBlendOpaque)(const __m128i&, const __m128i&, const __m128i&);
typedef __m128i (OverlaySseCompare)(const __m128i&, const __m128i&, const __m128i&);
#ifdef X86_32
typedef   __m64 (OverlayMmxCompare)(const __m64&, const __m64&, const __m64&);
#endif

typedef int (OverlayCCompare)(BYTE, BYTE);

template<typename pixel_t, bool darken /* OverlayCCompare<pixel_t> compare*/>
AVS_FORCEINLINE void overlay_darklighten_c(BYTE *p1Y_8, BYTE *p1U_8, BYTE *p1V_8, const BYTE *p2Y_8, const BYTE *p2U_8, const BYTE *p2V_8, int p1_pitch, int p2_pitch, int width, int height) {
  pixel_t* p1Y = reinterpret_cast<pixel_t *>(p1Y_8);
  pixel_t* p1U = reinterpret_cast<pixel_t *>(p1U_8);
  pixel_t* p1V = reinterpret_cast<pixel_t *>(p1V_8);

  const pixel_t* p2Y = reinterpret_cast<const pixel_t *>(p2Y_8);
  const pixel_t* p2U = reinterpret_cast<const pixel_t *>(p2U_8);
  const pixel_t* p2V = reinterpret_cast<const pixel_t *>(p2V_8);

  // pitches are already scaled
  //p1_pitch /= sizeof(pixel_t);
  //p2_pitch /= sizeof(pixel_t);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int mask = darken ? (p2Y[x] <= p1Y[x]) : (p2Y[x] >= p1Y[x]); // compare(p1Y[x], p2Y[x]);
      p1Y[x] = overlay_blend_opaque_c_core<pixel_t>(p1Y[x], p2Y[x], mask);
      p1U[x] = overlay_blend_opaque_c_core<pixel_t>(p1U[x], p2U[x], mask);
      p1V[x] = overlay_blend_opaque_c_core<pixel_t>(p1V[x], p2V[x], mask);
    }

    p1Y += p1_pitch;
    p1U += p1_pitch;
    p1V += p1_pitch;

    p2Y += p2_pitch;
    p2U += p2_pitch;
    p2V += p2_pitch;
  }
}

#ifdef X86_32
template<OverlayMmxCompare compare, OverlayCCompare compare_c>
AVS_FORCEINLINE void overlay_darklighten_mmx(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  __m64 zero = _mm_setzero_si64();

  int wMod8 = (width/8) * 8;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < wMod8; x+=8) {
      // Load Y Plane
      __m64 p1_y = *(reinterpret_cast<const __m64*>(p1Y+x));
      __m64 p2_y = *(reinterpret_cast<const __m64*>(p2Y+x));

      // Compare
      __m64 cmp_result = compare(p1_y, p2_y, zero);

      // Process U Plane
      __m64 result_y = overlay_blend_opaque_mmx_core(p1_y, p2_y, cmp_result);
      *reinterpret_cast<__m64*>(p1Y+x) = result_y;

      // Process U plane
      __m64 p1_u = *(reinterpret_cast<const __m64*>(p1U+x));
      __m64 p2_u = *(reinterpret_cast<const __m64*>(p2U+x));

      __m64 result_u = overlay_blend_opaque_mmx_core(p1_u, p2_u, cmp_result);
      *reinterpret_cast<__m64*>(p1U+x) = result_u;

      // Process V plane
      __m64 p1_v = *(reinterpret_cast<const __m64*>(p1V+x));
      __m64 p2_v = *(reinterpret_cast<const __m64*>(p2V+x));

      __m64 result_v = overlay_blend_opaque_mmx_core(p1_v, p2_v, cmp_result);
      *reinterpret_cast<__m64*>(p1V+x) = result_v;
    }

    // Leftover value
    for (int x = wMod8; x < width; x++) {
      int mask = compare_c(p1Y[x], p2Y[x]);
      p1Y[x] = overlay_blend_opaque_c_core<uint8_t>(p1Y[x], p2Y[x], mask);
      p1U[x] = overlay_blend_opaque_c_core<uint8_t>(p1U[x], p2U[x], mask);
      p1V[x] = overlay_blend_opaque_c_core<uint8_t>(p1V[x], p2V[x], mask);
    }

    p1Y += p1_pitch;
    p1U += p1_pitch;
    p1V += p1_pitch;

    p2Y += p2_pitch;
    p2U += p2_pitch;
    p2V += p2_pitch;
  }

  _mm_empty();
}
#endif

template <OverlaySseCompare compare, OverlayCCompare compare_c>
void overlay_darklighten_sse2(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  __m128i zero = _mm_setzero_si128();

  int wMod16 = (width/16) * 16;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < wMod16; x+=16) {
      // Load Y Plane
      __m128i p1_y = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1Y+x));
      __m128i p2_y = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2Y+x));

      // Compare
      __m128i cmp_result = compare(p1_y, p2_y, zero);

      // Process U Plane
      __m128i result_y = overlay_blend_opaque_sse2_core(p1_y, p2_y, cmp_result);
      _mm_storeu_si128(reinterpret_cast<__m128i*>(p1Y+x), result_y);

      // Process U plane
      __m128i p1_u = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1U+x));
      __m128i p2_u = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2U+x));

      __m128i result_u = overlay_blend_opaque_sse2_core(p1_u, p2_u, cmp_result);
      _mm_storeu_si128(reinterpret_cast<__m128i*>(p1U+x), result_u);

      // Process V plane
      __m128i p1_v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1V+x));
      __m128i p2_v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2V+x));

      __m128i result_v = overlay_blend_opaque_sse2_core(p1_v, p2_v, cmp_result);
      _mm_storeu_si128(reinterpret_cast<__m128i*>(p1V+x), result_v);
    }

    // Leftover value
    for (int x = wMod16; x < width; x++) {
      int mask = compare_c(p1Y[x], p2Y[x]);
      p1Y[x] = overlay_blend_opaque_c_core<uint8_t>(p1Y[x], p2Y[x], mask);
      p1U[x] = overlay_blend_opaque_c_core<uint8_t>(p1U[x], p2U[x], mask);
      p1V[x] = overlay_blend_opaque_c_core<uint8_t>(p1V[x], p2V[x], mask);
    }

    p1Y += p1_pitch;
    p1U += p1_pitch;
    p1V += p1_pitch;

    p2Y += p2_pitch;
    p2U += p2_pitch;
    p2V += p2_pitch;
  }
}

template <OverlaySseCompare compare, OverlayCCompare compare_c>
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void overlay_darklighten_sse41(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height)
{
  __m128i zero = _mm_setzero_si128();

  int wMod16 = (width / 16) * 16;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < wMod16; x += 16) {
      // Load Y Plane
      __m128i p1_y = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1Y + x));
      __m128i p2_y = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2Y + x));

      // Compare
      __m128i cmp_result = compare(p1_y, p2_y, zero);

      // Process Y Plane
      __m128i result_y = _mm_blendv_epi8(p1_y, p2_y, cmp_result); // SSE4.1
      _mm_storeu_si128(reinterpret_cast<__m128i*>(p1Y + x), result_y);

      // Process U plane
      __m128i p1_u = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1U + x));
      __m128i p2_u = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2U + x));

      __m128i result_u = _mm_blendv_epi8(p1_u, p2_u, cmp_result);
      _mm_storeu_si128(reinterpret_cast<__m128i*>(p1U + x), result_u);

      // Process V plane
      __m128i p1_v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1V + x));
      __m128i p2_v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2V + x));

      __m128i result_v = _mm_blendv_epi8(p1_v, p2_v, cmp_result);
      _mm_storeu_si128(reinterpret_cast<__m128i*>(p1V + x), result_v);
    }

    // Leftover value
    for (int x = wMod16; x < width; x++) {
      int mask = compare_c(p1Y[x], p2Y[x]);
      p1Y[x] = overlay_blend_opaque_c_core<uint8_t>(p1Y[x], p2Y[x], mask);
      p1U[x] = overlay_blend_opaque_c_core<uint8_t>(p1U[x], p2U[x], mask);
      p1V[x] = overlay_blend_opaque_c_core<uint8_t>(p1V[x], p2V[x], mask);
    }

    p1Y += p1_pitch;
    p1U += p1_pitch;
    p1V += p1_pitch;

    p2Y += p2_pitch;
    p2U += p2_pitch;
    p2V += p2_pitch;
  }
}

// Compare functions for lighten and darken mode
AVS_FORCEINLINE static int overlay_darken_c_cmp(BYTE p1, BYTE p2) {
  return p2 <= p1;
}

#ifdef X86_32
AVS_FORCEINLINE __m64 overlay_darken_mmx_cmp(const __m64& p1, const __m64& p2, const __m64& zero) {
  __m64 diff = _mm_subs_pu8(p2, p1);
  return _mm_cmpeq_pi8(diff, zero);
}
#endif

AVS_FORCEINLINE __m128i overlay_darken_sse_cmp(const __m128i& p1, const __m128i& p2, const __m128i& zero) {
  __m128i diff = _mm_subs_epu8(p2, p1);
  return _mm_cmpeq_epi8(diff, zero);
}

template<typename pixel_t>
AVS_FORCEINLINE int overlay_lighten_c_cmp(pixel_t p1, pixel_t p2) {
  return p2 >= p1;
}

#ifdef X86_32
AVS_FORCEINLINE __m64 overlay_lighten_mmx_cmp(const __m64& p1, const __m64& p2, const __m64& zero) {
  __m64 diff = _mm_subs_pu8(p1, p2);
  return _mm_cmpeq_pi8(diff, zero);
}
#endif

AVS_FORCEINLINE __m128i overlay_lighten_sse_cmp(const __m128i& p1, const __m128i& p2, const __m128i& zero) {
  __m128i diff = _mm_subs_epu8(p1, p2);
  return _mm_cmpeq_epi8(diff, zero);
}

#ifdef X86_32
void overlay_darken_mmx(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  overlay_darklighten_mmx<overlay_darken_mmx_cmp, overlay_darken_c_cmp>(p1Y, p1U, p1V, p2Y, p2U, p2V, p1_pitch, p2_pitch, width, height);
}
void overlay_lighten_mmx(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  overlay_darklighten_mmx<overlay_lighten_mmx_cmp, overlay_lighten_c_cmp>(p1Y, p1U, p1V, p2Y, p2U, p2V, p1_pitch, p2_pitch, width, height);
}
#endif

void overlay_darken_sse2(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  overlay_darklighten_sse2<overlay_darken_sse_cmp, overlay_darken_c_cmp>(p1Y, p1U, p1V, p2Y, p2U, p2V, p1_pitch, p2_pitch, width, height);
}
void overlay_lighten_sse2(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  overlay_darklighten_sse2<overlay_lighten_sse_cmp, overlay_lighten_c_cmp>(p1Y, p1U, p1V, p2Y, p2U, p2V, p1_pitch, p2_pitch, width, height);
}

void overlay_darken_sse41(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  overlay_darklighten_sse41<overlay_darken_sse_cmp, overlay_darken_c_cmp>(p1Y, p1U, p1V, p2Y, p2U, p2V, p1_pitch, p2_pitch, width, height);
}
void overlay_lighten_sse41(BYTE *p1Y, BYTE *p1U, BYTE *p1V, const BYTE *p2Y, const BYTE *p2U, const BYTE *p2V, int p1_pitch, int p2_pitch, int width, int height) {
  overlay_darklighten_sse41<overlay_lighten_sse_cmp, overlay_lighten_c_cmp>(p1Y, p1U, p1V, p2Y, p2U, p2V, p1_pitch, p2_pitch, width, height);
}
