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
struct tcq_node_t;
struct tcq_ctx_t;
struct tcq_lf_ctx_t;
struct prequant_t;
struct tcq_rate_t;
struct tcq_coeff_ctx_t;
struct tcq_param_t;
struct LV_MAP_COEFF_COST;

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

#ifdef __cplusplus
extern "C" {
#endif

void av1_highbd_convolve_horiz_rs_c(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const int16_t *x_filters, int x0_qn, int x_step_qn, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_convolve_horiz_rs)(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const int16_t *x_filters, int x0_qn, int x_step_qn, int bd);
void av1_highbd_convolve_horiz_rs_sse4_1(const uint16_t *src, int src_stride, uint16_t *dst, int dst_stride, int w, int h, const int16_t *x_filters, int x0_qn, int x_step_qn, int bd);
#else
#define av1_highbd_convolve_horiz_rs av1_highbd_convolve_horiz_rs_c
#endif

void av1_highbd_wiener_convolve_add_src_c(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const WienerConvolveParams *conv_params, int bd);
#if HAVE_SIMD
#if defined(__SSSE3__)
RTCD_EXTERN void (*av1_highbd_wiener_convolve_add_src)(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const WienerConvolveParams *conv_params, int bd);
void av1_highbd_wiener_convolve_add_src_ssse3(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const WienerConvolveParams *conv_params, int bd);
#endif
#if defined(__AVX2__)
void av1_highbd_wiener_convolve_add_src_avx2(const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, const WienerConvolveParams *conv_params, int bd);
#endif
#else
#define av1_highbd_wiener_convolve_add_src av1_highbd_wiener_convolve_add_src_c
#endif

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

// Non-separable Wiener filter
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
#define av1_convolve_symmetric_subtract_center_highbd av1_convolve_symmetric_subtract_center_highbd_c
#endif

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
void av1_convolve_mixedsymmetric_highbd_c(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_convolve_mixedsymmetric_highbd)(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#if defined(__AVX2__)
void av1_convolve_mixedsymmetric_highbd_avx2(const uint16_t *dgd, int stride, const NonsepFilterConfig *filter_config, const int16_t *filter, uint16_t *dst, int dst_stride, int bit_depth, int block_row_begin, int block_row_end, int block_col_begin, int block_col_end);
#endif
#else
#define av1_convolve_mixedsymmetric_highbd av1_convolve_mixedsymmetric_highbd_c
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
void av1_bicubic_grad_interpolation_highbd_c(const int16_t *pred_src,int16_t *x_grad,int16_t *y_grad,const int blk_width,const int blk_height);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_bicubic_grad_interpolation_highbd)(const int16_t *pred_src,int16_t *x_grad,int16_t *y_grad,const int blk_width,const int blk_height);
#if defined(__SSE4_1__)
void av1_bicubic_grad_interpolation_highbd_sse4_1(const int16_t *pred_src,int16_t *x_grad,int16_t *y_grad,const int blk_width,const int blk_height);
#endif
#if defined(__AVX2__)
void av1_bicubic_grad_interpolation_highbd_avx2(const int16_t *pred_src,int16_t *x_grad,int16_t *y_grad,const int blk_width,const int blk_height);
#endif
#else
#define av1_bicubic_grad_interpolation_highbd av1_bicubic_grad_interpolation_highbd_c
#endif
#if CONFIG_E191_OFS_PRED_RES_HANDLE
int av1_opfl_mv_refinement_nxn_c(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int mi_x, int mi_y, int mi_cols, int mi_rows, int is_decode, int *vx0, int *vy0, int *vx1, int *vy1);
#if HAVE_SIMD
RTCD_EXTERN int (*av1_opfl_mv_refinement_nxn)(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int mi_x, int mi_y, int mi_cols, int mi_rows, int is_decode, int *vx0, int *vy0, int *vx1, int *vy1);
#if defined(__SSE4_1__)
int av1_opfl_mv_refinement_nxn_sse4_1(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int mi_x, int mi_y, int mi_cols, int mi_rows, int is_decode, int *vx0, int *vy0, int *vx1, int *vy1);
#endif
#if defined(__AVX2__)
int av1_opfl_mv_refinement_nxn_avx2(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n, int d0, int d1, int grad_prec_bits, int mv_prec_bits, int mi_x, int mi_y, int mi_cols, int mi_rows, int is_decode, int *vx0, int *vy0, int *vx1, int *vy1);
#endif
#else
#define av1_opfl_mv_refinement_nxn av1_opfl_mv_refinement_nxn_c
#endif
#else
int av1_opfl_mv_refinement_nxn_c(const int16_t *pdiff, int pstride,const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n,int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
#if HAVE_SIMD
RTCD_EXTERN int (*av1_opfl_mv_refinement_nxn)(const int16_t *pdiff, int pstride,const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n,int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
#if defined(__SSE4_1__)
int av1_opfl_mv_refinement_nxn_sse4_1(const int16_t *pdiff, int pstride,const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n,int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
#endif
#if defined(__AVX2__)
int av1_opfl_mv_refinement_nxn_avx2(const int16_t *pdiff, int pstride,const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int n,int d0, int d1, int grad_prec_bits, int mv_prec_bits, int *vx0, int *vy0, int *vx1, int *vy1);
#endif
#else
#define av1_opfl_mv_refinement_nxn av1_opfl_mv_refinement_nxn_c
#endif
#endif



void av1_copy_pred_array_highbd_c(const uint16_t *src1, const uint16_t *src2, int16_t *dst1,int16_t *dst2, int bw, int bh, int d0, int d1, int centered);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_copy_pred_array_highbd)(const uint16_t *src1, const uint16_t *src2, int16_t *dst1,int16_t *dst2, int bw, int bh, int d0, int d1, int centered);
void av1_copy_pred_array_highbd_sse4_1(const uint16_t *src1, const uint16_t *src2, int16_t *dst1,int16_t *dst2, int bw, int bh, int d0, int d1, int centered);
#else
#define av1_copy_pred_array_highbd av1_copy_pred_array_highbd_c
#endif

// High bitdepth functions

// inv txfm
void inv_stxfm_c(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*inv_stxfm)(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
void inv_stxfm_sse4_1(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
#endif
#if defined(__AVX2__)
void inv_stxfm_avx2(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
#endif
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
#if CONFIG_CORE_TX
void inv_txfm_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if HAVE_SIMD
RTCD_EXTERN void (*inv_txfm)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__AVX2__)
void inv_txfm_avx2(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#endif
#else
#define inv_txfm inv_txfm_c
#endif
#endif
#if CONFIG_LOSSLESS_DPCM
void av1_highbd_inv_txfm_add_vert_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_vert av1_highbd_inv_txfm_add_vert_c
void av1_highbd_inv_txfm_add_horz_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_highbd_inv_txfm_add_horz av1_highbd_inv_txfm_add_horz_c
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
void av1_highbd_inv_txfm_add_4x32_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_4x32)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_4x32_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_4x32 av1_highbd_inv_txfm_add_4x32_c
#endif
void av1_highbd_inv_txfm_add_32x4_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_32x4)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_32x4_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_32x4 av1_highbd_inv_txfm_add_32x4_c
#endif
void av1_highbd_inv_txfm_add_64x4_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_64x4)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_64x4_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_64x4 av1_highbd_inv_txfm_add_64x4_c
#endif
void av1_highbd_inv_txfm_add_4x64_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_highbd_inv_txfm_add_4x64)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
void av1_highbd_inv_txfm_add_4x64_sse4_1(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#else
#define av1_highbd_inv_txfm_add_4x64 av1_highbd_inv_txfm_add_4x64_c
#endif

#if CONFIG_INTER_DDT
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

#if CONFIG_LOSSLESS_DPCM
void av1_highbd_iwht4x4_1_vert_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_1_vert_add av1_highbd_iwht4x4_1_vert_add_c
void av1_highbd_iwht4x4_16_vert_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_16_vert_add av1_highbd_iwht4x4_16_vert_add_c
void av1_highbd_iwht4x4_1_horz_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_1_horz_add av1_highbd_iwht4x4_1_horz_add_c
void av1_highbd_iwht4x4_16_horz_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_16_horz_add av1_highbd_iwht4x4_16_horz_add_c
#endif

void av1_inv_txfm2d_add_4x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_4x8 av1_inv_txfm2d_add_4x8_c
void av1_inv_txfm2d_add_8x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_8x4 av1_inv_txfm2d_add_8x4_c
void av1_inv_txfm2d_add_8x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_8x16 av1_inv_txfm2d_add_8x16_c
void av1_inv_txfm2d_add_16x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_16x8 av1_inv_txfm2d_add_16x8_c
void av1_inv_txfm2d_add_16x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_16x32 av1_inv_txfm2d_add_16x32_c
void av1_inv_txfm2d_add_32x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_32x16 av1_inv_txfm2d_add_32x16_c
void av1_inv_txfm2d_add_4x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_inv_txfm2d_add_4x4)(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
void av1_inv_txfm2d_add_4x4_sse4_1(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_inv_txfm2d_add_4x4 av1_inv_txfm2d_add_4x4_c
#endif
void av1_inv_txfm2d_add_8x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
RTCD_EXTERN void (*av1_inv_txfm2d_add_8x8)(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
void av1_inv_txfm2d_add_8x8_sse4_1(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_inv_txfm2d_add_8x8 av1_inv_txfm2d_add_8x8_c
#endif
void av1_inv_txfm2d_add_16x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_16x16 av1_inv_txfm2d_add_16x16_c
void av1_inv_txfm2d_add_32x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_32x32 av1_inv_txfm2d_add_32x32_c

void av1_inv_txfm2d_add_64x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_64x64 av1_inv_txfm2d_add_64x64_c
void av1_inv_txfm2d_add_32x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_32x64 av1_inv_txfm2d_add_32x64_c
void av1_inv_txfm2d_add_64x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_64x32 av1_inv_txfm2d_add_64x32_c
void av1_inv_txfm2d_add_16x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_16x64 av1_inv_txfm2d_add_16x64_c
void av1_inv_txfm2d_add_64x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_64x16 av1_inv_txfm2d_add_64x16_c

void av1_inv_txfm2d_add_4x16_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_4x16 av1_inv_txfm2d_add_4x16_c
void av1_inv_txfm2d_add_16x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_16x4 av1_inv_txfm2d_add_16x4_c
void av1_inv_txfm2d_add_8x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_8x32 av1_inv_txfm2d_add_8x32_c
void av1_inv_txfm2d_add_32x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_32x8 av1_inv_txfm2d_add_32x8_c

void av1_inv_txfm2d_add_4x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_4x32 av1_inv_txfm2d_add_4x32_c
void av1_inv_txfm2d_add_32x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_32x4 av1_inv_txfm2d_add_32x4_c
void av1_inv_txfm2d_add_8x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_8x64 av1_inv_txfm2d_add_8x64_c
void av1_inv_txfm2d_add_64x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_64x8 av1_inv_txfm2d_add_64x8_c
void av1_inv_txfm2d_add_4x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_4x64 av1_inv_txfm2d_add_4x64_c
void av1_inv_txfm2d_add_64x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#define av1_inv_txfm2d_add_64x4 av1_inv_txfm2d_add_64x4_c

#else

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

#if CONFIG_LOSSLESS_DPCM
void av1_highbd_iwht4x4_1_vert_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_1_vert_add av1_highbd_iwht4x4_1_vert_add_c
void av1_highbd_iwht4x4_16_vert_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_16_vert_add av1_highbd_iwht4x4_16_vert_add_c
void av1_highbd_iwht4x4_1_horz_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_1_horz_add av1_highbd_iwht4x4_1_horz_add_c
void av1_highbd_iwht4x4_16_horz_add_c(const tran_low_t *input, uint16_t *dest, int dest_stride, int bd);
#define av1_highbd_iwht4x4_16_horz_add av1_highbd_iwht4x4_16_horz_add_c
#endif

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

void av1_inv_txfm2d_add_4x32_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_4x32 av1_inv_txfm2d_add_4x32_c
void av1_inv_txfm2d_add_32x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_32x4 av1_inv_txfm2d_add_32x4_c
void av1_inv_txfm2d_add_8x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_8x64 av1_inv_txfm2d_add_8x64_c
void av1_inv_txfm2d_add_64x8_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_64x8 av1_inv_txfm2d_add_64x8_c
void av1_inv_txfm2d_add_4x64_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_4x64 av1_inv_txfm2d_add_4x64_c
void av1_inv_txfm2d_add_64x4_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_txfm2d_add_64x4 av1_inv_txfm2d_add_64x4_c
#endif //CONFIG_INTER_DDT

#if CONFIG_LOSSLESS_DPCM
void av1_inv_idfm2d_add_4x4_vert_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_idfm2d_add_4x4_vert av1_inv_idfm2d_add_4x4_vert_c
void av1_inv_idfm2d_add_4x4_horz_c(const int32_t *input, uint16_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_inv_idfm2d_add_4x4_horz av1_inv_idfm2d_add_4x4_horz_c
#endif

#if CONFIG_IMPROVE_LOSSLESS_TXM
//#if CONFIG_AV1_ENCODER
void av1_lossless_fwd_idtx_c(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_lossless_fwd_idtx)(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#if defined(__AVX2__)
void av1_lossless_fwd_idtx_avx2(const int16_t *src_diff, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#endif
#else
#define av1_lossless_fwd_idtx av1_lossless_fwd_idtx_c
#endif
//#endif
void av1_lossless_inv_idtx_add_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_lossless_inv_idtx_add)(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#if defined(__AVX2__)
void av1_lossless_inv_idtx_add_avx2(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#endif
#else
#define av1_lossless_inv_idtx_add av1_lossless_inv_idtx_add_c
#endif
#if CONFIG_LOSSLESS_DPCM
void av1_lossless_inv_idtx_add_vert_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_lossless_inv_idtx_add_vert av1_lossless_inv_idtx_add_vert_c
void av1_lossless_inv_idtx_add_horz_c(const tran_low_t *input, uint16_t *dest, int stride, const TxfmParam *txfm_param);
#define av1_lossless_inv_idtx_add_horz av1_lossless_inv_idtx_add_horz_c
#endif
#endif

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

#if CONFIG_IBP_WEIGHT
void av1_highbd_ibp_dr_prediction_z1_c(const IbpWeightsType weights[][IBP_WEIGHT_SIZE][DIR_MODES_0_90], int mode_idx, uint16_t *dst, ptrdiff_t stride, uint16_t* second_pred, ptrdiff_t second_stride, int bw, int bh);
#define av1_highbd_ibp_dr_prediction_z1 av1_highbd_ibp_dr_prediction_z1_c
void av1_highbd_ibp_dr_prediction_z3_c(const IbpWeightsType weights[][IBP_WEIGHT_SIZE][DIR_MODES_0_90], int mode_idx, uint16_t *dst, ptrdiff_t stride, uint16_t* second_pred, ptrdiff_t second_stride, int bw, int bh);
#define av1_highbd_ibp_dr_prediction_z3 av1_highbd_ibp_dr_prediction_z3_c
#else
void av1_highbd_ibp_dr_prediction_z1_c(uint8_t* weights, uint16_t *dst, ptrdiff_t stride, uint16_t* second_pred, ptrdiff_t second_stride, int bw, int bh);
#define av1_highbd_ibp_dr_prediction_z1 av1_highbd_ibp_dr_prediction_z1_c
void av1_highbd_ibp_dr_prediction_z3_c(uint8_t* weights, uint16_t *dst, ptrdiff_t stride, uint16_t* second_pred, ptrdiff_t second_stride, int bw, int bh);
#define av1_highbd_ibp_dr_prediction_z3 av1_highbd_ibp_dr_prediction_z3_c
#endif

// Data-driven intra prediction (DIP)
#if CONFIG_DIP
void av1_dip_matrix_multiplication_c(const uint16_t *A, const uint16_t *B, uint16_t *C, int bd);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_dip_matrix_multiplication)(const uint16_t *A, const uint16_t *B, uint16_t *C, int bd);
#if defined(__AVX2__)
void av1_dip_matrix_multiplication_avx2(const uint16_t *A, const uint16_t *B, uint16_t *C, int bd);
#endif
#else
#define av1_dip_matrix_multiplication av1_dip_matrix_multiplication_c
#endif
#endif

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
#if CONFIG_AFFINE_REFINEMENT
#if CONFIG_AFFINE_REFINEMENT_SB
void av1_calc_affine_autocorrelation_matrix_c(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int x_offset, int y_offset, int32_t *mat_a, int32_t *vec_b);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_calc_affine_autocorrelation_matrix)(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int x_offset, int y_offset, int32_t *mat_a, int32_t *vec_b);
#if defined(__AVX2__)
void av1_calc_affine_autocorrelation_matrix_avx2(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int x_offset, int y_offset, int32_t *mat_a, int32_t *vec_b);
#endif
#else
#define av1_calc_affine_autocorrelation_matrix av1_calc_affine_autocorrelation_matrix_c
#endif
#else
void av1_calc_affine_autocorrelation_matrix_c(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int32_t *mat_a, int32_t *vec_b);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_calc_affine_autocorrelation_matrix)(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int32_t *mat_a, int32_t *vec_b);
#if defined(__AVX2__)
void av1_calc_affine_autocorrelation_matrix_avx2(const int16_t *pdiff, int pstride, const int16_t *gx, const int16_t *gy, int gstride, int bw, int bh, int32_t *mat_a, int32_t *vec_b);
#endif
#else
#define av1_calc_affine_autocorrelation_matrix av1_calc_affine_autocorrelation_matrix_c
#endif
#endif
#endif

#if CONFIG_OPFL_MV_SEARCH || CONFIG_AFFINE_REFINEMENT
void av1_avg_pooling_pdiff_gradients_c(int16_t *pdiff, const int pstride, int16_t *gx, int16_t *gy, const int gstride, const int bw, const int bh, const int n);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_avg_pooling_pdiff_gradients)(int16_t *pdiff, const int pstride, int16_t *gx, int16_t *gy, const int gstride, const int bw, const int bh, const int n);
#if defined(__AVX2__)
void av1_avg_pooling_pdiff_gradients_avx2(int16_t *pdiff, const int pstride, int16_t *gx, int16_t *gy, const int gstride, const int bw, const int bh, const int n);
#endif
#else
#define av1_avg_pooling_pdiff_gradients av1_avg_pooling_pdiff_gradients_c
#endif
#endif


// Block Adaptive Weighted Prediction
void av1_make_bawp_block_c(uint16_t *dst, int dst_stride, int16_t alpha, int32_t beta, int shift, int bw, int bh, int bd);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_make_bawp_block)(uint16_t *dst, int dst_stride, int16_t alpha, int32_t beta, int shift, int bw, int bh, int bd);
#if defined(__AVX2__)
void av1_make_bawp_block_avx2(uint16_t *dst, int dst_stride, int16_t alpha, int32_t beta, int shift, int bw, int bh, int bd);
#endif
#else
#define av1_make_bawp_block av1_make_bawp_block_c
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
//#if CONFIG_AV1_ENCODER

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

  // trellis quant
#if CONFIG_TCQ
  void av1_decide_states_c(const struct tcq_node_t *prev, const struct tcq_rate_t *rd, const struct prequant_t *pq, int limits, int tru_eob, int64_t rdmult, struct tcq_node_t *decision);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_decide_states)(const struct tcq_node_t *prev, const struct tcq_rate_t *rd, const struct prequant_t *pq, int limits, int tru_eob, int64_t rdmult, struct tcq_node_t *decision);
#if defined(__AVX2__)
  void av1_decide_states_avx2(const struct tcq_node_t *prev, const struct tcq_rate_t *rd, const struct prequant_t *pq, int limits, int tru_eob, int64_t rdmult, struct tcq_node_t *decision);
#endif
#else
  #define av1_decide_states av1_decide_states_c
#endif
  void av1_pre_quant_c(tran_low_t tqc, struct prequant_t* pqData, const int32_t* quant_ptr, int dqv, int log_scale, int scan_pos);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_pre_quant)(tran_low_t tqc, struct prequant_t* pqData, const int32_t* quant_ptr, int dqv, int log_scale, int scan_pos);
#if defined(__AVX2__)
  void av1_pre_quant_avx2(tran_low_t tqc, struct prequant_t* pqData, const int32_t* quant_ptr, int dqv, int log_scale, int scan_pos);
#endif
#else
  #define av1_pre_quant av1_pre_quant_c
#endif

  void av1_get_rate_dist_def_luma_c(const struct tcq_param_t *p, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, struct tcq_rate_t *rd);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_get_rate_dist_def_luma)(const struct tcq_param_t *p, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, struct tcq_rate_t *rd);
#if defined(__AVX2__)
  void av1_get_rate_dist_def_luma_avx2(const struct tcq_param_t *p, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, struct tcq_rate_t *rd);
#endif
#else
  #define av1_get_rate_dist_def_luma av1_get_rate_dist_def_luma_c
#endif
  void av1_get_rate_dist_def_chroma_c(const struct LV_MAP_COEFF_COST* txb_costs, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int bwl, TX_CLASS tx_class, int diag_ctx, int eob_rate, int plane, int t_sign, int sign, struct tcq_rate_t *rd);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_get_rate_dist_def_chroma)(const struct LV_MAP_COEFF_COST* txb_costs, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int bwl, TX_CLASS tx_class, int diag_ctx, int eob_rate, int plane, int t_sign, int sign, struct tcq_rate_t *rd);
#if defined(__AVX2__)
  void av1_get_rate_dist_def_chroma_avx2(const struct LV_MAP_COEFF_COST* txb_costs, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int bwl, TX_CLASS tx_class, int diag_ctx, int eob_rate, int plane, int t_sign, int sign, struct tcq_rate_t *rd);
#endif
#else
  #define av1_get_rate_dist_def_chroma av1_get_rate_dist_def_chroma_c
#endif
  void av1_get_rate_dist_lf_luma_c(const struct tcq_param_t *p, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, int coeff_sign, struct tcq_rate_t *rd);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_get_rate_dist_lf_luma)(const struct tcq_param_t *p, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, int coeff_sign, struct tcq_rate_t *rd);
#if defined(__AVX2__)
  void av1_get_rate_dist_lf_luma_avx2(const struct tcq_param_t *p, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, int coeff_sign, struct tcq_rate_t *rd);
#endif
#else
  #define av1_get_rate_dist_lf_luma av1_get_rate_dist_lf_luma_c
#endif
  void av1_get_rate_dist_lf_chroma_c(const struct LV_MAP_COEFF_COST *txb_costs, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, int dc_sign_ctx, const int32_t *tmp_sign, int bwl, TX_CLASS tx_class, int plane, int coeff_sign, struct tcq_rate_t *rd);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_get_rate_dist_lf_chroma)(const struct LV_MAP_COEFF_COST *txb_costs, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, int dc_sign_ctx, const int32_t *tmp_sign, int bwl, TX_CLASS tx_class, int plane, int coeff_sign, struct tcq_rate_t *rd);
#if defined(__AVX2__)
  void av1_get_rate_dist_lf_chroma_avx2(const struct LV_MAP_COEFF_COST *txb_costs, const struct prequant_t *pq, const struct tcq_coeff_ctx_t *coeff_ctx, int blk_pos, int diag_ctx, int eob_rate, int dc_sign_ctx, const int32_t *tmp_sign, int bwl, TX_CLASS tx_class, int plane, int coeff_sign, struct tcq_rate_t *rd);
#endif
#else
  #define av1_get_rate_dist_lf_chroma av1_get_rate_dist_lf_chroma_c
#endif

  void av1_update_states_c(const struct tcq_node_t *decision, int col, struct tcq_ctx_t *tcq_ctx);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_update_states)(const struct tcq_node_t *decision, int col, struct tcq_ctx_t *tcq_ctx);
#if defined(__AVX2__)
  void av1_update_states_avx2(const struct tcq_node_t *decision, int col, struct tcq_ctx_t *tcq_ctx);
#endif
#else
  #define av1_update_states av1_update_states_c
#endif
  void av1_calc_block_eob_rate_c(struct macroblock *x, int plane, TX_SIZE tx_size, int eob, uint16_t *block_eob_rate);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_calc_block_eob_rate)(struct macroblock *x, int plane, TX_SIZE tx_size, int eob, uint16_t *block_eob_rate);
#if defined(__AVX2__)
  void av1_calc_block_eob_rate_avx2(struct macroblock *x, int plane, TX_SIZE tx_size, int eob, uint16_t *block_eob_rate);
#endif
#else
  #define av1_calc_block_eob_rate av1_calc_block_eob_rate_c
#endif
  int av1_find_best_path_c(const struct tcq_node_t *trellis, const int16_t *scan, const int32_t *dequant, const qm_val_t *iqmatrix, const tran_low_t *tcoeff, int first_scan_pos, int log_scale, tran_low_t *qcoeff, tran_low_t *dqcoeff, int *min_rate, int64_t *min_cost);
#if HAVE_SIMD
  RTCD_EXTERN int (*av1_find_best_path)(const struct tcq_node_t *trellis, const int16_t *scan, const int32_t *dequant, const qm_val_t *iqmatrix, const tran_low_t *tcoeff, int first_scan_pos, int log_scale, tran_low_t *qcoeff, tran_low_t *dqcoeff, int *min_rate, int64_t *min_cost);
#if defined(__AVX2__)
  int av1_find_best_path_avx2(const struct tcq_node_t *trellis, const int16_t *scan, const int32_t *dequant, const qm_val_t *iqmatrix, const tran_low_t *tcoeff, int first_scan_pos, int log_scale, tran_low_t *qcoeff, tran_low_t *dqcoeff, int *min_rate, int64_t *min_cost);
#endif
#else
  #define av1_find_best_path av1_find_best_path_c
#endif
  void av1_get_coeff_ctx_c(const struct tcq_ctx_t *tcq_ctx, int col, struct tcq_coeff_ctx_t *coeff_ctx);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_get_coeff_ctx)(const struct tcq_ctx_t *tcq_ctx, int col, struct tcq_coeff_ctx_t *coeff_ctx);
#if defined(__AVX2__)
  void av1_get_coeff_ctx_avx2(const struct tcq_ctx_t *tcq_ctx, int col, struct tcq_coeff_ctx_t *coeff_ctx);
#endif
#else
  #define av1_get_coeff_ctx av1_get_coeff_ctx_c
#endif
  void av1_update_nbr_diagonal_c(struct tcq_ctx_t *tcq_ctx, int row, int col, int bwl);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_update_nbr_diagonal)(struct tcq_ctx_t *tcq_ctx, int row, int col, int bwl);
#if defined(__AVX2__)
  void av1_update_nbr_diagonal_avx2(struct tcq_ctx_t *tcq_ctx, int row, int col, int bwl);
#endif
#else
  #define av1_update_nbr_diagonal av1_update_nbr_diagonal_c
#endif
#endif

  // fdct functions
  void av1_fwht4x4_c(const int16_t *input, tran_low_t *output, int stride);
  #define av1_fwht4x4 av1_fwht4x4_c

  // fwd cctx
  void av1_fwd_cross_chroma_tx_block_c(tran_low_t *coeff_c1, tran_low_t *coeff_c2, TX_SIZE tx_size, CctxType cctx_type, const int bd);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_cross_chroma_tx_block)(tran_low_t *coeff_c1, tran_low_t *coeff_c2, TX_SIZE tx_size, CctxType cctx_type, const int bd);
#if defined(__AVX2__)
  void av1_fwd_cross_chroma_tx_block_avx2(tran_low_t *coeff_c1, tran_low_t *coeff_c2, TX_SIZE tx_size, CctxType cctx_type, const int bd);
#endif
#else
  #define av1_fwd_cross_chroma_tx_block av1_fwd_cross_chroma_tx_block_c
#endif

  // fwd txfm
  void fwd_stxfm_c(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*fwd_stxfm)(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
  void fwd_stxfm_sse4_1(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
#endif
#if defined(__AVX2__)
  void fwd_stxfm_avx2(tran_low_t *src, tran_low_t *dst, const PREDICTION_MODE mode, const uint8_t stx_idx, const int size, const int bd);
#endif
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

#if CONFIG_INTER_DDT
  void av1_fwd_txfm2d_4x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_4x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_4x8 av1_fwd_txfm2d_4x8_c
#endif
  void av1_fwd_txfm2d_8x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_8x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_8x4 av1_fwd_txfm2d_8x4_c
#endif
  void av1_fwd_txfm2d_8x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_8x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_8x16_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#else
  #define av1_fwd_txfm2d_8x16 av1_fwd_txfm2d_8x16_c
#endif
  void av1_fwd_txfm2d_16x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_16x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_16x8_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#else
  #define av1_fwd_txfm2d_16x8 av1_fwd_txfm2d_16x8_c
#endif
  void av1_fwd_txfm2d_16x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_16x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_16x32 av1_fwd_txfm2d_16x32_c
#endif
  void av1_fwd_txfm2d_32x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_32x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_32x16 av1_fwd_txfm2d_32x16_c
#endif
  void av1_fwd_txfm2d_4x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_4x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_4x16 av1_fwd_txfm2d_4x16_c
#endif
  void av1_fwd_txfm2d_16x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_16x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_16x4 av1_fwd_txfm2d_16x4_c
#endif
  void av1_fwd_txfm2d_8x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_8x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_8x32 av1_fwd_txfm2d_8x32_c
#endif
  void av1_fwd_txfm2d_32x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_32x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_32x8 av1_fwd_txfm2d_32x8_c
#endif
  void av1_fwd_txfm2d_4x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if defined(__SSE4_1__) && HAVE_SIMD
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_4x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_4x4 av1_fwd_txfm2d_4x4_c
#endif
  void av1_fwd_txfm2d_8x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_8x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_8x8_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#else
  #define av1_fwd_txfm2d_8x8 av1_fwd_txfm2d_8x8_c
#endif
  void av1_fwd_txfm2d_16x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_16x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_16x16_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#else
  #define av1_fwd_txfm2d_16x16 av1_fwd_txfm2d_16x16_c
#endif
  void av1_fwd_txfm2d_32x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_32x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_32x32_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#else
  #define av1_fwd_txfm2d_32x32 av1_fwd_txfm2d_32x32_c
#endif
#if CONFIG_CORE_TX
void fwd_txfm_c(const int16_t *resi, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#if HAVE_SIMD
RTCD_EXTERN void (*fwd_txfm)(const int16_t *resi, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#if defined(__AVX2__)
void fwd_txfm_avx2(const int16_t *resi, tran_low_t *coeff, int diff_stride, TxfmParam *txfm_param);
#endif
#else
#define fwd_txfm fwd_txfm_c
#endif
#endif

#else

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
#endif //CONFIG_INTER_DDT

#if CONFIG_INTER_DDT
  void av1_fwd_txfm2d_64x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD
#if defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_64x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#if defined(__AVX2__)
  void av1_fwd_txfm2d_64x64_avx2(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#endif
#else
  #define av1_fwd_txfm2d_64x64 av1_fwd_txfm2d_64x64_c
#endif
  void av1_fwd_txfm2d_32x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_32x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_32x64 av1_fwd_txfm2d_32x64_c
#endif
  void av1_fwd_txfm2d_64x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_64x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_64x32 av1_fwd_txfm2d_64x32_c
#endif
  void av1_fwd_txfm2d_16x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_16x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_16x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_16x64 av1_fwd_txfm2d_16x64_c
#endif
  void av1_fwd_txfm2d_64x16_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x16)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_64x16_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
  #define av1_fwd_txfm2d_64x16 av1_fwd_txfm2d_64x16_c
#endif
void av1_fwd_txfm2d_4x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x32)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_4x32_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_fwd_txfm2d_4x32 av1_fwd_txfm2d_4x32_c
#endif
void av1_fwd_txfm2d_32x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_32x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_32x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_fwd_txfm2d_32x4 av1_fwd_txfm2d_32x4_c
#endif
void av1_fwd_txfm2d_8x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_8x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_8x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_fwd_txfm2d_8x64 av1_fwd_txfm2d_8x64_c
#endif
void av1_fwd_txfm2d_64x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x8)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_64x8_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_fwd_txfm2d_64x8 av1_fwd_txfm2d_64x8_c
#endif
void av1_fwd_txfm2d_4x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_4x64)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_4x64_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_fwd_txfm2d_4x64 av1_fwd_txfm2d_4x64_c
#endif
void av1_fwd_txfm2d_64x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#if HAVE_SIMD && defined(__SSE4_1__)
  RTCD_EXTERN void (*av1_fwd_txfm2d_64x4)(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
  void av1_fwd_txfm2d_64x4_sse4_1(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int use_ddt, int bd);
#else
#define av1_fwd_txfm2d_64x4 av1_fwd_txfm2d_64x4_c
#endif

#else

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
void av1_fwd_txfm2d_4x32_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_fwd_txfm2d_4x32 av1_fwd_txfm2d_4x32_c
void av1_fwd_txfm2d_32x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_fwd_txfm2d_32x4 av1_fwd_txfm2d_32x4_c
void av1_fwd_txfm2d_8x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_fwd_txfm2d_8x64 av1_fwd_txfm2d_8x64_c
void av1_fwd_txfm2d_64x8_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_fwd_txfm2d_64x8 av1_fwd_txfm2d_64x8_c
void av1_fwd_txfm2d_4x64_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_fwd_txfm2d_4x64 av1_fwd_txfm2d_4x64_c
void av1_fwd_txfm2d_64x4_c(const int16_t *input, int32_t *output, int stride, TX_TYPE tx_type, int bd);
#define av1_fwd_txfm2d_64x4 av1_fwd_txfm2d_64x4_c
#endif //CONFIG_INTER_DDT

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
  void av1_get_nz_map_contexts_skip_c(const uint8_t *const levels, const int16_t *const scan, const uint16_t bob, const uint16_t eob, const TX_SIZE tx_size, int8_t *const coeff_contexts);
  #define av1_get_nz_map_contexts_skip av1_get_nz_map_contexts_skip_c

  void av1_get_nz_map_contexts_c(const uint8_t *const levels, const int16_t *const scan, const uint16_t eob, const TX_SIZE tx_size, const TX_CLASS tx_class, int8_t *const coeff_contexts, const int plane);
#if HAVE_SIMD
  void av1_get_nz_map_contexts_sse2(const uint8_t *const levels, const int16_t *const scan, const uint16_t eob, const TX_SIZE tx_size, const TX_CLASS tx_class, int8_t *const coeff_contexts, const int plane);
  #define av1_get_nz_map_contexts av1_get_nz_map_contexts_sse2
#else
  #define av1_get_nz_map_contexts av1_get_nz_map_contexts_c
#endif

  void av1_txb_init_levels_signs_c(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels, int8_t *const signs);
#if HAVE_SIMD
  RTCD_EXTERN void (*av1_txb_init_levels_signs)(const tran_low_t *const coeff, const int width, const int height, uint8_t *const levels, int8_t *const signs);
#if defined(__SSE4_1__)
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
#if HAVE_SIMD && !CONFIG_EXCLUDE_SIMD_MISMATCH && defined(__SSE3__)
  RTCD_EXTERN void (*av1_nn_predict)(const float *input_nodes, const NN_CONFIG *const nn_config, int reduce_prec, float *const output);
  void av1_nn_predict_sse3(const float *input_nodes, const NN_CONFIG *const nn_config, int reduce_prec, float *const output);
#else
  #define av1_nn_predict av1_nn_predict_c
#endif
//#endif
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
#if HAVE_SIMD //&& !defined(__MSVCRT__)
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
void cdef_filter_block_c(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, BLOCK_SIZE bsize, int coeff_shift);
#if HAVE_SIMD //&& !defined(__MSVCRT__)
  RTCD_EXTERN void (*cdef_filter_block)(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, BLOCK_SIZE bsize, int coeff_shift);
  void cdef_filter_block_sse2(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, BLOCK_SIZE bsize, int coeff_shift);
#if defined(__SSSE3__)
  void cdef_filter_block_ssse3(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, BLOCK_SIZE bsize, int coeff_shift);
#endif
#if defined(__SSE4_1__)
  void cdef_filter_block_sse4_1(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, BLOCK_SIZE bsize, int coeff_shift);
#endif
#if defined(__AVX2__)
  void cdef_filter_block_avx2(uint8_t *dst8, uint16_t *dst16, int dstride, const uint16_t *in, int pri_strength, int sec_strength, int dir, int pri_damping, int sec_damping, BLOCK_SIZE bsize, int coeff_shift);
#endif
#else
  #define cdef_filter_block cdef_filter_block_c
#endif

void cdef_copy_rect8_16bit_to_16bit_c(uint16_t *dst, int dstride, const uint16_t *src, int sstride, int v, int h);
#if HAVE_SIMD //&& !defined(__MSVCRT__)
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

#if CONFIG_GDF
void gdf_set_lap_and_cls_unit_c(const int i_min, const int i_max, const int j_min, const int j_max, const int stripe_size, const uint16_t *rec_pnt, const int rec_stride, const int bit_depth, uint16_t *const *gdf_lap_y, const int gdf_lap_y_stride, uint32_t *gdf_cls_y, const int gdf_cls_y_stride);
#if HAVE_SIMD
  RTCD_EXTERN void (*gdf_set_lap_and_cls_unit)(const int i_min, const int i_max, const int j_min, const int j_max, const int stripe_size, const uint16_t *rec_pnt, const int rec_stride, const int bit_depth, uint16_t *const *gdf_lap_y, const int gdf_lap_y_stride, uint32_t *gdf_cls_y, const int gdf_cls_y_stride);
#if defined(__AVX2__)
  void gdf_set_lap_and_cls_unit_avx2(const int i_min, const int i_max, const int j_min, const int j_max, const int stripe_size, const uint16_t *rec_pnt, const int rec_stride, const int bit_depth, uint16_t *const *gdf_lap_y, const int gdf_lap_y_stride, uint32_t *gdf_cls_y, const int gdf_cls_y_stride);
#endif
#else
  #define gdf_set_lap_and_cls_unit gdf_set_lap_and_cls_unit_c
#endif
void gdf_inference_unit_c(const int i_min, const int i_max, const int j_min, const int j_max, const int stripe_size, const int qp_idx, const uint16_t* rec_pnt, const int rec_stride, uint16_t *const *gdf_lap_pnt, const int gdf_lap_stride, const uint32_t *gdf_cls_pnt, const int gdf_cls_stride, int16_t* err_pnt, const int err_stride, const int pxl_shift, const int ref_dst_idx);
#if HAVE_SIMD
  RTCD_EXTERN void (*gdf_inference_unit)(const int i_min, const int i_max, const int j_min, const int j_max, const int stripe_size, const int qp_idx, const uint16_t* rec_pnt, const int rec_stride, uint16_t *const *gdf_lap_pnt, const int gdf_lap_stride, const uint32_t *gdf_cls_pnt, const int gdf_cls_stride, int16_t* err_pnt, const int err_stride, const int pxl_shift, const int ref_dst_idx);
#if defined(__AVX2__)
  void gdf_inference_unit_avx2(const int i_min, const int i_max, const int j_min, const int j_max, const int stripe_size, const int qp_idx, const uint16_t* rec_pnt, const int rec_stride, uint16_t *const *gdf_lap_pnt, const int gdf_lap_stride, const uint32_t *gdf_cls_pnt, const int gdf_cls_stride, int16_t* err_pnt, const int err_stride, const int pxl_shift, const int ref_dst_idx);
#endif
#else
  #define gdf_inference_unit gdf_inference_unit_c
#endif
void gdf_compensation_unit_c(uint16_t* rec_pnt, const int rec_stride, int16_t* err_pnt, const int err_stride, const int err_shift, const int scale, const int pxl_max, const int blk_height, const int blk_width);
#if HAVE_SIMD
  RTCD_EXTERN void (*gdf_compensation_unit)(uint16_t* rec_pnt, const int rec_stride, int16_t* err_pnt, const int err_stride, const int err_shift, const int scale, const int pxl_max, const int blk_height, const int blk_width);
#if defined(__AVX2__)
  void gdf_compensation_unit_avx2(uint16_t* rec_pnt, const int rec_stride, int16_t* err_pnt, const int err_stride, const int err_shift, const int scale, const int pxl_max, const int blk_height, const int blk_width);
#endif
#else
  #define gdf_compensation_unit gdf_compensation_unit_c
#endif
#endif // CONFIG_GDF

// Cross-component Sample Offset
#if CONFIG_CCSO_FU_BUGFIX
void ccso_filter_block_hbd_wo_buf_c(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size_x, const int blk_size_y, const bool isSingleBand, const uint8_t shift_bits, const int edge_clf, const uint8_t ccso_bo_only);
#if HAVE_SIMD
  RTCD_EXTERN void (*ccso_filter_block_hbd_wo_buf)(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size_x, const int blk_size_y, const bool isSingleBand, const uint8_t shift_bits, const int edge_clf, const uint8_t ccso_bo_only);
#if defined(__AVX2__)
  void ccso_filter_block_hbd_wo_buf_avx2(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size_x, const int blk_size_y, const bool isSingleBand, const uint8_t shift_bits, const int edge_clf, const uint8_t ccso_bo_only);
#endif
#else
  #define ccso_filter_block_hbd_wo_buf ccso_filter_block_hbd_wo_buf_c
#endif
#else
void ccso_filter_block_hbd_wo_buf_c(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size, const bool isSingleBand, const uint8_t shift_bits, const int edge_clf, const uint8_t ccso_bo_only);
#if HAVE_SIMD
  RTCD_EXTERN void (*ccso_filter_block_hbd_wo_buf)(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size, const bool isSingleBand, const uint8_t shift_bits, const int edge_clf, const uint8_t ccso_bo_only);
#if defined(__AVX2__)
  void ccso_filter_block_hbd_wo_buf_avx2(const uint16_t *src_y, uint16_t *dst_yuv, const int x, const int y, const int pic_width, const int pic_height, int *src_cls, const int8_t *offset_buf, const int scaled_ext_stride, const int dst_stride, const int y_uv_hscale, const int y_uv_vscale, const int thr, const int neg_thr, const int *src_loc, const int max_val, const int blk_size, const bool isSingleBand, const uint8_t shift_bits, const int edge_clf, const uint8_t ccso_bo_only);
#endif
#else
  #define ccso_filter_block_hbd_wo_buf ccso_filter_block_hbd_wo_buf_c
#endif
#endif //CONFIG_CCSO_FU_BUGFIX

//#if CONFIG_AV1_ENCODER
#if CONFIG_CCSO_FU_BUGFIX
    void ccso_filter_block_hbd_with_buf_c(const uint16_t *src_y,
                      uint16_t *dst_yuv,
                      const uint8_t *src_cls0,
                      const uint8_t *src_cls1,
                      const int src_y_stride, const int dst_stride,
                      const int ccso_stride,
                      const int x, const int y,
                      const int pic_width, const int pic_height,
                      const int8_t *filter_offset, const int blk_size_x,
                      const int blk_size_y,
                      const int y_uv_hscale,  const int y_uv_vscale,
                      const int max_val, const uint8_t shift_bits,
                      const uint8_t ccso_bo_only);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_filter_block_hbd_with_buf)(const uint16_t *src_y,
                      uint16_t *dst_yuv,
                      const uint8_t *src_cls0,
                      const uint8_t *src_cls1,
                      const int src_y_stride, const int dst_stride,
                      const int ccso_stride,
                      const int x, const int y,
                      const int pic_width, const int pic_height,
                      const int8_t *filter_offset, const int blk_size_x,
                      const int blk_size_y,
                      const int y_uv_hscale,  const int y_uv_vscale,
                      const int max_val, const uint8_t shift_bits,
                      const uint8_t ccso_bo_only);
#if defined(__AVX2__)
    void ccso_filter_block_hbd_with_buf_avx2(const uint16_t *src_y,
                      uint16_t *dst_yuv,
                      const uint8_t *src_cls0,
                      const uint8_t *src_cls1,
                      const int src_y_stride, const int dst_stride,
                      const int ccso_stride,
                      const int x, const int y,
                      const int pic_width, const int pic_height,
                      const int8_t *filter_offset, const int blk_size_x,
                      const int blk_size_y,
                      const int y_uv_hscale,  const int y_uv_vscale,
                      const int max_val, const uint8_t shift_bits,
                      const uint8_t ccso_bo_only);
#endif
#else
    #define ccso_filter_block_hbd_with_buf ccso_filter_block_hbd_with_buf_c
#endif
    void ccso_filter_block_hbd_with_buf_bo_only_c(const uint16_t *src_y,
                      uint16_t *dst_yuv,
                      const uint8_t *src_cls0,
                      const uint8_t *src_cls1,
                      const int src_y_stride, const int dst_stride,
                      const int ccso_stride,
                      const int x, const int y,
                      const int pic_width, const int pic_height,
                      const int8_t *filter_offset, const int blk_size_x,
                      const int blk_size_y,
                      const int y_uv_hscale,  const int y_uv_vscale,
                      const int max_val, const uint8_t shift_bits,
                      const uint8_t ccso_bo_only);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_filter_block_hbd_with_buf_bo_only)(const uint16_t *src_y,
                      uint16_t *dst_yuv,
                      const uint8_t *src_cls0,
                      const uint8_t *src_cls1,
                      const int src_y_stride, const int dst_stride,
                      const int ccso_stride,
                      const int x, const int y,
                      const int pic_width, const int pic_height,
                      const int8_t *filter_offset, const int blk_size_x,
                      const int blk_size_y,
                      const int y_uv_hscale,  const int y_uv_vscale,
                      const int max_val, const uint8_t shift_bits,
                      const uint8_t ccso_bo_only);
#if defined(__AVX2__)
    void ccso_filter_block_hbd_with_buf_bo_only_avx2(const uint16_t *src_y,
                      uint16_t *dst_yuv,
                      const uint8_t *src_cls0,
                      const uint8_t *src_cls1,
                      const int src_y_stride, const int dst_stride,
                      const int ccso_stride,
                      const int x, const int y,
                      const int pic_width, const int pic_height,
                      const int8_t *filter_offset, const int blk_size_x,
                      const int blk_size_y,
                      const int y_uv_hscale,  const int y_uv_vscale,
                      const int max_val, const uint8_t shift_bits,
                      const uint8_t ccso_bo_only);
#endif
#else
    #define ccso_filter_block_hbd_with_buf_bo_only ccso_filter_block_hbd_with_buf_bo_only_c
#endif
#else
    void ccso_filter_block_hbd_with_buf_c(const uint16_t *src_y, uint16_t *dst_yuv,
                                          const uint8_t *src_cls0, const uint8_t *src_cls1,
                                          const int src_y_stride, const int dst_stride,
                                          const int ccso_stride,
                                          const int x, const int y,
                                          const int pic_width, const int pic_height,
                                          const int8_t *filter_offset, const int blk_size,
                                          const int y_uv_hscale,  const int y_uv_vscale,
                                          const int max_val, const uint8_t shift_bits,
                                          const uint8_t ccso_bo_only);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_filter_block_hbd_with_buf)(const uint16_t *src_y, uint16_t *dst_yuv,
                                          const uint8_t *src_cls0, const uint8_t *src_cls1,
                                          const int src_y_stride, const int dst_stride,
                                          const int ccso_stride,
                                          const int x, const int y,
                                          const int pic_width, const int pic_height,
                                          const int8_t *filter_offset, const int blk_size,
                                          const int y_uv_hscale,  const int y_uv_vscale,
                                          const int max_val, const uint8_t shift_bits,
                                          const uint8_t ccso_bo_only);
#if defined(__AVX2__)
    void ccso_filter_block_hbd_with_buf_avx2(const uint16_t *src_y, uint16_t *dst_yuv,
                                          const uint8_t *src_cls0, const uint8_t *src_cls1,
                                          const int src_y_stride, const int dst_stride,
                                          const int ccso_stride,
                                          const int x, const int y,
                                          const int pic_width, const int pic_height,
                                          const int8_t *filter_offset, const int blk_size,
                                          const int y_uv_hscale,  const int y_uv_vscale,
                                          const int max_val, const uint8_t shift_bits,
                                          const uint8_t ccso_bo_only);
#endif
#else
    #define ccso_filter_block_hbd_with_buf ccso_filter_block_hbd_with_buf_c
#endif
    void ccso_filter_block_hbd_with_buf_bo_only_c(const uint16_t *src_y, uint16_t *dst_yuv,
                                                  const uint8_t *src_cls0, const uint8_t *src_cls1,
                                                  const int src_y_stride, const int dst_stride,
                                                  const int ccso_stride,
                                                  const int x, const int y,
                                                  const int pic_width, const int pic_height,
                                                  const int8_t *filter_offset, const int blk_size,
                                                  const int y_uv_hscale,  const int y_uv_vscale,
                                                  const int max_val, const uint8_t shift_bits,
                                                  const uint8_t ccso_bo_only);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_filter_block_hbd_with_buf_bo_only)(const uint16_t *src_y, uint16_t *dst_yuv,
                                                  const uint8_t *src_cls0, const uint8_t *src_cls1,
                                                  const int src_y_stride, const int dst_stride,
                                                  const int ccso_stride,
                                                  const int x, const int y,
                                                  const int pic_width, const int pic_height,
                                                  const int8_t *filter_offset, const int blk_size,
                                                  const int y_uv_hscale,  const int y_uv_vscale,
                                                  const int max_val, const uint8_t shift_bits,
                                                  const uint8_t ccso_bo_only);
#if defined(__AVX2__)
    void ccso_filter_block_hbd_with_buf_bo_only_avx2(const uint16_t *src_y, uint16_t *dst_yuv,
                                                  const uint8_t *src_cls0, const uint8_t *src_cls1,
                                                  const int src_y_stride, const int dst_stride,
                                                  const int ccso_stride,
                                                  const int x, const int y,
                                                  const int pic_width, const int pic_height,
                                                  const int8_t *filter_offset, const int blk_size,
                                                  const int y_uv_hscale,  const int y_uv_vscale,
                                                  const int max_val, const uint8_t shift_bits,
                                                  const uint8_t ccso_bo_only);
#endif
#else
    #define ccso_filter_block_hbd_with_buf_bo_only ccso_filter_block_hbd_with_buf_bo_only_c
#endif
#endif // CONFIG_CCSO_FU_BUGFIX

    uint64_t compute_distortion_block_c(const uint16_t *org, const int org_stride, const uint16_t *rec16, const int rec_stride, const int x, const int y, const int log2_filter_unit_size_y, const int log2_filter_unit_size_x, const int height, const int width);
#if HAVE_SIMD
    RTCD_EXTERN uint64_t (*compute_distortion_block)(const uint16_t *org, const int org_stride, const uint16_t *rec16, const int rec_stride, const int x, const int y, const int log2_filter_unit_size_y, const int log2_filter_unit_size_x, const int height, const int width);
#if defined(__AVX2__)
    uint64_t compute_distortion_block_avx2(const uint16_t *org, const int org_stride, const uint16_t *rec16, const int rec_stride, const int x, const int y, const int log2_filter_unit_size_y, const int log2_filter_unit_size_x, const int height, const int width);
#endif
#else
    #define compute_distortion_block compute_distortion_block_c
#endif
#if CONFIG_CCSO_FU_BUGFIX
    void ccso_derive_src_block_c(const uint16_t *src_y, uint8_t *const src_cls0,
                          uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride,
                          const int x, const int y, const int pic_width, const int pic_height,
                          const int y_uv_hscale, const int y_uv_vscale, const int qstep,
                          const int neg_qstep, const int *src_loc, const int blk_size_x,
                          const int blk_size_y, const int edge_clf);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_derive_src_block)(const uint16_t *src_y, uint8_t *const src_cls0,
                          uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride,
                          const int x, const int y, const int pic_width, const int pic_height,
                          const int y_uv_hscale, const int y_uv_vscale, const int qstep,
                          const int neg_qstep, const int *src_loc, const int blk_size_x,
                          const int blk_size_y, const int edge_clf);
#if defined(__AVX2__)
    void ccso_derive_src_block_avx2(const uint16_t *src_y, uint8_t *const src_cls0,
                          uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride,
                          const int x, const int y, const int pic_width, const int pic_height,
                          const int y_uv_hscale, const int y_uv_vscale, const int qstep,
                          const int neg_qstep, const int *src_loc, const int blk_size_x,
                          const int blk_size_y, const int edge_clf);
#endif
#else
    #define ccso_derive_src_block ccso_derive_src_block_c
#endif
#else
    void ccso_derive_src_block_c(const uint16_t *src_y, uint8_t *const src_cls0, uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int y_uv_hscale, const int y_uv_vscale, const int qstep, const int neg_qstep, const int *src_loc, const int blk_size, const int edge_clf);
#if HAVE_SIMD
    RTCD_EXTERN void (*ccso_derive_src_block)(const uint16_t *src_y, uint8_t *const src_cls0, uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int y_uv_hscale, const int y_uv_vscale, const int qstep, const int neg_qstep, const int *src_loc, const int blk_size, const int edge_clf);
#if defined(__AVX2__)
    void ccso_derive_src_block_avx2(const uint16_t *src_y, uint8_t *const src_cls0, uint8_t *const src_cls1, const int src_y_stride, const int ccso_stride, const int x, const int y, const int pic_width, const int pic_height, const int y_uv_hscale, const int y_uv_vscale, const int qstep, const int neg_qstep, const int *src_loc, const int blk_size, const int edge_clf);
#endif
#else
    #define ccso_derive_src_block ccso_derive_src_block_c
#endif
#endif // CONFIG_CCSO_FU_BUGFIX
//#endif

// WARPED_MOTION / GLOBAL_MOTION functions



#if CONFIG_OPFL_MEMBW_REDUCTION
void av1_highbd_warp_affine_c(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta, int use_damr_padding, ReferenceArea *ref_area);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_highbd_warp_affine)(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta, int use_damr_padding, ReferenceArea *ref_area);
void av1_highbd_warp_affine_sse4_1(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta, int use_damr_padding, ReferenceArea *ref_area);
#endif
#if defined(__AVX2__)
void av1_highbd_warp_affine_avx2(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int16_t alpha, int16_t beta, int16_t gamma, int16_t delta, int use_damr_padding, ReferenceArea *ref_area);
#endif
#else
#define av1_highbd_warp_affine av1_highbd_warp_affine_c
#endif
#else
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
#endif

#if CONFIG_EXT_WARP_FILTER
#if CONFIG_WARP_BD_BOX
void av1_ext_highbd_warp_affine_c(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int use_warp_bd_box, WarpBoundaryBox *warp_bd_box, int use_warp_bd_damr, WarpBoundaryBox *warp_bd_box_damr);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_ext_highbd_warp_affine)(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int use_warp_bd_box, WarpBoundaryBox *warp_bd_box, int use_warp_bd_damr, WarpBoundaryBox *warp_bd_box_damr);
void av1_ext_highbd_warp_affine_sse4_1(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params, int use_warp_bd_box, WarpBoundaryBox *warp_bd_box, int use_warp_bd_damr, WarpBoundaryBox *warp_bd_box_damr);
#endif
#else
#define av1_ext_highbd_warp_affine av1_ext_highbd_warp_affine_c
#endif
#else
void av1_ext_highbd_warp_affine_c(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params);
#if HAVE_SIMD
#if defined(__SSE4_1__)
RTCD_EXTERN void (*av1_ext_highbd_warp_affine)(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params);
void av1_ext_highbd_warp_affine_sse4_1(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params);
#endif
#if defined(__AVX2__)
void av1_ext_highbd_warp_affine_avx2(const int32_t *mat, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row, int p_width, int p_height, int p_stride, int subsampling_x, int subsampling_y, int bd, ConvolveParams *conv_params);
#endif
#else
#define av1_ext_highbd_warp_affine av1_ext_highbd_warp_affine_c
#endif
#endif
#endif

#if CONFIG_AFFINE_REFINEMENT
void av1_warp_plane_bilinear_c(WarpedMotionParams *wm, int bd, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row,int p_width, int p_height, int p_stride,int subsampling_x, int subsampling_y, ConvolveParams *conv_params, ReferenceArea *ref_area);
#if HAVE_SIMD
RTCD_EXTERN void (*av1_warp_plane_bilinear)(WarpedMotionParams *wm, int bd, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row,int p_width, int p_height, int p_stride,int subsampling_x, int subsampling_y, ConvolveParams *conv_params, ReferenceArea *ref_area);
#if defined(__AVX2__)
void av1_warp_plane_bilinear_avx2(WarpedMotionParams *wm, int bd, const uint16_t *ref, int width, int height, int stride, uint16_t *pred, int p_col, int p_row,int p_width, int p_height, int p_stride,int subsampling_x, int subsampling_y, ConvolveParams *conv_params, ReferenceArea *ref_area);
#endif
#else
#define av1_warp_plane_bilinear av1_warp_plane_bilinear_c
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
void mhccp_predict_hv_hbd_c(const uint16_t *input, uint16_t *dst, bool have_top, bool have_left, int dst_stride, int64_t *alpha_q3, int bit_depth, int width, int height, int dir);
#if HAVE_SIMD
RTCD_EXTERN void (*mhccp_predict_hv_hbd)(const uint16_t *input, uint16_t *dst, bool have_top, bool have_left, int dst_stride, int64_t *alpha_q3, int bit_depth, int width, int height, int dir);
#if defined(__SSE4_1__) && MHCCP_CONVOLVE_SIMPLIFY && 0
void mhccp_predict_hv_hbd_sse4_1(const uint16_t *input, uint16_t *dst, bool have_top, bool have_left, int dst_stride, int64_t *alpha_q3, int bit_depth, int width, int height, int dir);
#endif
#else
#define mhccp_predict_hv_hbd mhccp_predict_hv_hbd_c
#endif
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
RTCD_EXTERN cfl_predict_hbd_fn (*cfl_get_predict_hbd_fn)(TX_SIZE tx_size);
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
  av1_fill_tskip_sum_buffer = av1_fill_tskip_sum_buffer_c;
  fill_directional_feature_buffers_highbd = fill_directional_feature_buffers_highbd_c;
  av1_fill_directional_feature_accumulators = av1_fill_directional_feature_accumulators_c;
  av1_fill_tskip_feature_accumulator =av1_fill_tskip_feature_accumulator_c;
  av1_convolve_symmetric_highbd = av1_convolve_symmetric_highbd_c;
  av1_convolve_symmetric_subtract_center_highbd = av1_convolve_symmetric_subtract_center_highbd_c;
  av1_convolve_symmetric_dual_highbd = av1_convolve_symmetric_dual_highbd_c;
  av1_convolve_symmetric_dual_subtract_center_highbd = av1_convolve_symmetric_dual_subtract_center_highbd_c;
  av1_convolve_mixedsymmetric_highbd = av1_convolve_mixedsymmetric_highbd_c;
  av1_filter_intra_predictor = av1_filter_intra_predictor_c;
  av1_bicubic_grad_interpolation_highbd = av1_bicubic_grad_interpolation_highbd_c;
  av1_opfl_mv_refinement_nxn = av1_opfl_mv_refinement_nxn_c;
  av1_copy_pred_array_highbd = av1_copy_pred_array_highbd_c;
  inv_stxfm = inv_stxfm_c;
  av1_highbd_inv_txfm_add = av1_highbd_inv_txfm_add_c;
  av1_highbd_inv_txfm_add_4x4 = av1_highbd_inv_txfm_add_4x4_c;
  av1_highbd_inv_txfm_add_8x8 = av1_highbd_inv_txfm_add_8x8_c;
  av1_highbd_inv_txfm_add_4x8 = av1_highbd_inv_txfm_add_4x8_c;
  av1_highbd_inv_txfm_add_8x4 = av1_highbd_inv_txfm_add_8x4_c;
  av1_highbd_inv_txfm_add_4x16 = av1_highbd_inv_txfm_add_4x16_c;
  av1_highbd_inv_txfm_add_16x4 = av1_highbd_inv_txfm_add_16x4_c;
  av1_highbd_inv_txfm_add_4x32 = av1_highbd_inv_txfm_add_4x32_c;
  av1_highbd_inv_txfm_add_32x4 = av1_highbd_inv_txfm_add_32x4_c;
  av1_highbd_inv_txfm_add_4x64 = av1_highbd_inv_txfm_add_4x64_c;
  av1_highbd_inv_txfm_add_64x4 = av1_highbd_inv_txfm_add_64x4_c;
  av1_inv_txfm2d_add_4x4 = av1_inv_txfm2d_add_4x4_c;
  av1_inv_txfm2d_add_8x8 = av1_inv_txfm2d_add_8x8_c;
  av1_highbd_dr_prediction_z1 = av1_highbd_dr_prediction_z1_c;
  av1_highbd_dr_prediction_z2 = av1_highbd_dr_prediction_z2_c;
  av1_highbd_dr_prediction_z3 = av1_highbd_dr_prediction_z3_c;
#if CONFIG_IMPROVE_LOSSLESS_TXM
//#if CONFIG_AV1_ENCODER
  av1_lossless_fwd_idtx = av1_lossless_fwd_idtx_c;
//#endif
  av1_lossless_inv_idtx_add = av1_lossless_inv_idtx_add_c;
#endif
#if CONFIG_IDIF
  av1_highbd_dr_prediction_z1_idif = av1_highbd_dr_prediction_z1_idif_c;
  av1_highbd_dr_prediction_z2_idif = av1_highbd_dr_prediction_z2_idif_c;
  av1_highbd_dr_prediction_z3_idif = av1_highbd_dr_prediction_z3_idif_c;
#endif
  av1_build_compound_diffwtd_mask_highbd = av1_build_compound_diffwtd_mask_highbd_c;
  av1_build_compound_diffwtd_mask_d16 = av1_build_compound_diffwtd_mask_d16_c;
#if CONFIG_AFFINE_REFINEMENT
  av1_calc_affine_autocorrelation_matrix = av1_calc_affine_autocorrelation_matrix_c;
  av1_avg_pooling_pdiff_gradients = av1_avg_pooling_pdiff_gradients_c;
#endif
  av1_round_shift_array = av1_round_shift_array_c;
#if CONFIG_TCQ
  av1_decide_states = av1_decide_states_c;
  av1_pre_quant = av1_pre_quant_c;
  av1_get_rate_dist_def_luma = av1_get_rate_dist_def_luma_c;
  av1_get_rate_dist_def_chroma = av1_get_rate_dist_def_chroma_c;
  av1_get_rate_dist_lf_luma = av1_get_rate_dist_lf_luma_c;
  av1_get_rate_dist_lf_chroma = av1_get_rate_dist_lf_chroma_c;
  av1_update_states = av1_update_states_c;
  av1_calc_block_eob_rate = av1_calc_block_eob_rate_c;
  av1_find_best_path = av1_find_best_path_c;
  av1_get_coeff_ctx = av1_get_coeff_ctx_c;
  av1_update_nbr_diagonal = av1_update_nbr_diagonal_c;
#endif

//#if CONFIG_AV1_ENCODER
  av1_fwd_cross_chroma_tx_block = av1_fwd_cross_chroma_tx_block_c;
  fwd_stxfm = fwd_stxfm_c;
  inv_txfm = inv_txfm_c;
  fwd_txfm = fwd_txfm_c;
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
#if CONFIG_FLEX_PARTITION
  av1_fwd_txfm2d_4x32 = av1_fwd_txfm2d_4x32_c;
  av1_fwd_txfm2d_32x4 = av1_fwd_txfm2d_32x4_c;
  av1_fwd_txfm2d_8x64 = av1_fwd_txfm2d_8x64_c;
  av1_fwd_txfm2d_64x8 = av1_fwd_txfm2d_64x8_c;
  av1_fwd_txfm2d_4x64 = av1_fwd_txfm2d_4x64_c;
  av1_fwd_txfm2d_64x4 = av1_fwd_txfm2d_64x4_c;
#endif
  av1_highbd_quantize_fp = av1_highbd_quantize_fp_c;
  av1_txb_init_levels_skip = av1_txb_init_levels_skip_c;
  av1_txb_init_levels_signs = av1_txb_init_levels_signs_c;
  av1_txb_init_levels = av1_txb_init_levels_c;
  av1_get_crc32c_value = av1_get_crc32c_value_c;
  av1_compute_stats_highbd = av1_compute_stats_highbd_c;
  av1_highbd_pixel_proj_error = av1_highbd_pixel_proj_error_c;
  av1_get_horver_correlation_full = av1_get_horver_correlation_full_c;
#if !CONFIG_EXCLUDE_SIMD_MISMATCH
  av1_nn_predict = av1_nn_predict_c;
#endif
//#endif
#if CONFIG_GDF
  gdf_set_lap_and_cls_unit = gdf_set_lap_and_cls_unit_c;
  gdf_inference_unit = gdf_inference_unit_c;
  gdf_compensation_unit = gdf_compensation_unit_c;
#endif
  ccso_filter_block_hbd_wo_buf = ccso_filter_block_hbd_wo_buf_c;
//#if CONFIG_AV1_ENCODER
  ccso_filter_block_hbd_with_buf = ccso_filter_block_hbd_with_buf_c;
  ccso_filter_block_hbd_with_buf_bo_only = ccso_filter_block_hbd_with_buf_bo_only_c;
  compute_distortion_block = compute_distortion_block_c;
  ccso_derive_src_block = ccso_derive_src_block_c;
//#endif
  av1_highbd_warp_affine = av1_highbd_warp_affine_c;
#if CONFIG_EXT_WARP_FILTER
  av1_ext_highbd_warp_affine = av1_ext_highbd_warp_affine_c;
#endif
#if CONFIG_AFFINE_REFINEMENT
  av1_warp_plane_bilinear = av1_warp_plane_bilinear_c;
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
#if CONFIG_DIP
  av1_dip_matrix_multiplication = av1_dip_matrix_multiplication_c;
#endif
  mhccp_predict_hv_hbd = mhccp_predict_hv_hbd_c;
  av1_make_bawp_block = av1_make_bawp_block_c;
#endif
#if defined(__SSE2__) && HAVE_SIMD
  av1_lowbd_fwd_txfm = av1_lowbd_fwd_txfm_sse2;
  av1_highbd_block_error = av1_highbd_block_error_sse2;
  av1_wedge_sse_from_residuals = av1_wedge_sse_from_residuals_sse2;
  av1_wedge_sign_from_residuals = av1_wedge_sign_from_residuals_sse2;
  av1_wedge_compute_delta_squares = av1_wedge_compute_delta_squares_sse2;
//#if !defined(__MSVCRT__)
  cdef_find_dir = cdef_find_dir_sse2;
  cdef_filter_block = cdef_filter_block_sse2;
  cdef_copy_rect8_16bit_to_16bit = cdef_copy_rect8_16bit_to_16bit_sse2;
//#endif
  cfl_get_subtract_average_fn = cfl_get_subtract_average_fn_sse2;
#endif
#if defined(__SSSE3__) && HAVE_SIMD
  if (flags & HAS_SSSE3) av1_highbd_wiener_convolve_add_src = av1_highbd_wiener_convolve_add_src_ssse3;
  if (flags & HAS_SSSE3) av1_build_compound_diffwtd_mask_highbd = av1_build_compound_diffwtd_mask_highbd_ssse3;
//#if !defined(__MSVCRT__)
  if (flags & HAS_SSSE3) cdef_find_dir = cdef_find_dir_ssse3;
  if (flags & HAS_SSSE3) cdef_filter_block = cdef_filter_block_ssse3;
  if (flags & HAS_SSSE3) cdef_copy_rect8_16bit_to_16bit = cdef_copy_rect8_16bit_to_16bit_ssse3;
//#endif
  if (flags & HAS_SSSE3) av1_highbd_convolve_2d_sr = av1_highbd_convolve_2d_sr_ssse3;
  if (flags & HAS_SSSE3) av1_highbd_convolve_x_sr = av1_highbd_convolve_x_sr_ssse3;
  if (flags & HAS_SSSE3) av1_highbd_convolve_y_sr = av1_highbd_convolve_y_sr_ssse3;
  if (flags & HAS_SSSE3) cfl_get_luma_subsampling_420_hbd = cfl_get_luma_subsampling_420_hbd_ssse3;
  if (flags & HAS_SSSE3) cfl_get_luma_subsampling_422_hbd = cfl_get_luma_subsampling_422_hbd_ssse3;
  if (flags & HAS_SSSE3) cfl_get_luma_subsampling_444_hbd = cfl_get_luma_subsampling_444_hbd_ssse3;
#if !CONFIG_EXCLUDE_SIMD_MISMATCH
  if (flags & HAS_SSE3) av1_nn_predict = av1_nn_predict_sse3;
#endif
#endif
#if defined(__SSE4_1__) && HAVE_SIMD
#if MHCCP_CONVOLVE_SIMPLIFY && 0
  if (flags & HAS_SSE4_1) mhccp_predict_hv_hbd = mhccp_predict_hv_hbd_sse4_1;
#endif
  if (flags & HAS_SSE4_1) av1_highbd_convolve_horiz_rs = av1_highbd_convolve_horiz_rs_sse4_1;
  if (flags & HAS_SSE4_1) av1_filter_intra_predictor = av1_filter_intra_predictor_sse4_1;
  if (flags & HAS_SSE4_1) av1_bicubic_grad_interpolation_highbd = av1_bicubic_grad_interpolation_highbd_sse4_1;
  if (flags & HAS_SSE4_1) av1_opfl_mv_refinement_nxn = av1_opfl_mv_refinement_nxn_sse4_1;
  if (flags & HAS_SSE4_1) av1_copy_pred_array_highbd = av1_copy_pred_array_highbd_sse4_1;
  if (flags & HAS_SSE4_1) inv_stxfm = inv_stxfm_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add = av1_highbd_inv_txfm_add_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x4 = av1_highbd_inv_txfm_add_4x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_8x8 = av1_highbd_inv_txfm_add_8x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x8 = av1_highbd_inv_txfm_add_4x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_8x4 = av1_highbd_inv_txfm_add_8x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x16 = av1_highbd_inv_txfm_add_4x16_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_16x4 = av1_highbd_inv_txfm_add_16x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x32 = av1_highbd_inv_txfm_add_4x32_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_32x4 = av1_highbd_inv_txfm_add_32x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_4x64 = av1_highbd_inv_txfm_add_4x64_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_inv_txfm_add_64x4 = av1_highbd_inv_txfm_add_64x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_inv_txfm2d_add_4x4 = av1_inv_txfm2d_add_4x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_inv_txfm2d_add_8x8 = av1_inv_txfm2d_add_8x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_build_compound_diffwtd_mask_d16 = av1_build_compound_diffwtd_mask_d16_sse4_1;
  if (flags & HAS_SSE4_1) av1_round_shift_array = av1_round_shift_array_sse4_1;
//#if CONFIG_AV1_ENCODER
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
#if CONFIG_FLEX_PARTITION
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_4x32 = av1_fwd_txfm2d_4x32_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_32x4 = av1_fwd_txfm2d_32x4_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_8x64 = av1_fwd_txfm2d_8x64_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_64x8 = av1_fwd_txfm2d_64x8_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_4x64 = av1_fwd_txfm2d_4x64_sse4_1;
  if (flags & HAS_SSE4_1) av1_fwd_txfm2d_64x4 = av1_fwd_txfm2d_64x4_sse4_1;
#endif
  if (flags & HAS_SSE4_1) av1_highbd_quantize_fp = av1_highbd_quantize_fp_sse4_1;
  if (flags & HAS_SSE4_1) av1_txb_init_levels_skip = av1_txb_init_levels_skip_sse4_1;
  if (flags & HAS_SSE4_1) av1_txb_init_levels_signs = av1_txb_init_levels_signs_sse4_1;
  if (flags & HAS_SSE4_1) av1_txb_init_levels = av1_txb_init_levels_sse4_1;
  if (flags & HAS_SSE4_1) av1_compute_stats_highbd = av1_compute_stats_highbd_sse4_1;
  if (flags & HAS_SSE4_1) av1_highbd_pixel_proj_error = av1_highbd_pixel_proj_error_sse4_1;
  if (flags & HAS_SSE4_1) av1_get_horver_correlation_full = av1_get_horver_correlation_full_sse4_1;
//#endif
//#if !defined(__MSVCRT__)
  if (flags & HAS_SSE4_1) cdef_find_dir = cdef_find_dir_sse4_1;
  if (flags & HAS_SSE4_1) cdef_filter_block = cdef_filter_block_sse4_1;
  if (flags & HAS_SSE4_1) cdef_copy_rect8_16bit_to_16bit = cdef_copy_rect8_16bit_to_16bit_sse4_1;
//#endif
  if (flags & HAS_SSE4_1) av1_highbd_warp_affine = av1_highbd_warp_affine_sse4_1;
#if CONFIG_EXT_WARP_FILTER
  if (flags & HAS_SSE4_1) av1_ext_highbd_warp_affine = av1_ext_highbd_warp_affine_sse4_1;
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
  if (flags & HAS_AVX2) av1_make_bawp_block = av1_make_bawp_block_avx2;
  if (flags & HAS_AVX2) av1_fwd_cross_chroma_tx_block = av1_fwd_cross_chroma_tx_block_avx2;
  if (flags & HAS_AVX2) av1_highbd_wiener_convolve_add_src = av1_highbd_wiener_convolve_add_src_avx2;
  if (flags & HAS_AVX2) av1_fill_tskip_sum_buffer = av1_fill_tskip_sum_buffer_avx2;
  if (flags & HAS_AVX2) fill_directional_feature_buffers_highbd = fill_directional_feature_buffers_highbd_avx2;
  if (flags & HAS_AVX2) av1_fill_directional_feature_accumulators = av1_fill_directional_feature_accumulators_avx2;
  if (flags & HAS_AVX2) av1_fill_tskip_feature_accumulator = av1_fill_tskip_feature_accumulator_avx2;
  if (flags & HAS_AVX2) av1_convolve_symmetric_highbd = av1_convolve_symmetric_highbd_avx2;
  if (flags & HAS_AVX2) av1_convolve_symmetric_subtract_center_highbd = av1_convolve_symmetric_subtract_center_highbd_avx2;
  if (flags & HAS_AVX2) av1_convolve_symmetric_dual_highbd = av1_convolve_symmetric_dual_highbd_avx2;
  if (flags & HAS_AVX2) av1_convolve_symmetric_dual_subtract_center_highbd = av1_convolve_symmetric_dual_subtract_center_highbd_avx2;
  if (flags & HAS_AVX2) av1_convolve_mixedsymmetric_highbd = av1_convolve_mixedsymmetric_highbd_avx2;
  if (flags & HAS_AVX2) av1_highbd_dr_prediction_z1 = av1_highbd_dr_prediction_z1_avx2;
  if (flags & HAS_AVX2) av1_highbd_dr_prediction_z2 = av1_highbd_dr_prediction_z2_avx2;
  if (flags & HAS_AVX2) av1_highbd_dr_prediction_z3 = av1_highbd_dr_prediction_z3_avx2;
#if CONFIG_IMPROVE_LOSSLESS_TXM
//#if CONFIG_AV1_ENCODER
  if (flags & HAS_AVX2) av1_lossless_fwd_idtx = av1_lossless_fwd_idtx_avx2;
//#endif
  if (flags & HAS_AVX2) av1_lossless_inv_idtx_add = av1_lossless_inv_idtx_add_avx2;
#endif
#if CONFIG_IDIF
  if (flags & HAS_AVX2) av1_highbd_dr_prediction_z1_idif = av1_highbd_dr_prediction_z1_idif_avx2;
  if (flags & HAS_AVX2) av1_highbd_dr_prediction_z2_idif = av1_highbd_dr_prediction_z2_idif_avx2;
  if (flags & HAS_AVX2) av1_highbd_dr_prediction_z3_idif = av1_highbd_dr_prediction_z3_idif_avx2;
#endif
  if (flags & HAS_AVX2) av1_build_compound_diffwtd_mask_highbd = av1_build_compound_diffwtd_mask_highbd_avx2;
  if (flags & HAS_AVX2) av1_build_compound_diffwtd_mask_d16 = av1_build_compound_diffwtd_mask_d16_avx2;
#if CONFIG_AFFINE_REFINEMENT
  if (flags & HAS_AVX2) av1_calc_affine_autocorrelation_matrix = av1_calc_affine_autocorrelation_matrix_avx2;
  if (flags & HAS_AVX2) av1_avg_pooling_pdiff_gradients = av1_avg_pooling_pdiff_gradients_avx2;
#endif
#if CONFIG_TCQ
  if (flags & HAS_AVX2) av1_decide_states = av1_decide_states_avx2;
  if (flags & HAS_AVX2) av1_pre_quant = av1_pre_quant_avx2;
  if (flags & HAS_AVX2) av1_get_rate_dist_def_luma = av1_get_rate_dist_def_luma_avx2;
  if (flags & HAS_AVX2) av1_get_rate_dist_def_chroma = av1_get_rate_dist_def_chroma_avx2;
  if (flags & HAS_AVX2) av1_get_rate_dist_lf_luma = av1_get_rate_dist_lf_luma_avx2;
  if (flags & HAS_AVX2) av1_get_rate_dist_lf_chroma = av1_get_rate_dist_lf_chroma_avx2;
  if (flags & HAS_AVX2) av1_update_states = av1_update_states_avx2;
  if (flags & HAS_AVX2) av1_calc_block_eob_rate = av1_calc_block_eob_rate_avx2;
  if (flags & HAS_AVX2) av1_find_best_path = av1_find_best_path_avx2;
  if (flags & HAS_AVX2) av1_get_coeff_ctx = av1_get_coeff_ctx_avx2;
  if (flags & HAS_AVX2) av1_update_nbr_diagonal = av1_update_nbr_diagonal_avx2;
#endif
  if (flags & HAS_AVX2) fwd_stxfm = fwd_stxfm_avx2;
  if (flags & HAS_AVX2) av1_lowbd_fwd_txfm = av1_lowbd_fwd_txfm_avx2;
  if (flags & HAS_AVX2) inv_txfm = inv_txfm_avx2;
  if (flags & HAS_AVX2) fwd_txfm = fwd_txfm_avx2;
  if (flags & HAS_AVX2) av1_fwd_txfm2d_8x16 = av1_fwd_txfm2d_8x16_avx2;
  if (flags & HAS_AVX2) av1_fwd_txfm2d_16x8 = av1_fwd_txfm2d_16x8_avx2;
  if (flags & HAS_AVX2) av1_fwd_txfm2d_8x8 = av1_fwd_txfm2d_8x8_avx2;
  if (flags & HAS_AVX2) av1_fwd_txfm2d_16x16 = av1_fwd_txfm2d_16x16_avx2;
  if (flags & HAS_AVX2) av1_fwd_txfm2d_32x32 = av1_fwd_txfm2d_32x32_avx2;
  if (flags & HAS_AVX2) av1_fwd_txfm2d_64x64 = av1_fwd_txfm2d_64x64_avx2;
  if (flags & HAS_AVX2) av1_highbd_block_error = av1_highbd_block_error_avx2;
  if (flags & HAS_AVX2) av1_highbd_quantize_fp = av1_highbd_quantize_fp_avx2;
  if (flags & HAS_AVX2) av1_txb_init_levels_skip = av1_txb_init_levels_skip_avx2;
  if (flags & HAS_AVX2) av1_txb_init_levels_signs = av1_txb_init_levels_signs_avx2;
  if (flags & HAS_AVX2) av1_txb_init_levels = av1_txb_init_levels_avx2;
  if (flags & HAS_AVX2) av1_wedge_sse_from_residuals = av1_wedge_sse_from_residuals_avx2;
  if (flags & HAS_AVX2) av1_wedge_sign_from_residuals = av1_wedge_sign_from_residuals_avx2;
  if (flags & HAS_AVX2) av1_wedge_compute_delta_squares = av1_wedge_compute_delta_squares_avx2;
  if (flags & HAS_AVX2) av1_compute_stats_highbd = av1_compute_stats_highbd_avx2;
  if (flags & HAS_AVX2) av1_highbd_pixel_proj_error = av1_highbd_pixel_proj_error_avx2;
  if (flags & HAS_AVX2) av1_get_horver_correlation_full = av1_get_horver_correlation_full_avx2;
  if (flags & HAS_AVX2) cdef_find_dir = cdef_find_dir_avx2;
  if (flags & HAS_AVX2) cdef_filter_block = cdef_filter_block_avx2;
  if (flags & HAS_AVX2) cdef_copy_rect8_16bit_to_16bit = cdef_copy_rect8_16bit_to_16bit_avx2;
#if CONFIG_GDF
  if (flags & HAS_AVX2) gdf_set_lap_and_cls_unit = gdf_set_lap_and_cls_unit_avx2;
  if (flags & HAS_AVX2) gdf_inference_unit = gdf_inference_unit_avx2;
  if (flags & HAS_AVX2) gdf_compensation_unit = gdf_compensation_unit_avx2;
#endif
  if (flags & HAS_AVX2) ccso_filter_block_hbd_wo_buf = ccso_filter_block_hbd_wo_buf_avx2;
//#if CONFIG_AV1_ENCODER
  if (flags & HAS_AVX2) ccso_filter_block_hbd_with_buf = ccso_filter_block_hbd_with_buf_avx2;
  if (flags & HAS_AVX2) ccso_filter_block_hbd_with_buf_bo_only = ccso_filter_block_hbd_with_buf_bo_only_avx2;
  if (flags & HAS_AVX2) compute_distortion_block = compute_distortion_block_avx2;
  if (flags & HAS_AVX2) ccso_derive_src_block = ccso_derive_src_block_avx2;
//#endif
#if !CONFIG_OPFL_MEMBW_REDUCTION
  if (flags & HAS_AVX2) av1_highbd_warp_affine = av1_highbd_warp_affine_avx2;
#endif
#if CONFIG_EXT_WARP_FILTER && !CONFIG_WARP_BD_BOX
  if (flags & HAS_AVX2) av1_ext_highbd_warp_affine = av1_ext_highbd_warp_affine_avx2;
#endif
#if CONFIG_AFFINE_REFINEMENT
  if (flags & HAS_AVX2) av1_warp_plane_bilinear = av1_warp_plane_bilinear_avx2;
#endif
  if (flags & HAS_AVX2) cfl_get_subtract_average_fn = cfl_get_subtract_average_fn_avx2;
#if CONFIG_DIP
  if (flags & HAS_AVX2) av1_dip_matrix_multiplication = av1_dip_matrix_multiplication_avx2;
#endif
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
