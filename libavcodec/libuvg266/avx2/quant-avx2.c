/*****************************************************************************
 * This file is part of uvg266 VVC encoder.
 *
 * Copyright (c) 2021, Tampere University, ITU/ISO/IEC, project contributors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of the Tampere University or ITU/ISO/IEC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS
 ****************************************************************************/

/*
* \file
*/

#include "quant-avx2.h"

#if COMPILE_INTEL_AVX2 && defined X86_64
#include <immintrin.h>
#include <stdlib.h>

#include "avx2_common_functions.h"
#include "../cu.h"
#include "../encoder.h"
#include "../encoderstate.h"
#include "../uvg266.h"
#include "../rdo.h"
#include "../scalinglist.h"
#include "../quant-generic.h"
#include "../strategies-quant.h"
#include "../strategies-picture.h"
#include "../strategyselector.h"
#include "../tables.h"
#include "../transform.h"
#include "../fast_coeff_cost.h"
#include "../reshape.h"

static INLINE int32_t hsum32_8x32i(__m256i src)
{
  __m128i a = _mm256_extracti128_si256(src, 0);
  __m128i b = _mm256_extracti128_si256(src, 1);

  a = _mm_add_epi32(a, b);
  b = _mm_shuffle_epi32(a, _MM_SHUFFLE(0, 1, 2, 3));

  a = _mm_add_epi32(a, b);
  b = _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1));

  a = _mm_add_epi32(a, b);
  return _mm_cvtsi128_si32(a);
}

static INLINE int32_t hsum32_16x16i(__m256i src)
{
  __m128i a = _mm256_extracti128_si256(src, 0);
  __m128i b = _mm256_extracti128_si256(src, 1);
  __m256i c = _mm256_cvtepi16_epi32(a);
  __m256i d = _mm256_cvtepi16_epi32(b);

  c = _mm256_add_epi32(c, d);
  return hsum32_8x32i(c);
}

// Rearranges a 16x32b double vector into a format suitable for a stable SIMD
// max algorithm:
// (abcd|efgh) (ijkl|mnop) => (aceg|ikmo) (bdfh|jlnp)
static INLINE void rearrange_512(__m256i *hi, __m256i *lo)
{
  const __m256i perm8x32mask = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);

  __m256i tmphi = _mm256_permutevar8x32_epi32(*hi, perm8x32mask);
  __m256i tmplo = _mm256_permutevar8x32_epi32(*lo, perm8x32mask);

  *hi = _mm256_permute2x128_si256(tmplo, tmphi, 0x31);
  *lo = _mm256_permute2x128_si256(tmplo, tmphi, 0x20);
}

static INLINE void get_cheapest_alternative(__m256i costs_hi, __m256i costs_lo,
    __m256i ns, __m256i changes,
    int16_t *final_change, int32_t *min_pos)
{
  // Interleave ns and lo into 32-bit variables and to two 256-bit wide vecs,
  // to have the same data layout as in costs. Zero extend to 32b width, shift
  // changes 16 bits to the left, and store them into the same vectors.
  __m256i tmp1hi = _mm256_unpackhi_epi16(ns, changes);
  __m256i tmp1lo = _mm256_unpacklo_epi16(ns, changes);

  __m256i pl1hi = _mm256_permute2x128_si256(tmp1lo, tmp1hi, 0x31);
  __m256i pl1lo = _mm256_permute2x128_si256(tmp1lo, tmp1hi, 0x20);

  // Reorder to afford result stability (if multiple atoms tie for cheapest,
  // rightmost ie. the highest is the wanted one)
  rearrange_512(&costs_hi, &costs_lo);
  rearrange_512(&pl1hi, &pl1lo);

  // 0: pick hi, 1: pick lo (equality evaluates as 0)
  __m256i cmpmask1 = _mm256_cmpgt_epi32(costs_hi, costs_lo);
  __m256i cost1    = _mm256_blendv_epi8(costs_hi, costs_lo, cmpmask1);
  __m256i pl1_1    = _mm256_blendv_epi8(pl1hi,    pl1lo,    cmpmask1);

  __m256i cost2    = _mm256_shuffle_epi32(cost1, _MM_SHUFFLE(2, 3, 0, 1));
  __m256i pl1_2    = _mm256_shuffle_epi32(pl1_1, _MM_SHUFFLE(2, 3, 0, 1));

  __m256i cmpmask2 = _mm256_cmpgt_epi32(cost2, cost1);
  __m256i cost3    = _mm256_blendv_epi8(cost2, cost1, cmpmask2);
  __m256i pl1_3    = _mm256_blendv_epi8(pl1_2, pl1_1, cmpmask2);

  __m256i cost4    = _mm256_shuffle_epi32(cost3, _MM_SHUFFLE(1, 0, 3, 2));
  __m256i pl1_4    = _mm256_shuffle_epi32(pl1_3, _MM_SHUFFLE(1, 0, 3, 2));

  __m256i cmpmask3 = _mm256_cmpgt_epi32(cost4, cost3);
  __m256i cost5    = _mm256_blendv_epi8(cost4, cost3, cmpmask3);
  __m256i pl1_5    = _mm256_blendv_epi8(pl1_4, pl1_3, cmpmask3);

  __m256i cost6    = _mm256_permute4x64_epi64(cost5, _MM_SHUFFLE(1, 0, 3, 2));
  __m256i pl1_6    = _mm256_permute4x64_epi64(pl1_5, _MM_SHUFFLE(1, 0, 3, 2));

  __m256i cmpmask4 = _mm256_cmpgt_epi32(cost6, cost5);
  __m256i pl1_7    = _mm256_blendv_epi8(pl1_6, pl1_5, cmpmask4);

  __m128i res1_128 = _mm256_castsi256_si128(pl1_7);
  uint32_t tmp1 = (uint32_t)_mm_extract_epi32(res1_128, 0);
  uint16_t n = (uint16_t)(tmp1 & 0xffff);
  uint16_t chng = (uint16_t)(tmp1 >> 16);

  *final_change = (int16_t)chng;
  *min_pos = (int32_t)n;
}

static INLINE __m256i concatenate_2x128i(__m128i lo, __m128i hi)
{
  __m256i v = _mm256_castsi128_si256(lo);
  return _mm256_inserti128_si256(v, hi, 1);
}

static INLINE void scanord_read_vector_32(const int32_t  *__restrict quant_coeff,
                                          const uint32_t *__restrict scan,
                                          int8_t scan_mode,
                                          int32_t subpos,
                                          int32_t width,
                                          __m256i *__restrict v_quant_coeffs)
{
  const size_t row_offsets[4] = {
    scan[subpos] + width * 0,
    scan[subpos] + width * 1,
    scan[subpos] + width * 2,
    scan[subpos] + width * 3,
  };

  const __m256i shufmasks[3] = {
    _mm256_setr_epi32(5, 2, 6, 0, 3, 7, 4, 1),
    _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7),
    _mm256_setr_epi32(2, 3, 0, 1, 6, 7, 4, 5),
  };

  const __m256i blend_masks[3] = {
    _mm256_setr_epi32( 0,  0,  0, -1,  0,  0, -1, -1),
    _mm256_setr_epi32( 0,  0,  0,  0,  0,  0,  0,  0),
    _mm256_setr_epi32( 0,  0, -1, -1,  0,  0, -1, -1),
  };

  const __m256i rearr_masks_lo[3] = {
    _mm256_setr_epi32(0, 4, 1, 3, 5, 2, 6, 7),
    _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7),
    _mm256_setr_epi32(0, 4, 2, 6, 1, 5, 3, 7),
  };

  const __m256i rearr_masks_hi[3] = {
    _mm256_setr_epi32(6, 3, 0, 1, 7, 2, 4, 5),
    _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7),
    _mm256_setr_epi32(2, 6, 0, 4, 3, 7, 1, 5),
  };

  __m128i coeffs[4] = {
    _mm_loadu_si128((__m128i *)(quant_coeff + row_offsets[0])),
    _mm_loadu_si128((__m128i *)(quant_coeff + row_offsets[1])),
    _mm_loadu_si128((__m128i *)(quant_coeff + row_offsets[2])),
    _mm_loadu_si128((__m128i *)(quant_coeff + row_offsets[3])),
  };

  __m256i coeffs_upper = concatenate_2x128i(coeffs[0], coeffs[1]);
  __m256i coeffs_lower = concatenate_2x128i(coeffs[2], coeffs[3]);

  __m256i lower_shuffled = _mm256_permutevar8x32_epi32(coeffs_lower, shufmasks[scan_mode]);

  __m256i upper_blended  = _mm256_blendv_epi8(coeffs_upper,   lower_shuffled, blend_masks[scan_mode]);
  __m256i lower_blended  = _mm256_blendv_epi8(lower_shuffled, coeffs_upper,   blend_masks[scan_mode]);

  __m256i result_lo      = _mm256_permutevar8x32_epi32(upper_blended, rearr_masks_lo[scan_mode]);
  __m256i result_hi      = _mm256_permutevar8x32_epi32(lower_blended, rearr_masks_hi[scan_mode]);

  v_quant_coeffs[0] = result_lo;
  v_quant_coeffs[1] = result_hi;
}

#define VEC_WIDTH 16
#define SCAN_SET_SIZE 16
#define LOG2_SCAN_SET_SIZE 4

static INLINE int32_t hide_block_sign(__m256i coefs, __m256i q_coefs, __m256i deltas_h, __m256i deltas_l, coeff_t * __restrict q_coef, const uint32_t * __restrict scan, int32_t subpos, int32_t last_cg)
{
  assert(SCAN_SET_SIZE == 16);

  int32_t first_nz_pos_in_cg, last_nz_pos_in_cg;
  int32_t abssum = 0;

  // Find first and last nonzero coeffs
  get_first_last_nz_int16(q_coefs, &first_nz_pos_in_cg, &last_nz_pos_in_cg);

  // Sum all uvg_quant coeffs between first and last
  abssum = hsum32_16x16i(q_coefs);

  if (last_nz_pos_in_cg >= 0 && last_cg == -1) {
    last_cg = 1;
  }

  if (last_nz_pos_in_cg - first_nz_pos_in_cg >= 4) {

    uint32_t q_coef_signbits = _mm256_movemask_epi8(q_coefs);
    int32_t signbit = (q_coef_signbits >> (2 * first_nz_pos_in_cg + 1)) & 0x1;

    if (signbit != (abssum & 0x1)) { // compare signbit with sum_parity
      int32_t min_pos;
      int16_t final_change;
      int16_t cheapest_q;

      const int32_t mask_max = (last_cg == 1) ? last_nz_pos_in_cg : SCAN_SET_SIZE - 1;

      const __m256i zero = _mm256_setzero_si256();
      const __m256i ones = _mm256_set1_epi16(1);
      const __m256i maxiters = _mm256_set1_epi16(mask_max);
      const __m256i ff = _mm256_set1_epi8(0xff);

      const __m256i fnpics = _mm256_set1_epi16((int16_t)first_nz_pos_in_cg);
      const __m256i ns = _mm256_setr_epi16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

      __m256i block_signbit = _mm256_set1_epi16(((int16_t)signbit) * -1);
      __m256i coef_signbits = _mm256_cmpgt_epi16(zero, coefs);
      __m256i signbits_equal_block = _mm256_cmpeq_epi16(coef_signbits, block_signbit);

      __m256i q_coefs_zero = _mm256_cmpeq_epi16(q_coefs, zero);

      __m256i dus_packed = _mm256_packs_epi32(deltas_l, deltas_h);
      __m256i dus_ordered = _mm256_permute4x64_epi64(dus_packed, _MM_SHUFFLE(3, 1, 2, 0));
      __m256i dus_positive = _mm256_cmpgt_epi16(dus_ordered, zero);

      __m256i q_coef_abss = _mm256_abs_epi16(q_coefs);
      __m256i q_coefs_plusminus_one = _mm256_cmpeq_epi16(q_coef_abss, ones);

      __m256i eq_fnpics = _mm256_cmpeq_epi16(fnpics, ns);
      __m256i lt_fnpics = _mm256_cmpgt_epi16(fnpics, ns);

      __m256i maxcost_subcond1s = _mm256_and_si256(eq_fnpics, q_coefs_plusminus_one);
      __m256i maxcost_subcond2s = _mm256_andnot_si256(signbits_equal_block, lt_fnpics);
      __m256i elsecond1s_inv = _mm256_or_si256(dus_positive, maxcost_subcond1s);
      __m256i elsecond1s = _mm256_andnot_si256(elsecond1s_inv, ff);

      __m256i outside_maxiters = _mm256_cmpgt_epi16(ns, maxiters);

      __m256i negdelta_cond1s = _mm256_andnot_si256(q_coefs_zero, dus_positive);
      __m256i negdelta_cond2s = _mm256_andnot_si256(maxcost_subcond2s, q_coefs_zero);
      __m256i negdelta_mask16s_part1 = _mm256_or_si256(negdelta_cond1s, negdelta_cond2s);
      __m256i negdelta_mask16s = _mm256_andnot_si256(outside_maxiters, negdelta_mask16s_part1);

      __m256i posdelta_mask16s_part1 = _mm256_andnot_si256(q_coefs_zero, elsecond1s);
      __m256i posdelta_mask16s = _mm256_andnot_si256(outside_maxiters, posdelta_mask16s_part1);

      __m256i maxcost_cond1_parts = _mm256_andnot_si256(dus_positive, maxcost_subcond1s);
      __m256i maxcost_cond1s = _mm256_andnot_si256(q_coefs_zero, maxcost_cond1_parts);
      __m256i maxcost_cond2s = _mm256_and_si256(q_coefs_zero, maxcost_subcond2s);
      __m256i maxcost_mask16s_parts = _mm256_or_si256(maxcost_cond1s, maxcost_cond2s);
      __m256i maxcost_mask16s = _mm256_or_si256(maxcost_mask16s_parts, outside_maxiters);

      __m128i tmp_l, tmp_h;
      tmp_l = _mm256_extracti128_si256(negdelta_mask16s, 0);
      tmp_h = _mm256_extracti128_si256(negdelta_mask16s, 1);
      __m256i negdelta_mask32s_l = _mm256_cvtepi16_epi32(tmp_l);
      __m256i negdelta_mask32s_h = _mm256_cvtepi16_epi32(tmp_h);

      tmp_l = _mm256_extracti128_si256(posdelta_mask16s, 0);
      tmp_h = _mm256_extracti128_si256(posdelta_mask16s, 1);
      __m256i posdelta_mask32s_l = _mm256_cvtepi16_epi32(tmp_l);
      __m256i posdelta_mask32s_h = _mm256_cvtepi16_epi32(tmp_h);

      tmp_l = _mm256_extracti128_si256(maxcost_mask16s, 0);
      tmp_h = _mm256_extracti128_si256(maxcost_mask16s, 1);
      __m256i maxcost_mask32s_l = _mm256_cvtepi16_epi32(tmp_l);
      __m256i maxcost_mask32s_h = _mm256_cvtepi16_epi32(tmp_h);

      // Output value generation
      // cur_change_max: zero
      // cur_change_negdelta: ff
      // cur_change_posdelta: ones
      __m256i costs_negdelta_h = _mm256_sub_epi32(zero, deltas_h);
      __m256i costs_negdelta_l = _mm256_sub_epi32(zero, deltas_l);
      // costs_posdelta_l and _h: deltas_l and _h
      __m256i costs_max_lh = _mm256_set1_epi32(0x7fffffff);

      __m256i change_neg = _mm256_and_si256(negdelta_mask16s, ones);
      __m256i change_pos = _mm256_and_si256(posdelta_mask16s, ff);
      __m256i change_max = _mm256_and_si256(maxcost_mask16s, zero);

      __m256i cost_neg_l = _mm256_and_si256(negdelta_mask32s_l, costs_negdelta_l);
      __m256i cost_neg_h = _mm256_and_si256(negdelta_mask32s_h, costs_negdelta_h);
      __m256i cost_pos_l = _mm256_and_si256(posdelta_mask32s_l, deltas_l);
      __m256i cost_pos_h = _mm256_and_si256(posdelta_mask32s_h, deltas_h);
      __m256i cost_max_l = _mm256_and_si256(maxcost_mask32s_l, costs_max_lh);
      __m256i cost_max_h = _mm256_and_si256(maxcost_mask32s_h, costs_max_lh);

      __m256i changes = _mm256_or_si256(change_neg, _mm256_or_si256(change_pos, change_max));
      __m256i costs_l = _mm256_or_si256(cost_neg_l, _mm256_or_si256(cost_pos_l, cost_max_l));
      __m256i costs_h = _mm256_or_si256(cost_neg_h, _mm256_or_si256(cost_pos_h, cost_max_h));

      get_cheapest_alternative(costs_h, costs_l, ns, changes, &final_change, &min_pos);
      const int32_t best_id = scan[min_pos + subpos];

      cheapest_q = q_coef[best_id];
      if (cheapest_q == 32767 || cheapest_q == -32768)
        final_change = -1;

      uint32_t coef_signs = _mm256_movemask_epi8(coef_signbits);
      uint32_t cheapest_coef_sign_mask = (uint32_t)(1 << (2 * min_pos));

      if (!(coef_signs & cheapest_coef_sign_mask))
        cheapest_q += final_change;
      else
        cheapest_q -= final_change;

      q_coef[best_id] = cheapest_q;
    } // Hide
  }
  if (last_cg == 1)
    last_cg = 0;

  return last_cg;
}


static INLINE unsigned uvg_math_floor_log2(unsigned value)
{
  assert(value > 0);

  unsigned result = 0;

  for (int i = 4; i >= 0; --i) {
    unsigned bits = 1ull << i;
    unsigned shift = value >= (unsigned)(1 << bits) ? bits : 0;
    result += shift;
    value >>= shift;
  }

  return result;
}
/**
 * \brief quantize transformed coefficents
 *
 */
void uvg_quant_avx2(const encoder_state_t * const state, const coeff_t * __restrict coef, coeff_t * __restrict q_coef, int32_t width,
  int32_t height, color_t color, int8_t scan_idx, int8_t block_type, int8_t transform_skip, uint8_t lfnst_idx)
{
  const encoder_control_t * const encoder = state->encoder_control;
  const uint32_t log2_tr_width  = uvg_g_convert_to_log2[width];
  const uint32_t log2_tr_height = uvg_g_convert_to_log2[height];
  const uint32_t* const scan = uvg_get_scan_order_table(SCAN_GROUP_4X4, scan_idx, log2_tr_width, log2_tr_height, 0);

  int32_t qp_scaled = uvg_get_scaled_qp(color, state->qp, (encoder->bitdepth - 8) * 6, encoder->qp_map[0]);
  qp_scaled = transform_skip ? MAX(qp_scaled, 4 + 6 * MIN_QP_PRIME_TS) : qp_scaled;
  bool needs_block_size_trafo_scale = !transform_skip && ((log2_tr_height + log2_tr_width) % 2 == 1);
  needs_block_size_trafo_scale |= 0; // Non log2 block size

  const int32_t scalinglist_type = (block_type == CU_INTRA ? 0 : 3) + (int8_t)color;
  const int32_t *quant_coeff = encoder->scaling_list.quant_coeff[log2_tr_width][log2_tr_height][scalinglist_type][qp_scaled % 6];
  const int32_t transform_shift = MAX_TR_DYNAMIC_RANGE - encoder->bitdepth - ((log2_tr_width + log2_tr_height) >> 1); //!< Represents scaling through forward transform
  const int64_t q_bits = QUANT_SHIFT + qp_scaled / 6 + (transform_skip ? 0 : transform_shift - needs_block_size_trafo_scale);
  const int32_t add = ((state->frame->slicetype == UVG_SLICE_I) ? 171 : 85) << (q_bits - 9);
  const int32_t q_bits8 = q_bits - 8;

  const int32_t default_quant_coeff = uvg_g_quant_scales[needs_block_size_trafo_scale][qp_scaled % 6];

  uint32_t ac_sum = 0;
  int32_t last_cg = -1;

  __m256i v_ac_sum = _mm256_setzero_si256();

  // Loading once is enough if scaling lists are not off
  __m256i low_b = _mm256_setzero_si256(), high_b = _mm256_setzero_si256();
  if (!(state->encoder_control->scaling_list.enable)) {
    low_b  = _mm256_set1_epi32(default_quant_coeff);
    high_b = low_b;
  }

  if(lfnst_idx == 0) {
    for (int32_t n = 0; n < width * height; n += VEC_WIDTH) {

      __m256i v_level = _mm256_loadu_si256((__m256i *)(coef + n));
      __m256i v_sign = _mm256_cmpgt_epi16(_mm256_setzero_si256(), v_level);
      v_sign = _mm256_or_si256(v_sign, _mm256_set1_epi16(1));

      if (state->encoder_control->scaling_list.enable) {
        __m256i v_quant_coeff_lo = _mm256_loadu_si256(((__m256i *)(quant_coeff + n)) + 0);
        __m256i v_quant_coeff_hi = _mm256_loadu_si256(((__m256i *)(quant_coeff + n)) + 1);

        low_b  = _mm256_permute2x128_si256(v_quant_coeff_lo,
                                           v_quant_coeff_hi,
                                           0x20);

        high_b = _mm256_permute2x128_si256(v_quant_coeff_lo,
                                           v_quant_coeff_hi,
                                           0x31);
      }

      // TODO: do we need to have this?
      // #define CHECK_QUANT_COEFFS
      #ifdef CHECK_QUANT_COEFFS
      __m256i abs_vq_lo = _mm256_abs_epi32(v_quant_coeff_lo);
      __m256i abs_vq_hi = _mm256_abs_epi32(v_quant_coeff_hi);

      __m256i vq_over_16b_lo = _mm256_cmpgt_epi32(abs_vq_lo, _mm256_set1_epi32(0x7fff));
      __m256i vq_over_16b_hi = _mm256_cmpgt_epi32(abs_vq_hi, _mm256_set1_epi32(0x7fff));

      uint32_t over_16b_mask_lo = _mm256_movemask_epi8(vq_over_16b_lo);
      uint32_t over_16b_mask_hi = _mm256_movemask_epi8(vq_over_16b_hi);

      assert(!(over_16b_mask_lo || over_16b_mask_hi));
#endif

      v_level = _mm256_abs_epi16(v_level);
      __m256i low_a  = _mm256_unpacklo_epi16(v_level, _mm256_setzero_si256());
      __m256i high_a = _mm256_unpackhi_epi16(v_level, _mm256_setzero_si256());

      __m256i v_level32_a = _mm256_mullo_epi32(low_a,  low_b);
      __m256i v_level32_b = _mm256_mullo_epi32(high_a, high_b);

      v_level32_a = _mm256_add_epi32(v_level32_a, _mm256_set1_epi32(add));
      v_level32_b = _mm256_add_epi32(v_level32_b, _mm256_set1_epi32(add));

      v_level32_a = _mm256_srai_epi32(v_level32_a, q_bits);
      v_level32_b = _mm256_srai_epi32(v_level32_b, q_bits);

      v_level = _mm256_packs_epi32(v_level32_a, v_level32_b);
      v_level = _mm256_sign_epi16(v_level, v_sign);

      _mm256_storeu_si256((__m256i *)(q_coef + n), v_level);

      v_ac_sum = _mm256_add_epi32(v_ac_sum, v_level32_a);
      v_ac_sum = _mm256_add_epi32(v_ac_sum, v_level32_b);
    }
  }
  else {
    const int max_number_of_coeffs = ((width == 4 && height == 4) || (width == 8 && height == 8)) ? 8 : 16;
    memset(q_coef, 0, width * height * sizeof(coeff_t));
    for (int32_t n = 0; n < max_number_of_coeffs; n++) {
      const uint32_t idx = scan[n];
      int32_t level = coef[idx];
      int64_t abs_level = (int64_t)abs(level);
      int32_t sign;

      sign = (level < 0 ? -1 : 1);

      int32_t curr_quant_coeff = quant_coeff[n];
      level = (abs_level * curr_quant_coeff + add) >> q_bits;
      ac_sum += level;

      level *= sign;
      q_coef[idx] = (coeff_t)(CLIP(-32768, 32767, level));
    }
  }

  __m128i temp = _mm_add_epi32(_mm256_castsi256_si128(v_ac_sum), _mm256_extracti128_si256(v_ac_sum, 1));
  temp = _mm_add_epi32(temp, _mm_shuffle_epi32(temp, _MM_SHUFFLE(1, 0, 3, 2)));
  temp = _mm_add_epi32(temp, _mm_shuffle_epi32(temp, _MM_SHUFFLE(0, 1, 0, 1)));
  ac_sum += _mm_cvtsi128_si32(temp);

  // Signhiding disabled in VVC
  if (!encoder->cfg.signhide_enable || ac_sum < 2)
    return;

  assert(VEC_WIDTH == SCAN_SET_SIZE);
  for (int32_t subpos = (width * height - 1) & (~(VEC_WIDTH - 1)); subpos >= 0; subpos -= VEC_WIDTH) {
    const int16_t *coeffs[2] = {coef, q_coef};
    __m256i result_coeffs[2];
    __m256i v_quant_coeffs[2];

    __m256i v_coef, q_coefs;
    __m256i v_quant_coeff_lo, v_quant_coeff_hi;

    scanord_read_vector(coeffs, scan, scan_idx, subpos, width, result_coeffs, 2);

    v_coef  = result_coeffs[0];
    q_coefs = result_coeffs[1];

    if (state->encoder_control->scaling_list.enable) {
      scanord_read_vector_32(quant_coeff, scan, scan_idx, subpos, width, v_quant_coeffs);

      v_quant_coeff_lo = v_quant_coeffs[0];
      v_quant_coeff_hi = v_quant_coeffs[1];

      low_b  = _mm256_permute2x128_si256(v_quant_coeff_lo,
                                         v_quant_coeff_hi,
                                         0x20);

      high_b = _mm256_permute2x128_si256(v_quant_coeff_lo,
                                         v_quant_coeff_hi,
                                         0x31);
    }

    __m256i v_level = _mm256_abs_epi16(v_coef);
    __m256i low_a  = _mm256_unpacklo_epi16(v_level, _mm256_setzero_si256());
    __m256i high_a = _mm256_unpackhi_epi16(v_level, _mm256_setzero_si256());

    __m256i v_quant_coeff_a = _mm256_or_si256(low_b,  _mm256_setzero_si256());
    __m256i v_quant_coeff_b = _mm256_or_si256(high_b, _mm256_setzero_si256());

    __m256i v_level32_a = _mm256_mullo_epi32(low_a,  low_b);
    __m256i v_level32_b = _mm256_mullo_epi32(high_a, high_b);

    v_level32_a = _mm256_add_epi32(v_level32_a, _mm256_set1_epi32(add));
    v_level32_b = _mm256_add_epi32(v_level32_b, _mm256_set1_epi32(add));

    v_level32_a = _mm256_srai_epi32(v_level32_a, q_bits);
    v_level32_b = _mm256_srai_epi32(v_level32_b, q_bits);

    v_level = _mm256_packs_epi32(v_level32_a, v_level32_b);

    __m256i v_coef_a = _mm256_unpacklo_epi16(_mm256_abs_epi16(v_coef), _mm256_set1_epi16(0));
    __m256i v_coef_b = _mm256_unpackhi_epi16(_mm256_abs_epi16(v_coef), _mm256_set1_epi16(0));

    v_coef_a = _mm256_mullo_epi32(v_coef_a, v_quant_coeff_a);
    v_coef_b = _mm256_mullo_epi32(v_coef_b, v_quant_coeff_b);

    v_coef_a = _mm256_sub_epi32(v_coef_a, _mm256_slli_epi32(_mm256_unpacklo_epi16(v_level, _mm256_set1_epi16(0)), q_bits) );
    v_coef_b = _mm256_sub_epi32(v_coef_b, _mm256_slli_epi32(_mm256_unpackhi_epi16(v_level, _mm256_set1_epi16(0)), q_bits) );
    v_coef_a = _mm256_srai_epi32(v_coef_a, q_bits8);
    v_coef_b = _mm256_srai_epi32(v_coef_b, q_bits8);

    __m256i deltas_h = _mm256_permute2x128_si256(v_coef_a, v_coef_b, 0x31);
    __m256i deltas_l = _mm256_permute2x128_si256(v_coef_a, v_coef_b, 0x20);

    last_cg = hide_block_sign(v_coef, q_coefs, deltas_h, deltas_l, q_coef, scan, subpos, last_cg);
  }

#undef VEC_WIDTH
#undef SCAN_SET_SIZE
#undef LOG2_SCAN_SET_SIZE

}

#if UVG_BIT_DEPTH == 8


static INLINE int32_t get_quantized_recon_4x1_avx2(int16_t *residual, const uint8_t *pred_in){
  __m128i res = _mm_loadl_epi64((__m128i*)residual);
  __m128i pred = _mm_cvtsi32_si128(*(int32_t*)pred_in);
  __m128i rec = _mm_add_epi16(res, _mm_cvtepu8_epi16(pred));
  return _mm_cvtsi128_si32(_mm_packus_epi16(rec, rec));
}

static INLINE int64_t get_quantized_recon_8x1_avx2(int16_t *residual, const uint8_t *pred_in){
  __m128i res = _mm_loadu_si128((__m128i*)residual);
  __m128i pred = _mm_cvtsi64_si128(*(int64_t*)pred_in);
  __m128i rec = _mm_add_epi16(res, _mm_cvtepu8_epi16(pred));
  return _mm_cvtsi128_si64(_mm_packus_epi16(rec, rec));
}

static void get_quantized_recon_avx2(int16_t *residual, const uint8_t *pred_in, int in_stride, uint8_t *rec_out, int out_stride, int width, int height){

  if (height == width || width >= 16) {
    switch (width) {
    case 4:
      *(int32_t*) & (rec_out[0 * out_stride]) = get_quantized_recon_4x1_avx2(residual + 0 * width, pred_in + 0 * in_stride);
      *(int32_t*)& (rec_out[1 * out_stride]) = get_quantized_recon_4x1_avx2(residual + 1 * width, pred_in + 1 * in_stride);
      *(int32_t*)& (rec_out[2 * out_stride]) = get_quantized_recon_4x1_avx2(residual + 2 * width, pred_in + 2 * in_stride);
      *(int32_t*)& (rec_out[3 * out_stride]) = get_quantized_recon_4x1_avx2(residual + 3 * width, pred_in + 3 * in_stride);
      break;
    case 8:
      *(int64_t*) & (rec_out[0 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 0 * width, pred_in + 0 * in_stride);
      *(int64_t*)& (rec_out[1 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 1 * width, pred_in + 1 * in_stride);
      *(int64_t*)& (rec_out[2 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 2 * width, pred_in + 2 * in_stride);
      *(int64_t*)& (rec_out[3 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 3 * width, pred_in + 3 * in_stride);
      *(int64_t*)& (rec_out[4 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 4 * width, pred_in + 4 * in_stride);
      *(int64_t*)& (rec_out[5 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 5 * width, pred_in + 5 * in_stride);
      *(int64_t*)& (rec_out[6 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 6 * width, pred_in + 6 * in_stride);
      *(int64_t*)& (rec_out[7 * out_stride]) = get_quantized_recon_8x1_avx2(residual + 7 * width, pred_in + 7 * in_stride);
      break;
    default:
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; x += 16) {
          *(int64_t*)& (rec_out[x + y * out_stride]) = get_quantized_recon_8x1_avx2(residual + x + y * width, pred_in + x + y * in_stride);
          *(int64_t*)& (rec_out[(x + 8) + y * out_stride]) = get_quantized_recon_8x1_avx2(residual + (x + 8) + y * width, pred_in + (x + 8) + y * in_stride);
        }
      }
      break;
    }
  }
  else {
    switch (width) {
    case 4:
      for (int y = 0; y < height; y += 4) {
        *(int32_t*)& (rec_out[(y + 0) * out_stride]) = get_quantized_recon_4x1_avx2(residual + (y + 0) * width, pred_in + (y + 0) * in_stride);
        *(int32_t*)& (rec_out[(y + 1) * out_stride]) = get_quantized_recon_4x1_avx2(residual + (y + 1) * width, pred_in + (y + 1) * in_stride);
        *(int32_t*)& (rec_out[(y + 2) * out_stride]) = get_quantized_recon_4x1_avx2(residual + (y + 2) * width, pred_in + (y + 2) * in_stride);
        *(int32_t*)& (rec_out[(y + 3) * out_stride]) = get_quantized_recon_4x1_avx2(residual + (y + 3) * width, pred_in + (y + 3) * in_stride);
      }
      break;
    case 8:
      for (int y = 0; y < height; ++y) {
        *(int32_t*)& (rec_out[y * out_stride]) = get_quantized_recon_8x1_avx2(residual + y * width, pred_in + y * in_stride);
      }
      break;
    default:
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          int16_t val = residual[x + y * width] + pred_in[x + y * in_stride];
          rec_out[x + y * out_stride] = (uvg_pixel)CLIP(0, PIXEL_MAX, val);
        }
      }
      break;
    }
  }
}

/**
* \brief Quantize residual and get both the reconstruction and coeffs.
*
* \param width  Transform width.
* \param color  Color.
* \param scan_order  Coefficient scan order.
* \param use_trskip  Whether transform skip is used.
* \param stride  Stride for ref_in, pred_in and rec_out.
* \param ref_in  Reference pixels.
* \param pred_in  Predicted pixels.
* \param rec_out  Reconstructed pixels.
* \param coeff_out  Coefficients used for reconstruction of rec_out.
* \param early_skip if this is used for early skip, bypass IT and IQ
*
* \returns  Whether coeff_out contains any non-zero coefficients.
*/
int uvg_quantize_residual_avx2(encoder_state_t *const state,
  const cu_info_t *const cur_cu, const int width, const int height, const color_t color,
  const coeff_scan_order_t scan_order, const int use_trskip,
  const int in_stride, const int out_stride,
  const uint8_t *const ref_in, const uint8_t *const pred_in,
  uint8_t *rec_out, coeff_t *coeff_out,
  bool early_skip, int lmcs_chroma_adj,
  enum uvg_tree_type tree_type)
{
  // Temporary arrays to pass data to and from uvg_quant and transform functions.
  ALIGNED(64) int16_t residual[TR_MAX_WIDTH * TR_MAX_WIDTH];
  ALIGNED(64) coeff_t coeff[TR_MAX_WIDTH * TR_MAX_WIDTH];
  // ISP_TODO: non-square block implementation, height is passed but not used

  int has_coeffs = 0;

  assert(width <= TR_MAX_WIDTH);
  assert(width >= TR_MIN_WIDTH);

  // Get residual. (ref_in - pred_in -> residual)
  uvg_generate_residual(ref_in, pred_in, residual, width, height, in_stride, in_stride);

  if (state->tile->frame->lmcs_aps->m_sliceReshapeInfo.enableChromaAdj && color != COLOR_Y) {
    int y, x;
    int sign, absval;
    int maxAbsclipBD = (1 << UVG_BIT_DEPTH) - 1;
    for (y = 0; y < width; ++y) {
      for (x = 0; x < width; ++x) {
        sign = residual[x + y * width] >= 0 ? 1 : -1;
        absval = sign * residual[x + y * width];
        residual[x + y * width] = (int16_t)CLIP(-maxAbsclipBD, maxAbsclipBD, sign * (((absval << CSCALE_FP_PREC) + (lmcs_chroma_adj >> 1)) / lmcs_chroma_adj));
      }
    }
  }

  // Transform residual. (residual -> coeff)
  if (use_trskip) {
    uvg_transformskip(state->encoder_control, residual, coeff, width, height);
  }
  else {
    uvg_transform2d(state->encoder_control, residual, coeff, width, height, color, cur_cu);
  }

  const uint16_t lfnst_index = color == COLOR_Y ? cur_cu->lfnst_idx : cur_cu->cr_lfnst_idx;

  if (state->encoder_control->cfg.lfnst && cur_cu->type == CU_INTRA) {
    // Forward low frequency non-separable transform
    uvg_fwd_lfnst(cur_cu, width, height, color, lfnst_index, coeff, tree_type, state->collocated_luma_mode);
  }

  // Quantize coeffs. (coeff -> coeff_out)
  int abs_sum = 0;
  if(!use_trskip && state->encoder_control->cfg.dep_quant) {
    uvg_dep_quant(
      state,
      cur_cu,
      width,
      height,
      coeff,
      coeff_out,
      color,
      tree_type,
      &abs_sum,
      state->encoder_control->cfg.scaling_list);
  }
  else if (state->encoder_control->cfg.rdoq_enable &&
      (width > 4 || !state->encoder_control->cfg.rdoq_skip) && !use_trskip)
  {
    uvg_rdoq(state, coeff, coeff_out, width, height, color,
      scan_order, cur_cu->type, cur_cu->cbf, lfnst_index, color == 0 ? cur_cu->tr_idx : 0);
  }
  else if (state->encoder_control->cfg.rdoq_enable && use_trskip) {
    uvg_ts_rdoq(state, coeff, coeff_out, width, height, color,
      scan_order);
  }
  else {
    uvg_quant(state, coeff, coeff_out, width, height, color,
      scan_order, cur_cu->type, cur_cu->tr_idx == MTS_SKIP && color == COLOR_Y, lfnst_index);
  }

  // Check if there are any non-zero coefficients.
  for (int i = 0; i < width * height; i += 8) {
    __m128i v_quant_coeff = _mm_loadu_si128((__m128i*)&(coeff_out[i]));
    has_coeffs = !_mm_testz_si128(_mm_set1_epi8(0xFF), v_quant_coeff);
    if(has_coeffs) break;
  }

  // Do the inverse quantization and transformation and the reconstruction to
  // rec_out.
  if (has_coeffs && !early_skip) {
    // Get quantized residual. (coeff_out -> coeff -> residual)
    uvg_dequant(state, coeff_out, coeff, width, height, color,
      cur_cu->type, cur_cu->tr_idx == MTS_SKIP && color == COLOR_Y);

    if (state->encoder_control->cfg.lfnst && cur_cu->type == CU_INTRA) {
      // Inverse low frequency non-separable transform
      uvg_inv_lfnst(cur_cu, width, height, color, lfnst_index, coeff, tree_type, state->collocated_luma_mode);
    }
    if (use_trskip) {
      uvg_itransformskip(state->encoder_control, residual, coeff, width, height);
    }
    else {
      uvg_itransform2d(state->encoder_control, residual, coeff, width, height, color, cur_cu);
    }

    if (state->tile->frame->lmcs_aps->m_sliceReshapeInfo.enableChromaAdj && color != COLOR_Y) {
      int y, x;
      int sign, absval;
      int maxAbsclipBD = (1 << UVG_BIT_DEPTH) - 1;
      for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
          residual[x + y * width] = (int16_t)CLIP((int16_t)(-maxAbsclipBD - 1), (int16_t)maxAbsclipBD, residual[x + y * width]);
          sign = residual[x + y * width] >= 0 ? 1 : -1;
          absval = sign * residual[x + y * width];
          int val = sign * ((absval * lmcs_chroma_adj + (1 << (CSCALE_FP_PREC - 1))) >> CSCALE_FP_PREC);
          if (sizeof(uvg_pixel) == 2) // avoid overflow when storing data
          {
            val = CLIP(-32768, 32767, val);
          }
          residual[x + y * width] = (int16_t)val;
        }
      }
    }

    // Get quantized reconstruction. (residual + pred_in -> rec_out)
    get_quantized_recon_avx2(residual, pred_in, in_stride, rec_out, out_stride, width, height);
  }
  else if (rec_out != pred_in) {
    // With no coeffs and rec_out == pred_int we skip copying the coefficients
    // because the reconstruction is just the prediction.
    int y, x;

    for (y = 0; y < height; ++y) {
      for (x = 0; x < width; ++x) {
        rec_out[x + y * out_stride] = pred_in[x + y * in_stride];
      }
    }
  }

  return has_coeffs;
}

/**
 * \brief inverse quantize transformed and quantized coefficents
 *
 */
void uvg_dequant_avx2(const encoder_state_t * const state, coeff_t *q_coef, coeff_t *coef, int32_t width, int32_t height,color_t color, int8_t block_type, int8_t transform_skip)
{
  const encoder_control_t * const encoder = state->encoder_control;
  if (encoder->cfg.dep_quant && !transform_skip) {
    uvg_dep_quant_dequant(state, block_type, width, height, color, q_coef, coef, encoder->cfg.scaling_list);
    return;
  }
  int32_t shift,add,coeff_q;
  int32_t n;
  const uint32_t log2_tr_width =  uvg_g_convert_to_log2[width];
  const uint32_t log2_tr_height = uvg_g_convert_to_log2[height];
  int32_t transform_shift = MAX_TR_DYNAMIC_RANGE - encoder->bitdepth - ((log2_tr_width + log2_tr_height) >> 1);
  bool needs_block_size_trafo_scale = !transform_skip && ((log2_tr_height + log2_tr_width) % 2 == 1);
  needs_block_size_trafo_scale |= 0; // Non log2 block size// Represents scaling through forward transform


  int32_t qp_scaled = uvg_get_scaled_qp(color, state->qp, (encoder->bitdepth-8)*6, encoder->qp_map[0]);
  qp_scaled = transform_skip ? MAX(qp_scaled, 4 + 6 * MIN_QP_PRIME_TS) : qp_scaled;

  shift = 20 - QUANT_SHIFT - (transform_skip ? 0 : transform_shift - needs_block_size_trafo_scale);

  if (encoder->scaling_list.enable)
  {
    int32_t scalinglist_type = (block_type == CU_INTRA ? 0 : 3) + (int8_t)(color);

    const int32_t* dequant_coef = encoder->scaling_list.de_quant_coeff[log2_tr_width - 2][log2_tr_height - 2][scalinglist_type][qp_scaled % 6];
    shift += 4;

    if (shift >qp_scaled / 6) {
      add = 1 << (shift - qp_scaled/6 - 1);

      for (n = 0; n < width * height; n++) {
        coeff_q = ((q_coef[n] * dequant_coef[n]) + add ) >> (shift -  qp_scaled/6);
        coef[n] = (coeff_t)CLIP(-32768,32767,coeff_q);
      }
    } else {
      for (n = 0; n < width * height; n++) {
        // Clip to avoid possible overflow in following shift left operation
        coeff_q   = CLIP(-32768, 32767, q_coef[n] * dequant_coef[n]);
        coef[n] = (coeff_t)CLIP(-32768, 32767, coeff_q << (qp_scaled/6 - shift));
      }
    }
  } else {
    int32_t scale = uvg_g_inv_quant_scales[needs_block_size_trafo_scale][qp_scaled%6] << (qp_scaled/6);
    add = 1 << (shift-1);

    __m256i v_scale = _mm256_set1_epi32(scale);
    __m256i v_add = _mm256_set1_epi32(add);

    for (n = 0; n < width*height; n+=16) {
      __m128i temp0 = _mm_loadu_si128((__m128i*)&(q_coef[n]));
      __m128i temp1 = _mm_loadu_si128((__m128i*)&(q_coef[n + 8]));
      __m256i v_coeff_q_lo = _mm256_cvtepi16_epi32(_mm_unpacklo_epi64(temp0, temp1));
      __m256i v_coeff_q_hi = _mm256_cvtepi16_epi32(_mm_unpackhi_epi64(temp0, temp1));
      v_coeff_q_lo = _mm256_mullo_epi32(v_coeff_q_lo, v_scale);
      v_coeff_q_hi = _mm256_mullo_epi32(v_coeff_q_hi, v_scale);
      v_coeff_q_lo = _mm256_add_epi32(v_coeff_q_lo, v_add);
      v_coeff_q_hi = _mm256_add_epi32(v_coeff_q_hi, v_add);
      v_coeff_q_lo = _mm256_srai_epi32(v_coeff_q_lo, shift);
      v_coeff_q_hi = _mm256_srai_epi32(v_coeff_q_hi, shift);
      v_coeff_q_lo = _mm256_packs_epi32(v_coeff_q_lo, v_coeff_q_hi);
      _mm_storeu_si128((__m128i*)&(coef[n]), _mm256_castsi256_si128(v_coeff_q_lo) );
      _mm_storeu_si128((__m128i*)&(coef[n + 8]), _mm256_extracti128_si256(v_coeff_q_lo, 1) );
    }
  }
}

#endif // UVG_BIT_DEPTH == 8

static uint32_t coeff_abs_sum_avx2(const coeff_t *coeffs, const size_t length)
{
  assert(length % 8 == 0);

  __m256i total = _mm256_abs_epi32(_mm256_cvtepi16_epi32(_mm_loadu_si128((__m128i*) coeffs)));

  for (int i = 8; i < length; i += 8) {
    __m256i temp = _mm256_abs_epi32(_mm256_cvtepi16_epi32(_mm_loadu_si128((__m128i*) &coeffs[i])));
    total = _mm256_add_epi32(total, temp);
  }

  __m128i result128 = _mm_add_epi32(
    _mm256_castsi256_si128(total),
    _mm256_extractf128_si256(total, 1)
  );

  uint32_t parts[4];
  _mm_storeu_si128((__m128i*) parts, result128);

  return parts[0] + parts[1] + parts[2] + parts[3];
}

static uint32_t fast_coeff_cost_avx2(const coeff_t *coeff, int32_t width, int32_t height, uint64_t weights)
{
  assert((width == height) && "Non-square block handling not implemented for this function.");
  const __m256i zero           = _mm256_setzero_si256();
  const __m256i threes         = _mm256_set1_epi16(3);
  const __m256i negate_hibytes = _mm256_set1_epi16(0xff00);
  const __m128i wt_extract_los = _mm_cvtsi32_si128(0x06040200);
  const __m128i wt_extract_his = _mm_cvtsi32_si128(0x07050301);

  __m256i lo_sum     = _mm256_setzero_si256();
  __m256i hi_sum     = _mm256_setzero_si256();

  __m128i wts_128    = _mm_loadl_epi64 ((const __m128i *)&weights);
  __m128i wts_lo_128 = _mm_shuffle_epi8(wts_128, wt_extract_los);
  __m128i wts_hi_128 = _mm_shuffle_epi8(wts_128, wt_extract_his);

  __m256i wts_lo     = _mm256_broadcastsi128_si256(wts_lo_128);
  __m256i wts_hi     = _mm256_broadcastsi128_si256(wts_hi_128);

  for (int i = 0; i < width * height; i += 32) {
    __m256i curr_lo      = _mm256_loadu_si256 ((const __m256i *)(coeff + i));
    __m256i curr_abs_lo  = _mm256_abs_epi16   (curr_lo);
    __m256i curr_max3_lo = _mm256_min_epu16   (curr_abs_lo, threes);

    // 4x4 blocks only have 16 coeffs, so handle them separately
    __m256i curr_max3_hi;
    if (width >= 8) {
      __m256i curr_hi      = _mm256_loadu_si256 ((const __m256i *)(coeff + i + 16));
      __m256i curr_abs_hi  = _mm256_abs_epi16   (curr_hi);
              curr_max3_hi = _mm256_min_epu16   (curr_abs_hi, threes);
              curr_max3_hi = _mm256_slli_epi16  (curr_max3_hi, 8);
    } else {
      // Set MSBs for high bytes if they're meaningless, so shuffles will
      // return zeros for them
      curr_max3_hi = negate_hibytes;
    }
    __m256i curr_max3    = _mm256_or_si256    (curr_max3_lo, curr_max3_hi);
    __m256i curr_wts_lo  = _mm256_shuffle_epi8(wts_lo, curr_max3);
    __m256i curr_wts_hi  = _mm256_shuffle_epi8(wts_hi, curr_max3);

    __m256i curr_sum_lo  = _mm256_sad_epu8    (curr_wts_lo, zero);
    __m256i curr_sum_hi  = _mm256_sad_epu8    (curr_wts_hi, zero);

            lo_sum       = _mm256_add_epi64   (lo_sum, curr_sum_lo);
            hi_sum       = _mm256_add_epi64   (hi_sum, curr_sum_hi);
  }
          hi_sum = _mm256_slli_epi64(hi_sum, 8);
  __m256i sum0   = _mm256_add_epi64(lo_sum, hi_sum);

  __m256i sum1   = _mm256_permute4x64_epi64(sum0, _MM_SHUFFLE(1, 0, 3, 2));
  __m256i sum2   = _mm256_add_epi64        (sum0, sum1);
  __m256i sum3   = _mm256_shuffle_epi32    (sum2, _MM_SHUFFLE(1, 0, 3, 2));
  __m256i sum4   = _mm256_add_epi64        (sum2, sum3);

  __m128i sum128 = _mm256_castsi256_si128  (sum4);
  return (_mm_cvtsi128_si32(sum128) + (1 << 7)) >> 8;
}

#endif //COMPILE_INTEL_AVX2 && defined X86_64

int uvg_strategy_register_quant_avx2(void* opaque, uint8_t bitdepth)
{
  bool success = true;

#if COMPILE_INTEL_AVX2 && defined X86_64
#if UVG_BIT_DEPTH == 8
  if (bitdepth == 8) {
    success &= uvg_strategyselector_register(opaque, "quantize_residual", "avx2", 40, &uvg_quantize_residual_avx2);
    success &= uvg_strategyselector_register(opaque, "dequant", "avx2", 40, &uvg_dequant_avx2);
  }
#endif // UVG_BIT_DEPTH == 8
  success &= uvg_strategyselector_register(opaque, "quant", "avx2", 40, &uvg_quant_avx2);
  success &= uvg_strategyselector_register(opaque, "coeff_abs_sum", "avx2", 0, &coeff_abs_sum_avx2);
  success &= uvg_strategyselector_register(opaque, "fast_coeff_cost", "avx2", 40, &fast_coeff_cost_avx2);
#endif //COMPILE_INTEL_AVX2 && defined X86_64

  return success;
}
