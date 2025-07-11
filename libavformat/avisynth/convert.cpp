// Avisynth v2.5.  Copyright 2002-2009 Ben Rudiak-Gould et al.
// http://avisynth.nl

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
// http://www.gnu.org/copyleft/gpl.html .
//
// Linking Avisynth statically or dynamically with other modules is making a
// combined work based on Avisynth.  Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Avisynth give you
// permission to link Avisynth with independent modules that communicate with
// Avisynth solely through the interfaces defined in avisynth.h, regardless of the license
// terms of these independent modules, and to copy and distribute the
// resulting combined work under terms of your choice, provided that
// every copy of the combined work is accompanied by a complete copy of
// the source code of Avisynth (the version of Avisynth used to produce the
// combined work), being distributed under the terms of the GNU General
// Public License plus this exception.  An independent module is a module
// which is not derived from or based on Avisynth, such as 3rd-party filters,
// import and export plugins, or graphical user interfaces.


#include "convert.h"
#include "convert_matrix.h"
#include "convert_helper.h"
#include "convert_bits.h"
#include "convert_planar.h"
#include "convert_rgb.h"
#include "convert_yuy2.h"
//#include "plugins/ConvertStacked.cpp"
//#include "plugins/directshow_source.cpp"
//#include "plugins/deflicker.cpp"
//#include "plugins/maxcllfind.cpp"
//#include "plugins/perfpan.cpp"
//#include "plugins/perfpan_impl.cpp"
//#include "plugins/Frfun7.cpp"
//#include "plugins/ColorMatrixTransform.cpp"
//#include "plugins/FillBorders.cpp"
//#include "plugins/dedup.cpp"
//#include "plugins/depan_compensate.cpp"
//#include "plugins/depan_interface.cpp"
//#include "plugins/depan_interpolate.cpp"
//#include "plugins/depan_scenes.cpp"
//#include "plugins/depan_stabilize.cpp"
//#include "plugins/depan_transform.cpp"
//#include "plugins/depanestimate.cpp"
//#include "plugins/estimate_fftw.cpp"
//#include "plugins/depanio.cpp"
//#include "plugins/info.cpp"
//#include "plugins/yuy2planes.cpp"*/
//#include "plugins/soxfilter.cpp"
#include "plugins/audioboost/AudioBoost.cpp"
#include "plugins/avsresize/avsresize.cpp"
#include "plugins/descratch/descratch.cpp"
#include "plugins/knlmeanscl/NLMAvisynth.cpp"
#include "plugins/knlmeanscl/NLMKernel.cpp"
#include "plugins/knlmeanscl/shared/startchar.cpp"
#include "plugins/knlmeanscl/shared/ocl_utils.cpp"
#include "plugins/knlmeanscl/shared/common.cpp"
#include "plugins/waveform/waveform.cpp"
//#include "plugins/amDCT/amDCT.cpp"
//#include "plugins/tp7_pinterf/average.cpp"
//#include "plugins/tp7_pinterf/average_avx.cpp"
//#include "plugins/tp7_pinterf/medianblur.cpp"
//#include "plugins/tp7_pinterf/medianblur_sse2.cpp"
#include "plugins/removegrain/repair.cpp"
#include "plugins/removegrain/clense.cpp"
#include "plugins/removegrain/removegrain.cpp"
//#include "plugins/tp7_pinterf/RemoveGrainT.cpp"
//#include "plugins/tp7_pinterf/vertical_cleaner.cpp"

//#include "plugins/tp7_pinterf/RemoveGrainHD.cpp"
//#include "plugins/tp7_pinterf/planar.cpp"
//#include "plugins/vsfilter/EEDI3CL.cpp"
//#include "plugins/EEDI3/eedi3.cpp"
//#include "plugins/EEDI3/eedi3sse.cpp"
//#include "plugins/EEDI3/nnedi3.cpp" //asm
//#include "plugins/jpsdr/PlanarFrame.cpp"
//#include "plugins/jpsdr/HDRTools.cpp" //asm
//#include "plugins/jpsdr/MatrixClass.cpp"
//#include "plugins/jpsdr/TransferFunctions.cpp"
//#include "plugins/jpsdr/ThreadPool.cpp"
//#//include "plugins/jpsdr/ThreadPoolInterface.cpp"
#if defined(__AVX2__)
//#include "plugins/vsfilter/EEDI3CL_AVX2.cpp "
//#include "plugins/vsfilter/EEDI3CL_SSE2.cpp "
//#include "plugins/tp7_pinterf/average_avx2.cpp"
//#include "plugins/tp7_pinterf/medianblur_avx2.cpp"
#include "plugins/removegrain/removegrain_avx2.cpp"
#endif

#include "plugins/audio_output.cpp"
#include "plugins/lsmashsource.cpp"
#include "plugins/video_output.cpp"
#include "plugins/libavsmash_source.cpp"
#include "plugins/lwlibav_source.cpp"

#include "plugins/AddGrainC/AddGrain.cpp"
#include "plugins/RemoveDirt/RemoveDirt.cpp"
#include "plugins/RemoveDirt/common.cpp"
#include "plugins/RemoveDirt/cpufeatures.cpp"
//#include "plugins/neo_FFT3D/main.cpp"
//#include "plugins/FFT3DFilter/FFT3DFilter.cpp"
//#include "plugins/SubtitleEX/Antialiaser.cpp"
//#include "plugins/SubtitleEX/SubtitleEx.cpp"
#include "plugins/FastBlur/fastblur.cpp"
#include "plugins/FastBlur/pyramid.cpp"
#include "plugins/FastBlur/pyramidavs.cpp"
#include "plugins/FastBlur/threadpool.cpp"
#include "plugins/TIVTC/TFM.cpp"
#include "plugins/TIVTC/TDecimate.cpp"
#include "plugins/TIVTC/MergeHints.cpp"
#include "plugins/TIVTC/FieldDiff.cpp"
#include "plugins/TIVTC/FrameDiff.cpp"
#include "plugins/TIVTC/IsCombedTIVTC.cpp"
#include "plugins/TIVTC/RequestLinear.cpp"
#include "plugins/TDeint/TDeinterlace.cpp"
#include "plugins/TDeint/TSwitch.cpp"
//#include "plugins/RIFE/plugin.cpp"
//#include "plugins/RIFE/rife.cpp"
//#include "plugins/RIFE/warp.cpp"

#include "plugins/vsfilter/vsTTempSmooth.cpp"
#include "plugins/vsfilter/simd/vsTTempSmooth_SSE2.cpp"
#include "plugins/vsfilter/BinaryMM.cpp"
#include "plugins/vsfilter/BuldMM.cpp"
#include "plugins/vsfilter/CreateMM.cpp"
#include "plugins/vsfilter/vsTMM.cpp"
#include "plugins/vsfilter/simd/vsTMM_SSE2.cpp"
#include "plugins/vsfilter/vsTEdgeMask.cpp"
#include "plugins/vsfilter/simd/vsTEdgeMask_SSE2.cpp"
#include "plugins/vsfilter/vsTCanny.cpp"
#include "plugins/vsfilter/simd/vsTCanny_SSE2.cpp"
#include "plugins/vsfilter/vsTBilateral.cpp"
#include "plugins/vsfilter/vsMSmooth.cpp"
#include "plugins/vsfilter/vsLGhost.cpp"
#include "plugins/vsfilter/simd/vsLGhost_SSE2.cpp"
#include "plugins/vsfilter/vsDeGrainMedian.cpp"
#include "plugins/vsfilter/simd/vsDeGrainMedian_SSE2.cpp"
#include "plugins/vsfilter/vsDeblockPP7.cpp"
#include "plugins/vsfilter/vsCnr2.cpp"
//#include "plugins/vsfilter/FFTSpectrum.cpp"
#include "plugins/vsfilter/JincResize.cpp"
#include "plugins/vsfilter/simd/resize_plane_sse41.cpp"
#if defined(__AVX2__)
#include "plugins/vsfilter/simd/vsLGhost_AVX2.cpp"
#include "plugins/vsfilter/simd/vsTCanny_AVX2.cpp"
#include "plugins/vsfilter/simd/vsTEdgeMask_AVX2.cpp"
#include "plugins/vsfilter/simd/vsTMM_AVX2.cpp"
#include "plugins/vsfilter/simd/vsTTempSmooth_AVX2.cpp"
#include "plugins/vsfilter/simd/vsTTempSmooth_pmode1_AVX2.cpp"
#include "plugins/vsfilter/simd/resize_plane_avx2.cpp"
#endif
#include "plugins/vsfilter/simd/VCL2/instrset_detect.cpp"

//#include "plugins/Deathray2/deathray.cpp"
#if HAVE_RESAMPLEMT
#include "plugins/ResampleMT/resample.cc"
//#include "plugins/ResampleMT/resample_sse.cc"
//#include "plugins/ResampleMT/MatrixClass.cc"
#endif
//#include "plugins/bestsource/avisynth.cpp"
//#include "plugins/audiosource.cpp"
//#include "plugins/bsshared.cpp"
//#include "plugins/synthshared.cpp"
//#include "plugins/videosource.cpp"

#ifdef INTEL_INTRINSICS
#include "intel/convert_sse.h"
#include "intel/convert_yuy2_sse.h"
#endif

#include "avs/alignment.h"
#include "avs/minmax.h"
#include "avs/config.h"
#include <tuple>
#include <map>
#include <algorithm>

#ifdef AVS_WINDOWS
#include "avs/win.h"
#else
#include "avs/posix.h"
#endif

/********************************************************************
***** Declare index of new filters for Avisynth's filter engine *****
********************************************************************/

extern const AVSFunction Convert_filters[] = {       // matrix can be "rec601", "rec709", "PC.601" or "PC.709" or "rec2020" or "PC.2020"
  { "ConvertToRGB",   BUILTIN_FUNC_PREFIX, "c[matrix]s[interlaced]b[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToRGB::Create, (void *)0 },
  { "ConvertToRGB24", BUILTIN_FUNC_PREFIX, "c[matrix]s[interlaced]b[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToRGB::Create, (void *)24 },
  { "ConvertToRGB32", BUILTIN_FUNC_PREFIX, "c[matrix]s[interlaced]b[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToRGB::Create, (void *)32 },
  { "ConvertToRGB48", BUILTIN_FUNC_PREFIX, "c[matrix]s[interlaced]b[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToRGB::Create, (void *)48 },
  { "ConvertToRGB64", BUILTIN_FUNC_PREFIX, "c[matrix]s[interlaced]b[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToRGB::Create, (void *)64 },
  { "ConvertToPlanarRGB",  BUILTIN_FUNC_PREFIX, "c[matrix]s[interlaced]b[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToRGB::Create, (void *)-1 },
  { "ConvertToPlanarRGBA", BUILTIN_FUNC_PREFIX, "c[matrix]s[interlaced]b[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToRGB::Create, (void *)-2 },
  { "ConvertToY8",    BUILTIN_FUNC_PREFIX, "c[matrix]s", ConvertToY::Create, (void*)0 }, // user_data == 0 -> only 8 bit sources
  { "ConvertToYV12",  BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[ChromaOutPlacement]s[param1]f[param2]f[param3]f", ConvertToYV12::Create, (void*)0 },
  { "ConvertToYV24",  BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV444, (void*)0},
  { "ConvertToYV16",  BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[ChromaOutPlacement]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV422, (void*)0},
  { "ConvertToYV411", BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYV411, (void*)0},
  { "ConvertToYUY2",  BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToYUY2::Create },
  { "ConvertBackToYUY2", BUILTIN_FUNC_PREFIX, "c[matrix]s", ConvertBackToYUY2::Create },
  { "ConvertToY",       BUILTIN_FUNC_PREFIX, "c[matrix]s", ConvertToY::Create, (void*)1 }, // user_data == 1 -> any bit depth sources
  { "ConvertToYUV411", BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYV411, (void*)1}, // alias for ConvertToYV411, 8 bit check later
  { "ConvertToYUV420",  BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[ChromaOutPlacement]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV420, (void*)1},
  { "ConvertToYUV422",  BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[ChromaOutPlacement]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV422, (void*)1},
  { "ConvertToYUV444",  BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV444, (void*)1},
  { "ConvertToYUVA420", BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[ChromaOutPlacement]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV420, (void*)2},
  { "ConvertToYUVA422", BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[ChromaOutPlacement]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV422, (void*)2},
  { "ConvertToYUVA444", BUILTIN_FUNC_PREFIX, "c[interlaced]b[matrix]s[ChromaInPlacement]s[chromaresample]s[param1]f[param2]f[param3]f", ConvertToPlanarGeneric::CreateYUV444, (void*)2},
  { "ConvertTo8bit",  BUILTIN_FUNC_PREFIX, "c[bits]i[truerange]b[dither]i[dither_bits]i[fulls]b[fulld]b", ConvertBits::Create, (void *)8 },
  { "ConvertTo16bit", BUILTIN_FUNC_PREFIX, "c[bits]i[truerange]b[dither]i[dither_bits]i[fulls]b[fulld]b", ConvertBits::Create, (void *)16 },
  { "ConvertToFloat", BUILTIN_FUNC_PREFIX, "c[bits]i[truerange]b[dither]i[dither_bits]i[fulls]b[fulld]b", ConvertBits::Create, (void *)32 },
  { "ConvertBits",    BUILTIN_FUNC_PREFIX, "c[bits]i[truerange]b[dither]i[dither_bits]i[fulls]b[fulld]b", ConvertBits::Create, (void *)0 },
  { "AddAlphaPlane",  BUILTIN_FUNC_PREFIX, "c[mask].", AddAlphaPlane::Create},
  { "RemoveAlphaPlane",  BUILTIN_FUNC_PREFIX, "c", RemoveAlphaPlane::Create},

  { "AudioBoost", BUILTIN_FUNC_PREFIX, "c[boost]f[limit]f[curve]i[norm]b", Create_AudioBoost, (void *)0 },
  //{ "deathray2", BUILTIN_FUNC_PREFIX, "c[hY]f[hUV]f[tY]i[tUV]i[s]f[x]i[l]b[c]b[b]b[a]i", CreateDeathray, (void *)0 },
  { "descratch", BUILTIN_FUNC_PREFIX, "c[mindif]i[asym]i[maxgap]i[maxwidth]i[minlen]i[maxlen]i[maxangle]f[blurlen]i[keep]i[border]i[modeY]i[modeU]i[modeV]i[mindifUV]i[mark]b[minwidth]i[left]i[right]i", Create_DeScratch, (void *)0 },
  { "KNLMeansCL", BUILTIN_FUNC_PREFIX, "c[d]i[a]i[s]i[h]f[channels]s[wmode]i[wref]f[rclip]c[device_type]s[device_id]i[ocl_x]i[ocl_y]i[ocl_r]i[stacked]b[info]b[mode_9_to_15bits]i", AviSynthPluginCreate, (void *)0 },
  { "Waveform", BUILTIN_FUNC_PREFIX, "c[window]f[height]f[zoom]f[under]b[smooth]b[aa]b", CreateWaveform, (void *)0 },
  { "GetAudio", BUILTIN_FUNC_PREFIX, "c", CreateGetAudio, (void *)0 },
  { "SCSelect", BUILTIN_FUNC_PREFIX, "cccc[dfactor]f[debug]b[planar]b[cache]i[gcache]i", CreateSCSelect, (void *)0 },
  { "RestoreMotionBlocks", BUILTIN_FUNC_PREFIX, "cc[neighbour]c[neighbour2]c[alternative]c[planar]b[show]b[debug]b[gmthreshold]i[mthreshold]i[noise]i[noisy]i[dist]i[tolerance]i[dmode]i[pthreshold]i[cthreshold]i[grey]b", CreateRestoreMotionBlocks, (void *)0 },
  //{ "amDCT", BUILTIN_FUNC_PREFIX, "c[pf1]c[bf1]c[quant]i[adapt]i[shift]i[matrix]i[qtype]i[sharpWPos]i[sharpWAmt]i[expand]i[sharpTPos]i[sharpTAmt]i[quality]i[brightStart]i[brightAmt]i[darkStart]i[darkAmt]i[showMask]i[T2]i[ncpu]i", Create_amDCT, (void *)0 },
  { "AddGrain", BUILTIN_FUNC_PREFIX, "c[var]f[hcorr]f[vcorr]f[uvar]f[seed]i[constant]b[sse2]b", Create_AddGrain, (void *)0 },
  { "AddGrainC", BUILTIN_FUNC_PREFIX, "c[var]f[uvar]f[hcorr]f[vcorr]f[seed]i[constant]b[sse2]b", Create_AddGrainC, (void *)0 },
  //{ "SubtitleEx", BUILTIN_FUNC_PREFIX, "c[text]s[x]i[y]i[firstframe]i[lastframe]i[font]s[effects]s[size]i[textcolor]i[halocolor]i[spc]i[linemargin]i", Create_CSubTitlteEx, (void *)0 },
  { "FastBlur", BUILTIN_FUNC_PREFIX, "cf[y_blur]f[iterations]i[dither]b[gamma]b[threads]f", Create_FastBlur, (void *)0 },

  //{ "average", BUILTIN_FUNC_PREFIX, ".*", create_average, (void *)0 },
  //{ "MedianBlur", BUILTIN_FUNC_PREFIX, "c[radiusy]i[radiusu]i[radiusv]i[opt]i", create_median_blur, (void *)0 },
  //{ "MedianBlurTemporal", BUILTIN_FUNC_PREFIX, "c[radiusy]i[radiusu]i[radiusv]i[temporalradius]i[opt]i", create_temporal_median_blur, (void *)0 },

  { "RemoveGrain", BUILTIN_FUNC_PREFIX, "c[mode]i[modeU]i[modeV]i[planar]b[opt]i", Create_RemoveGrain, (void *)0 },
  { "Repair", BUILTIN_FUNC_PREFIX, "cc[mode]i[modeU]i[modeV]i[planar]b[opt]i", Create_Repair, (void *)0 },
  { "Clense", BUILTIN_FUNC_PREFIX, "c[previous]c[next]c[grey]b[reduceflicker]b[planar]b[cache]i", Create_Clense, (void *)0 },
  { "ForwardClense", BUILTIN_FUNC_PREFIX, "c[grey]b[planar]b[cache]i", Create_ForwardClense, (void *)0 },
  { "BackwardClense", BUILTIN_FUNC_PREFIX, "c[grey]b[planar]b[cache]i", Create_BackwardClense, (void *)0 },
  //{ "VerticalCleaner", BUILTIN_FUNC_PREFIX, "c[mode]i[modeU]i[modeV]i[planar]b", Create_VerticalCleaner, (void *)0 },
  //{ "TemporalRepair", BUILTIN_FUNC_PREFIX, "cc[mode]i[smooth]i[grey]b[planar]b[opt]i", Create_TemporalRepair, (void *)0 },

  //{ "Quantile", BUILTIN_FUNC_PREFIX, "c[radius]i[rank]i[radius_y]i[yradius_y]i[rank_y]i[radius_u]i[yradius_u]i[rank_u]i[radius_v]i[yradius_v]i[rank_v]i[planar]b", CreateQuantile, (void *)0 },
  //{ "SmartMedian", BUILTIN_FUNC_PREFIX, "c[radius]i[pixels]i[radius_y]i[yradius_y]i[pixels_y]i[radius_u]i[yradius_u]i[pixels_u]i[radius_v]i[yradius_v]i[pixels_v]i[planar]b", CreateSmartMedian, (void *)0 },
  //{ "RemoveGrainHD", BUILTIN_FUNC_PREFIX, "c[repair]c[radius]i[rank]i[radius_y]i[yradius_y]i[rank_y]i[urank_y]i[radius_u]i[yradius_u]i[rank_u]i[urank_u]i[radius_v]i[yradius_v]i[rank_v]i[urank_v]i[planar]b", CreateRemoveGrainHD, (void *)0 },
  //{ "TemporalRemoveGrainHD", BUILTIN_FUNC_PREFIX, "c[repair]c[radius]i[rank]i[radius_y]i[yradius_y]i[rank_y]i[urank_y]i[radius_u]i[yradius_u]i[rank_u]i[urank_u]i[radius_v]i[yradius_v]i[rank_v]i[urank_v]i[weight]i[planar]b", CreateTemporalRemoveGrainHD, (void *)0 },
  //{ "TemporalSmartMedian", BUILTIN_FUNC_PREFIX, "c[radius]i[pixels]i[radius_y]i[yradius_y]i[pixels_y]i[radius_u]i[yradius_u]i[pixels_u]i[radius_v]i[yradius_v]i[pixels_v]i[weight]i[planar]b", CreateTemporalSmartMedian, (void *)0 },
  //{ "SmartMedian2", BUILTIN_FUNC_PREFIX, "cc[radius]i[pixels]i[radius_y]i[yradius_y]i[pixels_y]i[radius_u]i[yradius_u]i[pixels_u]i[radius_v]i[yradius_v]i[pixels_v]i[weight]i[planar]b", CreateSmartMedian2, (void *)0 },
  //{ "RankRepair", BUILTIN_FUNC_PREFIX, "cc[radius]i[radius_y]i[yradius_y]i[radius_u]i[yradius_u]i[radius_v]i[yradius_v]i[restore_chroma]b[planar]b", CreateRankRepair, (void *)0 },
  //{ "EEDI3CL", BUILTIN_FUNC_PREFIX, "c[field]i[dh]b[planes]i*[alpha]f[beta]f[gamma]f[nrad]i[mdis]i[hp]b[ucubic]b[cost3]b[vcheck]i[vthresh0]f[vthresh1]f[vthresh2]f[sclip]c[opt]i[device]i[list_device]b[info]b[luma]b[dw]b", Create_EEDI3CL, (void *)0 },
  //{ "Nnedi3CL", BUILTIN_FUNC_PREFIX, "c[field]i[dh]b[dw]b[planes]i[nsize]i[nns]i[qual]i[etype]i[pscrn]i[device]i[list_device]b[info]b[st]b[luma]b", Create_NNEDI3CL, (void *)0 },
  /*{ "eedi3", BUILTIN_FUNC_PREFIX, "c[field]i[dh]b[Y]b[U]b[V]b[alpha]f[beta]f[gamma]f[nrad]i[mdis]i[hp]b[ucubic]b[cost3]b" \
                                   "[vcheck]i[vthresh0]f[vthresh1]f[vthresh2]f[sclip]c[threads]i[mclip]c[opt]i", Create_eedi3, (void *)0 },
  { "nnedi3", BUILTIN_FUNC_PREFIX, "c[field]i[dh]b[Y]b[U]b[V]b[nsize]i[nns]i[qual]i[etype]i[pscrn]i[threads]i[opt]i[fapprox]i" \
                                    "[logicalCores]b[MaxPhysCore]b[SetAffinity]b[A]b[sleep]b[prefetch]i[range]i[ThreadLevel]i", Create_nnedi3, (void *)0 },
  { "nnedi3_rpow2", BUILTIN_FUNC_PREFIX, "c[rfactor]i[nsize]i[nns]i[qual]i[etype]i[pscrn]i[cshift]s[fwidth]i[fheight]i[ep0]f[ep1]f" \
        "[threads]i[opt]i[fapprox]i[csresize]b[mpeg2]b[logicalCores]b[MaxPhysCore]b[SetAffinity]b[threads_rs]i[logicalCores_rs]b" \
        "[MaxPhysCore_rs]b[SetAffinity_rs]b[sleep]b[prefetch]i[range]i[ThreadLevel]i[ThreadLevel_rs]i", Create_nnedi3_rpow2, (void *)0 },*/

  { "z_PointResize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[chromaloc_op]s[dither]s", create_legacy_resize_default, reinterpret_cast<void*>(const_cast<char*>("point")) },
  { "z_BilinearResize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[chromaloc_op]s[dither]s", create_legacy_resize_default, reinterpret_cast<void*>(const_cast<char*>("bilinear")) },
  { "z_BicubicResize", BUILTIN_FUNC_PREFIX, "cii[b]f[c]f[src_left]f[src_top]f[src_width]f[src_height]f[chromaloc_op]s[dither]s", create_legacy_resize_bicubic, reinterpret_cast<void*>(const_cast<char*>("bicubic")) },
  { "z_LanczosResize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[taps]i[chromaloc_op]s[dither]s", create_legacy_resize_lanczos<3>, reinterpret_cast<void*>(const_cast<char*>("lanczos")) },
  { "z_Lanczos4Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[taps]i[chromaloc_op]s[dither]s", create_legacy_resize_lanczos<4>, reinterpret_cast<void*>(const_cast<char*>("lanczos")) },
  { "z_Spline16Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[chromaloc_op]s[dither]s", create_legacy_resize_default, reinterpret_cast<void*>(const_cast<char*>("spline16")) },
  { "z_Spline36Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[chromaloc_op]s[dither]s", create_legacy_resize_default, reinterpret_cast<void*>(const_cast<char*>("spline36")) },
  { "z_Spline64Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[chromaloc_op]s[dither]s", create_legacy_resize_default, reinterpret_cast<void*>(const_cast<char*>("spline64")) },
  { CONVERT_FUNCTION_NAME, BUILTIN_FUNC_PREFIX, CONVERT_SIGNATURE, create_resize, (void *)0 },
  //{ "BSVideoSource", BUILTIN_FUNC_PREFIX, BSVideoSourceAvsArgs, CreateBSVideoSource, nullptr },
  //{ "BSAudioSource", BUILTIN_FUNC_PREFIX, BSAudioSourceAvsArgs, CreateBSAudioSource, nullptr },
  //{ "BSSource", BUILTIN_FUNC_PREFIX, BSSourceAvsArgs, CreateBSSource, nullptr },
  //{ "BSSetDebugOutput", BUILTIN_FUNC_PREFIX, "[enable]b", BSSetDebugOutput, nullptr },
  //{ "BSSetFFmpegLogLevel", BUILTIN_FUNC_PREFIX, "[level]i", BSSetFFmpegLogLevel, nullptr },
  { "LSMASHVideoSource", BUILTIN_FUNC_PREFIX, "[source]s[track]i[threads]i[seek_mode]i[seek_threshold]i[dr]b[fpsnum]i[fpsden]i[format]s[decoder]s[prefer_hw]i[ff_loglevel]i[ff_options]s", CreateLSMASHVideoSource, (void *)0 },
  { "LSMASHAudioSource", BUILTIN_FUNC_PREFIX, "[source]s[track]i[skip_priming]b[layout]s[rate]i[decoder]s[ff_loglevel]i[drc_scale]f[ff_options]s", CreateLSMASHAudioSource, (void *)0 },
  { "LWLibavVideoSource", BUILTIN_FUNC_PREFIX, "[source]s[stream_index]i[threads]i[cache]b[cachefile]s[seek_mode]i[seek_threshold]i[dr]b[fpsnum]i[fpsden]i[repeat]b[dominance]i[format]s[decoder]s[prefer_hw]i[ff_loglevel]i[cachedir]s[indexingpr]b[ff_options]s[rap_verification]b", CreateLWLibavVideoSource, (void *)0 },
  { "LWLibavAudioSource", BUILTIN_FUNC_PREFIX, "[source]s[stream_index]i[cache]b[cachefile]s[av_sync]b[layout]s[rate]i[decoder]s[ff_loglevel]i[cachedir]s[indexingpr]b[drc_scale]f[ff_options]s[fill_agaps]i", CreateLWLibavAudioSource, (void *)0 },

  { "vsCnr2", BUILTIN_FUNC_PREFIX, "c[mode]s[scdthr]f[ln]i[lm]i[un]i[um]i[vn]i[vm]i[sceneChroma]b", Create_vsCnr2, (void *)0 },
  { "vsDeblockPP7", BUILTIN_FUNC_PREFIX, "c[qp]f[mode]i[y]i[u]i[v]i", Create_vsDeblockPP7, (void *)0 },
  { "vsDeGrainMedian", BUILTIN_FUNC_PREFIX, "c[limitY]i[limitU]i[limitV]i[modeY]i[modeU]i[modeV]i[interlaced]b[norow]b[opt]i", Create_vsDeGrainMedian, (void *)0 },
  { "vsLGhost", BUILTIN_FUNC_PREFIX, "c[mode]i+[shift]i+[intensity]i+[y]i[u]i[v]i[opt]i", Create_vsLGhost, (void *)0 },
  { "vsMSmooth", BUILTIN_FUNC_PREFIX, "c[threshold]f[strength]f[mask]b[luma]b[chroma]b", Create_vsMSmooth, (void *)0 },
  { "vsTBilateral", BUILTIN_FUNC_PREFIX, "c[ppclip]c[diameterY]i[diameterU]i[diameterV]i[sdevY]f[sdevU]f[sdevV]f[idevY]f[idevU]f[idevV]f[csY]f[csU]f[csV]f[d2]b[kerns]i[kerni]i[restype]i[y]i[u]i[v]i", Create_vsTBilateral, (void *)0 },
  { "vsTCanny", BUILTIN_FUNC_PREFIX, "c[sigmaY]f[sigmaU]f[sigmaV]f[sigma_vY]f[sigma_vU]f[sigma_vV]f[t_h]f[t_l]f[mode]i[op]i[scale]f[y]i[u]i[v]i[opt]i", Create_vsTCanny, (void *)0 },
  { "vsTEdgeMask", BUILTIN_FUNC_PREFIX, "c[threshY]f[threshU]f[threshV]f[type]i[link]i[scale]f[y]i[u]i[v]i[opt]i", Create_vsTEdgeMask, (void *)0 },
  { "vsTMM", BUILTIN_FUNC_PREFIX, "c[mode]i[order]i[field]i[length]i[mtype]i[ttype]i[mtqL]i[mthL]i[mtqC]i[mthC]i[nt]i[minthresh]i[maxthresh]i[cstr]i[athresh]i[metric]i[expand]i[binary]b[link]b[eight]b[y]i[u]i[v]i[opt]i", Create_vsTMM, (void *)0 },
  { "vsTTempSmooth", BUILTIN_FUNC_PREFIX, "c[maxr]i[ythresh]i[uthresh]i[vthresh]i[ymdiff]i[umdiff]i[vmdiff]i[strength]i[scthresh]f[fp]b[y]i[u]i[v]i[pfclip]c[opt]i[pmode]i[ythupd]i[uthupd]i[vthupd]i[ypnew]i[upnew]i[vpnew]i[threads]i", Create_TTempSmooth, (void *)0 },

  //{ "FFTSpectrum", BUILTIN_FUNC_PREFIX, "c[grid]b", Create_FFTSpectrum, (void *)0 },
  { "JincResize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[tap]i[blur]f[cplace]s[threads]i[opt]i[initial_capacity]i[initial_factor]f", Create_JincResize, (void *)0 },
  { "Jinc36Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<3>, (void *)0 },
  { "Jinc64Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<4>, (void *)0 },
  { "Jinc144Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<6>, (void *)0 },
  { "Jinc256Resize", BUILTIN_FUNC_PREFIX, "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<8>, (void *)0 },
  /*{ "neo_fft3d", BUILTIN_FUNC_PREFIX, "c[sigma]f[beta]f[bw]i[bh]i[bt]i[ow]i[oh]i[kratio]f[sharpen]f[scutoff]f[svr]f[smin]f[smax]f[measure]b[interlaced]b[wintype]i[pframe]i[px]i[py]i[pshow]b[pcutoff]f[pfactor]f" \
    "[sigma2]f[sigma3]f[sigma4]f[degrid]f[dehalo]f[hr]f[ht]f[l]i[t]i[r]i[b]i[opt]i[y]i[u]i[v]i[ncpu]i[mt]b", AVSInterface::Create_neo, (void *)0 },
  { "FFT3DFilter", BUILTIN_FUNC_PREFIX, "c[sigma]f[beta]f[plane]i[bw]i[bh]i[bt]i[ow]i[oh]i[kratio]f[sharpen]f[scutoff]f[svr]f[smin]f[smax]f[measure]b[interlaced]b[wintype]i[pframe]i[px]i[py]i[pshow]b[pcutoff]f" \
    "[pfactor]f[sigma2]f[sigma3]f[sigma4]f[degrid]f[dehalo]f[hr]f[ht]f[ncpu]i", Create_FFT3DFilterMulti, (void *)0 },*/

  { "TFM", BUILTIN_FUNC_PREFIX, "c[order]i[field]i[mode]i[PP]i[ovr]s[input]s[output]s[outputC]s[debug]b[display]b[slow]i[mChroma]b[cNum]i[cthresh]i[MI]i" \
    "[chroma]b[blockx]i[blocky]i[y0]i[y1]i[mthresh]i[clip2]c[d2v]s[ovrDefault]i[flags]i[scthresh]f[micout]i[micmatching]i[trimIn]s" \
    "[hint]b[metric]i[batch]b[ubsco]b[mmsco]b[opt]i", Create_TFM, (void *)0 },
  { "TDecimate", BUILTIN_FUNC_PREFIX, "c[mode]i[cycleR]i[cycle]i[rate]f[dupThresh]f[vidThresh]f[sceneThresh]f[hybrid]i[vidDetect]i[conCycle]i[conCycleTP]i" \
    "[ovr]s[output]s[input]s[tfmIn]s[mkvOut]s[nt]i[blockx]i[blocky]i[debug]b[display]b[vfrDec]i[batch]b[tcfv1]b[se]b" \
    "[chroma]b[exPP]b[maxndl]i[m2PA]b[denoise]b[noblend]b[ssd]b[hint]b[clip2]c[sdlim]i[opt]i[orgOut]s[displayDecimation]i[displayOpt]i[dclip]c[sceneDec]b", Create_TDecimate, (void *)0 },
  { "MergeHints", BUILTIN_FUNC_PREFIX, "c[hintClip]c[debug]b", Create_MergeHints, (void *)0 },
  { "FieldDiff", BUILTIN_FUNC_PREFIX, "c[nt]i[chroma]b[display]b[debug]b[sse]b[opt]i", Create_FieldDiff, (void *)0 },
  { "CFieldDiff", BUILTIN_FUNC_PREFIX, "c[nt]i[chroma]b[debug]b[sse]b[opt]i", Create_CFieldDiff, (void *)0 },
  { "FrameDiff", BUILTIN_FUNC_PREFIX, "c[mode]i[prevf]b[nt]i[blockx]i[blocky]i[chroma]b[thresh]f[display]i[debug]b[norm]b[denoise]b[ssd]b[opt]i[offset]i", Create_FrameDiff, (void *)0 },
  { "CFrameDiff", BUILTIN_FUNC_PREFIX, "c[mode]i[prevf]b[nt]i[blockx]i[blocky]i[chroma]b[debug]b[norm]b[denoise]b[ssd]b[rpos]b[opt]i[offset]i", Create_CFrameDiff, (void *)0 },
  { "ShowCombedTIVTC", BUILTIN_FUNC_PREFIX, "c[cthresh]i[chroma]b[MI]i[blockx]i[blocky]i[metric]i[debug]b[display]i[fill]b[opt]i", Create_ShowCombedTIVTC, (void *)0 },
  { "IsCombedTIVTC", BUILTIN_FUNC_PREFIX, "c[cthresh]i[MI]i[chroma]b[blockx]i[blocky]i[metric]i[opt]i", Create_IsCombedTIVTC, (void *)0 },
  { "RequestLinear", BUILTIN_FUNC_PREFIX, "c[rlim]i[clim]i[elim]i[rall]b[debug]b", Create_RequestLinear, (void *)0 },
  { "TDeint", BUILTIN_FUNC_PREFIX, "c[mode]i[order]i[field]i[mthreshL]i[mthreshC]i[map]i[ovr]s[ovrDefault]i[type]i[debug]b[mtnmode]i[sharp]b[hints]b[clip2]c[full]b[cthresh]i" \
    "[chroma]b[MI]i[tryWeave]b[link]i[denoise]b[AP]i[blockx]i[blocky]i[APType]i[edeint]c[emask]c[blim]f[metric]i[expand]i[slow]i[emtn]c[tshints]b[opt]i", Create_TDeinterlace, (void *)0 },
  { "TSwitch", BUILTIN_FUNC_PREFIX, "c[c1]c[c2]c[debug]b", Create_TSwitch, (void *)0 },

  //{ "ConvertFromStacked", BUILTIN_FUNC_PREFIX, "c[bits]i", ConvertFromStacked::Create, (void *)0 },
  //{ "ConvertToStacked", BUILTIN_FUNC_PREFIX, "c", ConvertToStacked::Create, (void *)0 },
  //{ "ConvertFromDoubleWidth", BUILTIN_FUNC_PREFIX, "c[bits]i", ConvertFromDoubleWidth::Create, (void *)0 },
  //{ "ConvertToDoubleWidth", BUILTIN_FUNC_PREFIX, "c", ConvertToDoubleWidth::Create, (void *)0 },
  //{ "DirectShowSource", BUILTIN_FUNC_PREFIX, "s[fps]f[seek]b[audio]b[video]b[convertfps]b[seekzero]b[timeout]i[pixel_type]s[framecount]i[logfile]s[logmask]i[utf8]b", DirectShowSource::Create, (void *)0 },
  //{ "Deflicker", BUILTIN_FUNC_PREFIX, "c[percent]f[lag]i[noise]f[scene]i[lmin]i[lmax]i[border]i[info]b[debug]b[opt]i", Create_Deflicker, (void *)0 },
  //{ "maxcllfind", BUILTIN_FUNC_PREFIX, "c[maxFallAlgorithm]i", xmaxcllfind::Create, (void *)0 },
  //{ "PerfPan", BUILTIN_FUNC_PREFIX, "c[perforation]c[blank_threshold]f[reference_frame]i[max_search]i[log]s[plot_scores]b[hintfile]s[copy_on_limit]b", Create_PerfPan, (void *)0 },
  //{ "frfun7", BUILTIN_FUNC_PREFIX, "c[lambda]f[T]f[Tuv]f[P]i[TP1]i[R1]i[opt]i", AvsFilter::Create, (void *)0 },
  //{ "ColorMatrixTransform", BUILTIN_FUNC_PREFIX, "cffffffffffff", Create_ColorMatrixTransform, (void *)0 },

  #if HAVE_RESAMPLEMT
  { "PointResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_PointResize, (void *)0 },
  { "BilinearResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_BilinearResize, (void *)0 },
  { "BicubicResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[b]f[c]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_BicubicResize, (void *)0 },
  { "LanczosResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_LanczosResize, (void *)0 },
  { "Lanczos4ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_Lanczos4Resize, (void *)0 },
  { "BlackmanResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_BlackmanResize, (void *)0 },
  { "Spline16ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_Spline16Resize, (void *)0 },
  { "Spline36ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_Spline36Resize, (void *)0 },
  { "Spline64ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_Spline64Resize, (void *)0 },
  { "GaussResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[p]f[b]f[s]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_GaussianResize, (void *)0 },
  { "SincResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_SincResize, (void *)0 },
  { "SinPowResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[p]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_SinPowerResize, (void *)0 },
  { "SincLin2ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i",FilteredResizeMT::Create_SincLin2Resize, (void *)0 },
  { "UserDefined2ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[b]f[c]f[s]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[ThreadLevel]i", FilteredResizeMT::Create_UserDefined2Resize, (void *)0 },

// Desample functions

  { "DeBilinearResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeBilinearResize, (void *)0 },
  { "DeBicubicResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[b]f[c]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeBicubicResize, (void *)0 },
  { "DeLanczosResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeLanczosResize, (void *)0 },
  { "DeLanczos4ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeLanczos4Resize, (void *)0 },
  { "DeBlackmanResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeBlackmanResize, (void *)0 },
  { "DeSpline16ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeSpline16Resize, (void *)0 },
  { "DeSpline36ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeSpline36Resize, (void *)0 },
  { "DeSpline64ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeSpline64Resize, (void *)0 },
  { "DeGaussResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[p]f[b]f[s]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeGaussianResize, (void *)0 },
  { "DeSincResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeSincResize, (void *)0 },
  { "DeSinPowResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[p]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeSinPowerResize, (void *)0 },
  { "DeSincLin2ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[taps]i[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeSincLin2Resize, (void *)0 },
  { "DeUserDefined2ResizeMT", BUILTIN_FUNC_PREFIX, "c[target_width]i[target_height]i[b]f[c]f[s]f[src_left]f[src_top]f[src_width]f[src_height]f[force]i[keep_center]b[placement]s[threads]i" \
		"[logicalCores]b[MaxPhysCore]b[SetAffinity]b[sleep]b[prefetch]i[range]i[accuracy]i[order]i[ThreadLevel]i",FilteredResizeMT::Create_DeUserDefined2Resize, (void *)0 },
#endif

  //{ "FillBorders", BUILTIN_FUNC_PREFIX, "c[left]i*[top]i*[right]i*[bottom]i*[mode]i[y]i[u]i[v]i[a]i[interlaced]b", Create_FillBorders, (void *)0 },
  //{ "FillMargins", BUILTIN_FUNC_PREFIX, "c[left]i[top]i[right]i[bottom]i[y]i[u]i[v]i", Create_FillMargins, (void *)0 },
  /*{ "DeDup", BUILTIN_FUNC_PREFIX, \
        "c[threshold]f[threshold2]f[range2]i[trigger2]f[show]b[dec]b[maxcopies]i[maxdrops]i" \
        "[decwhich]i[log]s[times]s[timesin]s[ovr]s[debug]s", Create_DeDup, (void *)0 },*/
  //{ "DupMC", BUILTIN_FUNC_PREFIX, "c[chroma]b[log]s[search]i", Create_DupMetric, (void *)0 },
  //{ "DePan", BUILTIN_FUNC_PREFIX, "c[data]c[offset]f[subpixel]i[pixaspect]f[matchfields]b[mirror]i[blur]i[info]b[inputlog]s", Create_DePan, (void *)0 },
  //{ "DePanInterleave", BUILTIN_FUNC_PREFIX, "c[data]c[prev]i[next]i[subpixel]i[pixaspect]f[matchfields]b[mirror]i[blur]i[info]b[inputlog]s", Create_DePanInterleave, (void *)0 },
  /*{ "DePanStabilize", BUILTIN_FUNC_PREFIX, \
        "c[data]c[cutoff]f[damping]f[initzoom]f[addzoom]b[prev]i[next]i[mirror]i[blur]i[dxmax]f" \
        "[dymax]f[zoommax]f[rotmax]f[subpixel]i[pixaspect]f[fitlast]i[tzoom]f[info]b[inputlog]s" \
        "[vdx]s[vdy]s[vzoom]s[vrot]s[method]i[debuglog]s", Create_DePanStabilize, (void *)0 },*/
  //{ "DePanScenes", BUILTIN_FUNC_PREFIX, "c[plane]i[inputlog]s", Create_DePanScenes, (void *)0 },
  //{ "SoxFilter", BUILTIN_FUNC_PREFIX, "cs+", Create_SoxFilter, (void *)0 },
  //{ "SoxFilter_ListEffects", BUILTIN_FUNC_PREFIX, "", SoxFilter_ListEffects, (void *)0 },
  //{ "SoxFilter_GetAllEffects", BUILTIN_FUNC_PREFIX, "", SoxFilter_GetAllEffects, (void *)0 },
  //{ "SoxFilter_GetEffectUsage", BUILTIN_FUNC_PREFIX, "s", SoxFilter_GetEffectUsage, (void *)0 },

  { 0 }
};


/****************************************
*******   Convert to RGB / RGBA   ******
***************************************/

// YUY2 to packed RGB only
ConvertToRGB::ConvertToRGB( PClip _child, bool rgb24, const char* matrix_name,
                           IScriptEnvironment* env )
                           : GenericVideoFilter(_child)
{
  auto frame0 = _child->GetFrame(0, env);
  const AVSMap* props = env->getFramePropsRO(frame0);
  matrix_parse_merge_with_props(vi.IsRGB(), true, matrix_name, props, theMatrix, theColorRange, theOutColorRange, env);

  const int shift = 16; // for integer arithmetic; YUY2 is using 16 bits, later is divided back by 4 or 8
  const int bits_per_pixel = 8; // YUY2
  if (!do_BuildMatrix_Yuv2Rgb(theMatrix, theColorRange, theOutColorRange, shift, bits_per_pixel, /*ref*/matrix))
    env->ThrowError("ConvertToRGB: invalid \"matrix\" parameter");

  theOutMatrix = Matrix_e::AVS_MATRIX_RGB;

  // these constants are used with intentional minus operator in core calculations
  matrix.v_g = -matrix.v_g;
  matrix.u_g = -matrix.u_g;
  matrix.offset_y = -matrix.offset_y;

  vi.pixel_type = rgb24 ? VideoInfo::CS_BGR24 : VideoInfo::CS_BGR32;
}

template<int rgb_size>
static void convert_yuy2_to_rgb_c(const BYTE *srcp, BYTE* dstp, int src_pitch, int dst_pitch, int height, int width, int crv, int cgv, int cgu, int cbu, int cy, int tv_scale, int rgb_offset) {
  srcp += height * src_pitch;
  rgb_offset <<= 16; // integer arithmetic range
  for (int y = height; y > 0; --y) {
    srcp -= src_pitch;
    int x;
    for (x = 0; x < width-2; x+=2) {
      int scaled_y0 = (srcp[x*2+0] - tv_scale) * cy + rgb_offset;
      int u0 = srcp[x*2+1]-128;
      int v0 = srcp[x*2+3]-128;
      int scaled_y1 = (srcp[x*2+2] - tv_scale) * cy + rgb_offset;
      int u1 = srcp[x*2+5]-128;
      int v1 = srcp[x*2+7]-128;

      dstp[x*rgb_size + 0] = ScaledPixelClip(scaled_y0 + u0 * cbu);                 // blue
      dstp[x*rgb_size + 1] = ScaledPixelClip(scaled_y0 - u0 * cgu - v0 * cgv); // green
      dstp[x*rgb_size + 2] = ScaledPixelClip(scaled_y0            + v0 * crv); // red

      dstp[(x+1)*rgb_size + 0] = ScaledPixelClip(scaled_y1 + (u0+u1) * (cbu / 2));                     // blue
      dstp[(x+1)*rgb_size + 1] = ScaledPixelClip(scaled_y1 - (u0+u1) * (cgu / 2) - (v0+v1) * (cgv/2)); // green
      dstp[(x+1)*rgb_size + 2] = ScaledPixelClip(scaled_y1                       + (v0+v1) * (crv/2)); // red

      if constexpr(rgb_size == 4) {
        dstp[x*4+3] = 255;
        dstp[x*4+7] = 255;
      }
    }

    int scaled_y0 = (srcp[x*2+0] - tv_scale) * cy + rgb_offset;
    int scaled_y1 = (srcp[x*2+2] - tv_scale) * cy + rgb_offset;
    int u = srcp[x*2+1]-128;
    int v = srcp[x*2+3]-128;

    dstp[x*rgb_size + 0]     = ScaledPixelClip(scaled_y0 + u * cbu);                 // blue
    dstp[x*rgb_size + 1]     = ScaledPixelClip(scaled_y0 - u * cgu - v * cgv); // green
    dstp[x*rgb_size + 2]     = ScaledPixelClip(scaled_y0           + v * crv); // red

    dstp[(x+1)*rgb_size + 0] = ScaledPixelClip(scaled_y1 + u * cbu);                 // blue
    dstp[(x+1)*rgb_size + 1] = ScaledPixelClip(scaled_y1 - u * cgu - v * cgv); // green
    dstp[(x+1)*rgb_size + 2] = ScaledPixelClip(scaled_y1           + v * crv); // red

    if constexpr(rgb_size == 4) {
      dstp[x*4+3] = 255;
      dstp[x*4+7] = 255;
    }
    dstp += dst_pitch;
  }
}

// YUY2 only
PVideoFrame __stdcall ConvertToRGB::GetFrame(int n, IScriptEnvironment* env)
{
  PVideoFrame src = child->GetFrame(n, env);
  const int src_pitch = src->GetPitch();
  const BYTE* srcp = src->GetReadPtr();

  PVideoFrame dst = env->NewVideoFrameP(vi, &src);

  auto props = env->getFramePropsRW(dst);
  update_Matrix_and_ColorRange(props, theOutMatrix, theOutColorRange, env);
  update_ChromaLocation(props, -1, env); // RGB target: delete _ChromaLocation

  const int dst_pitch = dst->GetPitch();
  BYTE* dstp = dst->GetWritePtr();
  int tv_scale = matrix.offset_y;


#ifdef INTEL_INTRINSICS
  if (env->GetCPUFlags() & CPUF_SSE2) {
    if (vi.IsRGB32()) {
      convert_yuy2_to_rgb_sse2<4>(srcp, dstp, src_pitch, dst_pitch, vi.height, vi.width,
      matrix.v_r, matrix.v_g, matrix.u_g, matrix.u_b, matrix.y_r, tv_scale, matrix.offset_rgb);
    } else {
      convert_yuy2_to_rgb_sse2<3>(srcp, dstp, src_pitch, dst_pitch, vi.height, vi.width,
        matrix.v_r, matrix.v_g, matrix.u_g, matrix.u_b, matrix.y_r, tv_scale, matrix.offset_rgb);
    }
  }
  else
#ifdef X86_32
  if (env->GetCPUFlags() & CPUF_INTEGER_SSE) {
    if (vi.IsRGB32()) {
      convert_yuy2_to_rgb_isse<4>(srcp, dstp, src_pitch, dst_pitch, vi.height, vi.width,
        matrix.v_r, matrix.v_g, matrix.u_g, matrix.u_b, matrix.y_r, tv_scale, matrix.offset_rgb);
    } else {
      convert_yuy2_to_rgb_isse<3>(srcp, dstp, src_pitch, dst_pitch, vi.height, vi.width,
        matrix.v_r, matrix.v_g, matrix.u_g, matrix.u_b, matrix.y_r, tv_scale, matrix.offset_rgb);
    }
  }
  else
#endif
#endif
  {
    if (vi.IsRGB32()) {
      convert_yuy2_to_rgb_c<4>(srcp, dstp, src_pitch, dst_pitch, vi.height, vi.width,
        matrix.v_r, matrix.v_g, matrix.u_g, matrix.u_b, matrix.y_r, tv_scale, matrix.offset_rgb);
    } else {
      convert_yuy2_to_rgb_c<3>(srcp, dstp, src_pitch, dst_pitch, vi.height, vi.width,
        matrix.v_r, matrix.v_g, matrix.u_g, matrix.u_b, matrix.y_r, tv_scale, matrix.offset_rgb);
    }
  }
  return dst;
}

// general for all colorspaces
// however class is constructed only for YUY2 input
AVSValue __cdecl ConvertToRGB::Create(AVSValue args, void* user_data, IScriptEnvironment* env)
{
  const bool haveOpts = args[3].Defined() || args[4].Defined();
  PClip clip = args[0].AsClip();
  const char* const matrix_name = args[1].AsString(0);
  VideoInfo vi = clip->GetVideoInfo();

  // common Create for all CreateRGB24/32/48/64/Planar(RGBP:-1, RGPAP:-2) using user_data
  int target_rgbtype = (int)reinterpret_cast<intptr_t>(user_data);
  // -1,-2: Planar RGB(A)
  //  0: not specified (leave if input is packed RGB, convert to rgb32/64 input colorspace dependent)
  // 24,32,48,64: RGB24/32/48/64

  if (target_rgbtype == 0 && vi.BitsPerComponent() != 8 && vi.BitsPerComponent() != 16)
    env->ThrowError("ConvertToRGB: conversion is allowed only from 8 or 16 bit colorspaces");

  // planar YUV-like
  if (vi.IsPlanar() && (vi.IsYUV() || vi.IsYUVA())) {
    bool needConvertFinalBitdepth = false;
    int finalBitdepth = -1;

    AVSValue new_args[8] = { clip, args[2], args[1], args[3], args[4], args[5], args[6], args[7]};
    // conversion to planar or packed RGB is always from 444
    // clip, interlaced, matrix, chromainplacement, chromaresample, param1, param2, param3
    clip = ConvertToPlanarGeneric::CreateYUV444(AVSValue(new_args, 8), (void *)1, env).AsClip(); // (void *)1: not restricted to 8 bits
    if ((target_rgbtype == 24 || target_rgbtype == 32)) {
      if (vi.BitsPerComponent() != 8) {
        needConvertFinalBitdepth = true;
        finalBitdepth = 8;
        target_rgbtype = (target_rgbtype == 24) ? -1 : -2; // planar rgb intermediate
      }
    }
    else if ((target_rgbtype == 48 || target_rgbtype == 64)) {
      if (vi.BitsPerComponent() != 16) {
        needConvertFinalBitdepth = true;
        finalBitdepth = 16;
        target_rgbtype = (target_rgbtype == 48) ? -1 : -2; // planar rgb intermediate
      }
    }
    int rgbtype_param = 0;
    bool reallyConvert = true;
    switch (target_rgbtype)
    {
    case -1: case -2:
        rgbtype_param = target_rgbtype; break; // planar RGB(A)
    case 0:
        rgbtype_param = vi.ComponentSize() == 1 ? 4 : 8; break; // input bitdepth adaptive
    case 24:
        rgbtype_param = 3; break; // RGB24
    case 32:
        rgbtype_param = 4; break; // RGB32
    case 48: {
            // instead of C code of YUV444P16->RGB48
            // we convert to PlanarRGB then to RGB48 (both is fast)
          AVSValue new_args2[8] = { clip, args[1], args[2], args[3], args[4], args[5], args[6], args[7] };
          clip = ConvertToRGB::Create(AVSValue(new_args2, 8), (void *)-1, env).AsClip();
          vi = clip->GetVideoInfo();
          reallyConvert = false;
          rgbtype_param = 6; // old option RGB48 target, slow C
        }
        break; // RGB48
    case 64: {
        // instead of C code of YUV(A)444P16->RGB64
        // we convert to PlanarRGB(A) then to RGB64 (both is fast)
        AVSValue new_args2[8] = { clip, args[1], args[2], args[3], args[4], args[5], args[6], args[7] };
        clip = ConvertToRGB::Create(AVSValue(new_args2, 8), vi.IsYUVA() ? (void *)-2 : (void *)-1, env).AsClip();
        vi = clip->GetVideoInfo();
        reallyConvert = false;
        rgbtype_param = 8; // old option RGB64 target, slow C
      }
      break; // RGB64
    }
    if (reallyConvert) {
      clip = new ConvertYUV444ToRGB(clip, matrix_name, rgbtype_param, env);

      if (needConvertFinalBitdepth) {
        // from any planar rgb(a) -> rgb24/32/48/64
        clip = new ConvertBits(clip, -1 /*dither_type*/, finalBitdepth /*target_bitdepth*/, true /*assume_truerange*/,
          ColorRange_e::AVS_RANGE_FULL /*fulls*/, ColorRange_e::AVS_RANGE_FULL /*fulld*/,
          8 /*n/a dither_bitdepth*/, env);
        vi = clip->GetVideoInfo();

        // source here is always a 8/16bit planar RGB(A), finally it has to be converted to RGB24/32/48/64
        const bool isRGBA = target_rgbtype == -2;
        clip = new PlanarRGBtoPackedRGB(clip, isRGBA);
        vi = clip->GetVideoInfo();
      }
      return clip;
    }
  }

  if (haveOpts)
    env->ThrowError("ConvertToRGB: ChromaPlacement and ChromaResample options are not supported.");

  // planar RGB-like source
  if (vi.IsPlanarRGB() || vi.IsPlanarRGBA())
  {
    if (target_rgbtype < 0) // planar to planar
    {
      if (vi.IsPlanarRGB()) {
        if (target_rgbtype == -1)
          return clip;
        // prgb->prgba create with default alpha
        return new AddAlphaPlane(clip, nullptr, 0.0f, false, env);
      }
      // planar rgba source
      if (target_rgbtype == -2)
        return clip;
      return new RemoveAlphaPlane(clip, env);
    }

    // planar to packed 24/32/48/64
    bool needConvertFinalBitdepth = false;
    int finalBitdepth = -1;

    if (target_rgbtype == 24 || target_rgbtype == 32) {
      if (vi.BitsPerComponent() != 8) {
        needConvertFinalBitdepth = true;
        finalBitdepth = 8;
      }
    }
    else if (target_rgbtype == 48 || target_rgbtype == 64) {
      if (vi.BitsPerComponent() != 16) {
        needConvertFinalBitdepth = true;
        finalBitdepth = 16;
      }
    }

    if (needConvertFinalBitdepth) {
      // from any bitdepth planar rgb(a) -> 8/16 bits
      clip = new ConvertBits(clip, -1 /*dither_type*/, finalBitdepth /*target_bitdepth*/, true /*assume_truerange*/,
        ColorRange_e::AVS_RANGE_FULL /*fulls*/, ColorRange_e::AVS_RANGE_FULL /*fulld*/,
        8 /*n/a dither_bitdepth*/, env);
      vi = clip->GetVideoInfo();
    }

    bool hasAlpha = target_rgbtype == 32 || target_rgbtype == 64 ||
      (target_rgbtype == 0 && vi.IsPlanarRGBA());

    return new PlanarRGBtoPackedRGB(clip, hasAlpha);
  }

  // YUY2
  if (vi.IsYUV()) // at this point IsYUV means YUY2 (non-planar)
  {
    if (target_rgbtype == 48 || target_rgbtype == 64)
      env->ThrowError("ConvertToRGB: conversion from YUY2 is allowed only to 8 bits");
    if (target_rgbtype < 0) {
      // rgb32 intermediate is faster
      clip = new ConvertToRGB(clip, false, matrix_name, env); // YUY2->RGB32
      return new PackedRGBtoPlanarRGB(clip, true, target_rgbtype == -2);
    }
    else
      return new ConvertToRGB(clip, target_rgbtype == 24, matrix_name, env);
  }

  // conversions from packed RGB

  if (target_rgbtype == 24 || target_rgbtype == 32) {
    if (vi.ComponentSize() != 1) {
      // 64->32, 48->24
      clip = new ConvertBits(clip, -1 /*dither_type*/, 8 /*target_bitdepth*/, true /*assume_truerange*/,
        ColorRange_e::AVS_RANGE_FULL /*fulls*/, ColorRange_e::AVS_RANGE_FULL /*fulld*/,
        8 /*n/a dither_bitdepth*/, env);
      vi = clip->GetVideoInfo(); // new format
    }
  }
  else if (target_rgbtype == 48 || target_rgbtype == 64) {
    if (vi.ComponentSize() != 2) {
      // 32->64, 24->48
      clip = new ConvertBits(clip, -1 /*dither_type*/, 16 /*target_bitdepth*/, true /*assume_truerange*/,
        ColorRange_e::AVS_RANGE_FULL /*fulls*/, ColorRange_e::AVS_RANGE_FULL /*fulld*/,
        8 /*n/a dither_bitdepth*/, env);
      vi = clip->GetVideoInfo(); // new format
    }
  }

  if(target_rgbtype==32 || target_rgbtype==64)
      if (vi.IsRGB24() || vi.IsRGB48())
          return new RGBtoRGBA(clip); // 24->32 or 48->64

  if(target_rgbtype==24 || target_rgbtype==48)
      if (vi.IsRGB32() || vi.IsRGB64())
          return new RGBAtoRGB(clip); // 32->24 or 64->48

  // <0: target is planar RGB(A)
  if (target_rgbtype < 0) {
    // RGB24/32/48/64 ->
    const bool isSrcRGBA = vi.IsRGB32() || vi.IsRGB64();
    const bool isTargetRGBA = target_rgbtype == -2;
    return new PackedRGBtoPlanarRGB(clip, isSrcRGBA, isTargetRGBA);
  }

  return clip;
}


/**********************************
*******   Convert to YV12   ******
*********************************/

// for YUY2->YV12 only
// all other sources use ConvertToPlanarGeneric
ConvertToYV12::ConvertToYV12(PClip _child, bool _interlaced, IScriptEnvironment* env)
  : GenericVideoFilter(_child),
  interlaced(_interlaced)
{
  if (vi.width & 1)
    env->ThrowError("ConvertToYV12: Image width must be multiple of 2");

  if (interlaced && (vi.height & 3))
    env->ThrowError("ConvertToYV12: Interlaced image height must be multiple of 4");

  if ((!interlaced) && (vi.height & 1))
    env->ThrowError("ConvertToYV12: Image height must be multiple of 2");

  if (!vi.IsYUY2())
    env->ThrowError("ConvertToYV12: Source must be YUY2.");

  vi.pixel_type = VideoInfo::CS_YV12;
}

PVideoFrame __stdcall ConvertToYV12::GetFrame(int n, IScriptEnvironment* env) {
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);

  if (interlaced) {
#ifdef INTEL_INTRINSICS
    if (env->GetCPUFlags() & CPUF_SSE2)
    {
      convert_yuy2_to_yv12_interlaced_sse2(src->GetReadPtr(), src->GetRowSize(), src->GetPitch(),
        dst->GetWritePtr(PLANAR_Y), dst->GetWritePtr(PLANAR_U), dst->GetWritePtr(PLANAR_V),
        dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), src->GetHeight());
    }
    else
#ifdef X86_32
      if ((env->GetCPUFlags() & CPUF_INTEGER_SSE))
      {
        convert_yuy2_to_yv12_interlaced_isse(src->GetReadPtr(), src->GetRowSize(), src->GetPitch(),
          dst->GetWritePtr(PLANAR_Y), dst->GetWritePtr(PLANAR_U), dst->GetWritePtr(PLANAR_V),
          dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), src->GetHeight());
      }
      else
#endif
#endif
      {
        convert_yuy2_to_yv12_interlaced_c(src->GetReadPtr(), src->GetRowSize(), src->GetPitch(),
          dst->GetWritePtr(PLANAR_Y), dst->GetWritePtr(PLANAR_U), dst->GetWritePtr(PLANAR_V),
          dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), src->GetHeight());
      }
  }
  else
  {
#ifdef INTEL_INTRINSICS
    if (env->GetCPUFlags() & CPUF_SSE2)
    {
      convert_yuy2_to_yv12_progressive_sse2(src->GetReadPtr(), src->GetRowSize(), src->GetPitch(),
        dst->GetWritePtr(PLANAR_Y), dst->GetWritePtr(PLANAR_U), dst->GetWritePtr(PLANAR_V),
        dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), src->GetHeight());
    }
    else
#ifdef X86_32
      if ((env->GetCPUFlags() & CPUF_INTEGER_SSE))
      {
        convert_yuy2_to_yv12_progressive_isse(src->GetReadPtr(), src->GetRowSize(), src->GetPitch(),
          dst->GetWritePtr(PLANAR_Y), dst->GetWritePtr(PLANAR_U), dst->GetWritePtr(PLANAR_V),
          dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), src->GetHeight());
      }
      else
#endif
#endif
      {
        convert_yuy2_to_yv12_progressive_c(src->GetReadPtr(), src->GetRowSize(), src->GetPitch(),
          dst->GetWritePtr(PLANAR_Y), dst->GetWritePtr(PLANAR_U), dst->GetWritePtr(PLANAR_V),
          dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), src->GetHeight());
      }
  }

  return dst;
}


/**********************************
*******   Convert to YV12   ******
*********************************/


AVSValue __cdecl ConvertToYV12::Create(AVSValue args, void* user_data, IScriptEnvironment* env)
{
  PClip clip = args[0].AsClip();
  const VideoInfo& vi = clip->GetVideoInfo();
  bool only_8bit = reinterpret_cast<intptr_t>(user_data) == 0;
  if (only_8bit && vi.BitsPerComponent() != 8)
    env->ThrowError("ConvertToYV12: only 8 bit sources allowed");

  if (vi.IsYUY2() && !args[3].Defined() && !args[4].Defined() && !args[5].Defined() && !args[6].Defined() && !args[7].Defined())  // User has not requested options, do it fast!
    return new ConvertToYV12(clip,args[1].AsBool(false),env);

  return ConvertToPlanarGeneric::CreateYUV420(args, NULL,env);
}

AVSValue AddAlphaPlane::Create(AVSValue args, void*, IScriptEnvironment* env)
{
  bool isMaskDefined = args[1].Defined();
  bool maskIsClip = false;
  // if mask is not defined and videoformat has Alpha then we return
  if(isMaskDefined && !args[1].IsClip() && !args[1].IsFloat())
    env->ThrowError("AddAlphaPlane: mask parameter should be clip or number");
  const VideoInfo& vi = args[0].AsClip()->GetVideoInfo();
  if (!isMaskDefined && (vi.IsPlanarRGBA() || vi.IsYUVA() || vi.IsRGB32() || vi.IsRGB64()))
    return args[0].AsClip();
  PClip alphaClip = nullptr;
  if (isMaskDefined && args[1].IsClip()) {
    const VideoInfo& viAlphaClip = args[1].AsClip()->GetVideoInfo();
    maskIsClip = true;
    if(viAlphaClip.BitsPerComponent() != vi.BitsPerComponent())
      env->ThrowError("AddAlphaPlane: alpha clip is of different bit depth");
    if(viAlphaClip.width != vi.width || viAlphaClip.height != vi.height )
      env->ThrowError("AddAlphaPlane: alpha clip is of different size");
    if (viAlphaClip.IsY())
      alphaClip = args[1].AsClip();
    else if (viAlphaClip.NumComponents() == 4) {
      AVSValue new_args[1] = { args[1].AsClip() };
      alphaClip = env->Invoke("ExtractA", AVSValue(new_args, 1)).AsClip();
    } else {
      env->ThrowError("AddAlphaPlane: alpha clip should be greyscale or should have alpha plane");
    }
    // alphaClip is always greyscale here
  }
  float maskAsFloat = -1.0f;
  if (!maskIsClip)
    maskAsFloat = (float)args[1].AsFloat(-1.0f);
  if (vi.IsRGB24()) {
    AVSValue new_args[1] = { args[0].AsClip() };
    PClip child = env->Invoke("ConvertToRGB32", AVSValue(new_args, 1)).AsClip();
    return new AddAlphaPlane(child, alphaClip, maskAsFloat, isMaskDefined, env);
  } else if(vi.IsRGB48()) {
    AVSValue new_args[1] = { args[0].AsClip() };
    PClip child = env->Invoke("ConvertToRGB64", AVSValue(new_args, 1)).AsClip();
    return new AddAlphaPlane(child, alphaClip, maskAsFloat, isMaskDefined, env);
  }
  return new AddAlphaPlane(args[0].AsClip(), alphaClip, maskAsFloat, isMaskDefined, env);
}

AddAlphaPlane::AddAlphaPlane(PClip _child, PClip _alphaClip, float _mask_f, bool isMaskDefined, IScriptEnvironment* env)
  : GenericVideoFilter(_child), alphaClip(_alphaClip)
{
  if(vi.IsYUY2())
    env->ThrowError("AddAlphaPlane: YUY2 is not allowed");
  if(vi.IsY())
    env->ThrowError("AddAlphaPlane: greyscale source is not allowed");
  if(vi.IsYUV() && !vi.Is420() && !vi.Is422() && !vi.Is444()) // e.g. 410
    env->ThrowError("AddAlphaPlane: YUV format not supported, must be 420, 422 or 444");
  if(!vi.IsYUV() && !vi.IsYUVA() && !vi.IsRGB())
    env->ThrowError("AddAlphaPlane: format not supported");

  pixelsize = vi.ComponentSize();
  bits_per_pixel = vi.BitsPerComponent();

  if (vi.IsYUV()) {
    int pixel_type = vi.pixel_type;
    if (vi.IsYV12())
      pixel_type = VideoInfo::CS_YV12;
    int new_pixel_type = (pixel_type & ~VideoInfo::CS_YUV) | VideoInfo::CS_YUVA;
    vi.pixel_type = new_pixel_type;
  } else if(vi.IsPlanarRGB()) {
    int pixel_type = vi.pixel_type;
    int new_pixel_type = (pixel_type & ~VideoInfo::CS_RGB_TYPE) | VideoInfo::CS_RGBA_TYPE;
    vi.pixel_type = new_pixel_type;
  }
  // RGB24 and RGB48 already converted to 32/64
  // RGB32, RGB64, YUVA and RGBA: no change

  // mask parameter. If none->max transparency

  if (!alphaClip) {
    int max_pixel_value = (1 << bits_per_pixel) - 1; // n/a for float
    if (!isMaskDefined) {
      mask_f = 1.0f;
      mask = max_pixel_value;
    }
    else {
      mask_f = _mask_f;
      mask = (mask_f < 0) ? 0 : (mask_f > max_pixel_value) ? max_pixel_value : (int)mask_f;
      mask = clamp(mask, 0, max_pixel_value);
      // no clamp for float
    }
  }
}

PVideoFrame AddAlphaPlane::GetFrame(int n, IScriptEnvironment* env)
{
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);
  if(vi.IsPlanar())
  {
    int planes_y[4] = { PLANAR_Y, PLANAR_U, PLANAR_V, PLANAR_A };
    int planes_r[4] = { PLANAR_G, PLANAR_B, PLANAR_R, PLANAR_A };
    int *planes = (vi.IsYUV() || vi.IsYUVA()) ? planes_y : planes_r;
    // copy existing 3 planes
    for (int p = 0; p < 3; ++p) {
      const int plane = planes[p];
      env->BitBlt(dst->GetWritePtr(plane), dst->GetPitch(plane), src->GetReadPtr(plane),
           src->GetPitch(plane), src->GetRowSize(plane), src->GetHeight(plane));
    }
  } else {
    // Packed RGB, already converted to RGB32 or RGB64
    env->BitBlt(dst->GetWritePtr(), dst->GetPitch(), src->GetReadPtr(),
      src->GetPitch(), src->GetRowSize(), src->GetHeight());
  }

  if (vi.IsPlanarRGBA() || vi.IsYUVA()) {
    if (alphaClip) {
      PVideoFrame srcAlpha = alphaClip->GetFrame(n, env);
      env->BitBlt(dst->GetWritePtr(PLANAR_A), dst->GetPitch(PLANAR_A), srcAlpha->GetReadPtr(PLANAR_Y),
        srcAlpha->GetPitch(PLANAR_Y), srcAlpha->GetRowSize(PLANAR_Y), srcAlpha->GetHeight(PLANAR_Y));
    }
    else {
      // default constant
      const int rowsizeA = dst->GetRowSize(PLANAR_A);
      const int dst_pitchA = dst->GetPitch(PLANAR_A);
      BYTE* dstp_a = dst->GetWritePtr(PLANAR_A);
      const int heightA = dst->GetHeight(PLANAR_A);

      switch (vi.ComponentSize())
      {
      case 1:
        fill_plane<BYTE>(dstp_a, heightA, rowsizeA, dst_pitchA, mask);
        break;
      case 2:
        fill_plane<uint16_t>(dstp_a, heightA, rowsizeA, dst_pitchA, mask);
        break;
      case 4:
        fill_plane<float>(dstp_a, heightA, rowsizeA, dst_pitchA, mask_f);
        break;
      }
    }
    return dst;
  }
  // RGB32 and RGB64

  BYTE* pf = dst->GetWritePtr();
  int pitch = dst->GetPitch();
  int rowsize = dst->GetRowSize();
  int height = dst->GetHeight();
  int width = vi.width;

  if (alphaClip) {
    // fill by alpha clip already converted to grey-only
    PVideoFrame srcAlpha = alphaClip->GetFrame(n, env);
    const BYTE* srcp_a = srcAlpha->GetReadPtr(PLANAR_Y);
    size_t pitch_a = srcAlpha->GetPitch(PLANAR_Y);

    pf += pitch * (vi.height - 1); // start from bottom: packed RGB is upside down

    if (vi.IsRGB32()) {
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x ++) {
          pf[x*4+3] = srcp_a[x];
        }
        pf -= pitch; // packed RGB is upside down
        srcp_a += pitch_a;
      }
    }
    else if (vi.IsRGB64()) {
      rowsize /= sizeof(uint16_t);
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x ++) {
          reinterpret_cast<uint16_t *>(pf)[x*4+3] = reinterpret_cast<const uint16_t *>(srcp_a)[x];
        }
        pf -= pitch; // packed RGB is upside down
        srcp_a += pitch_a;
      }
    }
  }
  else {
    // fill with constant
    if (vi.IsRGB32()) {
      for (int y = 0; y < height; y++) {
        for (int x = 3; x < rowsize; x += 4) {
          pf[x] = mask;
        }
        pf += pitch;
      }
    }
    else if (vi.IsRGB64()) {
      rowsize /= sizeof(uint16_t);
      for (int y = 0; y < height; y++) {
        for (int x = 3; x < rowsize; x += 4) {
          reinterpret_cast<uint16_t *>(pf)[x] = mask;
        }
        pf += pitch;
      }
    }
  }

  return dst;
}

AVSValue RemoveAlphaPlane::Create(AVSValue args, void*, IScriptEnvironment* env)
{
  // if videoformat has no Alpha then we return
  const VideoInfo& vi = args[0].AsClip()->GetVideoInfo();
  if(vi.IsPlanar() && (vi.IsYUV() || vi.IsPlanarRGB())) // planar and no alpha
    return args[0].AsClip();
  if (vi.IsYUY2()) // YUY2: no alpha
    return args[0].AsClip();
  if(vi.IsRGB24() || vi.IsRGB48()) // packed RGB and no alpha
    return args[0].AsClip();
  if (vi.IsRGB32()) {
    AVSValue new_args[1] = { args[0].AsClip() };
    return env->Invoke("ConvertToRGB24", AVSValue(new_args, 1)).AsClip();
  }
  if (vi.IsRGB64()) {
    AVSValue new_args[1] = { args[0].AsClip() };
    return env->Invoke("ConvertToRGB48", AVSValue(new_args, 1)).AsClip();
  }
  return new RemoveAlphaPlane(args[0].AsClip(), env);
}

RemoveAlphaPlane::RemoveAlphaPlane(PClip _child, IScriptEnvironment* env)
  : GenericVideoFilter(_child)
{
  if(vi.IsYUY2())
    env->ThrowError("RemoveAlphaPlane: YUY2 is not allowed");
  if(vi.IsY())
    env->ThrowError("RemoveAlphaPlane: greyscale source is not allowed");

  if (vi.IsYUVA()) {
    int pixel_type = vi.pixel_type;
    int new_pixel_type = (pixel_type & ~VideoInfo::CS_YUVA) | VideoInfo::CS_YUV;
    vi.pixel_type = new_pixel_type;
  } else if(vi.IsPlanarRGBA()) {
    int pixel_type = vi.pixel_type;
    int new_pixel_type = (pixel_type & ~VideoInfo::CS_RGBA_TYPE) | VideoInfo::CS_RGB_TYPE;
    vi.pixel_type = new_pixel_type;
  }
}

PVideoFrame RemoveAlphaPlane::GetFrame(int n, IScriptEnvironment* env)
{
  PVideoFrame src = child->GetFrame(n, env);
  // Packed RGB: already handled in ::Create through Invoke 32->24 or 64->48 conversion
  // only planar here
  int planes_y[4] = { PLANAR_Y, PLANAR_U, PLANAR_V, PLANAR_A };
  int planes_r[4] = { PLANAR_G, PLANAR_B, PLANAR_R, PLANAR_A };
  int *planes = (vi.IsYUV() || vi.IsYUVA()) ? planes_y : planes_r;
  // Abuse Subframe to snatch the YUV/GBR planes
  return env->SubframePlanar(src, 0, src->GetPitch(planes[0]), src->GetRowSize(planes[0]), src->GetHeight(planes[0]), 0, 0, src->GetPitch(planes[1]));

#if 0
  // BitBlt version. Kept for reference
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);
  // copy 3 planes w/o alpha
  for (int p = 0; p < 3; ++p) {
    const int plane = planes[p];
    env->BitBlt(dst->GetWritePtr(plane), dst->GetPitch(plane), src->GetReadPtr(plane),
      src->GetPitch(plane), src->GetRowSize(plane), src->GetHeight(plane));
  }
return dst;
#endif
}

