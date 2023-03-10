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

#include "../global.h"

#include "alf-avx2.h"

#if COMPILE_INTEL_AVX2
#include "../uvg266.h"
#if UVG_BIT_DEPTH == 8

#include <immintrin.h>
#include <stdlib.h>

#include "../strategyselector.h"

#define ALF_CLIP_AND_ADD(VAL0,VAL1) __m128i clips = _mm_loadl_epi64((__m128i*) clip); \
__m128i neg_clips = _mm_sign_epi16(clips, negate); \
__m128i val0 = _mm_set1_epi16((VAL0 - curr));\
__m128i val1 = _mm_set1_epi16((VAL1 - curr));\
__m128i min_clips_val0 = _mm_min_epi16(clips, val0);\
__m128i max_clips_val0 = _mm_max_epi16(min_clips_val0, neg_clips);\
\
__m128i min_clips_val1 = _mm_min_epi16(clips, val1);\
__m128i max_clips_val1 = _mm_max_epi16(min_clips_val1, neg_clips);\
\
__m128i e_local_original = _mm_loadl_epi64((__m128i*) & e_local[filter_pattern[k]][0]);\
__m128i result = _mm_add_epi16(e_local_original, _mm_add_epi16(max_clips_val0, max_clips_val1));\
_mm_storel_epi64((__m128i*)& e_local[filter_pattern[k]][0], result);

static void alf_calc_covariance_avx2(int16_t e_local[MAX_NUM_ALF_LUMA_COEFF][MAX_ALF_NUM_CLIPPING_VALUES],
  const uvg_pixel* rec,
  const int stride,
  const channel_type channel,
  const int transpose_idx,
  int vb_distance,
  short alf_clipping_values[MAX_NUM_CHANNEL_TYPE][MAX_ALF_NUM_CLIPPING_VALUES])
{
  static const int alf_pattern_5[13] = {
              0,
          1,  2,  3,
      4,  5,  6,  5,  4,
          3,  2,  1,
              0
  };

  static const int alf_pattern_7[25] = {
                0,
            1,  2,  3,
        4,  5,  6,  7,  8,
    9, 10, 11, 12, 11, 10, 9,
        8,  7,  6,  5,  4,
            3,  2,  1,
                0
  };

  int clip_top_row = -4;
  int clip_bot_row = 4;
  if (vb_distance >= -3 && vb_distance < 0)
  {
    clip_bot_row = -vb_distance - 1;
    clip_top_row = -clip_bot_row; // symmetric
  }
  else if (vb_distance >= 0 && vb_distance < 3)
  {
    clip_top_row = -vb_distance;
    clip_bot_row = -clip_top_row; // symmetric
  }

  const bool is_luma = channel == CHANNEL_TYPE_LUMA;
  const int* filter_pattern = is_luma ? alf_pattern_7 : alf_pattern_5;
  const int half_filter_length = (is_luma ? 7 : 5) >> 1;
  const short* clip = alf_clipping_values[channel];
  //const int num_bins = MAX_ALF_NUM_CLIPPING_VALUES;

  int k = 0;

  const int16_t curr = rec[0];

  const __m128i negate = _mm_setr_epi16(-1, -1, -1, -1, -1, -1, -1, -1);

  if (transpose_idx == 0)
  {
    for (int i = -half_filter_length; i < 0; i++)
    {
      const uvg_pixel* rec0 = rec + MAX(i, clip_top_row) * stride;
      const uvg_pixel* rec1 = rec - MAX(i, -clip_bot_row) * stride;
      for (int j = -half_filter_length - i; j <= half_filter_length + i; j++, k++)
      {
        ALF_CLIP_AND_ADD(rec0[j], rec1[-j]);
      }
    }
    for (int j = -half_filter_length; j < 0; j++, k++)
    {
      ALF_CLIP_AND_ADD(rec[j], rec[-j]);
    }
  }
  else if (transpose_idx == 1)
  {
    for (int j = -half_filter_length; j < 0; j++)
    {
      const uvg_pixel* rec0 = rec + j;
      const uvg_pixel* rec1 = rec - j;

      for (int i = -half_filter_length - j; i <= half_filter_length + j; i++, k++)
      {
        ALF_CLIP_AND_ADD(rec0[MAX(i, clip_top_row) * stride], rec1[-MAX(i, -clip_bot_row) * stride]);
      }
    }
    for (int i = -half_filter_length; i < 0; i++, k++)
    {
      ALF_CLIP_AND_ADD(rec[MAX(i, clip_top_row) * stride], rec[-MAX(i, -clip_bot_row) * stride]);
    }
  }
  else if (transpose_idx == 2)
  {
    for (int i = -half_filter_length; i < 0; i++)
    {
      const uvg_pixel* rec0 = rec + MAX(i, clip_top_row) * stride;
      const uvg_pixel* rec1 = rec - MAX(i, -clip_bot_row) * stride;

      for (int j = half_filter_length + i; j >= -half_filter_length - i; j--, k++)
      {
        ALF_CLIP_AND_ADD(rec0[j], rec1[-j]);
      }
    }
    for (int j = -half_filter_length; j < 0; j++, k++)
    {
      ALF_CLIP_AND_ADD(rec[j], rec[-j]);
    }
  }
  else
  {
    for (int j = -half_filter_length; j < 0; j++)
    {
      const uvg_pixel* rec0 = rec + j;
      const uvg_pixel* rec1 = rec - j;

      for (int i = half_filter_length + j; i >= -half_filter_length - j; i--, k++)
      {
        ALF_CLIP_AND_ADD(rec0[MAX(i, clip_top_row) * stride], rec1[-MAX(i, -clip_bot_row) * stride]);
      }
    }
    for (int i = -half_filter_length; i < 0; i++, k++)
    {
      ALF_CLIP_AND_ADD(rec[MAX(i, clip_top_row) * stride], rec[-MAX(i, -clip_bot_row) * stride]);
    }
  }

  __m128i e_local_original = _mm_loadl_epi64((__m128i*) & e_local[filter_pattern[k]][0]);
  __m128i result = _mm_add_epi16(e_local_original, _mm_set1_epi16(curr));
  _mm_storel_epi64((__m128i*)& e_local[filter_pattern[k]][0], result);

}

static void alf_get_blk_stats_avx2(encoder_state_t* const state,
  channel_type channel,
  alf_covariance* alf_covariance,
  alf_classifier** g_classifier,
  uvg_pixel* org,
  int32_t org_stride,
  uvg_pixel* rec,
  int32_t rec_stride,
  const int x_pos,
  const int y_pos,
  const int x_dst,
  const int y_dst,
  const int width,
  const int height,
  int vb_ctu_height,
  int vb_pos,
  short alf_clipping_values[MAX_NUM_CHANNEL_TYPE][MAX_ALF_NUM_CLIPPING_VALUES])
{
  int16_t e_local[MAX_NUM_ALF_LUMA_COEFF][MAX_ALF_NUM_CLIPPING_VALUES];

  //const int num_bins = MAX_ALF_NUM_CLIPPING_VALUES;

  int num_coeff = channel == CHANNEL_TYPE_LUMA ? 13 : 7;
  int transpose_idx = 0;
  int class_idx = 0;

  for (int i = 0; i < height; i++)
  {
    int vb_distance = ((y_dst + i) % vb_ctu_height) - vb_pos;
    for (int j = 0; j < width; j++)
    {
      if (g_classifier && g_classifier[y_dst + i][x_dst + j].class_idx == ALF_UNUSED_CLASS_IDX && g_classifier[y_dst + i][x_dst + j].transpose_idx == ALF_UNUSED_TRANSPOSE_IDX)
      {
        continue;
      }
      memset(e_local, 0, sizeof(e_local));
      if (g_classifier)
      {
        alf_classifier* cl = &g_classifier[y_dst + i][x_dst + j];
        transpose_idx = cl->transpose_idx;
        class_idx = cl->class_idx;
      }

      int16_t y_local = org[j] - rec[j];

      //__m256i const perm_mask = _mm256_set_epi32(14, 12, 10, 8, 6, 4, 2, 0);

      __m256i y_local_32 = _mm256_set1_epi32(y_local);
      alf_calc_covariance_avx2(e_local, rec + j, rec_stride, channel, transpose_idx, vb_distance, alf_clipping_values);
      for (int k = 0; k < num_coeff; k++)
      {
        for (int l = k; l < num_coeff; l++)
        {
          for (int b0 = 0; b0 < 4; b0++)
          {
            if (!e_local[k][b0]) continue;
            __m256i e_local_b0_d = _mm256_set1_epi32((int32_t)e_local[k][b0]);
            /*for (int b1 = 0; b1 < 4; b1++)
            {
              alf_covariance[class_idx].ee[k][l][b0][b1] += e_local[k][b0] * (double)e_local[l][b1];
            }*/

            __m128i e_local_1 = _mm_loadl_epi64((__m128i*) & e_local[l][0]);
            __m256i e_local_32 = _mm256_cvtepi16_epi64(e_local_1);
            __m256i multiplied = _mm256_mul_epi32(e_local_b0_d, e_local_32);
            __m256i orig = _mm256_loadu_si256((__m256i*)alf_covariance[class_idx].ee[k][l][b0]);
            _mm256_storeu_si256((__m256i*)alf_covariance[class_idx].ee[k][l][b0], _mm256_add_epi64(multiplied, orig));

          }
        }
        /*
        for (int b = 0; b < 4; b++)
        {
          alf_covariance[class_idx].y[k][b] += e_local[k][b] * (double)y_local;
        }*/

      }
      for (int k = 0; k < num_coeff-1; k+=2)
      {
        __m128i e_local_1 = _mm_loadu_si128((__m128i*) & e_local[k][0]);
        __m256i e_local_32 = _mm256_cvtepi16_epi32(e_local_1);
        __m256i multiplied = _mm256_mullo_epi32(y_local_32, e_local_32);
        __m256i orig = _mm256_loadu_si256((__m256i*) & alf_covariance[class_idx].y[k][0]);
        _mm256_storeu_si256((__m256i*)alf_covariance[class_idx].y[k], _mm256_add_epi32(multiplied, orig));
      }
      __m128i e_local_1 = _mm_loadl_epi64((__m128i*) & e_local[num_coeff-1][0]);
      __m256i e_local_32 = _mm256_cvtepi16_epi32(e_local_1);
      __m256i multiplied = _mm256_mullo_epi32(y_local_32, e_local_32);
      __m128i orig = _mm_loadu_si128((__m128i*) & alf_covariance[class_idx].y[num_coeff - 1][0]);
      _mm_storeu_si128((__m128i*)alf_covariance[class_idx].y[num_coeff - 1], _mm_add_epi32(_mm256_castsi256_si128(multiplied), orig));

      alf_covariance[class_idx].pix_acc += y_local * (double)y_local;
    }
    org += org_stride;
    rec += rec_stride;
  }

  int num_classes = g_classifier ? MAX_NUM_ALF_CLASSES : 1;
  for (class_idx = 0; class_idx < num_classes; class_idx++)
  {
    for (int k = 1; k < num_coeff; k++)
    {
      for (int l = 0; l < k; l++)
      {
        for (int b0 = 0; b0 < 4; b0++)
        {
          for (int b1 = 0; b1 < 4; b1++)
          {
            alf_covariance[class_idx].ee[k][l][b0][b1] = alf_covariance[class_idx].ee[l][k][b1][b0];
          }
        }
      }
    }
  }
}

#endif // UVG_BIT_DEPTH == 8
#endif //COMPILE_INTEL_AVX2


int uvg_strategy_register_alf_avx2(void* opaque, uint8_t bitdepth) {
  bool success = true;
#if COMPILE_INTEL_AVX2
#if UVG_BIT_DEPTH == 8
  if (bitdepth == 8){
    success &= uvg_strategyselector_register(opaque, "alf_get_blk_stats", "avx2", 40, &alf_get_blk_stats_avx2);
  }
#endif // UVG_BIT_DEPTH == 8
#endif
  return success;
}
