/*
 *  Copyright (c) 2024 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file is generated. Do not edit.
#ifndef VP9_RTCD_H_
#define VP9_RTCD_H_

#ifdef RTCD_C
#define RTCD_EXTERN
#else
#define RTCD_EXTERN extern
#endif

/*
 * VP9
 */

#include "vpx/vpx_integer.h"
#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_enums.h"
#include "vp9/common/vp9_filter.h"
#if !CONFIG_REALTIME_ONLY && CONFIG_VP9_ENCODER
#include "vp9/encoder/vp9_temporal_filter.h"
#endif

struct macroblockd;

/* Encoder forward decls */
struct macroblock;
struct macroblock_plane;
struct vp9_sad_table;
struct ScanOrder;
struct search_site_config;
struct mv;
union int_mv;
struct yv12_buffer_config;

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_VP9_POSTPROC
void vp9_filter_by_weight16x16_c(const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int src_weight);
#if HAVE_SIMD
void vp9_filter_by_weight16x16_sse2(const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int src_weight);
#define vp9_filter_by_weight16x16 vp9_filter_by_weight16x16_sse2
#else
#define vp9_filter_by_weight16x16 vp9_filter_by_weight16x16_c
#endif

void vp9_filter_by_weight8x8_c(const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int src_weight);
#if HAVE_SIMD
void vp9_filter_by_weight8x8_sse2(const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int src_weight);
#define vp9_filter_by_weight8x8 vp9_filter_by_weight8x8_sse2
#else
#define vp9_filter_by_weight8x8 vp9_filter_by_weight8x8_c
#endif
#endif

void vp9_iht16x16_256_add_c(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
#if HAVE_SIMD && CONFIG_EMULATE_HARDWARE && !CONFIG_VP9_HIGHBITDEPTH
void vp9_iht16x16_256_add_sse2(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
#define vp9_iht16x16_256_add vp9_iht16x16_256_add_sse2
#else
#define vp9_iht16x16_256_add vp9_iht16x16_256_add_c
#endif

void vp9_iht4x4_16_add_c(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
#if HAVE_SIMD && CONFIG_EMULATE_HARDWARE && !CONFIG_VP9_HIGHBITDEPTH
void vp9_iht4x4_16_add_sse2(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
#define vp9_iht4x4_16_add vp9_iht4x4_16_add_sse2
#else
#define vp9_iht4x4_16_add vp9_iht4x4_16_add_c
#endif

void vp9_iht8x8_64_add_c(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
#if HAVE_SIMD && CONFIG_EMULATE_HARDWARE && !CONFIG_VP9_HIGHBITDEPTH
void vp9_iht8x8_64_add_sse2(const tran_low_t *input, uint8_t *dest, int stride, int tx_type);
#define vp9_iht8x8_64_add vp9_iht8x8_64_add_sse2
#else
#define vp9_iht8x8_64_add vp9_iht8x8_64_add_c
#endif

#if CONFIG_VP9_HIGHBITDEPTH
#if CONFIG_VP9_POSTPROC
void vp9_highbd_mbpost_proc_down_c(uint16_t *dst, int pitch, int rows, int cols, int flimit);
#define vp9_highbd_mbpost_proc_down vp9_highbd_mbpost_proc_down_c

void vp9_highbd_mbpost_proc_across_ip_c(uint16_t *src, int pitch, int rows, int cols, int flimit);
#define vp9_highbd_mbpost_proc_across_ip vp9_highbd_mbpost_proc_across_ip_c

void vp9_highbd_post_proc_down_and_across_c(const uint16_t *src_ptr, uint16_t *dst_ptr, int src_pixels_per_line, int dst_pixels_per_line, int rows, int cols, int flimit);
#define vp9_highbd_post_proc_down_and_across vp9_highbd_post_proc_down_and_across_c
#endif

void vp9_highbd_iht4x4_16_add_c(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
#if HAVE_SIMD && CONFIG_EMULATE_HARDWARE
#if defined(__SSE4_1__)
void vp9_highbd_iht4x4_16_add_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
RTCD_EXTERN void (*vp9_highbd_iht4x4_16_add)(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
#endif
#else
#define vp9_highbd_iht4x4_16_add vp9_highbd_iht4x4_16_add_c
#endif

void vp9_highbd_iht8x8_64_add_c(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
#if HAVE_SIMD && CONFIG_EMULATE_HARDWARE
#if defined(__SSE4_1__)
void vp9_highbd_iht8x8_64_add_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
RTCD_EXTERN void (*vp9_highbd_iht8x8_64_add)(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
#endif
#else
#define vp9_highbd_iht8x8_64_add vp9_highbd_iht8x8_64_add_c
#endif

void vp9_highbd_iht16x16_256_add_c(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
#if HAVE_SIMD && CONFIG_EMULATE_HARDWARE
#if defined(__SSE4_1__)
void vp9_highbd_iht16x16_256_add_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
RTCD_EXTERN void (*vp9_highbd_iht16x16_256_add)(const tran_low_t *input, uint16_t *dest, int stride, int tx_type, int bd);
#endif
#else
#define vp9_highbd_iht16x16_256_add vp9_highbd_iht16x16_256_add_c
#endif
#endif

#if CONFIG_VP9_TEMPORAL_DENOISING
int vp9_denoiser_filter_c(const uint8_t *sig, int sig_stride, const uint8_t *mc_avg, int mc_avg_stride, uint8_t *avg, int avg_stride, int increase_denoising, BLOCK_SIZE bs, int motion_magnitude);
#if HAVE_SIMD
int vp9_denoiser_filter_sse2(const uint8_t *sig, int sig_stride, const uint8_t *mc_avg, int mc_avg_stride, uint8_t *avg, int avg_stride, int increase_denoising, BLOCK_SIZE bs, int motion_magnitude);
#define vp9_denoiser_filter vp9_denoiser_filter_sse2
#else
#define vp9_denoiser_filter vp9_denoiser_filter_c
#endif
#endif

int64_t vp9_block_error_c(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);
#if HAVE_SIMD
#if defined(__SSE2__)
int64_t vp9_block_error_sse2(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);
RTCD_EXTERN int64_t (*vp9_block_error)(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);
#endif
#if defined(__AVX2__)
int64_t vp9_block_error_avx2(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz);
#endif
#else
#define vp9_block_error vp9_block_error_c
#endif

int64_t vp9_block_error_fp_c(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);
#if HAVE_SIMD
#if defined(__SSE2__)
int64_t vp9_block_error_fp_sse2(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);
RTCD_EXTERN int64_t (*vp9_block_error_fp)(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);
#endif
#if defined(__AVX2__)
int64_t vp9_block_error_fp_avx2(const tran_low_t *coeff, const tran_low_t *dqcoeff, int block_size);
#endif
#else
#define vp9_block_error_fp vp9_block_error_fp_c
#endif

#if CONFIG_VP9_HIGHBITDEPTH
int64_t vp9_highbd_block_error_c(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz, int bd);
#if HAVE_SIMD
int64_t vp9_highbd_block_error_sse2(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz, int bd);
#define vp9_highbd_block_error vp9_highbd_block_error_sse2
#else
#define vp9_highbd_block_error vp9_highbd_block_error_c
#endif
#endif

void vp9_quantize_fp_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#if HAVE_SIMD
#if defined(__SSE2__)
int64_t vp9_quantize_fp_sse2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
RTCD_EXTERN int64_t (*vp9_quantize_fp)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#if defined(__SSSE3__)
int64_t vp9_quantize_fp_ssse3(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#if defined(__AVX2__)
int64_t vp9_quantize_fp_avx2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#else
#define vp9_quantize_fp vp9_quantize_fp_c
#endif

void vp9_quantize_fp_32x32_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#if HAVE_SIMD
#if defined(__SSE2__)
int64_t vp9_quantize_fp_32x32_sse2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
RTCD_EXTERN int64_t (*vp9_quantize_fp_32x32)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#if defined(__SSSE3__)
int64_t vp9_quantize_fp_32x32_ssse3(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#if defined(__AVX2__)
int64_t vp9_quantize_fp_32x32_avx2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#else
#define vp9_quantize_fp_32x32 vp9_quantize_fp_32x32_c
#endif

void vp9_fht16x16_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#if HAVE_SIMD
void vp9_fht16x16_sse2(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#define vp9_fht16x16 vp9_fht16x16_sse2
#else
#define vp9_fht16x16 vp9_fht16x16_c
#endif

void vp9_fht4x4_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#if HAVE_SIMD
void vp9_fht4x4_sse2(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#define vp9_fht4x4 vp9_fht4x4_sse2
#else
#define vp9_fht4x4 vp9_fht4x4_c
#endif

void vp9_fht8x8_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#if HAVE_SIMD
void vp9_fht8x8_sse2(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#define vp9_fht8x8 vp9_fht8x8_sse2
#else
#define vp9_fht8x8 vp9_fht8x8_c
#endif

void vp9_fwht4x4_c(const int16_t *input, tran_low_t *output, int stride);
#if HAVE_SIMD
void vp9_fwht4x4_sse2(const int16_t *input, tran_low_t *output, int stride);
#define vp9_fwht4x4 vp9_fwht4x4_sse2
#else
#define vp9_fwht4x4 vp9_fwht4x4_c
#endif

int vp9_diamond_search_sad_c(const struct macroblock *x, const struct search_site_config *cfg,  struct mv *ref_mv, uint32_t start_mv_sad, struct mv *best_mv, int search_param, int sad_per_bit, int *num00, const struct vp9_sad_table *sad_fn_ptr, const struct mv *center_mv);
/*#if HAVE_SIMD
#if defined(__AVX__)
int vp9_diamond_search_sad_avx(const struct macroblock *x, const struct search_site_config *cfg,  struct mv *ref_mv, uint32_t start_mv_sad, struct mv *best_mv, int search_param, int sad_per_bit, int *num00, const struct vp9_sad_table *sad_fn_ptr, const struct mv *center_mv);
RTCD_EXTERN int (*vp9_diamond_search_sad)(const struct macroblock *x, const struct search_site_config *cfg,  struct mv *ref_mv, uint32_t start_mv_sad, struct mv *best_mv, int search_param, int sad_per_bit, int *num00, const struct vp9_sad_table *sad_fn_ptr, const struct mv *center_mv);
#endif
#else*/
#define vp9_diamond_search_sad vp9_diamond_search_sad_c
//#endif

void vp9_apply_temporal_filter_c(const uint8_t *y_src, int y_src_stride, const uint8_t *y_pre, int y_pre_stride, const uint8_t *u_src, const uint8_t *v_src, int uv_src_stride, const uint8_t *u_pre, const uint8_t *v_pre, int uv_pre_stride, unsigned int block_width, unsigned int block_height, int ss_x, int ss_y, int strength, const int *const blk_fw, int use_32x32, uint32_t *y_accumulator, uint16_t *y_count, uint32_t *u_accumulator, uint16_t *u_count, uint32_t *v_accumulator, uint16_t *v_count);
#if HAVE_SIMD
#if defined(__SSE4_1__)
void vp9_apply_temporal_filter_sse4_1(const uint8_t *y_src, int y_src_stride, const uint8_t *y_pre, int y_pre_stride, const uint8_t *u_src, const uint8_t *v_src, int uv_src_stride, const uint8_t *u_pre, const uint8_t *v_pre, int uv_pre_stride, unsigned int block_width, unsigned int block_height, int ss_x, int ss_y, int strength, const int *const blk_fw, int use_32x32, uint32_t *y_accumulator, uint16_t *y_count, uint32_t *u_accumulator, uint16_t *u_count, uint32_t *v_accumulator, uint16_t *v_count);
RTCD_EXTERN void (*vp9_apply_temporal_filter)(const uint8_t *y_src, int y_src_stride, const uint8_t *y_pre, int y_pre_stride, const uint8_t *u_src, const uint8_t *v_src, int uv_src_stride, const uint8_t *u_pre, const uint8_t *v_pre, int uv_pre_stride, unsigned int block_width, unsigned int block_height, int ss_x, int ss_y, int strength, const int *const blk_fw, int use_32x32, uint32_t *y_accumulator, uint16_t *y_count, uint32_t *u_accumulator, uint16_t *u_count, uint32_t *v_accumulator, uint16_t *v_count);
#endif
#else
#define vp9_apply_temporal_filter vp9_apply_temporal_filter_c
#endif

#if CONFIG_VP9_HIGHBITDEPTH
void vp9_highbd_apply_temporal_filter_c(const uint16_t *y_src, int y_src_stride, const uint16_t *y_pre, int y_pre_stride, const uint16_t *u_src, const uint16_t *v_src, int uv_src_stride, const uint16_t *u_pre, const uint16_t *v_pre, int uv_pre_stride, unsigned int block_width, unsigned int block_height, int ss_x, int ss_y, int strength, const int *const blk_fw, int use_32x32, uint32_t *y_accum, uint16_t *y_count, uint32_t *u_accum, uint16_t *u_count, uint32_t *v_accum, uint16_t *v_count);
#if HAVE_SIMD
#if defined(__SSE4_1__)
void vp9_highbd_apply_temporal_filter_sse4_1(const uint16_t *y_src, int y_src_stride, const uint16_t *y_pre, int y_pre_stride, const uint16_t *u_src, const uint16_t *v_src, int uv_src_stride, const uint16_t *u_pre, const uint16_t *v_pre, int uv_pre_stride, unsigned int block_width, unsigned int block_height, int ss_x, int ss_y, int strength, const int *const blk_fw, int use_32x32, uint32_t *y_accum, uint16_t *y_count, uint32_t *u_accum, uint16_t *u_count, uint32_t *v_accum, uint16_t *v_count);
RTCD_EXTERN void (*vp9_highbd_apply_temporal_filter)(const uint16_t *y_src, int y_src_stride, const uint16_t *y_pre, int y_pre_stride, const uint16_t *u_src, const uint16_t *v_src, int uv_src_stride, const uint16_t *u_pre, const uint16_t *v_pre, int uv_pre_stride, unsigned int block_width, unsigned int block_height, int ss_x, int ss_y, int strength, const int *const blk_fw, int use_32x32, uint32_t *y_accum, uint16_t *y_count, uint32_t *u_accum, uint16_t *u_count, uint32_t *v_accum, uint16_t *v_count);
#endif
#else
#define vp9_highbd_apply_temporal_filter vp9_highbd_apply_temporal_filter_c
#endif

#if !CONFIG_REALTIME_ONLY
void vpx_convolve12_vert_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#if HAVE_SIMD
RTCD_EXTERN void (*vpx_convolve12_vert)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#if defined(__SSSE3__)
void vpx_convolve12_vert_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#endif
#if defined(__AVX2__)
void vpx_convolve12_vert_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#endif
#else
#define vpx_convolve12_vert vpx_convolve12_vert_c
#endif

void vpx_convolve12_horiz_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#if HAVE_SIMD
RTCD_EXTERN void (*vpx_convolve12_horiz)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#if defined(__SSSE3__)
void vpx_convolve12_horiz_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#endif
#if defined(__AVX2__)
void vpx_convolve12_horiz_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#endif
#else
#define vpx_convolve12_horiz vpx_convolve12_horiz_c
#endif

void vpx_convolve12_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#if HAVE_SIMD
RTCD_EXTERN void (*vpx_convolve12)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#if defined(__SSSE3__)
void vpx_convolve12_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#endif
#if defined(__AVX2__)
void vpx_convolve12_avx2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h);
#endif
#else
#define vpx_convolve12 vpx_convolve12_c
#endif

#if CONFIG_VP9_HIGHBITDEPTH
void vpx_highbd_convolve12_vert_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#if HAVE_SIMD
RTCD_EXTERN void (*vpx_highbd_convolve12_vert)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#if defined(__SSSE3__)
void vpx_highbd_convolve12_vert_ssse3(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#endif
#if defined(__AVX2__)
void vpx_highbd_convolve12_vert_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#endif
#else
#define vpx_highbd_convolve12_vert vpx_highbd_convolve12_vert_c
#endif

void vpx_highbd_convolve12_horiz_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#if HAVE_SIMD
RTCD_EXTERN void (*vpx_highbd_convolve12_horiz)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#if defined(__SSSE3__)
void vpx_highbd_convolve12_horiz_ssse3(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#endif
#if defined(__AVX2__)
void vpx_highbd_convolve12_horiz_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#endif
#else
#define vpx_highbd_convolve12_horiz vpx_highbd_convolve12_horiz_c
#endif

void vpx_highbd_convolve12_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#if HAVE_SIMD
RTCD_EXTERN void (*vpx_highbd_convolve12)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#if defined(__SSSE3__)
void vpx_highbd_convolve12_ssse3(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#endif
#if defined(__AVX2__)
void vpx_highbd_convolve12_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const InterpKernel12 *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h, int bd);
#endif
#else
#define vpx_highbd_convolve12 vpx_highbd_convolve12_c
#endif
#endif
#endif

void vp9_highbd_quantize_fp_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#if HAVE_SIMD
RTCD_EXTERN void (*vp9_highbd_quantize_fp)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#if defined(__AVX2__)
void vp9_highbd_quantize_fp_avx2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#else
#define vp9_highbd_quantize_fp vp9_highbd_quantize_fp_c
#endif

void vp9_highbd_quantize_fp_32x32_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#if HAVE_SIMD
RTCD_EXTERN void (*vp9_highbd_quantize_fp_32x32)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#if defined(__AVX2__)
void vp9_highbd_quantize_fp_32x32_avx2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const struct macroblock_plane * const mb_plane, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const struct ScanOrder * const scan_order);
#endif
#else
#define vp9_highbd_quantize_fp_32x32 vp9_highbd_quantize_fp_32x32_c
#endif

void vp9_highbd_fht4x4_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#define vp9_highbd_fht4x4 vp9_highbd_fht4x4_c

void vp9_highbd_fht8x8_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#define vp9_highbd_fht8x8 vp9_highbd_fht8x8_c

void vp9_highbd_fht16x16_c(const int16_t *input, tran_low_t *output, int stride, int tx_type);
#define vp9_highbd_fht16x16 vp9_highbd_fht16x16_c

void vp9_highbd_fwht4x4_c(const int16_t *input, tran_low_t *output, int stride);
#define vp9_highbd_fwht4x4 vp9_highbd_fwht4x4_c

void vp9_highbd_temporal_filter_apply_c(const uint8_t *frame1, unsigned int stride, const uint8_t *frame2, unsigned int block_width, unsigned int block_height, int strength, int *blk_fw, int use_32x32, uint32_t *accumulator, uint16_t *count);
#define vp9_highbd_temporal_filter_apply vp9_highbd_temporal_filter_apply_c
#endif

void vp9_scale_and_extend_frame_c(const struct yv12_buffer_config *src, struct yv12_buffer_config *dst, INTERP_FILTER filter_type, int phase_scaler);
#if HAVE_SIMD
#if defined(__SSSE3__)
void vp9_scale_and_extend_frame_ssse3(const struct yv12_buffer_config *src, struct yv12_buffer_config *dst, INTERP_FILTER filter_type, int phase_scaler);
RTCD_EXTERN void (*vp9_scale_and_extend_frame)(const struct yv12_buffer_config *src, struct yv12_buffer_config *dst, INTERP_FILTER filter_type, int phase_scaler);
#endif
#else
#define vp9_scale_and_extend_frame vp9_scale_and_extend_frame_c
#endif

void vp9_rtcd(void);

#include "vpx_config.h"

#ifdef RTCD_C
static void setup_rtcd_internal(void)
{
#if HAVE_SIMD
  int flags = x86_simd_caps();

  (void)flags;

  //vp9_diamond_search_sad = vp9_diamond_search_sad_c;
  vp9_scale_and_extend_frame = vp9_scale_and_extend_frame_c;
  vpx_convolve12_vert = vpx_convolve12_vert_c;
  vpx_convolve12_horiz = vpx_convolve12_horiz_c;
  vpx_convolve12 = vpx_convolve12_c;
#if defined(__SSE2__) && HAVE_SIMD
  vp9_block_error = vp9_block_error_sse2;
  vp9_block_error_fp = vp9_block_error_fp_sse2;
  vp9_quantize_fp = vp9_quantize_fp_sse2;
  vp9_quantize_fp_32x32 = vp9_quantize_fp_32x32_sse2;
#endif
#if defined(__SSSE3__) && HAVE_SIMD
  if (flags & HAS_SSSE3) vp9_scale_and_extend_frame = vp9_scale_and_extend_frame_ssse3;
  if (flags & HAS_SSSE3) vp9_quantize_fp = vp9_quantize_fp_ssse3;
  if (flags & HAS_SSSE3) vp9_quantize_fp_32x32 = vp9_quantize_fp_32x32_ssse3;
  if (flags & HAS_SSSE3) vpx_convolve12_vert = vpx_convolve12_vert_ssse3;
  if (flags & HAS_SSSE3) vpx_convolve12_horiz = vpx_convolve12_horiz_ssse3;
  if (flags & HAS_SSSE3) vpx_convolve12 = vpx_convolve12_ssse3;
#endif
#if defined(__SSE4_1__) && HAVE_SIMD
  if (flags & HAS_SSE4_1) vp9_apply_temporal_filter = vp9_apply_temporal_filter_sse4_1;
#endif
#if defined(__AVX__) && HAVE_SIMD
  //if (flags & HAS_AVX) vp9_diamond_search_sad = vp9_diamond_search_sad_avx;
#endif
#if defined(__AVX2__) && HAVE_SIMD
  if (flags & HAS_AVX2) vp9_block_error = vp9_block_error_avx2;
  if (flags & HAS_AVX2) vp9_block_error_fp = vp9_block_error_fp_avx2;
  if (flags & HAS_AVX2) vp9_quantize_fp = vp9_quantize_fp_avx2;
  if (flags & HAS_AVX2) vp9_quantize_fp_32x32 = vp9_quantize_fp_32x32_avx2;
  if (flags & HAS_AVX2) vpx_convolve12_vert = vpx_convolve12_vert_avx2;
  if (flags & HAS_AVX2) vpx_convolve12_horiz = vpx_convolve12_horiz_avx2;
  if (flags & HAS_AVX2) vpx_convolve12 = vpx_convolve12_avx2;
#endif

#if CONFIG_VP9_HIGHBITDEPTH
  vp9_highbd_iht4x4_16_add = vp9_highbd_iht4x4_16_add_c;
  vp9_highbd_iht8x8_64_add = vp9_highbd_iht8x8_64_add_c;
  vp9_highbd_iht16x16_256_add = vp9_highbd_iht16x16_256_add_c;
  vp9_highbd_apply_temporal_filter = vp9_highbd_apply_temporal_filter_c;
  vp9_highbd_quantize_fp = vp9_highbd_quantize_fp_c;
  vp9_highbd_quantize_fp_32x32 = vp9_highbd_quantize_fp_32x32_c;
  vpx_highbd_convolve12_vert = vpx_highbd_convolve12_vert_c;
  vpx_highbd_convolve12_horiz = vpx_highbd_convolve12_horiz_c;
  vpx_highbd_convolve12 = vpx_highbd_convolve12_c;
#if defined(__SSSE3__) && HAVE_SIMD
  if (flags & HAS_SSSE3) vpx_highbd_convolve12_vert = vpx_highbd_convolve12_vert_ssse3;
  if (flags & HAS_SSSE3) vpx_highbd_convolve12_horiz = vpx_highbd_convolve12_horiz_ssse3;
  if (flags & HAS_SSSE3) vpx_highbd_convolve12 = vpx_highbd_convolve12_ssse3;
#endif
#if defined(__SSE4_1__) && HAVE_SIMD
#if CONFIG_EMULATE_HARDWARE
  if (flags & HAS_SSE4_1) vp9_highbd_iht4x4_16_add = vp9_highbd_iht4x4_16_add_sse4_1;
  if (flags & HAS_SSE4_1) vp9_highbd_iht8x8_64_add = vp9_highbd_iht8x8_64_add_sse4_1;
  if (flags & HAS_SSE4_1) vp9_highbd_iht16x16_256_add = vp9_highbd_iht16x16_256_add_sse4_1;
#endif
  if (flags & HAS_SSE4_1) vp9_highbd_apply_temporal_filter = vp9_highbd_apply_temporal_filter_sse4_1;
#endif
#if defined(__AVX2__) && HAVE_SIMD
  if (flags & HAS_AVX2) vp9_highbd_quantize_fp = vp9_highbd_quantize_fp_avx2;
  if (flags & HAS_AVX2) vp9_highbd_quantize_fp_32x32 = vp9_highbd_quantize_fp_32x32_avx2;
  if (flags & HAS_AVX2) vpx_highbd_convolve12_vert = vpx_highbd_convolve12_vert_avx2;
  if (flags & HAS_AVX2) vpx_highbd_convolve12_horiz = vpx_highbd_convolve12_horiz_avx2;
  if (flags & HAS_AVX2) vpx_highbd_convolve12 = vpx_highbd_convolve12_avx2;
#endif
#endif
#endif
}
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
