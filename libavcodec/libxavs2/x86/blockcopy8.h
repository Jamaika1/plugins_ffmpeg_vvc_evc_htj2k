/*****************************************************************************
 * Copyright (C) 2013-2017 MulticoreWare, Inc
 *
 * Authors: Steve Borho <steve@borho.org>
;*          Min Chen <chenm003@163.com>
 *          Jiaqi Zhang <zhangjiaqi.cs@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at license @ x265.com.
 *****************************************************************************/

#ifndef XAVS2_BLOCKCOPY_H
#define XAVS2_BLOCKCOPY_H

#define xavs2_cpy2Dto1D_shr_4_sse2 FPFX(cpy2Dto1D_shr_4_sse2)
void xavs2_cpy2Dto1D_shr_4_sse2 (int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shr_8_sse2 FPFX(cpy2Dto1D_shr_8_sse2)
void xavs2_cpy2Dto1D_shr_8_sse2 (int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shr_16_sse2 FPFX(cpy2Dto1D_shr_16_sse2)
void xavs2_cpy2Dto1D_shr_16_sse2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shr_32_sse2 FPFX(cpy2Dto1D_shr_32_sse2)
void xavs2_cpy2Dto1D_shr_32_sse2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#if defined(__AVX2__)
#define xavs2_cpy1Dto2D_shl_4_avx2 FPFX(cpy1Dto2D_shl_4_avx2)
void xavs2_cpy1Dto2D_shl_4_avx2 (int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy1Dto2D_shl_8_avx2 FPFX(cpy1Dto2D_shl_8_avx2)
void xavs2_cpy1Dto2D_shl_8_avx2 (int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy1Dto2D_shl_16_avx2 FPFX(cpy1Dto2D_shl_16_avx2)
void xavs2_cpy1Dto2D_shl_16_avx2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy1Dto2D_shl_32_avx2 FPFX(cpy1Dto2D_shl_32_avx2)
void xavs2_cpy1Dto2D_shl_32_avx2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#endif
#define xavs2_cpy1Dto2D_shl_4_sse2 FPFX(cpy1Dto2D_shl_4_sse2)
void xavs2_cpy1Dto2D_shl_4_sse2 (int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shl_8_sse2 FPFX(cpy1Dto2D_shl_8_sse2)
void xavs2_cpy1Dto2D_shl_8_sse2 (int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shl_16_sse2 FPFX(cpy1Dto2D_shl_16_sse2)
void xavs2_cpy1Dto2D_shl_16_sse2(int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shl_32_sse2 FPFX(cpy1Dto2D_shl_32_sse2)
void xavs2_cpy1Dto2D_shl_32_sse2(int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#if defined(__AVX2__)
#define xavs2_cpy1Dto2D_shr_4_avx2 FPFX(cpy1Dto2D_shr_4_avx2)
void xavs2_cpy1Dto2D_shr_4_avx2 (int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy1Dto2D_shr_8_avx2 FPFX(cpy1Dto2D_shr_8_avx2)
void xavs2_cpy1Dto2D_shr_8_avx2 (int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shr_16_avx2 FPFX(cpy1Dto2D_shr_16_avx2)
void xavs2_cpy1Dto2D_shr_16_avx2(int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shr_32_avx2 FPFX(cpy1Dto2D_shr_32_avx2)
void xavs2_cpy1Dto2D_shr_32_avx2(int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#endif
#define xavs2_cpy1Dto2D_shr_4_sse2 FPFX(cpy1Dto2D_shr_4_sse2)
void xavs2_cpy1Dto2D_shr_4_sse2 (int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shr_8_sse2 FPFX(cpy1Dto2D_shr_8_sse2)
void xavs2_cpy1Dto2D_shr_8_sse2 (int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shr_16_sse2 FPFX(cpy1Dto2D_shr_16_sse2)
void xavs2_cpy1Dto2D_shr_16_sse2(int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#define xavs2_cpy1Dto2D_shr_32_sse2 FPFX(cpy1Dto2D_shr_32_sse2)
void xavs2_cpy1Dto2D_shr_32_sse2(int16_t* dst, const int16_t* src, intptr_t dstStride, int shift);
#if defined(__AVX2__)
#define xavs2_cpy2Dto1D_shl_8_avx2 FPFX(cpy2Dto1D_shl_8_avx2)
void xavs2_cpy2Dto1D_shl_8_avx2 (int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shl_16_avx2 FPFX(cpy2Dto1D_shl_16_avx2)
void xavs2_cpy2Dto1D_shl_16_avx2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shl_32_avx2 FPFX(cpy2Dto1D_shl_32_avx2)
void xavs2_cpy2Dto1D_shl_32_avx2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shr_8_avx2 FPFX(cpy2Dto1D_shr_8_avx2)
void xavs2_cpy2Dto1D_shr_8_avx2 (int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shr_16_avx2 FPFX(cpy2Dto1D_shr_16_avx2)
void xavs2_cpy2Dto1D_shr_16_avx2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#define xavs2_cpy2Dto1D_shr_32_avx2 FPFX(cpy2Dto1D_shr_32_avx2)
void xavs2_cpy2Dto1D_shr_32_avx2(int16_t* dst, const int16_t* src, intptr_t srcStride, int shift);
#endif

#define xavs2_copy_cnt_4_sse4 FPFX(copy_cnt_4_sse4)
uint32_t xavs2_copy_cnt_4_sse4  (int16_t* dst, const int16_t* src, intptr_t srcStride);
#define xavs2_copy_cnt_8_sse4 FPFX(copy_cnt_8_sse4)
uint32_t xavs2_copy_cnt_8_sse4  (int16_t* dst, const int16_t* src, intptr_t srcStride);
#define xavs2_copy_cnt_16_sse4 FPFX(copy_cnt_16_sse4)
uint32_t xavs2_copy_cnt_16_sse4 (int16_t* dst, const int16_t* src, intptr_t srcStride);
#define xavs2_copy_cnt_32_sse4 FPFX(copy_cnt_32_sse4)
uint32_t xavs2_copy_cnt_32_sse4 (int16_t* dst, const int16_t* src, intptr_t srcStride);
#if defined(__AVX2__)
#define xavs2_copy_cnt_4_avx2 FPFX(copy_cnt_4_avx2)
uint32_t xavs2_copy_cnt_4_avx2  (int16_t* dst, const int16_t* src, intptr_t srcStride);
#define xavs2_copy_cnt_8_avx2 FPFX(copy_cnt_8_avx2)
uint32_t xavs2_copy_cnt_8_avx2  (int16_t* dst, const int16_t* src, intptr_t srcStride);
#define xavs2_copy_cnt_16_avx2 FPFX(copy_cnt_16_avx2)
uint32_t xavs2_copy_cnt_16_avx2 (int16_t* dst, const int16_t* src, intptr_t srcStride);
#define xavs2_copy_cnt_32_avx2 FPFX(copy_cnt_32_avx2)
uint32_t xavs2_copy_cnt_32_avx2 (int16_t* dst, const int16_t* src, intptr_t srcStride);
#endif

#define SETUP_BLOCKCOPY_FUNC(W, H, cpu) \
    void xavs2_blockcopy_pp_ ## W ## x ## H ## cpu(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb); \
    void xavs2_blockcopy_sp_ ## W ## x ## H ## cpu(uint8_t *a, intptr_t stridea, const int16_t* b, intptr_t strideb); \
    void xavs2_blockcopy_ss_ ## W ## x ## H ## cpu(int16_t* a, intptr_t stridea, const int16_t* b, intptr_t strideb);

#define SETUP_BLOCKCOPY_PS(W, H, cpu) \
    void xavs2_blockcopy_ps_ ## W ## x ## H ## cpu(int16_t* dst, intptr_t dstStride, const uint8_t *src, intptr_t srcStride);

#define SETUP_BLOCKCOPY_SP(W, H, cpu) \
    void xavs2_blockcopy_sp_ ## W ## x ## H ## cpu(uint8_t *a, intptr_t stridea, const int16_t* b, intptr_t strideb);

#define SETUP_BLOCKCOPY_SS_PP(W, H, cpu) \
    void xavs2_blockcopy_pp_ ## W ## x ## H ## cpu(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb); \
    void xavs2_blockcopy_ss_ ## W ## x ## H ## cpu(int16_t* a, intptr_t stridea, const int16_t* b, intptr_t strideb);

#define BLOCKCOPY_COMMON(cpu) \
    SETUP_BLOCKCOPY_FUNC(64, 64, cpu);  /* 64x64 */ \
    SETUP_BLOCKCOPY_FUNC(64, 32, cpu); \
    SETUP_BLOCKCOPY_FUNC(32, 64, cpu); \
    SETUP_BLOCKCOPY_FUNC(64, 16, cpu); \
    SETUP_BLOCKCOPY_FUNC(64, 48, cpu); \
    SETUP_BLOCKCOPY_FUNC(16, 64, cpu); \
    SETUP_BLOCKCOPY_FUNC(48, 64, cpu); \
    SETUP_BLOCKCOPY_FUNC(32, 32, cpu);  /* 32x32 */ \
    SETUP_BLOCKCOPY_FUNC(32, 16, cpu); \
    SETUP_BLOCKCOPY_FUNC(16, 32, cpu); \
    SETUP_BLOCKCOPY_FUNC(32,  8, cpu); \
    SETUP_BLOCKCOPY_FUNC(32, 24, cpu); \
    SETUP_BLOCKCOPY_FUNC( 8, 32, cpu); \
    SETUP_BLOCKCOPY_FUNC(24, 32, cpu); \
    SETUP_BLOCKCOPY_FUNC(16, 16, cpu);  /* 16x16 */ \
    SETUP_BLOCKCOPY_FUNC(16,  8, cpu); \
    SETUP_BLOCKCOPY_FUNC( 8, 16, cpu); \
    SETUP_BLOCKCOPY_FUNC(16,  4, cpu); \
    SETUP_BLOCKCOPY_FUNC(16, 12, cpu); \
    SETUP_BLOCKCOPY_FUNC( 4, 16, cpu); \
    SETUP_BLOCKCOPY_FUNC(12, 16, cpu); \
    SETUP_BLOCKCOPY_FUNC( 8,  8, cpu);  /* 8x8 */ \
    SETUP_BLOCKCOPY_FUNC( 8,  4, cpu); \
    SETUP_BLOCKCOPY_FUNC( 4,  8, cpu); \
    SETUP_BLOCKCOPY_FUNC( 4,  4, cpu);  /* 4x4 */

#define BLOCKCOPY_SP(cpu) \
    SETUP_BLOCKCOPY_SP( 2,  4, cpu); \
    SETUP_BLOCKCOPY_SP( 2,  8, cpu); \
    SETUP_BLOCKCOPY_SP( 6,  8, cpu); \
    \
    SETUP_BLOCKCOPY_SP( 2, 16, cpu); \
    SETUP_BLOCKCOPY_SP( 4, 32, cpu); \
    SETUP_BLOCKCOPY_SP( 6, 16, cpu); \
    SETUP_BLOCKCOPY_SP( 8, 12, cpu); \
    SETUP_BLOCKCOPY_SP( 8, 64, cpu); \
    SETUP_BLOCKCOPY_SP(12, 32, cpu); \
    SETUP_BLOCKCOPY_SP(16, 24, cpu); \
    SETUP_BLOCKCOPY_SP(24, 64, cpu); \
    SETUP_BLOCKCOPY_SP(32, 48, cpu);

#define BLOCKCOPY_SS_PP(cpu) \
    SETUP_BLOCKCOPY_SS_PP( 2,  4, cpu); \
    SETUP_BLOCKCOPY_SS_PP( 2,  8, cpu); \
    SETUP_BLOCKCOPY_SS_PP( 6,  8, cpu); \
    \
    SETUP_BLOCKCOPY_SS_PP( 2, 16, cpu); \
    SETUP_BLOCKCOPY_SS_PP( 4, 32, cpu); \
    SETUP_BLOCKCOPY_SS_PP( 6, 16, cpu); \
    SETUP_BLOCKCOPY_SS_PP( 8, 12, cpu); \
    SETUP_BLOCKCOPY_SS_PP( 8, 64, cpu); \
    SETUP_BLOCKCOPY_SS_PP(12, 32, cpu); \
    SETUP_BLOCKCOPY_SS_PP(16, 24, cpu); \
    SETUP_BLOCKCOPY_SS_PP(24, 64, cpu); \
    SETUP_BLOCKCOPY_SS_PP(32, 48, cpu);


#define BLOCKCOPY_PS(cpu) \
    SETUP_BLOCKCOPY_PS(64, 64, cpu);    /* 64x64 */ \
    SETUP_BLOCKCOPY_PS(64, 32, cpu); \
    SETUP_BLOCKCOPY_PS(32, 64, cpu); \
    SETUP_BLOCKCOPY_PS(64, 16, cpu); \
    SETUP_BLOCKCOPY_PS(64, 48, cpu); \
    SETUP_BLOCKCOPY_PS(16, 64, cpu); \
    SETUP_BLOCKCOPY_PS(48, 64, cpu); \
    SETUP_BLOCKCOPY_PS(32, 32, cpu);    /* 32x32 */ \
    SETUP_BLOCKCOPY_PS(32, 16, cpu); \
    SETUP_BLOCKCOPY_PS(16, 32, cpu); \
    SETUP_BLOCKCOPY_PS(32,  8, cpu); \
    SETUP_BLOCKCOPY_PS(32, 24, cpu); \
    SETUP_BLOCKCOPY_PS( 8, 32, cpu); \
    SETUP_BLOCKCOPY_PS(24, 32, cpu); \
    SETUP_BLOCKCOPY_PS(16, 16, cpu);    /* 16x16 */ \
    SETUP_BLOCKCOPY_PS(16,  8, cpu); \
    SETUP_BLOCKCOPY_PS( 8, 16, cpu); \
    SETUP_BLOCKCOPY_PS(16,  4, cpu); \
    SETUP_BLOCKCOPY_PS(16, 12, cpu); \
    SETUP_BLOCKCOPY_PS( 4, 16, cpu); \
    SETUP_BLOCKCOPY_PS(12, 16, cpu); \
    SETUP_BLOCKCOPY_PS( 8,  8, cpu);    /* 8x8 */ \
    SETUP_BLOCKCOPY_PS( 8,  4, cpu); \
    SETUP_BLOCKCOPY_PS( 4,  8, cpu); \
    SETUP_BLOCKCOPY_PS( 8,  2, cpu); \
    SETUP_BLOCKCOPY_PS( 8,  6, cpu); \
    SETUP_BLOCKCOPY_PS( 2,  8, cpu); \
    SETUP_BLOCKCOPY_PS( 6,  8, cpu); \
    SETUP_BLOCKCOPY_PS( 4,  4, cpu);    /* 4x4 */ \
    SETUP_BLOCKCOPY_PS( 4,  2, cpu); \
    SETUP_BLOCKCOPY_PS( 2,  4, cpu); \
    \
    SETUP_BLOCKCOPY_PS(32, 48, cpu); \
    SETUP_BLOCKCOPY_PS(24, 64, cpu); \
    SETUP_BLOCKCOPY_PS(16, 24, cpu); \
    SETUP_BLOCKCOPY_PS(12, 32, cpu); \
    SETUP_BLOCKCOPY_PS( 8, 64, cpu); \
    SETUP_BLOCKCOPY_PS( 8, 12, cpu); \
    SETUP_BLOCKCOPY_PS( 6, 16, cpu); \
    SETUP_BLOCKCOPY_PS( 4, 32, cpu); \
    SETUP_BLOCKCOPY_PS( 2, 16, cpu);

BLOCKCOPY_COMMON(_sse2)
BLOCKCOPY_SS_PP(_sse2)
BLOCKCOPY_SP(_sse4)
BLOCKCOPY_PS(_sse4)

BLOCKCOPY_SP(_sse2)

#define xavs2_blockfill_s_4x4_sse2 FPFX(blockfill_s_4x4_sse2)
void xavs2_blockfill_s_4x4_sse2  (int16_t* dst, intptr_t dstride, int16_t val);
#define xavs2_blockfill_s_8x8_sse2 FPFX(blockfill_s_8x8_sse2)
void xavs2_blockfill_s_8x8_sse2  (int16_t* dst, intptr_t dstride, int16_t val);
#define xavs2_blockfill_s_16x16_sse2 FPFX(blockfill_s_16x16_sse2)
void xavs2_blockfill_s_16x16_sse2(int16_t* dst, intptr_t dstride, int16_t val);
#define xavs2_blockfill_s_32x32_sse2 FPFX(blockfill_s_32x32_sse2)
void xavs2_blockfill_s_32x32_sse2(int16_t* dst, intptr_t dstride, int16_t val);
#define xavs2_blockcopy_ss_16x4_avx FPFX(blockcopy_ss_16x4_avx)
void xavs2_blockcopy_ss_16x4_avx (int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_16x8_avx FPFX(blockcopy_ss_16x8_avx)
void xavs2_blockcopy_ss_16x8_avx (int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_16x12_avx FPFX(blockcopy_ss_16x12_avx)
void xavs2_blockcopy_ss_16x12_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_16x16_avx FPFX(blockcopy_ss_16x16_avx)
void xavs2_blockcopy_ss_16x16_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_16x24_avx FPFX(blockcopy_ss_16x24_avx)
void xavs2_blockcopy_ss_16x24_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_16x32_avx FPFX(blockcopy_ss_16x32_avx)
void xavs2_blockcopy_ss_16x32_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_16x64_avx FPFX(blockcopy_ss_16x64_avx)
void xavs2_blockcopy_ss_16x64_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_64x16_avx FPFX(blockcopy_ss_64x16_avx)
void xavs2_blockcopy_ss_64x16_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_64x32_avx FPFX(blockcopy_ss_64x32_avx)
void xavs2_blockcopy_ss_64x32_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_64x48_avx FPFX(blockcopy_ss_64x48_avx)
void xavs2_blockcopy_ss_64x48_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_64x64_avx FPFX(blockcopy_ss_64x64_avx)
void xavs2_blockcopy_ss_64x64_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_32x8_avx FPFX(blockcopy_ss_32x8_avx)
void xavs2_blockcopy_ss_32x8_avx (int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_32x16_avx FPFX(blockcopy_ss_32x16_avx)
void xavs2_blockcopy_ss_32x16_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_32x24_avx FPFX(blockcopy_ss_32x24_avx)
void xavs2_blockcopy_ss_32x24_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_32x32_avx FPFX(blockcopy_ss_32x32_avx)
void xavs2_blockcopy_ss_32x32_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_32x48_avx FPFX(blockcopy_ss_32x48_avx)
void xavs2_blockcopy_ss_32x48_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_32x64_avx FPFX(blockcopy_ss_32x64_avx)
void xavs2_blockcopy_ss_32x64_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_48x64_avx FPFX(blockcopy_ss_48x64_avx)
void xavs2_blockcopy_ss_48x64_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_24x32_avx FPFX(blockcopy_ss_24x32_avx)
void xavs2_blockcopy_ss_24x32_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);
#define xavs2_blockcopy_ss_24x64_avx FPFX(blockcopy_ss_24x64_avx)
void xavs2_blockcopy_ss_24x64_avx(int16_t* dst, intptr_t dstStride, const int16_t* src, intptr_t srcStride);

#define xavs2_blockcopy_pp_32x8_avx FPFX(blockcopy_pp_32x8_avx)
void xavs2_blockcopy_pp_32x8_avx (uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_32x16_avx FPFX(blockcopy_pp_32x16_avx)
void xavs2_blockcopy_pp_32x16_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_32x24_avx FPFX(blockcopy_pp_32x24_avx)
void xavs2_blockcopy_pp_32x24_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_32x32_avx FPFX(blockcopy_pp_32x32_avx)
void xavs2_blockcopy_pp_32x32_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_32x48_avx FPFX(blockcopy_pp_32x48_avx)
void xavs2_blockcopy_pp_32x48_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_32x64_avx FPFX(blockcopy_pp_32x64_avx)
void xavs2_blockcopy_pp_32x64_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_64x16_avx FPFX(blockcopy_pp_64x16_avx)
void xavs2_blockcopy_pp_64x16_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_64x32_avx FPFX(blockcopy_pp_64x32_avx)
void xavs2_blockcopy_pp_64x32_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_64x48_avx FPFX(blockcopy_pp_64x48_avx)
void xavs2_blockcopy_pp_64x48_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_64x64_avx FPFX(blockcopy_pp_64x64_avx)
void xavs2_blockcopy_pp_64x64_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_pp_48x64_avx FPFX(blockcopy_pp_48x64_avx)
void xavs2_blockcopy_pp_48x64_avx(uint8_t *a, intptr_t stridea, const uint8_t *b, intptr_t strideb);

#if defined(__AVX2__)
#define xavs2_blockfill_s_16x16_avx2 FPFX(blockfill_s_16x16_avx2)
void xavs2_blockfill_s_16x16_avx2(int16_t* dst, intptr_t dstride, int16_t val);
#define xavs2_blockfill_s_32x32_avx2 FPFX(blockfill_s_32x32_avx2)
void xavs2_blockfill_s_32x32_avx2(int16_t* dst, intptr_t dstride, int16_t val);
// copy_sp primitives
// 16 x N
#define xavs2_blockcopy_sp_16x16_avx2 FPFX(blockcopy_sp_16x16_avx2)
void xavs2_blockcopy_sp_16x16_avx2(uint8_t *a, intptr_t stridea, const int16_t* b, intptr_t strideb);
#define xavs2_blockcopy_sp_16x32_avx2 FPFX(blockcopy_sp_16x32_avx2)
void xavs2_blockcopy_sp_16x32_avx2(uint8_t *a, intptr_t stridea, const int16_t* b, intptr_t strideb);

// 32 x N
#define xavs2_blockcopy_sp_32x32_avx2 FPFX(blockcopy_sp_32x32_avx2)
void xavs2_blockcopy_sp_32x32_avx2(uint8_t *a, intptr_t stridea, const int16_t* b, intptr_t strideb);
#define xavs2_blockcopy_sp_32x64_avx2 FPFX(blockcopy_sp_32x64_avx2)
void xavs2_blockcopy_sp_32x64_avx2(uint8_t *a, intptr_t stridea, const int16_t* b, intptr_t strideb);

// 64 x N
#define xavs2_blockcopy_sp_64x64_avx2 FPFX(blockcopy_sp_64x64_avx2)
void xavs2_blockcopy_sp_64x64_avx2(uint8_t *a, intptr_t stridea, const int16_t* b, intptr_t strideb);
// copy_ps primitives
// 16 x N
#define xavs2_blockcopy_ps_16x16_avx2 FPFX(blockcopy_ps_16x16_avx2)
void xavs2_blockcopy_ps_16x16_avx2(int16_t* a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_ps_16x32_avx2 FPFX(blockcopy_ps_16x32_avx2)
void xavs2_blockcopy_ps_16x32_avx2(int16_t* a, intptr_t stridea, const uint8_t *b, intptr_t strideb);

// 32 x N
#define xavs2_blockcopy_ps_32x32_avx2 FPFX(blockcopy_ps_32x32_avx2)
void xavs2_blockcopy_ps_32x32_avx2(int16_t* a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#define xavs2_blockcopy_ps_32x64_avx2 FPFX(blockcopy_ps_32x64_avx2)
void xavs2_blockcopy_ps_32x64_avx2(int16_t* a, intptr_t stridea, const uint8_t *b, intptr_t strideb);

// 64 x N
#define xavs2_blockcopy_ps_64x64_avx2 FPFX(blockcopy_ps_64x64_avx2)
void xavs2_blockcopy_ps_64x64_avx2(int16_t* a, intptr_t stridea, const uint8_t *b, intptr_t strideb);
#endif

#undef BLOCKCOPY_COMMON
#undef BLOCKCOPY_SS_PP
#undef BLOCKCOPY_SP
#undef BLOCKCOPY_PS
#undef SETUP_BLOCKCOPY_PS
#undef SETUP_BLOCKCOPY_SP
#undef SETUP_BLOCKCOPY_SS_PP
#undef SETUP_BLOCKCOPY_FUNC

#endif // __xXAVS2_BLOCKCOPY_H
