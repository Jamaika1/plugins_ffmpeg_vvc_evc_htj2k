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

#include "ipol-generic.h"

#include <stdio.h>
#include <string.h>

#include "encoder.h"
#include "picture-generic.h"
#include "strategies-ipol.h"
#include "strategyselector.h"

extern int8_t uvg_g_luma_filter[16][8];
extern int8_t uvg_g_chroma_filter[32][4];

int32_t uvg_eight_tap_filter_hor_generic(int8_t *filter, uvg_pixel *data)
{
  int32_t temp = 0;
  for (int i = 0; i < 8; ++i)
  {
    temp += filter[i] * data[i];
  }

  return temp;
}

int32_t uvg_eight_tap_filter_hor_16bit_generic(int8_t *filter, int16_t *data)
{
  int32_t temp = 0;
  for (int i = 0; i < 8; ++i)
  {
    temp += filter[i] * data[i];
  }

  return temp;
}

int32_t uvg_eight_tap_filter_ver_generic(int8_t *filter, uvg_pixel *data, int16_t stride)
{
  int32_t temp = 0;
  for (int i = 0; i < 8; ++i)
  {
    temp += filter[i] * data[stride * i];
  }

  return temp;
}

int32_t uvg_eight_tap_filter_ver_16bit_generic(int8_t *filter, int16_t *data, int16_t stride)
{
  int32_t temp = 0;
  for (int i = 0; i < 8; ++i)
  {
    temp += filter[i] * data[stride * i];
  }

  return temp;
}

int32_t uvg_four_tap_filter_hor_generic(int8_t *filter, uvg_pixel *data)
{
  int32_t temp = 0;
  for (int i = 0; i < 4; ++i)
  {
    temp += filter[i] * data[i];
  }

  return temp;
}

int32_t uvg_four_tap_filter_hor_16bit_generic(int8_t *filter, int16_t *data)
{
  int32_t temp = 0;
  for (int i = 0; i < 4; ++i)
  {
    temp += filter[i] * data[i];
  }

  return temp;
}

int32_t uvg_four_tap_filter_ver_generic(int8_t *filter, uvg_pixel *data, int16_t stride)
{
  int32_t temp = 0;
  for (int i = 0; i < 4; ++i)
  {
    temp += filter[i] * data[stride * i];
  }

  return temp;
}

int32_t uvg_four_tap_filter_ver_16bit_generic(int8_t *filter, int16_t *data, int16_t stride)
{
  int32_t temp = 0;
  for (int i = 0; i < 4; ++i)
  {
    temp += filter[i] * data[stride * i];
  }

  return temp;
}

void uvg_sample_quarterpel_luma_generic(const encoder_control_t * const encoder,
  uvg_pixel *src,
  int16_t src_stride,
  int width,
  int height,
  uvg_pixel *dst,
  int16_t dst_stride,
  int8_t hor_flag,
  int8_t ver_flag,
  const mv_t mv[2])
{
  //TODO: horizontal and vertical only filtering
  int32_t x, y;

  // Interpolation filter shifts
  int16_t shift1 = UVG_BIT_DEPTH - 8;
  int32_t shift2 = 6;

  // Weighted prediction offset and shift
  int32_t wp_shift1 = 14 - UVG_BIT_DEPTH;
  int32_t wp_offset1 = 1 << (wp_shift1 - 1);

  // Select filters according to the fractional part of the x and y mv components
  int8_t *hor_filter = uvg_g_luma_filter[mv[0] & 15];
  int8_t *ver_filter = uvg_g_luma_filter[mv[1] & 15];

  int16_t hor_filtered[UVG_EXT_BLOCK_W_LUMA][LCU_WIDTH];
  int16_t hor_stride = LCU_WIDTH;

  // Filter horizontally
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_LUMA_FILTER_OFFSET;
      int xpos = x - UVG_LUMA_FILTER_OFFSET;
      hor_filtered[y][x] = uvg_eight_tap_filter_hor_generic(hor_filter, &src[src_stride * ypos + xpos]) >> shift1;
    }
  }

  // Filter vertically
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      dst[y * dst_stride + x] = uvg_fast_clip_32bit_to_pixel(((uvg_eight_tap_filter_ver_16bit_generic(ver_filter, &hor_filtered[y][x], hor_stride) >> shift2) + wp_offset1) >> wp_shift1);
    }
  }
}

void uvg_sample_quarterpel_luma_hi_generic(const encoder_control_t * const encoder, uvg_pixel *src, int16_t src_stride, int width, int height, int16_t *dst, int16_t dst_stride, int8_t hor_flag, int8_t ver_flag, const mv_t mv[2])
{
  //TODO: horizontal and vertical only filtering
  int32_t x, y;

  // Interpolation filter shifts
  int16_t shift1 = UVG_BIT_DEPTH - 8;
  int32_t shift2 = 6;

  // Select filters according to the fractional part of the x and y mv components
  int8_t *hor_filter = uvg_g_luma_filter[mv[0] & 15];
  int8_t *ver_filter = uvg_g_luma_filter[mv[1] & 15];

  int16_t hor_filtered[UVG_EXT_BLOCK_W_LUMA][LCU_WIDTH];
  int16_t hor_stride = LCU_WIDTH;

  // Filter horizontally
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_LUMA_FILTER_OFFSET;
      int xpos = x - UVG_LUMA_FILTER_OFFSET;
      hor_filtered[y][x] = uvg_eight_tap_filter_hor_generic(hor_filter, &src[src_stride * ypos + xpos]) >> shift1;
    }
  }

  // Filter vertically
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      dst[y * dst_stride + x] = uvg_eight_tap_filter_ver_16bit_generic(ver_filter, &hor_filtered[y][x], hor_stride) >> shift2;
    }
  }
}

void uvg_filter_hpel_blocks_hor_ver_luma_generic(const encoder_control_t * encoder,
  uvg_pixel *src,
  int16_t src_stride,
  int width,
  int height,
  uvg_pixel filtered[4][LCU_LUMA_SIZE],
  int16_t hor_intermediate[5][UVG_IPOL_MAX_IM_SIZE_LUMA_SIMD],
  int8_t fme_level,
  int16_t hor_first_cols[5][UVG_EXT_BLOCK_W_LUMA + 1],
  int8_t hpel_off_x, int8_t hpel_off_y)
{
  int x, y, first_y;

  // Interpolation filter shifts
  int16_t shift1 = UVG_BIT_DEPTH - 8;

  // Weighted prediction offset and shift
  int32_t wp_shift1 = 14 - UVG_BIT_DEPTH;
  int32_t wp_offset1 = 1 << (wp_shift1 - 1);

  int8_t *fir0 = uvg_g_luma_filter[0];
  int8_t *fir2 = uvg_g_luma_filter[8];

  int16_t dst_stride = LCU_WIDTH;
  int16_t hor_stride = LCU_WIDTH;
  int32_t first_row_offset = (UVG_LUMA_FILTER_OFFSET + 1) * hor_stride;

  int16_t *col_pos0 = hor_first_cols[0];
  int16_t *col_pos2 = hor_first_cols[2];

  // Horizontally filtered samples from the top row are
  // not needed unless samples for diagonal positions are filtered later.
  first_y = fme_level > 1 ? 0 : 1;

  // HORIZONTAL STEP
  // Integer pixels
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_LUMA_FILTER_OFFSET;
      int xpos = x - UVG_LUMA_FILTER_OFFSET + 1;
      hor_intermediate[0][y * hor_stride + x] = uvg_eight_tap_filter_hor_generic(fir0, &src[src_stride*ypos + xpos]) >> shift1;
    }
  }

  // Write the first column in contiguous memory
  x = 0;
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    int ypos = y - UVG_LUMA_FILTER_OFFSET;
    int xpos = x - UVG_LUMA_FILTER_OFFSET;
    col_pos0[y] = uvg_eight_tap_filter_hor_generic(fir0, &src[src_stride*ypos + xpos]) >> shift1;
  }

  // Half pixels
  for (y = first_y; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_LUMA_FILTER_OFFSET;
      int xpos = x - UVG_LUMA_FILTER_OFFSET + 1;
      hor_intermediate[1][y * hor_stride + x] = uvg_eight_tap_filter_hor_generic(fir2, &src[src_stride*ypos + xpos]) >> shift1;
    }
  }

  // Write the first column in contiguous memory
  x = 0;
  for (y = first_y; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    int ypos = y - UVG_LUMA_FILTER_OFFSET;
    int xpos = x - UVG_LUMA_FILTER_OFFSET;
    col_pos2[y] = uvg_eight_tap_filter_hor_generic(fir2, &src[src_stride*ypos + xpos]) >> shift1;
  }

  // VERTICAL STEP

  // Right
  // Only horizontal filter
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      filtered[1][y * dst_stride + x] = uvg_fast_clip_16bit_to_pixel((hor_intermediate[1][first_row_offset + y * hor_stride + x] + wp_offset1) >> wp_shift1);
    }
  }

  // Left
  // Copy from the right filtered block and the extra column
  for (y = 0; y < height; ++y) {
    x = 0;
    filtered[0][y * dst_stride + x] = uvg_fast_clip_16bit_to_pixel((col_pos2[y + UVG_LUMA_FILTER_OFFSET + 1] + wp_offset1) >> wp_shift1);
    for (x = 1; x < width; ++x) filtered[0][y * dst_stride + x] = filtered[1][y * dst_stride + x - 1];
  }

  // Top
  // Only vertical filter
  for (y = 0; y < height; ++y) {
    int ypos = y - UVG_LUMA_FILTER_OFFSET;
    for (x = 0; x < width; ++x) {
      int xpos = x;
      int16_t sample = uvg_eight_tap_filter_ver_generic(fir2, &src[src_stride*ypos + xpos + 1], src_stride) >> shift1;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[2][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  // Bottom
  // Copy what can be copied from the top filtered values.
  // Then filter the last row from horizontal intermediate buffer.
  for (y = 0; y < height - 1; ++y) {
    for (x = 0; x < width; ++x) filtered[3][y * dst_stride + x] = filtered[2][(y + 1) * dst_stride + x];
  }

  int ypos = y - UVG_LUMA_FILTER_OFFSET;
  for (x = 0; x < width; ++x) {
    int xpos = x;
    int16_t sample = uvg_eight_tap_filter_ver_generic(fir2, &src[src_stride*(ypos + 1) + xpos + 1], src_stride) >> shift1;
    sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
    filtered[3][y * dst_stride + x] = (uvg_pixel)sample;
  }
}

void uvg_filter_hpel_blocks_diag_luma_generic(const encoder_control_t * encoder,
  uvg_pixel *src,
  int16_t src_stride,
  int width,
  int height,
  uvg_pixel filtered[4][LCU_LUMA_SIZE],
  int16_t hor_intermediate[5][UVG_IPOL_MAX_IM_SIZE_LUMA_SIMD],
  int8_t fme_level,
  int16_t hor_first_cols[5][UVG_EXT_BLOCK_W_LUMA + 1],
  int8_t hpel_off_x, int8_t hpel_off_y)
{
  int x, y;

  // Interpolation filter shifts
  int32_t shift2 = 6;

  // Weighted prediction offset and shift
  int32_t wp_shift1 = 14 - UVG_BIT_DEPTH;
  int32_t wp_offset1 = 1 << (wp_shift1 - 1);

  int8_t *fir2 = uvg_g_luma_filter[8];

  int16_t dst_stride = LCU_WIDTH;
  int16_t hor_stride = LCU_WIDTH;

  // Horizontal positions
  int16_t *col_pos2 = hor_first_cols[2];

  // VERTICAL STEP

  // Top-right
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(fir2, &hor_intermediate[1][y * hor_stride + x], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[1][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  for (y = 0; y < height; ++y) {
    x = 0;
    filtered[0][y * dst_stride + x] = uvg_fast_clip_16bit_to_pixel((col_pos2[y + UVG_LUMA_FILTER_OFFSET + 1] + wp_offset1) >> wp_shift1);
    for (x = 1; x < width; ++x) filtered[0][y * dst_stride + x] = filtered[1][y * dst_stride + x - 1];
  }

  // Top-left
  // Copy what can be copied from top-right filtered values. Filter the first column from the column array.
  for (y = 0; y < height; ++y) {
    x = 0;
    int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(fir2, &col_pos2[y]) >> shift2;
    sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
    filtered[0][y * dst_stride + x] = (uvg_pixel)sample;
    for (x = 1; x < width; ++x) filtered[0][y * dst_stride + x] = filtered[1][y * dst_stride + x - 1];
  }

  // Bottom-right
  // Copy what can be copied from top-right filtered values. Filter the last row.
  for (y = 0; y < height - 1; ++y) {
    for (x = 0; x < width; ++x) filtered[3][y* dst_stride + x] = filtered[1][(y + 1) * dst_stride + x];
  }

  for (x = 0; x < width; ++x) {
    int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(fir2, &hor_intermediate[1][(y + 1) * hor_stride + x], hor_stride) >> shift2;
    sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
    filtered[3][y * dst_stride + x] = (uvg_pixel)sample;
  }

  // Bottom-left
  // Copy what can be copied from the top-left filtered values.
  // Copy what can be copied from the bottom-right filtered values.
  // Finally filter the last pixel from the column array.
  for (y = 0; y < height - 1; ++y) {
    for (x = 0; x < width; ++x) filtered[2][y * dst_stride + x] = filtered[0][(y + 1) * dst_stride + x];
  }
  for (x = 1; x < width; ++x) filtered[2][y * dst_stride + x] = filtered[3][y * dst_stride + x - 1];
  x = 0;
  int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(fir2, &col_pos2[(y + 1)]) >> shift2;
  sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
  filtered[2][y * dst_stride + x] = (uvg_pixel)sample;
}

void uvg_filter_qpel_blocks_hor_ver_luma_generic(const encoder_control_t * encoder,
  uvg_pixel *src,
  int16_t src_stride,
  int width,
  int height,
  uvg_pixel filtered[4][LCU_LUMA_SIZE],
  int16_t hor_intermediate[5][UVG_IPOL_MAX_IM_SIZE_LUMA_SIMD],
  int8_t fme_level,
  int16_t hor_first_cols[5][UVG_EXT_BLOCK_W_LUMA + 1],
  int8_t hpel_off_x, int8_t hpel_off_y)
{
  int x, y;

  // Interpolation filter shifts
  int16_t shift1 = UVG_BIT_DEPTH - 8;
  int32_t shift2 = 6;

  // Weighted prediction offset and shift
  int32_t wp_shift1 = 14 - UVG_BIT_DEPTH;
  int32_t wp_offset1 = 1 << (wp_shift1 - 1);

  int8_t *fir0 = uvg_g_luma_filter[0];
  int8_t *fir2 = uvg_g_luma_filter[8];
  int8_t *fir1 = uvg_g_luma_filter[4];
  int8_t *fir3 = uvg_g_luma_filter[12];

  // Horiziontal positions. Positions 0 and 2 have already been calculated in filtered.
  int16_t *hor_pos0 = hor_intermediate[0];
  int16_t *hor_pos2 = hor_intermediate[1];
  int16_t *hor_pos_l = hor_intermediate[3];
  int16_t *hor_pos_r = hor_intermediate[4];
  int8_t *hor_fir_l  = hpel_off_x != 0 ? fir1 : fir3;
  int8_t *hor_fir_r  = hpel_off_x != 0 ? fir3 : fir1;
  int16_t *col_pos_l = hor_first_cols[1];
  int16_t *col_pos_r = hor_first_cols[3];

  int16_t dst_stride = LCU_WIDTH;
  int16_t hor_stride = LCU_WIDTH;

  int16_t *hor_hpel_pos = hpel_off_x != 0 ? hor_pos2 : hor_pos0;
  int16_t *col_pos_hor  = hpel_off_x != 0 ? hor_first_cols[2] : hor_first_cols[0];

  // Specify if integer pixels are filtered from left or/and top integer samples
  int off_x_fir_l = hpel_off_x < 1 ? 0 : 1;
  int off_x_fir_r = hpel_off_x < 0 ? 0 : 1;
  int off_y_fir_t = hpel_off_y < 1 ? 0 : 1;
  int off_y_fir_b = hpel_off_y < 0 ? 0 : 1;

  // HORIZONTAL STEP
  // Left QPEL
  int sample_off_y = hpel_off_y < 0 ? 0 : 1;
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_LUMA_FILTER_OFFSET;
      int xpos = x - UVG_LUMA_FILTER_OFFSET + 1;
      hor_pos_l[y * hor_stride + x] = uvg_eight_tap_filter_hor_generic(hor_fir_l, &src[src_stride*ypos + xpos]) >> shift1;
    }
  }

  // Write the first column in contiguous memory
  x = 0;
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    int ypos = y - UVG_LUMA_FILTER_OFFSET;
    int xpos = x - UVG_LUMA_FILTER_OFFSET;
    col_pos_l[y] = uvg_eight_tap_filter_hor_generic(hor_fir_l, &src[src_stride*ypos + xpos]) >> shift1;
  }

  // Right QPEL
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_LUMA_FILTER_OFFSET;
      int xpos = x - UVG_LUMA_FILTER_OFFSET + 1;
      hor_pos_r[y * hor_stride + x] = uvg_eight_tap_filter_hor_generic(hor_fir_r, &src[src_stride*ypos + xpos]) >> shift1;
    }
  }

  // Write the first column in contiguous memory
  x = 0;
  for (y = 0; y < height + UVG_EXT_PADDING_LUMA + 1; ++y) {
    int ypos = y - UVG_LUMA_FILTER_OFFSET;
    int xpos = x - UVG_LUMA_FILTER_OFFSET;
    col_pos_r[y] = uvg_eight_tap_filter_hor_generic(hor_fir_r, &src[src_stride*ypos + xpos]) >> shift1;
  }

  // VERTICAL STEP
  int8_t *ver_fir_l = hpel_off_y != 0 ? fir2 : fir0;
  int8_t *ver_fir_r = hpel_off_y != 0 ? fir2 : fir0;
  int8_t *ver_fir_t = hpel_off_y != 0 ? fir1 : fir3;
  int8_t *ver_fir_b = hpel_off_y != 0 ? fir3 : fir1;

  // Left QPEL (1/4 or 3/4 x positions)
  for (y = 0; y < height; ++y) {
    if (!off_x_fir_l) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_l, &col_pos_l[y + sample_off_y]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[0][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !off_x_fir_l; x < width; ++x) {
      int ypos = y + sample_off_y;
      int xpos = x - !off_x_fir_l;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_l, &hor_pos_l[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[0][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  // Right QPEL (3/4 or 1/4 x positions)
  for (y = 0; y < height; ++y) {
    if (!off_x_fir_r) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_r, &col_pos_r[y + sample_off_y]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[1][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !off_x_fir_r; x < width; ++x) {
      int ypos = y + sample_off_y;
      int xpos = x - !off_x_fir_r;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_r, &hor_pos_r[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[1][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  // Top QPEL (1/4 or 3/4 y positions)
  int sample_off_x = (hpel_off_x > -1 ? 1 : 0);
  for (y = 0; y < height; ++y) {
    if (!sample_off_x) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_t, &col_pos_hor[y + off_y_fir_t]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[2][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !sample_off_x; x < width; ++x) {
      int ypos = y + off_y_fir_t;
      int xpos = x - !sample_off_x;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_t, &hor_hpel_pos[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[2][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  // Bottom QPEL (3/4 or 1/4 y positions)
  for (y = 0; y < height; ++y) {
    if (!sample_off_x) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_b, &col_pos_hor[y + off_y_fir_b]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[3][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !sample_off_x; x < width; ++x) {
      int ypos = y + off_y_fir_b;
      int xpos = x - !sample_off_x;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_b, &hor_hpel_pos[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[3][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }
}

void uvg_filter_qpel_blocks_diag_luma_generic(const encoder_control_t * encoder,
  uvg_pixel *src,
  int16_t src_stride,
  int width,
  int height,
  uvg_pixel filtered[4][LCU_LUMA_SIZE],
  int16_t hor_intermediate[5][UVG_IPOL_MAX_IM_SIZE_LUMA_SIMD],
  int8_t fme_level,
  int16_t hor_first_cols[5][UVG_EXT_BLOCK_W_LUMA + 1],
  int8_t hpel_off_x, int8_t hpel_off_y)
{
  int x, y;

  // Interpolation filter shifts
  int32_t shift2 = 6;

  // Weighted prediction offset and shift
  int32_t wp_shift1 = 14 - UVG_BIT_DEPTH;
  int32_t wp_offset1 = 1 << (wp_shift1 - 1);

  int8_t *fir1 = uvg_g_luma_filter[4];
  int8_t *fir3 = uvg_g_luma_filter[12];

  // Horiziontal positions.
  int16_t *hor_pos_l = hor_intermediate[3];
  int16_t *hor_pos_r = hor_intermediate[4];

  int16_t *col_pos_l = hor_first_cols[1];
  int16_t *col_pos_r = hor_first_cols[3];

  int16_t dst_stride = LCU_WIDTH;
  int16_t hor_stride = LCU_WIDTH;

  // VERTICAL STEP
  int8_t *ver_fir_t = hpel_off_y != 0 ? fir1 : fir3;
  int8_t *ver_fir_b = hpel_off_y != 0 ? fir3 : fir1;

  // Specify if integer pixels are filtered from left or/and top integer samples
  int off_x_fir_l = hpel_off_x < 1 ? 0 : 1;
  int off_x_fir_r = hpel_off_x < 0 ? 0 : 1;
  int off_y_fir_t = hpel_off_y < 1 ? 0 : 1;
  int off_y_fir_b = hpel_off_y < 0 ? 0 : 1;

  // Top-left QPEL
  for (y = 0; y < height; ++y) {
    if (!off_x_fir_l) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_t, &col_pos_l[y + off_y_fir_t]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[0][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !off_x_fir_l; x < width; ++x) {
      int ypos = y + off_y_fir_t;
      int xpos = x - !off_x_fir_l;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_t, &hor_pos_l[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[0][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  // Top-right QPEL
  for (y = 0; y < height; ++y) {
    if (!off_x_fir_r) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_t, &col_pos_r[y + off_y_fir_t]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[1][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !off_x_fir_r; x < width; ++x) {
      int ypos = y + off_y_fir_t;
      int xpos = x - !off_x_fir_r;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_t, &hor_pos_r[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[1][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  // Bottom-left QPEL
  for (y = 0; y < height; ++y) {
    if (!off_x_fir_l) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_b, &col_pos_l[y + off_y_fir_b]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[2][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !off_x_fir_l; x < width; ++x) {
      int ypos = y + off_y_fir_b;
      int xpos = x - !off_x_fir_l;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_b, &hor_pos_l[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[2][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }

  // Bottom-right QPEL
  for (y = 0; y < height; ++y) {
    if (!off_x_fir_r) {
      x = 0;
      int16_t sample = uvg_eight_tap_filter_hor_16bit_generic(ver_fir_b, &col_pos_r[y + off_y_fir_b]) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[3][y * dst_stride + x] = (uvg_pixel)sample;
    }
    for (x = !off_x_fir_r; x < width; ++x) {
      int ypos = y + off_y_fir_b;
      int xpos = x - !off_x_fir_r;
      int16_t sample = uvg_eight_tap_filter_ver_16bit_generic(ver_fir_b, &hor_pos_r[ypos * hor_stride + xpos], hor_stride) >> shift2;
      sample = uvg_fast_clip_16bit_to_pixel((sample + wp_offset1) >> wp_shift1);
      filtered[3][y * dst_stride + x] = (uvg_pixel)sample;
    }
  }
}

void uvg_sample_octpel_chroma_generic(const encoder_control_t * const encoder,
  uvg_pixel *src,
  int16_t src_stride,
  int width,
  int height,
  uvg_pixel *dst,
  int16_t dst_stride,
  int8_t hor_flag,
  int8_t ver_flag,
  const mv_t mv[2])
{
  //TODO: horizontal and vertical only filtering
  int32_t x, y;

  // Interpolation filter shifts
  int16_t shift1 = UVG_BIT_DEPTH - 8;
  int32_t shift2 = 6;

  // Weighted prediction offset and shift
  int32_t wp_shift1 = 14 - UVG_BIT_DEPTH;
  int32_t wp_offset1 = 1 << (wp_shift1 - 1);

  // Select filters according to the fractional part of the x and y mv components
  int8_t *hor_filter = uvg_g_chroma_filter[mv[0] & 31];
  int8_t *ver_filter = uvg_g_chroma_filter[mv[1] & 31];

  int16_t hor_filtered[UVG_EXT_BLOCK_W_CHROMA][LCU_WIDTH_C];
  int16_t hor_stride = LCU_WIDTH_C;

  // Filter horizontally
  for (y = 0; y < height + UVG_EXT_PADDING_CHROMA; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_CHROMA_FILTER_OFFSET;
      int xpos = x - UVG_CHROMA_FILTER_OFFSET;
      hor_filtered[y][x] = uvg_four_tap_filter_hor_generic(hor_filter, &src[src_stride * ypos + xpos]) >> shift1;
    }
  }

  // Filter vertically
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      dst[y * dst_stride + x] = uvg_fast_clip_32bit_to_pixel(((uvg_four_tap_filter_ver_16bit_generic(ver_filter, &hor_filtered[y][x], hor_stride) >> shift2) + wp_offset1) >> wp_shift1);
    }
  }
}

void uvg_sample_octpel_chroma_hi_generic(const encoder_control_t * const encoder, uvg_pixel *src, int16_t src_stride, int width, int height, int16_t *dst, int16_t dst_stride, int8_t hor_flag, int8_t ver_flag, const mv_t mv[2])
{
  //TODO: horizontal and vertical only filtering
  int32_t x, y;

  // Interpolation filter shifts
  int16_t shift1 = UVG_BIT_DEPTH - 8;
  int32_t shift2 = 6;

  // Select filters according to the fractional part of the x and y mv components
  int8_t *hor_filter = uvg_g_chroma_filter[mv[0] & 31];
  int8_t *ver_filter = uvg_g_chroma_filter[mv[1] & 31];

  int16_t hor_filtered[UVG_EXT_BLOCK_W_CHROMA][LCU_WIDTH_C];
  int16_t hor_stride = LCU_WIDTH_C;

  // Filter horizontally
  for (y = 0; y < height + UVG_EXT_PADDING_CHROMA; ++y) {
    for (x = 0; x < width; ++x) {
      int ypos = y - UVG_CHROMA_FILTER_OFFSET;
      int xpos = x - UVG_CHROMA_FILTER_OFFSET;
      hor_filtered[y][x] = uvg_four_tap_filter_hor_generic(hor_filter, &src[src_stride * ypos + xpos]) >> shift1;
    }
  }

  // Filter vertically
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      dst[y * dst_stride + x] = uvg_four_tap_filter_ver_16bit_generic(ver_filter, &hor_filtered[y][x], hor_stride) >> shift2;
    }
  }
}


void uvg_get_extended_block_generic(uvg_epol_args *args) {

  int min_y = args->blk_y - args->pad_t;
  int max_y = args->blk_y + args->blk_h + args->pad_b + args->pad_b_simd - 1;
  bool out_of_bounds_y = (min_y < 0) || (max_y >= args->src_h);

  int min_x = args->blk_x - args->pad_l;
  int max_x = args->blk_x + args->blk_w + args->pad_r - 1;
  bool out_of_bounds_x = (min_x < 0) || (max_x >= args->src_w);

  if (out_of_bounds_y || out_of_bounds_x) {

    *args->ext = args->buf;
    *args->ext_s = args->pad_l + args->blk_w + args->pad_r;
    *args->ext_origin = args->buf + args->pad_t * (*args->ext_s) + args->pad_l;

    // Note that stride equals width here.
    int cnt_l = CLIP(0, *args->ext_s, -min_x);
    int cnt_r = CLIP(0, *args->ext_s, max_x - (args->src_w - 1));
    int cnt_m = CLIP(0, *args->ext_s, *args->ext_s - cnt_l - cnt_r);

    // For each row including real padding.
    // Don't read "don't care" values (SIMD padding). Zero them out.
    int y;
    for (y = -args->pad_t; y < args->blk_h + args->pad_b; ++y) {

      int clipped_y = CLIP(0, args->src_h - 1, args->blk_y + y);
      uvg_pixel *sample_l = args->src + clipped_y * args->src_s;
      uvg_pixel *sample_r = args->src + clipped_y * args->src_s + args->src_w - 1;
      uvg_pixel *src_m = args->src + clipped_y * args->src_s + MAX(min_x, 0);
      uvg_pixel *dst_l = args->buf + (y + args->pad_t) * (*args->ext_s);
      uvg_pixel *dst_m = dst_l + cnt_l;
      uvg_pixel *dst_r = dst_m + cnt_m;
      for (int i = 0; i < cnt_l; ++i) *(dst_l + i) = *sample_l;
      for (int i = 0; i < cnt_m; ++i) *(dst_m + i) = *(src_m + i);
      for (int i = 0; i < cnt_r; ++i) *(dst_r + i) = *sample_r;
    }

    for (int y_simd = 0; y_simd < args->pad_b_simd; ++y_simd) {
      uvg_pixel *dst = args->buf + (y + args->pad_t + y_simd) * (*args->ext_s);
      FILL_ARRAY(dst, 0, *args->ext_s);
    }

  } else {

    *args->ext = args->src + (args->blk_y - args->pad_t) * args->src_s + (args->blk_x - args->pad_l);
    *args->ext_origin = args->src + args->blk_y * args->src_s + args->blk_x;
    *args->ext_s = args->src_s;
  }
}


void uvg_get_extended_block_wraparound_generic(uvg_epol_args *args)
{

  int  min_y = args->blk_y - args->pad_t;
  int  max_y = args->blk_y + args->blk_h + args->pad_b + args->pad_b_simd - 1;
  bool out_of_bounds_y = (min_y < 0) || (max_y >= args->src_h);

  int  min_x           = args->blk_x - args->pad_l;
  int  max_x           = args->blk_x + args->blk_w + args->pad_r;
  bool out_of_bounds_x = (min_x < 0) || (max_x >= args->src_w);

  if (out_of_bounds_y || out_of_bounds_x) {

    int first_x_start = 0;
    int first_x_count = 0;

    int second_x_start = 0;
    int second_x_count = 0;


    if (out_of_bounds_x) {
      if (min_x < 0) { // The block goes over the left edge of the frame.
        first_x_start = args->src_w + min_x;
        first_x_count = -min_x;
        if (max_x >= 0) { // Right side of the block doesn't wrap around the frame so we need to copy it separately.
          second_x_count = max_x;
        }
      } else { // The block wraps around the right edge of the frame.
        if (min_x >= args->src_w) { // The whole block wraps around the frame so copy in one go.
          first_x_start = min_x - args->src_w;
          first_x_count = max_x-min_x;
        } else { // The left side of the block doesn't wrap around the frame so we need to copy it separately.
          first_x_start  = min_x;
          first_x_count = args->src_w - min_x;
          second_x_count = max_x - args->src_w;
        }
      }
    } else { // The block is not over the horizontal edges of the frame, just copy directly
      first_x_count = max_x-min_x;
      first_x_start  = min_x;
    }

    *args->ext        = args->buf;
    *args->ext_s      = args->pad_l + args->blk_w + args->pad_r;
    *args->ext_origin = args->buf + args->pad_t * (*args->ext_s) + args->pad_l;

    // For each row including real padding.
    // Don't read "don't care" values (SIMD padding). Zero them out.
    int y;
    for (y = -args->pad_t; y < args->blk_h + args->pad_b; ++y) {
      int        clipped_y = CLIP(0, args->src_h - 1, args->blk_y + y);
      uvg_pixel *samples = args->src + clipped_y * args->src_s;
      uvg_pixel *dst = args->buf + (y + args->pad_t) * (*args->ext_s);

      if (first_x_count) memcpy(dst, samples + first_x_start, first_x_count*sizeof(uvg_pixel));
      if (second_x_count) memcpy(dst + first_x_count, samples + second_x_start, second_x_count*sizeof(uvg_pixel));
    }

    for (int y_simd = 0; y_simd < args->pad_b_simd; ++y_simd) {
      uvg_pixel *dst = args->buf + (y + args->pad_t + y_simd) * (*args->ext_s);
      FILL_ARRAY(dst, 0, *args->ext_s);
    }

  } else {

    *args->ext = args->src + (args->blk_y - args->pad_t) * args->src_s +
                 (args->blk_x - args->pad_l);
    *args->ext_origin = args->src + args->blk_y * args->src_s + args->blk_x;
    *args->ext_s      = args->src_s;
  }
}

int uvg_strategy_register_ipol_generic(void* opaque, uint8_t bitdepth)
{
  bool success = true;

  success &= uvg_strategyselector_register(opaque, "filter_hpel_blocks_hor_ver_luma", "generic", 0, &uvg_filter_hpel_blocks_hor_ver_luma_generic);
  success &= uvg_strategyselector_register(opaque, "filter_hpel_blocks_diag_luma", "generic", 0, &uvg_filter_hpel_blocks_diag_luma_generic);
  success &= uvg_strategyselector_register(opaque, "filter_qpel_blocks_hor_ver_luma", "generic", 0, &uvg_filter_qpel_blocks_hor_ver_luma_generic);
  success &= uvg_strategyselector_register(opaque, "filter_qpel_blocks_diag_luma", "generic", 0, &uvg_filter_qpel_blocks_diag_luma_generic);
  success &= uvg_strategyselector_register(opaque, "sample_quarterpel_luma", "generic", 0, &uvg_sample_quarterpel_luma_generic);
  success &= uvg_strategyselector_register(opaque, "sample_octpel_chroma", "generic", 0, &uvg_sample_octpel_chroma_generic);
  success &= uvg_strategyselector_register(opaque, "sample_quarterpel_luma_hi", "generic", 0, &uvg_sample_quarterpel_luma_hi_generic);
  success &= uvg_strategyselector_register(opaque, "sample_octpel_chroma_hi", "generic", 0, &uvg_sample_octpel_chroma_hi_generic);
  success &= uvg_strategyselector_register(opaque, "get_extended_block", "generic", 0, &uvg_get_extended_block_generic);
  success &= uvg_strategyselector_register(opaque, "get_extended_block_wraparound", "generic", 0, &uvg_get_extended_block_wraparound_generic);


  return success;
}
