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
#ifndef AOM_CONFIG_H_
#define AOM_CONFIG_H_


#define ARCH_ARM 0
#define ARCH_MIPS 0
#define ARCH_PPC 0
#if defined(__X86_64__)
#define ARCH_X86 0
#define ARCH_X86_64 1
#else
#define ARCH_X86 1
#define ARCH_X86_64 0
#endif
//#define CONFIG_2PASS_PARTITION_SEARCH_LVL 1
//#define CONFIG_ACCOUNTING 1
#define CONFIG_ANALYZER 0
//#define CONFIG_AV1_DECODER 1
//#define CONFIG_AV1_ENCODER 1
#define CONFIG_AV1_HIGHBITDEPTH 1
#define CONFIG_AV1_TEMPORAL_DENOISING 0 // cmake encoder av1_temporal_denoiser.c
#define CONFIG_BIG_ENDIAN 0
#define CONFIG_BITRATE_ACCURACY 0
#define CONFIG_RATECTRL_LOG 0
#define CONFIG_BITSTREAM_DEBUG 0
#define CONFIG_COEFFICIENT_RANGE_CHECKING 0 // printf
//#define CONFIG_COLLECT_INTER_MODE_RD_STATS 0
#define CONFIG_COLLECT_COMPONENT_TIMING 0
#define CONFIG_COLLECT_PARTITION_STATS 0
#define CONFIG_COLLECT_RD_STATS 0
#define CONFIG_CWG_C013 0
#define CONFIG_DEBUG 0 // printf
#define CONFIG_DENOISE 0 // cmake encoder
#define CONFIG_DISABLE_FULL_PIXEL_SPLIT_8X8 0 // cmake
#define CONFIG_ENTROPY_STATS 0
#define CONFIG_EXCLUDE_SIMD_MISMATCH 1
//#define CONFIG_FRAME_PARALLEL_ENCODE 1
//#define CONFIG_FRAME_PARALLEL_ENCODE_2 1
//#define CONFIG_FILEOPTIONS 1
//#define CONFIG_FIX_GF_LENGTH 1
//#define CONFIG_FP_MB_STATS 0
#define CONFIG_GCC 1
#define CONFIG_GCOV 0
//#define CONFIG_GLOBAL_MOTION_SEARCH 1
#define CONFIG_GPROF 0
//#define CONFIG_INSPECTION 1
#define CONFIG_INTER_STATS_ONLY 0
//#define CONFIG_LIBYUV 0
//#define CONFIG_LOWBITDEPTH 0
#define CONFIG_LPF_MASK 0 // cmake common loopfiltermask.c, error with libwebp2
#define CONFIG_MAX_DECODE_PROFILE 2
#define CONFIG_MISMATCH_DEBUG 0
#define CONFIG_MULTITHREAD 1
#define CONFIG_NN_V2 0
#define CONFIG_NORMAL_TILE_MODE 1 // cmake decoder obu.c
//#define CONFIG_ONE_PASS_SVM 0
#define CONFIG_OPTICAL_FLOW_API 0
#define CONFIG_OS_SUPPORT 1
#define CONFIG_PARTITION_SEARCH_ORDER 0
#define CONFIG_RD_DEBUG 0
//#define CONFIG_REDUCED_ENCODER_BORDER 0
#define CONFIG_REALTIME_ONLY 0
#define CONFIG_RT_ML_PARTITIONING 0
#define CONFIG_RUNTIME_CPU_DETECT 1
#define CONFIG_SHARED 0
#define CONFIG_SALIENCY_MAP 1
#define CONFIG_SIZE_LIMIT 1
#define CONFIG_SPATIAL_RESAMPLING 1 // cmake
#define CONFIG_SPEED_STATS 0
#define CONFIG_TFLITE 0
#define CONFIG_THREE_PASS 0
#define CONFIG_OUTPUT_FRAME_SIZE 0
//#define CONFIG_STATIC 1
//#define CONFIG_TUNE_BUTTERAUGLI 1
//#define CONFIG_TUNE_VMAF 0
//#define CONFIG_USE_VMAF_RC 0
#define CONFIG_WEBM_IO 0
#define DECODE_HEIGHT_LIMIT 16384
#define DECODE_WIDTH_LIMIT 16384
#define FORCE_HIGHBITDEPTH_DECODING 1
#define HAVE_DSPR2 0
#define HAVE_FEXCEPT 1
#define HAVE_MIPS32 0
#define HAVE_MIPS64 0
#define HAVE_MSA 0
#define HAVE_NEON 0
#define HAVE_PTHREAD_H 0
#if HAVE_SIMD
#define CONFIG_INTERNAL_STATS 0
#if defined(__X86_64__)
#define CONFIG_PIC 1
#endif
#define HAVE_MMX 1
#define HAVE_SSE 1
#define HAVE_SSE2 1
#define HAVE_SSE3 1
#define HAVE_SSSE3 1
#define HAVE_SSE4_1 1
#define HAVE_SSE4_2 1
#define HAVE_AVX 1
#if defined(__AVX2__)
#define HAVE_AVX2 1
#endif
#else
#define CONFIG_INTERNAL_STATS 0
#define CONFIG_PIC 0
#endif
#define HAVE_UNISTD_H 0
#define HAVE_VSX 0
#define HAVE_WXWIDGETS 0
#define INLINE inline
//#define STATIC_LINK_JXL 0
#endif  // AOM_CONFIG_H_
