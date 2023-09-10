// This file is generated. Do not edit.
#ifndef AV2_RTCD_H_
#define AV2_RTCD_H_

#ifdef RTCD_C
#define RTCD_EXTERN
#else
#define RTCD_EXTERN extern
#endif

/*
 * AV1
 */

#include "avm/aom_integer.h"
#include "avm_dsp/txfm_common.h"
#include "av2/common/common.h"
#include "av2/common/enums.h"
#include "av2/common/quant_common.h"
#include "av2/common/filter.h"
#include "av2/common/convolve.h"
#include "av2/common/av1_txfm.h"
#include "av2/common/odintrin.h"
#include "av2/common/restoration.h"

struct macroblockd;

/* Encoder forward decls */
struct macroblock;
struct txfm_param;
struct aom_variance_vtable;
struct search_site_config;
struct yv12_buffer_config;
struct NN_CONFIG;
typedef struct NN_CONFIG NN_CONFIG;

enum { NONE, RELU, SOFTSIGN, SIGMOID } UENUM1BYTE(ACTIVATION);
#if CONFIG_NN_V2
enum { SOFTMAX_CROSS_ENTROPY } UENUM1BYTE(LOSS);
struct NN_CONFIG_V2;
typedef struct NN_CONFIG_V2 NN_CONFIG_V2;
struct FC_LAYER;
typedef struct FC_LAYER FC_LAYER;
#endif  // CONFIG_NN_V2

struct CNN_CONFIG;
typedef struct CNN_CONFIG CNN_CONFIG;
struct CNN_LAYER_CONFIG;
typedef struct CNN_LAYER_CONFIG CNN_LAYER_CONFIG;
struct CNN_THREAD_DATA;
typedef struct CNN_THREAD_DATA CNN_THREAD_DATA;
struct CNN_BRANCH_CONFIG;
typedef struct CNN_BRANCH_CONFIG CNN_BRANCH_CONFIG;
struct CNN_MULTI_OUT;
typedef struct CNN_MULTI_OUT CNN_MULTI_OUT;

/* Function pointers return by CfL functions */
typedef void (*cfl_subtract_average_fn)(const uint16_t* src, int16_t* dst);

typedef void (*cfl_subsample_hbd_fn)(const uint16_t* input,
                                     int input_stride,
                                     uint16_t* output_q3);

typedef void (*cfl_predict_hbd_fn)(const int16_t* src,
                                   uint16_t* dst,
                                   int dst_stride,
                                   int alpha_q3,
                                   int bd);

void av1_highbd_convolve_horiz_rs_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const int16_t *x_filters, int x0_qn, int x_step_qn, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_convolve_horiz_rs)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const int16_t *x_filters, int x0_qn, int x_step_qn, int bd);
void av1_highbd_convolve_horiz_rs_sse4_1(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const int16_t *x_filters, int x0_qn, int x_step_qn, int bd);
#else
#define av1_highbd_convolve_horiz_rs av1_highbd_convolve_horiz_rs_c
#endif

void av1_highbd_wiener_convolve_add_src_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN void (*av1_highbd_wiener_convolve_add_src)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const ConvolveParams *conv_params, int bd);
void av1_highbd_wiener_convolve_add_src_ssse3(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_wiener_convolve_add_src_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_wiener_convolve_add_src av1_highbd_wiener_convolve_add_src_c
#endif

#if CONFIG_PC_WIENER
void av1_fill_tskip_sum_buffer_c(int row, const uint8_t *tskip, int tskip_stride, int8_t *tskip_sum_buffer, int width, int height, int tskip_lead, int tskip_lag, bool use_strict_bounds);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_fill_tskip_sum_buffer)(int row, const uint8_t *tskip, int tskip_stride, int8_t *tskip_sum_buffer, int width, int height, int tskip_lead, int tskip_lag, bool use_strict_bounds);
#if defined(__AVX2__)
void av1_fill_tskip_sum_buffer_avx2(int row, const uint8_t *tskip, int tskip_stride, int8_t *tskip_sum_buffer, int width, int height, int tskip_lead, int tskip_lag, bool use_strict_bounds);
#endif
#else
#define av1_fill_tskip_sum_buffer av1_fill_tskip_sum_buffer_c
#endif
void fill_directional_feature_buffers_highbd_c(int *feature_sum_buffers[], int16_t *feature_line_buffers[], int row, int buffer_row, const uint16_t *dgd, int dgd_stride, int width, int feature_lead, int feature_lag);
#if HAVE_SIMD
RTCD_EXTERN void (*fill_directional_feature_buffers_highbd)(int *feature_sum_buffers[], int16_t *feature_line_buffers[], int row, int buffer_row, const uint16_t *dgd, int dgd_stride, int width, int feature_lead, int feature_lag);
#if defined(__AVX2__)
void fill_directional_feature_buffers_highbd_avx2(int *feature_sum_buffers[], int16_t *feature_line_buffers[], int row, int buffer_row, const uint16_t *dgd, int dgd_stride, int width, int feature_lead, int feature_lag);
#endif
#else
#define fill_directional_feature_buffers_highbd fill_directional_feature_buffers_highbd_c
#endif
void av1_fill_directional_feature_accumulators_c(int dir_feature_accum[NUM_PC_WIENER_FEATURES][PC_WIENER_FEATURE_ACC_SIZE], int *feature_sum_buff[NUM_PC_WIENER_FEATURES], int width, int col_offset, int feature_lead, int feature_lag);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_fill_directional_feature_accumulators)(int dir_feature_accum[NUM_PC_WIENER_FEATURES][PC_WIENER_FEATURE_ACC_SIZE], int *feature_sum_buff[NUM_PC_WIENER_FEATURES], int width, int col_offset, int feature_lead, int feature_lag);
#if defined(__AVX2__)
void av1_fill_directional_feature_accumulators_avx2(int dir_feature_accum[NUM_PC_WIENER_FEATURES][PC_WIENER_FEATURE_ACC_SIZE], int *feature_sum_buff[NUM_PC_WIENER_FEATURES], int width, int col_offset, int feature_lead, int feature_lag);
#endif
#else
#define av1_fill_directional_feature_accumulators av1_fill_directional_feature_accumulators_c
#endif
void av1_fill_tskip_feature_accumulator_c(int16_t tskip_feature_accum[PC_WIENER_FEATURE_ACC_SIZE], int8_t* tskip_sum_buff, int width, int col_offset,int tskip_lead, int tskip_lag);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_fill_tskip_feature_accumulator)(int16_t tskip_feature_accum[PC_WIENER_FEATURE_ACC_SIZE], int8_t* tskip_sum_buff, int width, int col_offset,int tskip_lead, int tskip_lag);
#if defined(__AVX2__)
void av1_fill_tskip_feature_accumulator_avx2(int16_t tskip_feature_accum[PC_WIENER_FEATURE_ACC_SIZE], int8_t* tskip_sum_buff, int width, int col_offset,int tskip_lead, int tskip_lag);
#endif
#else
#define av1_fill_tskip_feature_accumulator av1_fill_tskip_feature_accumulator_c
#endif
#endif

// Non-separable Wiener filter
#if CONFIG_PC_WIENER || CONFIG_WIENER_NONSEP
void av1_convolve_symmetric_highbd_c(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_convolve_symmetric_highbd)(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if defined(__AVX2__)
void av1_convolve_symmetric_highbd_avx2(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#endif
#else
#define av1_convolve_symmetric_highbd av1_convolve_symmetric_highbd_c
#endif
void av1_convolve_symmetric_subtract_center_highbd_c(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_convolve_symmetric_subtract_center_highbd)(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if defined(__AVX2__)
void av1_convolve_symmetric_subtract_center_highbd_avx2(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#endif
#else
#define av1_convolve_symmetric_highbd av1_convolve_symmetric_highbd_c
#endif
#endif

#if CONFIG_WIENER_NONSEP_CROSS_FILT
void av1_convolve_symmetric_dual_highbd_c(const uint16_t *dgd, int dgd_stride, const uint16_t *dgd_dual, int dgd_dual_stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_convolve_symmetric_dual_highbd)(const uint16_t *dgd, int dgd_stride, const uint16_t *dgd_dual, int dgd_dual_stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if defined(__AVX2__)
void av1_convolve_symmetric_dual_highbd_avx2(const uint16_t *dgd, int dgd_stride, const uint16_t *dgd_dual, int dgd_dual_stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#endif
#else
#define av1_convolve_symmetric_dual_highbd av1_convolve_symmetric_dual_highbd_c
#endif
void av1_convolve_symmetric_dual_subtract_center_highbd_c(const uint16_t *dgd, int dgd_stride, const uint16_t *dgd_dual, int dgd_dual_stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_convolve_symmetric_dual_subtract_center_highbd)(const uint16_t *dgd, int dgd_stride, const uint16_t *dgd_dual, int dgd_dual_stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if defined(__AVX2__)
void av1_convolve_symmetric_dual_subtract_center_highbd_avx2(const uint16_t *dgd, int dgd_stride, const uint16_t *dgd_dual, int dgd_dual_stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#endif
#else
#define av1_convolve_symmetric_dual_subtract_center_highbd av1_convolve_symmetric_dual_subtract_center_highbd_c
#endif
#endif


// FILTER_INTRA predictor functions
void av1_filter_intra_predictor_c(uint8_t *dst, ptrdiff_t stride, TX_SIZE tx_size, const uint8_t *above, const uint8_t *left, int mode);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_filter_intra_predictor)(uint8_t *dst, ptrdiff_t stride, TX_SIZE tx_size, const uint8_t *above, const uint8_t *left, int mode);
void av1_filter_intra_predictor_sse4_1(uint8_t *dst, ptrdiff_t stride, TX_SIZE tx_size, const uint8_t *above, const uint8_t *left, int mode);
#else
#define av1_filter_intra_predictor av1_filter_intra_predictor_c
#endif

// optical flow interpolation function
#if CONFIG_OPTFLOW_REFINEMENT
void av1_bicubic_grad_interpolation_highbd_c(const int16_t *pred_src,int16_t *x_grad,int16_t *y_grad,const int blk_width,const int blk_height);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_bicubic_grad_interpolation_highbd)(const int16_t *pred_src,int16_t *x_grad,int16_t *y_grad,const int blk_width,const int blk_height);
void av1_bicubic_grad_interpolation_highbd_sse4_1(const int16_t *pred_src,int16_t *x_grad,int16_t *y_grad,const int blk_width,const int blk_height);
#else
#define av1_bicubic_grad_interpolation_highbd av1_bicubic_grad_interpolation_highbd_c
#endif
int av1_opfl_mv_refinement_nxn_highbd_c(const uint16_t *p0, int pstride0, const uint16_t *p1, int pstride1, const int16_t *gx0, const int16_t *gy0, const int16_t *gx1, const int16_t *gy1, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN int (*av1_opfl_mv_refinement_nxn_highbd)(const uint16_t *p0, int pstride0, const uint16_t *p1, int pstride1, const int16_t *gx0, const int16_t *gy0, const int16_t *gx1, const int16_t *gy1, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
int av1_opfl_mv_refinement_nxn_highbd_sse4_1(const uint16_t *p0, int pstride0, const uint16_t *p1, int pstride1, const int16_t *gx0, const int16_t *gy0, const int16_t *gx1, const int16_t *gy1, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
#else
#define av1_opfl_mv_refinement_nxn_highbd av1_opfl_mv_refinement_nxn_highbd_c
#endif
int av1_opfl_mv_refinement_nxn_interp_grad_c( const int16_t *pdiff, int pstride,const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n,int d0, int d1, int grad_prec_bits,int mv_prec_bits, int *vx0, int *vy0,int *vx1, int *vy1);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN int (*av1_opfl_mv_refinement_nxn_interp_grad)(const uint16_t *p0, int pstride0, const uint16_t *p1, int pstride1, const int16_t *gx0, const int16_t *gy0, const int16_t *gx1, const int16_t *gy1, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
int av1_opfl_mv_refinement_nxn_interp_grad_sse4_1(const uint16_t *p0, int pstride0, const uint16_t *p1, int pstride1, const int16_t *gx0, const int16_t *gy0, const int16_t *gx1, const int16_t *gy1, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
#else
#define av1_opfl_mv_refinement_nxn_interp_grad av1_opfl_mv_refinement_nxn_interp_grad_c
#endif
void av1_copy_pred_array_highbd_c(const uint16_t *src1, const uint16_t *src2, int16_t *dst1,int16_t *dst2, int bw, int bh, int d0, int d1);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_copy_pred_array_highbd)(const uint16_t *src1, const uint16_t *src2, int16_t *dst1,int16_t *dst2, int bw, int bh, int d0, int d1);
void av1_copy_pred_array_highbd_sse4_1(const uint16_t *src1, const uint16_t *src2, int16_t *dst1,int16_t *dst2, int bw, int bh, int d0, int d1);
#else
#define av1_copy_pred_array_highbd av1_copy_pred_array_highbd_c
#endif
#endif

// High bitdepth functions

// inv txfm
void inv_stxfm_c(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*inv_stxfm)(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size);
void inv_stxfm_sse4_1(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size);
#else
#define inv_stxfm inv_stxfm_c
#endif
void av1_highbd_inv_txfm_add_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_inv_txfm_add)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#endif
#if defined(__AVX2__)
void av1_highbd_inv_txfm_add_avx2(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#endif
#else
#define av1_highbd_inv_txfm_add av1_highbd_inv_txfm_add_c
#endif
void av1_highbd_inv_txfm_add_4x4_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_4x4)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_4x4_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_4x4 av1_highbd_inv_txfm_add_4x4_c
#endif
void av1_highbd_inv_txfm_add_8x8_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_8x8)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_8x8_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_8x8 av1_highbd_inv_txfm_add_8x8_c
#endif
void av1_highbd_inv_txfm_add_4x8_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_4x8)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_4x8_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_4x8 av1_highbd_inv_txfm_add_4x8_c
#endif
void av1_highbd_inv_txfm_add_8x4_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_8x4)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_8x4_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_8x4 av1_highbd_inv_txfm_add_8x4_c
#endif
void av1_highbd_inv_txfm_add_4x16_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_4x16)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_4x16_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_4x16 av1_highbd_inv_txfm_add_4x16_c
#endif
void av1_highbd_inv_txfm_add_16x4_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_16x4)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_16x4_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_16x4 av1_highbd_inv_txfm_add_16x4_c
#endif

void av1_highbd_inv_txfm_add_8x16_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_8x16 av1_highbd_inv_txfm_add_8x16_c
void av1_highbd_inv_txfm_add_16x8_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_16x8 av1_highbd_inv_txfm_add_16x8_c
void av1_highbd_inv_txfm_add_16x32_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_16x32 av1_highbd_inv_txfm_add_16x32_c
void av1_highbd_inv_txfm_add_32x16_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_32x16 av1_highbd_inv_txfm_add_32x16_c
void av1_highbd_inv_txfm_add_32x32_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_32x32 av1_highbd_inv_txfm_add_32x32_c
void av1_highbd_inv_txfm_add_32x64_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_32x64 av1_highbd_inv_txfm_add_32x64_c
void av1_highbd_inv_txfm_add_64x32_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_64x32 av1_highbd_inv_txfm_add_64x32_c
void av1_highbd_inv_txfm_add_64x64_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_64x64 av1_highbd_inv_txfm_add_64x64_c
void av1_highbd_inv_txfm_add_8x32_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_8x32 av1_highbd_inv_txfm_add_8x32_c
void av1_highbd_inv_txfm_add_32x8_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_32x8 av1_highbd_inv_txfm_add_32x8_c
void av1_highbd_inv_txfm_add_16x64_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_16x64 av1_highbd_inv_txfm_add_16x64_c
void av1_highbd_inv_txfm_add_64x16_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_64x16 av1_highbd_inv_txfm_add_64x16_c

void av1_highbd_iwht4x4_1_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_1_add av1_highbd_iwht4x4_1_add_c
void av1_highbd_iwht4x4_16_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_16_add av1_highbd_iwht4x4_16_add_c

void av1_inv_txfm2d_add_4x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_4x8 av1_inv_txfm2d_add_4x8_c
void av1_inv_txfm2d_add_8x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_8x4 av1_inv_txfm2d_add_8x4_c
void av1_inv_txfm2d_add_8x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_8x16 av1_inv_txfm2d_add_8x16_c
void av1_inv_txfm2d_add_16x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_16x8 av1_inv_txfm2d_add_16x8_c
void av1_inv_txfm2d_add_16x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_16x32 av1_inv_txfm2d_add_16x32_c
void av1_inv_txfm2d_add_32x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_32x16 av1_inv_txfm2d_add_32x16_c
void av1_inv_txfm2d_add_4x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_inv_txfm2d_add_4x4)(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
void av1_inv_txfm2d_add_4x4_sse4_1(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#else
#define av1_inv_txfm2d_add_4x4 av1_inv_txfm2d_add_4x4_c
#endif
void av1_inv_txfm2d_add_8x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_inv_txfm2d_add_8x8)(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
void av1_inv_txfm2d_add_8x8_sse4_1(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#else
#define av1_inv_txfm2d_add_8x8 av1_inv_txfm2d_add_8x8_c
#endif
void av1_inv_txfm2d_add_16x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_16x16 av1_inv_txfm2d_add_16x16_c
void av1_inv_txfm2d_add_32x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_32x32 av1_inv_txfm2d_add_32x32_c

void av1_inv_txfm2d_add_64x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_64x64 av1_inv_txfm2d_add_64x64_c
void av1_inv_txfm2d_add_32x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_32x64 av1_inv_txfm2d_add_32x64_c
void av1_inv_txfm2d_add_64x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_64x32 av1_inv_txfm2d_add_64x32_c
void av1_inv_txfm2d_add_16x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_16x64 av1_inv_txfm2d_add_16x64_c
void av1_inv_txfm2d_add_64x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_64x16 av1_inv_txfm2d_add_64x16_c

void av1_inv_txfm2d_add_4x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_4x16 av1_inv_txfm2d_add_4x16_c
void av1_inv_txfm2d_add_16x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_16x4 av1_inv_txfm2d_add_16x4_c
void av1_inv_txfm2d_add_8x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_8x32 av1_inv_txfm2d_add_8x32_c
void av1_inv_txfm2d_add_32x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_32x8 av1_inv_txfm2d_add_32x8_c

// directional intra predictor functions
void av1_highbd_dr_prediction_z1_c(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_above, int dx, int dy, int bd, int mrl_index);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_dr_prediction_z1)(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_above, int dx, int dy, int bd, int mrl_index);
#if defined(__AVX2__)
void av1_highbd_dr_prediction_z1_avx2(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_above, int dx, int dy, int bd, int mrl_index);
#endif
#else
#define av1_highbd_dr_prediction_z1 av1_highbd_dr_prediction_z1_c
#endif
void av1_highbd_dr_prediction_z2_c(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_above, int upsample_left, int dx, int dy, int bd, int mrl_index);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_dr_prediction_z2)(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_above, int upsample_left, int dx, int dy, int bd, int mrl_index);
#if defined(__AVX2__)
void av1_highbd_dr_prediction_z2_avx2(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_above, int upsample_left, int dx, int dy, int bd, int mrl_index);
#endif
#else
#define av1_highbd_dr_prediction_z2 av1_highbd_dr_prediction_z2_c
#endif
void av1_highbd_dr_prediction_z3_c(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_left, int dx, int dy, int bd, int mrl_index);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_dr_prediction_z3)(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_left, int dx, int dy, int bd, int mrl_index);
#if defined(__AVX2__)
void av1_highbd_dr_prediction_z3_avx2(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int upsample_left, int dx, int dy, int bd, int mrl_index);
#endif
#else
#define av1_highbd_dr_prediction_z3 av1_highbd_dr_prediction_z3_c
#endif

#if CONFIG_IDIF
void av1_highbd_dr_prediction_z1_idif_c(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_dr_prediction_z1_idif)(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#if defined(__AVX2__)
void av1_highbd_dr_prediction_z1_idif_avx2(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#endif
#else
#define av1_highbd_dr_prediction_z1_idif av1_highbd_dr_prediction_z1_idif_c
#endif
void av1_highbd_dr_prediction_z2_idif_c(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_dr_prediction_z2_idif)(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#if defined(__AVX2__)
void av1_highbd_dr_prediction_z2_idif_avx2(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#endif
#else
#define av1_highbd_dr_prediction_z2_idif av1_highbd_dr_prediction_z2_idif_c
#endif
void av1_highbd_dr_prediction_z3_idif_c(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_dr_prediction_z3_idif)(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#if defined(__AVX2__)
void av1_highbd_dr_prediction_z3_idif_avx2(uint16_t *dst, ptrdiff_t stride, int bw, int bh, const uint16_t *above, const uint16_t *left, int dx, int dy, int bd, int mrl_index);
#endif
#else
#define av1_highbd_dr_prediction_z3_idif av1_highbd_dr_prediction_z3_idif_c
#endif
#endif

void av1_highbd_ibp_dr_prediction_z1_c(uint8_t* weights, uint16_t *dst, ptrdiff_t stride, uint16_t* second_pred, ptrdiff_t second_stride, int bw, int bh);
#define av1_highbd_ibp_dr_prediction_z1 av1_highbd_ibp_dr_prediction_z1_c
void av1_highbd_ibp_dr_prediction_z3_c(uint8_t* weights, uint16_t *dst, ptrdiff_t stride, uint16_t* second_pred, ptrdiff_t second_stride, int bw, int bh);
#define av1_highbd_ibp_dr_prediction_z3 av1_highbd_ibp_dr_prediction_z3_c

// build compound seg mask functions
void av1_build_compound_diffwtd_mask_highbd_c(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const uint16_t *src0, int src0_stride, const uint16_t *src1, int src1_stride, int h, int w, int bd);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN void (*av1_build_compound_diffwtd_mask_highbd)(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const uint16_t *src0, int src0_stride, const uint16_t *src1, int src1_stride, int h, int w, int bd);
void av1_build_compound_diffwtd_mask_highbd_ssse3(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const uint16_t *src0, int src0_stride, const uint16_t *src1, int src1_stride, int h, int w, int bd);
#endif
#if defined(__AVX2__)
void av1_build_compound_diffwtd_mask_highbd_avx2(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const uint16_t *src0, int src0_stride, const uint16_t *src1, int src1_stride, int h, int w, int bd);
#endif
#else
#define av1_build_compound_diffwtd_mask_highbd av1_build_compound_diffwtd_mask_highbd_c
#endif
void av1_build_compound_diffwtd_mask_d16_c(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const CONV_BUF_TYPE *src0, int src0_stride, const CONV_BUF_TYPE *src1, int src1_stride, int h, int w, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_build_compound_diffwtd_mask_d16)(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const CONV_BUF_TYPE *src0, int src0_stride, const CONV_BUF_TYPE *src1, int src1_stride, int h, int w, ConvolveParams *conv_params, int bd);
void av1_build_compound_diffwtd_mask_d16_sse4_1(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const CONV_BUF_TYPE *src0, int src0_stride, const CONV_BUF_TYPE *src1, int src1_stride, int h, int w, ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_build_compound_diffwtd_mask_d16_avx2(uint8_t *mask, DIFFWTD_MASK_TYPE mask_type, const CONV_BUF_TYPE *src0, int src0_stride, const CONV_BUF_TYPE *src1, int src1_stride, int h, int w, ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_build_compound_diffwtd_mask_d16 av1_build_compound_diffwtd_mask_d16_c
#endif

// Helper functions.
void av1_round_shift_array_c(int32_t *arr, int size, int bit);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_round_shift_array)(int32_t *arr, int size, int bit);
void av1_round_shift_array_sse4_1(int32_t *arr, int size, int bit);
#else
#define av1_round_shift_array av1_round_shift_array_c
#endif

// Resize functions.
void av1_resize_and_extend_frame_c(const YV12_BUFFER_CONFIG *src, YV12_BUFFER_CONFIG *dst, const InterpFilter filter, const int phase, const int num_planes);
#define av1_resize_and_extend_frame av1_resize_and_extend_frame_c

// Encoder functions below this point.
#if CONFIG_AV1_ENCODER

  // ENCODEMB INVOKE
  void av1_quantize_fp_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
  #define av1_quantize_fp av1_quantize_fp_c
  void av1_quantize_fp_32x32_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
  #define av1_quantize_fp_32x32 av1_quantize_fp_32x32_c
  void av1_quantize_fp_64x64_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
  #define av1_quantize_fp_64x64 av1_quantize_fp_64x64_c
  void av1_quantize_lp_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *round_ptr, const int32_t *quant_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan);
  #define av1_quantize_lp av1_quantize_lp_c
  void aom_quantize_b_helper_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan, const qm_val_t *qm_ptr, const qm_val_t *iqm_ptr, const int log_scale);
  #define aom_quantize_b_helper aom_quantize_b_helper_c

  // fdct functions
  void av1_fwht4x4_c(const int16_t *input, tran_low_t *output, int stride);
  #define av1_fwht4x4 av1_fwht4x4_c

  // fwd txfm
  void fwd_stxfm_c(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*fwd_stxfm)(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size);
  void fwd_stxfm_sse4_1(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size);
#else
  #define fwd_stxfm fwd_stxfm_c
#endif
  void av1_lowbd_fwd_txfm_c(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_lowbd_fwd_txfm)(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
  void av1_lowbd_fwd_txfm_sse2(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#if defined(__SSE4_1__)
  void av1_lowbd_fwd_txfm_sse4_1(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#endif
#if defined(__AVX2__)
  void av1_lowbd_fwd_txfm_avx2(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#endif
#else
  #define av1_lowbd_fwd_txfm av1_lowbd_fwd_txfm_c
#endif

  void av1_fwd_txfm2d_4x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_4x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_4x8 av1_fwd_txfm2d_4x8_c
#endif
  void av1_fwd_txfm2d_8x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_8x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_8x4 av1_fwd_txfm2d_8x4_c
#endif
  void av1_fwd_txfm2d_8x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_8x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_8x16_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#else
  #define av1_fwd_txfm2d_8x16 av1_fwd_txfm2d_8x16_c
#endif
  void av1_fwd_txfm2d_16x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_16x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_16x8_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#else
  #define av1_fwd_txfm2d_16x8 av1_fwd_txfm2d_16x8_c
#endif
  void av1_fwd_txfm2d_16x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_16x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_16x32 av1_fwd_txfm2d_16x32_c
#endif
  void av1_fwd_txfm2d_32x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_32x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_32x16 av1_fwd_txfm2d_32x16_c
#endif
  void av1_fwd_txfm2d_4x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_4x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_4x16 av1_fwd_txfm2d_4x16_c
#endif
  void av1_fwd_txfm2d_16x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_16x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_16x4 av1_fwd_txfm2d_16x4_c
#endif
  void av1_fwd_txfm2d_8x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_8x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_8x32 av1_fwd_txfm2d_8x32_c
#endif
  void av1_fwd_txfm2d_32x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_32x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_32x8 av1_fwd_txfm2d_32x8_c
#endif
  void av1_fwd_txfm2d_4x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_4x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_4x4 av1_fwd_txfm2d_4x4_c
#endif
  void av1_fwd_txfm2d_8x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_8x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_8x8_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#else
  #define av1_fwd_txfm2d_8x8 av1_fwd_txfm2d_8x8_c
#endif
  void av1_fwd_txfm2d_16x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_16x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_16x16_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#else
  #define av1_fwd_txfm2d_16x16 av1_fwd_txfm2d_16x16_c
#endif
  void av1_fwd_txfm2d_32x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_32x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_32x32_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#else
  #define av1_fwd_txfm2d_32x32 av1_fwd_txfm2d_32x32_c
#endif
  void av1_fwd_txfm2d_64x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_64x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_64x64_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#endif
#else
  #define av1_fwd_txfm2d_64x64 av1_fwd_txfm2d_64x64_c
#endif
  void av1_fwd_txfm2d_32x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_32x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_32x64 av1_fwd_txfm2d_32x64_c
#endif
  void av1_fwd_txfm2d_64x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_64x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_64x32 av1_fwd_txfm2d_64x32_c
#endif
  void av1_fwd_txfm2d_16x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_16x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_16x64 av1_fwd_txfm2d_16x64_c
#endif
  void av1_fwd_txfm2d_64x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
  void av1_fwd_txfm2d_64x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#else
  #define av1_fwd_txfm2d_64x16 av1_fwd_txfm2d_64x16_c
#endif

  // Motion search
  void av1_highbd_apply_temporal_filter_c(const struct yv12_buffer_config *ref_frame, const struct macroblockd *mbd, const BLOCK_SIZE block_size, const int mb_row, const int mb_col, const int num_planes, const double *noise_levels, const MV *subblock_mvs, const int *subblock_mses, const int q_factor, const int filter_strength, const uint16_t *pred, uint32_t *accum, uint16_t *count);
#if HAVE_SIMD
  void av1_highbd_apply_temporal_filter_sse2(const struct yv12_buffer_config *ref_frame, const struct macroblockd *mbd, const BLOCK_SIZE block_size, const int mb_row, const int mb_col, const int num_planes, const double *noise_levels, const MV *subblock_mvs, const int *subblock_mses, const int q_factor, const int filter_strength, const uint16_t *pred, uint32_t *accum, uint16_t *count);
  #define av1_highbd_apply_temporal_filter av1_highbd_apply_temporal_filter_sse2
#else
  #define av1_highbd_apply_temporal_filter av1_highbd_apply_temporal_filter_c
#endif

  void av1_quantize_b_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan, const qm_val_t * qm_ptr, const qm_val_t * iqm_ptr, int log_scale);
  #define av1_quantize_b av1_quantize_b_c

  // ENCODEMB INVOKE
  int64_t av1_highbd_block_error_c(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz, int bd);
#if HAVE_SIMD
  RTCD_EXTERN int64_t (*av1_highbd_block_error)(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz, int bd);
  int64_t av1_highbd_block_error_sse2(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz, int bd);
#if defined(__AVX2__)
  int64_t av1_highbd_block_error_avx2(const tran_low_t *coeff, const tran_low_t *dqcoeff, intptr_t block_size, int64_t *ssz, int bd);
#endif
#else
  #define av1_highbd_block_error av1_highbd_block_error_c
#endif

  void av1_highbd_quantize_fp_c(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan, int log_scale);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_highbd_quantize_fp)(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan, int log_scale);
  void av1_highbd_quantize_fp_sse4_1(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan, int log_scale);
#endif
#if defined(__AVX2__)
  void av1_highbd_quantize_fp_avx2(const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int32_t *zbin_ptr, const int32_t *round_ptr, const int32_t *quant_ptr, const int32_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int32_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan, int log_scale);
#endif
#else
  #define av1_highbd_quantize_fp av1_highbd_quantize_fp_c
#endif

  void av1_highbd_fwht4x4_c(const int16_t *input, tran_low_t *output, int stride);
  #define av1_highbd_fwht4x4 av1_highbd_fwht4x4_c

  // End av1_high encoder functions

  // txb
  void av1_txb_init_levels_skip_c(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_txb_init_levels_skip)(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
  void av1_txb_init_levels_skip_sse4_1(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
#endif
#if defined(__AVX2__)
  void av1_txb_init_levels_skip_avx2(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
#endif
#else
  #define av1_txb_init_levels_skip av1_txb_init_levels_skip_c
#endif
#if CONFIG_ATC_DCTX_ALIGNED
    void av1_get_nz_map_contexts_skip_c(const uint8_t *const levels, const int16_t *const scan, const uint16_t bob, const uint16_t eob, const TX_SIZE tx_size, int8_t *const coeff_contexts);
    #define av1_get_nz_map_contexts_skip av1_get_nz_map_contexts_skip_c
#else
    void av1_get_nz_map_contexts_skip_c(const uint8_t *const levels, const int16_t *const scan, const uint16_t eob, const TX_SIZE tx_size, int8_t *const coeff_contexts);
#if HAVE_SIMD
    void av1_get_nz_map_contexts_skip_sse2(const uint8_t *const levels, const int16_t *const scan, const uint16_t eob, const TX_SIZE tx_size, int8_t *const coeff_contexts);
    #define av1_get_nz_map_contexts_skip av1_get_nz_map_contexts_skip_sse2
#else
    #define av1_get_nz_map_contexts_skip av1_get_nz_map_contexts_skip_c
#endif
#endif

#if CONFIG_ATC
    void av1_get_nz_map_contexts_c(const uint8_t *const levels, const int16_t *const scan, const uint16_t eob, const TX_SIZE tx_size, const TX_CLASS tx_class, int8_t *const coeff_contexts, const int plane);
    #define av1_get_nz_map_contexts av1_get_nz_map_contexts_c
#else
    void av1_get_nz_map_contexts_c(const uint8_t *const levels, const int16_t *const scan, const uint16_t eob, const TX_SIZE tx_size, const TX_CLASS tx_class, int8_t *const coeff_contexts);
#if HAVE_SIMD
    void av1_get_nz_map_contexts_sse2(const uint8_t *const levels, const int16_t *const scan, const uint16_t eob, const TX_SIZE tx_size, const TX_CLASS tx_class, int8_t *const coeff_contexts);
    #define av1_get_nz_map_contexts av1_get_nz_map_contexts_sse2
#else
    #define av1_get_nz_map_contexts av1_get_nz_map_contexts_c
#endif
#endif

  void av1_txb_init_levels_signs_c(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels, int8_t *const signs);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_txb_init_levels_signs)(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels, int8_t *const signs);
  void av1_txb_init_levels_signs_sse4_1(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels, int8_t *const signs);
#endif
#if defined(__AVX2__)
  void av1_txb_init_levels_signs_avx2(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels, int8_t *const signs);
#endif
#else
  #define av1_txb_init_levels_signs av1_txb_init_levels_signs_c
#endif

  void av1_txb_init_levels_c(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_txb_init_levels)(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
  void av1_txb_init_levels_sse4_1(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
#endif
#if defined(__AVX2__)
  void av1_txb_init_levels_avx2(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels);
#endif
#else
  #define av1_txb_init_levels av1_txb_init_levels_c
#endif

  uint64_t av1_wedge_sse_from_residuals_c(const int16_t *r1, const int16_t *d, const uint8_t *m, int N);
#if HAVE_SIMD
  RTCD_EXTERN uint64_t (*av1_wedge_sse_from_residuals)(const int16_t *r1, const int16_t *d, const uint8_t *m, int N);
  uint64_t av1_wedge_sse_from_residuals_sse2(const int16_t *r1, const int16_t *d, const uint8_t *m, int N);
#if defined(__AVX2__)
  uint64_t av1_wedge_sse_from_residuals_avx2(const int16_t *r1, const int16_t *d, const uint8_t *m, int N);
#endif
#else
  #define av1_wedge_sse_from_residuals av1_wedge_sse_from_residuals_c
#endif
  int8_t av1_wedge_sign_from_residuals_c(const int16_t *ds, const uint8_t *m, int N, int64_t limit);
#if HAVE_SIMD
  RTCD_EXTERN int8_t (*av1_wedge_sign_from_residuals)(const int16_t *ds, const uint8_t *m, int N, int64_t limit);
  int8_t av1_wedge_sign_from_residuals_sse2(const int16_t *ds, const uint8_t *m, int N, int64_t limit);
#if defined(__AVX2__)
  int8_t av1_wedge_sign_from_residuals_avx2(const int16_t *ds, const uint8_t *m, int N, int64_t limit);
#endif
#else
  #define av1_wedge_sign_from_residuals av1_wedge_sign_from_residuals_c
#endif
  void av1_wedge_compute_delta_squares_c(int16_t *d, const int16_t *a, const int16_t *b, int N);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_wedge_compute_delta_squares)(int16_t *d, const int16_t *a, const int16_t *b, int N);
  void av1_wedge_compute_delta_squares_sse2(int16_t *d, const int16_t *a, const int16_t *b, int N);
#if defined(__AVX2__)
  void av1_wedge_compute_delta_squares_avx2(int16_t *d, const int16_t *a, const int16_t *b, int N);
#endif
#else
  #define av1_wedge_compute_delta_squares av1_wedge_compute_delta_squares_c
#endif

  // hash
  uint32_t av1_get_crc32c_value_c(void *crc_calculator, uint8_t *p, size_t length);
#if HAVE_SIMD && defined(__SSE4_2__)
  RTCD_EXTERN uint32_t (*av1_get_crc32c_value)(void *crc_calculator, uint8_t *p, size_t length);
  uint32_t av1_get_crc32c_value_sse4_2(void *crc_calculator, uint8_t *p, size_t length);
#else
  #define av1_get_crc32c_value av1_get_crc32c_value_c
#endif

  void av1_compute_stats_highbd_c(int wiener_win, const uint16_t *dgd8, const uint16_t *src, int h_start, int h_end, int v_start, int v_end, int dgd_stride, int src_stride, int64_t *M, int64_t *H, aom_bit_depth_t bit_depth);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_compute_stats_highbd)(int wiener_win, const uint16_t *dgd8, const uint16_t *src, int h_start, int h_end, int v_start, int v_end, int dgd_stride, int src_stride, int64_t *M, int64_t *H, aom_bit_depth_t bit_depth);
  void av1_compute_stats_highbd_sse4_1(int wiener_win, const uint16_t *dgd8, const uint16_t *src, int h_start, int h_end, int v_start, int v_end, int dgd_stride, int src_stride, int64_t *M, int64_t *H, aom_bit_depth_t bit_depth);
#endif
#if defined(__AVX2__)
  void av1_compute_stats_highbd_avx2(int wiener_win, const uint16_t *dgd8, const uint16_t *src, int h_start, int h_end, int v_start, int v_end, int dgd_stride, int src_stride, int64_t *M, int64_t *H, aom_bit_depth_t bit_depth);
#endif
#else
  #define av1_compute_stats_highbd av1_compute_stats_highbd_c
#endif

  int64_t av1_highbd_pixel_proj_error_c(const uint16_t *src8, int width, int height, int src_stride, const uint16_t *dat8, int dat_stride, int32_t *flt0, int flt0_stride, int32_t *flt1, int flt1_stride, int xq[2], const sgr_params_type *params);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN int64_t (*av1_highbd_pixel_proj_error)(const uint16_t *src8, int width, int height, int src_stride, const uint16_t *dat8, int dat_stride, int32_t *flt0, int flt0_stride, int32_t *flt1, int flt1_stride, int xq[2], const sgr_params_type *params);
  int64_t av1_highbd_pixel_proj_error_sse4_1(const uint16_t *src8, int width, int height, int src_stride, const uint16_t *dat8, int dat_stride, int32_t *flt0, int flt0_stride, int32_t *flt1, int flt1_stride, int xq[2], const sgr_params_type *params);
#endif
#if defined(__AVX2__)
  int64_t av1_highbd_pixel_proj_error_avx2(const uint16_t *src8, int width, int height, int src_stride, const uint16_t *dat8, int dat_stride, int32_t *flt0, int flt0_stride, int32_t *flt1, int flt1_stride, int xq[2], const sgr_params_type *params);
#endif
#else
  #define av1_highbd_pixel_proj_error av1_highbd_pixel_proj_error_c
#endif
  void av1_get_horver_correlation_full_c(const int16_t *diff, int stride, int w, int h, float *hcorr, float *vcorr);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_get_horver_correlation_full)(const int16_t *diff, int stride, int w, int h, float *hcorr, float *vcorr);
  void av1_get_horver_correlation_full_sse4_1(const int16_t *diff, int stride, int w, int h, float *hcorr, float *vcorr);
#endif
#if defined(__AVX2__)
  void av1_get_horver_correlation_full_avx2(const int16_t *diff, int stride, int w, int h, float *hcorr, float *vcorr);
#endif
#else
  #define av1_get_horver_correlation_full av1_get_horver_correlation_full_c
#endif

  void av1_nn_predict_c(const float *input_nodes, const NN_CONFIG *const nn_config, int reduce_prec, float *const output);
#if HAVE_SIMD && CONFIG_EXCLUDE_SIMD_MISMATCH && defined(__SSSE3__)
  RTCD_EXTERN void (*av1_nn_predict)(const float *input_nodes, const NN_CONFIG *const nn_config, int reduce_prec, float *const output);
  void av1_nn_predict_ssse3(const float *input_nodes, const NN_CONFIG *const nn_config, int reduce_prec, float *const output);
#else
  #define av1_nn_predict av1_nn_predict_c
#endif
#endif
// end encoder functions

// CNN functions

void av1_cnn_activate_c( float **input, int channels, int width, int height, int stride, ACTIVATION layer_activation);
#define av1_cnn_activate av1_cnn_activate_c
void av1_cnn_add_c( float **input, int channels, int width, int height, int stride, const float **add);
#define av1_cnn_add av1_cnn_add_c
void av1_cnn_predict_c( const float **input, int in_width, int in_height, int in_stride, const CNN_CONFIG *cnn_config, const CNN_THREAD_DATA *thread_data, CNN_MULTI_OUT *output_struct);
#define av1_cnn_predict av1_cnn_predict_c
void av1_cnn_convolve_c( const float **input, int in_width, int in_height, int in_stride, const CNN_LAYER_CONFIG *layer_config, float **output, int out_stride, int start_idx, int step);
#define av1_cnn_convolve av1_cnn_convolve_c
void av1_cnn_deconvolve_c( const float **input, int in_width, int in_height, int in_stride, const CNN_LAYER_CONFIG *layer_config, float **output, int out_stride);
#define av1_cnn_deconvolve av1_cnn_deconvolve_c
void av1_cnn_batchnorm_c(float **image, int channels, int width, int height, int stride, const float *gamma, const float *beta, const float *mean, const float *std);
#define av1_cnn_batchnorm av1_cnn_batchnorm_c

// Deringing Functions

// VS compiling for 32 bit targets does not support vector types in
// structs as arguments, which makes the v256 type of the intrinsics
// hard to support, so optimizations for this target are disabled.

int cdef_find_dir_c(const uint16_t *img, int stride, int32_t *var, int coeff_shift);
#if HAVE_SIMD && !defined(__MSVCRT__)
  RTCD_EXTERN int (*cdef_find_dir)(const uint16_t *img, int stride, int32_t *var, int coeff_shift);
  int cdef_find_dir_sse2(const uint16_t *img, int stride, int32_t *var, int coeff_shift);
#if defined(__SSSE3__)
  int cdef_find_dir_ssse3(const uint16_t *img, int stride, int32_t *var, int coeff_shift);
#endif
#if defined(__SSE4_1__)
  int cdef_find_dir_sse4_1(const uint16_t *img, int stride, int32_t *var, int coeff_shift);
#endif
#if defined(__AVX2__)
  int cdef_find_dir_avx2(const uint16_t *img, int stride, int32_t *var, int coeff_shift);
#endif
#else
  #define cdef_find_dir cdef_find_dir_c
#endif
void cdef_filter_block_c(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, int bsize, int coeff_shift);
#if HAVE_SIMD && !defined(__MSVCRT__)
  RTCD_EXTERN void (*cdef_filter_block)(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, int bsize, int coeff_shift);
  void cdef_filter_block_sse2(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, int bsize, int coeff_shift);
#if defined(__SSSE3__)
  void cdef_filter_block_ssse3(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, int bsize, int coeff_shift);
#endif
#if defined(__SSE4_1__)
  void cdef_filter_block_sse4_1(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, int bsize, int coeff_shift);
#endif
#if defined(__AVX2__)
  void cdef_filter_block_avx2(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, int bsize, int coeff_shift);
#endif
#else
  #define cdef_filter_block cdef_filter_block_c
#endif

void cdef_copy_rect8_16bit_to_16bit_c(uint16_t *dst, int dstride, const uint16_t *src, int sstride, int v, int h);
#if HAVE_SIMD && !defined(__MSVCRT__)
  RTCD_EXTERN void (*cdef_copy_rect8_16bit_to_16bit)(uint16_t *dst, int dstride, const uint16_t *src, int sstride, int v, int h);
  void cdef_copy_rect8_16bit_to_16bit_sse2(uint16_t *dst, int dstride, const uint16_t *src, int sstride, int v, int h);
#if defined(__SSSE3__)
  void cdef_copy_rect8_16bit_to_16bit_ssse3(uint16_t *dst, int dstride, const uint16_t *src, int sstride, int v, int h);
#endif
#if defined(__SSE4_1__)
  void cdef_copy_rect8_16bit_to_16bit_sse4_1(uint16_t *dst, int dstride, const uint16_t *src, int sstride, int v, int h);
#endif
#if defined(__AVX2__)
  void cdef_copy_rect8_16bit_to_16bit_avx2(uint16_t *dst, int dstride, const uint16_t *src, int sstride, int v, int h);
#endif
#else
  #define cdef_copy_rect8_16bit_to_16bit cdef_copy_rect8_16bit_to_16bit_c
#endif

// Cross-component Sample Offset
#if CONFIG_CCSO && CONFIG_CCSO_EXT
  void ccso_filter_block_hbd_wo_buf_c(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size, const bool isSingleBand, const uint8_t shift_bits);
#if HAVE_SIMD
  RTCD_EXTERN void (*ccso_filter_block_hbd_wo_buf)(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size, const bool isSingleBand, const uint8_t shift_bits);
#if defined(__AVX2__)
  void ccso_filter_block_hbd_wo_buf_avx2(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size, const bool isSingleBand, const uint8_t shift_bits);
#endif
#else
  #define ccso_filter_block_hbd_wo_buf ccso_filter_block_hbd_wo_buf_c
#endif

#if CONFIG_AV1_ENCODER
    void ccso_filter_block_hbd_with_buf_c(const uint16_t *src_y, uint16_t *dst_yuv, const uint8_t *src_cls0, const uint8_t *src_cls1, const int src_y_stride, const int dst_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int8_t *filter_offset, const int blk_size, const int y_uv_hscale,  const int y_uv_vscale, const int max_val, const uint8_t shift_bits);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_filter_block_hbd_with_buf)(const uint16_t *src_y, uint16_t *dst_yuv, const uint8_t *src_cls0, const uint8_t *src_cls1, const int src_y_stride, const int dst_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int8_t *filter_offset, const int blk_size, const int y_uv_hscale,  const int y_uv_vscale, const int max_val, const uint8_t shift_bits);
#if defined(__AVX2__)
    void ccso_filter_block_hbd_with_buf_avx2(const uint16_t *src_y, uint16_t *dst_yuv, const uint8_t *src_cls0, const uint8_t *src_cls1, const int src_y_stride, const int dst_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int8_t *filter_offset, const int blk_size, const int y_uv_hscale,  const int y_uv_vscale, const int max_val, const uint8_t shift_bits);
#endif
#else
    #define ccso_filter_block_hbd_with_buf ccso_filter_block_hbd_with_buf_c
#endif
    uint64_t compute_distortion_block_c(const uint16_t *org, const int org_stride, const uint16_t *rec16, const int rec_stride, const int x, const int y, const int log2_filter_unit_size, const int height, const int width);
#if HAVE_SIMD
    RTCD_EXTERN uint64_t (*compute_distortion_block)(const uint16_t *org, const int org_stride, const uint16_t *rec16, const int rec_stride, const int x, const int y, const int log2_filter_unit_size, const int height, const int width);
#if defined(__AVX2__)
    uint64_t compute_distortion_block_avx2(const uint16_t *org, const int org_stride, const uint16_t *rec16, const int rec_stride, const int x, const int y, const int log2_filter_unit_size, const int height, const int width);
#endif
#else
    #define compute_distortion_block compute_distortion_block_c
#endif
    void ccso_derive_src_block_c(const uint16_t *src_y, uint8_t *const src_cls0, uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int y_uv_hscale, const int y_uv_vscale, const int qstep, const int neg_qstep, const int *src_loc, const int blk_size);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_derive_src_block)(const uint16_t *src_y, uint8_t *const src_cls0, uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int y_uv_hscale, const int y_uv_vscale, const int qstep, const int neg_qstep, const int *src_loc, const int blk_size);
#if defined(__AVX2__)
    void ccso_derive_src_block_avx2(const uint16_t *src_y, uint8_t *const src_cls0, uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int y_uv_hscale, const int y_uv_vscale, const int qstep, const int neg_qstep, const int *src_loc, const int blk_size);
#endif
#else
    #define ccso_derive_src_block ccso_derive_src_block_c
#endif
#endif
#endif

// Prediction enhancement filter
#if CONFIG_PEF
  void highbd_filt_horz_pred_c(uint16_t *s, int stride, int bd, uint16_t q_thresh, uint16_t side_thresh, int q_mult, int w_mult, int n, int filt_len);
#if HAVE_SIMD
  RTCD_EXTERN void (*highbd_filt_horz_pred)(uint16_t *s, int stride, int bd, uint16_t q_thresh, uint16_t side_thresh, int q_mult, int w_mult, int n, int filt_len);
#if defined(__AVX2__)
  void highbd_filt_horz_pred_avx2(uint16_t *s, int stride, int bd, uint16_t q_thresh, uint16_t side_thresh, int q_mult, int w_mult, int n, int filt_len);
#endif
#else
  #define highbd_filt_horz_pred highbd_filt_horz_pred_c
#endif
  void highbd_filt_vert_pred_c(uint16_t *s, int stride, int bd, uint16_t q_thresh, uint16_t side_thresh, int q_mult, int w_mult, int n, int filt_len);
#if HAVE_SIMD
  RTCD_EXTERN void (*highbd_filt_vert_pred)(uint16_t *s, int stride, int bd, uint16_t q_thresh, uint16_t side_thresh, int q_mult, int w_mult, int n, int filt_len);
#if defined(__AVX2__)
  void highbd_filt_vert_pred_avx2(uint16_t *s, int stride, int bd, uint16_t q_thresh, uint16_t side_thresh, int q_mult, int w_mult, int n, int filt_len);
#endif
#else
  #define highbd_filt_vert_pred highbd_filt_vert_pred_c
#endif
#endif

// WARPED_MOTION / GLOBAL_MOTION functions

void av1_highbd_warp_affine_c(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_warp_affine)(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta);
void av1_highbd_warp_affine_sse4_1(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta);
#endif
#if defined(__AVX2__)
void av1_highbd_warp_affine_avx2(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta);
#endif
#else
#define av1_highbd_warp_affine av1_highbd_warp_affine_c
#endif

#if CONFIG_AV1_ENCODER
  double av1_compute_cross_correlation_c(unsigned char *im1, int stride1, int x1, int y1, unsigned char *im2, int stride2, int x2, int y2);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN double (*av1_compute_cross_correlation)(unsigned char *im1, int stride1, int x1, int y1, unsigned char *im2, int stride2, int x2, int y2);
  double av1_compute_cross_correlation_sse4_1(unsigned char *im1, int stride1, int x1, int y1, unsigned char *im2, int stride2, int x2, int y2);
#endif
#if defined(__AVX2__)
  double av1_compute_cross_correlation_avx2(unsigned char *im1, int stride1, int x1, int y1, unsigned char *im2, int stride2, int x2, int y2);
#endif
#else
  #define av1_compute_cross_correlation av1_compute_cross_correlation_c
#endif
#endif

// LOOP_RESTORATION functions

void av1_apply_selfguided_restoration_c(const uint16_t *dat, int width, int height, int stride, int eps, const int *xqd, uint16_t *dst, int dst_stride, int32_t *tmpbuf, int bit_depth);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_apply_selfguided_restoration)(const uint16_t *dat, int width, int height, int stride, int eps, const int *xqd, uint16_t *dst, int dst_stride, int32_t *tmpbuf, int bit_depth);
void av1_apply_selfguided_restoration_sse4_1(const uint16_t *dat, int width, int height, int stride, int eps, const int *xqd, uint16_t *dst, int dst_stride, int32_t *tmpbuf, int bit_depth);
#endif
#if defined(__AVX2__)
void av1_apply_selfguided_restoration_avx2(const uint16_t *dat, int width, int height, int stride, int eps, const int *xqd, uint16_t *dst, int dst_stride, int32_t *tmpbuf, int bit_depth);
#endif
#else
#define av1_apply_selfguided_restoration av1_apply_selfguided_restoration_c
#endif

int av1_selfguided_restoration_c(const uint16_t *dgd, int width, int height, int dgd_stride, int32_t *flt0, int32_t *flt1, int flt_stride, int sgr_params_idx, int bit_depth);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN int (*av1_selfguided_restoration)(const uint16_t *dgd, int width, int height, int dgd_stride, int32_t *flt0, int32_t *flt1, int flt_stride, int sgr_params_idx, int bit_depth);
int av1_selfguided_restoration_sse4_1(const uint16_t *dgd, int width, int height, int dgd_stride, int32_t *flt0, int32_t *flt1, int flt_stride, int sgr_params_idx, int bit_depth);
#endif
#if defined(__AVX2__)
int av1_selfguided_restoration_avx2(const uint16_t *dgd, int width, int height, int dgd_stride, int32_t *flt0, int32_t *flt1, int flt_stride, int sgr_params_idx, int bit_depth);
#endif
#else
#define av1_selfguided_restoration av1_selfguided_restoration_c
#endif

// CONVOLVE_ROUND/COMPOUND_ROUND functions

void av1_highbd_convolve_2d_sr_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN void (*av1_highbd_convolve_2d_sr)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
void av1_highbd_convolve_2d_sr_ssse3(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_convolve_2d_sr_avx2(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_convolve_2d_sr av1_highbd_convolve_2d_sr_c
#endif
void av1_highbd_convolve_x_sr_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN void (*av1_highbd_convolve_x_sr)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
void av1_highbd_convolve_x_sr_ssse3(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_convolve_x_sr_avx2(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_convolve_x_sr av1_highbd_convolve_x_sr_c
#endif
void av1_highbd_convolve_y_sr_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, int bd);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN void (*av1_highbd_convolve_y_sr)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, int bd);
void av1_highbd_convolve_y_sr_ssse3(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_convolve_y_sr_avx2(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, int bd);
#endif
#else
#define av1_highbd_convolve_y_sr av1_highbd_convolve_y_sr_c
#endif
void av1_highbd_dist_wtd_convolve_2d_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_dist_wtd_convolve_2d)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
void av1_highbd_dist_wtd_convolve_2d_sse4_1(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_dist_wtd_convolve_2d_avx2(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_dist_wtd_convolve_2d av1_highbd_dist_wtd_convolve_2d_c
#endif
void av1_highbd_dist_wtd_convolve_x_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_dist_wtd_convolve_x)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
void av1_highbd_dist_wtd_convolve_x_sse4_1(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_dist_wtd_convolve_x_avx2(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const int subpel_x_qn, ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_dist_wtd_convolve_x av1_highbd_dist_wtd_convolve_x_c
#endif
void av1_highbd_dist_wtd_convolve_y_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_dist_wtd_convolve_y)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
void av1_highbd_dist_wtd_convolve_y_sse4_1(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_dist_wtd_convolve_y_avx2(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_y, const int subpel_y_qn, ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_dist_wtd_convolve_y av1_highbd_dist_wtd_convolve_y_c
#endif
void av1_highbd_dist_wtd_convolve_2d_copy_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_dist_wtd_convolve_2d_copy)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, ConvolveParams *conv_params, int bd);
void av1_highbd_dist_wtd_convolve_2d_copy_sse4_1(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, ConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_dist_wtd_convolve_2d_copy_avx2(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, ConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_dist_wtd_convolve_2d_copy av1_highbd_dist_wtd_convolve_2d_copy_c
#endif
void av1_highbd_convolve_2d_scale_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int x_step_qn, const int subpel_y_qn, const int y_step_qn, ConvolveParams *conv_params, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_convolve_2d_scale)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int x_step_qn, const int subpel_y_qn, const int y_step_qn, ConvolveParams *conv_params, int bd);
void av1_highbd_convolve_2d_scale_sse4_1(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const InterpFilterParams *filter_params_x, const InterpFilterParams *filter_params_y, const int subpel_x_qn, const int x_step_qn, const int subpel_y_qn, const int y_step_qn, ConvolveParams *conv_params, int bd);
#else
#define av1_highbd_convolve_2d_scale av1_highbd_convolve_2d_scale_c
#endif

// INTRA_EDGE functions
void av1_filter_intra_edge_high_c(uint16_t *p, int sz, int strength);
#if HAVE_SIMD && defined(__SSE4_1__)
RTCD_EXTERN void (*av1_filter_intra_edge_high)(uint16_t *p, int sz, int strength);
void av1_filter_intra_edge_high_sse4_1(uint16_t *p, int sz, int strength);
#else
#define av1_filter_intra_edge_high av1_filter_intra_edge_high_c
#endif
void av1_upsample_intra_edge_high_c(uint16_t *p, int sz, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
RTCD_EXTERN void (*av1_upsample_intra_edge_high)(uint16_t *p, int sz, int bd);
void av1_upsample_intra_edge_high_sse4_1(uint16_t *p, int sz, int bd);
#else
#define av1_upsample_intra_edge_high av1_upsample_intra_edge_high_c
#endif

// CFL
cfl_subtract_average_fn cfl_get_subtract_average_fn_c(TX_SIZE tx_size);
#if HAVE_SIMD
RTCD_EXTERN cfl_subtract_average_fn (*cfl_get_subtract_average_fn)(TX_SIZE tx_size);
cfl_subtract_average_fn cfl_get_subtract_average_fn_sse2(TX_SIZE tx_size);
#if defined(__AVX2__)
cfl_subtract_average_fn cfl_get_subtract_average_fn_avx2(TX_SIZE tx_size);
#endif
#else
#define cfl_get_subtract_average_fn cfl_get_subtract_average_fn_c
#endif

cfl_subsample_hbd_fn cfl_get_luma_subsampling_420_hbd_c(TX_SIZE tx_size);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN cfl_subsample_hbd_fn (*cfl_get_luma_subsampling_420_hbd)(TX_SIZE tx_size);
cfl_subsample_hbd_fn cfl_get_luma_subsampling_420_hbd_ssse3(TX_SIZE tx_size);
#endif
#if defined(__AVX2__)
cfl_subsample_hbd_fn cfl_get_luma_subsampling_420_hbd_avx2(TX_SIZE tx_size);
#endif
#else
#define cfl_get_luma_subsampling_420_hbd cfl_get_luma_subsampling_420_hbd_c
#endif

cfl_subsample_hbd_fn cfl_get_luma_subsampling_422_hbd_c(TX_SIZE tx_size);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN cfl_subsample_hbd_fn (*cfl_get_luma_subsampling_422_hbd)(TX_SIZE tx_size);
cfl_subsample_hbd_fn cfl_get_luma_subsampling_422_hbd_ssse3(TX_SIZE tx_size);
#endif
#if defined(__AVX2__)
cfl_subsample_hbd_fn cfl_get_luma_subsampling_422_hbd_avx2(TX_SIZE tx_size);
#endif
#else
#define cfl_get_luma_subsampling_422_hbd cfl_get_luma_subsampling_422_hbd_c
#endif

cfl_subsample_hbd_fn cfl_get_luma_subsampling_444_hbd_c(TX_SIZE tx_size);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN cfl_subsample_hbd_fn (*cfl_get_luma_subsampling_444_hbd)(TX_SIZE tx_size);
cfl_subsample_hbd_fn cfl_get_luma_subsampling_444_hbd_ssse3(TX_SIZE tx_size);
#endif
#if defined(__AVX2__)
cfl_subsample_hbd_fn cfl_get_luma_subsampling_444_hbd_avx2(TX_SIZE tx_size);
#endif
#else
#define cfl_get_luma_subsampling_444_hbd cfl_get_luma_subsampling_444_hbd_c
#endif

cfl_predict_hbd_fn cfl_get_predict_hbd_fn_c(TX_SIZE tx_size);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN cfl_predict_hbd_fn (*cfl_get_predict_hbd_fn)(TX_SIZE tx_size);
cfl_predict_hbd_fn cfl_get_predict_hbd_fn_ssse3(TX_SIZE tx_size);
#endif
#if defined(__AVX2__)
cfl_predict_hbd_fn cfl_get_predict_hbd_fn_avx2(TX_SIZE tx_size);
#endif
#else
#define cfl_get_predict_hbd_fn cfl_get_predict_hbd_fn_c
#endif

void av1_rtcd(void);

#ifdef RTCD_C
#include "avm_ports/x86.h"
static void setup_rtcd_internal(void) {
  int flags = x86_simd_caps();

  (void)flags;

#if HAVE_SIMD
  av1_highbd_convolve_horiz_rs = av1_highbd_convolve_horiz_rs_c;
  av1_highbd_wiener_convolve_add_src = av1_highbd_wiener_convolve_add_src_c;
#if CONFIG_PC_WIENER
  av1_fill_tskip_sum_buffer = av1_fill_tskip_sum_buffer_c;
  fill_directional_feature_buffers_highbd = fill_directional_feature_buffers_highbd_c;
  av1_fill_directional_feature_accumulators = av1_fill_directional_feature_accumulators_c;
  av1_fill_tskip_feature_accumulator =av1_fill_tskip_feature_accumulator_c;
#endif
#if CONFIG_PC_WIENER || CONFIG_WIENER_NONSEP
  av1_convolve_symmetric_highbd = av1_convolve_symmetric_highbd_c;
  av1_convolve_symmetric_subtract_center_highbd = av1_convolve_symmetric_subtract_center_highbd_c;
#endif
#if CONFIG_WIENER_NONSEP_CROSS_FILT
  av1_convolve_symmetric_dual_highbd = av1_convolve_symmetric_dual_highbd_c;
  av1_convolve_symmetric_dual_subtract_center_highbd = av1_convolve_symmetric_dual_subtract_center_highbd_c;
#endif
  av1_filter_intra_predictor = av1_filter_intra_predictor_c;
#if CONFIG_OPTFLOW_REFINEMENT
  av1_bicubic_grad_interpolation_highbd = av1_bicubic_grad_interpolation_highbd_c;
  av1_opfl_mv_refinement_nxn_highbd = av1_opfl_mv_refinement_nxn_highbd_c;
  av1_opfl_mv_refinement_nxn_interp_grad = av1_opfl_mv_refinement_nxn_interp_grad_c;
  av1_copy_pred_array_highbd = av1_copy_pred_array_highbd_c;
#endif
  inv_stxfm = inv_stxfm_c;
  av1_highbd_inv_txfm_add = av1_highbd_inv_txfm_add_c;
  av1_highbd_inv_txfm_add_4x4 = av1_highbd_inv_txfm_add_4x4_c;
  av1_highbd_inv_txfm_add_8x8 = av1_highbd_inv_txfm_add_8x8_c;
  av1_highbd_inv_txfm_add_4x8 = av1_highbd_inv_txfm_add_4x8_c;
  av1_highbd_inv_txfm_add_8x4 = av1_highbd_inv_txfm_add_8x4_c;
  av1_highbd_inv_txfm_add_4x16 = av1_highbd_inv_txfm_add_4x16_c;
  av1_highbd_inv_txfm_add_16x4 = av1_highbd_inv_txfm_add_16x4_c;
  av1_inv_txfm2d_add_8x8 = av1_inv_txfm2d_add_8x8_c;
  av1_highbd_dr_prediction_z1 = av1_highbd_dr_prediction_z1_c;
  av1_highbd_dr_prediction_z2 = av1_highbd_dr_prediction_z2_c;
  av1_highbd_dr_prediction_z3 = av1_highbd_dr_prediction_z3_c;
#if CONFIG_IDIF
  av1_highbd_dr_prediction_z1_idif = av1_highbd_dr_prediction_z1_idif_c;
  av1_highbd_dr_prediction_z2_idif = av1_highbd_dr_prediction_z2_idif_c;
  av1_highbd_dr_prediction_z3_idif = av1_highbd_dr_prediction_z3_idif_c;
#endif
  av1_build_compound_diffwtd_mask_highbd = av1_build_compound_diffwtd_mask_highbd_c;
  av1_build_compound_diffwtd_mask_d16 = av1_build_compound_diffwtd_mask_d16_c;
  av1_round_shift_array = av1_round_shift_array_c;
#if CONFIG_AV1_ENCODER
  fwd_stxfm = fwd_stxfm_c;
  av1_fwd_txfm2d_4x8 = av1_fwd_txfm2d_4x8_c;
  av1_fwd_txfm2d_8x4 = av1_fwd_txfm2d_8x4_c;
  av1_fwd_txfm2d_8x16 = av1_fwd_txfm2d_8x16_c;
  av1_fwd_txfm2d_16x8 = av1_fwd_txfm2d_16x8_c;
  av1_fwd_txfm2d_16x32 = av1_fwd_txfm2d_16x32_c;
  av1_fwd_txfm2d_32x16 = av1_fwd_txfm2d_32x16_c;
  av1_fwd_txfm2d_4x16 = av1_fwd_txfm2d_4x16_c;
  av1_fwd_txfm2d_16x4 = av1_fwd_txfm2d_16x4_c;
  av1_fwd_txfm2d_8x32 = av1_fwd_txfm2d_8x32_c;
  av1_fwd_txfm2d_32x8 = av1_fwd_txfm2d_32x8_c;
  av1_fwd_txfm2d_4x4 = av1_fwd_txfm2d_4x4_c;
  av1_fwd_txfm2d_8x8 = av1_fwd_txfm2d_8x8_c;
  av1_fwd_txfm2d_16x16 = av1_fwd_txfm2d_16x16_c;
  av1_fwd_txfm2d_32x32 = av1_fwd_txfm2d_32x32_c;
  av1_fwd_txfm2d_64x64 = av1_fwd_txfm2d_64x64_c;
  av1_fwd_txfm2d_32x64 = av1_fwd_txfm2d_32x64_c;
  av1_fwd_txfm2d_64x32 = av1_fwd_txfm2d_64x32_c;
  av1_fwd_txfm2d_16x64 = av1_fwd_txfm2d_16x64_c;
  av1_fwd_txfm2d_64x16 = av1_fwd_txfm2d_64x16_c;
  av1_highbd_quantize_fp = av1_highbd_quantize_fp_c;
  av1_txb_init_levels_skip = av1_txb_init_levels_skip_c;
  av1_txb_init_levels_signs = av1_txb_init_levels_signs_c;
  av1_txb_init_levels = av1_txb_init_levels_c;
  av1_get_crc32c_value = av1_get_crc32c_value_c;
  av1_compute_stats_highbd = av1_compute_stats_highbd_c;
  av1_highbd_pixel_proj_error = av1_highbd_pixel_proj_error_c;
  av1_get_horver_correlation_full = av1_get_horver_correlation_full_c;
#if CONFIG_EXCLUDE_SIMD_MISMATCH
  av1_nn_predict = av1_nn_predict_c;
#endif
#endif
#if CONFIG_CCSO && CONFIG_CCSO_EXT
  ccso_filter_block_hbd_wo_buf = ccso_filter_block_hbd_wo_buf_c;
#if CONFIG_AV1_ENCODER
  ccso_filter_block_hbd_with_buf = ccso_filter_block_hbd_with_buf_c;
  compute_distortion_block = compute_distortion_block_c;
  ccso_derive_src_block = ccso_derive_src_block_c;
#endif
#endif
#if CONFIG_PEF
  highbd_filt_horz_pred = highbd_filt_horz_pred_c;
  highbd_filt_vert_pred = highbd_filt_vert_pred_c;
#endif
  av1_highbd_warp_affine = av1_highbd_warp_affine_c;
#if CONFIG_AV1_ENCODER
  av1_compute_cross_correlation = av1_compute_cross_correlation_c;
#endif
  av1_apply_selfguided_restoration = av1_apply_selfguided_restoration_c;
  av1_selfguided_restoration = av1_selfguided_restoration_c;
  av1_highbd_dist_wtd_convolve_2d = av1_highbd_dist_wtd_convolve_2d_c;
  av1_highbd_dist_wtd_convolve_x = av1_highbd_dist_wtd_convolve_x_c;
  av1_highbd_dist_wtd_convolve_y = av1_highbd_dist_wtd_convolve_y_c;
  av1_highbd_dist_wtd_convolve_2d_copy = av1_highbd_dist_wtd_convolve_2d_copy_c;
  av1_highbd_convolve_2d_sr = av1_highbd_convolve_2d_sr_c;
  av1_highbd_convolve_x_sr = av1_highbd_convolve_x_sr_c;
  av1_highbd_convolve_y_sr = av1_highbd_convolve_y_sr_c;
  av1_highbd_convolve_2d_scale = av1_highbd_convolve_2d_scale_c;
  av1_filter_intra_edge_high = av1_filter_intra_edge_high_c;
  av1_upsample_intra_edge_high = av1_upsample_intra_edge_high_c;
  cfl_get_luma_subsampling_420_hbd = cfl_get_luma_subsampling_420_hbd_c;
  cfl_get_luma_subsampling_422_hbd = cfl_get_luma_subsampling_422_hbd_c;
  cfl_get_luma_subsampling_444_hbd = cfl_get_luma_subsampling_444_hbd_c;
  cfl_get_predict_hbd_fn = cfl_get_predict_hbd_fn_c;
#endif
#if defined(__SSE2__) && HAVE_SIMD
  av1_lowbd_fwd_txfm = av1_lowbd_fwd_txfm_sse2;
  av1_highbd_block_error = av1_highbd_block_error_sse2;
#if !CONFIG_ATC
  av1_get_nz_map_contexts = av1_get_nz_map_contexts_sse2;
#endif
  av1_wedge_sse_from_residuals = av1_wedge_sse_from_residuals_sse2;
  av1_wedge_sign_from_residuals = av1_wedge_sign_from_residuals_sse2;
  av1_wedge_compute_delta_squares = av1_wedge_compute_delta_squares_sse2;
#if !defined(__MSVCRT__)
  cdef_find_dir = cdef_find_dir_sse2;
  cdef_filter_block = cdef_filter_block_sse2;
  cdef_copy_rect8_16bit_to_16bit = cdef_copy_rect8_16bit_to_16bit_sse2;
#endif
  cfl_get_subtract_average_fn = cfl_get_subtract_average_fn_sse2;
#endif
#if defined(__SSSE3__) && HAVE_SIMD
  if (flags & HAS_SSSE3) av1_highbd_wiener_convolve_add_src = av1_highbd_wiener_convolve_add_src_ssse3;
  if (flags & HAS_SSSE3) av1_build_compound_diffwtd_mask_highbd = av1_build_compound_diffwtd_mask_highbd_ssse3;
#if !defined(__MSVCRT__)
  if (flags & HAS_SSSE3) cdef_find_dir = cdef_find_dir_ssse3;
  if (flags & HAS_SSSE3) cdef_filter_block = cdef_filter_block_ssse3;
  if (flags & HAS_SSSE3) cdef_copy_rect8_16bit_to_16bit = cdef_copy_rect8_16bit_to_16bit_ssse3;
#endif
  if (flags & HAS_SSSE3) av1_highbd_convolve_2d_sr = av1_highbd_convolve_2d_sr_ssse3;
  if (flags & HAS_SSSE3) av1_highbd_convolve_x_sr = av1_highbd_convolve_x_sr_ssse3;
  if (flags & HAS_SSSE3) av1_highbd_convolve_y_sr = av1_highbd_convolve_y_sr_ssse3;
  if (flags & HAS_SSSE3) cfl_get_luma_subsampling_420_hbd = cfl_get_luma_subsampling_420_hbd_ssse3;
  if (flags & HAS_SSSE3) cfl_get_luma_subsampling_422_hbd = cfl_get_luma_subsampling_422_hbd_ssse3;
  if (flags & HAS_SSSE3) cfl_get_luma_subsampling_444_hbd = cfl_get_luma_subsampling_444_hbd_ssse3;
  if (flags & HAS_SSSE3) cfl_get_predict_hbd_fn = cfl_get_predict_hbd_fn_ssse3;
#endif
#if defined(__SSE4_1__) && HAVE_SIMD
  if (flags & HAS_SSE4_1) av1_highbd_convolve_horiz_rs = av1_highbd_convolve_horiz_rs_sse4_1;
  if (flags & HAS_SSE4_1) av1_filter_intra_predictor = av1_filter_intra_predictor_sse4_1;
#if CONFIG_OPTFLOW_REFINEMENT
  if (flags & HAS_SSE4_1) av1_bicubic_grad_interpolation_highbd = av1_bicubic_grad_interpolation_highbd_sse4_1;
  if (flags & HAS_SSE4_1) av1_opfl_mv_refinement_nxn_highbd = av1_opfl_mv_refinement_nxn_highbd_sse4_1;
  if (flags & HAS_SSE4_1) av1_opfl_mv_refinement_nxn_interp_grad = av1_opfl_mv_refinement_nxn_interp_grad_sse4_1;
  if (flags & HAS_SSE4_1) av1_copy_pred_array_highbd = av1_copy_pred_array_highbd_sse4_1;
#endif
  if (flags & HAS_SSE4_1) inv_stxfm = inv_stxfm_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add = av1_highbd_inv_txfm_add_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x4 = av1_highbd_inv_txfm_add_4x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_8x8 = av1_highbd_inv_txfm_add_8x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x8 = av1_highbd_inv_txfm_add_4x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_8x4 = av1_highbd_inv_txfm_add_8x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x16 = av1_highbd_inv_txfm_add_4x16_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_16x4 = av1_highbd_inv_txfm_add_16x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_inv_txfm2d_add_4x4 = av1_inv_txfm2d_add_4x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_inv_txfm2d_add_8x8 = av1_inv_txfm2d_add_8x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_build_compound_diffwtd_mask_d16 = av1_build_compound_diffwtd_mask_d16_sse4_1;
  if (flags & HAS_SSE4_1) av1_round_shift_array = av1_round_shift_array_sse4_1;
#if CONFIG_AV1_ENCODER
  if (flags & HAS_SSE4_1) fwd_stxfm = fwd_stxfm_sse4_1;
  if (flags & HAS_SSE4_1) av1_lowbd_fwd_txfm = av1_lowbd_fwd_txfm_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_4x8 = av1_fwd_txfm2d_4x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_8x4 = av1_fwd_txfm2d_8x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_8x16 = av1_fwd_txfm2d_8x16_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_16x8 = av1_fwd_txfm2d_16x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_16x32 = av1_fwd_txfm2d_16x32_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_32x16 = av1_fwd_txfm2d_32x16_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_4x16 = av1_fwd_txfm2d_4x16_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_16x4 = av1_fwd_txfm2d_16x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_8x32 = av1_fwd_txfm2d_8x32_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_32x8 = av1_fwd_txfm2d_32x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_4x4 = av1_fwd_txfm2d_4x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_8x8 = av1_fwd_txfm2d_8x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_16x16 = av1_fwd_txfm2d_16x16_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_32x32 = av1_fwd_txfm2d_32x32_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_64x64 = av1_fwd_txfm2d_64x64_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_32x64 = av1_fwd_txfm2d_32x64_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_64x32 = av1_fwd_txfm2d_64x32_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_16x64 = av1_fwd_txfm2d_16x64_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_64x16 = av1_fwd_txfm2d_64x16_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_quantize_fp = av1_highbd_quantize_fp_sse4_1;
  if (flags & HAS_SSE4_1) av1_txb_init_levels_skip = av1_txb_init_levels_skip_sse4_1;
  if (flags & HAS_SSE4_1) av1_txb_init_levels_signs = av1_txb_init_levels_signs_sse4_1;
  if (flags & HAS_SSE4_1) av1_txb_init_levels = av1_txb_init_levels_sse4_1;
  if (flags & HAS_SSE4_1) av1_compute_stats_highbd = av1_compute_stats_highbd_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_pixel_proj_error = av1_highbd_pixel_proj_error_sse4_1;
  if (flags & HAS_SSE4_1) av1_get_horver_correlation_full = av1_get_horver_correlation_full_sse4_1;
#endif
#if !defined(__MSVCRT__)
  if (flags & HAS_SSE4_1) cdef_find_dir = cdef_find_dir_sse4_1;
  if (flags & HAS_SSE4_1) cdef_filter_block = cdef_filter_block_sse4_1;
  if (flags & HAS_SSE4_1) cdef_copy_rect8_16bit_to_16bit = cdef_copy_rect8_16bit_to_16bit_sse4_1;
#endif
  if (flags & HAS_SSE4_1) av1_highbd_warp_affine = av1_highbd_warp_affine_sse4_1;
#if CONFIG_AV1_ENCODER
  if (flags & HAS_SSE4_1) av1_compute_cross_correlation = av1_compute_cross_correlation_sse4_1;
#endif
  if (flags & HAS_SSE4_1) av1_apply_selfguided_restoration = av1_apply_selfguided_restoration_sse4_1;
  if (flags & HAS_SSE4_1) av1_selfguided_restoration = av1_selfguided_restoration_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_dist_wtd_convolve_2d = av1_highbd_dist_wtd_convolve_2d_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_dist_wtd_convolve_x = av1_highbd_dist_wtd_convolve_x_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_dist_wtd_convolve_y = av1_highbd_dist_wtd_convolve_y_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_dist_wtd_convolve_2d_copy = av1_highbd_dist_wtd_convolve_2d_copy_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_convolve_2d_scale = av1_highbd_convolve_2d_scale_sse4_1;
  if (flags & HAS_SSE4_1) av1_filter_intra_edge_high = av1_filter_intra_edge_high_sse4_1;
  if (flags & HAS_SSE4_1) av1_upsample_intra_edge_high = av1_upsample_intra_edge_high_sse4_1;
#endif
#if defined(__SSE4_2__)&& HAVE_SIMD
  if (flags & HAS_SSE4_2) av1_get_crc32c_value = av1_get_crc32c_value_sse4_2;
#endif
#if defined(__AVX2__) && HAVE_SIMD

#endif

/*#if CONFIG_AV1_HIGHBITDEPTH
#if HAVE_SIMD

#endif
#if defined(__SSE2__) && HAVE_SIMD

#endif
#if defined(__SSSE3__) && HAVE_SIMD

#endif
#if defined(__SSE4_1__) && HAVE_SIMD

#endif
#if defined(__AVX2__) && HAVE_SIMD

#endif
#endif*/ // CONFIG_AV1_HIGHBITDEPTH

}
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
