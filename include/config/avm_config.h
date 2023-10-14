/*
 * Copyright (c) 2020, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */
#ifndef AVM_CONFIG_H_
#define AVM_CONFIG_H_

// CPUs.
#define ARCH_ARM 0 //Enables ARM architecture.
#define ARCH_MIPS 0 //Enables MIPS architecture.
#define ARCH_PPC 0 //Enables PPC architecture.
#define ARCH_X86 0 //Enables X86 architecture.
#define ARCH_X86_64 1 //Enables X86_64 architecture.

// ARM feature flags.
#define HAVE_NEON 0 //Enables NEON intrinsics optimizations.

// MIPS feature flags.
#define HAVE_DSPR2 0 //Enables DSPR2 optimizations.
#define HAVE_MIPS32 0 //Enables MIPS32 optimizations.
#define HAVE_MIPS64 0 //Enables MIPS64 optimizations.
#define HAVE_MSA 0 //Enables MSA optimizations.

// PPC feature flags.
#define HAVE_VSX 0 //Enables VSX optimizations.

// x86/x86_64 feature flags.

#define HAVE_AVX 1 //Enables AVX optimizations.
#define HAVE_AVX2 0 //Enables AVX2 optimizations.
#define HAVE_MMX 1 //Enables MMX optimizations.
#define HAVE_SSE 1 //Enables SSE optimizations.
#define HAVE_SSE2 1 //Enables SSE2 optimizations.
#define HAVE_SSE3 1 //Enables SSE3 optimizations.
#define HAVE_SSE4_1 1 //Enables SSE 4.1 optimizations.
#define HAVE_SSE4_2 1 //Enables SSE 4.2 optimizations.
#define HAVE_SSSE3 1 //Enables SSSE3 optimizations.

// Flags describing the build environment.

#define HAVE_FEXCEPT 0
                   //Internal flag, GNU fenv.h present for target.
#define HAVE_PTHREAD_H 0 //Internal flag, target pthread support.
#define HAVE_UNISTD_H 0
                   //Internal flag, unistd.h present for target.
#define HAVE_WXWIDGETS 0 //WxWidgets present.

// Variables in this section can be set from the CMake command line or from
// within the CMake GUI. The variables control libaom features.

// Build configuration flags.

#define AOM_RTCD_FLAGS //"
                   //Arguments to pass to rtcd.pl. Separate with ';'
#define CONFIG_AV1_DECODER 1 //Enable AV1 decoder.
#define CONFIG_AV1_ENCODER 1 //Enable AV1 encoder.
#define CONFIG_BIG_ENDIAN 0 //Internal flag.
#define CONFIG_GCC 0 //Building with GCC (detect).
#define CONFIG_GCOV 0 //Enable gcov support.
#define CONFIG_GPROF 0 //Enable gprof support.
#define CONFIG_LIBYUV 1 //Enables libyuv scaling/conversion support.

#define CONFIG_MULTITHREAD 1 //Multithread support.
#define CONFIG_OS_SUPPORT 0 //Internal flag.
#define CONFIG_PIC 0 //Build with PIC enabled.
#define CONFIG_RUNTIME_CPU_DETECT 1 //Runtime CPU detection support.
#define CONFIG_SHARED 0 //Build shared libs.
#define CONFIG_WEBM_IO 0 //Enables WebM support.

// Debugging flags.
#define CONFIG_DEBUG 0 //Debug build flag.
#define CONFIG_MISMATCH_DEBUG 0 //Mismatch debugging flag.
#define CONFIG_EXCLUDE_SIMD_MISMATCH 1
                   //Exclude mismatch in SIMD functions for testing/debugging.

// AV1 feature flags.

#define CONFIG_ACCOUNTING 0 //Enables bit accounting.
#define CONFIG_ANALYZER 0 //Enables bit stream analyzer.
#define CONFIG_COEFFICIENT_RANGE_CHECKING 0
                   //Coefficient range check.
#define CONFIG_DENOISE 1
                   //Denoise/noise modeling support in encoder.
#define CONFIG_INSPECTION 0 //Enables bitstream inspection.
#define CONFIG_INTERNAL_STATS 0 //Enables internal encoder stats.
#define CONFIG_MAX_DECODE_PROFILE 2
                   //Max profile to support decoding.
#define CONFIG_NORMAL_TILE_MODE 0 //Only enables normal tile mode.
#define CONFIG_SIZE_LIMIT 0 //Limit max decode width/height.
#define CONFIG_SPATIAL_RESAMPLING 1 //Spatial resampling.
#define DECODE_HEIGHT_LIMIT 0 //Set limit for decode height.
#define DECODE_WIDTH_LIMIT 0 //Set limit for decode width.
#define CONFIG_TUNE_VMAF 0 //Enable encoding tuning for VMAF.
#define CONFIG_USE_VMAF_RC 0 //Use libvmaf_rc tune for VMAF_NEG.

// AV1 experiment flags.
#define CONFIG_SPEED_STATS 0 //AV1 experiment flag.
#define CONFIG_COLLECT_RD_STATS 0 //AV1 experiment flag.
#define CONFIG_DIST_8X8 0 //AV1 experiment flag.
#define CONFIG_ENTROPY_STATS 0 //AV1 experiment flag.
#define CONFIG_INTER_STATS_ONLY 0 //AV1 experiment flag.
#define CONFIG_BITSTREAM_DEBUG 0
                   //AV1 experiment flag for bitstream debugging.
#define CONFIG_RD_DEBUG 0 //AV1 experiment flag.
#define CONFIG_SHARP_SETTINGS 0 //AV1 experiment flag.
#define CONFIG_DISABLE_FULL_PIXEL_SPLIT_8X8 1
                   //Disable full_pixel_motion_search_based_split on BLOCK_8X8.
#define CONFIG_COLLECT_PARTITION_STATS 0
                   //Collect stats on partition decisions.
#define CONFIG_COLLECT_COMPONENT_TIMING 0
                   //Collect encoding component timing information.
#define CONFIG_LPF_MASK 0
                   //Enable the use loop filter bitmasks for optimizations.
#define CONFIG_AV1_TEMPORAL_DENOISING 0
                   //Build with temporal denoising support.
#define CONFIG_NN_V2 0 //Fully-connected neural nets ver.2.
#define CONFIG_OPTICAL_FLOW_API 0
                   //AV1 experiment flag for optical flow API.
#define CONFIG_AV2CTC_PSNR_PEAK 1
                   //Use AV2 CTC type PSNR peak for 10- and 12-bit
#define CONFIG_ZERO_OFFSET_BITUPSHIFT 0
                   //Use zero offset for non-normative bit upshift

// AV2 experiment flags.

#define CONFIG_ATC 1 //AV2 enable adaptive transform coding.
#define CONFIG_ATC_REDUCED_TXSET 1
                   //AV2 enable reduced transform set.
#define CONFIG_ATC_DCTX_ALIGNED 1
  //AV2 TX signaling restriction for DC blocks with EOB alignment.
#define CONFIG_BYPASS_IMPROVEMENT 1
                   //AV2 enable entropy bypass improvement.
#define CONFIG_EXT_DIR 1 //AV2 extended intra prediction angles.
#define CONFIG_EXT_RECUR_PARTITIONS 1 //NUMBER
  //AV2 Fully recursive partitions including H partitions experiment flag
#define CONFIG_ERP_TFLITE 0 //NUMBER //Build ERP with TFLite
#define CONFIG_UNEVEN_4WAY 0 //NUMBER
                   //AV2 uneven 4-way partition experiment flag
#define CONFIG_COMPOUND_WARP_SAMPLES 1 //NUMBER
                   //AV2 compound warped motion samples experiment flag
#define CONFIG_NEW_TX_PARTITION 1
                   //AV2 new transform partitions experiment flag.
#define CONFIG_ORIP 1
  //AV2 experiment flag to enable offset based refinement of intra prediction.
#define CONFIG_IDIF 1
  //AV2 experiment flag to enable Intra Directional Interpolation Filter.
#define CONFIG_ORIP_DC_DISABLED 0
                   //AV2 experiment flag to disable ORIP for DC mode.
#define CONFIG_ORIP_NONDC_DISABLED 0
                   //AV2 experiment flag to disable ORIP for non-DC modes.
#define CONFIG_MVP_IMPROVEMENT 1 //Enable MVP improvement
#define CONFIG_REF_MV_BANK 1 //AV2 ref mv bank experiment flag
#define CONFIG_CCSO 1 //AV2 experiment flag to enable cross component sample offset.
#define CONFIG_OPTFLOW_REFINEMENT 1
                   //AV2 experiment flag for optical flow MV refinement
#define CONFIG_IBP_DC 1
  //AV2 experiment flag to enable intra bi-prediction for DC mode.
#define CONFIG_AIMC 1 //AV2 adaptive intra mode coding flag.
#define CONFIG_CONTEXT_DERIVATION 1
  //AV2 experiment flag to enable modified context derivation : CWG-B065.
#define CONFIG_EXTENDED_WARP_PREDICTION 1
                   //AV2 experiment flag to add new local warp modes
#define CONFIG_WARP_REF_LIST 1
                   //AV2 experiment flag to improve warp-delta mode
// Begin: CWG-C016
#define CONFIG_LR_MERGE_COEFFS 1
                   //AV2 experiment flag to enable LR coefficient merging
#define CONFIG_LR_FLEX_SYNTAX 1
                   //AV2 experiment flag to enable LR flexible syntax
#define CONFIG_WIENER_NONSEP 1 //NUMBER
                   //AV2 nonsep Wiener filter experiment flag
#define CONFIG_WIENER_NONSEP_CROSS_FILT 1 //NUMBER
                   //AV2 nonsep Wiener cross filter experiment flag
#define CONFIG_PC_WIENER 1 //NUMBER
                   //AV2 pixel-classified Wiener filter experiment flag
// End: CWG-C016
#define CONFIG_HIGH_PASS_CROSS_WIENER_FILTER 1
                   //AV2 high pass cross non-sep wiener filter experiment flag
#define CONFIG_FLEXIBLE_RU_SIZE 1
  //AV2 experiment flag to choose RU size between 128x128, 256x256 and 512x512

// Source of throughput analysis : CWG-B065
#define CONFIG_THROUGHPUT_ANALYSIS 0
                   //AV2 experiment flag to measure throughput.
#define CONFIG_IBC_SR_EXT 1 //Enables IntraBC search range extension
#define CONFIG_IBC_BV_IMPROVEMENT 1
                   //Enables BV improvements for IBC
#define CONFIG_CCSO_EXT 1
                   //AV2 experiment flag to enable extended CCSO.
#define CONFIG_CFL_IMPROVEMENTS 1
                   //AV2 Cfl improvements from CWG-D029.
#define CONFIG_ADPTIVE_DS_422 1
                   //AV2 adaptive downsampling in CfL for 422 from CWG-D028.
#define CONFIG_ADAPTIVE_MVD 1 //Enable adaptive MVD resolution
#define CONFIG_JOINT_MVD 1 //Enable joint MVD coding
#define CONFIG_IMPROVED_JMVD 1
                   //Enable joint MVD coding with multiple scaling factors
#define CONFIG_PALETTE_IMPROVEMENTS 1
  //AV2 experiment flag for palette parsing independency and improved palette color map coding.

#define CONFIG_SKIP_MODE_SSE_BUG_FIX 1
                   //AV2 experiment flag to fix the SSE calc bug for skip mode.
#define CONFIG_SKIP_MODE_ENHANCEMENT 1
                   //AV2 experiment flag to enable skip mode enhancement.
#define CONFIG_TIP 1 //Enable temporal interpolated prediction (TIP)
#define CONFIG_OUTPUT_FRAME_BASED_ON_ORDER_HINT 1
                   //Enable frame output order derivation from order hint
#define CONFIG_OPTFLOW_ON_TIP 1
                   //Enable optical flow refinement on top of TIP
#define CONFIG_FLEX_MVRES 1
                   //AV2 flexible mv precision experiment flag
#define CONFIG_ALLOW_SAME_REF_COMPOUND 1
                   //Allow compound mode to refer to the same reference frame
#define CONFIG_DISPLAY_ORDER_HINT_FIX 1
                   //Bug fix on display order hints of key frames
#define CONFIG_PAR_HIDING 1
                   //Enable parity hiding for coefficients coding. (PH)
#define CONFIG_BAWP 1 //Enable block adaptive weighted prediction
#define CONFIG_WARPMV 1 //Enable warpmv modes
#define CONFIG_IMPROVED_ANGULAR_INTRA 1
                   //Improved angular intra prediction mode
#define CONFIG_D071_IMP_MSK_BLD 1
                   //Enable single reference mode for frame boundary

#define CONFIG_SKIP_TXFM_OPT 1
                   //Enable to optimize the signaling of skip_txfm
#define CONFIG_CWP 1 //Enables compound weighted prediction.
#define CONFIG_REFINEMV 1 //Enable refinemv modes

#define CONFIG_EXPLICIT_TEMPORAL_DIST_CALC 1
                   //Enable to explicit temporal distance calculation

#define CONFIG_IMPROVED_GLOBAL_MOTION 1
                   //New global motion syntax for AV2
#define CONFIG_SEP_COMP_DRL 1
                   //Use separate drl list for compound modes
#define CONFIG_SKIP_ME_FOR_OPFL_MODES 1
                   //Reuse the mvs of compound mode from non-opfl path

#define CONFIG_CWG_D067_IMPROVED_WARP 1
                   //Improvement of warp motions
#define CONFIG_ADST_TUNED 1
                   //AV2 experiment to replace the ADST 4, 8 and 16 basis
#define CONFIG_TX_PARTITION_CTX 1
                   //Enable to optimize txfm partition context

// This is an encode-only change.
#define CONFIG_MV_SEARCH_RANGE 1
                   //Enable a sufficient MV search range.
#define CONFIG_FIX_CDEF_SYNTAX 1
                   //AV2 experiment flag to fix CDEF syntax.
#define CONFIG_IMPROVED_CFL 1
                   //Enable improved CfL mode from CWG-C044
#define CONFIG_BLEND_MODE 1
                   //Enable improved intra blend mode from CWG-D046

#define CONFIG_PEF 1 //AV2 experiment flag to enable prediction enhancement filter

#define CONFIG_ADAPTIVE_DS_FILTER 1
  //Enable adaptive downsampling filter: 0: AV1 filter, 1: 121 filter, 2: 1511 filter

#define CONFIG_C071_SUBBLK_WARPMV 1
                   //AV2 experiment flag to use subblock warp MV for SMVP

#define CONFIG_C076_INTER_MOD_CTX 1
                   //AV2 experiment flag to simplify inter mode contexts

#define CONFIG_NEW_CONTEXT_MODELING 1
                   //Enable to improve the context modeling
#define CONFIG_CROSS_CHROMA_TX 1
                   //AV2 cross chroma component transform experiment flag.
#define CONFIG_WEDGE_MOD_EXT 1 //AV2 wedge modes extensions.

#define CONFIG_MF_IMPROVEMENT 1
                   //Enable to improve temporal motion projection
#define CONFIG_MRSSE 0 //Enable MRSSE

#define CONFIG_IST_SET_FLAG 1
                   //AV2 experiment flag to signal Secondary Tx set ID.
#define INLINE inline

#endif  // AOM_CONFIG_H_
