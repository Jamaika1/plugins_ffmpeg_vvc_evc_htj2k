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
#define AOM_ARCH_AARCH64 0 //Enables AArch64 architecture.
#define ARCH_ARM 0 //Enables ARM architecture.
#define ARCH_MIPS 0 //Enables MIPS architecture.
#define ARCH_PPC 0 //Enables PPC architecture.
#if defined(__X86_64__)
#define ARCH_X86 0 //Enables X86 architecture.
#define ARCH_X86_64 1 //Enables X86_64 architecture.
#else
#define ARCH_X86 1 //Enables X86 architecture.
#define ARCH_X86_64 0 //Enables X86_64 architecture.
#endif

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

#define HAVE_FEXCEPT 0 //Internal flag, GNU fenv.h present for target.
#define HAVE_PTHREAD_H 1 //Internal flag, target pthread support.
#define HAVE_UNISTD_H 0 //Internal flag, unistd.h present for target.
#define HAVE_WXWIDGETS 0 //WxWidgets present.

// Variables in this section can be set from the CMake command line or from
// within the CMake GUI. The variables control libaom features.

// Build configuration flags.

//#define AOM_RTCD_FLAGS "" //Arguments to pass to rtcd.pl. Separate with ';'
//#define CONFIG_AV1_DECODER 1 //Enable AV1 decoder.
//#define CONFIG_AV1_ENCODER 1 //Enable AV1 encoder.
#define CONFIG_BIG_ENDIAN 0 //Internal flag.
#define CONFIG_GCC 0 //Building with GCC (detect).
#define CONFIG_GCOV 0 //Enable gcov support.
#define CONFIG_GPROF 0 //Enable gprof support.
#define CONFIG_LIBYUV 0 //Enables libyuv scaling/conversion support.
#define CONFIG_LANCZOS_RESAMPLE 1 //Enables lanczos resize support.

#define CONFIG_MULTITHREAD 1 //Multithread support.
#define CONFIG_OS_SUPPORT 1 //Internal flag EXE.
#define CONFIG_PIC 0 //Build with PIC enabled.
#define CONFIG_RUNTIME_CPU_DETECT 1 //Runtime CPU detection support.
#define CONFIG_SHARED 0 //Build shared libs.
#define CONFIG_WEBM_IO 0 //Enables WebM support.

#define CONFIG_ENABLE_IBC_NAT 0 //Enables IBC for natural content //error
#define CONFIG_ENABLE_INLOOP_FILTER_GIBC 1
                   //Enables In-loop filters for key-frames with GIBC enabled
#define CONFIG_IBC_SUBPEL_PRECISION 1 //Allow sub-pel precision for intraBC block vector.

// Debugging flags.
#define CONFIG_DEBUG 0 //Enable debug-only code.
#define CONFIG_MISMATCH_DEBUG 0 //Mismatch debugging flag.
#define CONFIG_EXCLUDE_SIMD_MISMATCH 0
                   //Exclude mismatch in SIMD functions for testing/debugging.

// AV1 feature flags.

//#define CONFIG_ACCOUNTING 0 //Enables bit accounting.
#define CONFIG_ANALYZER 0 //Enables bit stream analyzer.
#define CONFIG_EXTRACT_PROTO 0
                   //Enables protobuf-based inspection tool.
#define CONFIG_COEFFICIENT_RANGE_CHECKING 0
                   //Coefficient range check.
#define CONFIG_DENOISE 1
                   //Denoise/noise modeling support in encoder.
//#define CONFIG_INSPECTION 0 //Enables bitstream inspection.
#define CONFIG_INTERNAL_STATS 0 //Enables internal encoder stats.
#define CONFIG_MAX_DECODE_PROFILE 2
                   //Max profile to support decoding.
#define CONFIG_NORMAL_TILE_MODE 0 //Only enables normal tile mode.
#define CONFIG_SIZE_LIMIT 1 //Limit max decode width/height.
#define CONFIG_SPATIAL_RESAMPLING 1 //Spatial resampling.
#define DECODE_HEIGHT_LIMIT 16384 //Set limit for decode height.
#define DECODE_WIDTH_LIMIT 16384 //Set limit for decode width.
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
#define CONFIG_AV1_TEMPORAL_DENOISING 0
                   //Build with temporal denoising support.
#define CONFIG_NN_V2 0 //Fully-connected neural nets ver.2.
#define CONFIG_OPTICAL_FLOW_API 0
                   //AV1 experiment flag for optical flow API.
#define CONFIG_AV2CTC_PSNR_PEAK 1
                   //Use AV2 CTC type PSNR peak for 10- and 12-bit
#define CONFIG_ZERO_OFFSET_BITUPSHIFT 1
                   //Use zero offset for non-normative bit upshift
#define CONFIG_PARAKIT_COLLECT_DATA 0
                   //enables data collection for ParaKit training.
#define CONFIG_CTX_V_AC_SIGN 1 //FG8-Test1.
#define CONFIG_CTX_MV_SHELL_OFFSET_OTHER 1 //FG8-Test3.

// AV2 experiment flags.

#define CONFIG_WIENERNS_9x9 1
  //AV2 non-separable 16-tap Wiener filter with enlarged 9x9 diamond shape
#define CONFIG_EXT_QUANT_UPD 1 //AV2 updated ext-quant.
#define CONFIG_COEFF_HR_LR1 1 //AV2 enable coding 1 LR in coefficient coding.
#define CONFIG_PARA_BD_REDUCE 1 //AV2 bitdepth reduction for PARA.
#define CONFIG_BYPASS_IMPROVEMENT 1 //AV2 enable entropy bypass improvement.
#define CONFIG_CDF_SCALE 1 //AV2 enable entropy cdf scaling improvement.
#define CONFIG_EOB_POS_LUMA 1 //EOB position coding for luma.
#define CONFIG_DIP 1 //AV2 intra data-driven prediction.
#define CONFIG_DIP_EXT 0 //AV2 DIP Improvements. //c++
#define CONFIG_DIP_EXT_PRUNING 0 //AV2 DIP TFLite pruning. //c++
#define CONFIG_ERP_TFLITE 0 //Build ERP with TFLite
#define CONFIG_TCQ 1 //AV2 trellis coded quantization flag"
#define CONFIG_TCQ_IMP 1 //AV2 TCQ improvement
#define CONFIG_COMPOUND_WARP_SAMPLES 1 //AV2 compound warped motion samples experiment flag
#define CONFIG_RELAX_AFFINE_CONSTRAINTS 1 //AV2 relax affine constraints
#define CONFIG_NEW_TX_PARTITION 1 //AV2 new transform partitions experiment flag.
#define CONFIG_4WAY_5WAY_TX_PARTITION 1 //AV2 4way+5way txfm partitions experiment flag.
#define CONFIG_INTER_COMPOUND_BY_JOINT 1 //AV2 inter compound mode by joint.
#define CONFIG_NO_JOINTMODE_WHEN_SAME_REFINDEX 1 //AV2 no joint mode when same ref index.
#define CONFIG_DISABLE_4X4_INTER 1 //Disable 4x4 inter blocks
#define CONFIG_16_FULL_SEARCH_DMVR 1
    //AV2 remove 2-stage search and early termination in neighbors for DMVR with 16-neighbor full search"
#define CONFIG_24_FULL_SEARCH_DMVR 1 //AV2 DMVR with 24-neighbor full search
#define CONFIG_IDIF 1 //AV2 experiment flag to enable Intra Directional Interpolation Filter.
#define CONFIG_ORIP_DC_DISABLED 0 //AV2 experiment flag to disable ORIP for DC mode.
#define CONFIG_ORIP_NONDC_DISABLED 0 //AV2 experiment flag to disable ORIP for non-DC modes.
#define CONFIG_MVP_IMPROVEMENT 1 //Enable MVP improvement
#define CONFIG_IBP_DC 1 //AV2 experiment flag to enable intra bi-prediction for DC mode.
#define CONFIG_WAIP 1 //AV2 wide angular intra prediction flag.
#define CONFIG_MORPH_PRED 1 //AV2 intra prediction mode flag.
#define CONFIG_IMPROVED_MORPH_PRED 1 //Improvement of AV2 linear intra prediction mode.
#define CONFIG_CONTEXT_DERIVATION 1 //AV2 experiment flag to enable modified context derivation : CWG-B065.

// End: CWG-C016
#define CONFIG_COMBINE_PC_NS_WIENER 1 //AV2 pixel-classified, frame-level, nonsep Wiener filter experiment flag
#define CONFIG_TEMP_LR 1 //AV2 experiment flag to use temporal LR when frame-level filter is on
#define CONFIG_COMBINE_PC_NS_WIENER_ADD 1 //Frame-level, nonsep Wiener filter for chroma experiment flag
#define CONFIG_RETRAIN_PC_WIENER 1 //Precision Adjustment and Retraining for RESTORE_PC_WIENER flag
#define CONFIG_IMPROVED_DS_CC_WIENER 1 //AV2 improved luma downsampling for cross-plane non-sep wiener filter
#define CONFIG_REMOVE_SIX_TAP_DS_CROSS_LR 1
    //Replace the six-tap luma downsampling filter with a two-tap filter for cross-plane non-sep wiener filter

// CWG-D178
#define CONFIG_LOSSLESS_DPCM 1 //AV2 enable DPCM and FSC for lossless coding mode
#define CONFIG_IMPROVE_LOSSLESS_TXM 1
    //AV2 enable 4x4 IDTX for inter blocks and 8x8 IDTX for all blocks in lossless mode (luma only)
#define CONFIG_ACROSS_SCALE_TPL_MVS 0
  //AV2 experiment flag to enable across scale temporal mv projection
#define CONFIG_ACROSS_SCALE_WARP 0 //AV2 experiment flag to enable across scale warp modes.
#define CONFIG_ACROSS_SCALE_REFINEMV 0 //AV2 experiment flag to enable across scale refinemv modes.

// CWG-E210
#define CONFIG_RECT_CTX 1 //AV2 enable Context Reduction for Rectangle Partition Type

// Source of throughput analysis : CWG-B065
#define CONFIG_THROUGHPUT_ANALYSIS 0 //AV2 experiment flag to measure throughput.
#define CONFIG_IBC_SR_EXT 2 //Enables IntraBC search range extension
#define CONFIG_IBC_BV_IMPROVEMENT 1 //Enables BV improvements for IBC
#define CONFIG_IBC_MAX_DRL 1 //Enables Max DRL index signaling for IntraBC
#define CONFIG_PALETTE_IMPROVEMENTS 1 //AV2 experiment flag for palette parsing independency and improved palette color map coding.
#define CONFIG_PALETTE_LINE_COPY 1 //AV2 experiment flag to enable palette line copy and transverse coding, CWG-D114.
#define CONFIG_SKIP_MODE_SSE_BUG_FIX 1 //AV2 experiment flag to fix the SSE calc bug for skip mode.
#define CONFIG_SKIP_MODE_ENHANCEMENT 1 //AV2 experiment flag to enable skip mode enhancement.
#define CONFIG_SKIP_MODE_PARSING_DEPENDENCY_REMOVAL 1 //AV2 experiment flag to remove parsing dependency of skip mode.
#define CONFIG_OUTPUT_FRAME_BASED_ON_ORDER_HINT_ENHANCEMENT 1 //Enable enhanced frame output order derivation
#define CONFIG_REF_LIST_DERIVATION_FOR_TEMPORAL_SCALABILITY 1 //Enable temporal scalability
#define CONFIG_SAME_REF_COMPOUND 1 //Allow compound mode to refer to the same reference frame
#define CONFIG_DISPLAY_ORDER_HINT_FIX 1 //Bug fix on display order hints of key frames
#define CONFIG_BAWP_FIX_DIVISION_16x16_MC 1
    //Fix BAWP from CWG-E245, only use up to 16 samples from left and above and remove division using 8, 16 , or 32 samples"
#define CONFIG_BAWP_ACROSS_SCALES 0 //Enable BAWP across scales prediction
#define CONFIG_IMPROVED_INTRA_DIR_PRED 1 //Improved intra prediction
#define CONFIG_D071_IMP_MSK_BLD 1 //Enable single reference mode for frame boundary
#define CONFIG_SKIP_TXFM_OPT 1 //Enable to optimize the signaling of skip_txfm
#define CONFIG_REFINEMV 1 //Enable refinemv modes
#define CONFIG_EXPLICIT_TEMPORAL_DIST_CALC 1 //Enable to explicit temporal distance calculation
#define CONFIG_PRIMARY_REF_FRAME_OPT 1 //Enable the primary reference frame improvement
#define CONFIG_IMPROVED_GLOBAL_MOTION 1 //New global motion syntax for AV2
#define CONFIG_SEP_COMP_DRL 1 //Use separate drl list for compound modes
#define CONFIG_SKIP_ME_FOR_OPFL_MODES 1 //Reuse the mvs of compound mode from non-opfl path
#define CONFIG_E191_OFS_PRED_RES_HANDLE 1 //Enable outside frame boundary block handling
#define CONFIG_EXT_WARP_FILTER 1 //Enable extended warp filter
#define CONFIG_IMPROVE_EXT_WARP 1 //Enable ext warp filter for chroma blocks with bw=4 or bh=4
#define CONFIG_ADST_TUNED 1 //AV2 experiment to replace the ADST 4, 8 and 16 basis
#define CONFIG_EXTRA_DPB 1 //Use extra dpb
#define CONFIG_CORE_TX 1 //AV2 core transform
#define CONFIG_TX_PARTITION_CTX 1 //Enable to optimize txfm partition context
#define CONFIG_BUGFIX_TX_PARTITION_TYPE_SIGNALING 1 //TX partition type signalling bugfix")
#define CONFIG_INTERINTRA_IMPROVEMENT 1 //Enable additional inter-intra block sizes
#define CONFIG_REFRESH_FLAG 0 //Experiment flag to Signal refresh frame flag with 3 bits
#define CONFIG_D149_CTX_MODELING_OPT 1 //Enable to optimize block size dependent context modeling
#define CONFIG_COMPOUND_WARP_CAUSAL 0 //AV2 experiment flag to enable compound new_newmv warp_causal mode
#define CONFIG_DSMVP_REFBANK_MV_SWAP 1 //Swap ref bank mv and derived smvp in DRL generation
#define CONFIG_D072_SKIP_MODE_IMPROVE 1 //Enable to improve skip mode
#define CONFIG_SKIP_MODE_NO_REFINEMENTS 1 //Disable refinement algorithms for the skip mode
#define CONFIG_MF_HOLE_FILL_ALWAYS_ENABLE 1 //Always enable motion field hole filling
#define CONFIG_TX_TYPE_FLEX_IMPROVE 1 //Enable transform type flexbility improvement for large transform blocks
#define CONFIG_CWG_E099_DRL_WRL_SIMPLIFY 1 //Enable the simplification of DRL and WRL
#define CONFIG_BANK_IMPROVE 1 //Enable to improve refmv bank and warp parameter bank
#define CONFIG_PARTITION_CONTEXT_REDUCE 1 //Enable to reduce partition contexts
#define CONFIG_CCSO_FU_BUGFIX 1 //Enable CCSO improvements
#define CONFIG_OPT_INTER_MODE_CTX 1 //Improvement of all inter mode related contexts
#define CONFIG_WRL_PRUNE_FOUR_PARAMETERS 1 //Enable WRL only prune four non-translational parameters
#define CONFIG_KEY_OVERLAY 1 //Enable to support the key overlay frame
#define CONFIG_DRL_REORDER_CONTROL 1 //Enable to have a flag to turn on and off DRL reorder
#define CONFIG_OPFL_CTX_OPT 1 //Enable optimization of the CDFs for use optflow flag
#define CONFIG_CDEF_ENHANCEMENTS 1 //Enable the optimization of CDEF strengths
#define CONFIG_MVD_CDF_REDUCTION 1 //Enable reduction of the CDFs for MVD related sybmols
#define CONFIG_TMVP_SIMPLIFICATIONS_F085 1 //Improvements to TMVP & MV trajectory
#define CONFIG_TMVP_SIMPLIFICATION 1 //Enable to reduce the number of TMVP candidates
#define CONFIG_IMPROVE_TIP_SMVP 1 //Improve SMVP with TIP (F043)
#define CONFIG_MRLS_IMPROVE 1
  //Enable MRLS improvement to enable two reference lines for intra prediction
#define CONFIG_TMVP_MV_COMPRESSION 1 //Enable to compress TMVP MV
#define CONFIG_DRL_WRL_LINE_BUFFER_REDUCTION 1
  //Reduce the line buffer size for DRL and WRL. The access unit is changed from 4x4 to 8x8

// This is an encode-only change.
#define CONFIG_ENABLE_SR 0 //Enable super resolution mode
#define CONFIG_TIP_MV_SIMPLIFICATION 1 //Enable to address the motion field range issue in TIP
#define CONFIG_MV_SEARCH_RANGE 1 //Enable a sufficient MV search range.
#define CONFIG_LARGE_TF_BLOCK 1 //Enable large adaptively selected temporal filter blocks.
#define CONFIG_FIX_CDEF_SYNTAX 1 //AV2 experiment flag to fix CDEF syntax.
#define CONFIG_IMPROVED_CFL 1 //Enable improved CfL mode from CWG-C044
#define CONFIG_CFL_64x64 1 //Enable cross-component prediction modes up to luma size 64x64
#define CONFIG_CFL_SIMPLIFICATION 1 //Fix issues of CfL
#define CONFIG_BLEND_MODE 1 //Enable improved intra blend mode from CWG-D046
#define CONFIG_ENABLE_MHCCP 1 //Enable multi hypothesis cross component prediction
#define CONFIG_E125_MHCCP_SIMPLIFY 1 //Simplify the parameter derivation for MHCCP
#define CONFIG_E149_MHCCP_4PARA 1 //Using 4 parameters in MHCCP
#define CONFIG_MHCCP_SB_BOUNDARY 1 //Using only 1 lines when MHCCP block at the superblock top boundary
#define MHCCP_BUFFER_4LINES 1 //Using 2 lines for the chroma reference region in MHCCP
#define CONFIG_MHCCP_GAUSSIAN 1 //Fix gaussian elimination precision for MHCCP
#define MHCCP_3_PARAMETERS 1 //Using 3 parameters in MHCCP
#define CONFIG_C071_SUBBLK_WARPMV 1 //AV2 experiment flag to use subblock warp MV for SMVP
#define CONFIG_C076_INTER_MOD_CTX 1 //AV2 experiment flag to simplify inter mode contexts
#define CONFIG_NEW_CONTEXT_MODELING 1 //Enable to improve the context modeling
#define CONFIG_WEDGE_MOD_EXT 1 //AV2 wedge modes extensions.

#define CONFIG_INTER_DDT 1 //Use data driven transform to replace ADST for inter.
#define CONFIG_MF_IMPROVEMENT 1 //Enable to improve temporal motion projection
#define CONFIG_MVP_SIMPLIFY 1 //Enable to simplify MVP list construction
#define CONFIG_BRU 1 //enable BRU update

// IST for sub-TU partitions from CWG-E151
#define CONFIG_IST_NON_ZERO_DEPTH 1
                    //Enable IST non zero depth TUs from CWG-E151.
// IST set reduction (non-normative)
#define CONFIG_IST_REDUCTION 1
                    //Non-normatively use 4 sets for IST encoder search from CWG-E142.
#define CONFIG_TMVP_IMPROVE 1 //Enable to improve TMVP candidate selection in DRL list
#define CONFIG_COMPOUND_4XN 1 //Enable compound modes for 4xN/Nx4 blocks
#define CONFIG_TILE_CDFS_AVG_TO_FRAME 1
                   //Average the CDFs from tiles for frame CDF
#define CONFIG_LC_REF_MV_BANK 1
                   //Enable low complexity refmv bank design
#define CONFIG_FRAME_HEADER_SIGNAL_OPT 1
  //Enable the signaling optimization for certain frame header syntax elements
#define CONFIG_TIP_LD 1 //Enable TIP for low delay
#define CONFIG_TIP_ENHANCEMENT 1 //Enable different weighted prediction for TIP
#define CONFIG_REDESIGN_WARP_MODES_SIGNALING_FLOW 0
                   //Enable the redesign of warp modes signaling flow
#define CONFIG_PALETTE_THREE_NEIGHBOR 1
                   //Derive palette context using three neighbors

// This is an encode-only change.
#define CONFIG_MOTION_MODE_RD_PRUNE 1 //Enable fast motion mode search
#define CONFIG_OPFL_MV_SEARCH 1 //Optical flow based MV search
#define CONFIG_MRSSE 0 //Enable MRSSE
#define CONFIG_IST_SET_FLAG 1 //AV2 experiment flag to signal Secondary Tx set ID.
#define CONFIG_SCC_DETERMINATION 1 //Enable the screen content tools determination improvement.
#define CONFIG_IST_ANY_SET 1 //Enable R-D Optimized IST set selection from CWG-D159.
#define CONFIG_CCSO_BO_REDUCE 1 //Reduce band only 128 case.
#define CONFIG_TX_SKIP_FLAG_MODE_DEP_CTX 1 //CWG-D086: Context modeling for transform block zero flag signaling
#define CONFIG_TIP_IMPLICIT_QUANT 1 //Enable implicit quantization derivation for TIP direct mode
#define CONFIG_REFINED_MVS_IN_TMVP 1 //Keep optical flow refined MVs in TMVP list.
#define CONFIG_IMPROVE_REFINED_MV 1 //Keep refined MVs in TMVP list for TIP modes.
#define CONFIG_IMPROVE_TMVP_LIST 1 //Improve TMVP list population.
#define CONFIG_AFFINE_REFINEMENT 1 //Decoder side affine motion refinement.
#define CONFIG_AFFINE_REFINEMENT_SB 1 //Subblock based affine refinement
#define CONFIG_LF_SUB_PU 1 //AV2 enable LF on sub blocks
#define CONFIG_ASYM_DF 1 //Enable asymmetric DF
#define CONFIG_DERIVED_MVD_SIGN 1 //Enable MVD sign derivations
#define CONFIG_VQ_MVD_CODING 1 //Enable VQ based MVD coding
#define CONFIG_OPFL_BI 1 //Enable bilinear for initial opfl.
#define CONFIG_SUBBLK_REF_EXT 1 //Enable extension for subblock MV refinement.
#define CONFIG_SUBBLK_REF_DS 1 //Enable sad downsampling for subblock MV refinement.
#define CONFIG_CHROMA_TX 1 //Sequence-level flag to use only DCT for chroma.
#define CONFIG_SUBBLK_PAD 1 //Enable subblock padding for subblock mv refinement.
#define CONFIG_ML_PART_SPLIT 0 //Partition SPLIT pruning/forcing as predicted by ML. //part_split_prune_tflite.cc
#define CONFIG_F105_IST_MEM_REDUCE 1 //Prune ADST_ADST IST sets for 8x8 intra blocks
#define CONFIG_E124_IST_REDUCE_METHOD1 0 //AV2 IST reduction.
#define CONFIG_E124_IST_REDUCE_METHOD4 1 //AV2 remove worst-case multiplications.
#define CONFIG_COEFF_HR_ADAPTIVE 1 //AV2 enable adaptive coding of HR coefficients.
#define CONFIG_IBP_WEIGHT 0 //Reduce IBP weights memory. //error
#define CONFIG_FIX_IBP_DC 1 //Simplify prediction of IBP DC mode.
#define CONFIG_WEDGE_TMVP 1 //Improvements to tmvp MV storing for wedge mode.
#define CONFIG_TMVP_MEM_OPT 1 //Enables directional storage and adaptive sampling for TMVP MVs.
#define CONFIG_MV_TRAJECTORY 1 //Enables TMVP MV trajectory tracking.
#define CONFIG_ADAPTATION_RATE_IMPROVE 1 //Enables adaptation rate improvement with CDF propagation.
#define CONFIG_ENHANCED_FRAME_CONTEXT_INIT 1
  //Enables improved frame context initialization with frame averaging.
#define CONFIG_PRIMARY_QP_FIRST 1 //Improvements to the primary reference frame by selecting the closet QP.
#define CONFIG_IMPROVED_SECONDARY_REFERENCE 1
  //Enables improved secondary reference frame derivation for frame context initialization.
#define CONFIG_FIX_INTER_DDT_PRECISION 1 //Fix precision of inter DDT.
#define CONFIG_WEDGE_SIMPL 1 //Wedge mode simplificaitons.
#define CONFIG_WARP_PRECISION 1 //Enable precisions of warp models.
#define CONFIG_SIX_PARAM_WARP_DELTA 1 //Enable six parameter warp models.
#define CONFIG_REORDER_SIX_PARAM_DELTA 1 //Enable to six parameter warp models signal when warp_ref_idx is 1.
#define CONFIG_WARP_INTER_INTRA 1 //Enable inter-intra mode for warp block.
#define CONFIG_OPFL_MEMBW_REDUCTION 1 //Reduce memory bandwith for OPFL/subblk ref/DAMR to 15x15.
#define CONFIG_WARP_BD_BOX 1 //4x4 warp constraints.
#define CONFIG_DISABLE_4X4_IBP_ORIP 1 //Disable 4x4 for IBP/ORIP.
#define CONFIG_DF_PAR_BITS 1 //Flexible control of deblocking parameter bits.
#define CONFIG_NEW_PART_CTX 1 //New partition context models
#define CONFIG_EXT_SEG 1 //Extend the maximum number of segments to 16, CWG-F069.
#define CONFIG_DELTAQ_OPT 1 //Enable delta-q entropy coding optimization.
#define CONFIG_TX_PARTITION_RESTRICT 1 //Disallow transform partition for large coding blocks.
#define CONFIG_INTER_MODE_CONSOLIDATION 1 //Inter modes redesign and consolidation.
#define CONFIG_F054_PIC_BOUNDARY 1
  //Alignment of ref picture to be integer multiples of 8 for loop filter and inter prediction
#define CONFIG_GDF 1 //Enable guided detail filter.
#define CONFIG_REDUCE_SYMBOL_SIZE 1 //Symbol size reduction from 16 to 8.
#define CONFIG_DAMR_CLEAN_UP 1 //Clean up DAMR memory bandwith issue.
#define CONFIG_ADJ_Q_OFFSET 1 //Encoder-only config to adjust qp offsets.

#define INLINE inline

#endif  // AOM_CONFIG_H_
