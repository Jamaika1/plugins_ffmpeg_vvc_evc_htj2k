/*
 * Copyright (c) 2013 Seppo Tomperi
 * Copyright (c) 2013-2014 Pierre-Edouard Lepere
 * Copyright (c) 2023-2024 Wu Jianhua
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/config.h"

#include "libavutil/cpu.h"
#include "libavutil/mem_internal.h"
#include "libavutil/x86/asm.h"
#include "libavutil/x86/cpu.h"
#include "libavcodec/hevcdsp.h"
#include "hevcdsp.h"
#include "h2656dsp.h"

#define LFC_FUNC(DIR, DEPTH, OPT) \
void ff_hevc_ ## DIR ## _loop_filter_chroma_ ## DEPTH ## _ ## OPT(uint8_t *pix, ptrdiff_t stride, const int *tc, const uint8_t *no_p, const uint8_t *no_q);

#define LFL_FUNC(DIR, DEPTH, OPT) \
void ff_hevc_ ## DIR ## _loop_filter_luma_ ## DEPTH ## _ ## OPT(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q);

#define LFC_FUNCS(type, depth, opt) \
    LFC_FUNC(h, depth, opt)  \
    LFC_FUNC(v, depth, opt)

#define LFL_FUNCS(type, depth, opt) \
    LFL_FUNC(h, depth, opt)  \
    LFL_FUNC(v, depth, opt)

LFC_FUNCS(uint8_t,   8, sse2)
LFC_FUNCS(uint8_t,  10, sse2)
LFC_FUNCS(uint8_t,  12, sse2)
LFC_FUNCS(uint8_t,   8, avx)
LFC_FUNCS(uint8_t,  10, avx)
LFC_FUNCS(uint8_t,  12, avx)
LFL_FUNCS(uint8_t,   8, sse2)
LFL_FUNCS(uint8_t,  10, sse2)
LFL_FUNCS(uint8_t,  12, sse2)
LFL_FUNCS(uint8_t,   8, ssse3)
LFL_FUNCS(uint8_t,  10, ssse3)
LFL_FUNCS(uint8_t,  12, ssse3)
LFL_FUNCS(uint8_t,   8, avx)
LFL_FUNCS(uint8_t,  10, avx)
LFL_FUNCS(uint8_t,  12, avx)

#define IDCT_DC_FUNCS(W, opt) \
void ff_hevc_idct_ ## W ## _dc_8_ ## opt(int16_t *coeffs); \
void ff_hevc_idct_ ## W ## _dc_10_ ## opt(int16_t *coeffs); \
void ff_hevc_idct_ ## W ## _dc_12_ ## opt(int16_t *coeffs)

IDCT_DC_FUNCS(4x4,   mmxext);
IDCT_DC_FUNCS(8x8,   sse2);
IDCT_DC_FUNCS(16x16, sse2);
IDCT_DC_FUNCS(32x32, sse2);
IDCT_DC_FUNCS(16x16, avx2);
IDCT_DC_FUNCS(32x32, avx2);

#define IDCT_FUNCS(opt)                                             \
void ff_hevc_idct_4x4_8_    ## opt(int16_t *coeffs, int col_limit); \
void ff_hevc_idct_4x4_10_   ## opt(int16_t *coeffs, int col_limit); \
void ff_hevc_idct_8x8_8_    ## opt(int16_t *coeffs, int col_limit); \
void ff_hevc_idct_8x8_10_   ## opt(int16_t *coeffs, int col_limit); \
void ff_hevc_idct_16x16_8_  ## opt(int16_t *coeffs, int col_limit); \
void ff_hevc_idct_16x16_10_ ## opt(int16_t *coeffs, int col_limit); \
void ff_hevc_idct_32x32_8_  ## opt(int16_t *coeffs, int col_limit); \
void ff_hevc_idct_32x32_10_ ## opt(int16_t *coeffs, int col_limit);

IDCT_FUNCS(sse2)
IDCT_FUNCS(avx)


#define ff_hevc_pel_filters ff_hevc_qpel_filters
#define DECL_HV_FILTER(f)                                  \
    const uint8_t *hf = ff_hevc_ ## f ## _filters[mx];     \
    const uint8_t *vf = ff_hevc_ ## f ## _filters[my];

#define FW_PUT(p, a, b, depth, opt) \
static void hevc_put_ ## a ## _ ## depth ## _##opt(int16_t *dst, const uint8_t *src, ptrdiff_t srcstride,    \
                                               int height, intptr_t mx, intptr_t my,int width)               \
{                                                                                                            \
    DECL_HV_FILTER(p)                                                                                        \
    ff_h2656_put_ ## b ## _ ## depth ## _##opt(dst, 2 * MAX_PB_SIZE, src, srcstride, height, hf, vf, width); \
}

#define FW_PUT_UNI(p, a, b, depth, opt) \
static void hevc_put_uni_ ## a ## _ ## depth ## _##opt(uint8_t *dst, ptrdiff_t dststride,                        \
                                                        const uint8_t *src, ptrdiff_t srcstride,             \
                                                        int height, intptr_t mx, intptr_t my, int width)     \
{                                                                                                            \
    DECL_HV_FILTER(p)                                                                                        \
    ff_h2656_put_uni_ ## b ## _ ## depth ## _##opt(dst, dststride, src, srcstride, height, hf, vf, width);   \
}

#if ARCH_X86_64 && HAVE_SSE4_EXTERNAL

#define FW_PUT_FUNCS(p, a, b, depth, opt) \
    FW_PUT(p, a, b, depth, opt) \
    FW_PUT_UNI(p, a, b, depth, opt)

#define FW_PEL(w, depth, opt) FW_PUT_FUNCS(pel, pel_pixels##w, pixels##w, depth, opt)

#define FW_DIR(npel, n, w, depth, opt) \
    FW_PUT_FUNCS(npel, npel ## _h##w,  n ## tap_h##w,  depth, opt) \
    FW_PUT_FUNCS(npel, npel ## _v##w,  n ## tap_v##w,  depth, opt)

#define FW_DIR_HV(npel, n, w, depth, opt) \
    FW_PUT_FUNCS(npel, npel ## _hv##w,  n ## tap_hv##w,  depth, opt)

FW_PEL(4,   8, sse4)
FW_PEL(6,   8, sse4)
FW_PEL(8,   8, sse4)
FW_PEL(12,  8, sse4)
FW_PEL(16,  8, sse4)
FW_PEL(4,  10, sse4)
FW_PEL(6,  10, sse4)
FW_PEL(8,  10, sse4)
FW_PEL(4,  12, sse4)
FW_PEL(6,  12, sse4)
FW_PEL(8,  12, sse4)

#define FW_EPEL(w, depth, opt) FW_DIR(epel, 4, w, depth, opt)
#define FW_EPEL_HV(w, depth, opt) FW_DIR_HV(epel, 4, w, depth, opt)
#define FW_EPEL_FUNCS(w, depth, opt) \
    FW_EPEL(w, depth, opt)           \
    FW_EPEL_HV(w, depth, opt)

FW_EPEL(12,  8, sse4)

FW_EPEL_FUNCS(4,   8, sse4)
FW_EPEL_FUNCS(6,   8, sse4)
FW_EPEL_FUNCS(8,   8, sse4)
FW_EPEL_FUNCS(16,  8, sse4)
FW_EPEL_FUNCS(4,  10, sse4)
FW_EPEL_FUNCS(6,  10, sse4)
FW_EPEL_FUNCS(8,  10, sse4)
FW_EPEL_FUNCS(4,  12, sse4)
FW_EPEL_FUNCS(6,  12, sse4)
FW_EPEL_FUNCS(8,  12, sse4)

#define FW_QPEL(w, depth, opt) FW_DIR(qpel, 8, w, depth, opt)
#define FW_QPEL_HV(w, depth, opt) FW_DIR_HV(qpel, 8, w, depth, opt)
#define FW_QPEL_FUNCS(w, depth, opt) \
    FW_QPEL(w, depth, opt)           \
    FW_QPEL_HV(w, depth, opt)

FW_QPEL(12, 8, sse4)
FW_QPEL(16, 8, sse4)

FW_QPEL_FUNCS(4,   8, sse4)
FW_QPEL_FUNCS(8,   8, sse4)
FW_QPEL_FUNCS(4,  10, sse4)
FW_QPEL_FUNCS(8,  10, sse4)
FW_QPEL_FUNCS(4,  12, sse4)
FW_QPEL_FUNCS(8,  12, sse4)

#if HAVE_AVX2_EXTERNAL

FW_PEL(32,  8, avx2)
FW_PUT(pel, pel_pixels16, pixels16, 10, avx2)

FW_EPEL(32,  8, avx2)
FW_EPEL(16, 10, avx2)

FW_EPEL_HV(32,  8, avx2)
FW_EPEL_HV(16, 10, avx2)

FW_QPEL(32,  8, avx2)
FW_QPEL(16, 10, avx2)

FW_QPEL_HV(16, 10, avx2)

#endif
#endif

#define mc_rep_func(name, bitd, step, W, opt) \
static void hevc_put_##name##W##_##bitd##_##opt(int16_t *_dst,                                                  \
                                            const uint8_t *_src, ptrdiff_t _srcstride, int height,              \
                                            intptr_t mx, intptr_t my, int width)                                \
{                                                                                                               \
    int i;                                                                                                      \
    int16_t *dst;                                                                                               \
    for (i = 0; i < W; i += step) {                                                                             \
        const uint8_t *src  = _src + (i * ((bitd + 7) / 8));                                                    \
        dst = _dst + i;                                                                                         \
        hevc_put_##name##step##_##bitd##_##opt(dst, src, _srcstride, height, mx, my, width);                    \
    }                                                                                                           \
}
#define mc_rep_uni_func(name, bitd, step, W, opt) \
static void hevc_put_uni_##name##W##_##bitd##_##opt(uint8_t *_dst, ptrdiff_t dststride,                         \
                                                const uint8_t *_src, ptrdiff_t _srcstride, int height,          \
                                                intptr_t mx, intptr_t my, int width)                            \
{                                                                                                               \
    int i;                                                                                                      \
    uint8_t *dst;                                                                                               \
    for (i = 0; i < W; i += step) {                                                                             \
        const uint8_t *src = _src + (i * ((bitd + 7) / 8));                                                     \
        dst = _dst + (i * ((bitd + 7) / 8));                                                                    \
        hevc_put_uni_##name##step##_##bitd##_##opt(dst, dststride, src, _srcstride,                             \
                                                      height, mx, my, width);                                   \
    }                                                                                                           \
}
#define mc_rep_bi_func(name, bitd, step, W, opt) \
static void ff_hevc_put_bi_##name##W##_##bitd##_##opt(uint8_t *_dst, ptrdiff_t dststride, const uint8_t *_src,  \
                                               ptrdiff_t _srcstride, const int16_t *_src2,                      \
                                               int height, intptr_t mx, intptr_t my, int width)                 \
{                                                                                                               \
    int i;                                                                                                      \
    uint8_t  *dst;                                                                                              \
    for (i = 0; i < W ; i += step) {                                                                            \
        const uint8_t *src  = _src + (i * ((bitd + 7) / 8));                                                    \
        const int16_t *src2 = _src2 + i;                                                                        \
        dst  = _dst + (i * ((bitd + 7) / 8));                                                                   \
        ff_hevc_put_bi_##name##step##_##bitd##_##opt(dst, dststride, src, _srcstride, src2,                     \
                                                     height, mx, my, width);                                    \
    }                                                                                                           \
}

#define mc_rep_funcs(name, bitd, step, W, opt)        \
    mc_rep_func(name, bitd, step, W, opt)            \
    mc_rep_uni_func(name, bitd, step, W, opt)        \
    mc_rep_bi_func(name, bitd, step, W, opt)

#define mc_rep_func2(name, bitd, step1, step2, W, opt) \
static void hevc_put_##name##W##_##bitd##_##opt(int16_t *dst,                                                   \
                                                const uint8_t *src, ptrdiff_t _srcstride, int height,           \
                                                intptr_t mx, intptr_t my, int width)                            \
{                                                                                                               \
    hevc_put_##name##step1##_##bitd##_##opt(dst, src, _srcstride, height, mx, my, width);                       \
    hevc_put_##name##step2##_##bitd##_##opt(dst + step1, src + (step1 * ((bitd + 7) / 8)),                      \
                                            _srcstride, height, mx, my, width);                                 \
}
#define mc_rep_uni_func2(name, bitd, step1, step2, W, opt) \
static void hevc_put_uni_##name##W##_##bitd##_##opt(uint8_t *dst, ptrdiff_t dststride,                          \
                                                const uint8_t *src, ptrdiff_t _srcstride, int height,           \
                                                intptr_t mx, intptr_t my, int width)                            \
{                                                                                                               \
    hevc_put_uni_##name##step1##_##bitd##_##opt(dst, dststride, src, _srcstride, height, mx, my, width);     \
    hevc_put_uni_##name##step2##_##bitd##_##opt(dst + (step1 * ((bitd + 7) / 8)), dststride,                 \
                                                   src + (step1 * ((bitd + 7) / 8)), _srcstride,                \
                                                   height, mx, my, width);                                      \
}
#define mc_rep_bi_func2(name, bitd, step1, step2, W, opt) \
static void ff_hevc_put_bi_##name##W##_##bitd##_##opt(uint8_t *dst, ptrdiff_t dststride, const uint8_t *src,    \
                                               ptrdiff_t _srcstride, const int16_t *src2,                       \
                                               int height, intptr_t mx, intptr_t my, int width)                 \
{                                                                                                               \
    ff_hevc_put_bi_##name##step1##_##bitd##_##opt(dst, dststride, src, _srcstride, src2, height, mx, my, width);\
    ff_hevc_put_bi_##name##step2##_##bitd##_##opt(dst + (step1 * ((bitd + 7) / 8)), dststride,                  \
                                                  src + (step1 * ((bitd + 7) / 8)), _srcstride,                 \
                                                  src2 + step1, height, mx, my, width);                         \
}

#define mc_rep_funcs2(name, bitd, step1, step2, W, opt) \
    mc_rep_func2(name, bitd, step1, step2, W, opt)      \
    mc_rep_uni_func2(name, bitd, step1, step2, W, opt)  \
    mc_rep_bi_func2(name, bitd, step1, step2, W, opt)

#if ARCH_X86_64 && HAVE_SSE4_EXTERNAL

#define mc_rep_mix_10(name, width1, width2, width3, opt1, opt2, width4)                                       \
static void hevc_put_##name##width1##_10_##opt1(int16_t *dst, const uint8_t *src, ptrdiff_t _srcstride,       \
                                            int height, intptr_t mx, intptr_t my, int width)                  \
                                                                                                              \
{                                                                                                             \
    hevc_put_##name##width2##_10_##opt1(dst, src, _srcstride, height, mx, my, width);                         \
    hevc_put_##name##width3##_10_##opt2(dst+ width2, src+ width4, _srcstride, height, mx, my, width);         \
}

#define mc_bi_rep_mix_10(name, width1, width2, width3, opt1, opt2, width4)                                    \
static void ff_hevc_put_bi_##name##width1##_10_##opt1(uint8_t *dst, ptrdiff_t dststride, const uint8_t *src,  \
                                               ptrdiff_t _srcstride, const int16_t *src2,                     \
                                               int height, intptr_t mx, intptr_t my, int width)               \
{                                                                                                             \
    ff_hevc_put_bi_##name##width2##_10_##opt1(dst, dststride, src, _srcstride, src2,                          \
                                              height, mx, my, width);                                         \
    ff_hevc_put_bi_##name##width3##_10_##opt2(dst+width4, dststride, src+width4, _srcstride, src2+width2,     \
                                              height, mx, my, width);                                         \
}

#define mc_uni_rep_mix_10(name, width1, width2, width3, opt1, opt2, width4)                                   \
static void hevc_put_uni_##name##width1##_10_##opt1(uint8_t *dst, ptrdiff_t dststride,                        \
                                                const uint8_t *src, ptrdiff_t _srcstride, int height,         \
                                                intptr_t mx, intptr_t my, int width)                          \
{                                                                                                             \
    hevc_put_uni_##name##width2##_10_##opt1(dst, dststride, src, _srcstride,                               \
                                               height, mx, my, width);                                        \
    hevc_put_uni_##name##width3##_10_##opt2(dst+width4, dststride, src+width4, _srcstride,                 \
                                               height, mx, my, width);                                        \
}

#define mc_rep_mixs_10(name, width1, width2, width3, opt1, opt2, width4)   \
mc_rep_mix_10(name, width1, width2, width3, opt1, opt2, width4)            \
mc_bi_rep_mix_10(name, width1, width2, width3, opt1, opt2, width4)         \
mc_uni_rep_mix_10(name, width1, width2, width3, opt1, opt2, width4)

#define mc_rep_mix_8(name, width1, width2, width3, opt1, opt2)                                                \
static void hevc_put_##name##width1##_8_##opt1(int16_t *dst, const uint8_t *src, ptrdiff_t _srcstride,        \
                                           int height, intptr_t mx, intptr_t my, int width)                   \
                                                                                                              \
{                                                                                                             \
    hevc_put_##name##width2##_8_##opt1(dst, src, _srcstride, height, mx, my, width);                          \
    hevc_put_##name##width3##_8_##opt2(dst+ width2, src+ width2, _srcstride, height, mx, my, width);          \
}

#define mc_bi_rep_mix_8(name, width1, width2, width3, opt1, opt2)                                             \
static void ff_hevc_put_bi_##name##width1##_8_##opt1(uint8_t *dst, ptrdiff_t dststride, const uint8_t *src,   \
                                              ptrdiff_t _srcstride, const int16_t *src2,                      \
                                              int height, intptr_t mx, intptr_t my, int width)                \
{                                                                                                             \
    ff_hevc_put_bi_##name##width2##_8_##opt1(dst, dststride, src, _srcstride,                                 \
                                             src2, height, mx, my, width);                                    \
    ff_hevc_put_bi_##name##width3##_8_##opt2(dst+width2, dststride, src+width2, _srcstride,                   \
                                             src2+width2, height, mx, my, width);                             \
}

#define mc_uni_rep_mix_8(name, width1, width2, width3, opt1, opt2)                                            \
static void hevc_put_uni_##name##width1##_8_##opt1(uint8_t *dst, ptrdiff_t dststride,                             \
                                               const uint8_t *src, ptrdiff_t _srcstride, int height,          \
                                               intptr_t mx, intptr_t my, int width)                           \
{                                                                                                             \
    hevc_put_uni_##name##width2##_8_##opt1(dst, dststride, src, _srcstride,                                \
                                              height, mx, my, width);                                         \
    hevc_put_uni_##name##width3##_8_##opt2(dst+width2, dststride, src+width2, _srcstride,                  \
                                              height, mx, my, width);                                         \
}

#define mc_rep_mixs_8(name, width1, width2, width3, opt1, opt2)   \
mc_rep_mix_8(name, width1, width2, width3, opt1, opt2)            \
mc_bi_rep_mix_8(name, width1, width2, width3, opt1, opt2)         \
mc_uni_rep_mix_8(name, width1, width2, width3, opt1, opt2)

#if HAVE_AVX2_EXTERNAL

mc_rep_mixs_8(pel_pixels, 48, 32, 16, avx2, sse4)
mc_rep_mixs_8(epel_hv,    48, 32, 16, avx2, sse4)
mc_rep_mixs_8(epel_h ,    48, 32, 16, avx2, sse4)
mc_rep_mixs_8(epel_v ,    48, 32, 16, avx2, sse4)

mc_rep_mix_10(pel_pixels, 24, 16, 8, avx2, sse4, 32)
mc_bi_rep_mix_10(pel_pixels,24, 16, 8, avx2, sse4, 32)
mc_rep_mixs_10(epel_hv,   24, 16, 8, avx2, sse4, 32)
mc_rep_mixs_10(epel_h ,   24, 16, 8, avx2, sse4, 32)
mc_rep_mixs_10(epel_v ,   24, 16, 8, avx2, sse4, 32)


mc_rep_mixs_10(qpel_h ,   24, 16, 8, avx2, sse4, 32)
mc_rep_mixs_10(qpel_v ,   24, 16, 8, avx2, sse4, 32)
mc_rep_mixs_10(qpel_hv,   24, 16, 8, avx2, sse4, 32)


mc_rep_funcs(pel_pixels, 8, 32, 64, avx2)

mc_rep_uni_func(pel_pixels, 8, 64, 128, avx2)//used for 10bit
mc_rep_uni_func(pel_pixels, 8, 32, 96, avx2) //used for 10bit

mc_rep_func(pel_pixels, 10, 16, 32, avx2)
mc_rep_func(pel_pixels, 10, 16, 48, avx2)
mc_rep_func(pel_pixels, 10, 32, 64, avx2)

mc_rep_bi_func(pel_pixels, 10, 16, 32, avx2)
mc_rep_bi_func(pel_pixels, 10, 16, 48, avx2)
mc_rep_bi_func(pel_pixels, 10, 32, 64, avx2)

mc_rep_funcs(epel_h, 8, 32, 64, avx2)

mc_rep_funcs(epel_v, 8, 32, 64, avx2)

mc_rep_funcs(epel_h, 10, 16, 32, avx2)
mc_rep_funcs(epel_h, 10, 16, 48, avx2)
mc_rep_funcs(epel_h, 10, 32, 64, avx2)

mc_rep_funcs(epel_v, 10, 16, 32, avx2)
mc_rep_funcs(epel_v, 10, 16, 48, avx2)
mc_rep_funcs(epel_v, 10, 32, 64, avx2)


mc_rep_funcs(epel_hv,  8, 32, 64, avx2)

mc_rep_funcs(epel_hv, 10, 16, 32, avx2)
mc_rep_funcs(epel_hv, 10, 16, 48, avx2)
mc_rep_funcs(epel_hv, 10, 32, 64, avx2)

mc_rep_funcs(qpel_h, 8, 32, 64, avx2)
mc_rep_mixs_8(qpel_h ,  48, 32, 16, avx2, sse4)

mc_rep_funcs(qpel_v, 8, 32, 64, avx2)
mc_rep_mixs_8(qpel_v,  48, 32, 16, avx2, sse4)

mc_rep_funcs(qpel_h, 10, 16, 32, avx2)
mc_rep_funcs(qpel_h, 10, 16, 48, avx2)
mc_rep_funcs(qpel_h, 10, 32, 64, avx2)

mc_rep_funcs(qpel_v, 10, 16, 32, avx2)
mc_rep_funcs(qpel_v, 10, 16, 48, avx2)
mc_rep_funcs(qpel_v, 10, 32, 64, avx2)

mc_rep_funcs(qpel_hv, 10, 16, 32, avx2)
mc_rep_funcs(qpel_hv, 10, 16, 48, avx2)
mc_rep_funcs(qpel_hv, 10, 32, 64, avx2)

#endif //AVX2

mc_rep_funcs(pel_pixels, 8, 16, 64, sse4)
mc_rep_funcs(pel_pixels, 8, 16, 48, sse4)
mc_rep_funcs(pel_pixels, 8, 16, 32, sse4)
mc_rep_funcs(pel_pixels, 8,  8, 24, sse4)
mc_rep_funcs(pel_pixels,10,  8, 64, sse4)
mc_rep_funcs(pel_pixels,10,  8, 48, sse4)
mc_rep_funcs(pel_pixels,10,  8, 32, sse4)
mc_rep_funcs(pel_pixels,10,  8, 24, sse4)
mc_rep_funcs(pel_pixels,10,  8, 16, sse4)
mc_rep_funcs(pel_pixels,10,  4, 12, sse4)
mc_rep_funcs(pel_pixels,12,  8, 64, sse4)
mc_rep_funcs(pel_pixels,12,  8, 48, sse4)
mc_rep_funcs(pel_pixels,12,  8, 32, sse4)
mc_rep_funcs(pel_pixels,12,  8, 24, sse4)
mc_rep_funcs(pel_pixels,12,  8, 16, sse4)
mc_rep_funcs(pel_pixels,12,  4, 12, sse4)

mc_rep_funcs(epel_h, 8, 16, 64, sse4)
mc_rep_funcs(epel_h, 8, 16, 48, sse4)
mc_rep_funcs(epel_h, 8, 16, 32, sse4)
mc_rep_funcs(epel_h, 8,  8, 24, sse4)
mc_rep_funcs(epel_h,10,  8, 64, sse4)
mc_rep_funcs(epel_h,10,  8, 48, sse4)
mc_rep_funcs(epel_h,10,  8, 32, sse4)
mc_rep_funcs(epel_h,10,  8, 24, sse4)
mc_rep_funcs(epel_h,10,  8, 16, sse4)
mc_rep_funcs(epel_h,10,  4, 12, sse4)
mc_rep_funcs(epel_h,12,  8, 64, sse4)
mc_rep_funcs(epel_h,12,  8, 48, sse4)
mc_rep_funcs(epel_h,12,  8, 32, sse4)
mc_rep_funcs(epel_h,12,  8, 24, sse4)
mc_rep_funcs(epel_h,12,  8, 16, sse4)
mc_rep_funcs(epel_h,12,  4, 12, sse4)
mc_rep_funcs(epel_v, 8, 16, 64, sse4)
mc_rep_funcs(epel_v, 8, 16, 48, sse4)
mc_rep_funcs(epel_v, 8, 16, 32, sse4)
mc_rep_funcs(epel_v, 8,  8, 24, sse4)
mc_rep_funcs(epel_v,10,  8, 64, sse4)
mc_rep_funcs(epel_v,10,  8, 48, sse4)
mc_rep_funcs(epel_v,10,  8, 32, sse4)
mc_rep_funcs(epel_v,10,  8, 24, sse4)
mc_rep_funcs(epel_v,10,  8, 16, sse4)
mc_rep_funcs(epel_v,10,  4, 12, sse4)
mc_rep_funcs(epel_v,12,  8, 64, sse4)
mc_rep_funcs(epel_v,12,  8, 48, sse4)
mc_rep_funcs(epel_v,12,  8, 32, sse4)
mc_rep_funcs(epel_v,12,  8, 24, sse4)
mc_rep_funcs(epel_v,12,  8, 16, sse4)
mc_rep_funcs(epel_v,12,  4, 12, sse4)
mc_rep_funcs(epel_hv, 8, 16, 64, sse4)
mc_rep_funcs(epel_hv, 8, 16, 48, sse4)
mc_rep_funcs(epel_hv, 8, 16, 32, sse4)
mc_rep_funcs(epel_hv, 8,  8, 24, sse4)
mc_rep_funcs2(epel_hv,8,  8,  4, 12, sse4)
mc_rep_funcs(epel_hv,10,  8, 64, sse4)
mc_rep_funcs(epel_hv,10,  8, 48, sse4)
mc_rep_funcs(epel_hv,10,  8, 32, sse4)
mc_rep_funcs(epel_hv,10,  8, 24, sse4)
mc_rep_funcs(epel_hv,10,  8, 16, sse4)
mc_rep_funcs(epel_hv,10,  4, 12, sse4)
mc_rep_funcs(epel_hv,12,  8, 64, sse4)
mc_rep_funcs(epel_hv,12,  8, 48, sse4)
mc_rep_funcs(epel_hv,12,  8, 32, sse4)
mc_rep_funcs(epel_hv,12,  8, 24, sse4)
mc_rep_funcs(epel_hv,12,  8, 16, sse4)
mc_rep_funcs(epel_hv,12,  4, 12, sse4)

mc_rep_funcs(qpel_h, 8, 16, 64, sse4)
mc_rep_funcs(qpel_h, 8, 16, 48, sse4)
mc_rep_funcs(qpel_h, 8, 16, 32, sse4)
mc_rep_funcs(qpel_h, 8,  8, 24, sse4)
mc_rep_funcs(qpel_h,10,  8, 64, sse4)
mc_rep_funcs(qpel_h,10,  8, 48, sse4)
mc_rep_funcs(qpel_h,10,  8, 32, sse4)
mc_rep_funcs(qpel_h,10,  8, 24, sse4)
mc_rep_funcs(qpel_h,10,  8, 16, sse4)
mc_rep_funcs(qpel_h,10,  4, 12, sse4)
mc_rep_funcs(qpel_h,12,  8, 64, sse4)
mc_rep_funcs(qpel_h,12,  8, 48, sse4)
mc_rep_funcs(qpel_h,12,  8, 32, sse4)
mc_rep_funcs(qpel_h,12,  8, 24, sse4)
mc_rep_funcs(qpel_h,12,  8, 16, sse4)
mc_rep_funcs(qpel_h,12,  4, 12, sse4)
mc_rep_funcs(qpel_v, 8, 16, 64, sse4)
mc_rep_funcs(qpel_v, 8, 16, 48, sse4)
mc_rep_funcs(qpel_v, 8, 16, 32, sse4)
mc_rep_funcs(qpel_v, 8,  8, 24, sse4)
mc_rep_funcs(qpel_v,10,  8, 64, sse4)
mc_rep_funcs(qpel_v,10,  8, 48, sse4)
mc_rep_funcs(qpel_v,10,  8, 32, sse4)
mc_rep_funcs(qpel_v,10,  8, 24, sse4)
mc_rep_funcs(qpel_v,10,  8, 16, sse4)
mc_rep_funcs(qpel_v,10,  4, 12, sse4)
mc_rep_funcs(qpel_v,12,  8, 64, sse4)
mc_rep_funcs(qpel_v,12,  8, 48, sse4)
mc_rep_funcs(qpel_v,12,  8, 32, sse4)
mc_rep_funcs(qpel_v,12,  8, 24, sse4)
mc_rep_funcs(qpel_v,12,  8, 16, sse4)
mc_rep_funcs(qpel_v,12,  4, 12, sse4)
mc_rep_funcs(qpel_hv, 8,  8, 64, sse4)
mc_rep_funcs(qpel_hv, 8,  8, 48, sse4)
mc_rep_funcs(qpel_hv, 8,  8, 32, sse4)
mc_rep_funcs(qpel_hv, 8,  8, 24, sse4)
mc_rep_funcs(qpel_hv, 8,  8, 16, sse4)
mc_rep_funcs2(qpel_hv,8,  8,  4, 12, sse4)
mc_rep_funcs(qpel_hv,10,  8, 64, sse4)
mc_rep_funcs(qpel_hv,10,  8, 48, sse4)
mc_rep_funcs(qpel_hv,10,  8, 32, sse4)
mc_rep_funcs(qpel_hv,10,  8, 24, sse4)
mc_rep_funcs(qpel_hv,10,  8, 16, sse4)
mc_rep_funcs(qpel_hv,10,  4, 12, sse4)
mc_rep_funcs(qpel_hv,12,  8, 64, sse4)
mc_rep_funcs(qpel_hv,12,  8, 48, sse4)
mc_rep_funcs(qpel_hv,12,  8, 32, sse4)
mc_rep_funcs(qpel_hv,12,  8, 24, sse4)
mc_rep_funcs(qpel_hv,12,  8, 16, sse4)
mc_rep_funcs(qpel_hv,12,  4, 12, sse4)

#define mc_rep_uni_w(bitd, step, W, opt) \
void ff_hevc_put_uni_w##W##_##bitd##_##opt(uint8_t *_dst, ptrdiff_t dststride, const int16_t *_src, \
                                           int height, int denom,  int _wx, int _ox)                \
{                                                                                                                       \
    int i;                                                                                                              \
    uint8_t *dst;                                                                                                       \
    for (i = 0; i < W; i += step) {                                                                                     \
        const int16_t *src = _src + i;                                                                                  \
        dst= _dst + (i * ((bitd + 7) / 8));                                                                             \
        ff_hevc_put_uni_w##step##_##bitd##_##opt(dst, dststride, src,                               \
                                                 height, denom, _wx, _ox);                          \
    }                                                                                                                   \
}

mc_rep_uni_w(8, 6, 12, sse4)
mc_rep_uni_w(8, 8, 16, sse4)
mc_rep_uni_w(8, 8, 24, sse4)
mc_rep_uni_w(8, 8, 32, sse4)
mc_rep_uni_w(8, 8, 48, sse4)
mc_rep_uni_w(8, 8, 64, sse4)

mc_rep_uni_w(10, 6, 12, sse4)
mc_rep_uni_w(10, 8, 16, sse4)
mc_rep_uni_w(10, 8, 24, sse4)
mc_rep_uni_w(10, 8, 32, sse4)
mc_rep_uni_w(10, 8, 48, sse4)
mc_rep_uni_w(10, 8, 64, sse4)

mc_rep_uni_w(12, 6, 12, sse4)
mc_rep_uni_w(12, 8, 16, sse4)
mc_rep_uni_w(12, 8, 24, sse4)
mc_rep_uni_w(12, 8, 32, sse4)
mc_rep_uni_w(12, 8, 48, sse4)
mc_rep_uni_w(12, 8, 64, sse4)

#define mc_rep_bi_w(bitd, step, W, opt) \
void ff_hevc_put_bi_w##W##_##bitd##_##opt(uint8_t *_dst, ptrdiff_t dststride, const int16_t *_src, \
                                          const int16_t *_src2, int height,                        \
                                          int denom,  int _wx0,  int _wx1, int _ox0, int _ox1)     \
{                                                                                                                       \
    int i;                                                                                                              \
    uint8_t *dst;                                                                                                       \
    for (i = 0; i < W; i += step) {                                                                                     \
        const int16_t *src  = _src  + i;                                                                                \
        const int16_t *src2 = _src2 + i;                                                                                \
        dst  = _dst  + (i * ((bitd + 7) / 8));                                                                          \
        ff_hevc_put_bi_w##step##_##bitd##_##opt(dst, dststride, src, src2,                         \
                                                height, denom, _wx0, _wx1, _ox0, _ox1);            \
    }                                                                                                                   \
}

mc_rep_bi_w(8, 6, 12, sse4)
mc_rep_bi_w(8, 8, 16, sse4)
mc_rep_bi_w(8, 8, 24, sse4)
mc_rep_bi_w(8, 8, 32, sse4)
mc_rep_bi_w(8, 8, 48, sse4)
mc_rep_bi_w(8, 8, 64, sse4)

mc_rep_bi_w(10, 6, 12, sse4)
mc_rep_bi_w(10, 8, 16, sse4)
mc_rep_bi_w(10, 8, 24, sse4)
mc_rep_bi_w(10, 8, 32, sse4)
mc_rep_bi_w(10, 8, 48, sse4)
mc_rep_bi_w(10, 8, 64, sse4)

mc_rep_bi_w(12, 6, 12, sse4)
mc_rep_bi_w(12, 8, 16, sse4)
mc_rep_bi_w(12, 8, 24, sse4)
mc_rep_bi_w(12, 8, 32, sse4)
mc_rep_bi_w(12, 8, 48, sse4)
mc_rep_bi_w(12, 8, 64, sse4)

#define mc_uni_w_func(name, bitd, W, opt) \
static void hevc_put_uni_w_##name##W##_##bitd##_##opt(uint8_t *_dst, ptrdiff_t _dststride,          \
                                                      const uint8_t *_src, ptrdiff_t _srcstride,    \
                                                      int height, int denom,                        \
                                                      int _wx, int _ox,                             \
                                                      intptr_t mx, intptr_t my, int width)          \
{                                                                                                   \
    LOCAL_ALIGNED_16(int16_t, temp, [71 * MAX_PB_SIZE]);                                            \
    hevc_put_##name##W##_##bitd##_##opt(temp, _src, _srcstride, height, mx, my, width);             \
    ff_hevc_put_uni_w##W##_##bitd##_##opt(_dst, _dststride, temp, height, denom, _wx, _ox);         \
}

#define mc_uni_w_funcs(name, bitd, opt)      \
        mc_uni_w_func(name, bitd, 4, opt)    \
        mc_uni_w_func(name, bitd, 8, opt)    \
        mc_uni_w_func(name, bitd, 12, opt)   \
        mc_uni_w_func(name, bitd, 16, opt)   \
        mc_uni_w_func(name, bitd, 24, opt)   \
        mc_uni_w_func(name, bitd, 32, opt)   \
        mc_uni_w_func(name, bitd, 48, opt)   \
        mc_uni_w_func(name, bitd, 64, opt)

mc_uni_w_funcs(pel_pixels, 8, sse4)
mc_uni_w_func(pel_pixels, 8, 6, sse4)
mc_uni_w_funcs(epel_h, 8, sse4)
mc_uni_w_func(epel_h, 8, 6, sse4)
mc_uni_w_funcs(epel_v, 8, sse4)
mc_uni_w_func(epel_v, 8, 6, sse4)
mc_uni_w_funcs(epel_hv, 8, sse4)
mc_uni_w_func(epel_hv, 8, 6, sse4)
mc_uni_w_funcs(qpel_h, 8, sse4)
mc_uni_w_funcs(qpel_v, 8, sse4)
mc_uni_w_funcs(qpel_hv, 8, sse4)

mc_uni_w_funcs(pel_pixels, 10, sse4)
mc_uni_w_func(pel_pixels, 10, 6, sse4)
mc_uni_w_funcs(epel_h, 10, sse4)
mc_uni_w_func(epel_h, 10, 6, sse4)
mc_uni_w_funcs(epel_v, 10, sse4)
mc_uni_w_func(epel_v, 10, 6, sse4)
mc_uni_w_funcs(epel_hv, 10, sse4)
mc_uni_w_func(epel_hv, 10, 6, sse4)
mc_uni_w_funcs(qpel_h, 10, sse4)
mc_uni_w_funcs(qpel_v, 10, sse4)
mc_uni_w_funcs(qpel_hv, 10, sse4)

mc_uni_w_funcs(pel_pixels, 12, sse4)
mc_uni_w_func(pel_pixels, 12, 6, sse4)
mc_uni_w_funcs(epel_h, 12, sse4)
mc_uni_w_func(epel_h, 12, 6, sse4)
mc_uni_w_funcs(epel_v, 12, sse4)
mc_uni_w_func(epel_v, 12, 6, sse4)
mc_uni_w_funcs(epel_hv, 12, sse4)
mc_uni_w_func(epel_hv, 12, 6, sse4)
mc_uni_w_funcs(qpel_h, 12, sse4)
mc_uni_w_funcs(qpel_v, 12, sse4)
mc_uni_w_funcs(qpel_hv, 12, sse4)

#define mc_bi_w_func(name, bitd, W, opt) \
static void hevc_put_bi_w_##name##W##_##bitd##_##opt(uint8_t *_dst, ptrdiff_t _dststride,            \
                                                     const uint8_t *_src, ptrdiff_t _srcstride,      \
                                                     const int16_t *_src2,                           \
                                                     int height, int denom,                          \
                                                     int _wx0, int _wx1, int _ox0, int _ox1,         \
                                                     intptr_t mx, intptr_t my, int width)            \
{                                                                                                    \
    LOCAL_ALIGNED_16(int16_t, temp, [71 * MAX_PB_SIZE]);                                             \
    hevc_put_##name##W##_##bitd##_##opt(temp, _src, _srcstride, height, mx, my, width);              \
    ff_hevc_put_bi_w##W##_##bitd##_##opt(_dst, _dststride, temp, _src2,                              \
                                         height, denom, _wx0, _wx1, _ox0, _ox1);                     \
}

#define mc_bi_w_funcs(name, bitd, opt)      \
        mc_bi_w_func(name, bitd, 4, opt)    \
        mc_bi_w_func(name, bitd, 8, opt)    \
        mc_bi_w_func(name, bitd, 12, opt)   \
        mc_bi_w_func(name, bitd, 16, opt)   \
        mc_bi_w_func(name, bitd, 24, opt)   \
        mc_bi_w_func(name, bitd, 32, opt)   \
        mc_bi_w_func(name, bitd, 48, opt)   \
        mc_bi_w_func(name, bitd, 64, opt)

mc_bi_w_funcs(pel_pixels, 8, sse4)
mc_bi_w_func(pel_pixels, 8, 6, sse4)
mc_bi_w_funcs(epel_h, 8, sse4)
mc_bi_w_func(epel_h, 8, 6, sse4)
mc_bi_w_funcs(epel_v, 8, sse4)
mc_bi_w_func(epel_v, 8, 6, sse4)
mc_bi_w_funcs(epel_hv, 8, sse4)
mc_bi_w_func(epel_hv, 8, 6, sse4)
mc_bi_w_funcs(qpel_h, 8, sse4)
mc_bi_w_funcs(qpel_v, 8, sse4)
mc_bi_w_funcs(qpel_hv, 8, sse4)

mc_bi_w_funcs(pel_pixels, 10, sse4)
mc_bi_w_func(pel_pixels, 10, 6, sse4)
mc_bi_w_funcs(epel_h, 10, sse4)
mc_bi_w_func(epel_h, 10, 6, sse4)
mc_bi_w_funcs(epel_v, 10, sse4)
mc_bi_w_func(epel_v, 10, 6, sse4)
mc_bi_w_funcs(epel_hv, 10, sse4)
mc_bi_w_func(epel_hv, 10, 6, sse4)
mc_bi_w_funcs(qpel_h, 10, sse4)
mc_bi_w_funcs(qpel_v, 10, sse4)
mc_bi_w_funcs(qpel_hv, 10, sse4)

mc_bi_w_funcs(pel_pixels, 12, sse4)
mc_bi_w_func(pel_pixels, 12, 6, sse4)
mc_bi_w_funcs(epel_h, 12, sse4)
mc_bi_w_func(epel_h, 12, 6, sse4)
mc_bi_w_funcs(epel_v, 12, sse4)
mc_bi_w_func(epel_v, 12, 6, sse4)
mc_bi_w_funcs(epel_hv, 12, sse4)
mc_bi_w_func(epel_hv, 12, 6, sse4)
mc_bi_w_funcs(qpel_h, 12, sse4)
mc_bi_w_funcs(qpel_v, 12, sse4)
mc_bi_w_funcs(qpel_hv, 12, sse4)
#endif //ARCH_X86_64 && HAVE_SSE4_EXTERNAL

#define SAO_BAND_FILTER_FUNCS(bitd, opt)                                                                                   \
void ff_hevc_sao_band_filter_8_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src,  \
                                              const int16_t *sao_offset_val, int sao_left_class, int width, int height);         \
void ff_hevc_sao_band_filter_16_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src, \
                                               const int16_t *sao_offset_val, int sao_left_class, int width, int height);        \
void ff_hevc_sao_band_filter_32_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src, \
                                               const int16_t *sao_offset_val, int sao_left_class, int width, int height);        \
void ff_hevc_sao_band_filter_48_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src, \
                                               const int16_t *sao_offset_val, int sao_left_class, int width, int height);        \
void ff_hevc_sao_band_filter_64_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t _stride_dst, ptrdiff_t _stride_src, \
                                               const int16_t *sao_offset_val, int sao_left_class, int width, int height);

SAO_BAND_FILTER_FUNCS(8,  sse2)
SAO_BAND_FILTER_FUNCS(10, sse2)
SAO_BAND_FILTER_FUNCS(12, sse2)
SAO_BAND_FILTER_FUNCS(8,   avx)
SAO_BAND_FILTER_FUNCS(10,  avx)
SAO_BAND_FILTER_FUNCS(12,  avx)
SAO_BAND_FILTER_FUNCS(8,  avx2)
SAO_BAND_FILTER_FUNCS(10, avx2)
SAO_BAND_FILTER_FUNCS(12, avx2)

#define SAO_BAND_INIT(bitd, opt) do {                                       \
    c->sao_band_filter[0]      = ff_hevc_sao_band_filter_8_##bitd##_##opt;  \
    c->sao_band_filter[1]      = ff_hevc_sao_band_filter_16_##bitd##_##opt; \
    c->sao_band_filter[2]      = ff_hevc_sao_band_filter_32_##bitd##_##opt; \
    c->sao_band_filter[3]      = ff_hevc_sao_band_filter_48_##bitd##_##opt; \
    c->sao_band_filter[4]      = ff_hevc_sao_band_filter_64_##bitd##_##opt; \
} while (0)

#define SAO_EDGE_FILTER_FUNCS(bitd, opt)                                                                      \
void ff_hevc_sao_edge_filter_8_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t stride_dst,       \
                                              const int16_t *sao_offset_val, int eo, int width, int height);  \
void ff_hevc_sao_edge_filter_16_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t stride_dst,      \
                                               const int16_t *sao_offset_val, int eo, int width, int height); \
void ff_hevc_sao_edge_filter_32_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t stride_dst,      \
                                               const int16_t *sao_offset_val, int eo, int width, int height); \
void ff_hevc_sao_edge_filter_48_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t stride_dst,      \
                                               const int16_t *sao_offset_val, int eo, int width, int height); \
void ff_hevc_sao_edge_filter_64_##bitd##_##opt(uint8_t *_dst, const uint8_t *_src, ptrdiff_t stride_dst,      \
                                               const int16_t *sao_offset_val, int eo, int width, int height); \

SAO_EDGE_FILTER_FUNCS(8, ssse3)
SAO_EDGE_FILTER_FUNCS(8, avx2)
SAO_EDGE_FILTER_FUNCS(10, sse2)
SAO_EDGE_FILTER_FUNCS(10, avx2)
SAO_EDGE_FILTER_FUNCS(12, sse2)
SAO_EDGE_FILTER_FUNCS(12, avx2)

#define SAO_EDGE_INIT(bitd, opt) do {                                       \
    c->sao_edge_filter[0]      = ff_hevc_sao_edge_filter_8_##bitd##_##opt;  \
    c->sao_edge_filter[1]      = ff_hevc_sao_edge_filter_16_##bitd##_##opt; \
    c->sao_edge_filter[2]      = ff_hevc_sao_edge_filter_32_##bitd##_##opt; \
    c->sao_edge_filter[3]      = ff_hevc_sao_edge_filter_48_##bitd##_##opt; \
    c->sao_edge_filter[4]      = ff_hevc_sao_edge_filter_64_##bitd##_##opt; \
} while (0)

#define PEL_LINK(dst, idx1, idx2, idx3, name, D, opt) \
dst          [idx1][idx2][idx3] = hevc_put_       ## name ## _ ## D ## _##opt; \
dst ## _bi   [idx1][idx2][idx3] = ff_hevc_put_bi_ ## name ## _ ## D ## _##opt; \
dst ## _uni  [idx1][idx2][idx3] = hevc_put_uni_   ## name ## _ ## D ## _##opt; \
dst ## _uni_w[idx1][idx2][idx3] = hevc_put_uni_w_ ## name ## _ ## D ## _##opt; \
dst ## _bi_w [idx1][idx2][idx3] = hevc_put_bi_w_  ## name ## _ ## D ## _##opt

#define EPEL_LINKS(pointer, my, mx, fname, bitd, opt )           \
        PEL_LINK(pointer, 1, my , mx , fname##4 ,  bitd, opt ); \
        PEL_LINK(pointer, 2, my , mx , fname##6 ,  bitd, opt ); \
        PEL_LINK(pointer, 3, my , mx , fname##8 ,  bitd, opt ); \
        PEL_LINK(pointer, 4, my , mx , fname##12,  bitd, opt ); \
        PEL_LINK(pointer, 5, my , mx , fname##16,  bitd, opt ); \
        PEL_LINK(pointer, 6, my , mx , fname##24,  bitd, opt ); \
        PEL_LINK(pointer, 7, my , mx , fname##32,  bitd, opt ); \
        PEL_LINK(pointer, 8, my , mx , fname##48,  bitd, opt ); \
        PEL_LINK(pointer, 9, my , mx , fname##64,  bitd, opt )
#define QPEL_LINKS(pointer, my, mx, fname, bitd, opt)           \
        PEL_LINK(pointer, 1, my , mx , fname##4 ,  bitd, opt ); \
        PEL_LINK(pointer, 3, my , mx , fname##8 ,  bitd, opt ); \
        PEL_LINK(pointer, 4, my , mx , fname##12,  bitd, opt ); \
        PEL_LINK(pointer, 5, my , mx , fname##16,  bitd, opt ); \
        PEL_LINK(pointer, 6, my , mx , fname##24,  bitd, opt ); \
        PEL_LINK(pointer, 7, my , mx , fname##32,  bitd, opt ); \
        PEL_LINK(pointer, 8, my , mx , fname##48,  bitd, opt ); \
        PEL_LINK(pointer, 9, my , mx , fname##64,  bitd, opt )

void ff_hevc_dsp_init_x86(HEVCDSPContext *c, const int bit_depth)
{
    int cpu_flags = av_get_cpu_flags();

    if (bit_depth == 8) {
        if (EXTERNAL_MMXEXT(cpu_flags)) {
            c->idct_dc[0] = ff_hevc_idct_4x4_dc_8_mmxext;

            c->add_residual[0] = ff_hevc_add_residual_4_8_mmxext;
        }
        if (EXTERNAL_SSE2(cpu_flags)) {
            c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_8_sse2;
            c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_8_sse2;
            if (ARCH_X86_64) {
                c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_8_sse2;
                c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_8_sse2;

                c->idct[2] = ff_hevc_idct_16x16_8_sse2;
                c->idct[3] = ff_hevc_idct_32x32_8_sse2;
            }
            SAO_BAND_INIT(8, sse2);

            c->idct_dc[1] = ff_hevc_idct_8x8_dc_8_sse2;
            c->idct_dc[2] = ff_hevc_idct_16x16_dc_8_sse2;
            c->idct_dc[3] = ff_hevc_idct_32x32_dc_8_sse2;

            c->idct[0]    = ff_hevc_idct_4x4_8_sse2;
            c->idct[1]    = ff_hevc_idct_8x8_8_sse2;

            c->add_residual[1] = ff_hevc_add_residual_8_8_sse2;
            c->add_residual[2] = ff_hevc_add_residual_16_8_sse2;
            c->add_residual[3] = ff_hevc_add_residual_32_8_sse2;
        }
        if (EXTERNAL_SSSE3(cpu_flags)) {
            if(ARCH_X86_64) {
                c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_8_ssse3;
                c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_8_ssse3;
            }
            SAO_EDGE_INIT(8, ssse3);
        }
#if HAVE_SSE4_EXTERNAL && ARCH_X86_64
        if (EXTERNAL_SSE4(cpu_flags)) {

            EPEL_LINKS(c->put_hevc_epel, 0, 0, pel_pixels,  8, sse4);
            EPEL_LINKS(c->put_hevc_epel, 0, 1, epel_h,      8, sse4);
            EPEL_LINKS(c->put_hevc_epel, 1, 0, epel_v,      8, sse4);
            EPEL_LINKS(c->put_hevc_epel, 1, 1, epel_hv,     8, sse4);

            QPEL_LINKS(c->put_hevc_qpel, 0, 0, pel_pixels, 8, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 0, 1, qpel_h,     8, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 1, 0, qpel_v,     8, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 1, 1, qpel_hv,    8, sse4);
        }
#endif
        if (EXTERNAL_AVX(cpu_flags)) {
            c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_8_avx;
            c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_8_avx;
            if (ARCH_X86_64) {
                c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_8_avx;
                c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_8_avx;

                c->idct[2] = ff_hevc_idct_16x16_8_avx;
                c->idct[3] = ff_hevc_idct_32x32_8_avx;
            }
            SAO_BAND_INIT(8, avx);

            c->idct[0] = ff_hevc_idct_4x4_8_avx;
            c->idct[1] = ff_hevc_idct_8x8_8_avx;

            c->add_residual[1] = ff_hevc_add_residual_8_8_avx;
            c->add_residual[2] = ff_hevc_add_residual_16_8_avx;
            c->add_residual[3] = ff_hevc_add_residual_32_8_avx;
        }
        if (EXTERNAL_AVX2(cpu_flags)) {
            c->sao_band_filter[0] = ff_hevc_sao_band_filter_8_8_avx2;
            c->sao_band_filter[1] = ff_hevc_sao_band_filter_16_8_avx2;
        }
#if HAVE_AVX2_EXTERNAL
        if (EXTERNAL_AVX2_FAST(cpu_flags)) {
            c->idct_dc[2] = ff_hevc_idct_16x16_dc_8_avx2;
            c->idct_dc[3] = ff_hevc_idct_32x32_dc_8_avx2;

#if ARCH_X86_64
            c->put_hevc_epel[7][0][0] = hevc_put_pel_pixels32_8_avx2;
            c->put_hevc_epel[8][0][0] = hevc_put_pel_pixels48_8_avx2;
            c->put_hevc_epel[9][0][0] = hevc_put_pel_pixels64_8_avx2;

            c->put_hevc_qpel[7][0][0] = hevc_put_pel_pixels32_8_avx2;
            c->put_hevc_qpel[8][0][0] = hevc_put_pel_pixels48_8_avx2;
            c->put_hevc_qpel[9][0][0] = hevc_put_pel_pixels64_8_avx2;

            c->put_hevc_epel_uni[7][0][0] = hevc_put_uni_pel_pixels32_8_avx2;
            c->put_hevc_epel_uni[8][0][0] = hevc_put_uni_pel_pixels48_8_avx2;
            c->put_hevc_epel_uni[9][0][0] = hevc_put_uni_pel_pixels64_8_avx2;

            c->put_hevc_qpel_uni[7][0][0] = hevc_put_uni_pel_pixels32_8_avx2;
            c->put_hevc_qpel_uni[8][0][0] = hevc_put_uni_pel_pixels48_8_avx2;
            c->put_hevc_qpel_uni[9][0][0] = hevc_put_uni_pel_pixels64_8_avx2;

            c->put_hevc_qpel_bi[7][0][0] = ff_hevc_put_bi_pel_pixels32_8_avx2;
            c->put_hevc_qpel_bi[8][0][0] = ff_hevc_put_bi_pel_pixels48_8_avx2;
            c->put_hevc_qpel_bi[9][0][0] = ff_hevc_put_bi_pel_pixels64_8_avx2;

            c->put_hevc_epel_bi[7][0][0] = ff_hevc_put_bi_pel_pixels32_8_avx2;
            c->put_hevc_epel_bi[8][0][0] = ff_hevc_put_bi_pel_pixels48_8_avx2;
            c->put_hevc_epel_bi[9][0][0] = ff_hevc_put_bi_pel_pixels64_8_avx2;

            c->put_hevc_epel[7][0][1] = hevc_put_epel_h32_8_avx2;
            c->put_hevc_epel[8][0][1] = hevc_put_epel_h48_8_avx2;
            c->put_hevc_epel[9][0][1] = hevc_put_epel_h64_8_avx2;

            c->put_hevc_epel_uni[7][0][1] = hevc_put_uni_epel_h32_8_avx2;
            c->put_hevc_epel_uni[8][0][1] = hevc_put_uni_epel_h48_8_avx2;
            c->put_hevc_epel_uni[9][0][1] = hevc_put_uni_epel_h64_8_avx2;

            c->put_hevc_epel_bi[7][0][1] = ff_hevc_put_bi_epel_h32_8_avx2;
            c->put_hevc_epel_bi[8][0][1] = ff_hevc_put_bi_epel_h48_8_avx2;
            c->put_hevc_epel_bi[9][0][1] = ff_hevc_put_bi_epel_h64_8_avx2;

            c->put_hevc_epel[7][1][0] = hevc_put_epel_v32_8_avx2;
            c->put_hevc_epel[8][1][0] = hevc_put_epel_v48_8_avx2;
            c->put_hevc_epel[9][1][0] = hevc_put_epel_v64_8_avx2;

            c->put_hevc_epel_uni[7][1][0] = hevc_put_uni_epel_v32_8_avx2;
            c->put_hevc_epel_uni[8][1][0] = hevc_put_uni_epel_v48_8_avx2;
            c->put_hevc_epel_uni[9][1][0] = hevc_put_uni_epel_v64_8_avx2;

            c->put_hevc_epel_bi[7][1][0] = ff_hevc_put_bi_epel_v32_8_avx2;
            c->put_hevc_epel_bi[8][1][0] = ff_hevc_put_bi_epel_v48_8_avx2;
            c->put_hevc_epel_bi[9][1][0] = ff_hevc_put_bi_epel_v64_8_avx2;

            c->put_hevc_epel[7][1][1] = hevc_put_epel_hv32_8_avx2;
            c->put_hevc_epel[8][1][1] = hevc_put_epel_hv48_8_avx2;
            c->put_hevc_epel[9][1][1] = hevc_put_epel_hv64_8_avx2;

            c->put_hevc_epel_uni[7][1][1] = hevc_put_uni_epel_hv32_8_avx2;
            c->put_hevc_epel_uni[8][1][1] = hevc_put_uni_epel_hv48_8_avx2;
            c->put_hevc_epel_uni[9][1][1] = hevc_put_uni_epel_hv64_8_avx2;

            c->put_hevc_epel_bi[7][1][1] = ff_hevc_put_bi_epel_hv32_8_avx2;
            c->put_hevc_epel_bi[8][1][1] = ff_hevc_put_bi_epel_hv48_8_avx2;
            c->put_hevc_epel_bi[9][1][1] = ff_hevc_put_bi_epel_hv64_8_avx2;

            c->put_hevc_qpel[7][0][1] = hevc_put_qpel_h32_8_avx2;
            c->put_hevc_qpel[8][0][1] = hevc_put_qpel_h48_8_avx2;
            c->put_hevc_qpel[9][0][1] = hevc_put_qpel_h64_8_avx2;

            c->put_hevc_qpel[7][1][0] = hevc_put_qpel_v32_8_avx2;
            c->put_hevc_qpel[8][1][0] = hevc_put_qpel_v48_8_avx2;
            c->put_hevc_qpel[9][1][0] = hevc_put_qpel_v64_8_avx2;

            c->put_hevc_qpel_uni[7][0][1] = hevc_put_uni_qpel_h32_8_avx2;
            c->put_hevc_qpel_uni[8][0][1] = hevc_put_uni_qpel_h48_8_avx2;
            c->put_hevc_qpel_uni[9][0][1] = hevc_put_uni_qpel_h64_8_avx2;

            c->put_hevc_qpel_uni[7][1][0] = hevc_put_uni_qpel_v32_8_avx2;
            c->put_hevc_qpel_uni[8][1][0] = hevc_put_uni_qpel_v48_8_avx2;
            c->put_hevc_qpel_uni[9][1][0] = hevc_put_uni_qpel_v64_8_avx2;

            c->put_hevc_qpel_bi[7][0][1] = ff_hevc_put_bi_qpel_h32_8_avx2;
            c->put_hevc_qpel_bi[8][0][1] = ff_hevc_put_bi_qpel_h48_8_avx2;
            c->put_hevc_qpel_bi[9][0][1] = ff_hevc_put_bi_qpel_h64_8_avx2;

            c->put_hevc_qpel_bi[7][1][0] = ff_hevc_put_bi_qpel_v32_8_avx2;
            c->put_hevc_qpel_bi[8][1][0] = ff_hevc_put_bi_qpel_v48_8_avx2;
            c->put_hevc_qpel_bi[9][1][0] = ff_hevc_put_bi_qpel_v64_8_avx2;
#endif /* ARCH_X86_64 */

            SAO_BAND_INIT(8, avx2);

            c->sao_edge_filter[2] = ff_hevc_sao_edge_filter_32_8_avx2;
            c->sao_edge_filter[3] = ff_hevc_sao_edge_filter_48_8_avx2;
            c->sao_edge_filter[4] = ff_hevc_sao_edge_filter_64_8_avx2;

            c->add_residual[3] = ff_hevc_add_residual_32_8_avx2;
        }
#endif /* HAVE_AVX2_EXTERNAL */
        if (EXTERNAL_AVX512ICL(cpu_flags) && ARCH_X86_64) {
            c->put_hevc_qpel[1][0][1] = ff_hevc_put_qpel_h4_8_avx512icl;
            c->put_hevc_qpel[3][0][1] = ff_hevc_put_qpel_h8_8_avx512icl;
            c->put_hevc_qpel[5][0][1] = ff_hevc_put_qpel_h16_8_avx512icl;
            c->put_hevc_qpel[7][0][1] = ff_hevc_put_qpel_h32_8_avx512icl;
            c->put_hevc_qpel[9][0][1] = ff_hevc_put_qpel_h64_8_avx512icl;
            c->put_hevc_qpel[3][1][1] = ff_hevc_put_qpel_hv8_8_avx512icl;
        }
    } else if (bit_depth == 10) {
        if (EXTERNAL_MMXEXT(cpu_flags)) {
            c->add_residual[0] = ff_hevc_add_residual_4_10_mmxext;
            c->idct_dc[0] = ff_hevc_idct_4x4_dc_10_mmxext;
        }
        if (EXTERNAL_SSE2(cpu_flags)) {
            c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_10_sse2;
            c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_10_sse2;
            if (ARCH_X86_64) {
                c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_10_sse2;
                c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_10_sse2;

                c->idct[2] = ff_hevc_idct_16x16_10_sse2;
                c->idct[3] = ff_hevc_idct_32x32_10_sse2;
            }
            SAO_BAND_INIT(10, sse2);
            SAO_EDGE_INIT(10, sse2);

            c->idct_dc[1] = ff_hevc_idct_8x8_dc_10_sse2;
            c->idct_dc[2] = ff_hevc_idct_16x16_dc_10_sse2;
            c->idct_dc[3] = ff_hevc_idct_32x32_dc_10_sse2;

            c->idct[0]    = ff_hevc_idct_4x4_10_sse2;
            c->idct[1]    = ff_hevc_idct_8x8_10_sse2;

            c->add_residual[1] = ff_hevc_add_residual_8_10_sse2;
            c->add_residual[2] = ff_hevc_add_residual_16_10_sse2;
            c->add_residual[3] = ff_hevc_add_residual_32_10_sse2;
        }
        if (EXTERNAL_SSSE3(cpu_flags) && ARCH_X86_64) {
            c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_10_ssse3;
            c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_10_ssse3;
        }
#if HAVE_SSE4_EXTERNAL && ARCH_X86_64
        if (EXTERNAL_SSE4(cpu_flags)) {
            EPEL_LINKS(c->put_hevc_epel, 0, 0, pel_pixels, 10, sse4);
            EPEL_LINKS(c->put_hevc_epel, 0, 1, epel_h,     10, sse4);
            EPEL_LINKS(c->put_hevc_epel, 1, 0, epel_v,     10, sse4);
            EPEL_LINKS(c->put_hevc_epel, 1, 1, epel_hv,    10, sse4);

            QPEL_LINKS(c->put_hevc_qpel, 0, 0, pel_pixels, 10, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 0, 1, qpel_h,     10, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 1, 0, qpel_v,     10, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 1, 1, qpel_hv,    10, sse4);
        }
#endif
        if (EXTERNAL_AVX(cpu_flags)) {
            c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_10_avx;
            c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_10_avx;
            if (ARCH_X86_64) {
                c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_10_avx;
                c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_10_avx;

                c->idct[2] = ff_hevc_idct_16x16_10_avx;
                c->idct[3] = ff_hevc_idct_32x32_10_avx;
            }

            c->idct[0] = ff_hevc_idct_4x4_10_avx;
            c->idct[1] = ff_hevc_idct_8x8_10_avx;

            SAO_BAND_INIT(10, avx);
        }
        if (EXTERNAL_AVX2(cpu_flags)) {
            c->sao_band_filter[0] = ff_hevc_sao_band_filter_8_10_avx2;
        }
#if HAVE_AVX2_EXTERNAL
        if (EXTERNAL_AVX2_FAST(cpu_flags)) {
            c->idct_dc[2] = ff_hevc_idct_16x16_dc_10_avx2;
            c->idct_dc[3] = ff_hevc_idct_32x32_dc_10_avx2;

#if ARCH_X86_64
            c->put_hevc_epel[5][0][0] = hevc_put_pel_pixels16_10_avx2;
            c->put_hevc_epel[6][0][0] = hevc_put_pel_pixels24_10_avx2;
            c->put_hevc_epel[7][0][0] = hevc_put_pel_pixels32_10_avx2;
            c->put_hevc_epel[8][0][0] = hevc_put_pel_pixels48_10_avx2;
            c->put_hevc_epel[9][0][0] = hevc_put_pel_pixels64_10_avx2;

            c->put_hevc_qpel[5][0][0] = hevc_put_pel_pixels16_10_avx2;
            c->put_hevc_qpel[6][0][0] = hevc_put_pel_pixels24_10_avx2;
            c->put_hevc_qpel[7][0][0] = hevc_put_pel_pixels32_10_avx2;
            c->put_hevc_qpel[8][0][0] = hevc_put_pel_pixels48_10_avx2;
            c->put_hevc_qpel[9][0][0] = hevc_put_pel_pixels64_10_avx2;

            c->put_hevc_epel_uni[5][0][0] = hevc_put_uni_pel_pixels32_8_avx2;
            c->put_hevc_epel_uni[6][0][0] = hevc_put_uni_pel_pixels48_8_avx2;
            c->put_hevc_epel_uni[7][0][0] = hevc_put_uni_pel_pixels64_8_avx2;
            c->put_hevc_epel_uni[8][0][0] = hevc_put_uni_pel_pixels96_8_avx2;
            c->put_hevc_epel_uni[9][0][0] = hevc_put_uni_pel_pixels128_8_avx2;

            c->put_hevc_qpel_uni[5][0][0] = hevc_put_uni_pel_pixels32_8_avx2;
            c->put_hevc_qpel_uni[6][0][0] = hevc_put_uni_pel_pixels48_8_avx2;
            c->put_hevc_qpel_uni[7][0][0] = hevc_put_uni_pel_pixels64_8_avx2;
            c->put_hevc_qpel_uni[8][0][0] = hevc_put_uni_pel_pixels96_8_avx2;
            c->put_hevc_qpel_uni[9][0][0] = hevc_put_uni_pel_pixels128_8_avx2;

            c->put_hevc_epel_bi[5][0][0] = ff_hevc_put_bi_pel_pixels16_10_avx2;
            c->put_hevc_epel_bi[6][0][0] = ff_hevc_put_bi_pel_pixels24_10_avx2;
            c->put_hevc_epel_bi[7][0][0] = ff_hevc_put_bi_pel_pixels32_10_avx2;
            c->put_hevc_epel_bi[8][0][0] = ff_hevc_put_bi_pel_pixels48_10_avx2;
            c->put_hevc_epel_bi[9][0][0] = ff_hevc_put_bi_pel_pixels64_10_avx2;
            c->put_hevc_qpel_bi[5][0][0] = ff_hevc_put_bi_pel_pixels16_10_avx2;
            c->put_hevc_qpel_bi[6][0][0] = ff_hevc_put_bi_pel_pixels24_10_avx2;
            c->put_hevc_qpel_bi[7][0][0] = ff_hevc_put_bi_pel_pixels32_10_avx2;
            c->put_hevc_qpel_bi[8][0][0] = ff_hevc_put_bi_pel_pixels48_10_avx2;
            c->put_hevc_qpel_bi[9][0][0] = ff_hevc_put_bi_pel_pixels64_10_avx2;

            c->put_hevc_epel[5][0][1] = hevc_put_epel_h16_10_avx2;
            c->put_hevc_epel[6][0][1] = hevc_put_epel_h24_10_avx2;
            c->put_hevc_epel[7][0][1] = hevc_put_epel_h32_10_avx2;
            c->put_hevc_epel[8][0][1] = hevc_put_epel_h48_10_avx2;
            c->put_hevc_epel[9][0][1] = hevc_put_epel_h64_10_avx2;

            c->put_hevc_epel_uni[5][0][1] = hevc_put_uni_epel_h16_10_avx2;
            c->put_hevc_epel_uni[6][0][1] = hevc_put_uni_epel_h24_10_avx2;
            c->put_hevc_epel_uni[7][0][1] = hevc_put_uni_epel_h32_10_avx2;
            c->put_hevc_epel_uni[8][0][1] = hevc_put_uni_epel_h48_10_avx2;
            c->put_hevc_epel_uni[9][0][1] = hevc_put_uni_epel_h64_10_avx2;

            c->put_hevc_epel_bi[5][0][1] = ff_hevc_put_bi_epel_h16_10_avx2;
            c->put_hevc_epel_bi[6][0][1] = ff_hevc_put_bi_epel_h24_10_avx2;
            c->put_hevc_epel_bi[7][0][1] = ff_hevc_put_bi_epel_h32_10_avx2;
            c->put_hevc_epel_bi[8][0][1] = ff_hevc_put_bi_epel_h48_10_avx2;
            c->put_hevc_epel_bi[9][0][1] = ff_hevc_put_bi_epel_h64_10_avx2;

            c->put_hevc_epel[5][1][0] = hevc_put_epel_v16_10_avx2;
            c->put_hevc_epel[6][1][0] = hevc_put_epel_v24_10_avx2;
            c->put_hevc_epel[7][1][0] = hevc_put_epel_v32_10_avx2;
            c->put_hevc_epel[8][1][0] = hevc_put_epel_v48_10_avx2;
            c->put_hevc_epel[9][1][0] = hevc_put_epel_v64_10_avx2;

            c->put_hevc_epel_uni[5][1][0] = hevc_put_uni_epel_v16_10_avx2;
            c->put_hevc_epel_uni[6][1][0] = hevc_put_uni_epel_v24_10_avx2;
            c->put_hevc_epel_uni[7][1][0] = hevc_put_uni_epel_v32_10_avx2;
            c->put_hevc_epel_uni[8][1][0] = hevc_put_uni_epel_v48_10_avx2;
            c->put_hevc_epel_uni[9][1][0] = hevc_put_uni_epel_v64_10_avx2;

            c->put_hevc_epel_bi[5][1][0] = ff_hevc_put_bi_epel_v16_10_avx2;
            c->put_hevc_epel_bi[6][1][0] = ff_hevc_put_bi_epel_v24_10_avx2;
            c->put_hevc_epel_bi[7][1][0] = ff_hevc_put_bi_epel_v32_10_avx2;
            c->put_hevc_epel_bi[8][1][0] = ff_hevc_put_bi_epel_v48_10_avx2;
            c->put_hevc_epel_bi[9][1][0] = ff_hevc_put_bi_epel_v64_10_avx2;

            c->put_hevc_epel[5][1][1] = hevc_put_epel_hv16_10_avx2;
            c->put_hevc_epel[6][1][1] = hevc_put_epel_hv24_10_avx2;
            c->put_hevc_epel[7][1][1] = hevc_put_epel_hv32_10_avx2;
            c->put_hevc_epel[8][1][1] = hevc_put_epel_hv48_10_avx2;
            c->put_hevc_epel[9][1][1] = hevc_put_epel_hv64_10_avx2;

            c->put_hevc_epel_uni[5][1][1] = hevc_put_uni_epel_hv16_10_avx2;
            c->put_hevc_epel_uni[6][1][1] = hevc_put_uni_epel_hv24_10_avx2;
            c->put_hevc_epel_uni[7][1][1] = hevc_put_uni_epel_hv32_10_avx2;
            c->put_hevc_epel_uni[8][1][1] = hevc_put_uni_epel_hv48_10_avx2;
            c->put_hevc_epel_uni[9][1][1] = hevc_put_uni_epel_hv64_10_avx2;

            c->put_hevc_epel_bi[5][1][1] = ff_hevc_put_bi_epel_hv16_10_avx2;
            c->put_hevc_epel_bi[6][1][1] = ff_hevc_put_bi_epel_hv24_10_avx2;
            c->put_hevc_epel_bi[7][1][1] = ff_hevc_put_bi_epel_hv32_10_avx2;
            c->put_hevc_epel_bi[8][1][1] = ff_hevc_put_bi_epel_hv48_10_avx2;
            c->put_hevc_epel_bi[9][1][1] = ff_hevc_put_bi_epel_hv64_10_avx2;

            c->put_hevc_qpel[5][0][1] = hevc_put_qpel_h16_10_avx2;
            c->put_hevc_qpel[6][0][1] = hevc_put_qpel_h24_10_avx2;
            c->put_hevc_qpel[7][0][1] = hevc_put_qpel_h32_10_avx2;
            c->put_hevc_qpel[8][0][1] = hevc_put_qpel_h48_10_avx2;
            c->put_hevc_qpel[9][0][1] = hevc_put_qpel_h64_10_avx2;

            c->put_hevc_qpel_uni[5][0][1] = hevc_put_uni_qpel_h16_10_avx2;
            c->put_hevc_qpel_uni[6][0][1] = hevc_put_uni_qpel_h24_10_avx2;
            c->put_hevc_qpel_uni[7][0][1] = hevc_put_uni_qpel_h32_10_avx2;
            c->put_hevc_qpel_uni[8][0][1] = hevc_put_uni_qpel_h48_10_avx2;
            c->put_hevc_qpel_uni[9][0][1] = hevc_put_uni_qpel_h64_10_avx2;

            c->put_hevc_qpel_bi[5][0][1] = ff_hevc_put_bi_qpel_h16_10_avx2;
            c->put_hevc_qpel_bi[6][0][1] = ff_hevc_put_bi_qpel_h24_10_avx2;
            c->put_hevc_qpel_bi[7][0][1] = ff_hevc_put_bi_qpel_h32_10_avx2;
            c->put_hevc_qpel_bi[8][0][1] = ff_hevc_put_bi_qpel_h48_10_avx2;
            c->put_hevc_qpel_bi[9][0][1] = ff_hevc_put_bi_qpel_h64_10_avx2;

            c->put_hevc_qpel[5][1][0] = hevc_put_qpel_v16_10_avx2;
            c->put_hevc_qpel[6][1][0] = hevc_put_qpel_v24_10_avx2;
            c->put_hevc_qpel[7][1][0] = hevc_put_qpel_v32_10_avx2;
            c->put_hevc_qpel[8][1][0] = hevc_put_qpel_v48_10_avx2;
            c->put_hevc_qpel[9][1][0] = hevc_put_qpel_v64_10_avx2;

            c->put_hevc_qpel_uni[5][1][0] = hevc_put_uni_qpel_v16_10_avx2;
            c->put_hevc_qpel_uni[6][1][0] = hevc_put_uni_qpel_v24_10_avx2;
            c->put_hevc_qpel_uni[7][1][0] = hevc_put_uni_qpel_v32_10_avx2;
            c->put_hevc_qpel_uni[8][1][0] = hevc_put_uni_qpel_v48_10_avx2;
            c->put_hevc_qpel_uni[9][1][0] = hevc_put_uni_qpel_v64_10_avx2;

            c->put_hevc_qpel_bi[5][1][0] = ff_hevc_put_bi_qpel_v16_10_avx2;
            c->put_hevc_qpel_bi[6][1][0] = ff_hevc_put_bi_qpel_v24_10_avx2;
            c->put_hevc_qpel_bi[7][1][0] = ff_hevc_put_bi_qpel_v32_10_avx2;
            c->put_hevc_qpel_bi[8][1][0] = ff_hevc_put_bi_qpel_v48_10_avx2;
            c->put_hevc_qpel_bi[9][1][0] = ff_hevc_put_bi_qpel_v64_10_avx2;

            c->put_hevc_qpel[5][1][1] = hevc_put_qpel_hv16_10_avx2;
            c->put_hevc_qpel[6][1][1] = hevc_put_qpel_hv24_10_avx2;
            c->put_hevc_qpel[7][1][1] = hevc_put_qpel_hv32_10_avx2;
            c->put_hevc_qpel[8][1][1] = hevc_put_qpel_hv48_10_avx2;
            c->put_hevc_qpel[9][1][1] = hevc_put_qpel_hv64_10_avx2;

            c->put_hevc_qpel_uni[5][1][1] = hevc_put_uni_qpel_hv16_10_avx2;
            c->put_hevc_qpel_uni[6][1][1] = hevc_put_uni_qpel_hv24_10_avx2;
            c->put_hevc_qpel_uni[7][1][1] = hevc_put_uni_qpel_hv32_10_avx2;
            c->put_hevc_qpel_uni[8][1][1] = hevc_put_uni_qpel_hv48_10_avx2;
            c->put_hevc_qpel_uni[9][1][1] = hevc_put_uni_qpel_hv64_10_avx2;

            c->put_hevc_qpel_bi[5][1][1] = ff_hevc_put_bi_qpel_hv16_10_avx2;
            c->put_hevc_qpel_bi[6][1][1] = ff_hevc_put_bi_qpel_hv24_10_avx2;
            c->put_hevc_qpel_bi[7][1][1] = ff_hevc_put_bi_qpel_hv32_10_avx2;
            c->put_hevc_qpel_bi[8][1][1] = ff_hevc_put_bi_qpel_hv48_10_avx2;
            c->put_hevc_qpel_bi[9][1][1] = ff_hevc_put_bi_qpel_hv64_10_avx2;
#endif /* ARCH_X86_64 */

            SAO_BAND_INIT(10, avx2);
            SAO_EDGE_INIT(10, avx2);

            c->add_residual[2] = ff_hevc_add_residual_16_10_avx2;
            c->add_residual[3] = ff_hevc_add_residual_32_10_avx2;
        }
#endif /* HAVE_AVX2_EXTERNAL */
    } else if (bit_depth == 12) {
        if (EXTERNAL_MMXEXT(cpu_flags)) {
            c->idct_dc[0] = ff_hevc_idct_4x4_dc_12_mmxext;
        }
        if (EXTERNAL_SSE2(cpu_flags)) {
            c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_12_sse2;
            c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_12_sse2;
            if (ARCH_X86_64) {
                c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_12_sse2;
                c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_12_sse2;
            }
            SAO_BAND_INIT(12, sse2);
            SAO_EDGE_INIT(12, sse2);

            c->idct_dc[1] = ff_hevc_idct_8x8_dc_12_sse2;
            c->idct_dc[2] = ff_hevc_idct_16x16_dc_12_sse2;
            c->idct_dc[3] = ff_hevc_idct_32x32_dc_12_sse2;
        }
        if (EXTERNAL_SSSE3(cpu_flags) && ARCH_X86_64) {
            c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_12_ssse3;
            c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_12_ssse3;
        }
#if HAVE_SSE4_EXTERNAL && ARCH_X86_64
        if (EXTERNAL_SSE4(cpu_flags)) {
            EPEL_LINKS(c->put_hevc_epel, 0, 0, pel_pixels, 12, sse4);
            EPEL_LINKS(c->put_hevc_epel, 0, 1, epel_h,     12, sse4);
            EPEL_LINKS(c->put_hevc_epel, 1, 0, epel_v,     12, sse4);
            EPEL_LINKS(c->put_hevc_epel, 1, 1, epel_hv,    12, sse4);

            QPEL_LINKS(c->put_hevc_qpel, 0, 0, pel_pixels, 12, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 0, 1, qpel_h,     12, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 1, 0, qpel_v,     12, sse4);
            QPEL_LINKS(c->put_hevc_qpel, 1, 1, qpel_hv,    12, sse4);
        }
#endif
        if (EXTERNAL_AVX(cpu_flags)) {
            c->hevc_v_loop_filter_chroma = ff_hevc_v_loop_filter_chroma_12_avx;
            c->hevc_h_loop_filter_chroma = ff_hevc_h_loop_filter_chroma_12_avx;
            if (ARCH_X86_64) {
                c->hevc_v_loop_filter_luma = ff_hevc_v_loop_filter_luma_12_avx;
                c->hevc_h_loop_filter_luma = ff_hevc_h_loop_filter_luma_12_avx;
            }
            SAO_BAND_INIT(12, avx);
        }
        if (EXTERNAL_AVX2(cpu_flags)) {
            c->sao_band_filter[0] = ff_hevc_sao_band_filter_8_12_avx2;
        }
        if (EXTERNAL_AVX2_FAST(cpu_flags)) {
            c->idct_dc[2] = ff_hevc_idct_16x16_dc_12_avx2;
            c->idct_dc[3] = ff_hevc_idct_32x32_dc_12_avx2;

            SAO_BAND_INIT(12, avx2);
            SAO_EDGE_INIT(12, avx2);
        }
    }
}
