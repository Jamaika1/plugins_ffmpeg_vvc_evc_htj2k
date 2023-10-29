// This file is generated. Do not edit.
#ifndef AVM_DSP_RTCD_H_
#define AVM_DSP_RTCD_H_

#ifdef RTCD_C
#define RTCD_EXTERN
#else
#define RTCD_EXTERN extern
#endif

/*
 * DSP
 */

#include "avm/aom_integer.h"
#include "avm_dsp/aom_dsp_common.h"
#include "av2/common/blockd.h"
#include "av2/common/enums.h"

#ifdef __cplusplus
extern "C" {
#endif

void aom_highbd_v_predictor_4x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_4x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_4x4 aom_highbd_v_predictor_4x4_sse2
#else
#define aom_highbd_v_predictor_4x4 aom_highbd_v_predictor_4x4_c
#endif
void aom_highbd_v_predictor_4x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_4x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_4x8 aom_highbd_v_predictor_4x8_sse2
#else
#define aom_highbd_v_predictor_4x8 aom_highbd_v_predictor_4x8_c
#endif
void aom_highbd_v_predictor_8x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_8x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_8x4 aom_highbd_v_predictor_8x4_sse2
#else
#define aom_highbd_v_predictor_8x4 aom_highbd_v_predictor_8x4_c
#endif
void aom_highbd_v_predictor_4x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_4x16 aom_highbd_v_predictor_4x16_c
void aom_highbd_v_predictor_16x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_16x4 aom_highbd_v_predictor_16x4_c
void aom_highbd_v_predictor_8x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_8x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_8x8 aom_highbd_v_predictor_8x8_sse2
#else
#define aom_highbd_v_predictor_8x8 aom_highbd_v_predictor_8x8_c
#endif
void aom_highbd_v_predictor_8x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_8x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_8x16 aom_highbd_v_predictor_8x16_sse2
#else
#define aom_highbd_v_predictor_8x16 aom_highbd_v_predictor_8x16_c
#endif
void aom_highbd_v_predictor_16x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_16x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_16x8 aom_highbd_v_predictor_16x8_sse2
#else
#define aom_highbd_v_predictor_16x8 aom_highbd_v_predictor_16x8_c
#endif
void aom_highbd_v_predictor_8x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_8x32 aom_highbd_v_predictor_8x32_c
void aom_highbd_v_predictor_32x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_32x8 aom_highbd_v_predictor_32x8_c
void aom_highbd_v_predictor_4x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_4x32 aom_highbd_v_predictor_4x32_c
void aom_highbd_v_predictor_32x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_32x4 aom_highbd_v_predictor_32x4_c
void aom_highbd_v_predictor_16x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_16x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_16x16 aom_highbd_v_predictor_16x16_sse2
#else
#define aom_highbd_v_predictor_16x16 aom_highbd_v_predictor_16x16_c
#endif
void aom_highbd_v_predictor_16x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_16x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_16x32 aom_highbd_v_predictor_16x32_sse2
#else
#define aom_highbd_v_predictor_16x32 aom_highbd_v_predictor_16x32_c
#endif
void aom_highbd_v_predictor_32x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_32x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_32x16 aom_highbd_v_predictor_32x16_sse2
#else
#define aom_highbd_v_predictor_32x16 aom_highbd_v_predictor_32x16_c
#endif
void aom_highbd_v_predictor_16x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_16x64 aom_highbd_v_predictor_16x64_c
void aom_highbd_v_predictor_64x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_64x16 aom_highbd_v_predictor_64x16_c
void aom_highbd_v_predictor_8x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_8x64 aom_highbd_v_predictor_8x64_c
void aom_highbd_v_predictor_64x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_64x8 aom_highbd_v_predictor_64x8_c
void aom_highbd_v_predictor_4x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_4x64 aom_highbd_v_predictor_4x64_c
void aom_highbd_v_predictor_64x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_64x4 aom_highbd_v_predictor_64x4_c
void aom_highbd_v_predictor_32x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_v_predictor_32x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_32x32 aom_highbd_v_predictor_32x32_sse2
#else
#define aom_highbd_v_predictor_32x32 aom_highbd_v_predictor_32x32_c
#endif
void aom_highbd_v_predictor_32x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_32x64 aom_highbd_v_predictor_32x64_c
void aom_highbd_v_predictor_64x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_64x32 aom_highbd_v_predictor_64x32_c
void aom_highbd_v_predictor_64x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_v_predictor_64x64 aom_highbd_v_predictor_64x64_c

void aom_highbd_dc_predictor_4x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_4x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_4x4 aom_highbd_dc_predictor_4x4_sse2
#else
#define aom_highbd_dc_predictor_4x4 aom_highbd_dc_predictor_4x4_c
#endif
void aom_highbd_dc_predictor_4x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_4x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_4x8 aom_highbd_dc_predictor_4x8_sse2
#else
#define aom_highbd_dc_predictor_4x8 aom_highbd_dc_predictor_4x8_c
#endif
void aom_highbd_dc_predictor_8x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_8x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_8x4 aom_highbd_dc_predictor_8x4_sse2
#else
#define aom_highbd_dc_predictor_8x4 aom_highbd_dc_predictor_8x4_c
#endif
void aom_highbd_dc_predictor_4x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_4x16 aom_highbd_dc_predictor_4x16_c
void aom_highbd_dc_predictor_16x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_16x4 aom_highbd_dc_predictor_16x4_c
void aom_highbd_dc_predictor_4x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_4x32 aom_highbd_dc_predictor_4x32_c
void aom_highbd_dc_predictor_32x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_32x4 aom_highbd_dc_predictor_32x4_c
void aom_highbd_dc_predictor_4x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_4x64 aom_highbd_dc_predictor_4x64_c
void aom_highbd_dc_predictor_64x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_64x4 aom_highbd_dc_predictor_64x4_c
void aom_highbd_dc_predictor_8x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_8x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_8x8 aom_highbd_dc_predictor_8x8_sse2
#else
#define aom_highbd_dc_predictor_8x8 aom_highbd_dc_predictor_8x8_c
#endif
void aom_highbd_dc_predictor_8x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_8x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_8x16 aom_highbd_dc_predictor_8x16_sse2
#else
#define aom_highbd_dc_predictor_8x16 aom_highbd_dc_predictor_8x16_c
#endif
void aom_highbd_dc_predictor_16x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_16x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_16x8 aom_highbd_dc_predictor_16x8_sse2
#else
#define aom_highbd_dc_predictor_16x8 aom_highbd_dc_predictor_16x8_c
#endif
void aom_highbd_dc_predictor_8x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_8x32 aom_highbd_dc_predictor_8x32_c
void aom_highbd_dc_predictor_32x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_32x8 aom_highbd_dc_predictor_32x8_c
void aom_highbd_dc_predictor_8x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_8x64 aom_highbd_dc_predictor_8x64_c
void aom_highbd_dc_predictor_64x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_64x8 aom_highbd_dc_predictor_64x8_c
void aom_highbd_dc_predictor_16x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_16x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_16x16 aom_highbd_dc_predictor_16x16_sse2
#else
#define aom_highbd_dc_predictor_16x16 aom_highbd_dc_predictor_16x16_c
#endif
void aom_highbd_dc_predictor_16x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_16x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_16x32 aom_highbd_dc_predictor_16x32_sse2
#else
#define aom_highbd_dc_predictor_16x32 aom_highbd_dc_predictor_16x32_c
#endif
void aom_highbd_dc_predictor_32x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_32x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_32x16 aom_highbd_dc_predictor_32x16_sse2
#else
#define aom_highbd_dc_predictor_32x16 aom_highbd_dc_predictor_32x16_c
#endif
void aom_highbd_dc_predictor_16x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_16x64 aom_highbd_dc_predictor_16x64_c
void aom_highbd_dc_predictor_64x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_64x16 aom_highbd_dc_predictor_64x16_c
void aom_highbd_dc_predictor_32x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_predictor_32x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_32x32 aom_highbd_dc_predictor_32x32_sse2
#else
#define aom_highbd_dc_predictor_32x32 aom_highbd_dc_predictor_32x32_c
#endif
void aom_highbd_dc_predictor_32x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_32x64 aom_highbd_dc_predictor_32x64_c
void aom_highbd_dc_predictor_64x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_64x32 aom_highbd_dc_predictor_64x32_c
void aom_highbd_dc_predictor_64x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_predictor_64x64 aom_highbd_dc_predictor_64x64_c

void aom_highbd_h_predictor_4x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_4x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_4x4 aom_highbd_h_predictor_4x4_sse2
#else
#define aom_highbd_h_predictor_4x4 aom_highbd_h_predictor_4x4_c
#endif
void aom_highbd_h_predictor_4x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_4x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_4x8 aom_highbd_h_predictor_4x8_sse2
#else
#define aom_highbd_h_predictor_4x8 aom_highbd_h_predictor_4x8_c
#endif
void aom_highbd_h_predictor_8x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_8x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_8x4 aom_highbd_h_predictor_8x4_sse2
#else
#define aom_highbd_h_predictor_8x4 aom_highbd_h_predictor_8x4_c
#endif
void aom_highbd_h_predictor_4x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_4x16 aom_highbd_h_predictor_4x16_c
void aom_highbd_h_predictor_16x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_16x4 aom_highbd_h_predictor_16x4_c
void aom_highbd_h_predictor_8x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_8x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_8x8 aom_highbd_h_predictor_8x8_sse2
#else
#define aom_highbd_h_predictor_8x8 aom_highbd_h_predictor_8x8_c
#endif
void aom_highbd_h_predictor_8x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_8x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_8x16 aom_highbd_h_predictor_8x16_sse2
#else
#define aom_highbd_h_predictor_8x16 aom_highbd_h_predictor_8x16_c
#endif
void aom_highbd_h_predictor_16x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_16x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_16x8 aom_highbd_h_predictor_16x8_sse2
#else
#define aom_highbd_h_predictor_16x8 aom_highbd_h_predictor_16x8_c
#endif
void aom_highbd_h_predictor_8x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_8x32 aom_highbd_h_predictor_8x32_c
void aom_highbd_h_predictor_32x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_32x8 aom_highbd_h_predictor_32x8_c
void aom_highbd_h_predictor_4x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_4x32 aom_highbd_h_predictor_4x32_c
void aom_highbd_h_predictor_32x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_32x4 aom_highbd_h_predictor_32x4_c
void aom_highbd_h_predictor_16x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_16x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_16x16 aom_highbd_h_predictor_16x16_sse2
#else
#define aom_highbd_h_predictor_16x16 aom_highbd_h_predictor_16x16_c
#endif
void aom_highbd_h_predictor_16x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_16x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_16x32 aom_highbd_h_predictor_16x32_sse2
#else
#define aom_highbd_h_predictor_16x32 aom_highbd_h_predictor_16x32_c
#endif
void aom_highbd_h_predictor_32x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_32x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_32x16 aom_highbd_h_predictor_32x16_sse2
#else
#define aom_highbd_h_predictor_32x16 aom_highbd_h_predictor_32x16_c
#endif
void aom_highbd_h_predictor_16x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_16x64 aom_highbd_h_predictor_16x64_c
void aom_highbd_h_predictor_64x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_64x16 aom_highbd_h_predictor_64x16_c
void aom_highbd_h_predictor_8x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_8x64 aom_highbd_h_predictor_8x64_c
void aom_highbd_h_predictor_64x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_64x8 aom_highbd_h_predictor_64x8_c
void aom_highbd_h_predictor_4x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_4x64 aom_highbd_h_predictor_4x64_c
void aom_highbd_h_predictor_64x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_64x4 aom_highbd_h_predictor_64x4_c
void aom_highbd_h_predictor_32x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_h_predictor_32x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_32x32 aom_highbd_h_predictor_32x32_sse2
#else
#define aom_highbd_h_predictor_32x32 aom_highbd_h_predictor_32x32_c
#endif
void aom_highbd_h_predictor_32x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_32x64 aom_highbd_h_predictor_32x64_c
void aom_highbd_h_predictor_64x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_64x32 aom_highbd_h_predictor_64x32_c
void aom_highbd_h_predictor_64x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_h_predictor_64x64 aom_highbd_h_predictor_64x64_c

void aom_highbd_dc_left_predictor_4x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_4x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_4x4 aom_highbd_dc_left_predictor_4x4_sse2
#else
#define aom_highbd_dc_left_predictor_4x4 aom_highbd_dc_left_predictor_4x4_c
#endif
void aom_highbd_dc_left_predictor_4x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_4x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_4x8 aom_highbd_dc_left_predictor_4x8_sse2
#else
#define aom_highbd_dc_left_predictor_4x8 aom_highbd_dc_left_predictor_4x8_c
#endif
void aom_highbd_dc_left_predictor_8x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_8x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_8x4 aom_highbd_dc_left_predictor_8x4_sse2
#else
#define aom_highbd_dc_left_predictor_8x4 aom_highbd_dc_left_predictor_8x4_c
#endif
void aom_highbd_dc_left_predictor_4x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_4x16 aom_highbd_dc_left_predictor_4x16_c
void aom_highbd_dc_left_predictor_16x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_16x4 aom_highbd_dc_left_predictor_16x4_c
void aom_highbd_dc_left_predictor_4x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_4x32 aom_highbd_dc_left_predictor_4x32_c
void aom_highbd_dc_left_predictor_32x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_32x4 aom_highbd_dc_left_predictor_32x4_c
void aom_highbd_dc_left_predictor_4x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_4x64 aom_highbd_dc_left_predictor_4x64_c
void aom_highbd_dc_left_predictor_64x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_64x4 aom_highbd_dc_left_predictor_64x4_c
void aom_highbd_dc_left_predictor_8x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_8x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_8x8 aom_highbd_dc_left_predictor_8x8_sse2
#else
#define aom_highbd_dc_left_predictor_8x8 aom_highbd_dc_left_predictor_8x8_c
#endif
void aom_highbd_dc_left_predictor_8x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_8x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_8x16 aom_highbd_dc_left_predictor_8x16_sse2
#else
#define aom_highbd_dc_left_predictor_8x16 aom_highbd_dc_left_predictor_8x16_c
#endif
void aom_highbd_dc_left_predictor_16x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_16x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_16x8 aom_highbd_dc_left_predictor_16x8_sse2
#else
#define aom_highbd_dc_left_predictor_16x8 aom_highbd_dc_left_predictor_16x8_c
#endif
void aom_highbd_dc_left_predictor_8x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_8x32 aom_highbd_dc_left_predictor_8x32_c
void aom_highbd_dc_left_predictor_32x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_32x8 aom_highbd_dc_left_predictor_32x8_c
void aom_highbd_dc_left_predictor_8x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_8x64 aom_highbd_dc_left_predictor_8x64_c
void aom_highbd_dc_left_predictor_64x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_64x8 aom_highbd_dc_left_predictor_64x8_c
void aom_highbd_dc_left_predictor_16x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_16x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_16x16 aom_highbd_dc_left_predictor_16x16_sse2
#else
#define aom_highbd_dc_left_predictor_16x16 aom_highbd_dc_left_predictor_16x16_c
#endif
void aom_highbd_dc_left_predictor_16x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_16x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_16x32 aom_highbd_dc_left_predictor_16x32_sse2
#else
#define aom_highbd_dc_left_predictor_16x32 aom_highbd_dc_left_predictor_16x32_c
#endif
void aom_highbd_dc_left_predictor_32x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_32x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_32x16 aom_highbd_dc_left_predictor_32x16_sse2
#else
#define aom_highbd_dc_left_predictor_32x16 aom_highbd_dc_left_predictor_32x16_c
#endif
void aom_highbd_dc_left_predictor_16x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_16x64 aom_highbd_dc_left_predictor_16x64_c
void aom_highbd_dc_left_predictor_64x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_64x16 aom_highbd_dc_left_predictor_64x16_c
void aom_highbd_dc_left_predictor_32x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_left_predictor_32x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_32x32 aom_highbd_dc_left_predictor_32x32_sse2
#else
#define aom_highbd_dc_left_predictor_32x32 aom_highbd_dc_left_predictor_32x32_c
#endif
void aom_highbd_dc_left_predictor_32x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_32x64 aom_highbd_dc_left_predictor_32x64_c
void aom_highbd_dc_left_predictor_64x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_64x32 aom_highbd_dc_left_predictor_64x32_c
void aom_highbd_dc_left_predictor_64x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_left_predictor_64x64 aom_highbd_dc_left_predictor_64x64_c

void aom_highbd_dc_top_predictor_4x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_4x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_4x4 aom_highbd_dc_top_predictor_4x4_sse2
#else
#define aom_highbd_dc_top_predictor_4x4 aom_highbd_dc_top_predictor_4x4_c
#endif
void aom_highbd_dc_top_predictor_4x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_4x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_4x8 aom_highbd_dc_top_predictor_4x8_sse2
#else
#define aom_highbd_dc_top_predictor_4x8 aom_highbd_dc_top_predictor_4x8_c
#endif
void aom_highbd_dc_top_predictor_8x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_8x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_8x4 aom_highbd_dc_top_predictor_8x4_sse2
#else
#define aom_highbd_dc_top_predictor_8x4 aom_highbd_dc_top_predictor_8x4_c
#endif
void aom_highbd_dc_top_predictor_4x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_4x16 aom_highbd_dc_top_predictor_4x16_c
void aom_highbd_dc_top_predictor_16x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_16x4 aom_highbd_dc_top_predictor_16x4_c
void aom_highbd_dc_top_predictor_4x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_4x32 aom_highbd_dc_top_predictor_4x32_c
void aom_highbd_dc_top_predictor_32x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_32x4 aom_highbd_dc_top_predictor_32x4_c
void aom_highbd_dc_top_predictor_4x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_4x64 aom_highbd_dc_top_predictor_4x64_c
void aom_highbd_dc_top_predictor_64x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_64x4 aom_highbd_dc_top_predictor_64x4_c
void aom_highbd_dc_top_predictor_8x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_8x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_8x8 aom_highbd_dc_top_predictor_8x8_sse2
#else
#define aom_highbd_dc_top_predictor_8x8 aom_highbd_dc_top_predictor_8x8_c
#endif
void aom_highbd_dc_top_predictor_8x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_8x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_8x16 aom_highbd_dc_top_predictor_8x16_sse2
#else
#define aom_highbd_dc_top_predictor_8x16 aom_highbd_dc_top_predictor_8x16_c
#endif
void aom_highbd_dc_top_predictor_16x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_16x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_16x8 aom_highbd_dc_top_predictor_16x8_sse2
#else
#define aom_highbd_dc_top_predictor_16x8 aom_highbd_dc_top_predictor_16x8_c
#endif
void aom_highbd_dc_top_predictor_8x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_8x32 aom_highbd_dc_top_predictor_8x32_c
void aom_highbd_dc_top_predictor_32x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_32x8 aom_highbd_dc_top_predictor_32x8_c
void aom_highbd_dc_top_predictor_8x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_8x64 aom_highbd_dc_top_predictor_8x64_c
void aom_highbd_dc_top_predictor_64x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_64x8 aom_highbd_dc_top_predictor_64x8_c
void aom_highbd_dc_top_predictor_16x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_16x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_16x16 aom_highbd_dc_top_predictor_16x16_sse2
#else
#define aom_highbd_dc_top_predictor_16x16 aom_highbd_dc_top_predictor_16x16_c
#endif
void aom_highbd_dc_top_predictor_16x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_16x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_16x32 aom_highbd_dc_top_predictor_16x32_sse2
#else
#define aom_highbd_dc_top_predictor_16x32 aom_highbd_dc_top_predictor_16x32_c
#endif
void aom_highbd_dc_top_predictor_32x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_32x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_32x16 aom_highbd_dc_top_predictor_32x16_sse2
#else
#define aom_highbd_dc_top_predictor_32x16 aom_highbd_dc_top_predictor_32x16_c
#endif
void aom_highbd_dc_top_predictor_16x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_16x64 aom_highbd_dc_top_predictor_16x64_c
void aom_highbd_dc_top_predictor_64x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_64x16 aom_highbd_dc_top_predictor_64x16_c
void aom_highbd_dc_top_predictor_32x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_top_predictor_32x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_32x32 aom_highbd_dc_top_predictor_32x32_sse2
#else
#define aom_highbd_dc_top_predictor_32x32 aom_highbd_dc_top_predictor_32x32_c
#endif
void aom_highbd_dc_top_predictor_32x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_32x64 aom_highbd_dc_top_predictor_32x64_c
void aom_highbd_dc_top_predictor_64x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_64x32 aom_highbd_dc_top_predictor_64x32_c
void aom_highbd_dc_top_predictor_64x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_top_predictor_64x64 aom_highbd_dc_top_predictor_64x64_c

void aom_highbd_dc_128_predictor_4x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_4x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_4x4 aom_highbd_dc_128_predictor_4x4_sse2
#else
#define aom_highbd_dc_128_predictor_4x4 aom_highbd_dc_128_predictor_4x4_c
#endif
void aom_highbd_dc_128_predictor_4x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_4x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_4x8 aom_highbd_dc_128_predictor_4x8_sse2
#else
#define aom_highbd_dc_128_predictor_4x8 aom_highbd_dc_128_predictor_4x8_c
#endif
void aom_highbd_dc_128_predictor_8x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_8x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_8x4 aom_highbd_dc_128_predictor_8x4_sse2
#else
#define aom_highbd_dc_128_predictor_8x4 aom_highbd_dc_128_predictor_8x4_c
#endif
void aom_highbd_dc_128_predictor_4x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_4x16 aom_highbd_dc_128_predictor_4x16_c
void aom_highbd_dc_128_predictor_16x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_16x4 aom_highbd_dc_128_predictor_16x4_c
void aom_highbd_dc_128_predictor_4x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_4x32 aom_highbd_dc_128_predictor_4x32_c
void aom_highbd_dc_128_predictor_32x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_32x4 aom_highbd_dc_128_predictor_32x4_c
void aom_highbd_dc_128_predictor_8x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_8x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_8x8 aom_highbd_dc_128_predictor_8x8_sse2
#else
#define aom_highbd_dc_128_predictor_8x8 aom_highbd_dc_128_predictor_8x8_c
#endif
void aom_highbd_dc_128_predictor_8x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_8x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_8x16 aom_highbd_dc_128_predictor_8x16_sse2
#else
#define aom_highbd_dc_128_predictor_8x16 aom_highbd_dc_128_predictor_8x16_c
#endif
void aom_highbd_dc_128_predictor_16x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_16x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_16x8 aom_highbd_dc_128_predictor_16x8_sse2
#else
#define aom_highbd_dc_128_predictor_16x8 aom_highbd_dc_128_predictor_16x8_c
#endif
void aom_highbd_dc_128_predictor_4x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#if HAVE_SIMD
//void aom_highbd_dc_128_predictor_4x64_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#define aom_highbd_dc_128_predictor_4x64 aom_highbd_dc_128_predictor_4x64_sse2
//#else
#define aom_highbd_dc_128_predictor_4x64 aom_highbd_dc_128_predictor_4x64_c
//#endif
void aom_highbd_dc_128_predictor_64x4_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#if HAVE_SIMD
//void aom_highbd_dc_128_predictor_64x4_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#define aom_highbd_dc_128_predictor_64x4 aom_highbd_dc_128_predictor_64x4_sse2
//#else
#define aom_highbd_dc_128_predictor_64x4 aom_highbd_dc_128_predictor_64x4_c
//#endif
void aom_highbd_dc_128_predictor_8x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#if HAVE_SIMD
//void aom_highbd_dc_128_predictor_8x64_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#define aom_highbd_dc_128_predictor_8x64 aom_highbd_dc_128_predictor_8x64_sse2
//#else
#define aom_highbd_dc_128_predictor_8x64 aom_highbd_dc_128_predictor_8x64_c
//#endif
void aom_highbd_dc_128_predictor_64x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#if HAVE_SIMD
//void aom_highbd_dc_128_predictor_64x8_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
//#define aom_highbd_dc_128_predictor_64x8 aom_highbd_dc_128_predictor_64x8_sse2
//#else
#define aom_highbd_dc_128_predictor_64x8 aom_highbd_dc_128_predictor_64x8_c
//#endif
void aom_highbd_dc_128_predictor_8x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_8x32 aom_highbd_dc_128_predictor_8x32_c
void aom_highbd_dc_128_predictor_32x8_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_32x8 aom_highbd_dc_128_predictor_32x8_c
void aom_highbd_dc_128_predictor_16x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_16x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_16x16 aom_highbd_dc_128_predictor_16x16_sse2
#else
#define aom_highbd_dc_128_predictor_16x16 aom_highbd_dc_128_predictor_16x16_c
#endif
void aom_highbd_dc_128_predictor_16x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_16x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_16x32 aom_highbd_dc_128_predictor_16x32_sse2
#else
#define aom_highbd_dc_128_predictor_16x32 aom_highbd_dc_128_predictor_16x32_c
#endif
void aom_highbd_dc_128_predictor_32x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_32x16_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_32x16 aom_highbd_dc_128_predictor_32x16_sse2
#else
#define aom_highbd_dc_128_predictor_32x16 aom_highbd_dc_128_predictor_32x16_c
#endif
void aom_highbd_dc_128_predictor_16x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_16x64 aom_highbd_dc_128_predictor_16x64_c
void aom_highbd_dc_128_predictor_64x16_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_64x16 aom_highbd_dc_128_predictor_64x16_c
void aom_highbd_dc_128_predictor_32x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#if HAVE_SIMD
void aom_highbd_dc_128_predictor_32x32_sse2(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_32x32 aom_highbd_dc_128_predictor_32x32_sse2
#else
#define aom_highbd_dc_128_predictor_32x32 aom_highbd_dc_128_predictor_32x32_c
#endif
void aom_highbd_dc_128_predictor_32x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_32x64 aom_highbd_dc_128_predictor_32x64_c
void aom_highbd_dc_128_predictor_64x32_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_64x32 aom_highbd_dc_128_predictor_64x32_c
void aom_highbd_dc_128_predictor_64x64_c(uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd);
#define aom_highbd_dc_128_predictor_64x64 aom_highbd_dc_128_predictor_64x64_c

// Sub Pixel Filters
void aom_highbd_convolve8_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#define aom_highbd_convolve8 aom_highbd_convolve8_c
void aom_convolve_copy_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, int w, int h);
#if HAVE_SIMD
RTCD_EXTERN void (*aom_convolve_copy)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, int w, int h);
void aom_convolve_copy_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, int w, int h);
#if defined(__AVX2__)
void aom_convolve_copy_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, int w, int h);
#endif
#else
#define aom_convolve_copy aom_convolve_copy_c
#endif
void aom_highbd_convolve_copy_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, int w, int h);
#if HAVE_SIMD
RTCD_EXTERN void (*aom_highbd_convolve_copy)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, int w, int h);
void aom_highbd_convolve_copy_sse2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, int w, int h);
#if defined(__AVX2__)
void aom_highbd_convolve_copy_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, int w, int h);
#endif
#else
#define aom_highbd_convolve_copy aom_highbd_convolve_copy_c
#endif
void aom_highbd_convolve8_horiz_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
#if HAVE_SIMD
RTCD_EXTERN void (*aom_highbd_convolve8_horiz)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
void aom_highbd_convolve8_horiz_sse2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
#if defined(__AVX2__)
void aom_highbd_convolve8_horiz_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
#endif
#else
#define aom_highbd_convolve8_horiz aom_highbd_convolve8_horiz_c
#endif
void aom_highbd_convolve8_vert_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
#if HAVE_SIMD
RTCD_EXTERN void (*aom_highbd_convolve8_vert)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
void aom_highbd_convolve8_vert_sse2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
#if defined(__AVX2__)
void aom_highbd_convolve8_vert_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd);
#endif
#else
#define aom_highbd_convolve8_vert aom_highbd_convolve8_vert_c
#endif

// Loopfilter
void aom_highbd_lpf_horizontal_generic_c(uint16_t *s, int pitch, int filt_width, const uint16_t *q_thresh, const uint16_t *side_thresh, int bd);
#define aom_highbd_lpf_horizontal_generic aom_highbd_lpf_horizontal_generic_c
void aom_highbd_lpf_vertical_generic_c(uint16_t *s, int pitch, int filt_width, const uint16_t *q_thresh, const uint16_t *side_thresh, int bd);
#define aom_highbd_lpf_vertical_generic aom_highbd_lpf_vertical_generic_c

// Encoder functions.
//#if CONFIG_AV1_ENCODER
    // Forward transform
    void aom_fdct4x4_c(const int16_t *input, tran_low_t *output, int stride);
#if HAVE_SIMD
    void aom_fdct4x4_sse2(const int16_t *input, tran_low_t *output, int stride);
    #define aom_fdct4x4 aom_fdct4x4_sse2
#else
    #define aom_fdct4x4 aom_fdct4x4_c
#endif
    void aom_fdct4x4_lp_c(const int16_t *input, int16_t *output, int stride);
#if HAVE_SIMD
    void aom_fdct4x4_lp_sse2(const int16_t *input, int16_t *output, int stride);
    #define aom_fdct4x4_lp aom_fdct4x4_lp_sse2
#else
    #define aom_fdct4x4_lp aom_fdct4x4_lp_c
#endif
    void aom_highbd_fdct8x8_c(const int16_t *input, tran_low_t *output, int stride);
#if HAVE_SIMD
    void aom_highbd_fdct8x8_sse2(const int16_t *input, tran_low_t *output, int stride);
    #define aom_highbd_fdct8x8 aom_highbd_fdct8x8_sse2
#else
    #define aom_highbd_fdct8x8 aom_highbd_fdct8x8_c
#endif

    // FFT/IFFT (float) only used for denoising (and noise power spectral density estimation)
    void aom_fft2x2_float_c(const float *input, float *temp, float *output);
    #define aom_fft2x2_float aom_fft2x2_float_c
    void aom_fft4x4_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    void aom_fft4x4_float_sse2(const float *input, float *temp, float *output);
    #define aom_fft4x4_float aom_fft4x4_float_sse2
#else
    #define aom_fft4x4_float aom_fft4x4_float_c
#endif
    void aom_fft8x8_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    RTCD_EXTERN void (*aom_fft8x8_float)(const float *input, float *temp, float *output);
    void aom_fft8x8_float_sse2(const float *input, float *temp, float *output);
#if defined(__AVX2__)
    void aom_fft8x8_float_avx2(const float *input, float *temp, float *output);
#endif
#else
    #define aom_fft8x8_float aom_fft8x8_float_c
#endif
    void aom_fft16x16_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    RTCD_EXTERN void (*aom_fft16x16_float)(const float *input, float *temp, float *output);
    void aom_fft16x16_float_sse2(const float *input, float *temp, float *output);
#if defined(__AVX2__)
    void aom_fft16x16_float_avx2(const float *input, float *temp, float *output);
#endif
#else
    #define aom_fft16x16_float aom_fft16x16_float_c
#endif
    void aom_fft32x32_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    RTCD_EXTERN void (*aom_fft32x32_float)(const float *input, float *temp, float *output);
    void aom_fft32x32_float_sse2(const float *input, float *temp, float *output);
#if defined(__AVX2__)
    void aom_fft32x32_float_avx2(const float *input, float *temp, float *output);
#endif
#else
    #define aom_fft32x32_float aom_fft32x32_float_c
#endif

    void aom_ifft2x2_float_c(const float *input, float *temp, float *output);
    #define aom_ifft2x2_float aom_ifft2x2_float_c
    void aom_ifft4x4_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    void aom_ifft4x4_float_sse2(const float *input, float *temp, float *output);
    #define aom_ifft4x4_float aom_ifft4x4_float_sse2
#else
    #define aom_ifft4x4_float aom_ifft4x4_float_c
#endif
    void aom_ifft8x8_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    RTCD_EXTERN void (*aom_ifft8x8_float)(const float *input, float *temp, float *output);
    void aom_ifft8x8_float_sse2(const float *input, float *temp, float *output);
#if defined(__AVX2__)
    void aom_ifft8x8_float_avx2(const float *input, float *temp, float *output);
#endif
#else
    #define aom_ifft8x8_float aom_ifft8x8_float_c
#endif
    void aom_ifft16x16_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    RTCD_EXTERN void (*aom_ifft16x16_float)(const float *input, float *temp, float *output);
    void aom_ifft16x16_float_sse2(const float *input, float *temp, float *output);
#if defined(__AVX2__)
    void aom_ifft16x16_float_avx2(const float *input, float *temp, float *output);
#endif
#else
    #define aom_ifft16x16_float aom_ifft16x16_float_c
#endif
    void aom_ifft32x32_float_c(const float *input, float *temp, float *output);
#if HAVE_SIMD
    RTCD_EXTERN void (*aom_ifft32x32_float)(const float *input, float *temp, float *output);
    void aom_ifft32x32_float_sse2(const float *input, float *temp, float *output);
#if defined(__AVX2__)
    void aom_ifft32x32_float_avx2(const float *input, float *temp, float *output);
#endif
#else
    #define aom_ifft32x32_float aom_ifft32x32_float_c
#endif

    // Quantization
    void aom_highbd_quantize_b_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
#if HAVE_SIMD
    void aom_highbd_quantize_b_sse2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
    #define aom_highbd_quantize_b aom_highbd_quantize_b_sse2
#else
    #define aom_highbd_quantize_b aom_highbd_quantize_b_c
#endif
    void aom_highbd_quantize_b_adaptive_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
    #define aom_highbd_quantize_b_adaptive aom_highbd_quantize_b_adaptive_c
    void aom_highbd_quantize_b_32x32_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
#if HAVE_SIMD
    void aom_highbd_quantize_b_32x32_sse2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
    #define aom_highbd_quantize_b_32x32 aom_highbd_quantize_b_32x32_sse2
#else
    #define aom_highbd_quantize_b_32x32 aom_highbd_quantize_b_32x32_c
#endif
    void aom_highbd_quantize_b_32x32_adaptive_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
    #define aom_highbd_quantize_b_32x32_adaptive aom_highbd_quantize_b_32x32_adaptive_c
    void aom_highbd_quantize_b_64x64_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
#if HAVE_SIMD
    void aom_highbd_quantize_b_64x64_sse2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
    #define aom_highbd_quantize_b_64x64 aom_highbd_quantize_b_64x64_sse2
#else
    #define aom_highbd_quantize_b_64x64 aom_highbd_quantize_b_64x64_c
#endif
    void aom_highbd_quantize_b_64x64_adaptive_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
    #define aom_highbd_quantize_b_64x64_adaptive aom_highbd_quantize_b_64x64_adaptive_c
//#endif

// Alpha blending with mask
void aom_highbd_blend_a64_mask_c(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
RTCD_EXTERN void (*aom_highbd_blend_a64_mask)(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, int bd);
void aom_highbd_blend_a64_mask_sse4_1(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, int bd);
#else
#define aom_highbd_blend_a64_mask aom_highbd_blend_a64_mask_c
#endif
void aom_highbd_blend_a64_hmask_c(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
RTCD_EXTERN void (*aom_highbd_blend_a64_hmask)(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h, int bd);
void aom_highbd_blend_a64_hmask_sse4_1(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h, int bd);
#else
#define aom_highbd_blend_a64_hmask aom_highbd_blend_a64_hmask_c
#endif
void aom_highbd_blend_a64_vmask_c(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
RTCD_EXTERN void (*aom_highbd_blend_a64_vmask)(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h, int bd);
void aom_highbd_blend_a64_vmask_sse4_1(uint16_t *dst, uint32_t dst_stride, const uint16_t *src0, uint32_t src0_stride, const uint16_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h, int bd);
#else
#define aom_highbd_blend_a64_vmask aom_highbd_blend_a64_vmask_c
#endif
void aom_highbd_blend_a64_d16_mask_c(uint16_t *dst, uint32_t dst_stride, const CONV_BUF_TYPE *src0, uint32_t src0_stride, const CONV_BUF_TYPE *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, ConvolveParams *conv_params, const int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*aom_highbd_blend_a64_d16_mask)(uint16_t *dst, uint32_t dst_stride, const CONV_BUF_TYPE *src0, uint32_t src0_stride, const CONV_BUF_TYPE *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, ConvolveParams *conv_params, const int bd);
void aom_highbd_blend_a64_d16_mask_sse4_1(uint16_t *dst, uint32_t dst_stride, const CONV_BUF_TYPE *src0, uint32_t src0_stride, const CONV_BUF_TYPE *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, ConvolveParams *conv_params, const int bd);
#endif
#if defined(__AVX2__)
void aom_highbd_blend_a64_d16_mask_avx2(uint16_t *dst, uint32_t dst_stride, const CONV_BUF_TYPE *src0, uint32_t src0_stride, const CONV_BUF_TYPE *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, ConvolveParams *conv_params, const int bd);
#endif
#else
#define aom_highbd_blend_a64_d16_mask aom_highbd_blend_a64_d16_mask_c
#endif

// Block subtraction
void aom_highbd_subtract_block_c(int rows, int cols, int16_t *diff_ptr, ptrdiff_t diff_stride, const uint16_t *src_ptr, ptrdiff_t src_stride, const uint16_t *pred_ptr, ptrdiff_t pred_stride, int bd);
#if HAVE_SIMD
void aom_highbd_subtract_block_sse2(int rows, int cols, int16_t *diff_ptr, ptrdiff_t diff_stride, const uint16_t *src_ptr, ptrdiff_t src_stride, const uint16_t *pred_ptr, ptrdiff_t pred_stride, int bd);
#define aom_highbd_subtract_block aom_highbd_subtract_block_sse2
#else
#define aom_highbd_subtract_block aom_highbd_subtract_block_c
#endif
/*unsigned int aom_highbd_sad8x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
unsigned int aom_highbd_sad8x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#define aom_highbd_sad8x8 aom_highbd_sad8x8_sse2
#else
#define aom_highbd_sad8x8 aom_highbd_sad8x8_c
#endif
unsigned int aom_highbd_sad8x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
unsigned int aom_highbd_sad8x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#define aom_highbd_sad8x16 aom_highbd_sad8x16_sse2
#else
#define aom_highbd_sad8x16 aom_highbd_sad8x16_c
#endif
unsigned int aom_highbd_sad16x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
unsigned int aom_highbd_sad16x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#define aom_highbd_sad16x8 aom_highbd_sad16x8_sse2
#else
#define aom_highbd_sad16x8 aom_highbd_sad16x8_c
#endif
unsigned int aom_highbd_sad16x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
unsigned int aom_highbd_sad16x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#define aom_highbd_sad16x16 aom_highbd_sad16x16_sse2
#else
#define aom_highbd_sad16x16 aom_highbd_sad16x16_c
#endif*/

//#if CONFIG_AV1_ENCODER
  void aom_get_blk_sse_sum_c(const int16_t *data, int stride, int bw, int bh, int *x_sum, int64_t *x2_sum);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_get_blk_sse_sum)(const int16_t *data, int stride, int bw, int bh, int *x_sum, int64_t *x2_sum);
  void aom_get_blk_sse_sum_sse2(const int16_t *data, int stride, int bw, int bh, int *x_sum, int64_t *x2_sum);
#if defined(__AVX2__)
  void aom_get_blk_sse_sum_avx2(const int16_t *data, int stride, int bw, int bh, int *x_sum, int64_t *x2_sum);
#endif
#else
  #define aom_get_blk_sse_sum aom_get_blk_sse_sum_c
#endif
  int64_t aom_highbd_sse_c(const uint16_t *a, int a_stride, const uint16_t *b,int b_stride, int width, int height);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN int64_t (*aom_highbd_sse)(const uint16_t *a, int a_stride, const uint16_t *b,int b_stride, int width, int height);
  int64_t aom_highbd_sse_sse4_1(const uint16_t *a, int a_stride, const uint16_t *b,int b_stride, int width, int height);
#endif
#if defined(__AVX2__)
  int64_t aom_highbd_sse_avx2(const uint16_t *a, int a_stride, const uint16_t *b,int b_stride, int width, int height);
#endif
#else
  #define aom_highbd_sse aom_highbd_sse_c
#endif

#if CONFIG_MRSSE
  int64_t aom_highbd_mrsse_c(const uint16_t *a8, int a_stride, const uint16_t *b8,int b_stride, int width, int height);
#if HAVE_SIMD
  RTCD_EXTERN int64_t (*aom_highbd_mrsse)(const uint16_t *a8, int a_stride, const uint16_t *b8,int b_stride, int width, int height);
#if defined(__AVX2__)
  int64_t aom_highbd_mrsse_avx2(const uint16_t *a8, int a_stride, const uint16_t *b8,int b_stride, int width, int height);
#endif
#else
  #define aom_highbd_mrsse aom_highbd_mrsse_c
#endif
#endif

  // Sum of Squares
  uint64_t aom_sum_squares_2d_i16_c(const int16_t *src, int stride, int width, int height);
#if HAVE_SIMD
  RTCD_EXTERN uint64_t (*aom_sum_squares_2d_i16)(const int16_t *src, int stride, int width, int height);
  uint64_t aom_sum_squares_2d_i16_sse2(const int16_t *src, int stride, int width, int height);
#if defined(__AVX2__)
  uint64_t aom_sum_squares_2d_i16_avx2(const int16_t *src, int stride, int width, int height);
#endif
#else
  #define aom_sum_squares_2d_i16 aom_sum_squares_2d_i16_c
#endif
  uint64_t aom_sum_squares_i16_c(const int16_t *src, uint32_t N);
#if HAVE_SIMD
  uint64_t aom_sum_squares_i16_sse2(const int16_t *src, uint32_t N);
  #define aom_sum_squares_i16 aom_sum_squares_i16_sse2
#else
  #define aom_sum_squares_i16 aom_sum_squares_i16_c
#endif
  uint64_t aom_var_2d_u8_c(uint8_t *src, int src_stride, int width, int height);
#if HAVE_SIMD
  RTCD_EXTERN uint64_t (*aom_var_2d_u8)(uint8_t *src, int stride, int width, int height);
  uint64_t aom_var_2d_u8_sse2(uint8_t *src, int stride, int width, int height);
#if defined(__AVX2__)
  uint64_t aom_var_2d_u8_avx2(uint8_t *src, int stride, int width, int height);
#endif
#else
  #define aom_var_2d_u8 aom_var_2d_u8_c
#endif
  uint64_t aom_var_2d_u16_c(uint16_t *src, int src_stride, int width, int height);
#if HAVE_SIMD
  RTCD_EXTERN uint64_t (*aom_var_2d_u16)(uint16_t *src, int stride, int width, int height);
  uint64_t aom_var_2d_u16_sse2(uint16_t *src, int stride, int width, int height);
#if defined(__AVX2__)
  uint64_t aom_var_2d_u16_avx2(uint16_t *src, int stride, int width, int height);
#endif
#else
  #define aom_var_2d_u16 aom_var_2d_u16_c
#endif
  uint64_t aom_sum_sse_2d_i16_c(const int16_t *src, int src_stride, int width, int height, int *sum);
#if HAVE_SIMD
  RTCD_EXTERN uint64_t (*aom_sum_sse_2d_i16)(const int16_t *src, int src_stride, int width, int height, int *sum);
  uint64_t aom_sum_sse_2d_i16_sse2(const int16_t *src, int src_stride, int width, int height, int *sum);
#if defined(__AVX2__)
  uint64_t aom_sum_sse_2d_i16_avx2(const int16_t *src, int src_stride, int width, int height, int *sum);
#endif
#else
  #define aom_sum_sse_2d_i16 aom_sum_sse_2d_i16_c
#endif

#if CONFIG_BLOCK_256
  unsigned int aom_highbd_sad256x256_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad256x256)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad256x256_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad256x256 aom_highbd_sad256x256_c
#endif
  unsigned int aom_highbd_sad256x128_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad256x128)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad256x128_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad256x128 aom_highbd_sad256x128_c
#endif
  unsigned int aom_highbd_sad128x256_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad128x256)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad128x256_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad128x256 aom_highbd_sad128x256_c
#endif
#endif
  unsigned int aom_highbd_sad128x128_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad128x128)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad128x128_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad128x128 aom_highbd_sad128x128_c
#endif
  unsigned int aom_highbd_sad128x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad128x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad128x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad128x64 aom_highbd_sad128x64_c
#endif
  unsigned int aom_highbd_sad64x128_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x128)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x128_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad64x128 aom_highbd_sad64x128_c
#endif
  unsigned int aom_highbd_sad64x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad64x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad64x64 aom_highbd_sad64x64_c
#endif
  unsigned int aom_highbd_sad64x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x32)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad64x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x32_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad64x32 aom_highbd_sad64x32_c
#endif
  unsigned int aom_highbd_sad32x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad32x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad32x64 aom_highbd_sad32x64_c
#endif
  unsigned int aom_highbd_sad32x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x32)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad32x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x32_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad32x32 aom_highbd_sad32x32_c
#endif
  unsigned int aom_highbd_sad32x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x16)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad32x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x16_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad32x16 aom_highbd_sad32x16_c
#endif
  unsigned int aom_highbd_sad16x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x32)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad16x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x32_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad16x32 aom_highbd_sad16x32_c
#endif
  unsigned int aom_highbd_sad16x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x16)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_UNEVEN_4WAY
  unsigned int aom_highbd_sad16x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x16_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad16x16 aom_highbd_sad16x16_c
#endif
  unsigned int aom_highbd_sad16x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x8)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_UNEVEN_4WAY
  unsigned int aom_highbd_sad16x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x8_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad16x8 aom_highbd_sad16x8_c
#endif
  unsigned int aom_highbd_sad16x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x4)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_UNEVEN_4WAY
  unsigned int aom_highbd_sad16x4_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x4_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad16x4 aom_highbd_sad16x4_c
#endif
  unsigned int aom_highbd_sad8x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad8x16 aom_highbd_sad8x16_sse2
#else
  #define aom_highbd_sad8x16 aom_highbd_sad8x16_c
#endif
  unsigned int aom_highbd_sad8x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad8x8 aom_highbd_sad8x8_sse2
#else
  #define aom_highbd_sad8x8 aom_highbd_sad8x8_c
#endif
  unsigned int aom_highbd_sad8x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x4_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad8x4 aom_highbd_sad8x4_sse2
#else
  #define aom_highbd_sad8x4 aom_highbd_sad8x4_c
#endif
  unsigned int aom_highbd_sad4x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad4x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad4x8 aom_highbd_sad4x8_sse2
#else
  #define aom_highbd_sad4x8 aom_highbd_sad4x8_c
#endif
  unsigned int aom_highbd_sad4x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad4x4_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad4x4 aom_highbd_sad4x4_sse2
#else
  #define aom_highbd_sad4x4 aom_highbd_sad4x4_c
#endif
  unsigned int aom_highbd_sad4x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad4x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad4x16 aom_highbd_sad4x16_sse2
#else
  #define aom_highbd_sad4x16 aom_highbd_sad4x16_c
#endif
  unsigned int aom_highbd_sad8x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad8x32 aom_highbd_sad8x32_sse2
#else
  #define aom_highbd_sad8x32 aom_highbd_sad8x32_c
#endif
  unsigned int aom_highbd_sad32x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x8)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad32x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x8_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad32x8 aom_highbd_sad32x8_c
#endif
  unsigned int aom_highbd_sad16x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad16x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad16x64 aom_highbd_sad16x64_c
#endif
  unsigned int aom_highbd_sad64x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x16)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad64x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x16_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad64x16 aom_highbd_sad64x16_c
#endif
#if CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad32x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad32x4_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad32x4 aom_highbd_sad32x4_sse2
#else
  #define aom_highbd_sad32x4 aom_highbd_sad32x4_c
#endif
  unsigned int aom_highbd_sad4x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
//#if HAVE_SIMD
  //unsigned int aom_highbd_sad4x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  //#define aom_highbd_sad4x32 aom_highbd_sad4x32_sse2
//#else
  #define aom_highbd_sad4x32 aom_highbd_sad4x32_c
//#endif
  unsigned int aom_highbd_sad64x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad64x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad64x8 aom_highbd_sad64x8_sse2
#else
  #define aom_highbd_sad64x8 aom_highbd_sad64x8_c
#endif
  unsigned int aom_highbd_sad8x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad8x64 aom_highbd_sad8x64_sse2
#else
  #define aom_highbd_sad8x64 aom_highbd_sad8x64_c
#endif
  unsigned int aom_highbd_sad4x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad4x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad4x64 aom_highbd_sad4x64_sse2
#else
  #define aom_highbd_sad4x64 aom_highbd_sad4x64_c
#endif
  unsigned int aom_highbd_sad64x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad64x4_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad64x4 aom_highbd_sad64x4_sse2
#else
  #define aom_highbd_sad64x4 aom_highbd_sad64x4_c
#endif
#endif

#if CONFIG_BLOCK_256
  unsigned int aom_highbd_sad_skip_256x256_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_256x256)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_256x256_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_256x256 aom_highbd_sad_skip_256x256_c
#endif
  unsigned int aom_highbd_sad_skip_256x128_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_256x128)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_256x128_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_256x128 aom_highbd_sad_skip_256x128_c
#endif
  unsigned int aom_highbd_sad_skip_128x256_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_128x256)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_128x256_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_128x256 aom_highbd_sad_skip_128x256_c
#endif
#endif
  unsigned int aom_highbd_sad_skip_128x128_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_128x128)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_128x128_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_128x128 aom_highbd_sad_skip_128x128_c
#endif
  unsigned int aom_highbd_sad_skip_128x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_128x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_128x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_128x64 aom_highbd_sad_skip_128x64_c
#endif
  unsigned int aom_highbd_sad_skip_64x128_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_64x128)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_64x128_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_64x128 aom_highbd_sad_skip_64x128_c
#endif
  unsigned int aom_highbd_sad_skip_64x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_64x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad_skip_64x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_64x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_64x64 aom_highbd_sad_skip_64x64_c
#endif
  unsigned int aom_highbd_sad_skip_64x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_64x32)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad_skip_64x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_64x32_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_64x32 aom_highbd_sad_skip_64x32_c
#endif
  unsigned int aom_highbd_sad_skip_32x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_32x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad_skip_32x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_32x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_32x64 aom_highbd_sad_skip_32x64_c
#endif
  unsigned int aom_highbd_sad_skip_32x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_32x32)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad_skip_32x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_32x32_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_32x32 aom_highbd_sad_skip_32x32_c
#endif
  unsigned int aom_highbd_sad_skip_32x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_32x16)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad_skip_32x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_32x16_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_32x16 aom_highbd_sad_skip_32x16_c
#endif
  unsigned int aom_highbd_sad_skip_16x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_16x32)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad_skip_16x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_16x32_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_16x32 aom_highbd_sad_skip_16x32_c
#endif
  unsigned int aom_highbd_sad_skip_16x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_16x16)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_UNEVEN_4WAY
  unsigned int aom_highbd_sad_skip_16x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_16x16_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_16x16 aom_highbd_sad_skip_16x16_c
#endif
  unsigned int aom_highbd_sad_skip_16x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_16x8)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_UNEVEN_4WAY
  unsigned int aom_highbd_sad_skip_16x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_16x8_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_16x8 aom_highbd_sad_skip_16x8_c
#endif
  unsigned int aom_highbd_sad_skip_16x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_16x4 aom_highbd_sad_skip_16x4_c
  unsigned int aom_highbd_sad_skip_32x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_32x4 aom_highbd_sad_skip_32x4_c
  unsigned int aom_highbd_sad_skip_64x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_64x4 aom_highbd_sad_skip_64x4_c
  unsigned int aom_highbd_sad_skip_8x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad_skip_8x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_8x16 aom_highbd_sad_skip_8x16_sse2
#else
  #define aom_highbd_sad_skip_8x16 aom_highbd_sad_skip_8x16_c
#endif
  unsigned int aom_highbd_sad_skip_8x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad_skip_8x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_8x8 aom_highbd_sad_skip_8x8_sse2
#else
  #define aom_highbd_sad_skip_8x8 aom_highbd_sad_skip_8x8_c
#endif
  unsigned int aom_highbd_sad_skip_8x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad_skip_8x4_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_8x4 aom_highbd_sad_skip_8x4_sse2
#else
  #define aom_highbd_sad_skip_8x4 aom_highbd_sad_skip_8x4_c
#endif
  unsigned int aom_highbd_sad_skip_4x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad_skip_4x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_4x8 aom_highbd_sad_skip_4x8_sse2
#else
  #define aom_highbd_sad_skip_4x8 aom_highbd_sad_skip_4x8_c
#endif
  unsigned int aom_highbd_sad_skip_4x4_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad_skip_4x4_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_4x4 aom_highbd_sad_skip_4x4_sse2
#else
  #define aom_highbd_sad_skip_4x4 aom_highbd_sad_skip_4x4_c
#endif
  unsigned int aom_highbd_sad_skip_4x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad_skip_4x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_4x16 aom_highbd_sad_skip_4x16_sse2
#else
  #define aom_highbd_sad_skip_4x16 aom_highbd_sad_skip_4x16_c
#endif
  unsigned int aom_highbd_sad_skip_4x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
//#if HAVE_SIMD
  //unsigned int aom_highbd_sad_skip_4x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  //#define aom_highbd_sad_skip_4x32 aom_highbd_sad_skip_4x32_sse2
//#else
  #define aom_highbd_sad_skip_4x32 aom_highbd_sad_skip_4x32_c
//#endif
  unsigned int aom_highbd_sad_skip_4x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
//#if HAVE_SIMD
  //unsigned int aom_highbd_sad_skip_4x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  //#define aom_highbd_sad_skip_4x64 aom_highbd_sad_skip_4x64_sse2
//#else
  #define aom_highbd_sad_skip_4x64 aom_highbd_sad_skip_4x64_c
//#endif
  unsigned int aom_highbd_sad_skip_8x32_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  unsigned int aom_highbd_sad_skip_8x32_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  #define aom_highbd_sad_skip_8x32 aom_highbd_sad_skip_8x32_sse2
#else
  #define aom_highbd_sad_skip_8x32 aom_highbd_sad_skip_8x32_c
#endif
  unsigned int aom_highbd_sad_skip_8x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
//#if HAVE_SIMD
  //unsigned int aom_highbd_sad_skip_8x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  //#define aom_highbd_sad_skip_8x64 aom_highbd_sad_skip_8x64_sse2
//#else
  #define aom_highbd_sad_skip_8x64 aom_highbd_sad_skip_8x64_c
//#endif
  unsigned int aom_highbd_sad_skip_32x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_32x8)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad_skip_32x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_32x8_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_32x8 aom_highbd_sad_skip_32x8_c
#endif
  unsigned int aom_highbd_sad_skip_64x8_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
//#if HAVE_SIMD
  //RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_32x8)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  //unsigned int aom_highbd_sad_skip_32x8_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
//#if defined(__AVX2__)
  //unsigned int aom_highbd_sad_skip_32x8_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
//#endif
//#else
  #define aom_highbd_sad_skip_64x8 aom_highbd_sad_skip_64x8_c
//#endif
  unsigned int aom_highbd_sad_skip_16x64_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_16x64)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if !CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad_skip_16x64_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_16x64_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_16x64 aom_highbd_sad_skip_16x64_c
#endif
  unsigned int aom_highbd_sad_skip_64x16_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad_skip_64x16)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
  unsigned int aom_highbd_sad_skip_64x16_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad_skip_64x16_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride);
#endif
#else
  #define aom_highbd_sad_skip_64x16 aom_highbd_sad_skip_64x16_c
#endif

#if CONFIG_BLOCK_256
  unsigned int aom_highbd_sad256x256_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad256x256_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad256x256_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad256x256_avg aom_highbd_sad256x256_avg_c
#endif
  unsigned int aom_highbd_sad256x128_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad256x128_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad256x128_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad256x128_avg aom_highbd_sad256x128_avg_c
#endif
  unsigned int aom_highbd_sad128x256_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad128x256_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad128x256_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad128x256_avg aom_highbd_sad128x256_avg_c
#endif
#endif
  unsigned int aom_highbd_sad128x128_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad128x128_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad128x128_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad128x128_avg aom_highbd_sad128x128_avg_c
#endif
  unsigned int aom_highbd_sad128x64_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad128x64_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad128x64_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad128x64_avg aom_highbd_sad128x64_avg_c
#endif
  unsigned int aom_highbd_sad64x128_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x128_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x128_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad64x128_avg aom_highbd_sad64x128_avg_c
#endif
  unsigned int aom_highbd_sad64x64_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x64_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad64x64_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x64_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad64x64_avg aom_highbd_sad64x64_avg_c
#endif
  unsigned int aom_highbd_sad64x32_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x32_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad64x32_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x32_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad64x32_avg aom_highbd_sad64x32_avg_c
#endif
  unsigned int aom_highbd_sad32x64_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x64_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad32x64_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x64_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad32x64_avg aom_highbd_sad32x64_avg_c
#endif
  unsigned int aom_highbd_sad32x32_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x32_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad32x32_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x32_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad32x32_avg aom_highbd_sad32x32_avg_c
#endif
  unsigned int aom_highbd_sad32x16_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x16_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad32x16_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x16_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad32x16_avg aom_highbd_sad32x16_avg_c
#endif
  unsigned int aom_highbd_sad16x32_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x32_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if !CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad16x32_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x32_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad16x32_avg aom_highbd_sad16x32_avg_c
#endif
  unsigned int aom_highbd_sad16x16_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x16_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if !CONFIG_UNEVEN_4WAY
  unsigned int aom_highbd_sad16x16_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x16_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad16x16_avg aom_highbd_sad16x16_avg_c
#endif
  unsigned int aom_highbd_sad16x8_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x8_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if !CONFIG_UNEVEN_4WAY
  unsigned int aom_highbd_sad16x8_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x8_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad16x8_avg aom_highbd_sad16x8_avg_c
#endif
  unsigned int aom_highbd_sad16x4_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x4_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad16x4_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x4_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad16x4_avg aom_highbd_sad16x4_avg_c
#endif
  unsigned int aom_highbd_sad8x16_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x16_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad8x16_avg aom_highbd_sad8x16_avg_sse2
#else
  #define aom_highbd_sad8x16_avg aom_highbd_sad8x16_avg_c
#endif
  unsigned int aom_highbd_sad8x8_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x8_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad8x8_avg aom_highbd_sad8x8_avg_sse2
#else
  #define aom_highbd_sad8x8_avg aom_highbd_sad8x8_avg_c
#endif
  unsigned int aom_highbd_sad8x4_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x4_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad8x4_avg aom_highbd_sad8x4_avg_sse2
#else
  #define aom_highbd_sad8x4_avg aom_highbd_sad8x4_avg_c
#endif
  unsigned int aom_highbd_sad4x8_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad4x8_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad4x8_avg aom_highbd_sad4x8_avg_sse2
#else
  #define aom_highbd_sad4x8_avg aom_highbd_sad4x8_avg_c
#endif
  unsigned int aom_highbd_sad4x4_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad4x4_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad4x4_avg aom_highbd_sad4x4_avg_sse2
#else
  #define aom_highbd_sad4x4_avg aom_highbd_sad4x4_avg_c
#endif
  unsigned int aom_highbd_sad4x16_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad4x16_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad4x16_avg aom_highbd_sad4x16_avg_sse2
#else
  #define aom_highbd_sad4x16_avg aom_highbd_sad4x16_avg_c
#endif
  unsigned int aom_highbd_sad8x32_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x32_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad8x32_avg aom_highbd_sad8x32_avg_sse2
#else
  #define aom_highbd_sad8x32_avg aom_highbd_sad8x32_avg_c
#endif
  unsigned int aom_highbd_sad32x8_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad32x8_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad32x8_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad32x8_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad32x8_avg aom_highbd_sad32x8_avg_c
#endif
  unsigned int aom_highbd_sad16x64_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad16x64_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if !CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad16x64_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_sad16x64_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad16x64_avg aom_highbd_sad16x64_avg_c
#endif
  unsigned int aom_highbd_sad64x16_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_sad64x16_avg)(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  unsigned int aom_highbd_sad64x16_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if defined(__AVX2__)
  unsigned int aom_highbd_sad64x16_avg_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_sad64x16_avg aom_highbd_sad64x16_avg_c
#endif
#if CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_sad32x4_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad32x4_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad32x4_avg aom_highbd_sad32x4_avg_sse2
#else
  #define aom_highbd_sad32x4_avg aom_highbd_sad32x4_avg_c
#endif
  unsigned int aom_highbd_sad4x32_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
//#if HAVE_SIMD
  //unsigned int aom_highbd_sad4x32_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  //#define aom_highbd_sad4x32_avg aom_highbd_sad4x32_avg_sse2
//#else
  #define aom_highbd_sad4x32_avg aom_highbd_sad4x32_avg_c
//#endif
  unsigned int aom_highbd_sad8x64_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad8x64_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad8x64_avg aom_highbd_sad8x64_avg_sse2
#else
  #define aom_highbd_sad8x64_avg aom_highbd_sad8x64_avg_c
#endif
  unsigned int aom_highbd_sad64x8_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad64x8_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad64x8_avg aom_highbd_sad64x8_avg_sse2
#else
  #define aom_highbd_sad64x8_avg aom_highbd_sad64x8_avg_c
#endif
  unsigned int aom_highbd_sad4x64_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
//#if HAVE_SIMD
  //unsigned int aom_highbd_sad4x64_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  //#define aom_highbd_sad4x64_avg aom_highbd_sad4x64_avg_sse2
//#else
  #define aom_highbd_sad4x64_avg aom_highbd_sad4x64_avg_c
//#endif
  unsigned int aom_highbd_sad64x4_avg_c(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
#if HAVE_SIMD
  unsigned int aom_highbd_sad64x4_avg_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t *ref_ptr, int ref_stride, const uint16_t *second_pred);
  #define aom_highbd_sad64x4_avg aom_highbd_sad64x4_avg_sse2
#else
  #define aom_highbd_sad64x4_avg aom_highbd_sad64x4_avg_c
#endif
#endif

  // Masked SAD
#if CONFIG_BLOCK_256
  unsigned int aom_highbd_masked_sad256x256_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad256x256)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad256x256_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad256x256 aom_highbd_masked_sad256x256_c
#endif
  unsigned int aom_highbd_masked_sad256x128_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad256x128)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad256x128_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad256x128 aom_highbd_masked_sad256x128_c
#endif
  unsigned int aom_highbd_masked_sad128x256_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad128x256)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad128x256_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad128x256 aom_highbd_masked_sad128x256_c
#endif
#endif
  unsigned int aom_highbd_masked_sad128x128_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad128x128)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad128x128_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad128x128_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad128x128 aom_highbd_masked_sad128x128_c
#endif
  unsigned int aom_highbd_masked_sad128x64_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad128x64)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad128x64_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad128x64_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad128x64 aom_highbd_masked_sad128x64_c
#endif
  unsigned int aom_highbd_masked_sad64x128_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad64x128)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad64x128_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad64x128_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad64x128 aom_highbd_masked_sad64x128_c
#endif
  unsigned int aom_highbd_masked_sad64x64_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad64x64)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad64x64_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad64x64_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad64x64 aom_highbd_masked_sad64x64_c
#endif
  unsigned int aom_highbd_masked_sad64x32_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad64x32)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad64x32_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad64x32_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad64x32 aom_highbd_masked_sad64x32_c
#endif
  unsigned int aom_highbd_masked_sad32x64_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad32x64)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad32x64_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad32x64_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad32x64 aom_highbd_masked_sad32x64_c
#endif
  unsigned int aom_highbd_masked_sad32x32_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad32x32)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad32x32_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad32x32_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad32x32 aom_highbd_masked_sad32x32_c
#endif
  unsigned int aom_highbd_masked_sad32x16_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad32x16)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad32x16_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad32x16_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad32x16 aom_highbd_masked_sad32x16_c
#endif
  unsigned int aom_highbd_masked_sad16x32_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad16x32)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad16x32_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad16x32_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad16x32 aom_highbd_masked_sad16x32_c
#endif
  unsigned int aom_highbd_masked_sad16x16_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad16x16)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad16x16_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad16x16_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad16x16 aom_highbd_masked_sad16x16_c
#endif
  unsigned int aom_highbd_masked_sad16x8_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad16x8)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad16x8_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad16x8_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad16x8 aom_highbd_masked_sad16x8_c
#endif
  unsigned int aom_highbd_masked_sad8x16_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad8x16)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad8x16_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad8x16_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad8x16 aom_highbd_masked_sad8x16_c
#endif
  unsigned int aom_highbd_masked_sad8x8_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad8x8)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad8x8_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad8x8_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad8x8 aom_highbd_masked_sad8x8_c
#endif
  unsigned int aom_highbd_masked_sad8x4_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad8x4)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad8x4_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad8x4_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad8x4 aom_highbd_masked_sad8x4_c
#endif
  unsigned int aom_highbd_masked_sad4x8_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad4x8)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad4x8_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad4x8_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad4x8 aom_highbd_masked_sad4x8_c
#endif
  unsigned int aom_highbd_masked_sad4x4_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad4x4)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad4x4_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad4x4_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad4x4 aom_highbd_masked_sad4x4_c
#endif
  unsigned int aom_highbd_masked_sad4x16_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad4x16)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad4x16_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad4x16_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad4x16 aom_highbd_masked_sad4x16_c
#endif
  unsigned int aom_highbd_masked_sad16x4_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad16x4)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad16x4_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad16x4_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad16x4 aom_highbd_masked_sad16x4_c
#endif
  unsigned int aom_highbd_masked_sad4x64_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad4x64)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad4x64_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad4x64_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad4x64 aom_highbd_masked_sad4x64_c
#endif
  unsigned int aom_highbd_masked_sad64x4_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad64x4)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad64x4_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad64x4_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad64x4 aom_highbd_masked_sad64x4_c
#endif
  unsigned int aom_highbd_masked_sad4x32_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad4x32)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad4x32_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad4x32_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad4x32 aom_highbd_masked_sad4x32_c
#endif
  unsigned int aom_highbd_masked_sad32x4_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad32x4)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad32x4_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad32x4_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad32x4 aom_highbd_masked_sad32x4_c
#endif
  unsigned int aom_highbd_masked_sad8x32_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad8x32)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad8x32_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad8x32_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad8x32 aom_highbd_masked_sad8x32_c
#endif
  unsigned int aom_highbd_masked_sad32x8_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad32x8)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad32x8_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad32x8_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad32x8 aom_highbd_masked_sad32x8_c
#endif
  unsigned int aom_highbd_masked_sad8x64_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad8x64)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad8x64_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad8x64_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad8x64 aom_highbd_masked_sad8x64_c
#endif
  unsigned int aom_highbd_masked_sad64x8_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad64x8)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad64x8_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad64x8_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad64x8 aom_highbd_masked_sad64x8_c
#endif
  unsigned int aom_highbd_masked_sad16x64_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad16x64)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad16x64_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad16x64_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad16x64 aom_highbd_masked_sad16x64_c
#endif
  unsigned int aom_highbd_masked_sad64x16_c(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSSE3__)
  RTCD_EXTERN unsigned int (*aom_highbd_masked_sad64x16)(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
  unsigned int aom_highbd_masked_sad64x16_ssse3(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_masked_sad64x16_avx2(const uint16_t *src8, int src_stride, const uint16_t *ref8, int ref_stride, const uint16_t *second_pred8, const uint8_t *msk, int msk_stride, int invert_mask);
#endif
#else
  #define aom_highbd_masked_sad64x16 aom_highbd_masked_sad64x16_c
#endif

  // OBMC SAD
#if CONFIG_BLOCK_256
  unsigned int aom_highbd_obmc_sad256x256_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad256x256)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad256x256_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad256x256 aom_highbd_obmc_sad256x256_c
#endif
  unsigned int aom_highbd_obmc_sad256x128_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad256x128)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad256x128_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad256x128 aom_highbd_obmc_sad256x128_c
#endif
  unsigned int aom_highbd_obmc_sad128x256_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad128x256)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad128x256_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad128x256 aom_highbd_obmc_sad128x256_c
#endif
#endif
  unsigned int aom_highbd_obmc_sad128x128_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad128x128)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad128x128_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad128x128_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad128x128 aom_highbd_obmc_sad128x128_c
#endif
  unsigned int aom_highbd_obmc_sad128x64_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad128x64)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad128x64_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad128x64_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad128x64 aom_highbd_obmc_sad128x64_c
#endif
  unsigned int aom_highbd_obmc_sad64x128_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad64x128)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad64x128_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad64x128_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad64x128 aom_highbd_obmc_sad64x128_c
#endif
  unsigned int aom_highbd_obmc_sad64x64_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad64x64)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad64x64_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad64x64_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad64x64 aom_highbd_obmc_sad64x64_c
#endif
  unsigned int aom_highbd_obmc_sad128x32_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  #define aom_highbd_obmc_sad128x32 aom_highbd_obmc_sad128x32_c
  unsigned int aom_highbd_obmc_sad32x128_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  #define aom_highbd_obmc_sad32x128 aom_highbd_obmc_sad32x128_c
  unsigned int aom_highbd_obmc_sad64x32_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad64x32)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad64x32_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad64x32_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad64x32 aom_highbd_obmc_sad64x32_c
#endif
  unsigned int aom_highbd_obmc_sad32x64_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad32x64)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad32x64_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad32x64_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad32x64 aom_highbd_obmc_sad32x64_c
#endif
  unsigned int aom_highbd_obmc_sad32x32_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad32x32)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad32x32_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad32x32_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad32x32 aom_highbd_obmc_sad32x32_c
#endif
  unsigned int aom_highbd_obmc_sad32x16_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad32x16)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad32x16_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad32x16_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad32x16 aom_highbd_obmc_sad32x16_c
#endif
  unsigned int aom_highbd_obmc_sad16x32_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad16x32)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad16x32_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad16x32_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad16x32 aom_highbd_obmc_sad16x32_c
#endif
  unsigned int aom_highbd_obmc_sad16x16_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad16x16)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad16x16_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad16x16_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad16x16 aom_highbd_obmc_sad16x16_c
#endif
  unsigned int aom_highbd_obmc_sad16x8_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad16x8)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad16x8_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad16x8_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad16x8 aom_highbd_obmc_sad16x8_c
#endif
  unsigned int aom_highbd_obmc_sad8x16_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad8x16)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad8x16_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad8x16_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad8x16 aom_highbd_obmc_sad8x16_c
#endif
  unsigned int aom_highbd_obmc_sad8x64_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad8x64)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad8x64_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad8x64_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad8x64 aom_highbd_obmc_sad8x64_c
#endif
  unsigned int aom_highbd_obmc_sad8x8_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad8x8)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad8x8_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad8x8_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad8x8 aom_highbd_obmc_sad8x8_c
#endif
  unsigned int aom_highbd_obmc_sad8x4_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad8x4)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad8x4_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad8x4_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad8x4 aom_highbd_obmc_sad8x4_c
#endif
  unsigned int aom_highbd_obmc_sad4x8_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad4x8)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad4x8_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad4x8_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad4x8 aom_highbd_obmc_sad4x8_c
#endif
  unsigned int aom_highbd_obmc_sad4x4_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad4x4)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad4x4_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad4x4_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad4x4 aom_highbd_obmc_sad4x4_c
#endif
  unsigned int aom_highbd_obmc_sad4x16_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad4x16)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad4x16_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad4x16_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad4x16 aom_highbd_obmc_sad4x16_c
#endif
  unsigned int aom_highbd_obmc_sad4x32_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad4x32)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad4x32_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad4x32_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad4x32 aom_highbd_obmc_sad4x32_c
#endif
  unsigned int aom_highbd_obmc_sad4x64_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad4x64)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad4x64_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad4x64_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad4x64 aom_highbd_obmc_sad4x64_c
#endif
  unsigned int aom_highbd_obmc_sad16x4_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad16x4)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad16x4_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad16x4_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad16x4 aom_highbd_obmc_sad16x4_c
#endif
  unsigned int aom_highbd_obmc_sad32x4_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad32x4)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad32x4_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad32x4_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad32x4 aom_highbd_obmc_sad32x4_c
#endif
  unsigned int aom_highbd_obmc_sad64x4_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad64x4)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad64x4_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad64x4_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad64x4 aom_highbd_obmc_sad64x4_c
#endif
  unsigned int aom_highbd_obmc_sad8x32_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad8x32)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad8x32_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad8x32_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad8x32 aom_highbd_obmc_sad8x32_c
#endif
  unsigned int aom_highbd_obmc_sad32x8_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad32x8)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad32x8_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad32x8_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad32x8 aom_highbd_obmc_sad32x8_c
#endif
  unsigned int aom_highbd_obmc_sad64x8_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad64x8)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad64x8_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad64x8_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad64x8 aom_highbd_obmc_sad64x8_c
#endif
  unsigned int aom_highbd_obmc_sad16x64_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad16x64)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad16x64_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad16x64_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad16x64 aom_highbd_obmc_sad16x64_c
#endif
  unsigned int aom_highbd_obmc_sad64x16_c(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_obmc_sad64x16)(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
  unsigned int aom_highbd_obmc_sad64x16_sse4_1(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#if defined(__AVX2__)
  unsigned int aom_highbd_obmc_sad64x16_avx2(const uint16_t *pre, int pre_stride, const int32_t *wsrc, const int32_t *mask);
#endif
#else
  #define aom_highbd_obmc_sad64x16 aom_highbd_obmc_sad64x16_c
#endif

  // Multi-block SAD, comparing a reference to N independent blocks
#if CONFIG_BLOCK_256
  void aom_highbd_sad256x256x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad256x256x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad256x256x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad256x256x4d aom_highbd_sad256x256x4d_c
#endif
  void aom_highbd_sad256x128x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad256x128x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad256x128x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad256x128x4d aom_highbd_sad256x128x4d_c
#endif
  void aom_highbd_sad128x256x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad128x256x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad128x256x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad128x256x4d aom_highbd_sad128x256x4d_c
#endif
#endif
  void aom_highbd_sad128x128x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad128x128x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad128x128x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad128x128x4d aom_highbd_sad128x128x4d_c
#endif
  void aom_highbd_sad128x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad128x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad128x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad128x64x4d aom_highbd_sad128x64x4d_c
#endif
  void aom_highbd_sad64x128x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad64x128x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad64x128x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad64x128x4d aom_highbd_sad64x128x4d_c
#endif
  void aom_highbd_sad64x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad64x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad64x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad64x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad64x64x4d aom_highbd_sad64x64x4d_c
#endif
  void aom_highbd_sad64x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad64x32x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad64x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad64x32x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad64x32x4d aom_highbd_sad64x32x4d_c
#endif
  void aom_highbd_sad32x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad32x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad32x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad32x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad32x64x4d aom_highbd_sad32x64x4d_c
#endif
  void aom_highbd_sad32x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad32x32x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad32x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad32x32x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad32x32x4d aom_highbd_sad32x32x4d_c
#endif
  void aom_highbd_sad32x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad32x16x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad32x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad32x16x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad32x16x4d aom_highbd_sad32x16x4d_c
#endif
  void aom_highbd_sad16x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad16x32x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad16x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad16x32x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad16x32x4d aom_highbd_sad16x32x4d_c
#endif
  void aom_highbd_sad16x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad16x16x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad16x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad16x16x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad16x16x4d aom_highbd_sad16x16x4d_c
#endif
  void aom_highbd_sad16x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad16x8x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad16x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad16x8x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad16x8x4d aom_highbd_sad16x8x4d_c
#endif
  void aom_highbd_sad16x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad16x4x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad16x4x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad16x4x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad16x4x4d aom_highbd_sad16x4x4d_c
#endif
  void aom_highbd_sad8x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad8x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad8x16x4d aom_highbd_sad8x16x4d_sse2
#else
  #define aom_highbd_sad8x16x4d aom_highbd_sad8x16x4d_c
#endif
  void aom_highbd_sad8x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad8x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad8x8x4d aom_highbd_sad8x8x4d_sse2
#else
  #define aom_highbd_sad8x8x4d aom_highbd_sad8x8x4d_c
#endif
  void aom_highbd_sad8x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad8x4x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad8x4x4d aom_highbd_sad8x4x4d_sse2
#else
  #define aom_highbd_sad8x4x4d aom_highbd_sad8x4x4d_c
#endif
  void aom_highbd_sad4x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad4x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad4x8x4d aom_highbd_sad4x8x4d_sse2
#else
  #define aom_highbd_sad4x8x4d aom_highbd_sad4x8x4d_c
#endif
  void aom_highbd_sad4x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad4x4x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad4x4x4d aom_highbd_sad4x4x4d_sse2
#else
  #define aom_highbd_sad4x4x4d aom_highbd_sad4x4x4d_c
#endif
  void aom_highbd_sad4x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad4x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad4x16x4d aom_highbd_sad4x16x4d_sse2
#else
  #define aom_highbd_sad4x16x4d aom_highbd_sad4x16x4d_c
#endif
  void aom_highbd_sad8x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad8x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad8x32x4d aom_highbd_sad8x32x4d_sse2
#else
  #define aom_highbd_sad8x32x4d aom_highbd_sad8x32x4d_c
#endif
  void aom_highbd_sad32x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad32x8x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad32x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad32x8x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad32x8x4d aom_highbd_sad32x8x4d_c
#endif
  void aom_highbd_sad16x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad16x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad16x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad16x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad16x64x4d aom_highbd_sad16x64x4d_c
#endif
  void aom_highbd_sad64x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad64x16x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad64x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad64x16x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad64x16x4d aom_highbd_sad64x16x4d_c
#endif
#if CONFIG_FLEX_PARTITION
  void aom_highbd_sad4x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad4x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad4x32x4d aom_highbd_sad4x32x4d_sse2
#else
  #define aom_highbd_sad4x32x4d aom_highbd_sad4x32x4d_c
#endif
  void aom_highbd_sad32x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad32x4x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad32x4x4d aom_highbd_sad32x4x4d_sse2
#else
  #define aom_highbd_sad32x4x4d aom_highbd_sad32x4x4d_c
#endif
  void aom_highbd_sad8x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad8x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad8x64x4d aom_highbd_sad8x64x4d_sse2
#else
  #define aom_highbd_sad8x64x4d aom_highbd_sad8x64x4d_c
#endif
  void aom_highbd_sad64x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad64x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad64x8x4d aom_highbd_sad64x8x4d_sse2
#else
  #define aom_highbd_sad64x8x4d aom_highbd_sad64x8x4d_c
#endif
  void aom_highbd_sad4x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad4x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad4x64x4d aom_highbd_sad4x64x4d_sse2
#else
  #define aom_highbd_sad4x64x4d aom_highbd_sad4x64x4d_c
#endif
  void aom_highbd_sad64x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad64x4x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad64x4x4d aom_highbd_sad64x4x4d_sse2
#else
  #define aom_highbd_sad64x4x4d aom_highbd_sad64x4x4d_c
#endif
#endif

#if CONFIG_BLOCK_256
  void aom_highbd_sad_skip_256x256x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_256x256x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_256x256x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_256x256x4d aom_highbd_sad_skip_256x256x4d_c
#endif
  void aom_highbd_sad_skip_256x128x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_256x128x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_256x128x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_256x128x4d aom_highbd_sad_skip_256x128x4d_c
#endif
  void aom_highbd_sad_skip_128x256x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_128x256x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_128x256x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_128x256x4d aom_highbd_sad_skip_128x256x4d_c
#endif
#endif
  void aom_highbd_sad_skip_128x128x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_128x128x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_128x128x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_128x128x4d aom_highbd_sad_skip_128x128x4d_c
#endif
  void aom_highbd_sad_skip_128x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_128x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_128x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_128x64x4d aom_highbd_sad_skip_128x64x4d_c
#endif
  void aom_highbd_sad_skip_64x128x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_64x128x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_64x128x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_64x128x4d aom_highbd_sad_skip_64x128x4d_c
#endif
  void aom_highbd_sad_skip_64x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_64x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_64x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_64x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_64x64x4d aom_highbd_sad_skip_64x64x4d_c
#endif
  void aom_highbd_sad_skip_64x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_64x32x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_64x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_64x32x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_64x32x4d aom_highbd_sad_skip_64x32x4d_c
#endif
  void aom_highbd_sad_skip_32x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_32x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_32x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_32x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_32x64x4d aom_highbd_sad_skip_32x64x4d_c
#endif
  void aom_highbd_sad_skip_32x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_32x32x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_32x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_32x32x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_32x32x4d aom_highbd_sad_skip_32x32x4d_c
#endif
  void aom_highbd_sad_skip_32x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_32x16x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_32x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_32x16x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_32x16x4d aom_highbd_sad_skip_32x16x4d_c
#endif
  void aom_highbd_sad_skip_16x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_16x32x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_16x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_16x32x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_16x32x4d aom_highbd_sad_skip_16x32x4d_c
#endif
  void aom_highbd_sad_skip_16x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_16x16x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_16x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_16x16x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_16x16x4d aom_highbd_sad_skip_16x16x4d_c
#endif
  void aom_highbd_sad_skip_16x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_16x8x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_16x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_16x8x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_16x8x4d aom_highbd_sad_skip_16x8x4d_c
#endif
  void aom_highbd_sad_skip_16x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_16x4x4d aom_highbd_sad_skip_16x4x4d_c
  void aom_highbd_sad_skip_32x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_32x4x4d aom_highbd_sad_skip_32x4x4d_c
  void aom_highbd_sad_skip_64x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_64x4x4d aom_highbd_sad_skip_64x4x4d_c
  void aom_highbd_sad_skip_8x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad_skip_8x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_8x16x4d aom_highbd_sad_skip_8x16x4d_sse2
#else
  #define aom_highbd_sad_skip_8x16x4d aom_highbd_sad_skip_8x16x4d_c
#endif
  void aom_highbd_sad_skip_8x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad_skip_8x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_8x8x4d aom_highbd_sad_skip_8x8x4d_sse2
#else
  #define aom_highbd_sad_skip_8x8x4d aom_highbd_sad_skip_8x8x4d_c
#endif
  void aom_highbd_sad_skip_8x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_8x4x4d aom_highbd_sad_skip_8x4x4d_c
  void aom_highbd_sad_skip_4x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad_skip_4x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_4x8x4d aom_highbd_sad_skip_4x8x4d_sse2
#else
  #define aom_highbd_sad_skip_4x8x4d aom_highbd_sad_skip_4x8x4d_c
#endif
  void aom_highbd_sad_skip_4x4x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_4x4x4d aom_highbd_sad_skip_4x4x4d_c
  void aom_highbd_sad_skip_4x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad_skip_4x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_4x16x4d aom_highbd_sad_skip_4x16x4d_sse2
#else
  #define aom_highbd_sad_skip_4x16x4d aom_highbd_sad_skip_4x16x4d_c
#endif
  void aom_highbd_sad_skip_4x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
//#if HAVE_SIMD
  //void aom_highbd_sad_skip_4x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  //#define aom_highbd_sad_skip_4x32x4d aom_highbd_sad_skip_4x32x4d_sse2
//#else
  #define aom_highbd_sad_skip_4x32x4d aom_highbd_sad_skip_4x32x4d_c
//#endif
  void aom_highbd_sad_skip_4x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
//#if HAVE_SIMD
  //void aom_highbd_sad_skip_4x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  //#define aom_highbd_sad_skip_4x64x4d aom_highbd_sad_skip_4x64x4d_sse2
//#else
  #define aom_highbd_sad_skip_4x64x4d aom_highbd_sad_skip_4x64x4d_c
//#endif
  void aom_highbd_sad_skip_8x32x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  void aom_highbd_sad_skip_8x32x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  #define aom_highbd_sad_skip_8x32x4d aom_highbd_sad_skip_8x32x4d_sse2
#else
  #define aom_highbd_sad_skip_8x32x4d aom_highbd_sad_skip_8x32x4d_c
#endif
  void aom_highbd_sad_skip_8x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
//#if HAVE_SIMD
  //void aom_highbd_sad_skip_8x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  //#define aom_highbd_sad_skip_8x64x4d aom_highbd_sad_skip_8x64x4d_sse2
//#else
  #define aom_highbd_sad_skip_8x64x4d aom_highbd_sad_skip_8x64x4d_c
//#endif
  void aom_highbd_sad_skip_64x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
//#if HAVE_SIMD
  //RTCD_EXTERN void (*aom_highbd_sad_skip_64x8x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  //void aom_highbd_sad_skip_64x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
//#if defined(__AVX2__)
  //void aom_highbd_sad_skip_64x8x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
//#endif
//#else
  #define aom_highbd_sad_skip_64x8x4d aom_highbd_sad_skip_64x8x4d_c
//#endif
  void aom_highbd_sad_skip_32x8x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_32x8x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_32x8x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_32x8x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_32x8x4d aom_highbd_sad_skip_32x8x4d_c
#endif
  void aom_highbd_sad_skip_16x64x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_16x64x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_16x64x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_16x64x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_16x64x4d aom_highbd_sad_skip_16x64x4d_c
#endif
  void aom_highbd_sad_skip_64x16x4d_c(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_sad_skip_64x16x4d)(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
  void aom_highbd_sad_skip_64x16x4d_sse2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#if defined(__AVX2__)
  void aom_highbd_sad_skip_64x16x4d_avx2(const uint16_t *src_ptr, int src_stride, const uint16_t * const ref_ptr[], int ref_stride, uint32_t *sad_array);
#endif
#else
  #define aom_highbd_sad_skip_64x16x4d aom_highbd_sad_skip_64x16x4d_c
#endif

  // Avg
  unsigned int aom_highbd_avg_8x8_c(const uint16_t *, int p);
  #define aom_highbd_avg_8x8 aom_highbd_avg_8x8_c
  unsigned int aom_highbd_avg_4x4_c(const uint16_t *, int p);
  #define aom_highbd_avg_4x4 aom_highbd_avg_4x4_c
  void aom_highbd_minmax_8x8_c(const uint16_t *s, int p, const uint16_t *d, int dp, int *min, int *max);
  #define aom_highbd_minmax_8x8 aom_highbd_minmax_8x8_c

  int aom_vector_var_c(const int16_t *ref, const int16_t *src, const int bwl);
#if HAVE_SIMD
  int aom_vector_var_sse2(const int16_t *ref, const int16_t *src, const int bwl);
  #define aom_vector_var aom_vector_var_sse2
#else
  #define aom_vector_var aom_vector_var_c
#endif

  // hamadard transform and satd for implmenting temporal dependency model
  void aom_highbd_hadamard_8x8_c(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_hadamard_8x8)(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#if defined(__AVX2__)
  void aom_highbd_hadamard_8x8_avx2(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#endif
#else
  #define aom_highbd_hadamard_8x8 aom_highbd_hadamard_8x8_c
#endif
  void aom_highbd_hadamard_16x16_c(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_hadamard_16x16)(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#if defined(__AVX2__)
  void aom_highbd_hadamard_16x16_avx2(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#endif
#else
  #define aom_highbd_hadamard_16x16 aom_highbd_hadamard_16x16_c
#endif
  void aom_highbd_hadamard_32x32_c(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#if HAVE_SIMD
  RTCD_EXTERN void (*aom_highbd_hadamard_32x32)(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#if defined(__AVX2__)
  void aom_highbd_hadamard_32x32_avx2(const int16_t *src_diff, ptrdiff_t src_stride, tran_low_t *coeff);
#endif
#else
  #define aom_highbd_hadamard_32x32 aom_highbd_hadamard_32x32_c
#endif

  int aom_satd_c(const tran_low_t *coeff, int length);
#if HAVE_SIMD
  RTCD_EXTERN int (*aom_satd)(const tran_low_t *coeff, int length);
#if defined(__AVX2__)
  int aom_satd_avx2(const tran_low_t *coeff, int length);
#endif
#else
  #define aom_satd aom_satd_c
#endif

  int aom_satd_lp_c(const int16_t *coeff, int length);
#if HAVE_SIMD
  RTCD_EXTERN int (*aom_satd_lp)(const int16_t *coeff, int length);
#if defined(__AVX2__)
  int aom_satd_lp_avx2(const int16_t *coeff, int length);
#endif
#else
  #define aom_satd_lp aom_satd_lp_c
#endif

  // Structured Similarity (SSIM)
#if CONFIG_INTERNAL_STATS
  void aom_highbd_ssim_parms_8x8_c(const uint16_t *s, int sp, const uint16_t *r, int rp, uint32_t *sum_s, uint32_t *sum_r, uint32_t *sum_sq_s, uint32_t *sum_sq_r, uint32_t *sum_sxr);
  #define aom_highbd_ssim_parms_8x8 aom_highbd_ssim_parms_8x8_c
#endif
//#endif

//#if CONFIG_AV1_ENCODER
  void aom_highbd_upsampled_pred_c(MACROBLOCKD *xd, const struct AV1Common *const cm, int mi_row, int mi_col,
                                   const MV *const mv, uint16_t *comp_pred8, int width, int height, int subpel_x_q3,
                                   int subpel_y_q3, const uint16_t *ref8, int ref_stride, int bd, int subpel_search,
						           int is_scaled_ref);
#if HAVE_SIMD
  void aom_highbd_upsampled_pred_sse2(MACROBLOCKD *xd, const struct AV1Common *const cm, int mi_row, int mi_col,
                                   const MV *const mv, uint16_t *comp_pred8, int width, int height, int subpel_x_q3,
                                   int subpel_y_q3, const uint16_t *ref8, int ref_stride, int bd, int subpel_search,
						           int is_scaled_ref);
  #define aom_highbd_upsampled_pred aom_highbd_upsampled_pred_sse2
#else
  #define aom_highbd_upsampled_pred aom_highbd_upsampled_pred_c
#endif
  void aom_highbd_comp_avg_upsampled_pred_c(MACROBLOCKD *xd, const struct AV1Common *const cm, int mi_row, int mi_col,
                                            const MV *const mv, uint16_t *comp_pred8, const uint16_t *pred8, int width,
                                            int height, int subpel_x_q3, int subpel_y_q3, const uint16_t *ref8, int ref_stride,
							                int bd, int subpel_search);
#if HAVE_SIMD
  void aom_highbd_comp_avg_upsampled_pred_sse2(MACROBLOCKD *xd, const struct AV1Common *const cm, int mi_row, int mi_col,
                                            const MV *const mv, uint16_t *comp_pred8, const uint16_t *pred8, int width,
                                            int height, int subpel_x_q3, int subpel_y_q3, const uint16_t *ref8, int ref_stride,
							                int bd, int subpel_search);
  #define aom_highbd_comp_avg_upsampled_pred aom_highbd_comp_avg_upsampled_pred_sse2
#else
  #define aom_highbd_comp_avg_upsampled_pred aom_highbd_comp_avg_upsampled_pred_c
#endif
  void aom_highbd_dist_wtd_comp_avg_upsampled_pred_c(MACROBLOCKD *xd, const struct AV1Common *const cm, int mi_row, int mi_col,
                                                     const MV *const mv, uint16_t *comp_pred8, const uint16_t *pred8, int width,
                                                     int height, int subpel_x_q3, int subpel_y_q3, const uint16_t *ref8,
                                                     int ref_stride, int bd, const DIST_WTD_COMP_PARAMS *jcp_param, int subpel_search);
#if HAVE_SIMD
  void aom_highbd_dist_wtd_comp_avg_upsampled_pred_sse2(MACROBLOCKD *xd, const struct AV1Common *const cm, int mi_row, int mi_col,
                                                     const MV *const mv, uint16_t *comp_pred8, const uint16_t *pred8, int width,
                                                     int height, int subpel_x_q3, int subpel_y_q3, const uint16_t *ref8,
                                                     int ref_stride, int bd, const DIST_WTD_COMP_PARAMS *jcp_param, int subpel_search);
  #define aom_highbd_dist_wtd_comp_avg_upsampled_pred aom_highbd_dist_wtd_comp_avg_upsampled_pred_sse2
#else
  #define aom_highbd_dist_wtd_comp_avg_upsampled_pred aom_highbd_dist_wtd_comp_avg_upsampled_pred_c
#endif
  void aom_highbd_comp_mask_upsampled_pred_c(MACROBLOCKD *xd, const struct AV1Common *const cm, int mi_row, int mi_col,
                                             const MV *const mv, uint16_t *comp_pred8, const uint16_t *pred8, int width,
                                             int height, int subpel_x_q3, int subpel_y_q3, const uint16_t *ref8,
                                             int ref_stride, const uint8_t *mask, int mask_stride, int invert_mask,
                                             int bd, int subpel_search);
  #define aom_highbd_comp_mask_upsampled_pred aom_highbd_comp_mask_upsampled_pred_c

  unsigned int aom_get_mb_ss_c(const int16_t *);
#if HAVE_SIMD
  unsigned int aom_get_mb_ss_sse2(const int16_t *);
  #define aom_get_mb_ss aom_get_mb_ss_sse2
#else
  #define aom_get_mb_ss aom_get_mb_ss_c
#endif

  // Variance / Subpixel Variance / Subpixel Avg Variance
  unsigned int aom_highbd_8_variance2x2_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance2x2 aom_highbd_8_variance2x2_c
  unsigned int aom_highbd_10_variance2x2_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance2x2 aom_highbd_10_variance2x2_c
  unsigned int aom_highbd_12_variance2x2_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance2x2 aom_highbd_12_variance2x2_c

  unsigned int aom_highbd_8_variance2x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance2x4 aom_highbd_8_variance2x4_c
  unsigned int aom_highbd_10_variance2x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance2x4 aom_highbd_10_variance2x4_c
  unsigned int aom_highbd_12_variance2x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance2x4 aom_highbd_12_variance2x4_c

  unsigned int aom_highbd_8_variance4x2_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance4x2 aom_highbd_8_variance4x2_c
  unsigned int aom_highbd_10_variance4x2_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance4x2 aom_highbd_10_variance4x2_c
  unsigned int aom_highbd_12_variance4x2_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance4x2 aom_highbd_12_variance4x2_c

#if CONFIG_BLOCK_256
  unsigned int aom_highbd_8_variance256x256_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance256x256)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance256x256_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance256x256_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance256x256 aom_highbd_8_variance256x256_c
#endif
  unsigned int aom_highbd_8_variance256x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance256x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance256x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance256x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance256x128 aom_highbd_8_variance256x128_c
#endif
  unsigned int aom_highbd_8_variance128x256_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance128x256)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance128x256_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance128x256_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance128x256 aom_highbd_8_variance128x256_c
#endif
  unsigned int aom_highbd_10_variance256x256_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance256x256)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_10_variance256x256_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance256x256_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance256x256 aom_highbd_10_variance256x256_c
#endif
  unsigned int aom_highbd_10_variance256x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance256x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_10_variance256x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance256x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance256x128 aom_highbd_10_variance256x128_c
#endif
  unsigned int aom_highbd_10_variance128x256_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance128x256)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_10_variance128x256_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance128x256_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance128x256 aom_highbd_10_variance128x256_c
#endif
  unsigned int aom_highbd_12_variance256x256_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance256x256)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance256x256_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance256x256_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance256x256 aom_highbd_12_variance256x256_c
#endif
  unsigned int aom_highbd_12_variance256x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance256x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance256x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance256x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance256x128 aom_highbd_12_variance256x128_c
#endif
  unsigned int aom_highbd_12_variance128x256_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance128x256)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance128x256_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance128x256_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance128x256 aom_highbd_12_variance128x256_c
#endif
#endif
  unsigned int aom_highbd_8_variance128x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance128x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance128x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance128x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance128x128 aom_highbd_8_variance128x128_c
#endif
  unsigned int aom_highbd_10_variance128x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance128x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_10_variance128x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance128x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance128x128 aom_highbd_10_variance128x128_c
#endif
  unsigned int aom_highbd_12_variance128x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance128x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance128x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance128x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance128x128 aom_highbd_12_variance128x128_c
#endif

  unsigned int aom_highbd_8_variance64x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance64x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance64x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance64x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance64x128 aom_highbd_8_variance64x128_c
#endif
  unsigned int aom_highbd_10_variance64x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance64x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_10_variance64x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance64x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance64x128 aom_highbd_10_variance64x128_c
#endif
  unsigned int aom_highbd_12_variance64x128_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance64x128)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance64x128_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance64x128_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance64x128 aom_highbd_12_variance64x128_c
#endif

  unsigned int aom_highbd_8_variance128x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance128x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance128x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance128x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance128x64 aom_highbd_8_variance64x128_c
#endif
  unsigned int aom_highbd_10_variance128x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance128x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_10_variance128x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance128x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance128x64 aom_highbd_10_variance128x64_c
#endif
  unsigned int aom_highbd_12_variance128x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance128x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance128x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance128x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance128x64 aom_highbd_12_variance128x64_c
#endif

  unsigned int aom_highbd_8_variance4x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance4x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance4x4 aom_highbd_8_variance4x4_c
#endif
  unsigned int aom_highbd_10_variance4x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance4x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance4x4 aom_highbd_10_variance4x4_c
#endif
  unsigned int aom_highbd_12_variance4x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance4x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance4x4 aom_highbd_12_variance4x4_c
#endif

  unsigned int aom_highbd_8_variance4x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance4x8 aom_highbd_8_variance4x8_c
  unsigned int aom_highbd_10_variance4x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance4x8 aom_highbd_10_variance4x8_c
  unsigned int aom_highbd_12_variance4x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance4x8 aom_highbd_12_variance4x8_c

  unsigned int aom_highbd_8_variance8x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance8x4 aom_highbd_8_variance8x4_c
  unsigned int aom_highbd_10_variance8x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance8x4 aom_highbd_10_variance8x4_c
  unsigned int aom_highbd_12_variance8x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance8x4 aom_highbd_12_variance8x4_c

  unsigned int aom_highbd_8_variance4x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance4x16 aom_highbd_8_variance4x16_c
  unsigned int aom_highbd_10_variance4x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance4x16 aom_highbd_10_variance4x16_c
  unsigned int aom_highbd_12_variance4x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance4x16 aom_highbd_12_variance4x16_c

  unsigned int aom_highbd_8_variance16x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance16x4 aom_highbd_8_variance16x4_c
  unsigned int aom_highbd_10_variance16x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance16x4 aom_highbd_10_variance16x4_c
  unsigned int aom_highbd_12_variance16x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance16x4 aom_highbd_12_variance16x4_c

  unsigned int aom_highbd_8_variance4x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance4x32 aom_highbd_8_variance4x32_c
  unsigned int aom_highbd_10_variance4x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance4x32 aom_highbd_10_variance4x32_c
  unsigned int aom_highbd_12_variance4x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance4x32 aom_highbd_12_variance4x32_c

  unsigned int aom_highbd_8_variance4x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_8_variance4x64 aom_highbd_8_variance4x64_c
  unsigned int aom_highbd_10_variance4x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_10_variance4x64 aom_highbd_10_variance4x64_c
  unsigned int aom_highbd_12_variance4x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  #define aom_highbd_12_variance4x64 aom_highbd_12_variance4x64_c

  unsigned int aom_highbd_8_variance8x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance8x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance8x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance8x8 aom_highbd_8_variance8x8_c
#endif
  unsigned int aom_highbd_10_variance8x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance8x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance8x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance8x8 aom_highbd_10_variance8x8_c
#endif
  unsigned int aom_highbd_12_variance8x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance8x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance8x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance8x8 aom_highbd_12_variance8x8_c
#endif

  unsigned int aom_highbd_8_variance8x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance8x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance8x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance8x16 aom_highbd_8_variance8x16_c
#endif
  unsigned int aom_highbd_10_variance8x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance8x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance8x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance8x16 aom_highbd_10_variance8x16_c
#endif
  unsigned int aom_highbd_12_variance8x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance8x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance8x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance8x16 aom_highbd_12_variance8x16_c
#endif

  unsigned int aom_highbd_8_variance16x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance16x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance16x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance16x8 aom_highbd_8_variance16x8_c
#endif
  unsigned int aom_highbd_10_variance16x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance16x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance16x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance16x8 aom_highbd_10_variance16x8_c
#endif
  unsigned int aom_highbd_12_variance16x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance16x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance16x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance16x8 aom_highbd_12_variance16x8_c
#endif

  unsigned int aom_highbd_8_variance16x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance16x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance16x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance16x16 aom_highbd_8_variance16x16_c
#endif
  unsigned int aom_highbd_10_variance16x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance16x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance16x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance16x16 aom_highbd_10_variance16x16_c
#endif
  unsigned int aom_highbd_12_variance16x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance16x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance16x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance16x16 aom_highbd_12_variance16x16_c
#endif

  unsigned int aom_highbd_8_variance32x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance32x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance32x4_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance32x4 aom_highbd_8_variance32x4_c
#endif
  unsigned int aom_highbd_10_variance32x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance32x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance32x4_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance32x4 aom_highbd_10_variance32x4_c
#endif
  unsigned int aom_highbd_12_variance32x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance32x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance32x4_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance32x4 aom_highbd_12_variance32x4_c
#endif

  unsigned int aom_highbd_8_variance8x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance8x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance8x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance8x32 aom_highbd_8_variance8x32_c
#endif
  unsigned int aom_highbd_10_variance8x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance8x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance8x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance8x32 aom_highbd_10_variance8x32_c
#endif
  unsigned int aom_highbd_12_variance8x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance8x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance8x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance8x32 aom_highbd_12_variance8x32_c
#endif

  unsigned int aom_highbd_8_variance32x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance32x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance32x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance32x8 aom_highbd_8_variance32x8_c
#endif
  unsigned int aom_highbd_10_variance32x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance32x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance32x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance32x8 aom_highbd_10_variance32x8_c
#endif
  unsigned int aom_highbd_12_variance32x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance32x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance32x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance32x8 aom_highbd_12_variance32x8_c
#endif

  unsigned int aom_highbd_8_variance16x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance16x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance16x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance16x32 aom_highbd_8_variance16x32_c
#endif
  unsigned int aom_highbd_10_variance16x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance16x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance16x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance16x32 aom_highbd_10_variance16x32_c
#endif
  unsigned int aom_highbd_12_variance16x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance16x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance16x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance16x32 aom_highbd_12_variance16x32_c
#endif

  unsigned int aom_highbd_8_variance32x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance32x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance32x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance32x16 aom_highbd_8_variance32x16_c
#endif
  unsigned int aom_highbd_10_variance32x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance32x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance32x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance32x16 aom_highbd_10_variance32x16_c
#endif
  unsigned int aom_highbd_12_variance32x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance32x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance32x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance32x16 aom_highbd_12_variance32x16_c
#endif

  unsigned int aom_highbd_8_variance32x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance32x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance32x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance32x32 aom_highbd_8_variance32x32_c
#endif
  unsigned int aom_highbd_10_variance32x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance32x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance32x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance32x32 aom_highbd_10_variance32x32_c
#endif
  unsigned int aom_highbd_12_variance32x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance32x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance32x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance32x32 aom_highbd_12_variance32x32_c
#endif

  unsigned int aom_highbd_8_variance16x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance16x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance16x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance16x64 aom_highbd_8_variance16x64_c
#endif
  unsigned int aom_highbd_10_variance16x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance16x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance16x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance16x64 aom_highbd_10_variance16x64_c
#endif
  unsigned int aom_highbd_12_variance16x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance16x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance16x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance16x64 aom_highbd_12_variance16x64_c
#endif

  unsigned int aom_highbd_8_variance64x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance64x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance64x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance64x16 aom_highbd_8_variance64x16_c
#endif
  unsigned int aom_highbd_10_variance64x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance64x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance64x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance64x16 aom_highbd_10_variance64x16_c
#endif
  unsigned int aom_highbd_12_variance64x16_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance64x16)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance64x16_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance64x16 aom_highbd_12_variance64x16_c
#endif

#if CONFIG_FLEX_PARTITION
  unsigned int aom_highbd_8_variance64x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance64x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_8_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance64x4_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance64x4 aom_highbd_8_variance64x4_c
#endif
  unsigned int aom_highbd_10_variance64x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance64x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_10_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance64x4_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance64x4 aom_highbd_10_variance64x4_c
#endif
  unsigned int aom_highbd_12_variance64x4_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance64x4)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  //unsigned int aom_highbd_12_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance64x4_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance64x4 aom_highbd_12_variance64x4_c
#endif

  unsigned int aom_highbd_8_variance64x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance64x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance64x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance64x8 aom_highbd_8_variance64x8_c
#endif
  unsigned int aom_highbd_10_variance64x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance64x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance64x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance64x8 aom_highbd_10_variance64x8_c
#endif
  unsigned int aom_highbd_12_variance64x8_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance64x8)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance64x8_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance64x8 aom_highbd_12_variance64x8_c
#endif

  unsigned int aom_highbd_8_variance8x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance8x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance8x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance8x64 aom_highbd_8_variance8x64_c
#endif
  unsigned int aom_highbd_10_variance8x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance8x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance8x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance8x64 aom_highbd_10_variance8x64_c
#endif
  unsigned int aom_highbd_12_variance8x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance8x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance8x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance8x64 aom_highbd_12_variance8x64_c
#endif
#endif

  unsigned int aom_highbd_8_variance32x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance32x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance32x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance32x64 aom_highbd_8_variance32x64_c
#endif
  unsigned int aom_highbd_10_variance32x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance32x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance32x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance32x64 aom_highbd_10_variance32x64_c
#endif
  unsigned int aom_highbd_12_variance32x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance32x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance32x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance32x64 aom_highbd_12_variance32x64_c
#endif

  unsigned int aom_highbd_8_variance64x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance64x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance64x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance64x32 aom_highbd_8_variance64x32_c
#endif
  unsigned int aom_highbd_10_variance64x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance64x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance64x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance64x32 aom_highbd_10_variance64x32_c
#endif
  unsigned int aom_highbd_12_variance64x32_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance64x32)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance64x32_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance64x32 aom_highbd_12_variance64x32_c
#endif

  unsigned int aom_highbd_8_variance64x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_8_variance64x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_8_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_8_variance64x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_8_variance64x64 aom_highbd_8_variance64x64_c
#endif
  unsigned int aom_highbd_10_variance64x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_10_variance64x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_10_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_10_variance64x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_10_variance64x64 aom_highbd_10_variance64x64_c
#endif
  unsigned int aom_highbd_12_variance64x64_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if HAVE_SIMD
  RTCD_EXTERN unsigned int (*aom_highbd_12_variance64x64)(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
  unsigned int aom_highbd_12_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#if defined(__AVX2__)
  unsigned int aom_highbd_12_variance64x64_avx2(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse);
#endif
#else
  #define aom_highbd_12_variance64x64 aom_highbd_12_variance64x64_c
#endif

#if CONFIG_BLOCK_256
  uint32_t aom_highbd_8_sub_pixel_variance256x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance256x256)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_8_sub_pixel_variance256x256_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance256x256_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance256x256 aom_highbd_8_sub_pixel_variance256x256_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance256x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance256x256)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_10_sub_pixel_variance256x256_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance256x256_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance256x256 aom_highbd_10_sub_pixel_variance256x256_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance256x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance256x256)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_12_sub_pixel_variance256x256_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance256x256_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance256x256 aom_highbd_12_sub_pixel_variance256x256_c
#endif
  uint32_t aom_highbd_8_sub_pixel_variance256x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance256x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_8_sub_pixel_variance256x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance256x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance256x128 aom_highbd_8_sub_pixel_variance256x128_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance256x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance256x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_10_sub_pixel_variance256x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance256x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance256x128 aom_highbd_10_sub_pixel_variance256x128_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance256x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance256x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_12_sub_pixel_variance256x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance256x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance256x128 aom_highbd_12_sub_pixel_variance256x128_c
#endif
  uint32_t aom_highbd_8_sub_pixel_variance128x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance128x256)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_8_sub_pixel_variance128x256_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance128x256_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance128x256 aom_highbd_8_sub_pixel_variance128x256_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance128x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance128x256)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_10_sub_pixel_variance128x256_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance128x256_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance128x256 aom_highbd_10_sub_pixel_variance128x256_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance128x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance128x256)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_12_sub_pixel_variance128x256_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance128x256_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance128x256 aom_highbd_12_sub_pixel_variance128x256_c
#endif
#endif
  uint32_t aom_highbd_8_sub_pixel_variance128x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance128x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_8_sub_pixel_variance128x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance128x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance128x128 aom_highbd_8_sub_pixel_variance128x128_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance128x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance128x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_10_sub_pixel_variance128x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance128x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance128x128 aom_highbd_10_sub_pixel_variance128x128_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance128x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance128x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_12_sub_pixel_variance128x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance128x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance128x128 aom_highbd_12_sub_pixel_variance128x128_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance64x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance64x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_8_sub_pixel_variance64x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance64x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance64x128 aom_highbd_8_sub_pixel_variance64x128_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance64x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance64x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_10_sub_pixel_variance64x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance64x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance64x128 aom_highbd_10_sub_pixel_variance64x128_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance64x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance64x128)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_12_sub_pixel_variance64x128_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance64x128_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance64x128 aom_highbd_12_sub_pixel_variance64x128_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance128x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance128x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_8_sub_pixel_variance128x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance128x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance128x64 aom_highbd_8_sub_pixel_variance64x128_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance128x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance128x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_10_sub_pixel_variance128x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance128x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance128x64 aom_highbd_10_sub_pixel_variance128x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance128x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance128x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  //uint32_t aom_highbd_12_sub_pixel_variance128x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance128x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance128x64 aom_highbd_12_sub_pixel_variance128x64_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance4x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance4x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance4x4 aom_highbd_8_sub_pixel_variance4x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance4x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance4x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance4x4 aom_highbd_10_sub_pixel_variance4x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance4x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance4x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance4x4 aom_highbd_12_sub_pixel_variance4x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance4x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_8_sub_pixel_variance4x8 aom_highbd_8_sub_pixel_variance4x8_c
  uint32_t aom_highbd_10_sub_pixel_variance4x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_10_sub_pixel_variance4x8 aom_highbd_10_sub_pixel_variance4x8_c
  uint32_t aom_highbd_12_sub_pixel_variance4x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_12_sub_pixel_variance4x8 aom_highbd_12_sub_pixel_variance4x8_c

  uint32_t aom_highbd_8_sub_pixel_variance8x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_variance8x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_8_sub_pixel_variance8x4 aom_highbd_8_sub_pixel_variance8x4_sse2
#else
  #define aom_highbd_8_sub_pixel_variance8x4 aom_highbd_8_sub_pixel_variance8x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance8x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_variance8x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_10_sub_pixel_variance8x4 aom_highbd_10_sub_pixel_variance8x4_sse2
#else
  #define aom_highbd_10_sub_pixel_variance8x4 aom_highbd_10_sub_pixel_variance8x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance8x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_variance8x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_12_sub_pixel_variance8x4 aom_highbd_12_sub_pixel_variance8x4_sse2
#else
  #define aom_highbd_12_sub_pixel_variance8x4 aom_highbd_12_sub_pixel_variance8x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance4x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_8_sub_pixel_variance4x16 aom_highbd_8_sub_pixel_variance4x16_c
  uint32_t aom_highbd_10_sub_pixel_variance4x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_10_sub_pixel_variance4x16 aom_highbd_10_sub_pixel_variance4x16_c
  uint32_t aom_highbd_12_sub_pixel_variance4x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_12_sub_pixel_variance4x16 aom_highbd_12_sub_pixel_variance4x16_c

  uint32_t aom_highbd_8_sub_pixel_variance4x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_8_sub_pixel_variance4x32 aom_highbd_8_sub_pixel_variance4x32_c
  uint32_t aom_highbd_10_sub_pixel_variance4x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_10_sub_pixel_variance4x32 aom_highbd_10_sub_pixel_variance4x32_c
  uint32_t aom_highbd_12_sub_pixel_variance4x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_12_sub_pixel_variance4x32 aom_highbd_12_sub_pixel_variance4x32_c

  uint32_t aom_highbd_8_sub_pixel_variance4x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_8_sub_pixel_variance4x64 aom_highbd_8_sub_pixel_variance4x64_c
  uint32_t aom_highbd_10_sub_pixel_variance4x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_10_sub_pixel_variance4x64 aom_highbd_10_sub_pixel_variance4x64_c
  uint32_t aom_highbd_12_sub_pixel_variance4x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_12_sub_pixel_variance4x64 aom_highbd_12_sub_pixel_variance4x64_c

  uint32_t aom_highbd_8_sub_pixel_variance16x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance16x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_8_sub_pixel_variance16x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance16x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance16x4 aom_highbd_8_sub_pixel_variance16x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance16x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance16x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_10_sub_pixel_variance16x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance16x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance16x4 aom_highbd_10_sub_pixel_variance16x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance16x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance16x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_12_sub_pixel_variance16x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance16x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance16x4 aom_highbd_12_sub_pixel_variance16x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance8x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_8_sub_pixel_variance8x8 aom_highbd_8_sub_pixel_variance8x8_sse2
#else
  #define aom_highbd_8_sub_pixel_variance8x8 aom_highbd_8_sub_pixel_variance8x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance8x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_10_sub_pixel_variance8x8 aom_highbd_10_sub_pixel_variance8x8_sse2
#else
  #define aom_highbd_10_sub_pixel_variance8x8 aom_highbd_10_sub_pixel_variance8x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance8x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_12_sub_pixel_variance8x8 aom_highbd_12_sub_pixel_variance8x8_sse2
#else
  #define aom_highbd_12_sub_pixel_variance8x8 aom_highbd_12_sub_pixel_variance8x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance8x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_8_sub_pixel_variance8x16 aom_highbd_8_sub_pixel_variance8x16_sse2
#else
  #define aom_highbd_8_sub_pixel_variance8x16 aom_highbd_8_sub_pixel_variance8x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance8x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_10_sub_pixel_variance8x16 aom_highbd_10_sub_pixel_variance8x16_sse2
#else
  #define aom_highbd_10_sub_pixel_variance8x16 aom_highbd_10_sub_pixel_variance8x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance8x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  #define aom_highbd_12_sub_pixel_variance8x16 aom_highbd_12_sub_pixel_variance8x16_sse2
#else
  #define aom_highbd_12_sub_pixel_variance8x16 aom_highbd_12_sub_pixel_variance8x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance16x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance16x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_8_sub_pixel_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance16x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance16x8 aom_highbd_8_sub_pixel_variance16x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance16x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance16x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_10_sub_pixel_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance16x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance16x8 aom_highbd_10_sub_pixel_variance16x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance16x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance16x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_12_sub_pixel_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance16x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance16x8 aom_highbd_12_sub_pixel_variance16x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance16x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance16x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__) && CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_8_sub_pixel_variance16x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance16x16 aom_highbd_8_sub_pixel_variance16x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance16x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance16x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__) && CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_10_sub_pixel_variance16x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance16x16 aom_highbd_10_sub_pixel_variance16x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance16x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance16x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__) && CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_12_sub_pixel_variance16x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance16x16 aom_highbd_12_sub_pixel_variance16x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance32x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance32x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance32x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance32x4 aom_highbd_8_sub_pixel_variance32x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance32x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance32x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance32x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance32x4 aom_highbd_10_sub_pixel_variance32x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance32x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance32x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance32x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance32x4 aom_highbd_12_sub_pixel_variance32x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance8x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance8x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance8x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance8x32 aom_highbd_8_sub_pixel_variance8x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance8x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance8x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance8x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance8x32 aom_highbd_10_sub_pixel_variance8x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance8x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance8x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance8x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance8x32 aom_highbd_12_sub_pixel_variance8x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance32x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance32x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance32x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance32x8 aom_highbd_8_sub_pixel_variance32x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance32x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance32x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance32x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance32x8 aom_highbd_10_sub_pixel_variance32x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance32x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance32x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance32x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance32x8 aom_highbd_12_sub_pixel_variance32x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance8x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance8x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance8x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance8x64 aom_highbd_8_sub_pixel_variance8x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance8x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance8x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance8x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance8x64 aom_highbd_10_sub_pixel_variance8x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance8x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance8x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance8x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance8x64 aom_highbd_12_sub_pixel_variance8x64_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance16x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance16x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_8_sub_pixel_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance16x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance16x32 aom_highbd_8_sub_pixel_variance16x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance16x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance16x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_10_sub_pixel_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance16x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance16x32 aom_highbd_10_sub_pixel_variance16x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance16x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance16x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_12_sub_pixel_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance16x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance16x32 aom_highbd_12_sub_pixel_variance16x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance32x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance32x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance32x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance32x16 aom_highbd_8_sub_pixel_variance32x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance32x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance32x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance32x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance32x16 aom_highbd_10_sub_pixel_variance32x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance32x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance32x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance32x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance32x16 aom_highbd_12_sub_pixel_variance32x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance32x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance32x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance32x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance32x32 aom_highbd_8_sub_pixel_variance32x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance32x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance32x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance32x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance32x32 aom_highbd_10_sub_pixel_variance32x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance32x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance32x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance32x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance32x32 aom_highbd_12_sub_pixel_variance32x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance16x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance16x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_8_sub_pixel_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance16x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance16x64 aom_highbd_8_sub_pixel_variance16x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance16x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance16x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_10_sub_pixel_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance16x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance16x64 aom_highbd_10_sub_pixel_variance16x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance16x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance16x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_12_sub_pixel_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance16x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance16x64 aom_highbd_12_sub_pixel_variance16x64_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance64x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance64x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance64x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance64x16 aom_highbd_8_sub_pixel_variance64x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance64x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance64x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance64x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance64x16 aom_highbd_10_sub_pixel_variance64x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance64x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance64x16)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance64x16_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance64x16 aom_highbd_12_sub_pixel_variance64x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance64x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance64x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance64x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance64x8 aom_highbd_8_sub_pixel_variance64x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance64x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance64x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance64x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance64x8 aom_highbd_10_sub_pixel_variance64x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance64x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance64x8)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance64x8_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance64x8 aom_highbd_12_sub_pixel_variance64x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance64x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance64x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance64x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance64x4 aom_highbd_8_sub_pixel_variance64x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance64x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance64x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance64x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance64x4 aom_highbd_10_sub_pixel_variance64x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance64x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance64x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance64x4_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance64x4 aom_highbd_12_sub_pixel_variance64x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance32x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance32x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance32x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance32x64 aom_highbd_8_sub_pixel_variance32x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance32x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance32x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance32x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance32x64 aom_highbd_10_sub_pixel_variance32x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance32x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance32x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance32x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance32x64 aom_highbd_12_sub_pixel_variance32x64_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance64x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance64x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance64x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance64x32 aom_highbd_8_sub_pixel_variance64x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance64x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance64x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance64x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance64x32 aom_highbd_10_sub_pixel_variance64x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance64x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance64x32)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance64x32_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance64x32 aom_highbd_12_sub_pixel_variance64x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_variance64x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_variance64x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_8_sub_pixel_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_8_sub_pixel_variance64x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_8_sub_pixel_variance64x64 aom_highbd_8_sub_pixel_variance64x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_variance64x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_variance64x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_10_sub_pixel_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_10_sub_pixel_variance64x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_10_sub_pixel_variance64x64 aom_highbd_10_sub_pixel_variance64x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_variance64x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if HAVE_SIMD
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_variance64x64)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
  uint32_t aom_highbd_12_sub_pixel_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#if defined(__AVX2__)
  uint32_t aom_highbd_12_sub_pixel_variance64x64_avx2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse);
#endif
#else
  #define aom_highbd_12_sub_pixel_variance64x64 aom_highbd_12_sub_pixel_variance64x64_c
#endif

#if CONFIG_BLOCK_256
  uint32_t aom_highbd_8_sub_pixel_avg_variance256x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance256x256 aom_highbd_8_sub_pixel_avg_variance256x256_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance256x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance256x256 aom_highbd_10_sub_pixel_avg_variance256x256_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance256x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance256x256 aom_highbd_12_sub_pixel_avg_variance256x256_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance128x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance128x256 aom_highbd_8_sub_pixel_avg_variance128x256_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance128x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance128x256 aom_highbd_10_sub_pixel_avg_variance128x256_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance128x256_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance128x256 aom_highbd_12_sub_pixel_avg_variance128x256_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance256x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance256x128 aom_highbd_8_sub_pixel_avg_variance128x256_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance256x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance256x128 aom_highbd_10_sub_pixel_avg_variance256x128_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance256x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance256x128 aom_highbd_12_sub_pixel_avg_variance256x128_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance128x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance128x128 aom_highbd_8_sub_pixel_avg_variance128x128_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance128x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance128x128 aom_highbd_10_sub_pixel_avg_variance128x128_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance128x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance128x128 aom_highbd_12_sub_pixel_avg_variance128x128_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance64x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance64x128 aom_highbd_8_sub_pixel_avg_variance64x128_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance64x128 aom_highbd_10_sub_pixel_avg_variance64x128_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x128_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance64x128 aom_highbd_12_sub_pixel_avg_variance64x128_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance128x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance128x64 aom_highbd_8_sub_pixel_avg_variance64x128_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance128x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance128x64 aom_highbd_10_sub_pixel_avg_variance128x64_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance128x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance128x64 aom_highbd_12_sub_pixel_avg_variance128x64_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance4x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN uint32_t (*aom_highbd_8_sub_pixel_avg_variance4x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  uint32_t aom_highbd_8_sub_pixel_avg_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_8_sub_pixel_avg_variance4x4 aom_highbd_8_sub_pixel_avg_variance4x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance4x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN uint32_t (*aom_highbd_10_sub_pixel_avg_variance4x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  uint32_t aom_highbd_10_sub_pixel_avg_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_10_sub_pixel_avg_variance4x4 aom_highbd_10_sub_pixel_avg_variance4x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance4x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN uint32_t (*aom_highbd_12_sub_pixel_avg_variance4x4)(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  uint32_t aom_highbd_12_sub_pixel_avg_variance4x4_sse4_1(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#endif
#else
  #define aom_highbd_12_sub_pixel_avg_variance4x4 aom_highbd_12_sub_pixel_avg_variance4x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance4x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance4x8 aom_highbd_8_sub_pixel_avg_variance4x8_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance4x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance4x8 aom_highbd_10_sub_pixel_avg_variance4x8_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance4x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance4x8 aom_highbd_12_sub_pixel_avg_variance4x8_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance8x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance8x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance8x4 aom_highbd_8_sub_pixel_avg_variance8x4_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance8x4 aom_highbd_8_sub_pixel_avg_variance8x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance8x4 aom_highbd_10_sub_pixel_avg_variance8x4_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance8x4 aom_highbd_10_sub_pixel_avg_variance8x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance8x4 aom_highbd_12_sub_pixel_avg_variance8x4_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance8x4 aom_highbd_12_sub_pixel_avg_variance8x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance4x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance4x16 aom_highbd_8_sub_pixel_avg_variance4x16_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance4x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance4x16 aom_highbd_10_sub_pixel_avg_variance4x16_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance4x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance4x16 aom_highbd_12_sub_pixel_avg_variance4x16_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance16x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_8_sub_pixel_avg_variance16x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance16x4 aom_highbd_8_sub_pixel_avg_variance16x4_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance16x4 aom_highbd_8_sub_pixel_avg_variance16x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance16x4 aom_highbd_10_sub_pixel_avg_variance16x4_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance16x4 aom_highbd_10_sub_pixel_avg_variance16x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance16x4 aom_highbd_12_sub_pixel_avg_variance16x4_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance16x4 aom_highbd_12_sub_pixel_avg_variance16x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance4x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance4x32 aom_highbd_8_sub_pixel_avg_variance4x32_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance4x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance4x32 aom_highbd_10_sub_pixel_avg_variance4x32_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance4x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance4x32 aom_highbd_12_sub_pixel_avg_variance4x32_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance4x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance4x64 aom_highbd_8_sub_pixel_avg_variance4x64_c
  uint32_t aom_highbd_10_sub_pixel_avg_variance4x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance4x64 aom_highbd_10_sub_pixel_avg_variance4x64_c
  uint32_t aom_highbd_12_sub_pixel_avg_variance4x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance4x64 aom_highbd_12_sub_pixel_avg_variance4x64_c

  uint32_t aom_highbd_8_sub_pixel_avg_variance8x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance8x8 aom_highbd_8_sub_pixel_avg_variance8x8_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance8x8 aom_highbd_8_sub_pixel_avg_variance8x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance8x8 aom_highbd_10_sub_pixel_avg_variance8x8_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance8x8 aom_highbd_10_sub_pixel_avg_variance8x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance8x8 aom_highbd_12_sub_pixel_avg_variance8x8_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance8x8 aom_highbd_12_sub_pixel_avg_variance8x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance8x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance8x16 aom_highbd_8_sub_pixel_avg_variance8x16_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance8x16 aom_highbd_8_sub_pixel_avg_variance8x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance8x16 aom_highbd_10_sub_pixel_avg_variance8x16_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance8x16 aom_highbd_10_sub_pixel_avg_variance8x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance8x16 aom_highbd_12_sub_pixel_avg_variance8x16_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance8x16 aom_highbd_12_sub_pixel_avg_variance8x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance16x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_8_sub_pixel_avg_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance16x8 aom_highbd_8_sub_pixel_avg_variance16x8_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance16x8 aom_highbd_8_sub_pixel_avg_variance16x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance16x8 aom_highbd_10_sub_pixel_avg_variance16x8_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance16x8 aom_highbd_10_sub_pixel_avg_variance16x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance16x8 aom_highbd_12_sub_pixel_avg_variance16x8_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance16x8 aom_highbd_12_sub_pixel_avg_variance16x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance16x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_8_sub_pixel_avg_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance16x16 aom_highbd_8_sub_pixel_avg_variance16x16_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance16x16 aom_highbd_8_sub_pixel_avg_variance16x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance16x16 aom_highbd_10_sub_pixel_avg_variance16x16_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance16x16 aom_highbd_10_sub_pixel_avg_variance16x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_UNEVEN_4WAY
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance16x16 aom_highbd_12_sub_pixel_avg_variance16x16_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance16x16 aom_highbd_12_sub_pixel_avg_variance16x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance32x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance32x4 aom_highbd_8_sub_pixel_avg_variance32x4_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance32x4 aom_highbd_8_sub_pixel_avg_variance32x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance32x4 aom_highbd_10_sub_pixel_avg_variance32x4_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance32x4 aom_highbd_10_sub_pixel_avg_variance32x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance32x4 aom_highbd_12_sub_pixel_avg_variance32x4_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance32x4 aom_highbd_12_sub_pixel_avg_variance32x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance8x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance8x32 aom_highbd_8_sub_pixel_avg_variance8x32_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance8x32 aom_highbd_8_sub_pixel_avg_variance8x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance8x32 aom_highbd_10_sub_pixel_avg_variance8x32_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance8x32 aom_highbd_10_sub_pixel_avg_variance8x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance8x32 aom_highbd_12_sub_pixel_avg_variance8x32_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance8x32 aom_highbd_12_sub_pixel_avg_variance8x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance32x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance32x8 aom_highbd_8_sub_pixel_avg_variance32x8_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance32x8 aom_highbd_8_sub_pixel_avg_variance32x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance32x8 aom_highbd_10_sub_pixel_avg_variance32x8_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance32x8 aom_highbd_10_sub_pixel_avg_variance32x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance32x8 aom_highbd_12_sub_pixel_avg_variance32x8_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance32x8 aom_highbd_12_sub_pixel_avg_variance32x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance8x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance8x64 aom_highbd_8_sub_pixel_avg_variance8x64_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance8x64 aom_highbd_8_sub_pixel_avg_variance8x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance8x64 aom_highbd_10_sub_pixel_avg_variance8x64_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance8x64 aom_highbd_10_sub_pixel_avg_variance8x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance8x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance8x64 aom_highbd_12_sub_pixel_avg_variance8x64_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance8x64 aom_highbd_12_sub_pixel_avg_variance8x64_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance16x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_8_sub_pixel_avg_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance16x32 aom_highbd_8_sub_pixel_avg_variance16x32_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance16x32 aom_highbd_8_sub_pixel_avg_variance16x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance16x32 aom_highbd_10_sub_pixel_avg_variance16x32_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance16x32 aom_highbd_10_sub_pixel_avg_variance16x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance16x32 aom_highbd_12_sub_pixel_avg_variance16x32_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance16x32 aom_highbd_12_sub_pixel_avg_variance16x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance32x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance32x16 aom_highbd_8_sub_pixel_avg_variance32x16_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance32x16 aom_highbd_8_sub_pixel_avg_variance32x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance32x16 aom_highbd_10_sub_pixel_avg_variance32x16_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance32x16 aom_highbd_10_sub_pixel_avg_variance32x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance32x16 aom_highbd_12_sub_pixel_avg_variance32x16_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance32x16 aom_highbd_12_sub_pixel_avg_variance32x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance32x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance32x32 aom_highbd_8_sub_pixel_avg_variance32x32_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance32x32 aom_highbd_8_sub_pixel_avg_variance32x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance32x32 aom_highbd_10_sub_pixel_avg_variance32x32_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance32x32 aom_highbd_10_sub_pixel_avg_variance32x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance32x32 aom_highbd_12_sub_pixel_avg_variance32x32_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance32x32 aom_highbd_12_sub_pixel_avg_variance32x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance16x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_8_sub_pixel_avg_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance16x64 aom_highbd_8_sub_pixel_avg_variance16x64_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance16x64 aom_highbd_8_sub_pixel_avg_variance16x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_10_sub_pixel_avg_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance16x64 aom_highbd_10_sub_pixel_avg_variance16x64_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance16x64 aom_highbd_10_sub_pixel_avg_variance16x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD && !CONFIG_FLEX_PARTITION
  uint32_t aom_highbd_12_sub_pixel_avg_variance16x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance16x64 aom_highbd_12_sub_pixel_avg_variance16x64_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance16x64 aom_highbd_12_sub_pixel_avg_variance16x64_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance64x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance64x16 aom_highbd_8_sub_pixel_avg_variance64x16_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance64x16 aom_highbd_8_sub_pixel_avg_variance64x16_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance64x16 aom_highbd_10_sub_pixel_avg_variance64x16_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance64x16 aom_highbd_10_sub_pixel_avg_variance64x16_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x16_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x16_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance64x16 aom_highbd_12_sub_pixel_avg_variance64x16_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance64x16 aom_highbd_12_sub_pixel_avg_variance64x16_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance64x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance64x8 aom_highbd_8_sub_pixel_avg_variance64x8_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance64x8 aom_highbd_8_sub_pixel_avg_variance64x8_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance64x8 aom_highbd_10_sub_pixel_avg_variance64x8_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance64x8 aom_highbd_10_sub_pixel_avg_variance64x8_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x8_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x8_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance64x8 aom_highbd_12_sub_pixel_avg_variance64x8_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance64x8 aom_highbd_12_sub_pixel_avg_variance64x8_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance64x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance64x4 aom_highbd_8_sub_pixel_avg_variance64x4_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance64x4 aom_highbd_8_sub_pixel_avg_variance64x4_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance64x4 aom_highbd_10_sub_pixel_avg_variance64x4_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance64x4 aom_highbd_10_sub_pixel_avg_variance64x4_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x4_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x4_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance64x4 aom_highbd_12_sub_pixel_avg_variance64x4_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance64x4 aom_highbd_12_sub_pixel_avg_variance64x4_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance32x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance32x64 aom_highbd_8_sub_pixel_avg_variance32x64_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance32x64 aom_highbd_8_sub_pixel_avg_variance32x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance32x64 aom_highbd_10_sub_pixel_avg_variance32x64_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance32x64 aom_highbd_10_sub_pixel_avg_variance32x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance32x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance32x64 aom_highbd_12_sub_pixel_avg_variance32x64_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance32x64 aom_highbd_12_sub_pixel_avg_variance32x64_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance64x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance64x32 aom_highbd_8_sub_pixel_avg_variance64x32_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance64x32 aom_highbd_8_sub_pixel_avg_variance64x32_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance64x32 aom_highbd_10_sub_pixel_avg_variance64x32_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance64x32 aom_highbd_10_sub_pixel_avg_variance64x32_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x32_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x32_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance64x32 aom_highbd_12_sub_pixel_avg_variance64x32_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance64x32 aom_highbd_12_sub_pixel_avg_variance64x32_c
#endif

  uint32_t aom_highbd_8_sub_pixel_avg_variance64x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_8_sub_pixel_avg_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_8_sub_pixel_avg_variance64x64 aom_highbd_8_sub_pixel_avg_variance64x64_sse2
#else
  #define aom_highbd_8_sub_pixel_avg_variance64x64 aom_highbd_8_sub_pixel_avg_variance64x64_c
#endif
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_10_sub_pixel_avg_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_10_sub_pixel_avg_variance64x64 aom_highbd_10_sub_pixel_avg_variance64x64_sse2
#else
  #define aom_highbd_10_sub_pixel_avg_variance64x64 aom_highbd_10_sub_pixel_avg_variance64x64_c
#endif
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x64_c(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
#if HAVE_SIMD
  uint32_t aom_highbd_12_sub_pixel_avg_variance64x64_sse2(const uint16_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint16_t *ref_ptr, int ref_stride, uint32_t *sse, const uint16_t *second_pred);
  #define aom_highbd_12_sub_pixel_avg_variance64x64 aom_highbd_12_sub_pixel_avg_variance64x64_sse2
#else
  #define aom_highbd_12_sub_pixel_avg_variance64x64 aom_highbd_12_sub_pixel_avg_variance64x64_c
#endif

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance256x256_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance256x256 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance256x256_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance256x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance256x128 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance256x128_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x256_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x256 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x256_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x128 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x128_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x64 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance128x64_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x16 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x16_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x32 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x32_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x4 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x4_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x64 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x64_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x8 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance16x8_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x16 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x16_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x32 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x32_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x64 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x64_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x4 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x4_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x8 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance32x8_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x64 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x64_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x32 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x32_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x16 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x16_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x4 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x4_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x8 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance4x8_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x128 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x128_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x4 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x4_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x8 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x8_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x16 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x16_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x32 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x32_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x64 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance64x64_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x16 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x16_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x32 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x32_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x64 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x64_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x4 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x4_c

uint32_t aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x8 \
  aom_highbd_8_dist_wtd_sub_pixel_avg_variance8x8_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance256x256_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance256x256 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance256x256_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance256x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance256x128 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance256x128_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x256_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x256 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x256_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x128 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x128_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x64 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance128x64_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x16 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x16_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x32 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x32_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x4 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x4_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x64 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x64_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x8 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance16x8_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x16 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x16_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x32 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x32_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x64 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x64_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x8 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x8_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x4 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance32x4_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x32 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x32_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x64 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x64_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x16 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x16_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x4 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x4_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x8 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance4x8_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x128 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x128_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x8 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x8_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x4 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x4_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x16 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x16_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x32 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x32_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x64 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance64x64_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x16 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x16_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x32 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x32_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x64 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x64_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x4 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x4_c

uint32_t aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x8 \
  aom_highbd_10_dist_wtd_sub_pixel_avg_variance8x8_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance256x256_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance256x256 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance256x256_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance256x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance256x128 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance256x128_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x256_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x256 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x256_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x128 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x128_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x64 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance128x64_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x16 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x16_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x32 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x32_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x4 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x4_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x64 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x64_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int  yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x8 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance16x8_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x16 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x16_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x32 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x32_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x64 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x64_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x4 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x4_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x8 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance32x8_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x64 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x64_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x32 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x32_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x16 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x16_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x4 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x4_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x8 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance4x8_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x128_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x128 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x128_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x8 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x8_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x16 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x16_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x32 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x32_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x64 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x64_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x4 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance64x4_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x16_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x16 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x16_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x32_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x32 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x32_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x64_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x64 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x64_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x4_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x4 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x4_c

uint32_t aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x8_c(
    const uint16_t *src_ptr,
    int source_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref_ptr,
    int ref_stride,
    uint32_t *sse,
    const uint16_t *second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x8 \
  aom_highbd_12_dist_wtd_sub_pixel_avg_variance8x8_c

  // Masked Variance / Masked Subpixel Variance
unsigned int aom_highbd_8_masked_sub_pixel_variance256x256_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance256x256_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance256x256)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance256x256 \
  aom_highbd_8_masked_sub_pixel_variance256x256_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance256x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance256x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance256x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance256x128 \
  aom_highbd_8_masked_sub_pixel_variance256x128_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance128x256_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance128x256_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance128x256)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance128x256 \
  aom_highbd_8_masked_sub_pixel_variance128x256_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance128x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance128x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance128x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance128x128 \
  aom_highbd_8_masked_sub_pixel_variance128x128_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance128x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance128x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance128x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance128x64 \
  aom_highbd_8_masked_sub_pixel_variance128x64_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance16x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance16x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance16x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance16x16 \
  aom_highbd_8_masked_sub_pixel_variance16x16_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance16x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance16x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance16x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance16x32 \
  aom_highbd_8_masked_sub_pixel_variance16x32_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance16x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance16x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance16x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance16x4 \
  aom_highbd_8_masked_sub_pixel_variance16x4_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance16x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance16x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance16x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance16x64 \
  aom_highbd_8_masked_sub_pixel_variance16x64_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance16x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance16x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance16x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance16x8 \
  aom_highbd_8_masked_sub_pixel_variance16x8_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance32x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance32x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance32x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance32x16 \
  aom_highbd_8_masked_sub_pixel_variance32x16_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance32x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance32x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance32x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance32x32 \
  aom_highbd_8_masked_sub_pixel_variance32x32_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance32x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance32x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance32x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance32x64 \
  aom_highbd_8_masked_sub_pixel_variance32x64_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance32x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance32x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance32x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance32x8 \
  aom_highbd_8_masked_sub_pixel_variance32x8_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance32x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance32x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance32x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance32x4 \
  aom_highbd_8_masked_sub_pixel_variance32x4_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance4x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance4x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance4x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance4x32 \
  aom_highbd_8_masked_sub_pixel_variance4x32_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance4x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance4x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance4x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance4x16 \
  aom_highbd_8_masked_sub_pixel_variance4x16_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance4x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance4x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance4x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance4x4 \
  aom_highbd_8_masked_sub_pixel_variance4x4_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance4x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance4x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance4x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance4x8 \
  aom_highbd_8_masked_sub_pixel_variance4x8_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance64x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance64x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance64x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance64x128 \
  aom_highbd_8_masked_sub_pixel_variance64x128_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance64x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance64x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance64x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance64x4 \
  aom_highbd_8_masked_sub_pixel_variance64x4_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance4x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance4x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance4x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance4x64 \
  aom_highbd_8_masked_sub_pixel_variance4x64_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance64x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance64x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance64x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance64x8 \
  aom_highbd_8_masked_sub_pixel_variance64x8_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance64x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance64x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance64x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance64x16 \
  aom_highbd_8_masked_sub_pixel_variance64x16_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance64x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance64x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance64x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance64x32 \
  aom_highbd_8_masked_sub_pixel_variance64x32_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance64x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance64x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance64x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance64x64 \
  aom_highbd_8_masked_sub_pixel_variance64x64_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance8x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance8x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance8x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance8x16 \
  aom_highbd_8_masked_sub_pixel_variance8x16_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance8x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance8x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance8x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance8x32 \
  aom_highbd_8_masked_sub_pixel_variance8x32_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance8x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance8x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance8x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance8x64 \
  aom_highbd_8_masked_sub_pixel_variance8x64_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance8x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance8x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance8x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance8x4 \
  aom_highbd_8_masked_sub_pixel_variance8x4_c
#endif

unsigned int aom_highbd_8_masked_sub_pixel_variance8x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_8_masked_sub_pixel_variance8x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_8_masked_sub_pixel_variance8x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_8_masked_sub_pixel_variance8x8 \
  aom_highbd_8_masked_sub_pixel_variance8x8_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance256x256_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance256x256_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance256x256)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance256x256 \
  aom_highbd_10_masked_sub_pixel_variance256x256_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance256x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance256x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance256x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance256x128 \
  aom_highbd_10_masked_sub_pixel_variance256x128_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance128x256_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance128x256_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance128x256)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance128x256 \
  aom_highbd_10_masked_sub_pixel_variance128x256_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance128x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance128x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance128x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance128x128 \
  aom_highbd_10_masked_sub_pixel_variance128x128_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance128x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance128x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance128x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance128x64 \
  aom_highbd_10_masked_sub_pixel_variance128x64_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance16x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance16x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance16x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance16x16 \
  aom_highbd_10_masked_sub_pixel_variance16x16_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance16x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance16x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance16x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance16x32 \
  aom_highbd_10_masked_sub_pixel_variance16x32_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance16x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance16x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance16x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance16x4 \
  aom_highbd_10_masked_sub_pixel_variance16x4_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance16x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance16x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance16x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance16x64 \
  aom_highbd_10_masked_sub_pixel_variance16x64_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance8x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance8x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance8x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance8x64 \
  aom_highbd_10_masked_sub_pixel_variance8x64_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance16x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance16x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance16x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance16x8 \
  aom_highbd_10_masked_sub_pixel_variance16x8_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance32x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance32x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance32x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance32x16 \
  aom_highbd_10_masked_sub_pixel_variance32x16_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance32x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance32x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance32x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance32x32 \
  aom_highbd_10_masked_sub_pixel_variance32x32_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance32x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance32x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance32x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance32x64 \
  aom_highbd_10_masked_sub_pixel_variance32x64_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance32x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance32x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance32x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance32x8 \
  aom_highbd_10_masked_sub_pixel_variance32x8_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance32x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance32x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance32x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance32x4 \
  aom_highbd_10_masked_sub_pixel_variance32x4_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance4x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance4x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance4x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance4x64 \
  aom_highbd_10_masked_sub_pixel_variance4x64_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance4x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance4x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance4x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance4x32 \
  aom_highbd_10_masked_sub_pixel_variance4x32_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance4x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance4x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance4x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance4x16 \
  aom_highbd_10_masked_sub_pixel_variance4x16_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance4x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance4x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance4x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance4x4 \
  aom_highbd_10_masked_sub_pixel_variance4x4_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance4x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance4x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance4x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance4x8 \
  aom_highbd_10_masked_sub_pixel_variance4x8_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance64x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance64x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance64x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance64x128 \
  aom_highbd_10_masked_sub_pixel_variance64x128_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance64x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance64x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance64x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance64x4 \
  aom_highbd_10_masked_sub_pixel_variance64x4_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance64x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance64x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance64x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance64x8 \
  aom_highbd_10_masked_sub_pixel_variance64x8_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance64x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance64x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance64x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance64x16 \
  aom_highbd_10_masked_sub_pixel_variance64x16_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance64x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance64x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance64x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance64x32 \
  aom_highbd_10_masked_sub_pixel_variance64x32_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance64x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance64x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance64x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance64x64 \
  aom_highbd_10_masked_sub_pixel_variance64x64_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance8x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance8x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance8x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance8x16 \
  aom_highbd_10_masked_sub_pixel_variance8x16_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance8x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance8x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance8x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance8x32 \
  aom_highbd_10_masked_sub_pixel_variance8x32_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance8x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance8x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance8x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance8x4 \
  aom_highbd_10_masked_sub_pixel_variance8x4_c
#endif

unsigned int aom_highbd_10_masked_sub_pixel_variance8x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_10_masked_sub_pixel_variance8x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_masked_sub_pixel_variance8x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_10_masked_sub_pixel_variance8x8 \
  aom_highbd_10_masked_sub_pixel_variance8x8_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance256x256_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance256x256_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance256x256)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance256x256 \
  aom_highbd_12_masked_sub_pixel_variance256x256_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance256x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance256x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance256x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance256x128 \
  aom_highbd_12_masked_sub_pixel_variance256x128_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance128x256_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance128x256_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance128x256)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance128x256 \
  aom_highbd_12_masked_sub_pixel_variance128x256_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance128x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance128x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance128x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance128x128 \
  aom_highbd_12_masked_sub_pixel_variance128x128_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance128x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance128x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance128x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance128x64 \
  aom_highbd_12_masked_sub_pixel_variance128x64_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance16x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance16x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance16x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance16x16 \
  aom_highbd_12_masked_sub_pixel_variance16x16_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance16x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance16x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance16x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance16x32 \
  aom_highbd_12_masked_sub_pixel_variance16x32_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance16x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance16x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance16x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance16x4 \
  aom_highbd_12_masked_sub_pixel_variance16x4_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance16x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance16x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance16x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance16x64 \
  aom_highbd_12_masked_sub_pixel_variance16x64_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance16x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance16x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance16x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance16x8 \
  aom_highbd_12_masked_sub_pixel_variance16x8_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance32x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance32x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance32x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance32x16 \
  aom_highbd_12_masked_sub_pixel_variance32x16_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance32x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance32x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance32x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance32x32 \
  aom_highbd_12_masked_sub_pixel_variance32x32_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance32x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance32x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance32x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance32x64 \
  aom_highbd_12_masked_sub_pixel_variance32x64_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance32x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance32x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance32x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance32x4 \
  aom_highbd_12_masked_sub_pixel_variance32x4_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance32x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance32x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance32x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance32x8 \
  aom_highbd_12_masked_sub_pixel_variance32x8_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance4x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance4x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance4x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance4x64 \
  aom_highbd_12_masked_sub_pixel_variance4x64_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance4x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance4x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance4x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance4x32 \
  aom_highbd_12_masked_sub_pixel_variance4x32_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance4x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance4x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance4x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance4x16 \
  aom_highbd_12_masked_sub_pixel_variance4x16_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance4x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance4x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance4x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance4x4 \
  aom_highbd_12_masked_sub_pixel_variance4x4_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance4x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance4x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance4x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance4x8 \
  aom_highbd_12_masked_sub_pixel_variance4x8_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance64x128_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance64x128_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance64x128)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance64x128 \
  aom_highbd_12_masked_sub_pixel_variance64x128_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance64x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance64x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance64x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance64x4 \
  aom_highbd_12_masked_sub_pixel_variance64x4_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance64x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance64x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance64x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance64x8 \
  aom_highbd_12_masked_sub_pixel_variance64x8_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance64x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance64x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance64x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance64x16 \
  aom_highbd_12_masked_sub_pixel_variance64x16_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance64x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance64x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance64x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance64x32 \
  aom_highbd_12_masked_sub_pixel_variance64x32_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance64x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance64x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance64x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance64x64 \
  aom_highbd_12_masked_sub_pixel_variance64x64_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance8x16_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance8x16_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance8x16)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance8x16 \
  aom_highbd_12_masked_sub_pixel_variance8x16_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance8x32_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance8x32_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance8x32)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance8x32 \
  aom_highbd_12_masked_sub_pixel_variance8x32_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance8x64_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance8x64_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance8x64)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance8x64 \
  aom_highbd_12_masked_sub_pixel_variance8x64_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance8x4_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance8x4_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance8x4)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance8x4 \
  aom_highbd_12_masked_sub_pixel_variance8x4_c
#endif

unsigned int aom_highbd_12_masked_sub_pixel_variance8x8_c(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#if HAVE_SIMD
#if defined(__SSSE3__)
unsigned int aom_highbd_12_masked_sub_pixel_variance8x8_ssse3(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_masked_sub_pixel_variance8x8)(
    const uint16_t *src,
    int src_stride,
    int xoffset,
    int yoffset,
    const uint16_t *ref,
    int ref_stride,
    const uint16_t *second_pred,
    const uint8_t *msk,
    int msk_stride,
    int invert_mask,
    unsigned int *sse);
#endif
#else
#define aom_highbd_12_masked_sub_pixel_variance8x8 \
  aom_highbd_12_masked_sub_pixel_variance8x8_c
#endif

  // OBMC Variance / OBMC Subpixel Variance
unsigned int aom_highbd_obmc_variance256x256_c(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance256x256_sse4_1(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance256x256)(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#else
#define aom_highbd_obmc_variance256x256 aom_highbd_obmc_variance256x256_c
#endif

unsigned int aom_highbd_obmc_variance256x128_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance256x128_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance256x128)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_obmc_variance256x128 aom_highbd_obmc_variance256x128_c
#endif

unsigned int aom_highbd_obmc_variance128x256_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance128x256_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance128x256)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_obmc_variance128x256 aom_highbd_obmc_variance128x256_c
#endif

unsigned int aom_highbd_obmc_variance128x128_c(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance128x128_sse4_1(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance128x128)(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#else
#define aom_highbd_obmc_variance128x128 aom_highbd_obmc_variance128x128_c
#endif

unsigned int aom_highbd_obmc_variance128x64_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance128x64_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance128x64)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_obmc_variance128x64 aom_highbd_obmc_variance128x64_c
#endif

unsigned int aom_highbd_obmc_variance16x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance16x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance16x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance16x16 aom_highbd_obmc_variance16x16_c
#endif

unsigned int aom_highbd_obmc_variance16x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance16x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance16x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance16x32 aom_highbd_obmc_variance16x32_c
#endif

unsigned int aom_highbd_obmc_variance16x4_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance16x4_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance16x4)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance16x4 aom_highbd_obmc_variance16x4_c
#endif

unsigned int aom_highbd_obmc_variance16x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance16x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance16x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance16x64 aom_highbd_obmc_variance16x64_c
#endif

unsigned int aom_highbd_obmc_variance16x8_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance16x8_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance16x8)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance16x8 aom_highbd_obmc_variance16x8_c
#endif

unsigned int aom_highbd_obmc_variance32x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance32x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance32x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance32x16 aom_highbd_obmc_variance32x16_c
#endif

unsigned int aom_highbd_obmc_variance32x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance32x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance32x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance32x32 aom_highbd_obmc_variance32x32_c
#endif

unsigned int aom_highbd_obmc_variance32x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance32x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance32x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance32x64 aom_highbd_obmc_variance32x64_c
#endif

unsigned int aom_highbd_obmc_variance32x8_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance32x8_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance32x8)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance32x8 aom_highbd_obmc_variance32x8_c
#endif

unsigned int aom_highbd_obmc_variance32x4_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance32x4_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance32x4)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance32x4 aom_highbd_obmc_variance32x4_c
#endif

unsigned int aom_highbd_obmc_variance4x32_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance4x32_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance4x32)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance4x32 aom_highbd_obmc_variance4x32_c
#endif

unsigned int aom_highbd_obmc_variance4x16_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance4x16_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance4x16)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance4x16 aom_highbd_obmc_variance4x16_c
#endif

unsigned int aom_highbd_obmc_variance4x4_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance4x4_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance4x4)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_obmc_variance4x4 aom_highbd_obmc_variance4x4_c
#endif

unsigned int aom_highbd_obmc_variance4x8_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance4x8_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance4x8)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_obmc_variance4x8 aom_highbd_obmc_variance4x8_c
#endif

unsigned int aom_highbd_obmc_variance64x128_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance64x128_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance64x128)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_obmc_variance64x128 aom_highbd_obmc_variance64x128_c
#endif

unsigned int aom_highbd_obmc_variance64x8_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance64x8_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance64x8)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance64x8 aom_highbd_obmc_variance64x8_c
#endif

unsigned int aom_highbd_obmc_variance8x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance8x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance8x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance8x64 aom_highbd_obmc_variance8x64_c
#endif

unsigned int aom_highbd_obmc_variance64x4_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance64x4_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance64x4)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance64x4 aom_highbd_obmc_variance64x4_c
#endif

unsigned int aom_highbd_obmc_variance4x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance4x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance4x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance4x64 aom_highbd_obmc_variance4x64_c
#endif

unsigned int aom_highbd_obmc_variance64x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance64x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance64x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance64x16 aom_highbd_obmc_variance64x16_c
#endif

unsigned int aom_highbd_obmc_variance64x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance64x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance64x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance64x32 aom_highbd_obmc_variance64x32_c
#endif

unsigned int aom_highbd_obmc_variance64x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance64x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance64x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_obmc_variance64x64 aom_highbd_obmc_variance64x64_c
#endif

unsigned int aom_highbd_obmc_variance8x16_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance8x16_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance8x16)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance8x16 aom_highbd_obmc_variance8x16_c
#endif

unsigned int aom_highbd_obmc_variance8x32_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance8x32_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance8x32)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_obmc_variance8x32 aom_highbd_obmc_variance8x32_c
#endif

unsigned int aom_highbd_obmc_variance8x4_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance8x4_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance8x4)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_obmc_variance8x4 aom_highbd_obmc_variance8x4_c
#endif

unsigned int aom_highbd_obmc_variance8x8_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_obmc_variance8x8_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_obmc_variance8x8)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_obmc_variance8x8 aom_highbd_obmc_variance8x8_c
#endif

unsigned int aom_highbd_10_obmc_variance256x256_c(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance256x256_sse4_1(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance256x256)(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance256x256 aom_highbd_10_obmc_variance256x256_c
#endif

unsigned int aom_highbd_10_obmc_variance256x128_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance256x128_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance256x128)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance256x128 aom_highbd_10_obmc_variance256x128_c
#endif

unsigned int aom_highbd_10_obmc_variance128x256_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance128x256_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance128x256)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance128x256 aom_highbd_10_obmc_variance128x256_c
#endif

unsigned int aom_highbd_10_obmc_variance128x128_c(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance128x128_sse4_1(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance128x128)(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance128x128 aom_highbd_10_obmc_variance128x128_c
#endif

unsigned int aom_highbd_10_obmc_variance128x64_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance128x64_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance128x64)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance128x64 aom_highbd_10_obmc_variance128x64_c
#endif

unsigned int aom_highbd_10_obmc_variance16x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance16x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance16x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance16x16 aom_highbd_10_obmc_variance16x16_c
#endif

unsigned int aom_highbd_10_obmc_variance16x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance16x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance16x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance16x32 aom_highbd_10_obmc_variance16x32_c
#endif

unsigned int aom_highbd_10_obmc_variance16x4_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance16x4_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance16x4)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance16x4 aom_highbd_10_obmc_variance16x4_c
#endif

unsigned int aom_highbd_10_obmc_variance16x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance16x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance16x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance16x64 aom_highbd_10_obmc_variance16x64_c
#endif

unsigned int aom_highbd_10_obmc_variance16x8_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance16x8_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance16x8)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance16x8 aom_highbd_10_obmc_variance16x8_c
#endif

unsigned int aom_highbd_10_obmc_variance32x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance32x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance32x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance32x16 aom_highbd_10_obmc_variance32x16_c
#endif

unsigned int aom_highbd_10_obmc_variance32x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance32x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance32x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance32x32 aom_highbd_10_obmc_variance32x32_c
#endif

unsigned int aom_highbd_10_obmc_variance32x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance32x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance32x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance32x64 aom_highbd_10_obmc_variance32x64_c
#endif

unsigned int aom_highbd_10_obmc_variance32x4_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance32x4_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance32x4)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance32x4 aom_highbd_10_obmc_variance32x4_c
#endif

unsigned int aom_highbd_10_obmc_variance32x8_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance32x8_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance32x8)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance32x8 aom_highbd_10_obmc_variance32x8_c
#endif

unsigned int aom_highbd_10_obmc_variance4x16_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance4x16_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance4x16)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance4x16 aom_highbd_10_obmc_variance4x16_c
#endif

unsigned int aom_highbd_10_obmc_variance4x4_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance4x4_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance4x4)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance4x4 aom_highbd_10_obmc_variance4x4_c
#endif

unsigned int aom_highbd_10_obmc_variance4x8_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance4x8_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance4x8)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance4x8 aom_highbd_10_obmc_variance4x8_c
#endif

unsigned int aom_highbd_10_obmc_variance64x128_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance64x128_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance64x128)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance64x128 aom_highbd_10_obmc_variance64x128_c
#endif

unsigned int aom_highbd_10_obmc_variance64x4_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance64x4_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance64x4)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance64x4 aom_highbd_10_obmc_variance64x4_c
#endif

unsigned int aom_highbd_10_obmc_variance64x8_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance64x8_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance64x8)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance64x8 aom_highbd_10_obmc_variance64x8_c
#endif

unsigned int aom_highbd_10_obmc_variance64x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance64x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance64x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance64x16 aom_highbd_10_obmc_variance64x16_c
#endif

unsigned int aom_highbd_10_obmc_variance64x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance64x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance64x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance64x32 aom_highbd_10_obmc_variance64x32_c
#endif

unsigned int aom_highbd_10_obmc_variance64x64_c(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance64x64_sse4_1(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance64x64)(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance64x64 aom_highbd_10_obmc_variance64x64_c
#endif

unsigned int aom_highbd_10_obmc_variance8x16_c(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance8x16_sse4_1(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance8x16)(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance8x16 aom_highbd_10_obmc_variance8x16_c
#endif

unsigned int aom_highbd_10_obmc_variance4x32_c(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance4x32_sse4_1(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance4x32)(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance4x32 aom_highbd_10_obmc_variance4x32_c
#endif

unsigned int aom_highbd_10_obmc_variance8x32_c(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance8x32_sse4_1(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance8x32)(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance8x32 aom_highbd_10_obmc_variance8x32_c
#endif

unsigned int aom_highbd_10_obmc_variance8x64_c(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance8x64_sse4_1(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance8x64)(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance8x64 aom_highbd_10_obmc_variance8x64_c
#endif

unsigned int aom_highbd_10_obmc_variance4x64_c(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance4x64_sse4_1(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance4x64)(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance4x64 aom_highbd_10_obmc_variance4x64_c
#endif

unsigned int aom_highbd_10_obmc_variance8x4_c(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance8x4_sse4_1(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance8x4)(const uint16_t* pre,
     int pre_stride,
     const int32_t* wsrc,
     const int32_t* mask,
     unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance8x4 aom_highbd_10_obmc_variance8x4_c
#endif

unsigned int aom_highbd_10_obmc_variance8x8_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_10_obmc_variance8x8_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_10_obmc_variance8x8)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_10_obmc_variance8x8 aom_highbd_10_obmc_variance8x8_c
#endif

unsigned int aom_highbd_12_obmc_variance256x256_c(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance256x256_sse4_1(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance256x256)(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance256x256 aom_highbd_12_obmc_variance256x256_c
#endif

unsigned int aom_highbd_12_obmc_variance256x128_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance256x128_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance256x128)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance256x128 aom_highbd_12_obmc_variance256x128_c
#endif

unsigned int aom_highbd_12_obmc_variance128x256_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance128x256_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance128x256)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance128x256 aom_highbd_12_obmc_variance128x256_c
#endif

unsigned int aom_highbd_12_obmc_variance128x128_c(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance128x128_sse4_1(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance128x128)(const uint16_t* pre,
   int pre_stride,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance128x128 aom_highbd_12_obmc_variance128x128_c
#endif

unsigned int aom_highbd_12_obmc_variance128x64_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance128x64_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance128x64)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance128x64 aom_highbd_12_obmc_variance128x64_c
#endif

unsigned int aom_highbd_12_obmc_variance16x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance16x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance16x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance16x16 aom_highbd_12_obmc_variance16x16_c
#endif

unsigned int aom_highbd_12_obmc_variance16x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance16x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance16x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance16x32 aom_highbd_12_obmc_variance16x32_c
#endif

unsigned int aom_highbd_12_obmc_variance16x4_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance16x4_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance16x4)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance16x4 aom_highbd_12_obmc_variance16x4_c
#endif

unsigned int aom_highbd_12_obmc_variance16x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance16x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance16x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance16x64 aom_highbd_12_obmc_variance16x64_c
#endif

unsigned int aom_highbd_12_obmc_variance16x8_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance16x8_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance16x8)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance16x8 aom_highbd_12_obmc_variance16x8_c
#endif

unsigned int aom_highbd_12_obmc_variance32x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance32x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance32x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance32x16 aom_highbd_12_obmc_variance32x16_c
#endif

unsigned int aom_highbd_12_obmc_variance32x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance32x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance32x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance32x32 aom_highbd_12_obmc_variance32x32_c
#endif

unsigned int aom_highbd_12_obmc_variance32x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance32x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance32x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance32x64 aom_highbd_12_obmc_variance32x64_c
#endif

unsigned int aom_highbd_12_obmc_variance32x4_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance32x4_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance32x4)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance32x4 aom_highbd_12_obmc_variance32x4_c
#endif

unsigned int aom_highbd_12_obmc_variance32x8_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance32x8_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance32x8)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance32x8 aom_highbd_12_obmc_variance32x8_c
#endif

unsigned int aom_highbd_12_obmc_variance4x64_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance4x64_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance4x64)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance4x64 aom_highbd_12_obmc_variance4x64_c
#endif

unsigned int aom_highbd_12_obmc_variance4x32_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance4x32_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance4x32)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance4x32 aom_highbd_12_obmc_variance4x32_c
#endif

unsigned int aom_highbd_12_obmc_variance4x16_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance4x16_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance4x16)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance4x16 aom_highbd_12_obmc_variance4x16_c
#endif

unsigned int aom_highbd_12_obmc_variance4x4_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance4x4_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance4x4)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance4x4 aom_highbd_12_obmc_variance4x4_c
#endif

unsigned int aom_highbd_12_obmc_variance4x8_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance4x8_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance4x8)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance4x8 aom_highbd_12_obmc_variance4x8_c
#endif

unsigned int aom_highbd_12_obmc_variance64x128_c(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance64x128_sse4_1(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance64x128)(const uint16_t* pre,
  int pre_stride,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance64x128 aom_highbd_12_obmc_variance64x128_c
#endif

unsigned int aom_highbd_12_obmc_variance64x4_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance64x4_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance64x4)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance64x4 aom_highbd_12_obmc_variance64x4_c
#endif

unsigned int aom_highbd_12_obmc_variance64x8_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance64x8_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance64x8)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance64x8 aom_highbd_12_obmc_variance64x8_c
#endif

unsigned int aom_highbd_12_obmc_variance64x16_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance64x16_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance64x16)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance64x16 aom_highbd_12_obmc_variance64x16_c
#endif

unsigned int aom_highbd_12_obmc_variance64x32_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance64x32_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance64x32)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance64x32 aom_highbd_12_obmc_variance64x32_c
#endif

unsigned int aom_highbd_12_obmc_variance64x64_c(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance64x64_sse4_1(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance64x64)(const uint16_t* pre,
 int pre_stride,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance64x64 aom_highbd_12_obmc_variance64x64_c
#endif

unsigned int aom_highbd_12_obmc_variance8x16_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance8x16_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance8x16)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance8x16 aom_highbd_12_obmc_variance8x16_c
#endif

unsigned int aom_highbd_12_obmc_variance8x32_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance8x32_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance8x32)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance8x32 aom_highbd_12_obmc_variance8x32_c
#endif

unsigned int aom_highbd_12_obmc_variance8x64_c(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance8x64_sse4_1(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance8x64)(const uint16_t* pre,
int pre_stride,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance8x64 aom_highbd_12_obmc_variance8x64_c
#endif

unsigned int aom_highbd_12_obmc_variance8x4_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance8x4_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance8x4)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance8x4 aom_highbd_12_obmc_variance8x4_c
#endif

unsigned int aom_highbd_12_obmc_variance8x8_c(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#if HAVE_SIMD && defined(__SSE4_1__)
unsigned int aom_highbd_12_obmc_variance8x8_sse4_1(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
RTCD_EXTERN unsigned int (*aom_highbd_12_obmc_variance8x8)(const uint16_t* pre,
    int pre_stride,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#else
#define aom_highbd_12_obmc_variance8x8 aom_highbd_12_obmc_variance8x8_c
#endif

unsigned int aom_highbd_obmc_sub_pixel_variance256x256_c(const uint16_t* pre,
    int pre_stride,
    int xoffset,
    int yoffset,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance256x256 \
  aom_highbd_obmc_sub_pixel_variance256x256_c

unsigned int aom_highbd_obmc_sub_pixel_variance256x128_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance256x128 \
  aom_highbd_obmc_sub_pixel_variance256x128_c

unsigned int aom_highbd_obmc_sub_pixel_variance128x256_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance128x256 \
  aom_highbd_obmc_sub_pixel_variance128x256_c

unsigned int aom_highbd_obmc_sub_pixel_variance128x128_c(const uint16_t* pre,
    int pre_stride,
    int xoffset,
    int yoffset,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance128x128 \
  aom_highbd_obmc_sub_pixel_variance128x128_c

unsigned int aom_highbd_obmc_sub_pixel_variance128x64_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance128x64 \
  aom_highbd_obmc_sub_pixel_variance128x64_c

unsigned int aom_highbd_obmc_sub_pixel_variance16x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance16x16 \
  aom_highbd_obmc_sub_pixel_variance16x16_c

unsigned int aom_highbd_obmc_sub_pixel_variance16x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance16x32 \
  aom_highbd_obmc_sub_pixel_variance16x32_c

unsigned int aom_highbd_obmc_sub_pixel_variance16x4_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance16x4 \
  aom_highbd_obmc_sub_pixel_variance16x4_c

unsigned int aom_highbd_obmc_sub_pixel_variance16x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance16x64 \
  aom_highbd_obmc_sub_pixel_variance16x64_c

unsigned int aom_highbd_obmc_sub_pixel_variance8x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance8x64 \
  aom_highbd_obmc_sub_pixel_variance8x64_c

unsigned int aom_highbd_obmc_sub_pixel_variance16x8_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance16x8 \
  aom_highbd_obmc_sub_pixel_variance16x8_c

unsigned int aom_highbd_obmc_sub_pixel_variance32x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance32x16 \
  aom_highbd_obmc_sub_pixel_variance32x16_c

unsigned int aom_highbd_obmc_sub_pixel_variance32x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance32x32 \
  aom_highbd_obmc_sub_pixel_variance32x32_c

unsigned int aom_highbd_obmc_sub_pixel_variance32x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance32x64 \
  aom_highbd_obmc_sub_pixel_variance32x64_c

unsigned int aom_highbd_obmc_sub_pixel_variance32x4_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance32x4 \
  aom_highbd_obmc_sub_pixel_variance32x4_c

unsigned int aom_highbd_obmc_sub_pixel_variance4x32_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance4x32 \
  aom_highbd_obmc_sub_pixel_variance4x32_c

unsigned int aom_highbd_obmc_sub_pixel_variance4x64_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance4x64 \
  aom_highbd_obmc_sub_pixel_variance4x64_c

unsigned int aom_highbd_obmc_sub_pixel_variance32x8_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance32x8 \
  aom_highbd_obmc_sub_pixel_variance32x8_c

unsigned int aom_highbd_obmc_sub_pixel_variance4x16_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance4x16 \
  aom_highbd_obmc_sub_pixel_variance4x16_c

unsigned int aom_highbd_obmc_sub_pixel_variance4x4_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance4x4 \
  aom_highbd_obmc_sub_pixel_variance4x4_c

unsigned int aom_highbd_obmc_sub_pixel_variance4x8_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance4x8 \
  aom_highbd_obmc_sub_pixel_variance4x8_c

unsigned int aom_highbd_obmc_sub_pixel_variance64x128_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance64x128 \
  aom_highbd_obmc_sub_pixel_variance64x128_c

unsigned int aom_highbd_obmc_sub_pixel_variance64x8_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance64x8 \
  aom_highbd_obmc_sub_pixel_variance64x8_c

unsigned int aom_highbd_obmc_sub_pixel_variance64x4_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance64x4 \
  aom_highbd_obmc_sub_pixel_variance64x4_c

unsigned int aom_highbd_obmc_sub_pixel_variance64x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance64x16 \
  aom_highbd_obmc_sub_pixel_variance64x16_c

unsigned int aom_highbd_obmc_sub_pixel_variance64x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance64x32 \
  aom_highbd_obmc_sub_pixel_variance64x32_c

unsigned int aom_highbd_obmc_sub_pixel_variance64x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance64x64 \
  aom_highbd_obmc_sub_pixel_variance64x64_c

unsigned int aom_highbd_obmc_sub_pixel_variance8x16_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance8x16 \
  aom_highbd_obmc_sub_pixel_variance8x16_c

unsigned int aom_highbd_obmc_sub_pixel_variance8x32_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance8x32 \
  aom_highbd_obmc_sub_pixel_variance8x32_c

unsigned int aom_highbd_obmc_sub_pixel_variance8x4_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance8x4 \
  aom_highbd_obmc_sub_pixel_variance8x4_c

unsigned int aom_highbd_obmc_sub_pixel_variance8x8_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_obmc_sub_pixel_variance8x8 \
  aom_highbd_obmc_sub_pixel_variance8x8_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance256x256_c(const uint16_t* pre,
    int pre_stride,
    int xoffset,
    int yoffset,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance256x256 \
  aom_highbd_10_obmc_sub_pixel_variance256x256_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance256x128_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance256x128 \
  aom_highbd_10_obmc_sub_pixel_variance256x128_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance128x256_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance128x256 \
  aom_highbd_10_obmc_sub_pixel_variance128x256_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance128x128_c(const uint16_t* pre,
    int pre_stride,
    int xoffset,
    int yoffset,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance128x128 \
  aom_highbd_10_obmc_sub_pixel_variance128x128_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance128x64_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance128x64 \
  aom_highbd_10_obmc_sub_pixel_variance128x64_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance16x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance16x16 \
  aom_highbd_10_obmc_sub_pixel_variance16x16_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance16x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance16x32 \
  aom_highbd_10_obmc_sub_pixel_variance16x32_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance16x4_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance16x4 \
  aom_highbd_10_obmc_sub_pixel_variance16x4_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance16x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance16x64 \
  aom_highbd_10_obmc_sub_pixel_variance16x64_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance16x8_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance16x8 \
  aom_highbd_10_obmc_sub_pixel_variance16x8_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance32x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance32x16 \
  aom_highbd_10_obmc_sub_pixel_variance32x16_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance32x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance32x32 \
  aom_highbd_10_obmc_sub_pixel_variance32x32_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance32x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance32x64 \
  aom_highbd_10_obmc_sub_pixel_variance32x64_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance32x4_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance32x4 \
  aom_highbd_10_obmc_sub_pixel_variance32x4_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance32x8_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance32x8 \
  aom_highbd_10_obmc_sub_pixel_variance32x8_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance4x64_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance4x64 \
  aom_highbd_10_obmc_sub_pixel_variance4x64_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance4x32_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance4x32 \
  aom_highbd_10_obmc_sub_pixel_variance4x32_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance4x16_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance4x16 \
  aom_highbd_10_obmc_sub_pixel_variance4x16_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance4x4_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance4x4 \
  aom_highbd_10_obmc_sub_pixel_variance4x4_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance4x8_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance4x8 \
  aom_highbd_10_obmc_sub_pixel_variance4x8_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance64x128_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance64x128 \
  aom_highbd_10_obmc_sub_pixel_variance64x128_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance64x4_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance64x4 \
  aom_highbd_10_obmc_sub_pixel_variance64x4_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance64x8_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance64x8 \
  aom_highbd_10_obmc_sub_pixel_variance64x8_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance64x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance64x16 \
  aom_highbd_10_obmc_sub_pixel_variance64x16_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance64x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance64x32 \
  aom_highbd_10_obmc_sub_pixel_variance64x32_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance64x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance64x64 \
  aom_highbd_10_obmc_sub_pixel_variance64x64_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance8x16_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance8x16 \
  aom_highbd_10_obmc_sub_pixel_variance8x16_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance8x32_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance8x32 \
  aom_highbd_10_obmc_sub_pixel_variance8x32_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance8x64_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance8x64 \
  aom_highbd_10_obmc_sub_pixel_variance8x64_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance8x4_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance8x4 \
  aom_highbd_10_obmc_sub_pixel_variance8x4_c

unsigned int aom_highbd_10_obmc_sub_pixel_variance8x8_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_10_obmc_sub_pixel_variance8x8 \
  aom_highbd_10_obmc_sub_pixel_variance8x8_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance256x256_c(const uint16_t* pre,
    int pre_stride,
    int xoffset,
    int yoffset,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance256x256 \
  aom_highbd_12_obmc_sub_pixel_variance256x256_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance256x128_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance256x128 \
  aom_highbd_12_obmc_sub_pixel_variance256x128_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance128x256_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance128x256 \
  aom_highbd_12_obmc_sub_pixel_variance128x256_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance128x128_c(const uint16_t* pre,
    int pre_stride,
    int xoffset,
    int yoffset,
    const int32_t* wsrc,
    const int32_t* mask,
    unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance128x128 \
  aom_highbd_12_obmc_sub_pixel_variance128x128_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance128x64_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance128x64 \
  aom_highbd_12_obmc_sub_pixel_variance128x64_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance16x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance16x16 \
  aom_highbd_12_obmc_sub_pixel_variance16x16_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance16x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance16x32 \
  aom_highbd_12_obmc_sub_pixel_variance16x32_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance16x4_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance16x4 \
  aom_highbd_12_obmc_sub_pixel_variance16x4_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance16x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance16x64 \
  aom_highbd_12_obmc_sub_pixel_variance16x64_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance16x8_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance16x8 \
  aom_highbd_12_obmc_sub_pixel_variance16x8_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance32x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance32x16 \
  aom_highbd_12_obmc_sub_pixel_variance32x16_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance32x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance32x32 \
  aom_highbd_12_obmc_sub_pixel_variance32x32_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance32x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance32x64 \
  aom_highbd_12_obmc_sub_pixel_variance32x64_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance32x4_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance32x4 \
  aom_highbd_12_obmc_sub_pixel_variance32x4_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance32x8_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance32x8 \
  aom_highbd_12_obmc_sub_pixel_variance32x8_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance4x64_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance4x64 \
  aom_highbd_12_obmc_sub_pixel_variance4x64_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance4x32_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance4x32 \
  aom_highbd_12_obmc_sub_pixel_variance4x32_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance4x16_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance4x16 \
  aom_highbd_12_obmc_sub_pixel_variance4x16_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance4x4_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance4x4 \
  aom_highbd_12_obmc_sub_pixel_variance4x4_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance4x8_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance4x8 \
  aom_highbd_12_obmc_sub_pixel_variance4x8_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance64x128_c(const uint16_t* pre,
   int pre_stride,
   int xoffset,
   int yoffset,
   const int32_t* wsrc,
   const int32_t* mask,
   unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance64x128 \
  aom_highbd_12_obmc_sub_pixel_variance64x128_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance64x4_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance64x4 \
  aom_highbd_12_obmc_sub_pixel_variance64x4_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance64x8_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance64x8 \
  aom_highbd_12_obmc_sub_pixel_variance64x8_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance64x16_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance64x16 \
  aom_highbd_12_obmc_sub_pixel_variance64x16_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance64x32_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance64x32 \
  aom_highbd_12_obmc_sub_pixel_variance64x32_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance64x64_c(const uint16_t* pre,
  int pre_stride,
  int xoffset,
  int yoffset,
  const int32_t* wsrc,
  const int32_t* mask,
  unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance64x64 \
  aom_highbd_12_obmc_sub_pixel_variance64x64_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance8x16_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance8x16 \
  aom_highbd_12_obmc_sub_pixel_variance8x16_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance8x32_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance8x32 \
  aom_highbd_12_obmc_sub_pixel_variance8x32_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance8x64_c(const uint16_t* pre,
 int pre_stride,
 int xoffset,
 int yoffset,
 const int32_t* wsrc,
 const int32_t* mask,
 unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance8x64 \
  aom_highbd_12_obmc_sub_pixel_variance8x64_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance8x4_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance8x4 \
  aom_highbd_12_obmc_sub_pixel_variance8x4_c

unsigned int aom_highbd_12_obmc_sub_pixel_variance8x8_c(const uint16_t* pre,
int pre_stride,
int xoffset,
int yoffset,
const int32_t* wsrc,
const int32_t* mask,
unsigned int* sse);
#define aom_highbd_12_obmc_sub_pixel_variance8x8 \
  aom_highbd_12_obmc_sub_pixel_variance8x8_c

void aom_highbd_8_get16x16var_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
#define aom_highbd_8_get16x16var aom_highbd_8_get16x16var_c

void aom_highbd_8_get8x8var_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
#define aom_highbd_8_get8x8var aom_highbd_8_get8x8var_c

void aom_highbd_10_get16x16var_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
#define aom_highbd_10_get16x16var aom_highbd_10_get16x16var_c

void aom_highbd_10_get8x8var_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
#define aom_highbd_10_get8x8var aom_highbd_10_get8x8var_c

void aom_highbd_12_get16x16var_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
#define aom_highbd_12_get16x16var aom_highbd_12_get16x16var_c

void aom_highbd_12_get8x8var_c(const uint16_t *src_ptr, int source_stride, const uint16_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
#define aom_highbd_12_get8x8var aom_highbd_12_get8x8var_c

unsigned int aom_highbd_8_mse16x16_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#if HAVE_SIMD
//unsigned int aom_highbd_8_mse16x16_sse2(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#define aom_highbd_8_mse16x16 aom_highbd_8_mse16x16_sse2
//#else
#define aom_highbd_8_mse16x16 aom_highbd_8_mse16x16_c
//#endif

unsigned int aom_highbd_8_mse16x8_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define aom_highbd_8_mse16x8 aom_highbd_8_mse16x8_c

unsigned int aom_highbd_8_mse8x16_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define aom_highbd_8_mse8x16 aom_highbd_8_mse8x16_c

unsigned int aom_highbd_8_mse8x8_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#if HAVE_SIMD
//unsigned int aom_highbd_8_mse8x8_sse2(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#define aom_highbd_8_mse8x8 aom_highbd_8_mse8x8_sse2
//#else
#define aom_highbd_8_mse8x8 aom_highbd_8_mse8x8_c
//#endif

unsigned int aom_highbd_10_mse16x16_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#if HAVE_SIMD
//unsigned int aom_highbd_10_mse16x16_sse2(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#define aom_highbd_10_mse16x16 aom_highbd_10_mse16x16_sse2
//#else
#define aom_highbd_10_mse16x16 aom_highbd_10_mse16x16_c
//#endif

unsigned int aom_highbd_10_mse16x8_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define aom_highbd_10_mse16x8 aom_highbd_10_mse16x8_c

unsigned int aom_highbd_10_mse8x16_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define aom_highbd_10_mse8x16 aom_highbd_10_mse8x16_c

unsigned int aom_highbd_10_mse8x8_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#if HAVE_SIMD
//unsigned int aom_highbd_10_mse8x8_sse2(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#define aom_highbd_10_mse8x8 aom_highbd_10_mse8x8_sse2
//#else
#define aom_highbd_10_mse8x8 aom_highbd_10_mse8x8_c
//#endif

unsigned int aom_highbd_12_mse16x16_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#if HAVE_SIMD
//unsigned int aom_highbd_12_mse16x16_sse2(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#define aom_highbd_12_mse16x16 aom_highbd_12_mse16x16_sse2
//#else
#define aom_highbd_12_mse16x16 aom_highbd_12_mse16x16_c
//#endif

unsigned int aom_highbd_12_mse16x8_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define aom_highbd_12_mse16x8 aom_highbd_12_mse16x8_c

unsigned int aom_highbd_12_mse8x16_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define aom_highbd_12_mse8x16 aom_highbd_12_mse8x16_c

unsigned int aom_highbd_12_mse8x8_c(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#if HAVE_SIMD
//unsigned int aom_highbd_12_mse8x8_sse2(const uint16_t *src_ptr, int  source_stride, const uint16_t *ref_ptr, int  recon_stride, unsigned int *sse);
//#define aom_highbd_12_mse8x8 aom_highbd_12_mse8x8_sse2
//#else
#define aom_highbd_12_mse8x8 aom_highbd_12_mse8x8_c
//#endif

void aom_highbd_comp_avg_pred_c(uint16_t *comp_pred8, const uint16_t *pred8, int width, int height, const uint16_t *ref8, int ref_stride);
#define aom_highbd_comp_avg_pred aom_highbd_comp_avg_pred_c

void aom_highbd_dist_wtd_comp_avg_pred_c(uint16_t *comp_pred8, const uint16_t *pred8, int width, int height, const uint16_t *ref8, int ref_stride, const DIST_WTD_COMP_PARAMS *jcp_param);
#if HAVE_SIMD
void aom_highbd_dist_wtd_comp_avg_pred_sse2(uint16_t *comp_pred8, const uint16_t *pred8, int width, int height, const uint16_t *ref8, int ref_stride, const DIST_WTD_COMP_PARAMS *jcp_param);
#define aom_highbd_dist_wtd_comp_avg_pred aom_highbd_dist_wtd_comp_avg_pred_sse2
#else
#define aom_highbd_dist_wtd_comp_avg_pred aom_highbd_dist_wtd_comp_avg_pred_c
#endif

uint64_t aom_mse_wxh_16bit_highbd_c(uint16_t *dst, int dstride,uint16_t *src, int sstride, int w, int h);
#if HAVE_SIMD
#if defined(__SSE2__)
uint64_t aom_mse_wxh_16bit_highbd_sse2(uint16_t *dst, int dstride,uint16_t *src, int sstride, int w, int h);
RTCD_EXTERN uint64_t (*aom_mse_wxh_16bit_highbd)(uint16_t *dst, int dstride,uint16_t *src, int sstride, int w, int h);
#endif
#if defined(__AVX2__)
uint64_t aom_mse_wxh_16bit_highbd_avx2(uint16_t *dst, int dstride,uint16_t *src, int sstride, int w, int h);
#endif
#else
#define aom_mse_wxh_16bit_highbd aom_mse_wxh_16bit_highbd_c
#endif

void aom_highbd_comp_mask_pred_c(uint16_t *comp_pred, const uint16_t *pred8, int width, int height, const uint16_t *ref8, int ref_stride, const uint8_t *mask, int mask_stride, int invert_mask);
#if HAVE_SIMD
#if defined(__SSE2__)
void aom_highbd_comp_mask_pred_sse2(uint16_t *comp_pred, const uint16_t *pred8, int width, int height, const uint16_t *ref8, int ref_stride, const uint8_t *mask, int mask_stride, int invert_mask);
RTCD_EXTERN void (*aom_highbd_comp_mask_pred)(uint16_t *comp_pred, const uint16_t *pred8, int width, int height, const uint16_t *ref8, int ref_stride, const uint8_t *mask, int mask_stride, int invert_mask);
#endif
#if defined(__AVX2__)
void aom_highbd_comp_mask_pred_avx2(uint16_t *comp_pred, const uint16_t *pred8, int width, int height, const uint16_t *ref8, int ref_stride, const uint8_t *mask, int mask_stride, int invert_mask);
#endif
#else
#define aom_highbd_comp_mask_pred aom_highbd_comp_mask_pred_c
#endif

void aom_highbd_paeth_predictor_16x16_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_16x16 aom_highbd_paeth_predictor_16x16_c

void aom_highbd_paeth_predictor_16x32_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_16x32 aom_highbd_paeth_predictor_16x32_c

void aom_highbd_paeth_predictor_16x4_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_16x4 aom_highbd_paeth_predictor_16x4_c

void aom_highbd_paeth_predictor_16x64_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_16x64 aom_highbd_paeth_predictor_16x64_c

void aom_highbd_paeth_predictor_16x8_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_16x8 aom_highbd_paeth_predictor_16x8_c

void aom_highbd_paeth_predictor_2x2_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_paeth_predictor_2x2 aom_highbd_paeth_predictor_2x2_c

void aom_highbd_paeth_predictor_32x4_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_32x4 aom_highbd_paeth_predictor_32x4_c

void aom_highbd_paeth_predictor_32x8_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_32x8 aom_highbd_paeth_predictor_32x8_c

void aom_highbd_paeth_predictor_32x16_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_32x16 aom_highbd_paeth_predictor_32x16_c

void aom_highbd_paeth_predictor_32x32_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_32x32 aom_highbd_paeth_predictor_32x32_c

void aom_highbd_paeth_predictor_32x64_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_32x64 aom_highbd_paeth_predictor_32x64_c

void aom_highbd_paeth_predictor_32x8_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_32x8 aom_highbd_paeth_predictor_32x8_c

void aom_highbd_paeth_predictor_4x64_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_4x64 aom_highbd_paeth_predictor_4x64_c

void aom_highbd_paeth_predictor_4x32_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_4x32 aom_highbd_paeth_predictor_4x32_c

void aom_highbd_paeth_predictor_4x16_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_4x16 aom_highbd_paeth_predictor_4x16_c

void aom_highbd_paeth_predictor_4x4_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_paeth_predictor_4x4 aom_highbd_paeth_predictor_4x4_c

void aom_highbd_paeth_predictor_4x8_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_paeth_predictor_4x8 aom_highbd_paeth_predictor_4x8_c

void aom_highbd_paeth_predictor_64x4_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_64x4 aom_highbd_paeth_predictor_64x4_c

void aom_highbd_paeth_predictor_64x8_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_64x8 aom_highbd_paeth_predictor_64x8_c

void aom_highbd_paeth_predictor_64x16_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_64x16 aom_highbd_paeth_predictor_64x16_c

void aom_highbd_paeth_predictor_64x32_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_64x32 aom_highbd_paeth_predictor_64x32_c

void aom_highbd_paeth_predictor_64x64_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_paeth_predictor_64x64 aom_highbd_paeth_predictor_64x64_c

void aom_highbd_paeth_predictor_8x64_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_8x64 aom_highbd_paeth_predictor_8x64_c

void aom_highbd_paeth_predictor_8x16_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_8x16 aom_highbd_paeth_predictor_8x16_c

void aom_highbd_paeth_predictor_8x32_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_paeth_predictor_8x32 aom_highbd_paeth_predictor_8x32_c

void aom_highbd_paeth_predictor_8x4_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_paeth_predictor_8x4 aom_highbd_paeth_predictor_8x4_c

void aom_highbd_paeth_predictor_8x8_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_paeth_predictor_8x8 aom_highbd_paeth_predictor_8x8_c

void aom_highbd_smooth_predictor_16x16_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_16x16 aom_highbd_smooth_predictor_16x16_c

void aom_highbd_smooth_predictor_16x32_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_16x32 aom_highbd_smooth_predictor_16x32_c

void aom_highbd_smooth_predictor_16x4_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_16x4 aom_highbd_smooth_predictor_16x4_c

void aom_highbd_smooth_predictor_16x64_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_16x64 aom_highbd_smooth_predictor_16x64_c

void aom_highbd_smooth_predictor_16x8_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_16x8 aom_highbd_smooth_predictor_16x8_c

void aom_highbd_smooth_predictor_2x2_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_smooth_predictor_2x2 aom_highbd_smooth_predictor_2x2_c

void aom_highbd_smooth_predictor_32x16_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_32x16 aom_highbd_smooth_predictor_32x16_c

void aom_highbd_smooth_predictor_32x32_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_32x32 aom_highbd_smooth_predictor_32x32_c

void aom_highbd_smooth_predictor_32x64_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_32x64 aom_highbd_smooth_predictor_32x64_c

void aom_highbd_smooth_predictor_32x4_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_32x4 aom_highbd_smooth_predictor_32x4_c

void aom_highbd_smooth_predictor_32x8_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_32x8 aom_highbd_smooth_predictor_32x8_c

void aom_highbd_smooth_predictor_4x64_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_4x64 aom_highbd_smooth_predictor_4x64_c

void aom_highbd_smooth_predictor_4x32_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_4x32 aom_highbd_smooth_predictor_4x32_c

void aom_highbd_smooth_predictor_4x16_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_4x16 aom_highbd_smooth_predictor_4x16_c

void aom_highbd_smooth_predictor_4x4_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_smooth_predictor_4x4 aom_highbd_smooth_predictor_4x4_c

void aom_highbd_smooth_predictor_4x8_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_smooth_predictor_4x8 aom_highbd_smooth_predictor_4x8_c

void aom_highbd_smooth_predictor_64x4_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_64x4 aom_highbd_smooth_predictor_64x4_c

void aom_highbd_smooth_predictor_64x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_64x8 aom_highbd_smooth_predictor_64x8_c

void aom_highbd_smooth_predictor_64x16_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_64x16 aom_highbd_smooth_predictor_64x16_c

void aom_highbd_smooth_predictor_64x32_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_64x32 aom_highbd_smooth_predictor_64x32_c

void aom_highbd_smooth_predictor_64x64_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_predictor_64x64 aom_highbd_smooth_predictor_64x64_c

void aom_highbd_smooth_predictor_8x64_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_8x64 aom_highbd_smooth_predictor_8x64_c

void aom_highbd_smooth_predictor_8x16_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_8x16 aom_highbd_smooth_predictor_8x16_c

void aom_highbd_smooth_predictor_8x32_c(uint16_t* dst,
   ptrdiff_t y_stride,
   const uint16_t* above,
   const uint16_t* left,
   int bd);
#define aom_highbd_smooth_predictor_8x32 aom_highbd_smooth_predictor_8x32_c

void aom_highbd_smooth_predictor_8x4_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_smooth_predictor_8x4 aom_highbd_smooth_predictor_8x4_c

void aom_highbd_smooth_predictor_8x8_c(uint16_t* dst,
  ptrdiff_t y_stride,
  const uint16_t* above,
  const uint16_t* left,
  int bd);
#define aom_highbd_smooth_predictor_8x8 aom_highbd_smooth_predictor_8x8_c

void aom_highbd_smooth_v_predictor_16x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_v_predictor_16x16 \
  aom_highbd_smooth_v_predictor_16x16_c

void aom_highbd_smooth_v_predictor_16x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_v_predictor_16x32 \
  aom_highbd_smooth_v_predictor_16x32_c

void aom_highbd_smooth_v_predictor_16x4_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_smooth_v_predictor_16x4 aom_highbd_smooth_v_predictor_16x4_c

void aom_highbd_smooth_v_predictor_16x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_v_predictor_16x64 \
  aom_highbd_smooth_v_predictor_16x64_c

void aom_highbd_smooth_v_predictor_16x8_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_smooth_v_predictor_16x8 aom_highbd_smooth_v_predictor_16x8_c

void aom_highbd_smooth_v_predictor_2x2_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_v_predictor_2x2 aom_highbd_smooth_v_predictor_2x2_c

void aom_highbd_smooth_v_predictor_32x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_v_predictor_32x16 \
  aom_highbd_smooth_v_predictor_32x16_c

void aom_highbd_smooth_v_predictor_32x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_v_predictor_32x32 \
  aom_highbd_smooth_v_predictor_32x32_c

void aom_highbd_smooth_v_predictor_32x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_v_predictor_32x64 \
  aom_highbd_smooth_v_predictor_32x64_c

void aom_highbd_smooth_v_predictor_32x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_32x4 aom_highbd_smooth_v_predictor_32x4_c

void aom_highbd_smooth_v_predictor_32x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_32x8 aom_highbd_smooth_v_predictor_32x8_c

void aom_highbd_smooth_v_predictor_4x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_4x64 aom_highbd_smooth_v_predictor_4x64_c

void aom_highbd_smooth_v_predictor_4x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_4x32 aom_highbd_smooth_v_predictor_4x32_c

void aom_highbd_smooth_v_predictor_4x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_4x16 aom_highbd_smooth_v_predictor_4x16_c

void aom_highbd_smooth_v_predictor_4x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_4x4 aom_highbd_smooth_v_predictor_4x4_c

void aom_highbd_smooth_v_predictor_4x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_v_predictor_4x8 aom_highbd_smooth_v_predictor_4x8_c

void aom_highbd_smooth_v_predictor_64x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_64x4 \
  aom_highbd_smooth_v_predictor_64x4_c

void aom_highbd_smooth_v_predictor_64x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_64x8 \
  aom_highbd_smooth_v_predictor_64x8_c

void aom_highbd_smooth_v_predictor_64x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_64x16 \
  aom_highbd_smooth_v_predictor_64x16_c

void aom_highbd_smooth_v_predictor_64x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_64x32 \
  aom_highbd_smooth_v_predictor_64x32_c

void aom_highbd_smooth_v_predictor_64x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_v_predictor_64x64 \
  aom_highbd_smooth_v_predictor_64x64_c

void aom_highbd_smooth_v_predictor_8x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_8x64 aom_highbd_smooth_v_predictor_8x64_c

void aom_highbd_smooth_v_predictor_8x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_8x16 aom_highbd_smooth_v_predictor_8x16_c

void aom_highbd_smooth_v_predictor_8x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_v_predictor_8x32 aom_highbd_smooth_v_predictor_8x32_c

void aom_highbd_smooth_v_predictor_8x4_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_v_predictor_8x4 aom_highbd_smooth_v_predictor_8x4_c

void aom_highbd_smooth_v_predictor_8x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_v_predictor_8x8 aom_highbd_smooth_v_predictor_8x8_c

void aom_highbd_smooth_h_predictor_16x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_16x16 \
  aom_highbd_smooth_h_predictor_16x16_c

void aom_highbd_smooth_h_predictor_16x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_16x32 \
  aom_highbd_smooth_h_predictor_16x32_c

void aom_highbd_smooth_h_predictor_16x4_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_smooth_h_predictor_16x4 aom_highbd_smooth_h_predictor_16x4_c

void aom_highbd_smooth_h_predictor_16x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_16x64 \
  aom_highbd_smooth_h_predictor_16x64_c

void aom_highbd_smooth_h_predictor_16x8_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_smooth_h_predictor_16x8 aom_highbd_smooth_h_predictor_16x8_c

void aom_highbd_smooth_h_predictor_2x2_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_h_predictor_2x2 aom_highbd_smooth_h_predictor_2x2_c

void aom_highbd_smooth_h_predictor_32x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_32x16 \
  aom_highbd_smooth_h_predictor_32x16_c

void aom_highbd_smooth_h_predictor_32x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_32x32 \
  aom_highbd_smooth_h_predictor_32x32_c

void aom_highbd_smooth_h_predictor_32x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_32x64 \
  aom_highbd_smooth_h_predictor_32x64_c

void aom_highbd_smooth_h_predictor_32x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_32x8 aom_highbd_smooth_h_predictor_32x8_c

void aom_highbd_smooth_h_predictor_32x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_32x4 aom_highbd_smooth_h_predictor_32x4_c

void aom_highbd_smooth_h_predictor_4x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_4x32 aom_highbd_smooth_h_predictor_4x32_c

void aom_highbd_smooth_h_predictor_4x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_4x16 aom_highbd_smooth_h_predictor_4x16_c

void aom_highbd_smooth_h_predictor_4x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_4x4 aom_highbd_smooth_h_predictor_4x4_c

void aom_highbd_smooth_h_predictor_4x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_h_predictor_4x8 aom_highbd_smooth_h_predictor_4x8_c

void aom_highbd_smooth_h_predictor_64x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_64x4 \
  aom_highbd_smooth_h_predictor_64x4_c

void aom_highbd_smooth_h_predictor_64x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_64x8 \
  aom_highbd_smooth_h_predictor_64x8_c

void aom_highbd_smooth_h_predictor_64x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_64x16 \
  aom_highbd_smooth_h_predictor_64x16_c

void aom_highbd_smooth_h_predictor_64x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_64x32 \
  aom_highbd_smooth_h_predictor_64x32_c

void aom_highbd_smooth_h_predictor_64x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_smooth_h_predictor_64x64 \
  aom_highbd_smooth_h_predictor_64x64_c

void aom_highbd_smooth_h_predictor_4x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_4x64 aom_highbd_smooth_h_predictor_4x64_c

void aom_highbd_smooth_h_predictor_8x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_8x64 aom_highbd_smooth_h_predictor_8x64_c

void aom_highbd_smooth_h_predictor_8x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_smooth_h_predictor_8x16 aom_highbd_smooth_h_predictor_8x16_c

void aom_highbd_smooth_h_predictor_8x32_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_smooth_h_predictor_8x32 aom_highbd_smooth_h_predictor_8x32_c

void aom_highbd_smooth_h_predictor_8x4_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_h_predictor_8x4 aom_highbd_smooth_h_predictor_8x4_c

void aom_highbd_smooth_h_predictor_8x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_smooth_h_predictor_8x8 aom_highbd_smooth_h_predictor_8x8_c

void aom_highbd_ibp_dc_top_predictor_16x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_16x16 \
  aom_highbd_ibp_dc_top_predictor_16x16_c

void aom_highbd_ibp_dc_top_predictor_16x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_16x32 \
  aom_highbd_ibp_dc_top_predictor_16x32_c

void aom_highbd_ibp_dc_top_predictor_16x4_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_top_predictor_16x4 aom_highbd_ibp_dc_top_predictor_16x4_c

void aom_highbd_ibp_dc_top_predictor_16x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_16x64 \
  aom_highbd_ibp_dc_top_predictor_16x64_c

void aom_highbd_ibp_dc_top_predictor_16x8_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_top_predictor_16x8 aom_highbd_ibp_dc_top_predictor_16x8_c

void aom_highbd_ibp_dc_top_predictor_2x2_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_top_predictor_2x2 aom_highbd_ibp_dc_top_predictor_2x2_c

void aom_highbd_ibp_dc_top_predictor_32x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_32x16 \
  aom_highbd_ibp_dc_top_predictor_32x16_c

void aom_highbd_ibp_dc_top_predictor_32x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_32x32 \
  aom_highbd_ibp_dc_top_predictor_32x32_c

void aom_highbd_ibp_dc_top_predictor_32x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_32x64 \
  aom_highbd_ibp_dc_top_predictor_32x64_c

void aom_highbd_ibp_dc_top_predictor_32x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_32x4 aom_highbd_ibp_dc_top_predictor_32x4_c

void aom_highbd_ibp_dc_top_predictor_32x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_32x8 aom_highbd_ibp_dc_top_predictor_32x8_c

void aom_highbd_ibp_dc_top_predictor_4x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_4x64 aom_highbd_ibp_dc_top_predictor_4x64_c

void aom_highbd_ibp_dc_top_predictor_4x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_4x32 aom_highbd_ibp_dc_top_predictor_4x32_c

void aom_highbd_ibp_dc_top_predictor_4x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_4x16 aom_highbd_ibp_dc_top_predictor_4x16_c

void aom_highbd_ibp_dc_top_predictor_4x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_4x4 aom_highbd_ibp_dc_top_predictor_4x4_c

void aom_highbd_ibp_dc_top_predictor_4x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_4x8 aom_highbd_ibp_dc_top_predictor_4x8_c

void aom_highbd_ibp_dc_top_predictor_64x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_64x4 \
  aom_highbd_ibp_dc_top_predictor_64x4_c

void aom_highbd_ibp_dc_top_predictor_64x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_64x8 \
  aom_highbd_ibp_dc_top_predictor_64x8_c

void aom_highbd_ibp_dc_top_predictor_64x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_64x16 \
  aom_highbd_ibp_dc_top_predictor_64x16_c

void aom_highbd_ibp_dc_top_predictor_64x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_64x32 \
  aom_highbd_ibp_dc_top_predictor_64x32_c

void aom_highbd_ibp_dc_top_predictor_64x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_top_predictor_64x64 \
  aom_highbd_ibp_dc_top_predictor_64x64_c

void aom_highbd_ibp_dc_top_predictor_8x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_8x64 aom_highbd_ibp_dc_top_predictor_8x64_c

void aom_highbd_ibp_dc_top_predictor_8x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_8x16 aom_highbd_ibp_dc_top_predictor_8x16_c

void aom_highbd_ibp_dc_top_predictor_8x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_top_predictor_8x32 aom_highbd_ibp_dc_top_predictor_8x32_c

void aom_highbd_ibp_dc_top_predictor_8x4_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_top_predictor_8x4 aom_highbd_ibp_dc_top_predictor_8x4_c

void aom_highbd_ibp_dc_top_predictor_8x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_top_predictor_8x8 aom_highbd_ibp_dc_top_predictor_8x8_c

void aom_highbd_ibp_dc_left_predictor_16x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_16x16 \
  aom_highbd_ibp_dc_left_predictor_16x16_c

void aom_highbd_ibp_dc_left_predictor_16x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_16x32 \
  aom_highbd_ibp_dc_left_predictor_16x32_c

void aom_highbd_ibp_dc_left_predictor_16x4_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_left_predictor_16x4 aom_highbd_ibp_dc_left_predictor_16x4_c

void aom_highbd_ibp_dc_left_predictor_16x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_16x64 \
  aom_highbd_ibp_dc_left_predictor_16x64_c

void aom_highbd_ibp_dc_left_predictor_16x8_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_left_predictor_16x8 aom_highbd_ibp_dc_left_predictor_16x8_c

void aom_highbd_ibp_dc_left_predictor_2x2_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_left_predictor_2x2 aom_highbd_ibp_dc_left_predictor_2x2_c

void aom_highbd_ibp_dc_left_predictor_32x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_32x16 \
  aom_highbd_ibp_dc_left_predictor_32x16_c

void aom_highbd_ibp_dc_left_predictor_32x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_32x32 \
  aom_highbd_ibp_dc_left_predictor_32x32_c

void aom_highbd_ibp_dc_left_predictor_32x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_32x64 \
  aom_highbd_ibp_dc_left_predictor_32x64_c

void aom_highbd_ibp_dc_left_predictor_32x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_32x4 aom_highbd_ibp_dc_left_predictor_32x4_c

void aom_highbd_ibp_dc_left_predictor_32x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_32x8 aom_highbd_ibp_dc_left_predictor_32x8_c

void aom_highbd_ibp_dc_left_predictor_4x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_4x64 aom_highbd_ibp_dc_left_predictor_4x64_c

void aom_highbd_ibp_dc_left_predictor_4x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_4x32 aom_highbd_ibp_dc_left_predictor_4x32_c

void aom_highbd_ibp_dc_left_predictor_4x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_4x16 aom_highbd_ibp_dc_left_predictor_4x16_c

void aom_highbd_ibp_dc_left_predictor_4x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_4x4 aom_highbd_ibp_dc_left_predictor_4x4_c

void aom_highbd_ibp_dc_left_predictor_4x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_left_predictor_4x8 aom_highbd_ibp_dc_left_predictor_4x8_c

void aom_highbd_ibp_dc_left_predictor_64x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_64x4 \
  aom_highbd_ibp_dc_left_predictor_64x4_c

void aom_highbd_ibp_dc_left_predictor_64x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_64x8 \
  aom_highbd_ibp_dc_left_predictor_64x8_c

void aom_highbd_ibp_dc_left_predictor_64x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_64x16 \
  aom_highbd_ibp_dc_left_predictor_64x16_c

void aom_highbd_ibp_dc_left_predictor_64x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_64x32 \
  aom_highbd_ibp_dc_left_predictor_64x32_c

void aom_highbd_ibp_dc_left_predictor_64x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_left_predictor_64x64 \
  aom_highbd_ibp_dc_left_predictor_64x64_c

void aom_highbd_ibp_dc_left_predictor_8x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_8x64 aom_highbd_ibp_dc_left_predictor_8x64_c

void aom_highbd_ibp_dc_left_predictor_8x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_8x16 aom_highbd_ibp_dc_left_predictor_8x16_c

void aom_highbd_ibp_dc_left_predictor_8x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_left_predictor_8x32 aom_highbd_ibp_dc_left_predictor_8x32_c

void aom_highbd_ibp_dc_left_predictor_8x4_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_left_predictor_8x4 aom_highbd_ibp_dc_left_predictor_8x4_c

void aom_highbd_ibp_dc_left_predictor_8x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_left_predictor_8x8 aom_highbd_ibp_dc_left_predictor_8x8_c

void aom_highbd_ibp_dc_predictor_16x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_16x16 \
  aom_highbd_ibp_dc_predictor_16x16_c

void aom_highbd_ibp_dc_predictor_16x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_16x32 \
  aom_highbd_ibp_dc_predictor_16x32_c

void aom_highbd_ibp_dc_predictor_16x4_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_predictor_16x4 aom_highbd_ibp_dc_predictor_16x4_c

void aom_highbd_ibp_dc_predictor_16x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_16x64 \
  aom_highbd_ibp_dc_predictor_16x64_c

void aom_highbd_ibp_dc_predictor_16x8_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_predictor_16x8 aom_highbd_ibp_dc_predictor_16x8_c

void aom_highbd_ibp_dc_predictor_2x2_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_predictor_2x2 aom_highbd_ibp_dc_predictor_2x2_c

void aom_highbd_ibp_dc_predictor_32x16_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_32x16 \
  aom_highbd_ibp_dc_predictor_32x16_c

void aom_highbd_ibp_dc_predictor_32x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_32x32 \
  aom_highbd_ibp_dc_predictor_32x32_c

void aom_highbd_ibp_dc_predictor_32x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_32x64 \
  aom_highbd_ibp_dc_predictor_32x64_c

void aom_highbd_ibp_dc_predictor_32x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_32x4 aom_highbd_ibp_dc_predictor_32x4_c

void aom_highbd_ibp_dc_predictor_32x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_32x8 aom_highbd_ibp_dc_predictor_32x8_c

void aom_highbd_ibp_dc_predictor_4x64_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_4x64 aom_highbd_ibp_dc_predictor_4x64_c

void aom_highbd_ibp_dc_predictor_4x32_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_4x32 aom_highbd_ibp_dc_predictor_4x32_c

void aom_highbd_ibp_dc_predictor_4x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_4x16 aom_highbd_ibp_dc_predictor_4x16_c

void aom_highbd_ibp_dc_predictor_4x4_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_predictor_4x4 aom_highbd_ibp_dc_predictor_4x4_c

void aom_highbd_ibp_dc_predictor_4x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_predictor_4x8 aom_highbd_ibp_dc_predictor_4x8_c

void aom_highbd_ibp_dc_predictor_64x4_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_64x4 \
  aom_highbd_ibp_dc_predictor_64x4_c

void aom_highbd_ibp_dc_predictor_64x8_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_64x8 \
  aom_highbd_ibp_dc_predictor_64x8_c

void aom_highbd_ibp_dc_predictor_64x16_c(uint16_t* dst,
     ptrdiff_t y_stride,
     const uint16_t* above,
     const uint16_t* left,
     int bd);
#define aom_highbd_ibp_dc_predictor_64x16 \
  aom_highbd_ibp_dc_predictor_64x16_c

void aom_highbd_ibp_dc_predictor_64x32_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_64x32 \
  aom_highbd_ibp_dc_predictor_64x32_c

void aom_highbd_ibp_dc_predictor_64x64_c(uint16_t* dst,
 ptrdiff_t y_stride,
 const uint16_t* above,
 const uint16_t* left,
 int bd);
#define aom_highbd_ibp_dc_predictor_64x64 \
  aom_highbd_ibp_dc_predictor_64x64_c

void aom_highbd_ibp_dc_predictor_8x16_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_predictor_8x16 aom_highbd_ibp_dc_predictor_8x16_c

void aom_highbd_ibp_dc_predictor_8x32_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_predictor_8x32 aom_highbd_ibp_dc_predictor_8x32_c

void aom_highbd_ibp_dc_predictor_8x64_c(uint16_t* dst,
ptrdiff_t y_stride,
const uint16_t* above,
const uint16_t* left,
int bd);
#define aom_highbd_ibp_dc_predictor_8x64 aom_highbd_ibp_dc_predictor_8x64_c

void aom_highbd_ibp_dc_predictor_8x4_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_predictor_8x4 aom_highbd_ibp_dc_predictor_8x4_c

void aom_highbd_ibp_dc_predictor_8x8_c(uint16_t* dst,
    ptrdiff_t y_stride,
    const uint16_t* above,
    const uint16_t* left,
    int bd);
#define aom_highbd_ibp_dc_predictor_8x8 aom_highbd_ibp_dc_predictor_8x8_c

unsigned int aom_highbd_dist_wtd_sad256x256_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad256x256_avg aom_highbd_dist_wtd_sad256x256_avg_c

unsigned int aom_highbd_dist_wtd_sad256x128_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad256x128_avg aom_highbd_dist_wtd_sad256x128_avg_c

unsigned int aom_highbd_dist_wtd_sad128x256_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad128x256_avg aom_highbd_dist_wtd_sad128x256_avg_c

unsigned int aom_highbd_dist_wtd_sad128x128_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad128x128_avg aom_highbd_dist_wtd_sad128x128_avg_c

unsigned int aom_highbd_dist_wtd_sad128x64_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad128x64_avg aom_highbd_dist_wtd_sad128x64_avg_c

unsigned int aom_highbd_dist_wtd_sad16x16_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad16x16_avg aom_highbd_dist_wtd_sad16x16_avg_c

unsigned int aom_highbd_dist_wtd_sad16x32_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad16x32_avg aom_highbd_dist_wtd_sad16x32_avg_c

unsigned int aom_highbd_dist_wtd_sad16x4_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad16x4_avg aom_highbd_dist_wtd_sad16x4_avg_c

unsigned int aom_highbd_dist_wtd_sad16x64_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad16x64_avg aom_highbd_dist_wtd_sad16x64_avg_c

unsigned int aom_highbd_dist_wtd_sad16x8_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad16x8_avg aom_highbd_dist_wtd_sad16x8_avg_c

unsigned int aom_highbd_dist_wtd_sad32x16_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad32x16_avg aom_highbd_dist_wtd_sad32x16_avg_c

unsigned int aom_highbd_dist_wtd_sad32x32_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad32x32_avg aom_highbd_dist_wtd_sad32x32_avg_c

unsigned int aom_highbd_dist_wtd_sad32x64_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad32x64_avg aom_highbd_dist_wtd_sad32x64_avg_c

unsigned int aom_highbd_dist_wtd_sad32x8_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad32x8_avg aom_highbd_dist_wtd_sad32x8_avg_c

unsigned int aom_highbd_dist_wtd_sad4x32_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad4x32_avg aom_highbd_dist_wtd_sad4x32_avg_c

unsigned int aom_highbd_dist_wtd_sad32x4_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad32x4_avg aom_highbd_dist_wtd_sad32x4_avg_c

unsigned int aom_highbd_dist_wtd_sad4x64_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad4x64_avg aom_highbd_dist_wtd_sad4x64_avg_c

unsigned int aom_highbd_dist_wtd_sad64x4_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad64x4_avg aom_highbd_dist_wtd_sad64x4_avg_c

unsigned int aom_highbd_dist_wtd_sad8x64_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad8x64_avg aom_highbd_dist_wtd_sad8x64_avg_c

unsigned int aom_highbd_dist_wtd_sad64x8_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad64x8_avg aom_highbd_dist_wtd_sad64x8_avg_c

unsigned int aom_highbd_dist_wtd_sad4x16_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad4x16_avg aom_highbd_dist_wtd_sad4x16_avg_c

unsigned int aom_highbd_dist_wtd_sad4x4_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad4x4_avg aom_highbd_dist_wtd_sad4x4_avg_c

unsigned int aom_highbd_dist_wtd_sad4x8_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad4x8_avg aom_highbd_dist_wtd_sad4x8_avg_c

unsigned int aom_highbd_dist_wtd_sad64x128_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad64x128_avg aom_highbd_dist_wtd_sad64x128_avg_c

unsigned int aom_highbd_dist_wtd_sad64x16_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad64x16_avg aom_highbd_dist_wtd_sad64x16_avg_c

unsigned int aom_highbd_dist_wtd_sad64x32_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad64x32_avg aom_highbd_dist_wtd_sad64x32_avg_c

unsigned int aom_highbd_dist_wtd_sad64x64_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad64x64_avg aom_highbd_dist_wtd_sad64x64_avg_c

unsigned int aom_highbd_dist_wtd_sad8x16_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad8x16_avg aom_highbd_dist_wtd_sad8x16_avg_c

unsigned int aom_highbd_dist_wtd_sad8x32_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad8x32_avg aom_highbd_dist_wtd_sad8x32_avg_c

unsigned int aom_highbd_dist_wtd_sad8x4_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad8x4_avg aom_highbd_dist_wtd_sad8x4_avg_c

unsigned int aom_highbd_dist_wtd_sad8x8_avg_c(
    const uint16_t* src_ptr,
    int src_stride,
    const uint16_t* ref_ptr,
    int ref_stride,
    const uint16_t* second_pred,
    const DIST_WTD_COMP_PARAMS* jcp_param);
#define aom_highbd_dist_wtd_sad8x8_avg aom_highbd_dist_wtd_sad8x8_avg_c

//#endif

void aom_dsp_rtcd(void);

#ifdef RTCD_C
#include "avm_ports/x86.h"
static void setup_rtcd_internal(void) {
  int flags = x86_simd_caps();

  (void)flags;

#if HAVE_SIMD
aom_highbd_blend_a64_mask = aom_highbd_blend_a64_mask_c;
aom_highbd_blend_a64_hmask = aom_highbd_blend_a64_hmask_c;
aom_highbd_blend_a64_vmask = aom_highbd_blend_a64_vmask_c;
aom_highbd_blend_a64_d16_mask = aom_highbd_blend_a64_d16_mask_c;
aom_highbd_sse = aom_highbd_sse_c;
#if CONFIG_BLOCK_256
aom_highbd_sad256x256 = aom_highbd_sad256x256_c;
aom_highbd_sad256x128 = aom_highbd_sad256x128_c;
aom_highbd_sad128x256 = aom_highbd_sad128x256_c;
#endif
aom_highbd_sad128x128 = aom_highbd_sad128x128_c;
aom_highbd_sad128x64 = aom_highbd_sad128x64_c;
aom_highbd_sad64x128 = aom_highbd_sad64x128_c;
#if CONFIG_BLOCK_256
aom_highbd_sad_skip_256x256 = aom_highbd_sad_skip_256x256_c;
aom_highbd_sad_skip_256x128 = aom_highbd_sad_skip_256x128_c;
aom_highbd_sad_skip_128x256 = aom_highbd_sad_skip_128x256_c;
#endif
aom_highbd_sad_skip_128x128 = aom_highbd_sad_skip_128x128_c;
aom_highbd_sad_skip_128x64 = aom_highbd_sad_skip_128x64_c;
aom_highbd_sad_skip_64x128 = aom_highbd_sad_skip_64x128_c;
#if CONFIG_MRSSE
aom_highbd_mrsse = aom_highbd_mrsse_c;
#endif
#if CONFIG_UNEVEN_4WAY
aom_highbd_sad16x16 = aom_highbd_sad16x16_c;
aom_highbd_sad16x8 = aom_highbd_sad16x8_c;
aom_highbd_sad16x4 = aom_highbd_sad16x4_c;
aom_highbd_sad_skip_16x16 = aom_highbd_sad_skip_16x16_c;
aom_highbd_sad_skip_16x8 = aom_highbd_sad_skip_16x8_c;
aom_highbd_8_sub_pixel_variance16x16 = aom_highbd_8_sub_pixel_variance16x16_c;
aom_highbd_8_sub_pixel_variance16x8 = aom_highbd_8_sub_pixel_variance16x8_c;
aom_highbd_8_sub_pixel_variance16x4 = aom_highbd_8_sub_pixel_variance16x4_c;
aom_highbd_10_sub_pixel_variance16x16 = aom_highbd_10_sub_pixel_variance16x16_c;
aom_highbd_10_sub_pixel_variance16x8 = aom_highbd_10_sub_pixel_variance16x8_c;
aom_highbd_10_sub_pixel_variance16x4 = aom_highbd_10_sub_pixel_variance16x4_c;
aom_highbd_12_sub_pixel_variance16x16 = aom_highbd_12_sub_pixel_variance16x16_c;
aom_highbd_12_sub_pixel_variance16x8 = aom_highbd_12_sub_pixel_variance16x8_c;
aom_highbd_12_sub_pixel_variance16x4 = aom_highbd_12_sub_pixel_variance16x4_c;
//aom_highbd_8_sub_pixel_avg_variance16x16 = aom_highbd_8_sub_pixel_avg_variance16x16_c;
//aom_highbd_8_sub_pixel_avg_variance16x8 = aom_highbd_8_sub_pixel_avg_variance16x8_c;
//aom_highbd_8_sub_pixel_avg_variance16x4 = aom_highbd_8_sub_pixel_avg_variance16x4_c;
//aom_highbd_10_sub_pixel_avg_variance16x16 = aom_highbd_10_sub_pixel_avg_variance16x16_c;
//aom_highbd_10_sub_pixel_avg_variance16x8 = aom_highbd_10_sub_pixel_avg_variance16x8_c;
//aom_highbd_10_sub_pixel_avg_variance16x4 = aom_highbd_10_sub_pixel_avg_variance16x4_c;
//aom_highbd_12_sub_pixel_avg_variance16x16 = aom_highbd_12_sub_pixel_avg_variance16x16_c;
//aom_highbd_12_sub_pixel_avg_variance16x8 = aom_highbd_12_sub_pixel_avg_variance16x8_c;
//aom_highbd_12_sub_pixel_avg_variance16x4 = aom_highbd_12_sub_pixel_avg_variance16x4_c;
#endif
#if CONFIG_FLEX_PARTITION
aom_highbd_sad_skip_16x32 = aom_highbd_sad_skip_16x32_c;
aom_highbd_sad_skip_16x64 = aom_highbd_sad_skip_16x64_c;
aom_highbd_8_sub_pixel_variance16x32 = aom_highbd_8_sub_pixel_variance16x32_c;
aom_highbd_8_sub_pixel_variance16x64 = aom_highbd_8_sub_pixel_variance16x64_c;
aom_highbd_10_sub_pixel_variance16x32 = aom_highbd_10_sub_pixel_variance16x32_c;
aom_highbd_10_sub_pixel_variance16x64 = aom_highbd_10_sub_pixel_variance16x64_c;
aom_highbd_12_sub_pixel_variance16x32 = aom_highbd_12_sub_pixel_variance16x32_c;
aom_highbd_12_sub_pixel_variance16x64 = aom_highbd_12_sub_pixel_variance16x64_c;
#endif
#if CONFIG_BLOCK_256
aom_highbd_sad256x256_avg = aom_highbd_sad256x256_avg_c;
aom_highbd_sad256x128_avg = aom_highbd_sad256x128_avg_c;
aom_highbd_sad128x256_avg = aom_highbd_sad128x256_avg_c;
aom_highbd_obmc_sad256x256 = aom_highbd_obmc_sad256x256_c;
aom_highbd_obmc_sad256x128 = aom_highbd_obmc_sad256x128_c;
aom_highbd_obmc_sad128x256 = aom_highbd_obmc_sad128x256_c;
#endif
aom_highbd_sad128x128_avg = aom_highbd_sad128x128_avg_c;
aom_highbd_sad128x64_avg = aom_highbd_sad128x64_avg_c;
aom_highbd_sad64x128_avg = aom_highbd_sad64x128_avg_c;

aom_highbd_obmc_sad128x128 = aom_highbd_obmc_sad128x128_c;
aom_highbd_obmc_sad128x64 = aom_highbd_obmc_sad128x64_c;
aom_highbd_obmc_sad64x128 = aom_highbd_obmc_sad64x128_c;
aom_highbd_obmc_sad64x64 = aom_highbd_obmc_sad64x64_c;
aom_highbd_obmc_sad64x32 = aom_highbd_obmc_sad64x32_c;
aom_highbd_obmc_sad64x16 = aom_highbd_obmc_sad64x16_c;
aom_highbd_obmc_sad32x64 = aom_highbd_obmc_sad32x64_c;
aom_highbd_obmc_sad32x32 = aom_highbd_obmc_sad32x32_c;
aom_highbd_obmc_sad32x16 = aom_highbd_obmc_sad32x16_c;
aom_highbd_obmc_sad32x8 = aom_highbd_obmc_sad32x8_c;
aom_highbd_obmc_sad16x64 = aom_highbd_obmc_sad16x64_c;
aom_highbd_obmc_sad16x32 = aom_highbd_obmc_sad16x32_c;
aom_highbd_obmc_sad16x16 = aom_highbd_obmc_sad16x16_c;
aom_highbd_obmc_sad16x8 = aom_highbd_obmc_sad16x8_c;
aom_highbd_obmc_sad16x4 = aom_highbd_obmc_sad16x4_c;
aom_highbd_obmc_sad8x32 = aom_highbd_obmc_sad8x32_c;
aom_highbd_obmc_sad8x16 = aom_highbd_obmc_sad8x16_c;
aom_highbd_obmc_sad8x8 = aom_highbd_obmc_sad8x8_c;
aom_highbd_obmc_sad8x4 = aom_highbd_obmc_sad8x4_c;
aom_highbd_obmc_sad4x16 = aom_highbd_obmc_sad4x16_c;
aom_highbd_obmc_sad4x8 = aom_highbd_obmc_sad4x8_c;
aom_highbd_obmc_sad4x4 = aom_highbd_obmc_sad4x4_c;

#if CONFIG_BLOCK_256
aom_highbd_obmc_sad4x64 = aom_highbd_obmc_sad4x64_c;
aom_highbd_obmc_sad64x4 = aom_highbd_obmc_sad64x4_c;
aom_highbd_obmc_sad4x32 = aom_highbd_obmc_sad4x32_c;
aom_highbd_obmc_sad32x4 = aom_highbd_obmc_sad32x4_c;
aom_highbd_obmc_sad8x64 = aom_highbd_obmc_sad8x64_c;
aom_highbd_obmc_sad64x8 = aom_highbd_obmc_sad64x8_c;
aom_highbd_8_variance256x256 = aom_highbd_8_variance256x256_c;
aom_highbd_8_variance256x128 = aom_highbd_8_variance256x128_c;
aom_highbd_8_variance128x256 = aom_highbd_8_variance128x256_c;
#endif
aom_highbd_8_variance128x128 = aom_highbd_8_variance128x128_c;
aom_highbd_8_variance128x64 = aom_highbd_8_variance128x64_c;
aom_highbd_8_variance64x128 = aom_highbd_8_variance64x128_c;
//aom_highbd_8_variance8x4 = aom_highbd_8_variance8x4_c;
//aom_highbd_8_variance4x8 = aom_highbd_8_variance4x8_c;
aom_highbd_8_variance4x4 = aom_highbd_8_variance4x4_c;
//aom_highbd_8_variance4x16 = aom_highbd_8_variance4x16_c;
//aom_highbd_8_variance16x4 = aom_highbd_8_variance16x4_c;
#if CONFIG_BLOCK_256
aom_highbd_10_variance256x256 = aom_highbd_10_variance256x256_c;
aom_highbd_10_variance256x128 = aom_highbd_10_variance256x128_c;
aom_highbd_10_variance128x256 = aom_highbd_10_variance128x256_c;
#endif
aom_highbd_10_variance128x128 = aom_highbd_10_variance128x128_c;
aom_highbd_10_variance128x64 = aom_highbd_10_variance128x64_c;
aom_highbd_10_variance64x128 = aom_highbd_10_variance64x128_c;
//aom_highbd_10_variance8x4 = aom_highbd_10_variance8x4_c;
//aom_highbd_10_variance4x8 = aom_highbd_10_variance4x8_c;
aom_highbd_10_variance4x4 = aom_highbd_10_variance4x4_c;
//aom_highbd_10_variance4x16 = aom_highbd_10_variance4x16_c;
//aom_highbd_10_variance16x4 = aom_highbd_10_variance16x4_c;
#if CONFIG_BLOCK_256
aom_highbd_12_variance256x256 = aom_highbd_12_variance256x256_c;
aom_highbd_12_variance256x128 = aom_highbd_12_variance256x128_c;
aom_highbd_12_variance128x256 = aom_highbd_12_variance128x256_c;
#endif
aom_highbd_12_variance128x128 = aom_highbd_12_variance128x128_c;
aom_highbd_12_variance128x64 = aom_highbd_12_variance128x64_c;
aom_highbd_12_variance64x128 = aom_highbd_12_variance64x128_c;
//aom_highbd_12_variance8x4 = aom_highbd_12_variance8x4_c;
//aom_highbd_12_variance4x8 = aom_highbd_12_variance4x8_c;
aom_highbd_12_variance4x4 = aom_highbd_12_variance4x4_c;
//aom_highbd_12_variance4x16 = aom_highbd_12_variance4x16_c;
//aom_highbd_12_variance16x4 = aom_highbd_12_variance16x4_c;

#if CONFIG_BLOCK_256
aom_highbd_8_sub_pixel_variance256x256 = aom_highbd_8_sub_pixel_variance256x256_c;
aom_highbd_8_sub_pixel_variance256x128 = aom_highbd_8_sub_pixel_variance256x128_c;
aom_highbd_8_sub_pixel_variance128x256 = aom_highbd_8_sub_pixel_variance128x256_c;
aom_highbd_10_sub_pixel_variance256x256 = aom_highbd_10_sub_pixel_variance256x256_c;
aom_highbd_10_sub_pixel_variance256x128 = aom_highbd_10_sub_pixel_variance256x128_c;
aom_highbd_10_sub_pixel_variance128x256 = aom_highbd_10_sub_pixel_variance128x256_c;
aom_highbd_12_sub_pixel_variance256x256 = aom_highbd_12_sub_pixel_variance256x256_c;
aom_highbd_12_sub_pixel_variance256x128 = aom_highbd_12_sub_pixel_variance256x128_c;
aom_highbd_12_sub_pixel_variance128x256 = aom_highbd_12_sub_pixel_variance128x256_c;
#endif
aom_highbd_8_sub_pixel_variance128x128 = aom_highbd_8_sub_pixel_variance128x128_c;
aom_highbd_8_sub_pixel_variance128x64 = aom_highbd_8_sub_pixel_variance128x64_c;
aom_highbd_8_sub_pixel_variance64x128 = aom_highbd_8_sub_pixel_variance64x128_c;
aom_highbd_10_sub_pixel_variance128x128 = aom_highbd_10_sub_pixel_variance128x128_c;
aom_highbd_10_sub_pixel_variance128x64 = aom_highbd_10_sub_pixel_variance128x64_c;
aom_highbd_10_sub_pixel_variance64x128 = aom_highbd_10_sub_pixel_variance64x128_c;
aom_highbd_12_sub_pixel_variance128x128 = aom_highbd_12_sub_pixel_variance128x128_c;
aom_highbd_12_sub_pixel_variance128x64 = aom_highbd_12_sub_pixel_variance128x64_c;
aom_highbd_12_sub_pixel_variance64x128 = aom_highbd_12_sub_pixel_variance64x128_c;
//aom_highbd_8_sub_pixel_variance4x8 = aom_highbd_8_sub_pixel_variance4x8_c;
aom_highbd_8_sub_pixel_variance4x4 = aom_highbd_8_sub_pixel_variance4x4_c;
//aom_highbd_8_sub_pixel_variance4x16 = aom_highbd_8_sub_pixel_variance4x16_c;
//aom_highbd_10_sub_pixel_variance4x8 = aom_highbd_10_sub_pixel_variance4x8_c;
aom_highbd_10_sub_pixel_variance4x4 = aom_highbd_10_sub_pixel_variance4x4_c;
//aom_highbd_10_sub_pixel_variance4x16 = aom_highbd_10_sub_pixel_variance4x16_c;
//aom_highbd_12_sub_pixel_variance4x8 = aom_highbd_12_sub_pixel_variance4x8_c;
aom_highbd_12_sub_pixel_variance4x4 = aom_highbd_12_sub_pixel_variance4x4_c;
//aom_highbd_12_sub_pixel_variance4x16 = aom_highbd_12_sub_pixel_variance4x16_c;

//aom_highbd_8_sub_pixel_avg_variance4x8 = aom_highbd_8_sub_pixel_avg_variance4x8_c;
aom_highbd_8_sub_pixel_avg_variance4x4 = aom_highbd_8_sub_pixel_avg_variance4x4_c;
//aom_highbd_8_sub_pixel_avg_variance4x16 = aom_highbd_8_sub_pixel_avg_variance4x16_c;
//aom_highbd_10_sub_pixel_avg_variance4x8 = aom_highbd_10_sub_pixel_avg_variance4x8_c;
aom_highbd_10_sub_pixel_avg_variance4x4 = aom_highbd_10_sub_pixel_avg_variance4x4_c;
//aom_highbd_10_sub_pixel_avg_variance4x16 = aom_highbd_10_sub_pixel_avg_variance4x16_c;
//aom_highbd_12_sub_pixel_avg_variance4x8 = aom_highbd_12_sub_pixel_avg_variance4x8_c;
aom_highbd_12_sub_pixel_avg_variance4x4 = aom_highbd_12_sub_pixel_avg_variance4x4_c;
//aom_highbd_12_sub_pixel_avg_variance4x16 = aom_highbd_12_sub_pixel_avg_variance4x16_c;

#if CONFIG_BLOCK_256
aom_highbd_obmc_variance256x256 = aom_highbd_obmc_variance256x256_c;
aom_highbd_obmc_variance256x128 = aom_highbd_obmc_variance256x128_c;
aom_highbd_obmc_variance128x256 = aom_highbd_obmc_variance128x256_c;
#endif
aom_highbd_obmc_variance128x128 = aom_highbd_obmc_variance128x128_c;
aom_highbd_obmc_variance128x64 = aom_highbd_obmc_variance128x64_c;
aom_highbd_obmc_variance64x128 = aom_highbd_obmc_variance64x128_c;
aom_highbd_obmc_variance64x64 = aom_highbd_obmc_variance64x64_c;
aom_highbd_obmc_variance64x32 = aom_highbd_obmc_variance64x32_c;
aom_highbd_obmc_variance32x64 = aom_highbd_obmc_variance32x64_c;
aom_highbd_obmc_variance32x32 = aom_highbd_obmc_variance32x32_c;
aom_highbd_obmc_variance32x16 = aom_highbd_obmc_variance32x16_c;
aom_highbd_obmc_variance16x32 = aom_highbd_obmc_variance16x32_c;
aom_highbd_obmc_variance16x16 = aom_highbd_obmc_variance16x16_c;
aom_highbd_obmc_variance16x8 = aom_highbd_obmc_variance16x8_c;
aom_highbd_obmc_variance8x16 = aom_highbd_obmc_variance8x16_c;
aom_highbd_obmc_variance8x8 = aom_highbd_obmc_variance8x8_c;
aom_highbd_obmc_variance8x4 = aom_highbd_obmc_variance8x4_c;
aom_highbd_obmc_variance4x8 = aom_highbd_obmc_variance4x8_c;
aom_highbd_obmc_variance4x4 = aom_highbd_obmc_variance4x4_c;
aom_highbd_obmc_variance4x16 = aom_highbd_obmc_variance4x16_c;
aom_highbd_obmc_variance16x4 = aom_highbd_obmc_variance16x4_c;
aom_highbd_obmc_variance8x32 = aom_highbd_obmc_variance8x32_c;
aom_highbd_obmc_variance32x8 = aom_highbd_obmc_variance32x8_c;
aom_highbd_obmc_variance16x64 = aom_highbd_obmc_variance16x64_c;
aom_highbd_obmc_variance64x16 = aom_highbd_obmc_variance64x16_c;
#if CONFIG_BLOCK_256
aom_highbd_obmc_variance4x32 = aom_highbd_obmc_variance4x32_c;
aom_highbd_obmc_variance32x4 = aom_highbd_obmc_variance32x4_c;
aom_highbd_obmc_variance8x64 = aom_highbd_obmc_variance8x64_c;
aom_highbd_obmc_variance64x8 = aom_highbd_obmc_variance64x8_c;
aom_highbd_obmc_variance4x64 = aom_highbd_obmc_variance4x64_c;
aom_highbd_obmc_variance64x4 = aom_highbd_obmc_variance64x4_c;
aom_highbd_10_obmc_variance256x256 = aom_highbd_10_obmc_variance256x256_c;
aom_highbd_10_obmc_variance256x128 = aom_highbd_10_obmc_variance256x128_c;
aom_highbd_10_obmc_variance128x256 = aom_highbd_10_obmc_variance128x256_c;
#endif
aom_highbd_10_obmc_variance128x128 = aom_highbd_10_obmc_variance128x128_c;
aom_highbd_10_obmc_variance128x64 = aom_highbd_10_obmc_variance128x64_c;
aom_highbd_10_obmc_variance64x128 = aom_highbd_10_obmc_variance64x128_c;
aom_highbd_10_obmc_variance64x64 = aom_highbd_10_obmc_variance64x64_c;
aom_highbd_10_obmc_variance64x32 = aom_highbd_10_obmc_variance64x32_c;
aom_highbd_10_obmc_variance32x64 = aom_highbd_10_obmc_variance32x64_c;
aom_highbd_10_obmc_variance32x32 = aom_highbd_10_obmc_variance32x32_c;
aom_highbd_10_obmc_variance32x16 = aom_highbd_10_obmc_variance32x16_c;
aom_highbd_10_obmc_variance16x32 = aom_highbd_10_obmc_variance16x32_c;
aom_highbd_10_obmc_variance16x16 = aom_highbd_10_obmc_variance16x16_c;
aom_highbd_10_obmc_variance16x8 = aom_highbd_10_obmc_variance16x8_c;
aom_highbd_10_obmc_variance8x16 = aom_highbd_10_obmc_variance8x16_c;
aom_highbd_10_obmc_variance8x8 = aom_highbd_10_obmc_variance8x8_c;
aom_highbd_10_obmc_variance8x4 = aom_highbd_10_obmc_variance8x4_c;
aom_highbd_10_obmc_variance4x8 = aom_highbd_10_obmc_variance4x8_c;
aom_highbd_10_obmc_variance4x4 = aom_highbd_10_obmc_variance4x4_c;
aom_highbd_10_obmc_variance4x16 = aom_highbd_10_obmc_variance4x16_c;
aom_highbd_10_obmc_variance16x4 = aom_highbd_10_obmc_variance16x4_c;
aom_highbd_10_obmc_variance8x32 = aom_highbd_10_obmc_variance8x32_c;
aom_highbd_10_obmc_variance32x8 = aom_highbd_10_obmc_variance32x8_c;
aom_highbd_10_obmc_variance16x64 = aom_highbd_10_obmc_variance16x64_c;
aom_highbd_10_obmc_variance64x16 = aom_highbd_10_obmc_variance64x16_c;
#if CONFIG_BLOCK_256
aom_highbd_10_obmc_variance4x32 = aom_highbd_10_obmc_variance4x32_c;
aom_highbd_10_obmc_variance32x4 = aom_highbd_10_obmc_variance32x4_c;
aom_highbd_10_obmc_variance8x64 = aom_highbd_10_obmc_variance8x64_c;
aom_highbd_10_obmc_variance64x8 = aom_highbd_10_obmc_variance64x8_c;
aom_highbd_10_obmc_variance4x64 = aom_highbd_10_obmc_variance4x64_c;
aom_highbd_10_obmc_variance64x4 = aom_highbd_10_obmc_variance64x4_c;
aom_highbd_12_obmc_variance256x256 = aom_highbd_12_obmc_variance256x256_c;
aom_highbd_12_obmc_variance256x128 = aom_highbd_12_obmc_variance256x128_c;
aom_highbd_12_obmc_variance128x256 = aom_highbd_12_obmc_variance128x256_c;
#endif
aom_highbd_12_obmc_variance128x128 = aom_highbd_12_obmc_variance128x128_c;
aom_highbd_12_obmc_variance128x64 = aom_highbd_12_obmc_variance128x64_c;
aom_highbd_12_obmc_variance64x128 = aom_highbd_12_obmc_variance64x128_c;
aom_highbd_12_obmc_variance64x64 = aom_highbd_12_obmc_variance64x64_c;
aom_highbd_12_obmc_variance64x32 = aom_highbd_12_obmc_variance64x32_c;
aom_highbd_12_obmc_variance32x64 = aom_highbd_12_obmc_variance32x64_c;
aom_highbd_12_obmc_variance32x32 = aom_highbd_12_obmc_variance32x32_c;
aom_highbd_12_obmc_variance32x16 = aom_highbd_12_obmc_variance32x16_c;
aom_highbd_12_obmc_variance16x32 = aom_highbd_12_obmc_variance16x32_c;
aom_highbd_12_obmc_variance16x16 = aom_highbd_12_obmc_variance16x16_c;
aom_highbd_12_obmc_variance16x8 = aom_highbd_12_obmc_variance16x8_c;
aom_highbd_12_obmc_variance8x16 = aom_highbd_12_obmc_variance8x16_c;
aom_highbd_12_obmc_variance8x8 = aom_highbd_12_obmc_variance8x8_c;
aom_highbd_12_obmc_variance8x4 = aom_highbd_12_obmc_variance8x4_c;
aom_highbd_12_obmc_variance4x8 = aom_highbd_12_obmc_variance4x8_c;
aom_highbd_12_obmc_variance4x4 = aom_highbd_12_obmc_variance4x4_c;
aom_highbd_12_obmc_variance4x16 = aom_highbd_12_obmc_variance4x16_c;
aom_highbd_12_obmc_variance16x4 = aom_highbd_12_obmc_variance16x4_c;
aom_highbd_12_obmc_variance8x32 = aom_highbd_12_obmc_variance8x32_c;
aom_highbd_12_obmc_variance32x8 = aom_highbd_12_obmc_variance32x8_c;
aom_highbd_12_obmc_variance16x64 = aom_highbd_12_obmc_variance16x64_c;
aom_highbd_12_obmc_variance64x16 = aom_highbd_12_obmc_variance64x16_c;

#if CONFIG_BLOCK_256
aom_highbd_12_obmc_variance4x32 = aom_highbd_12_obmc_variance4x32_c;
aom_highbd_12_obmc_variance32x4 = aom_highbd_12_obmc_variance32x4_c;
aom_highbd_12_obmc_variance8x64 = aom_highbd_12_obmc_variance8x64_c;
aom_highbd_12_obmc_variance64x8 = aom_highbd_12_obmc_variance64x8_c;
aom_highbd_12_obmc_variance4x64 = aom_highbd_12_obmc_variance4x64_c;
aom_highbd_12_obmc_variance64x4 = aom_highbd_12_obmc_variance64x4_c;
aom_highbd_masked_sad256x256 = aom_highbd_masked_sad256x256_c;
aom_highbd_masked_sad256x128 = aom_highbd_masked_sad256x128_c;
aom_highbd_masked_sad128x256 = aom_highbd_masked_sad128x256_c;
#endif
aom_highbd_masked_sad128x128 = aom_highbd_masked_sad128x128_c;
aom_highbd_masked_sad128x64 = aom_highbd_masked_sad128x64_c;
aom_highbd_masked_sad64x128 = aom_highbd_masked_sad64x128_c;
aom_highbd_masked_sad64x64 = aom_highbd_masked_sad64x64_c;
aom_highbd_masked_sad64x32 = aom_highbd_masked_sad64x32_c;
aom_highbd_masked_sad32x64 = aom_highbd_masked_sad32x64_c;
aom_highbd_masked_sad32x32 = aom_highbd_masked_sad32x32_c;
aom_highbd_masked_sad32x16 = aom_highbd_masked_sad32x16_c;
aom_highbd_masked_sad16x32 = aom_highbd_masked_sad16x32_c;
aom_highbd_masked_sad16x16 = aom_highbd_masked_sad16x16_c;
aom_highbd_masked_sad16x8 = aom_highbd_masked_sad16x8_c;
aom_highbd_masked_sad8x16 = aom_highbd_masked_sad8x16_c;
aom_highbd_masked_sad8x8 = aom_highbd_masked_sad8x8_c;
aom_highbd_masked_sad8x4 = aom_highbd_masked_sad8x4_c;
aom_highbd_masked_sad4x8 = aom_highbd_masked_sad4x8_c;
aom_highbd_masked_sad4x4 = aom_highbd_masked_sad4x4_c;
aom_highbd_masked_sad4x16 = aom_highbd_masked_sad4x16_c;
aom_highbd_masked_sad16x4 = aom_highbd_masked_sad16x4_c;
aom_highbd_masked_sad8x32 = aom_highbd_masked_sad8x32_c;
aom_highbd_masked_sad32x8 = aom_highbd_masked_sad32x8_c;
aom_highbd_masked_sad16x64 = aom_highbd_masked_sad16x64_c;
aom_highbd_masked_sad64x16 = aom_highbd_masked_sad64x16_c;
#if CONFIG_BLOCK_256
aom_highbd_masked_sad4x32 = aom_highbd_masked_sad4x32_c;
aom_highbd_masked_sad32x4 = aom_highbd_masked_sad32x4_c;
aom_highbd_masked_sad8x64 = aom_highbd_masked_sad8x64_c;
aom_highbd_masked_sad64x8 = aom_highbd_masked_sad64x8_c;
aom_highbd_masked_sad4x64 = aom_highbd_masked_sad4x64_c;
aom_highbd_masked_sad64x4 = aom_highbd_masked_sad64x4_c;
aom_highbd_8_masked_sub_pixel_variance256x256 = aom_highbd_8_masked_sub_pixel_variance256x256_c;
aom_highbd_8_masked_sub_pixel_variance256x128 = aom_highbd_8_masked_sub_pixel_variance256x128_c;
aom_highbd_8_masked_sub_pixel_variance128x256 = aom_highbd_8_masked_sub_pixel_variance128x256_c;
#endif
aom_highbd_8_masked_sub_pixel_variance128x128 = aom_highbd_8_masked_sub_pixel_variance128x128_c;
aom_highbd_8_masked_sub_pixel_variance128x64 = aom_highbd_8_masked_sub_pixel_variance128x64_c;
aom_highbd_8_masked_sub_pixel_variance64x128 = aom_highbd_8_masked_sub_pixel_variance64x128_c;
aom_highbd_8_masked_sub_pixel_variance64x64 = aom_highbd_8_masked_sub_pixel_variance64x64_c;
aom_highbd_8_masked_sub_pixel_variance64x32 = aom_highbd_8_masked_sub_pixel_variance64x32_c;
aom_highbd_8_masked_sub_pixel_variance32x64 = aom_highbd_8_masked_sub_pixel_variance32x64_c;
aom_highbd_8_masked_sub_pixel_variance32x32 = aom_highbd_8_masked_sub_pixel_variance32x32_c;
aom_highbd_8_masked_sub_pixel_variance32x16 = aom_highbd_8_masked_sub_pixel_variance32x16_c;
aom_highbd_8_masked_sub_pixel_variance16x32 = aom_highbd_8_masked_sub_pixel_variance16x32_c;
aom_highbd_8_masked_sub_pixel_variance16x16 = aom_highbd_8_masked_sub_pixel_variance16x16_c;
aom_highbd_8_masked_sub_pixel_variance16x8 = aom_highbd_8_masked_sub_pixel_variance16x8_c;
aom_highbd_8_masked_sub_pixel_variance8x16 = aom_highbd_8_masked_sub_pixel_variance8x16_c;
aom_highbd_8_masked_sub_pixel_variance8x8 = aom_highbd_8_masked_sub_pixel_variance8x8_c;
aom_highbd_8_masked_sub_pixel_variance8x4 = aom_highbd_8_masked_sub_pixel_variance8x4_c;
aom_highbd_8_masked_sub_pixel_variance4x8 = aom_highbd_8_masked_sub_pixel_variance4x8_c;
aom_highbd_8_masked_sub_pixel_variance4x4 = aom_highbd_8_masked_sub_pixel_variance4x4_c;
aom_highbd_8_masked_sub_pixel_variance4x16 = aom_highbd_8_masked_sub_pixel_variance4x16_c;
aom_highbd_8_masked_sub_pixel_variance16x4 = aom_highbd_8_masked_sub_pixel_variance16x4_c;
aom_highbd_8_masked_sub_pixel_variance8x32 = aom_highbd_8_masked_sub_pixel_variance8x32_c;
aom_highbd_8_masked_sub_pixel_variance32x8 = aom_highbd_8_masked_sub_pixel_variance32x8_c;
aom_highbd_8_masked_sub_pixel_variance16x64 = aom_highbd_8_masked_sub_pixel_variance16x64_c;
aom_highbd_8_masked_sub_pixel_variance64x16 = aom_highbd_8_masked_sub_pixel_variance64x16_c;
#if CONFIG_BLOCK_256
aom_highbd_8_masked_sub_pixel_variance4x32 = aom_highbd_8_masked_sub_pixel_variance4x32_c;
aom_highbd_8_masked_sub_pixel_variance32x4 = aom_highbd_8_masked_sub_pixel_variance32x4_c;
aom_highbd_8_masked_sub_pixel_variance8x64 = aom_highbd_8_masked_sub_pixel_variance8x64_c;
aom_highbd_8_masked_sub_pixel_variance64x8 = aom_highbd_8_masked_sub_pixel_variance64x8_c;
aom_highbd_8_masked_sub_pixel_variance4x64 = aom_highbd_8_masked_sub_pixel_variance4x64_c;
aom_highbd_8_masked_sub_pixel_variance64x4 = aom_highbd_8_masked_sub_pixel_variance64x4_c;
aom_highbd_10_masked_sub_pixel_variance256x256 = aom_highbd_10_masked_sub_pixel_variance256x256_c;
aom_highbd_10_masked_sub_pixel_variance256x128 = aom_highbd_10_masked_sub_pixel_variance256x128_c;
aom_highbd_10_masked_sub_pixel_variance128x256 = aom_highbd_10_masked_sub_pixel_variance128x256_c;
#endif
aom_highbd_10_masked_sub_pixel_variance128x128 = aom_highbd_10_masked_sub_pixel_variance128x128_c;
aom_highbd_10_masked_sub_pixel_variance128x64 = aom_highbd_10_masked_sub_pixel_variance128x64_c;
aom_highbd_10_masked_sub_pixel_variance64x128 = aom_highbd_10_masked_sub_pixel_variance64x128_c;
aom_highbd_10_masked_sub_pixel_variance64x64 = aom_highbd_10_masked_sub_pixel_variance64x64_c;
aom_highbd_10_masked_sub_pixel_variance64x32 = aom_highbd_10_masked_sub_pixel_variance64x32_c;
aom_highbd_10_masked_sub_pixel_variance32x64 = aom_highbd_10_masked_sub_pixel_variance32x64_c;
aom_highbd_10_masked_sub_pixel_variance32x32 = aom_highbd_10_masked_sub_pixel_variance32x32_c;
aom_highbd_10_masked_sub_pixel_variance32x16 = aom_highbd_10_masked_sub_pixel_variance32x16_c;
aom_highbd_10_masked_sub_pixel_variance16x32 = aom_highbd_10_masked_sub_pixel_variance16x32_c;
aom_highbd_10_masked_sub_pixel_variance16x16 = aom_highbd_10_masked_sub_pixel_variance16x16_c;
aom_highbd_10_masked_sub_pixel_variance16x8 = aom_highbd_10_masked_sub_pixel_variance16x8_c;
aom_highbd_10_masked_sub_pixel_variance8x16 = aom_highbd_10_masked_sub_pixel_variance8x16_c;
aom_highbd_10_masked_sub_pixel_variance8x8 = aom_highbd_10_masked_sub_pixel_variance8x8_c;
aom_highbd_10_masked_sub_pixel_variance8x4 = aom_highbd_10_masked_sub_pixel_variance8x4_c;
aom_highbd_10_masked_sub_pixel_variance4x8 = aom_highbd_10_masked_sub_pixel_variance4x8_c;
aom_highbd_10_masked_sub_pixel_variance4x4 = aom_highbd_10_masked_sub_pixel_variance4x4_c;
aom_highbd_10_masked_sub_pixel_variance4x16 = aom_highbd_10_masked_sub_pixel_variance4x16_c;
aom_highbd_10_masked_sub_pixel_variance16x4 = aom_highbd_10_masked_sub_pixel_variance16x4_c;
aom_highbd_10_masked_sub_pixel_variance8x32 = aom_highbd_10_masked_sub_pixel_variance8x32_c;
aom_highbd_10_masked_sub_pixel_variance32x8 = aom_highbd_10_masked_sub_pixel_variance32x8_c;
aom_highbd_10_masked_sub_pixel_variance16x64 = aom_highbd_10_masked_sub_pixel_variance16x64_c;
aom_highbd_10_masked_sub_pixel_variance64x16 = aom_highbd_10_masked_sub_pixel_variance64x16_c;
#if CONFIG_BLOCK_256
aom_highbd_10_masked_sub_pixel_variance4x32 = aom_highbd_10_masked_sub_pixel_variance4x32_c;
aom_highbd_10_masked_sub_pixel_variance32x4 = aom_highbd_10_masked_sub_pixel_variance32x4_c;
aom_highbd_10_masked_sub_pixel_variance8x64 = aom_highbd_10_masked_sub_pixel_variance8x64_c;
aom_highbd_10_masked_sub_pixel_variance64x8 = aom_highbd_10_masked_sub_pixel_variance64x8_c;
aom_highbd_10_masked_sub_pixel_variance4x64 = aom_highbd_10_masked_sub_pixel_variance4x64_c;
aom_highbd_10_masked_sub_pixel_variance64x4 = aom_highbd_10_masked_sub_pixel_variance64x4_c;
aom_highbd_12_masked_sub_pixel_variance256x256 = aom_highbd_12_masked_sub_pixel_variance256x256_c;
aom_highbd_12_masked_sub_pixel_variance256x128 = aom_highbd_12_masked_sub_pixel_variance256x128_c;
aom_highbd_12_masked_sub_pixel_variance128x256 = aom_highbd_12_masked_sub_pixel_variance128x256_c;
#endif
aom_highbd_12_masked_sub_pixel_variance128x128 = aom_highbd_12_masked_sub_pixel_variance128x128_c;
aom_highbd_12_masked_sub_pixel_variance128x64 = aom_highbd_12_masked_sub_pixel_variance128x64_c;
aom_highbd_12_masked_sub_pixel_variance64x128 = aom_highbd_12_masked_sub_pixel_variance64x128_c;
aom_highbd_12_masked_sub_pixel_variance64x64 = aom_highbd_12_masked_sub_pixel_variance64x64_c;
aom_highbd_12_masked_sub_pixel_variance64x32 = aom_highbd_12_masked_sub_pixel_variance64x32_c;
aom_highbd_12_masked_sub_pixel_variance32x64 = aom_highbd_12_masked_sub_pixel_variance32x64_c;
aom_highbd_12_masked_sub_pixel_variance32x32 = aom_highbd_12_masked_sub_pixel_variance32x32_c;
aom_highbd_12_masked_sub_pixel_variance32x16 = aom_highbd_12_masked_sub_pixel_variance32x16_c;
aom_highbd_12_masked_sub_pixel_variance16x32 = aom_highbd_12_masked_sub_pixel_variance16x32_c;
aom_highbd_12_masked_sub_pixel_variance16x16 = aom_highbd_12_masked_sub_pixel_variance16x16_c;
aom_highbd_12_masked_sub_pixel_variance16x8 = aom_highbd_12_masked_sub_pixel_variance16x8_c;
aom_highbd_12_masked_sub_pixel_variance8x16 = aom_highbd_12_masked_sub_pixel_variance8x16_c;
aom_highbd_12_masked_sub_pixel_variance8x8 = aom_highbd_12_masked_sub_pixel_variance8x8_c;
aom_highbd_12_masked_sub_pixel_variance8x4 = aom_highbd_12_masked_sub_pixel_variance8x4_c;
aom_highbd_12_masked_sub_pixel_variance4x8 = aom_highbd_12_masked_sub_pixel_variance4x8_c;
aom_highbd_12_masked_sub_pixel_variance4x4 = aom_highbd_12_masked_sub_pixel_variance4x4_c;
aom_highbd_12_masked_sub_pixel_variance4x16 = aom_highbd_12_masked_sub_pixel_variance4x16_c;
aom_highbd_12_masked_sub_pixel_variance16x4 = aom_highbd_12_masked_sub_pixel_variance16x4_c;
aom_highbd_12_masked_sub_pixel_variance8x32 = aom_highbd_12_masked_sub_pixel_variance8x32_c;
aom_highbd_12_masked_sub_pixel_variance32x8 = aom_highbd_12_masked_sub_pixel_variance32x8_c;
aom_highbd_12_masked_sub_pixel_variance16x64 = aom_highbd_12_masked_sub_pixel_variance16x64_c;
aom_highbd_12_masked_sub_pixel_variance64x16 = aom_highbd_12_masked_sub_pixel_variance64x16_c;

#if CONFIG_BLOCK_256
aom_highbd_12_masked_sub_pixel_variance4x32 = aom_highbd_12_masked_sub_pixel_variance4x32_c;
aom_highbd_12_masked_sub_pixel_variance32x4 = aom_highbd_12_masked_sub_pixel_variance32x4_c;
aom_highbd_12_masked_sub_pixel_variance8x64 = aom_highbd_12_masked_sub_pixel_variance8x64_c;
aom_highbd_12_masked_sub_pixel_variance64x8 = aom_highbd_12_masked_sub_pixel_variance64x8_c;
aom_highbd_12_masked_sub_pixel_variance4x64 = aom_highbd_12_masked_sub_pixel_variance4x64_c;
aom_highbd_12_masked_sub_pixel_variance64x4 = aom_highbd_12_masked_sub_pixel_variance64x4_c;
aom_highbd_sad256x256x4d = aom_highbd_sad256x256x4d_c;
aom_highbd_sad256x128x4d = aom_highbd_sad256x128x4d_c;
aom_highbd_sad128x256x4d = aom_highbd_sad128x256x4d_c;
#endif
aom_highbd_sad128x128x4d = aom_highbd_sad128x128x4d_c;
aom_highbd_sad128x64x4d = aom_highbd_sad128x64x4d_c;
aom_highbd_sad64x128x4d = aom_highbd_sad64x128x4d_c;

#if CONFIG_BLOCK_256
aom_highbd_sad_skip_256x256x4d = aom_highbd_sad_skip_256x256x4d_c;
aom_highbd_sad_skip_256x128x4d = aom_highbd_sad_skip_256x128x4d_c;
aom_highbd_sad_skip_128x256x4d = aom_highbd_sad_skip_128x256x4d_c;
#endif
aom_highbd_sad_skip_128x128x4d = aom_highbd_sad_skip_128x128x4d_c;
aom_highbd_sad_skip_128x64x4d = aom_highbd_sad_skip_128x64x4d_c;
aom_highbd_sad_skip_64x128x4d = aom_highbd_sad_skip_64x128x4d_c;

aom_highbd_hadamard_8x8 = aom_highbd_hadamard_8x8_c;
aom_highbd_hadamard_16x16 = aom_highbd_hadamard_16x16_c;
aom_highbd_hadamard_32x32 = aom_highbd_hadamard_32x32_c;
aom_satd = aom_satd_c;
aom_satd_lp = aom_satd_lp_c;
#endif
#if defined(__SSE2__) && HAVE_SIMD
//aom_highbd_8_variance128x128 = aom_highbd_8_variance128x128_sse2;
//aom_highbd_8_variance128x64 = aom_highbd_8_variance128x64_sse2;
//aom_highbd_8_variance64x128 = aom_highbd_8_variance64x128_sse2;
aom_highbd_8_variance64x64 = aom_highbd_8_variance64x64_sse2;
aom_highbd_8_variance64x32 = aom_highbd_8_variance64x32_sse2;
aom_highbd_8_variance32x64 = aom_highbd_8_variance32x64_sse2;
aom_highbd_8_variance32x32 = aom_highbd_8_variance32x32_sse2;
aom_highbd_8_variance32x16 = aom_highbd_8_variance32x16_sse2;
aom_highbd_8_variance16x32 = aom_highbd_8_variance16x32_sse2;
aom_highbd_8_variance16x16 = aom_highbd_8_variance16x16_sse2;
aom_highbd_8_variance16x8 = aom_highbd_8_variance16x8_sse2;
aom_highbd_8_variance8x16 = aom_highbd_8_variance8x16_sse2;
aom_highbd_8_variance8x8 = aom_highbd_8_variance8x8_sse2;
aom_highbd_8_variance8x32 = aom_highbd_8_variance8x32_sse2;
aom_highbd_8_variance32x8 = aom_highbd_8_variance32x8_sse2;
aom_highbd_8_variance16x64 = aom_highbd_8_variance16x64_sse2;
aom_highbd_8_variance64x16 = aom_highbd_8_variance64x16_sse2;
//aom_highbd_10_variance128x128 = aom_highbd_10_variance128x128_sse2;
//aom_highbd_10_variance128x64 = aom_highbd_10_variance128x64_sse2;
//aom_highbd_10_variance64x128 = aom_highbd_10_variance64x128_sse2;
aom_highbd_10_variance64x64 = aom_highbd_10_variance64x64_sse2;
aom_highbd_10_variance64x32 = aom_highbd_10_variance64x32_sse2;
aom_highbd_10_variance32x64 = aom_highbd_10_variance32x64_sse2;
aom_highbd_10_variance32x32 = aom_highbd_10_variance32x32_sse2;
aom_highbd_10_variance32x16 = aom_highbd_10_variance32x16_sse2;
aom_highbd_10_variance16x32 = aom_highbd_10_variance16x32_sse2;
aom_highbd_10_variance16x16 = aom_highbd_10_variance16x16_sse2;
aom_highbd_10_variance16x8 = aom_highbd_10_variance16x8_sse2;
aom_highbd_10_variance8x16 = aom_highbd_10_variance8x16_sse2;
aom_highbd_10_variance8x8 = aom_highbd_10_variance8x8_sse2;
aom_highbd_10_variance8x32 = aom_highbd_10_variance8x32_sse2;
aom_highbd_10_variance32x8 = aom_highbd_10_variance32x8_sse2;
aom_highbd_10_variance16x64 = aom_highbd_10_variance16x64_sse2;
aom_highbd_10_variance64x16 = aom_highbd_10_variance64x16_sse2;
//aom_highbd_12_variance128x128 = aom_highbd_12_variance128x128_sse2;
//aom_highbd_12_variance128x64 = aom_highbd_12_variance128x64_sse2;
//aom_highbd_12_variance64x128 = aom_highbd_12_variance64x128_sse2;
aom_highbd_12_variance64x64 = aom_highbd_12_variance64x64_sse2;
aom_highbd_12_variance64x32 = aom_highbd_12_variance64x32_sse2;
aom_highbd_12_variance32x64 = aom_highbd_12_variance32x64_sse2;
aom_highbd_12_variance32x32 = aom_highbd_12_variance32x32_sse2;
aom_highbd_12_variance32x16 = aom_highbd_12_variance32x16_sse2;
aom_highbd_12_variance16x32 = aom_highbd_12_variance16x32_sse2;
aom_highbd_12_variance16x16 = aom_highbd_12_variance16x16_sse2;
aom_highbd_12_variance16x8 = aom_highbd_12_variance16x8_sse2;
aom_highbd_12_variance8x16 = aom_highbd_12_variance8x16_sse2;
aom_highbd_12_variance8x8 = aom_highbd_12_variance8x8_sse2;
aom_highbd_12_variance8x32 = aom_highbd_12_variance8x32_sse2;
aom_highbd_12_variance32x8 = aom_highbd_12_variance32x8_sse2;
aom_highbd_12_variance16x64 = aom_highbd_12_variance16x64_sse2;
aom_highbd_12_variance64x16 = aom_highbd_12_variance64x16_sse2;

aom_highbd_8_sub_pixel_variance64x64 = aom_highbd_8_sub_pixel_variance64x64_sse2;
aom_highbd_8_sub_pixel_variance64x32 = aom_highbd_8_sub_pixel_variance64x32_sse2;
aom_highbd_8_sub_pixel_variance32x64 = aom_highbd_8_sub_pixel_variance32x64_sse2;
aom_highbd_8_sub_pixel_variance32x32 = aom_highbd_8_sub_pixel_variance32x32_sse2;
aom_highbd_8_sub_pixel_variance32x16 = aom_highbd_8_sub_pixel_variance32x16_sse2;
#if !CONFIG_FLEX_PARTITION
aom_highbd_sad16x32 = aom_highbd_sad16x32_sse2;
aom_highbd_sad16x64 = aom_highbd_sad16x64_sse2;
aom_highbd_sad_skip_16x32 = aom_highbd_sad_skip_16x32_sse2;
aom_highbd_sad_skip_16x64 = aom_highbd_sad_skip_16x64_sse2;
aom_highbd_sad16x32_avg = aom_highbd_sad16x32_avg_sse2;
aom_highbd_sad16x64_avg = aom_highbd_sad16x64_avg_sse2;
aom_highbd_8_sub_pixel_variance16x32 = aom_highbd_8_sub_pixel_variance16x32_sse2;
aom_highbd_8_sub_pixel_variance16x64 = aom_highbd_8_sub_pixel_variance16x64_sse2;
aom_highbd_10_sub_pixel_variance16x32 = aom_highbd_10_sub_pixel_variance16x32_sse2;
aom_highbd_10_sub_pixel_variance16x64 = aom_highbd_10_sub_pixel_variance16x64_sse2;
aom_highbd_12_sub_pixel_variance16x32 = aom_highbd_12_sub_pixel_variance16x32_sse2;
aom_highbd_12_sub_pixel_variance16x64 = aom_highbd_12_sub_pixel_variance16x64_sse2;
#else
aom_highbd_8_variance64x8 = aom_highbd_8_variance64x8_sse2;
aom_highbd_10_variance64x8 = aom_highbd_10_variance64x8_sse2;
aom_highbd_12_variance64x8 = aom_highbd_12_variance64x8_sse2;
aom_highbd_8_variance8x64 = aom_highbd_8_variance8x64_sse2;
aom_highbd_10_variance8x64 = aom_highbd_10_variance8x64_sse2;
aom_highbd_12_variance8x64 = aom_highbd_12_variance8x64_sse2;
#endif
aom_highbd_8_sub_pixel_variance8x32 = aom_highbd_8_sub_pixel_variance8x32_sse2;
aom_highbd_8_sub_pixel_variance32x8 = aom_highbd_8_sub_pixel_variance32x8_sse2;
aom_highbd_8_sub_pixel_variance64x16 = aom_highbd_8_sub_pixel_variance64x16_sse2;
aom_highbd_10_sub_pixel_variance64x64 = aom_highbd_10_sub_pixel_variance64x64_sse2;
aom_highbd_10_sub_pixel_variance64x32 = aom_highbd_10_sub_pixel_variance64x32_sse2;
aom_highbd_10_sub_pixel_variance32x64 = aom_highbd_10_sub_pixel_variance32x64_sse2;
aom_highbd_10_sub_pixel_variance32x32 = aom_highbd_10_sub_pixel_variance32x32_sse2;
aom_highbd_10_sub_pixel_variance32x16 = aom_highbd_10_sub_pixel_variance32x16_sse2;
aom_highbd_10_sub_pixel_variance8x32 = aom_highbd_10_sub_pixel_variance8x32_sse2;
aom_highbd_10_sub_pixel_variance32x8 = aom_highbd_10_sub_pixel_variance32x8_sse2;
aom_highbd_10_sub_pixel_variance64x16 = aom_highbd_10_sub_pixel_variance64x16_sse2;
aom_highbd_12_sub_pixel_variance64x64 = aom_highbd_12_sub_pixel_variance64x64_sse2;
aom_highbd_12_sub_pixel_variance64x32 = aom_highbd_12_sub_pixel_variance64x32_sse2;
aom_highbd_12_sub_pixel_variance32x64 = aom_highbd_12_sub_pixel_variance32x64_sse2;
aom_highbd_12_sub_pixel_variance32x32 = aom_highbd_12_sub_pixel_variance32x32_sse2;
aom_highbd_12_sub_pixel_variance32x16 = aom_highbd_12_sub_pixel_variance32x16_sse2;
aom_highbd_12_sub_pixel_variance8x32 = aom_highbd_12_sub_pixel_variance8x32_sse2;
aom_highbd_12_sub_pixel_variance32x8 = aom_highbd_12_sub_pixel_variance32x8_sse2;
aom_highbd_12_sub_pixel_variance64x16 = aom_highbd_12_sub_pixel_variance64x16_sse2;

aom_convolve_copy = aom_convolve_copy_sse2;
aom_highbd_convolve_copy = aom_highbd_convolve_copy_sse2;
aom_highbd_convolve8_horiz = aom_highbd_convolve8_horiz_sse2;
aom_highbd_convolve8_vert = aom_highbd_convolve8_vert_sse2;
aom_fft8x8_float = aom_fft8x8_float_sse2;
aom_fft16x16_float = aom_fft16x16_float_sse2;
aom_fft32x32_float = aom_fft32x32_float_sse2;
aom_ifft8x8_float = aom_ifft8x8_float_sse2;
aom_ifft16x16_float = aom_ifft16x16_float_sse2;
aom_ifft32x32_float = aom_ifft32x32_float_sse2;
aom_get_blk_sse_sum = aom_get_blk_sse_sum_sse2;
aom_sum_squares_2d_i16 = aom_sum_squares_2d_i16_sse2;
aom_var_2d_u8 = aom_var_2d_u8_sse2;
aom_var_2d_u16 = aom_var_2d_u16_sse2;
aom_sum_sse_2d_i16 = aom_sum_sse_2d_i16_sse2;
aom_highbd_sad64x64 = aom_highbd_sad64x64_sse2;
aom_highbd_sad64x32 = aom_highbd_sad64x32_sse2;
aom_highbd_sad32x64 = aom_highbd_sad32x64_sse2;
aom_highbd_sad32x32 = aom_highbd_sad32x32_sse2;
aom_highbd_sad32x16 = aom_highbd_sad32x16_sse2;
aom_highbd_sad32x8 = aom_highbd_sad32x8_sse2;
aom_highbd_sad64x16 = aom_highbd_sad64x16_sse2;
aom_highbd_sad_skip_64x64 = aom_highbd_sad_skip_64x64_sse2;
aom_highbd_sad_skip_64x32 = aom_highbd_sad_skip_64x32_sse2;
aom_highbd_sad_skip_32x64 = aom_highbd_sad_skip_32x64_sse2;
aom_highbd_sad_skip_32x32 = aom_highbd_sad_skip_32x32_sse2;
aom_highbd_sad_skip_32x16 = aom_highbd_sad_skip_32x16_sse2;

#if !CONFIG_UNEVEN_4WAY
aom_highbd_8_sub_pixel_variance16x16 = aom_highbd_8_sub_pixel_variance16x16_sse2;
aom_highbd_8_sub_pixel_variance16x8 = aom_highbd_8_sub_pixel_variance16x8_sse2;
aom_highbd_8_sub_pixel_variance16x4 = aom_highbd_8_sub_pixel_variance16x4_sse2;
aom_highbd_10_sub_pixel_variance16x16 = aom_highbd_10_sub_pixel_variance16x16_sse2;
aom_highbd_10_sub_pixel_variance16x8 = aom_highbd_10_sub_pixel_variance16x8_sse2;
aom_highbd_10_sub_pixel_variance16x4 = aom_highbd_10_sub_pixel_variance16x4_sse2;
aom_highbd_12_sub_pixel_variance16x16 = aom_highbd_12_sub_pixel_variance16x16_sse2;
aom_highbd_12_sub_pixel_variance16x8 = aom_highbd_12_sub_pixel_variance16x8_sse2;
aom_highbd_12_sub_pixel_variance16x4 = aom_highbd_12_sub_pixel_variance16x4_sse2;

aom_highbd_sad16x16 = aom_highbd_sad16x16_sse2;
aom_highbd_sad16x8 = aom_highbd_sad16x8_sse2;
aom_highbd_sad16x4 = aom_highbd_sad16x4_sse2;
aom_highbd_sad16x16_avg = aom_highbd_sad16x16_avg_sse2;
aom_highbd_sad16x8_avg = aom_highbd_sad16x8_avg_sse2;
aom_highbd_sad_skip_16x16 = aom_highbd_sad_skip_16x16_sse2;
aom_highbd_sad_skip_16x8 = aom_highbd_sad_skip_16x8_sse2;
#endif
aom_highbd_sad_skip_32x8 = aom_highbd_sad_skip_32x8_sse2;
aom_highbd_sad_skip_64x16 = aom_highbd_sad_skip_64x16_sse2;
aom_highbd_sad64x64_avg = aom_highbd_sad64x64_avg_sse2;
aom_highbd_sad64x32_avg = aom_highbd_sad64x32_avg_sse2;
aom_highbd_sad32x64_avg = aom_highbd_sad32x64_avg_sse2;
aom_highbd_sad32x32_avg = aom_highbd_sad32x32_avg_sse2;
aom_highbd_sad32x16_avg = aom_highbd_sad32x16_avg_sse2;
aom_highbd_sad16x4_avg = aom_highbd_sad16x4_avg_sse2;
aom_highbd_sad32x8_avg = aom_highbd_sad32x8_avg_sse2;
aom_highbd_sad64x16_avg = aom_highbd_sad64x16_avg_sse2;
aom_highbd_sad64x64x4d = aom_highbd_sad64x64x4d_sse2;
aom_highbd_sad64x32x4d = aom_highbd_sad64x32x4d_sse2;
aom_highbd_sad32x64x4d = aom_highbd_sad32x64x4d_sse2;
aom_highbd_sad32x32x4d = aom_highbd_sad32x32x4d_sse2;
aom_highbd_sad32x16x4d = aom_highbd_sad32x16x4d_sse2;
aom_highbd_sad16x32x4d = aom_highbd_sad16x32x4d_sse2;
aom_highbd_sad16x16x4d = aom_highbd_sad16x16x4d_sse2;
aom_highbd_sad16x8x4d = aom_highbd_sad16x8x4d_sse2;
aom_highbd_sad16x4x4d = aom_highbd_sad16x4x4d_sse2;
aom_highbd_sad32x8x4d = aom_highbd_sad32x8x4d_sse2;
aom_highbd_sad16x64x4d = aom_highbd_sad16x64x4d_sse2;
aom_highbd_sad64x16x4d = aom_highbd_sad64x16x4d_sse2;
aom_highbd_sad_skip_64x64x4d = aom_highbd_sad_skip_64x64x4d_sse2;
aom_highbd_sad_skip_64x32x4d = aom_highbd_sad_skip_64x32x4d_sse2;
aom_highbd_sad_skip_32x64x4d = aom_highbd_sad_skip_32x64x4d_sse2;
aom_highbd_sad_skip_32x32x4d = aom_highbd_sad_skip_32x32x4d_sse2;
aom_highbd_sad_skip_32x16x4d = aom_highbd_sad_skip_32x16x4d_sse2;
aom_highbd_sad_skip_16x32x4d = aom_highbd_sad_skip_16x32x4d_sse2;
aom_highbd_sad_skip_16x16x4d = aom_highbd_sad_skip_16x16x4d_sse2;
aom_highbd_sad_skip_16x8x4d = aom_highbd_sad_skip_16x8x4d_sse2;
aom_highbd_sad_skip_32x8x4d = aom_highbd_sad_skip_32x8x4d_sse2;
aom_highbd_sad_skip_16x64x4d = aom_highbd_sad_skip_16x64x4d_sse2;
aom_highbd_sad_skip_64x16x4d = aom_highbd_sad_skip_64x16x4d_sse2;
aom_mse_wxh_16bit_highbd = aom_mse_wxh_16bit_highbd_sse2;
aom_highbd_comp_mask_pred = aom_highbd_comp_mask_pred_sse2;
#endif
#if defined(__SSSE3__) && HAVE_SIMD
  if (flags & HAS_SSSE3) aom_highbd_masked_sad128x128 = aom_highbd_masked_sad128x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad128x64 = aom_highbd_masked_sad128x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad64x128 = aom_highbd_masked_sad64x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad64x64 = aom_highbd_masked_sad64x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad64x32 = aom_highbd_masked_sad64x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad32x64 = aom_highbd_masked_sad32x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad32x32 = aom_highbd_masked_sad32x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad32x16 = aom_highbd_masked_sad32x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad16x32 = aom_highbd_masked_sad16x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad16x16 = aom_highbd_masked_sad16x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad16x8 = aom_highbd_masked_sad16x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad8x16 = aom_highbd_masked_sad8x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad8x8 = aom_highbd_masked_sad8x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad8x4 = aom_highbd_masked_sad8x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad4x8 = aom_highbd_masked_sad4x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad4x4 = aom_highbd_masked_sad4x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad4x16 = aom_highbd_masked_sad4x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad16x4 = aom_highbd_masked_sad16x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad8x32 = aom_highbd_masked_sad8x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad32x8 = aom_highbd_masked_sad32x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad8x64 = aom_highbd_masked_sad8x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad64x8 = aom_highbd_masked_sad64x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad4x64 = aom_highbd_masked_sad4x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad64x4 = aom_highbd_masked_sad64x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad4x32 = aom_highbd_masked_sad4x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad32x4 = aom_highbd_masked_sad32x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad16x64 = aom_highbd_masked_sad16x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_masked_sad64x16 = aom_highbd_masked_sad64x16_ssse3;
#if CONFIG_BLOCK_256
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance256x256 = aom_highbd_8_masked_sub_pixel_variance256x256_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance256x128 = aom_highbd_8_masked_sub_pixel_variance256x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance128x256 = aom_highbd_8_masked_sub_pixel_variance128x256_ssse3;
#endif
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance128x128 = aom_highbd_8_masked_sub_pixel_variance128x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance128x64 = aom_highbd_8_masked_sub_pixel_variance128x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance64x128 = aom_highbd_8_masked_sub_pixel_variance64x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance64x64 = aom_highbd_8_masked_sub_pixel_variance64x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance64x32 = aom_highbd_8_masked_sub_pixel_variance64x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance32x64 = aom_highbd_8_masked_sub_pixel_variance32x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance32x32 = aom_highbd_8_masked_sub_pixel_variance32x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance32x16 = aom_highbd_8_masked_sub_pixel_variance32x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance16x32 = aom_highbd_8_masked_sub_pixel_variance16x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance16x16 = aom_highbd_8_masked_sub_pixel_variance16x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance16x8 = aom_highbd_8_masked_sub_pixel_variance16x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance8x16 = aom_highbd_8_masked_sub_pixel_variance8x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance8x8 = aom_highbd_8_masked_sub_pixel_variance8x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance8x4 = aom_highbd_8_masked_sub_pixel_variance8x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance4x8 = aom_highbd_8_masked_sub_pixel_variance4x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance4x4 = aom_highbd_8_masked_sub_pixel_variance4x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance4x16 = aom_highbd_8_masked_sub_pixel_variance4x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance16x4 = aom_highbd_8_masked_sub_pixel_variance16x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance8x32 = aom_highbd_8_masked_sub_pixel_variance8x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance32x8 = aom_highbd_8_masked_sub_pixel_variance32x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance16x64 = aom_highbd_8_masked_sub_pixel_variance16x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance64x16 = aom_highbd_8_masked_sub_pixel_variance64x16_ssse3;
#if CONFIG_BLOCK_256
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance4x32 = aom_highbd_8_masked_sub_pixel_variance4x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance32x4 = aom_highbd_8_masked_sub_pixel_variance32x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance8x64 = aom_highbd_8_masked_sub_pixel_variance8x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance64x8 = aom_highbd_8_masked_sub_pixel_variance64x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance8x64 = aom_highbd_8_masked_sub_pixel_variance4x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_8_masked_sub_pixel_variance64x8 = aom_highbd_8_masked_sub_pixel_variance64x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance256x256 = aom_highbd_10_masked_sub_pixel_variance256x256_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance256x128 = aom_highbd_10_masked_sub_pixel_variance256x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance128x256 = aom_highbd_10_masked_sub_pixel_variance128x256_ssse3;
#endif
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance128x128 = aom_highbd_10_masked_sub_pixel_variance128x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance128x64 = aom_highbd_10_masked_sub_pixel_variance128x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance64x128 = aom_highbd_10_masked_sub_pixel_variance64x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance64x64 = aom_highbd_10_masked_sub_pixel_variance64x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance64x32 = aom_highbd_10_masked_sub_pixel_variance64x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance32x64 = aom_highbd_10_masked_sub_pixel_variance32x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance32x32 = aom_highbd_10_masked_sub_pixel_variance32x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance32x16 = aom_highbd_10_masked_sub_pixel_variance32x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance16x32 = aom_highbd_10_masked_sub_pixel_variance16x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance16x16 = aom_highbd_10_masked_sub_pixel_variance16x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance16x8 = aom_highbd_10_masked_sub_pixel_variance16x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance8x16 = aom_highbd_10_masked_sub_pixel_variance8x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance8x8 = aom_highbd_10_masked_sub_pixel_variance8x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance8x4 = aom_highbd_10_masked_sub_pixel_variance8x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance4x8 = aom_highbd_10_masked_sub_pixel_variance4x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance4x4 = aom_highbd_10_masked_sub_pixel_variance4x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance4x16 = aom_highbd_10_masked_sub_pixel_variance4x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance16x4 = aom_highbd_10_masked_sub_pixel_variance16x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance8x32 = aom_highbd_10_masked_sub_pixel_variance8x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance32x8 = aom_highbd_10_masked_sub_pixel_variance32x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance16x64 = aom_highbd_10_masked_sub_pixel_variance16x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance64x16 = aom_highbd_10_masked_sub_pixel_variance64x16_ssse3;
#if CONFIG_BLOCK_256
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance4x32 = aom_highbd_10_masked_sub_pixel_variance4x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance32x4 = aom_highbd_10_masked_sub_pixel_variance32x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance8x64 = aom_highbd_10_masked_sub_pixel_variance8x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance64x8 = aom_highbd_10_masked_sub_pixel_variance64x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance4x64 = aom_highbd_10_masked_sub_pixel_variance4x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_10_masked_sub_pixel_variance64x4 = aom_highbd_10_masked_sub_pixel_variance64x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance256x256 = aom_highbd_12_masked_sub_pixel_variance256x256_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance256x128 = aom_highbd_12_masked_sub_pixel_variance256x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance128x256 = aom_highbd_12_masked_sub_pixel_variance128x256_ssse3;
#endif
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance128x128 = aom_highbd_12_masked_sub_pixel_variance128x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance128x64 = aom_highbd_12_masked_sub_pixel_variance128x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance64x128 = aom_highbd_12_masked_sub_pixel_variance64x128_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance64x64 = aom_highbd_12_masked_sub_pixel_variance64x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance64x32 = aom_highbd_12_masked_sub_pixel_variance64x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance32x64 = aom_highbd_12_masked_sub_pixel_variance32x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance32x32 = aom_highbd_12_masked_sub_pixel_variance32x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance32x16 = aom_highbd_12_masked_sub_pixel_variance32x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance16x32 = aom_highbd_12_masked_sub_pixel_variance16x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance16x16 = aom_highbd_12_masked_sub_pixel_variance16x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance16x8 = aom_highbd_12_masked_sub_pixel_variance16x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance8x16 = aom_highbd_12_masked_sub_pixel_variance8x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance8x8 = aom_highbd_12_masked_sub_pixel_variance8x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance8x4 = aom_highbd_12_masked_sub_pixel_variance8x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance4x8 = aom_highbd_12_masked_sub_pixel_variance4x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance4x4 = aom_highbd_12_masked_sub_pixel_variance4x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance4x16 = aom_highbd_12_masked_sub_pixel_variance4x16_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance16x4 = aom_highbd_12_masked_sub_pixel_variance16x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance8x32 = aom_highbd_12_masked_sub_pixel_variance8x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance32x8 = aom_highbd_12_masked_sub_pixel_variance32x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance16x64 = aom_highbd_12_masked_sub_pixel_variance16x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance64x16 = aom_highbd_12_masked_sub_pixel_variance64x16_ssse3;
#if CONFIG_BLOCK_256
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance4x32 = aom_highbd_12_masked_sub_pixel_variance4x32_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance32x4 = aom_highbd_12_masked_sub_pixel_variance32x4_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance8x64 = aom_highbd_12_masked_sub_pixel_variance8x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance64x8 = aom_highbd_12_masked_sub_pixel_variance64x8_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance4x64 = aom_highbd_12_masked_sub_pixel_variance4x64_ssse3;
  if (flags & HAS_SSSE3) aom_highbd_12_masked_sub_pixel_variance64x4 = aom_highbd_12_masked_sub_pixel_variance64x4_ssse3;
#endif
#endif
#if defined(__SSE4_1__) && HAVE_SIMD
  if (flags & HAS_SSE4_1) aom_highbd_blend_a64_mask = aom_highbd_blend_a64_mask_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_blend_a64_hmask = aom_highbd_blend_a64_hmask_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_blend_a64_vmask = aom_highbd_blend_a64_vmask_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_blend_a64_d16_mask = aom_highbd_blend_a64_d16_mask_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_sse = aom_highbd_sse_sse4_1;

  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad128x128 = aom_highbd_obmc_sad128x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad128x64 = aom_highbd_obmc_sad128x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad64x128 = aom_highbd_obmc_sad64x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad64x64 = aom_highbd_obmc_sad64x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad64x32 = aom_highbd_obmc_sad64x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad64x16 = aom_highbd_obmc_sad64x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad32x64 = aom_highbd_obmc_sad32x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad32x32 = aom_highbd_obmc_sad32x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad32x16 = aom_highbd_obmc_sad32x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad32x8 = aom_highbd_obmc_sad32x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad16x64 = aom_highbd_obmc_sad16x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad16x32 = aom_highbd_obmc_sad16x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad16x16 = aom_highbd_obmc_sad16x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad16x8 = aom_highbd_obmc_sad16x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad16x4 = aom_highbd_obmc_sad16x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad8x32 = aom_highbd_obmc_sad8x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad8x16 = aom_highbd_obmc_sad8x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad8x8 = aom_highbd_obmc_sad8x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad8x4 = aom_highbd_obmc_sad8x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad4x16 = aom_highbd_obmc_sad4x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad4x8 = aom_highbd_obmc_sad4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad4x4 = aom_highbd_obmc_sad4x4_sse4_1;

  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad4x64 = aom_highbd_obmc_sad4x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad64x4 = aom_highbd_obmc_sad64x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad4x32 = aom_highbd_obmc_sad4x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad32x4 = aom_highbd_obmc_sad32x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad8x64 = aom_highbd_obmc_sad8x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_sad64x8 = aom_highbd_obmc_sad64x8_sse4_1;

  //if (flags & HAS_SSE4_1) aom_highbd_8_variance8x4 = aom_highbd_8_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_variance4x8 = aom_highbd_8_variance4x8_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_variance4x4 = aom_highbd_8_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_variance16x4 = aom_highbd_8_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_variance4x16 = aom_highbd_8_variance4x16_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_variance8x4 = aom_highbd_10_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_variance4x8 = aom_highbd_10_variance4x8_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_variance4x4 = aom_highbd_10_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_variance16x4 = aom_highbd_10_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_variance4x16 = aom_highbd_10_variance4x16_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_variance8x4 = aom_highbd_12_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_variance4x8 = aom_highbd_12_variance4x8_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_variance4x4 = aom_highbd_12_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_variance16x4 = aom_highbd_12_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_variance4x16 = aom_highbd_12_variance4x16_sse4_1;

  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_variance8x4 = aom_highbd_8_sub_pixel_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_variance4x8 = aom_highbd_8_sub_pixel_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_variance4x4 = aom_highbd_8_sub_pixel_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_variance16x4 = aom_highbd_8_sub_pixel_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_variance4x16 = aom_highbd_8_sub_pixel_variance4x16_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_variance8x4 = aom_highbd_10_sub_pixel_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_variance4x8 = aom_highbd_10_sub_pixel_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_variance4x4 = aom_highbd_10_sub_pixel_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_variance16x4 = aom_highbd_10_sub_pixel_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_variance4x16 = aom_highbd_10_sub_pixel_variance4x16_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_variance8x4 = aom_highbd_12_sub_pixel_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_variance4x8 = aom_highbd_12_sub_pixel_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_variance4x4 = aom_highbd_12_sub_pixel_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_variance16x4 = aom_highbd_12_sub_pixel_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_variance4x16 = aom_highbd_12_sub_pixel_variance4x16_sse4_1;

  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_avg_variance8x4 = aom_highbd_8_sub_pixel_avg_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_avg_variance4x8 = aom_highbd_8_sub_pixel_avg_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_avg_variance4x4 = aom_highbd_8_sub_pixel_avg_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_avg_variance16x4 = aom_highbd_8_sub_pixel_avg_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_8_sub_pixel_avg_variance4x16 = aom_highbd_8_sub_pixel_avg_variance4x16_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_avg_variance8x4 = aom_highbd_10_sub_pixel_avg_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_avg_variance4x8 = aom_highbd_10_sub_pixel_avg_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_avg_variance4x4 = aom_highbd_10_sub_pixel_avg_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_avg_variance16x4 = aom_highbd_10_sub_pixel_avg_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_10_sub_pixel_avg_variance4x16 = aom_highbd_10_sub_pixel_avg_variance4x16_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_avg_variance8x4 = aom_highbd_12_sub_pixel_avg_variance8x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_avg_variance4x8 = aom_highbd_12_sub_pixel_avg_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_avg_variance4x4 = aom_highbd_12_sub_pixel_avg_variance4x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_avg_variance16x4 = aom_highbd_12_sub_pixel_avg_variance16x4_sse4_1;
  //if (flags & HAS_SSE4_1) aom_highbd_12_sub_pixel_avg_variance4x16 = aom_highbd_12_sub_pixel_avg_variance4x16_sse4_1;

#if CONFIG_BLOCK_256
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance256x256 = aom_highbd_obmc_variance256x256_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance256x128 = aom_highbd_obmc_variance256x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance128x256 = aom_highbd_obmc_variance128x256_sse4_1;
#endif
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance128x128 = aom_highbd_obmc_variance128x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance128x64 = aom_highbd_obmc_variance128x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance64x128 = aom_highbd_obmc_variance64x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance64x64 = aom_highbd_obmc_variance64x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance64x32 = aom_highbd_obmc_variance64x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance32x64 = aom_highbd_obmc_variance32x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance32x32 = aom_highbd_obmc_variance32x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance32x16 = aom_highbd_obmc_variance32x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance16x32 = aom_highbd_obmc_variance16x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance16x16 = aom_highbd_obmc_variance16x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance16x8 = aom_highbd_obmc_variance16x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance8x16 = aom_highbd_obmc_variance8x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance8x8 = aom_highbd_obmc_variance8x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance8x4 = aom_highbd_obmc_variance8x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance4x8 = aom_highbd_obmc_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance4x4 = aom_highbd_obmc_variance4x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance4x16 = aom_highbd_obmc_variance4x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance16x4 = aom_highbd_obmc_variance16x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance8x32 = aom_highbd_obmc_variance8x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance32x8 = aom_highbd_obmc_variance32x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance16x64 = aom_highbd_obmc_variance16x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance64x16 = aom_highbd_obmc_variance64x16_sse4_1;
#if CONFIG_BLOCK_256
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance8x64 = aom_highbd_obmc_variance8x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance64x4 = aom_highbd_obmc_variance64x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance4x64 = aom_highbd_obmc_variance4x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_obmc_variance64x8 = aom_highbd_obmc_variance64x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance256x256 = aom_highbd_10_obmc_variance256x256_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance256x128 = aom_highbd_10_obmc_variance256x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance128x256 = aom_highbd_10_obmc_variance128x256_sse4_1;
#endif
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance128x128 = aom_highbd_10_obmc_variance128x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance128x64 = aom_highbd_10_obmc_variance128x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance64x128 = aom_highbd_10_obmc_variance64x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance64x64 = aom_highbd_10_obmc_variance64x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance64x32 = aom_highbd_10_obmc_variance64x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance32x64 = aom_highbd_10_obmc_variance32x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance32x32 = aom_highbd_10_obmc_variance32x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance32x16 = aom_highbd_10_obmc_variance32x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance16x32 = aom_highbd_10_obmc_variance16x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance16x16 = aom_highbd_10_obmc_variance16x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance16x8 = aom_highbd_10_obmc_variance16x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance8x16 = aom_highbd_10_obmc_variance8x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance8x8 = aom_highbd_10_obmc_variance8x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance8x4 = aom_highbd_10_obmc_variance8x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance4x8 = aom_highbd_10_obmc_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance4x4 = aom_highbd_10_obmc_variance4x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance4x16 = aom_highbd_10_obmc_variance4x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance16x4 = aom_highbd_10_obmc_variance16x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance8x32 = aom_highbd_10_obmc_variance8x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance32x8 = aom_highbd_10_obmc_variance32x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance16x64 = aom_highbd_10_obmc_variance16x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance64x16 = aom_highbd_10_obmc_variance64x16_sse4_1;
#if CONFIG_BLOCK_256
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance8x64 = aom_highbd_10_obmc_variance8x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance64x4 = aom_highbd_10_obmc_variance64x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance4x64 = aom_highbd_10_obmc_variance4x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_10_obmc_variance64x8 = aom_highbd_10_obmc_variance64x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance256x256 = aom_highbd_12_obmc_variance256x256_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance256x128 = aom_highbd_12_obmc_variance256x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance128x256 = aom_highbd_12_obmc_variance128x256_sse4_1;
#endif
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance128x128 = aom_highbd_12_obmc_variance128x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance128x64 = aom_highbd_12_obmc_variance128x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance64x128 = aom_highbd_12_obmc_variance64x128_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance64x64 = aom_highbd_12_obmc_variance64x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance64x32 = aom_highbd_12_obmc_variance64x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance32x64 = aom_highbd_12_obmc_variance32x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance32x32 = aom_highbd_12_obmc_variance32x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance32x16 = aom_highbd_12_obmc_variance32x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance16x32 = aom_highbd_12_obmc_variance16x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance16x16 = aom_highbd_12_obmc_variance16x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance16x8 = aom_highbd_12_obmc_variance16x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance8x16 = aom_highbd_12_obmc_variance8x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance8x8 = aom_highbd_12_obmc_variance8x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance8x4 = aom_highbd_12_obmc_variance8x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance4x8 = aom_highbd_12_obmc_variance4x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance4x4 = aom_highbd_12_obmc_variance4x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance4x16 = aom_highbd_12_obmc_variance4x16_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance16x4 = aom_highbd_12_obmc_variance16x4_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance8x32 = aom_highbd_12_obmc_variance8x32_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance32x8 = aom_highbd_12_obmc_variance32x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance16x64 = aom_highbd_12_obmc_variance16x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance64x16 = aom_highbd_12_obmc_variance64x16_sse4_1;
#if CONFIG_BLOCK_256
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance8x64 = aom_highbd_12_obmc_variance8x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance64x4 = aom_highbd_12_obmc_variance64x8_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance4x64 = aom_highbd_12_obmc_variance4x64_sse4_1;
  if (flags & HAS_SSE4_1) aom_highbd_12_obmc_variance64x8 = aom_highbd_12_obmc_variance64x4_sse4_1;
#endif
#endif
#if defined(__SSE4_2__)&& HAVE_SIMD

#endif
#if defined(__AVX__)&& HAVE_SIMD

#endif
#if defined(__AVX2__) && HAVE_SIMD
#if CONFIG_MRSSE
  if (flags & HAS_AVX2) aom_highbd_mrsse = aom_highbd_mrsse_avx2;
#endif
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_obmc_sad256x256 = aom_highbd_obmc_sad256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad256x128 = aom_highbd_obmc_sad256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad128x256 = aom_highbd_obmc_sad128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_obmc_sad128x128 = aom_highbd_obmc_sad128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad128x64 = aom_highbd_obmc_sad128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad64x128 = aom_highbd_obmc_sad64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad64x64 = aom_highbd_obmc_sad64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad64x32 = aom_highbd_obmc_sad64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad64x16 = aom_highbd_obmc_sad64x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad32x64 = aom_highbd_obmc_sad32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad32x32 = aom_highbd_obmc_sad32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad32x16 = aom_highbd_obmc_sad32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad32x8 = aom_highbd_obmc_sad32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad16x64 = aom_highbd_obmc_sad16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad16x32 = aom_highbd_obmc_sad16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad16x16 = aom_highbd_obmc_sad16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad16x8 = aom_highbd_obmc_sad16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad16x4 = aom_highbd_obmc_sad16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad8x32 = aom_highbd_obmc_sad8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad8x16 = aom_highbd_obmc_sad8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad8x8 = aom_highbd_obmc_sad8x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad8x4 = aom_highbd_obmc_sad8x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad4x16 = aom_highbd_obmc_sad4x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad4x8 = aom_highbd_obmc_sad4x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad4x4 = aom_highbd_obmc_sad4x4_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_obmc_sad4x64 = aom_highbd_obmc_sad4x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad64x4 = aom_highbd_obmc_sad64x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad4x32 = aom_highbd_obmc_sad4x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad32x4 = aom_highbd_obmc_sad32x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad8x64 = aom_highbd_obmc_sad8x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_obmc_sad64x8 = aom_highbd_obmc_sad64x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance256x256 = aom_highbd_8_variance256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance256x128 = aom_highbd_8_variance256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance128x256 = aom_highbd_8_variance128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_8_variance128x128 = aom_highbd_8_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance128x64 = aom_highbd_8_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance64x128 = aom_highbd_8_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance64x64 = aom_highbd_8_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance64x32 = aom_highbd_8_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance32x64 = aom_highbd_8_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance32x32 = aom_highbd_8_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance32x16 = aom_highbd_8_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance16x32 = aom_highbd_8_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance16x16 = aom_highbd_8_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance16x8 = aom_highbd_8_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance8x16 = aom_highbd_8_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance8x8 = aom_highbd_8_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_variance8x4 = aom_highbd_8_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_variance4x8 = aom_highbd_8_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_variance4x4 = aom_highbd_8_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_variance4x16 = aom_highbd_8_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_variance16x4 = aom_highbd_8_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance8x32 = aom_highbd_8_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance32x8 = aom_highbd_8_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance16x64 = aom_highbd_8_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance64x16 = aom_highbd_8_variance64x16_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_10_variance256x256 = aom_highbd_10_variance256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance256x128 = aom_highbd_10_variance256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance128x256 = aom_highbd_10_variance128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_10_variance128x128 = aom_highbd_10_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance128x64 = aom_highbd_10_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance64x128 = aom_highbd_10_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance64x64 = aom_highbd_10_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance64x32 = aom_highbd_10_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance32x64 = aom_highbd_10_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance32x32 = aom_highbd_10_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance32x16 = aom_highbd_10_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance16x32 = aom_highbd_10_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance16x16 = aom_highbd_10_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance16x8 = aom_highbd_10_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance8x16 = aom_highbd_10_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance8x8 = aom_highbd_10_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_variance8x4 = aom_highbd_10_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_variance4x8 = aom_highbd_10_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_variance4x4 = aom_highbd_10_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_variance4x16 = aom_highbd_10_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_variance16x4 = aom_highbd_10_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance8x32 = aom_highbd_10_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance32x8 = aom_highbd_10_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance16x64 = aom_highbd_10_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance64x16 = aom_highbd_10_variance64x16_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_12_variance256x256 = aom_highbd_12_variance256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance256x128 = aom_highbd_12_variance256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance128x256 = aom_highbd_12_variance128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_12_variance128x128 = aom_highbd_12_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance128x64 = aom_highbd_12_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance64x128 = aom_highbd_12_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance64x64 = aom_highbd_12_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance64x32 = aom_highbd_12_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance32x64 = aom_highbd_12_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance32x32 = aom_highbd_12_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance32x16 = aom_highbd_12_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance16x32 = aom_highbd_12_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance16x16 = aom_highbd_12_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance16x8 = aom_highbd_12_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance8x16 = aom_highbd_12_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance8x8 = aom_highbd_12_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_variance8x4 = aom_highbd_12_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_variance4x8 = aom_highbd_12_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_variance4x4 = aom_highbd_12_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_variance4x16 = aom_highbd_12_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_variance16x4 = aom_highbd_12_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance8x32 = aom_highbd_12_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance32x8 = aom_highbd_12_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance16x64 = aom_highbd_12_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance64x16 = aom_highbd_12_variance64x16_avx2;
#if CONFIG_FLEX_PARTITION
  if (flags & HAS_AVX2) aom_highbd_8_variance64x8 = aom_highbd_8_variance64x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance64x8 = aom_highbd_10_variance64x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance64x8 = aom_highbd_12_variance64x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_variance8x64 = aom_highbd_8_variance8x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_variance8x64 = aom_highbd_10_variance8x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_variance8x64 = aom_highbd_12_variance8x64_avx2;
#endif

#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance256x256 = aom_highbd_8_sub_pixel_variance256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance256x128 = aom_highbd_8_sub_pixel_variance256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance128x256 = aom_highbd_8_sub_pixel_variance128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance128x128 = aom_highbd_8_sub_pixel_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance128x64 = aom_highbd_8_sub_pixel_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance64x128 = aom_highbd_8_sub_pixel_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance64x64 = aom_highbd_8_sub_pixel_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance64x32 = aom_highbd_8_sub_pixel_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance32x64 = aom_highbd_8_sub_pixel_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance32x32 = aom_highbd_8_sub_pixel_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance32x16 = aom_highbd_8_sub_pixel_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance16x32 = aom_highbd_8_sub_pixel_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance16x16 = aom_highbd_8_sub_pixel_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance16x8 = aom_highbd_8_sub_pixel_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance8x16 = aom_highbd_8_sub_pixel_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance8x8 = aom_highbd_8_sub_pixel_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance8x4 = aom_highbd_8_sub_pixel_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance4x8 = aom_highbd_8_sub_pixel_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance4x4 = aom_highbd_8_sub_pixel_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance4x16 = aom_highbd_8_sub_pixel_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance16x4 = aom_highbd_8_sub_pixel_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance8x32 = aom_highbd_8_sub_pixel_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance32x8 = aom_highbd_8_sub_pixel_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance16x64 = aom_highbd_8_sub_pixel_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_variance64x16 = aom_highbd_8_sub_pixel_variance64x16_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance256x256 = aom_highbd_10_sub_pixel_variance256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance256x128 = aom_highbd_10_sub_pixel_variance256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance128x256 = aom_highbd_10_sub_pixel_variance128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance128x128 = aom_highbd_10_sub_pixel_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance128x64 = aom_highbd_10_sub_pixel_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance64x128 = aom_highbd_10_sub_pixel_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance64x64 = aom_highbd_10_sub_pixel_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance64x32 = aom_highbd_10_sub_pixel_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance32x64 = aom_highbd_10_sub_pixel_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance32x32 = aom_highbd_10_sub_pixel_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance32x16 = aom_highbd_10_sub_pixel_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance16x32 = aom_highbd_10_sub_pixel_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance16x16 = aom_highbd_10_sub_pixel_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance16x8 = aom_highbd_10_sub_pixel_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance8x16 = aom_highbd_10_sub_pixel_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance8x8 = aom_highbd_10_sub_pixel_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance8x4 = aom_highbd_10_sub_pixel_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance4x8 = aom_highbd_10_sub_pixel_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance4x4 = aom_highbd_10_sub_pixel_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance4x16 = aom_highbd_10_sub_pixel_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance16x4 = aom_highbd_10_sub_pixel_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance8x32 = aom_highbd_10_sub_pixel_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance32x8 = aom_highbd_10_sub_pixel_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance16x64 = aom_highbd_10_sub_pixel_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_variance64x16 = aom_highbd_10_sub_pixel_variance64x16_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance256x256 = aom_highbd_12_sub_pixel_variance256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance256x128 = aom_highbd_12_sub_pixel_variance256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance128x256 = aom_highbd_12_sub_pixel_variance128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance128x128 = aom_highbd_12_sub_pixel_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance128x64 = aom_highbd_12_sub_pixel_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance64x128 = aom_highbd_12_sub_pixel_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance64x64 = aom_highbd_12_sub_pixel_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance64x32 = aom_highbd_12_sub_pixel_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance32x64 = aom_highbd_12_sub_pixel_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance32x32 = aom_highbd_12_sub_pixel_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance32x16 = aom_highbd_12_sub_pixel_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance16x32 = aom_highbd_12_sub_pixel_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance16x16 = aom_highbd_12_sub_pixel_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance16x8 = aom_highbd_12_sub_pixel_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance8x16 = aom_highbd_12_sub_pixel_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance8x8 = aom_highbd_12_sub_pixel_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance8x4 = aom_highbd_12_sub_pixel_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance4x8 = aom_highbd_12_sub_pixel_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance4x4 = aom_highbd_12_sub_pixel_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance4x16 = aom_highbd_12_sub_pixel_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance16x4 = aom_highbd_12_sub_pixel_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance8x32 = aom_highbd_12_sub_pixel_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance32x8 = aom_highbd_12_sub_pixel_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance16x64 = aom_highbd_12_sub_pixel_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_variance64x16 = aom_highbd_12_sub_pixel_variance64x16_avx2;

  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance128x128 = aom_highbd_8_sub_pixel_avg_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance128x64 = aom_highbd_8_sub_pixel_avg_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance64x128 = aom_highbd_8_sub_pixel_avg_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance64x64 = aom_highbd_8_sub_pixel_avg_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance64x32 = aom_highbd_8_sub_pixel_avg_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance32x64 = aom_highbd_8_sub_pixel_avg_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance32x32 = aom_highbd_8_sub_pixel_avg_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance32x16 = aom_highbd_8_sub_pixel_avg_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance16x32 = aom_highbd_8_sub_pixel_avg_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance16x16 = aom_highbd_8_sub_pixel_avg_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance16x8 = aom_highbd_8_sub_pixel_avg_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance8x16 = aom_highbd_8_sub_pixel_avg_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance8x8 = aom_highbd_8_sub_pixel_avg_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance8x4 = aom_highbd_8_sub_pixel_avg_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance4x8 = aom_highbd_8_sub_pixel_avg_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance4x4 = aom_highbd_8_sub_pixel_avg_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance4x16 = aom_highbd_8_sub_pixel_avg_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance16x4 = aom_highbd_8_sub_pixel_avg_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance8x32 = aom_highbd_8_sub_pixel_avg_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance32x8 = aom_highbd_8_sub_pixel_avg_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance16x64 = aom_highbd_8_sub_pixel_avg_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_8_sub_pixel_avg_variance64x16 = aom_highbd_8_sub_pixel_avg_variance64x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance128x128 = aom_highbd_10_sub_pixel_avg_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance128x64 = aom_highbd_10_sub_pixel_avg_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance64x128 = aom_highbd_10_sub_pixel_avg_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance64x64 = aom_highbd_10_sub_pixel_avg_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance64x32 = aom_highbd_10_sub_pixel_avg_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance32x64 = aom_highbd_10_sub_pixel_avg_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance32x32 = aom_highbd_10_sub_pixel_avg_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance32x16 = aom_highbd_10_sub_pixel_avg_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance16x32 = aom_highbd_10_sub_pixel_avg_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance16x16 = aom_highbd_10_sub_pixel_avg_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance16x8 = aom_highbd_10_sub_pixel_avg_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance8x16 = aom_highbd_10_sub_pixel_avg_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance8x8 = aom_highbd_10_sub_pixel_avg_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance8x4 = aom_highbd_10_sub_pixel_avg_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance4x8 = aom_highbd_10_sub_pixel_avg_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance4x4 = aom_highbd_10_sub_pixel_avg_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance4x16 = aom_highbd_10_sub_pixel_avg_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance16x4 = aom_highbd_10_sub_pixel_avg_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance8x32 = aom_highbd_10_sub_pixel_avg_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance32x8 = aom_highbd_10_sub_pixel_avg_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance16x64 = aom_highbd_10_sub_pixel_avg_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_10_sub_pixel_avg_variance64x16 = aom_highbd_10_sub_pixel_avg_variance64x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance128x128 = aom_highbd_12_sub_pixel_avg_variance128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance128x64 = aom_highbd_12_sub_pixel_avg_variance128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance64x128 = aom_highbd_12_sub_pixel_avg_variance64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance64x64 = aom_highbd_12_sub_pixel_avg_variance64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance64x32 = aom_highbd_12_sub_pixel_avg_variance64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance32x64 = aom_highbd_12_sub_pixel_avg_variance32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance32x32 = aom_highbd_12_sub_pixel_avg_variance32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance32x16 = aom_highbd_12_sub_pixel_avg_variance32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance16x32 = aom_highbd_12_sub_pixel_avg_variance16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance16x16 = aom_highbd_12_sub_pixel_avg_variance16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance16x8 = aom_highbd_12_sub_pixel_avg_variance16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance8x16 = aom_highbd_12_sub_pixel_avg_variance8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance8x8 = aom_highbd_12_sub_pixel_avg_variance8x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance8x4 = aom_highbd_12_sub_pixel_avg_variance8x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance4x8 = aom_highbd_12_sub_pixel_avg_variance4x8_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance4x4 = aom_highbd_12_sub_pixel_avg_variance4x4_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance4x16 = aom_highbd_12_sub_pixel_avg_variance4x16_avx2;
  //if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance16x4 = aom_highbd_12_sub_pixel_avg_variance16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance8x32 = aom_highbd_12_sub_pixel_avg_variance8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance32x8 = aom_highbd_12_sub_pixel_avg_variance32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance16x64 = aom_highbd_12_sub_pixel_avg_variance16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_12_sub_pixel_avg_variance64x16 = aom_highbd_12_sub_pixel_avg_variance64x16_avx2;

  if (flags & HAS_AVX2) aom_convolve_copy = aom_convolve_copy_avx2;
  if (flags & HAS_AVX2) aom_highbd_convolve_copy = aom_highbd_convolve_copy_avx2;
  if (flags & HAS_AVX2) aom_highbd_convolve8_horiz = aom_highbd_convolve8_horiz_avx2;
  if (flags & HAS_AVX2) aom_highbd_convolve8_vert = aom_highbd_convolve8_vert_avx2;
  if (flags & HAS_AVX2) aom_fft8x8_float = aom_fft8x8_float_avx2;
  if (flags & HAS_AVX2) aom_fft16x16_float = aom_fft16x16_float_avx2;
  if (flags & HAS_AVX2) aom_fft32x32_float = aom_fft32x32_float_avx2;
  if (flags & HAS_AVX2) aom_ifft8x8_float = aom_ifft8x8_float_avx2;
  if (flags & HAS_AVX2) aom_ifft16x16_float = aom_ifft16x16_float_avx2;
  if (flags & HAS_AVX2) aom_ifft32x32_float = aom_ifft32x32_float_avx2;
  if (flags & HAS_AVX2) aom_highbd_blend_a64_d16_mask = aom_highbd_blend_a64_d16_mask_avx2;
  if (flags & HAS_AVX2) aom_get_blk_sse_sum = aom_get_blk_sse_sum_avx2;
  if (flags & HAS_AVX2) aom_highbd_sse = aom_highbd_sse_avx2;
  if (flags & HAS_AVX2) aom_sum_squares_2d_i16 = aom_sum_squares_2d_i16_avx2;
  if (flags & HAS_AVX2) aom_var_2d_u8 = aom_var_2d_u8_avx2;
  if (flags & HAS_AVX2) aom_var_2d_u16 = aom_var_2d_u16_avx2;
  if (flags & HAS_AVX2) aom_sum_sse_2d_i16 = aom_sum_sse_2d_i16_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_sad256x256 = aom_highbd_sad256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad256x128 = aom_highbd_sad256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad128x256 = aom_highbd_sad128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_sad128x128 = aom_highbd_sad128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad128x64 = aom_highbd_sad128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x128 = aom_highbd_sad64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x64 = aom_highbd_sad64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x32 = aom_highbd_sad64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x64 = aom_highbd_sad32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x32 = aom_highbd_sad32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x16 = aom_highbd_sad32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x32 = aom_highbd_sad16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x16 = aom_highbd_sad16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x8 = aom_highbd_sad16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x4 = aom_highbd_sad16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x8 = aom_highbd_sad32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x64 = aom_highbd_sad16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x16 = aom_highbd_sad64x16_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_sad_skip_256x256 = aom_highbd_sad_skip_256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_256x128 = aom_highbd_sad_skip_256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_128x256 = aom_highbd_sad_skip_128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_sad_skip_128x128 = aom_highbd_sad_skip_128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_128x64 = aom_highbd_sad_skip_128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x128 = aom_highbd_sad_skip_64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x64 = aom_highbd_sad_skip_64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x32 = aom_highbd_sad_skip_64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x64 = aom_highbd_sad_skip_32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x32 = aom_highbd_sad_skip_32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x16 = aom_highbd_sad_skip_32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x32 = aom_highbd_sad_skip_16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x16 = aom_highbd_sad_skip_16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x8 = aom_highbd_sad_skip_16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x8 = aom_highbd_sad_skip_32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x64 = aom_highbd_sad_skip_16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x16 = aom_highbd_sad_skip_64x16_avx2;
#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_sad256x256_avg = aom_highbd_sad256x256_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad256x128_avg = aom_highbd_sad256x128_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad128x256_avg = aom_highbd_sad128x256_avg_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_sad128x128_avg = aom_highbd_sad128x128_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad128x64_avg = aom_highbd_sad128x64_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x128_avg = aom_highbd_sad64x128_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x64_avg = aom_highbd_sad64x64_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x32_avg = aom_highbd_sad64x32_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x64_avg = aom_highbd_sad32x64_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x32_avg = aom_highbd_sad32x32_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x16_avg = aom_highbd_sad32x16_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x32_avg = aom_highbd_sad16x32_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x16_avg = aom_highbd_sad16x16_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x8_avg = aom_highbd_sad16x8_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x4_avg = aom_highbd_sad16x4_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x8_avg = aom_highbd_sad32x8_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x64_avg = aom_highbd_sad16x64_avg_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x16_avg = aom_highbd_sad64x16_avg_avx2;

#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_sad256x256x4d = aom_highbd_sad256x256x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad256x128x4d = aom_highbd_sad256x128x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad128x256x4d = aom_highbd_sad128x256x4d_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_sad128x128x4d = aom_highbd_sad128x128x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad128x64x4d = aom_highbd_sad128x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x128x4d = aom_highbd_sad64x128x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x64x4d = aom_highbd_sad64x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x32x4d = aom_highbd_sad64x32x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x64x4d = aom_highbd_sad32x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x32x4d = aom_highbd_sad32x32x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x16x4d = aom_highbd_sad32x16x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x32x4d = aom_highbd_sad16x32x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x16x4d = aom_highbd_sad16x16x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x8x4d = aom_highbd_sad16x8x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x4x4d = aom_highbd_sad16x4x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad32x8x4d = aom_highbd_sad32x8x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad16x64x4d = aom_highbd_sad16x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad64x16x4d = aom_highbd_sad64x16x4d_avx2;

#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_sad_skip_256x256x4d = aom_highbd_sad_skip_256x256x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_256x128x4d = aom_highbd_sad_skip_256x128x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_128x256x4d = aom_highbd_sad_skip_128x256x4d_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_sad_skip_128x128x4d = aom_highbd_sad_skip_128x128x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_128x64x4d = aom_highbd_sad_skip_128x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x128x4d = aom_highbd_sad_skip_64x128x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x64x4d = aom_highbd_sad_skip_64x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x32x4d = aom_highbd_sad_skip_64x32x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x64x4d = aom_highbd_sad_skip_32x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x32x4d = aom_highbd_sad_skip_32x32x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x16x4d = aom_highbd_sad_skip_32x16x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x32x4d = aom_highbd_sad_skip_16x32x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x16x4d = aom_highbd_sad_skip_16x16x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x8x4d = aom_highbd_sad_skip_16x8x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_32x8x4d = aom_highbd_sad_skip_32x8x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_16x64x4d = aom_highbd_sad_skip_16x64x4d_avx2;
  if (flags & HAS_AVX2) aom_highbd_sad_skip_64x16x4d = aom_highbd_sad_skip_64x16x4d_avx2;

  if (flags & HAS_AVX2) aom_highbd_hadamard_8x8 = aom_highbd_hadamard_8x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_hadamard_16x16 = aom_highbd_hadamard_16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_hadamard_32x32 = aom_highbd_hadamard_32x32_avx2;
  if (flags & HAS_AVX2) aom_satd = aom_satd_avx2;
  if (flags & HAS_AVX2) aom_satd_lp = aom_satd_lp_avx2;
  if (flags & HAS_AVX2) aom_mse_wxh_16bit_highbd = aom_mse_wxh_16bit_highbd_avx2;

#if CONFIG_BLOCK_256
  if (flags & HAS_AVX2) aom_highbd_masked_sad256x256 = aom_highbd_masked_sad256x256_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad256x128 = aom_highbd_masked_sad256x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad128x256 = aom_highbd_masked_sad128x256_avx2;
#endif
  if (flags & HAS_AVX2) aom_highbd_masked_sad128x128 = aom_highbd_masked_sad128x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad128x64 = aom_highbd_masked_sad128x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad64x128 = aom_highbd_masked_sad64x128_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad64x64 = aom_highbd_masked_sad64x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad64x32 = aom_highbd_masked_sad64x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad32x64 = aom_highbd_masked_sad32x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad32x32 = aom_highbd_masked_sad32x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad32x16 = aom_highbd_masked_sad32x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad16x32 = aom_highbd_masked_sad16x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad16x16 = aom_highbd_masked_sad16x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad16x8 = aom_highbd_masked_sad16x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad8x16 = aom_highbd_masked_sad8x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad8x8 = aom_highbd_masked_sad8x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad8x4 = aom_highbd_masked_sad8x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad4x8 = aom_highbd_masked_sad4x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad4x4 = aom_highbd_masked_sad4x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad4x16 = aom_highbd_masked_sad4x16_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad16x4 = aom_highbd_masked_sad16x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad8x32 = aom_highbd_masked_sad8x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad32x8 = aom_highbd_masked_sad32x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad8x64 = aom_highbd_masked_sad8x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad64x8 = aom_highbd_masked_sad64x8_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad8x64 = aom_highbd_masked_sad4x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad64x8 = aom_highbd_masked_sad64x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad4x32 = aom_highbd_masked_sad4x32_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad32x4 = aom_highbd_masked_sad32x4_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad16x64 = aom_highbd_masked_sad16x64_avx2;
  if (flags & HAS_AVX2) aom_highbd_masked_sad64x16 = aom_highbd_masked_sad64x16_avx2;

  if (flags & HAS_AVX2) aom_highbd_comp_mask_pred = aom_highbd_comp_mask_pred_avx2;
  //aom_highbd_12_variance32x4_avx2
  //aom_highbd_12_variance64x4_avx2
  //aom_highbd_10_variance32x4_avx2
  //aom_highbd_10_variance64x4_avx2
  //aom_highbd_8_variance32x4_avx2
  //aom_highbd_8_variance64x4_avx2
#endif

}
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
