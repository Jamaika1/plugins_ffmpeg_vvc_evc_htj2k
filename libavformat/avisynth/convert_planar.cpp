// Avisynth v2.5.  Copyright 2002 Ben Rudiak-Gould et al.
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

// ConvertPlanar (c) 2005 by Klaus Post


#include "convert.h"
#include "convert_matrix.h"
#include "convert_helper.h"
#include "convert_planar.h"
#ifdef INTEL_INTRINSICS
#include "intel/convert_planar_sse.h"
#if defined(__AVX2__)
#include "intel/convert_planar_avx2.h"
#endif
#endif
#include "convert_bits.h"
#include "resample.h"
#include "planeswap.h"
#include "field.h"

#ifdef AVS_WINDOWS
    #include "avs/win.h"
#else
    #include "avs/posix.h"
#endif

#include "avs/alignment.h"
#include <algorithm>
#include <string>

template <typename pixel_t>
void fill_chroma(uint8_t * dstp_u, uint8_t * dstp_v, int height, int row_size, int pitch, pixel_t val)
{
  if (pitch == row_size) {
    size_t size = height * pitch / sizeof(pixel_t);
    std::fill_n(reinterpret_cast<pixel_t*>(dstp_u), size, val);
    std::fill_n(reinterpret_cast<pixel_t*>(dstp_v), size, val);
  }
  else {
    size_t size = row_size / sizeof(pixel_t);
    for (int i = 0; i < height; i++) {
      std::fill_n(reinterpret_cast<pixel_t*>(dstp_u), size, val);
      std::fill_n(reinterpret_cast<pixel_t*>(dstp_v), size, val);
      dstp_u += pitch;
      dstp_v += pitch;
    }
  }
}

template <typename pixel_t>
void fill_plane(uint8_t * dstp, int height, int row_size, int pitch, pixel_t val)
{
  if (pitch == row_size) {
    size_t size = height * pitch / sizeof(pixel_t);
    std::fill_n(reinterpret_cast<pixel_t*>(dstp), size, val);
  }
  else {
    size_t size = row_size / sizeof(pixel_t);
    for (int i = 0; i < height; i++) {
      std::fill_n(reinterpret_cast<pixel_t*>(dstp), size, val);
      dstp += pitch;
    }
  }
}

// specialize it
template void fill_plane<uint8_t>(uint8_t * dstp, int height, int row_size, int pitch, uint8_t val);
template void fill_plane<uint16_t>(uint8_t * dstp, int height, int row_size, int pitch, uint16_t val);
template void fill_plane<float>(uint8_t * dstp, int height, int row_size, int pitch, float val);
template void fill_chroma<uint8_t>(uint8_t * dstp_u, uint8_t * dstp_v, int height, int row_size, int pitch, uint8_t val);
template void fill_chroma<uint16_t>(uint8_t * dstp_u, uint8_t * dstp_v, int height, int row_size, int pitch, uint16_t val);
template void fill_chroma<float>(uint8_t * dstp_u, uint8_t * dstp_v, int height, int row_size, int pitch, float val);

ConvertToY::ConvertToY(PClip src, const char *matrix_name, IScriptEnvironment* env) : GenericVideoFilter(src) {
  yuy2_input = blit_luma_only = packed_rgb_input = planar_rgb_input = false;

  int target_pixel_type;
  int bits_per_pixel = vi.BitsPerComponent();
  switch (bits_per_pixel)
  {
  case 8: target_pixel_type = VideoInfo::CS_Y8; break;
  case 10: target_pixel_type = VideoInfo::CS_Y10; break;
  case 12: target_pixel_type = VideoInfo::CS_Y12; break;
  case 14: target_pixel_type = VideoInfo::CS_Y14; break;
  case 16: target_pixel_type = VideoInfo::CS_Y16; break;
  case 32: target_pixel_type = VideoInfo::CS_Y32; break;
  default:
    env->ThrowError("ConvertToY does not support %d-bit formats.", vi.BitsPerComponent());
  }

  pixelsize = vi.ComponentSize();

  if (vi.IsPlanar() && (vi.IsYUV() || vi.IsYUVA())) { // not for Planar RGB
    blit_luma_only = true;
    vi.pixel_type = target_pixel_type;
    return;
  }

  if (vi.IsYUY2()) {
    yuy2_input = true;
    vi.pixel_type = target_pixel_type;
    return;
  }

  if (vi.IsRGB()) { // also Planar RGB
    if (vi.IsPlanarRGB() || vi.IsPlanarRGBA())
      planar_rgb_input = true;
    else
      packed_rgb_input = true;
    pixel_step = vi.BytesFromPixels(1); // for packed RGB 3,4,6,8

    const int shift = 15; // internally 15 bits precision, still no overflow in calculations

    // input _ColorRange frame property can appear for RGB source (studio range limited rgb)
    auto frame0 = src->GetFrame(0, env);
    const AVSMap* props = env->getFramePropsRO(frame0);
    matrix_parse_merge_with_props(vi.IsRGB(), vi.IsRGB(), matrix_name, props, theMatrix, theColorRange, theOutColorRange, env);

    if (!do_BuildMatrix_Rgb2Yuv(theMatrix, theColorRange, theOutColorRange, shift, bits_per_pixel, /*ref*/matrix))
      env->ThrowError("ConvertToY: Unknown matrix.");

    theColorRange = theOutColorRange; // final frame property

    vi.pixel_type = target_pixel_type;

    return;
  }

  env->ThrowError("ConvertToY: Unknown input format");
}


PVideoFrame __stdcall ConvertToY::GetFrame(int n, IScriptEnvironment* env) {
  PVideoFrame src = child->GetFrame(n, env);

  if (blit_luma_only) {
    // Abuse Subframe to snatch the Y plane
    return env->Subframe(src, 0, src->GetPitch(PLANAR_Y), src->GetRowSize(PLANAR_Y), src->GetHeight(PLANAR_Y));
  }

  const BYTE* srcp = src->GetReadPtr();
  const int src_pitch = src->GetPitch();

  PVideoFrame dst = env->NewVideoFrameP(vi, &src);
  BYTE* dstp = dst->GetWritePtr(PLANAR_Y);
  const int dst_pitch = dst->GetPitch(PLANAR_Y);
  int rowsize = dst->GetRowSize(PLANAR_Y);
  int width = rowsize / pixelsize;
  int height = dst->GetHeight(PLANAR_Y);

  if (packed_rgb_input || planar_rgb_input) {
    auto props = env->getFramePropsRW(dst);
    update_Matrix_and_ColorRange(props, theMatrix, theColorRange, env);
  }

  if (yuy2_input) {
#ifdef INTEL_INTRINSICS
    if (env->GetCPUFlags() & CPUF_SSE2) {
      convert_yuy2_to_y8_sse2(srcp, dstp, src_pitch, dst_pitch, width, height);
    } else
#ifdef X86_32
    if (env->GetCPUFlags() & CPUF_MMX) {
      convert_yuy2_to_y8_mmx(srcp, dstp, src_pitch, dst_pitch, width, height);
    } else
#endif
#endif
    {
      for (int y=0; y < height; y++) {
        for (int x=0; x < width; x++) {
          dstp[x] = srcp[x*2];
        }
        srcp += src_pitch;
        dstp += dst_pitch;
      }
    }
    return dst;
  }

  if (packed_rgb_input) {
    srcp += src_pitch * (vi.height-1);  // We start at last line
#ifdef INTEL_INTRINSICS
    if ((pixelsize==1) && (env->GetCPUFlags() & CPUF_SSE2)) {
      if (pixel_step == 4) {
        convert_rgb32_to_y8_sse2(srcp, dstp, src_pitch, dst_pitch, width, height, matrix);
      } else if(pixel_step == 3) {
        convert_rgb24_to_y8_sse2(srcp, dstp, src_pitch, dst_pitch, width, height, matrix);
      } else if(pixel_step == 8) {
        //todo
        //convert_rgb64_to_y8_sse2(srcp, dstp, src_pitch, dst_pitch, width, height, matrix);
      } else if(pixel_step == 6) {
        // todo
        //convert_rgb48_to_y8_sse2(srcp, dstp, src_pitch, dst_pitch, width, height, matrix);
      }
      return dst;
    }

#ifdef X86_32
    if ((pixelsize==1) && (env->GetCPUFlags() & CPUF_MMX)) {
      if (pixel_step == 4) {
        convert_rgb32_to_y8_mmx(srcp, dstp, src_pitch, dst_pitch, width, height, matrix);
      } else {
        convert_rgb24_to_y8_mmx(srcp, dstp, src_pitch, dst_pitch, width, height, matrix);
      }
      return dst;
    }
#endif
#endif
    // Slow C
    const int srcMod = src_pitch + width * pixel_step;
    if(pixelsize==1) {
      for (int y=0; y<vi.height; y++) {
        for (int x=0; x<vi.width; x++) {
          const int Y = matrix.offset_y + ((matrix.y_b * srcp[0] + matrix.y_g * srcp[1] + matrix.y_r * srcp[2] + 16384) >> 15);
          dstp[x] = PixelClip(Y);  // All the safety we can wish for.
          srcp += pixel_step; // 3,4
        }
        srcp -= srcMod;
        dstp += dst_pitch;
      }
    }
    else { // pixelsize==2
      for (int y=0; y<vi.height; y++) {
        for (int x=0; x<vi.width; x++) {
          const uint16_t *srcp16 = reinterpret_cast<const uint16_t *>(srcp);
          // int overflows!
          const int Y = matrix.offset_y + (int)(((int64_t)(matrix.y_b * srcp16[0] + matrix.y_g * srcp16[1]) + (int64_t)matrix.y_r * srcp16[2] + 16384) >> 15);
          reinterpret_cast<uint16_t *>(dstp)[x] = clamp(Y,0,65535);  // All the safety we can wish for. packed RGB 65535

          // int64_t version is a bit faster
          //const float Y = matrix.offset_y_f + (matrix.b_f * srcp16[0] + matrix.g_f * srcp16[1] + matrix.r_f * srcp16[2]);
          //reinterpret_cast<uint16_t *>(dstp)[x] = (uint16_t)clamp((int)Y,0,65535);  // All the safety we can wish for.
          srcp += pixel_step; // 6,8
        }
        srcp -= srcMod;
        dstp += dst_pitch;
      }
    }
  }

  if (planar_rgb_input)
  {
#ifdef INTEL_INTRINSICS
    // todo: SSE2, like convert_planarrgb_to_yuv_uint8_14_sse2 and convert_planarrgb_to_yuv_uint16_float_sse2
#endif
    const BYTE *srcpG = src->GetReadPtr(PLANAR_G);
    const BYTE *srcpB = src->GetReadPtr(PLANAR_B);
    const BYTE *srcpR = src->GetReadPtr(PLANAR_R);
    const int pitchG = src->GetPitch(PLANAR_G);
    const int pitchB = src->GetPitch(PLANAR_B);
    const int pitchR = src->GetPitch(PLANAR_R);
    if(pixelsize==1) {
      for (int y=0; y<vi.height; y++) {
        for (int x=0; x<vi.width; x++) {
          const int Y = matrix.offset_y + ((matrix.y_b * srcpB[x] + matrix.y_g * srcpG[x] + matrix.y_r * srcpR[x] + 16384) >> 15);
          dstp[x] = PixelClip(Y);  // All the safety we can wish for.
        }
        srcpG += pitchG; srcpB += pitchB; srcpR += pitchR;
        dstp += dst_pitch;
      }
    } else if(pixelsize==2) {
      int max_pixel_value = (1 << vi.BitsPerComponent()) - 1;
      for (int y=0; y<vi.height; y++) {
        for (int x=0; x<vi.width; x++) {
          // int overflows!
          const int Y = matrix.offset_y +
            (((int64_t)matrix.y_b * reinterpret_cast<const uint16_t *>(srcpB)[x] +
              (int64_t)matrix.y_g * reinterpret_cast<const uint16_t *>(srcpG)[x] +
              (int64_t)matrix.y_r * reinterpret_cast<const uint16_t *>(srcpR)[x] +
              16384) >> 15);
          reinterpret_cast<uint16_t *>(dstp)[x] = (uint16_t)clamp(Y,0,max_pixel_value);  // All the safety we can wish for.
        }
        srcpG += pitchG; srcpB += pitchB; srcpR += pitchR;
        dstp += dst_pitch;
      }
    }
    else if (pixelsize==4) {
      for (int y=0; y<vi.height; y++) {
        for (int x=0; x<vi.width; x++) {
          const float Y = matrix.offset_y_f +
            (matrix.y_b_f * reinterpret_cast<const float *>(srcpB)[x] +
             matrix.y_g_f * reinterpret_cast<const float *>(srcpG)[x] +
             matrix.y_r_f * reinterpret_cast<const float *>(srcpR)[x]
            );
          reinterpret_cast<float *>(dstp)[x] = Y;  // no clamping
        }
        srcpG += pitchG; srcpB += pitchB; srcpR += pitchR;
        dstp += dst_pitch;
      }
    }
  }
  return dst;
}

AVSValue __cdecl ConvertToY::Create(AVSValue args, void* user_data, IScriptEnvironment* env) {
  PClip clip = args[0].AsClip();
  bool only_8bit = reinterpret_cast<intptr_t>(user_data) == 0;
  if (only_8bit && clip->GetVideoInfo().BitsPerComponent() != 8)
    env->ThrowError("ConvertToY8: only 8 bit sources allowed");

  if (clip->GetVideoInfo().NumComponents() == 1)
    return clip;

  return new ConvertToY(clip, args[1].AsString(0), env);
}

/*****************************************************
 * ConvertRGBToYUV444
 ******************************************************/

ConvertRGBToYUV444::ConvertRGBToYUV444(PClip src, const char *matrix_name, bool keep_packedrgb_alpha, IScriptEnvironment* env)
  : GenericVideoFilter(src)
{
  if (!vi.IsRGB())
    env->ThrowError("ConvertRGBToYV24/YUV444: Only RGB data input accepted");

  // ChromaInPlacement parameter exists, (default none/-1) + input frame properties; 'left'-ish _ChromaLocation is allowed, checked later
  auto frame0 = src->GetFrame(0, env);
  const AVSMap* props = env->getFramePropsRO(frame0);

  // input _ColorRange frame property can appear for RGB source (studio range limited rgb)
  matrix_parse_merge_with_props(true /*in rgb*/, false /*out yuv*/, matrix_name, props, theMatrix, theColorRange, theOutColorRange, env);

  const int shift = 15; // internally 15 bits precision, still no overflow in calculations
  int bits_per_pixel = vi.BitsPerComponent();

  if (!do_BuildMatrix_Rgb2Yuv(theMatrix, theColorRange, theOutColorRange, shift, bits_per_pixel, /*ref*/matrix))
    env->ThrowError("ConvertRGBToYV24/YUV444: Unknown matrix.");

  theColorRange = theOutColorRange; // final frame property

  isPlanarRGBfamily = vi.IsPlanarRGB() || vi.IsPlanarRGBA();
  hasAlpha = vi.IsPlanarRGBA() || (keep_packedrgb_alpha && (vi.IsRGB32() || vi.IsRGB64()));
  // for packed RGB it depends: ConvertToYUVAxxx() can force YUVA target option
  if (isPlanarRGBfamily)
  {
    pixel_step = hasAlpha ? -2 : -1;
    switch (vi.BitsPerComponent())
    {
    case 8: vi.pixel_type  = hasAlpha ? VideoInfo::CS_YUVA444    : VideoInfo::CS_YV24; break;
    case 10: vi.pixel_type = hasAlpha ? VideoInfo::CS_YUVA444P10 : VideoInfo::CS_YUV444P10; break;
    case 12: vi.pixel_type = hasAlpha ? VideoInfo::CS_YUVA444P12 : VideoInfo::CS_YUV444P12; break;
    case 14: vi.pixel_type = hasAlpha ? VideoInfo::CS_YUVA444P14 : VideoInfo::CS_YUV444P14; break;
    case 16: vi.pixel_type = hasAlpha ? VideoInfo::CS_YUVA444P16 : VideoInfo::CS_YUV444P16; break;
    case 32: vi.pixel_type = hasAlpha ? VideoInfo::CS_YUVA444PS  : VideoInfo::CS_YUV444PS; break;
    }
  } else { // packed RGB24/32/48/64
    // for compatibility reasons ConvertToYUVxxx target is not YUVA even
    // if original has alpha, such as RGB32.
    // Unlike ConvertToYUVA which can force keeping RGB32's or RGB64's alpha
    pixel_step = vi.BytesFromPixels(1); // 3,4 for packed 8 bit, 6,8 for
    switch(vi.ComponentSize())
    {
    case 1: vi.pixel_type = hasAlpha ? VideoInfo::CS_YUVA444 : VideoInfo::CS_YV24; break;
    case 2: vi.pixel_type = hasAlpha ? VideoInfo::CS_YUVA444P16 : VideoInfo::CS_YUV444P16; break;
    }
  }

}



template<typename pixel_t, int bits_per_pixel>
static void convert_planarrgb_to_yuv_int_c(BYTE *(&dstp)[3], int (&dstPitch)[3], const BYTE *(&srcp)[3], const int (&srcPitch)[3], int width, int height, const ConversionMatrix &m)
{
  const bool has_offset_rgb = 0 != m.offset_rgb;

  const pixel_t half = 1 << (bits_per_pixel - 1 );
  typedef typename std::conditional < bits_per_pixel < 16, int, int64_t>::type sum_t;
  const int limit = (1 << bits_per_pixel) - 1;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int g = reinterpret_cast<const pixel_t *>(srcp[0])[x];
      int b = reinterpret_cast<const pixel_t *>(srcp[1])[x];
      int r = reinterpret_cast<const pixel_t *>(srcp[2])[x];
      if (has_offset_rgb) {
        b = b + m.offset_rgb;
        g = g + m.offset_rgb;
        r = r + m.offset_rgb;
      }
      int Y = m.offset_y + (int)(((sum_t)m.y_b * b + (sum_t)m.y_g * g + (sum_t)m.y_r * r + 16384)>>15);
      int U = half + (int)(((sum_t)m.u_b * b + (sum_t)m.u_g * g + (sum_t)m.u_r * r + 16384) >> 15);
      int V = half + (int)(((sum_t)m.v_b * b + (sum_t)m.v_g * g + (sum_t)m.v_r * r + 16384) >> 15);
      reinterpret_cast<pixel_t *>(dstp[0])[x] = (pixel_t)clamp(Y, 0, limit);
      reinterpret_cast<pixel_t *>(dstp[1])[x] = (pixel_t)clamp(U, 0, limit);
      reinterpret_cast<pixel_t *>(dstp[2])[x] = (pixel_t)clamp(V, 0, limit);
    }
    srcp[0] += srcPitch[0];
    srcp[1] += srcPitch[1];
    srcp[2] += srcPitch[2];
    dstp[0] += dstPitch[0];
    dstp[1] += dstPitch[1];
    dstp[2] += dstPitch[2];
  }
}

static void convert_planarrgb_to_yuv_float_c(BYTE *(&dstp)[3], int (&dstPitch)[3], const BYTE *(&srcp)[3], const int (&srcPitch)[3], int width, int height, const ConversionMatrix &m)
{
  const bool has_offset_rgb = 0 != m.offset_rgb_f;

  constexpr float limit = 1.0f; // we clamp on RGB conversions for float
  constexpr float limit_lo_chroma = -0.5f; // checked before shift
  constexpr float limit_hi_chroma = 0.5f;
  constexpr float half_chroma = 0.0f; // no more 0.5 center
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      float g = reinterpret_cast<const float *>(srcp[0])[x];
      float b = reinterpret_cast<const float *>(srcp[1])[x];
      float r = reinterpret_cast<const float *>(srcp[2])[x];
      if (has_offset_rgb) {
        b = b + m.offset_rgb_f;
        g = g + m.offset_rgb_f;
        r = r + m.offset_rgb_f;
      }
      float Y = m.offset_y_f + (m.y_b_f * b + m.y_g_f * g + m.y_r_f * r);
      float U = half_chroma + (m.u_b_f * b + m.u_g_f * g + m.u_r_f * r);
      float V = half_chroma + (m.v_b_f * b + m.v_g_f * g + m.v_r_f * r);
      // All the safety we can wish for.
      // theoretical question: should we clamp here?
      reinterpret_cast<float *>(dstp[0])[x] = clamp(Y, 0.0f, limit);
      reinterpret_cast<float *>(dstp[1])[x] = clamp(U, limit_lo_chroma, limit_hi_chroma);
      reinterpret_cast<float *>(dstp[2])[x] = clamp(V, limit_lo_chroma, limit_hi_chroma);
    }
    srcp[0] += srcPitch[0];
    srcp[1] += srcPitch[1];
    srcp[2] += srcPitch[2];
    dstp[0] += dstPitch[0];
    dstp[1] += dstPitch[1];
    dstp[2] += dstPitch[2];
  }
}

template<int pixel_step, bool copyalpha>
static void convert_rgb24or32_to_yuv444_c(BYTE* dstY, BYTE* dstU, BYTE* dstV, BYTE* dstA, const BYTE* srcp, size_t Ypitch, size_t UVpitch, size_t Apitch, size_t Spitch, size_t width, size_t height, const ConversionMatrix& m)
{
  const bool has_offset_rgb = 0 != m.offset_rgb;

  // RGB24 (pixel_step=3)
  // RGB32 (pixel_step=4) optional copyalpha
  srcp += Spitch * (height - 1);  // We start at last line
  const size_t Sstep = Spitch + width * pixel_step;
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      int b = srcp[0];
      int g = srcp[1];
      int r = srcp[2];
      if (has_offset_rgb) {
        b = b + m.offset_rgb;
        g = g + m.offset_rgb;
        r = r + m.offset_rgb;
      }
      int Y = m.offset_y + (((int)m.y_b * b + (int)m.y_g * g + (int)m.y_r * r + 16384) >> 15);
      int U = 128 + (((int)m.u_b * b + (int)m.u_g * g + (int)m.u_r * r + 16384) >> 15);
      int V = 128 + (((int)m.v_b * b + (int)m.v_g * g + (int)m.v_r * r + 16384) >> 15);
      dstY[x] = PixelClip(Y);  // All the safety we can wish for.
      dstU[x] = PixelClip(U);
      dstV[x] = PixelClip(V);
      if constexpr (copyalpha) {
        dstA[x] = srcp[3];
      }
      srcp += pixel_step;
    }
    srcp -= Sstep; // packed RGBs are upside down
    dstY += Ypitch;
    dstU += UVpitch;
    dstV += UVpitch;
    if constexpr (copyalpha) {
      dstA += Apitch;
    }
  }
}

template<int pixel_step, bool copyalpha>
static void convert_rgb48or64_to_yuv444_c(BYTE* dstY, BYTE* dstU, BYTE* dstV, BYTE* dstA, const BYTE* srcp, size_t Ypitch, size_t UVpitch, size_t Apitch, size_t Spitch, size_t width, size_t height, const ConversionMatrix& m)
{
  const bool has_offset_rgb = 0 != m.offset_rgb;

  // RGB48 (pixel_step=6)
  // RGB64 (pixel_step=8) optional copyalpha
  srcp += Spitch * (height - 1);  // We start at last line
  const size_t Sstep = Spitch + width * pixel_step;
  uint16_t* dstY16 = reinterpret_cast<uint16_t*>(dstY);
  uint16_t* dstU16 = reinterpret_cast<uint16_t*>(dstU);
  uint16_t* dstV16 = reinterpret_cast<uint16_t*>(dstV);
  uint16_t* dstA16 = reinterpret_cast<uint16_t*>(dstA);
  size_t Ypitch16 = Ypitch / sizeof(uint16_t);
  size_t UVpitch16 = UVpitch / sizeof(uint16_t);
  size_t Apitch16 = Apitch / sizeof(uint16_t);
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      int b = reinterpret_cast<const uint16_t*>(srcp)[0];
      int g = reinterpret_cast<const uint16_t*>(srcp)[1];
      int r = reinterpret_cast<const uint16_t*>(srcp)[2];
      if (has_offset_rgb) {
        b = b + m.offset_rgb;
        g = g + m.offset_rgb;
        r = r + m.offset_rgb;
      }
      int Y = m.offset_y + (((int64_t)m.y_b * b + (int64_t)m.y_g * g + (int64_t)m.y_r * r + 16384) >> 15);
      int U = 32768 + (((int64_t)m.u_b * b + (int64_t)m.u_g * g + (int64_t)m.u_r * r + 16384) >> 15);
      int V = 32768 + (((int64_t)m.v_b * b + (int64_t)m.v_g * g + (int64_t)m.v_r * r + 16384) >> 15);
      dstY16[x] = (uint16_t)clamp(Y, 0, 65535);// PixelClip(Y);  // All the safety we can wish for.
      dstU16[x] = (uint16_t)clamp(U, 0, 65535);
      dstV16[x] = (uint16_t)clamp(V, 0, 65535);
      if constexpr (copyalpha) {
        dstA16[x] = reinterpret_cast<const uint16_t*>(srcp)[3];
      }
      srcp += pixel_step;
    }
    srcp -= Sstep; // packed RGBs are upside down
    dstY16 += Ypitch16;
    dstU16 += UVpitch16;
    dstV16 += UVpitch16;
    if constexpr (copyalpha) {
      dstA16 += Apitch16;
    }
  }
}

PVideoFrame __stdcall ConvertRGBToYUV444::GetFrame(int n, IScriptEnvironment* env)
{
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);

  auto props = env->getFramePropsRW(dst);
  update_Matrix_and_ColorRange(props, theMatrix, theColorRange, env);

  const BYTE* srcp = src->GetReadPtr();

  BYTE* dstY = dst->GetWritePtr(PLANAR_Y);
  BYTE* dstU = dst->GetWritePtr(PLANAR_U);
  BYTE* dstV = dst->GetWritePtr(PLANAR_V);

  const int Spitch = src->GetPitch();

  const int Ypitch = dst->GetPitch(PLANAR_Y);
  const int UVpitch = dst->GetPitch(PLANAR_U);

  if (pixel_step != 4 && pixel_step != 3 && pixel_step != 8 && pixel_step != 6 && pixel_step != -1 && pixel_step != -2) {
    env->ThrowError("Invalid pixel step. This is a bug.");
  }
  if ((pixel_step == 3 || pixel_step == 6) && hasAlpha) {
    env->ThrowError("Alpha copy from RGB24 or RGB48 is not possible, this is a bug.");
  }

  const bool packedRGBsource = pixel_step > 0;

  if (packedRGBsource) {
    // hasAlpha is true only if Alpha must be preserved from RGB32 or RGB64 on converting to YUVA444
    BYTE* dstA = hasAlpha ? dst->GetWritePtr(PLANAR_A) : nullptr;
    const int Apitch = hasAlpha ? dst->GetPitch(PLANAR_A) : 0;

#ifdef INTEL_INTRINSICS
    // sse2 for 8 bit only (pixel_step==3,4), todo
    if (((pixel_step == 3) || (pixel_step == 4)) && (env->GetCPUFlags() & CPUF_SSE2)) {
      if (pixel_step == 3)
        convert_rgb24_to_yv24_sse2(dstY, dstU, dstV, srcp, Ypitch, UVpitch, Spitch, vi.width, vi.height, matrix);
      else if (pixel_step == 4 && !hasAlpha)
        convert_rgb32_to_yv24_sse2<false>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
      else if (pixel_step == 4 && hasAlpha)
        convert_rgb32_to_yv24_sse2<true>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
      return dst;
    }

#ifdef X86_32
    if (((pixel_step == 3) || (pixel_step == 4)) && (env->GetCPUFlags() & CPUF_MMX)) {
      if (pixel_step == 3)
        convert_rgb24_to_yv24_mmx(dstY, dstU, dstV, srcp, Ypitch, UVpitch, Spitch, vi.width, vi.height, matrix);
      else if (pixel_step == 4 && !hasAlpha)
        convert_rgb32_to_yv24_mmx<false>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
      else if (pixel_step == 4 && hasAlpha)
        convert_rgb32_to_yv24_mmx<true>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
      return dst;
  }
#endif
#endif
    // SIMD/C for planar RGB and C-code for packed RGB

    if(pixel_step == 3)
      convert_rgb24or32_to_yuv444_c<3, false>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
    else if(pixel_step == 4 && !hasAlpha)
      convert_rgb24or32_to_yuv444_c<4, false>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
    else if (pixel_step == 4 && hasAlpha)
      convert_rgb24or32_to_yuv444_c<4, true>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
    else if (pixel_step == 6)
      convert_rgb48or64_to_yuv444_c<6, false>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
    else if (pixel_step == 8 && !hasAlpha)
      convert_rgb48or64_to_yuv444_c<8, false>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
    else if (pixel_step == 8 && hasAlpha) {
      convert_rgb48or64_to_yuv444_c<8, true>(dstY, dstU, dstV, dstA, srcp, Ypitch, UVpitch, Apitch, Spitch, vi.width, vi.height, matrix);
    }
    return dst;
  }

  // isPlanarRGBfamily
  if(hasAlpha) {
    // simple copy
    BYTE* dstA = dst->GetWritePtr(PLANAR_A);
    const int Apitch = dst->GetPitch(PLANAR_A);
    env->BitBlt(dstA, Apitch, src->GetReadPtr(PLANAR_A), src->GetPitch(PLANAR_A), src->GetRowSize(PLANAR_A_ALIGNED), src->GetHeight(PLANAR_A));
  }
  int bits_per_pixel = vi.BitsPerComponent();

  const BYTE *srcps[3] = { src->GetReadPtr(PLANAR_G), src->GetReadPtr(PLANAR_B), src->GetReadPtr(PLANAR_R) };
  const int srcPitch[3] = { src->GetPitch(PLANAR_G), src->GetPitch(PLANAR_B), src->GetPitch(PLANAR_R) };

  BYTE *dstp[3] = { dstY, dstU, dstV };
  int dstPitch[3] = { Ypitch, UVpitch, UVpitch };

#ifdef INTEL_INTRINSICS
  // integer arithmetic restricted to 8 bits
  if (bits_per_pixel == 8 && (env->GetCPUFlags() & CPUF_SSE2))
  {
    convert_planarrgb_to_yuv_uint8_14_sse2<uint8_t, 8>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix);
#if 0
      // integer arithmetic - quicker for the similar lane width that the float version
    // available from 8 to 14 bits, for precision reasons used only for 8 bits.
    switch (bits_per_pixel) {
    case 8: convert_planarrgb_to_yuv_uint8_14_sse2<uint8_t, 8>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
    case 10: convert_planarrgb_to_yuv_uint8_14_sse2<uint16_t, 10>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
    case 12: convert_planarrgb_to_yuv_uint8_14_sse2<uint16_t, 12>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
    case 14: convert_planarrgb_to_yuv_uint8_14_sse2<uint16_t, 14>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
    }
#endif
    return dst;
  }

  // float arithmetic - more precision
  // decided to use it for 10+bits
  if (bits_per_pixel >= 10 && (env->GetCPUFlags() & CPUF_SSE2)) {
    if (bits_per_pixel == 32) // float 32 bit
      convert_planarrgb_to_yuv_float_sse2(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix);
    else
#if defined(__AVX2__)
    if (env->GetCPUFlags() & CPUF_AVX2) {
      switch (bits_per_pixel) {
      case 10: convert_planarrgb_to_yuv_uint16_avx2<10>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 12: convert_planarrgb_to_yuv_uint16_avx2<12>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 14: convert_planarrgb_to_yuv_uint16_avx2<14>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 16: convert_planarrgb_to_yuv_uint16_avx2<16>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      }
    } else
#endif
    if (env->GetCPUFlags() & CPUF_SSE4_1) {
      switch (bits_per_pixel) {
      case 10: convert_planarrgb_to_yuv_uint16_sse41<10>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 12: convert_planarrgb_to_yuv_uint16_sse41<12>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 14: convert_planarrgb_to_yuv_uint16_sse41<14>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 16: convert_planarrgb_to_yuv_uint16_sse41<16>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      }
    }
    else {
      switch (bits_per_pixel) {
      case 10: convert_planarrgb_to_yuv_uint16_sse2<10>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 12: convert_planarrgb_to_yuv_uint16_sse2<12>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 14: convert_planarrgb_to_yuv_uint16_sse2<14>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      case 16: convert_planarrgb_to_yuv_uint16_sse2<16>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
      }
    }
    return dst;
  }
#endif

  switch(bits_per_pixel) {
  case 8: convert_planarrgb_to_yuv_int_c<uint8_t, 8>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
  case 10: convert_planarrgb_to_yuv_int_c<uint16_t, 10>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
  case 12: convert_planarrgb_to_yuv_int_c<uint16_t, 12>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
  case 14: convert_planarrgb_to_yuv_int_c<uint16_t, 14>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
  case 16: convert_planarrgb_to_yuv_int_c<uint16_t, 16>(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
  case 32: convert_planarrgb_to_yuv_float_c(dstp, dstPitch, srcps, srcPitch, vi.width, vi.height, matrix); break;
  }
  return dst;
}



/*****************************************************
 * ConvertYV24ToRGB
 *
 * (c) Klaus Post, 2005
 * Generic 4:4:4(:4), 16 bit and Planar RGB(A) support 2016 by PF
 ******************************************************/


ConvertYUV444ToRGB::ConvertYUV444ToRGB(PClip src, const char *matrix_name, int _pixel_step, IScriptEnvironment* env)
 : GenericVideoFilter(src), pixel_step(_pixel_step)
{

  if (!vi.Is444())
    env->ThrowError("ConvertYUV444ToRGB: Only 4:4:4 data input accepted");

  auto frame0 = child->GetFrame(0, env);
  const AVSMap* props = env->getFramePropsRO(frame0);
  matrix_parse_merge_with_props(false /*in yuv*/, true /*out rgb*/, matrix_name, props, theMatrix, theColorRange, theOutColorRange, env);

  const int shift = 13; // for integer arithmetic, over 13 bits would overflow the internal calculation
  const int bits_per_pixel = vi.BitsPerComponent();

  if (!do_BuildMatrix_Yuv2Rgb(theMatrix, theColorRange, theOutColorRange, shift, bits_per_pixel, /*ref*/matrix))
    env->ThrowError("ConvertYV24ToRGB: Unknown matrix.");

  theOutMatrix = Matrix_e::AVS_MATRIX_RGB;
  //theOutColorRange = ColorRange_e::AVS_RANGE_FULL; // PC709 must keep the input one!

  switch (pixel_step)
  {
  case -1: case -2:
    switch (vi.BitsPerComponent())
    {
    case 8:  vi.pixel_type = pixel_step == -2 ? VideoInfo::CS_RGBAP : VideoInfo::CS_RGBP; break;
    case 10: vi.pixel_type = pixel_step == -2 ? VideoInfo::CS_RGBAP10 : VideoInfo::CS_RGBP10; break;
    case 12: vi.pixel_type = pixel_step == -2 ? VideoInfo::CS_RGBAP12 : VideoInfo::CS_RGBP12; break;
    case 14: vi.pixel_type = pixel_step == -2 ? VideoInfo::CS_RGBAP14 : VideoInfo::CS_RGBP14; break;
    case 16: vi.pixel_type = pixel_step == -2 ? VideoInfo::CS_RGBAP16 : VideoInfo::CS_RGBP16; break;
    case 32: vi.pixel_type = pixel_step == -2 ? VideoInfo::CS_RGBAPS : VideoInfo::CS_RGBPS; break;
    default:
      env->ThrowError("ConvertYUV444ToRGB: invalid vi.BitsPerComponent(): %d", vi.BitsPerComponent());
    }
    break;
  case 3: vi.pixel_type = VideoInfo::CS_BGR24; break;
  case 4: vi.pixel_type = VideoInfo::CS_BGR32; break;
  case 6: vi.pixel_type = VideoInfo::CS_BGR48; break;
  case 8: vi.pixel_type = VideoInfo::CS_BGR64; break;
  default:
    env->ThrowError("ConvertYUV444ToRGB: invalid pixel step: %d", pixel_step);
  }

}


PVideoFrame __stdcall ConvertYUV444ToRGB::GetFrame(int n, IScriptEnvironment* env)
{
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);

  auto props = env->getFramePropsRW(dst);
  update_Matrix_and_ColorRange(props, theOutMatrix, theOutColorRange, env);
  update_ChromaLocation(props, -1, env); // RGB target: delete _ChromaLocation

  const BYTE* srcY = src->GetReadPtr(PLANAR_Y);
  const BYTE* srcU = src->GetReadPtr(PLANAR_U);
  const BYTE* srcV = src->GetReadPtr(PLANAR_V);
  const BYTE* srcA = src->GetReadPtr(PLANAR_A);

  BYTE* dstp = dst->GetWritePtr();

  const int src_pitch_y = src->GetPitch(PLANAR_Y);
  const int src_pitch_uv = src->GetPitch(PLANAR_U);
  const int src_pitch_a = src->GetPitch(PLANAR_A); // zero if no Alpha

  const int dst_pitch = dst->GetPitch();


#ifdef INTEL_INTRINSICS
  // todo: SIMD for not only 8 bit RGB
  // packed RGB24 and RGB32

#if defined(__AVX2__)
  if ((env->GetCPUFlags() & CPUF_AVX2) && (pixel_step == 3 || pixel_step == 4)) {
    if (pixel_step == 4) {
      // RGB32
      if (src_pitch_a) // move alpha channel from YUVA
        convert_yv24_to_rgb_avx2<4, true>(dstp, srcY, srcU, srcV, srcA, dst_pitch, src_pitch_y, src_pitch_uv, src_pitch_a, vi.width, vi.height, matrix);
      else
        convert_yv24_to_rgb_avx2<4, false>(dstp, srcY, srcU, srcV, srcA, dst_pitch, src_pitch_y, src_pitch_uv, src_pitch_a, vi.width, vi.height, matrix);
    }
    else {
      // RGB24
      convert_yv24_to_rgb_avx2<3, false>(dstp, srcY, srcU, srcV, srcA, dst_pitch, src_pitch_y, src_pitch_uv, src_pitch_a, vi.width, vi.height, matrix);
    }
    return dst;
  } else
#endif
  if ((env->GetCPUFlags() & CPUF_SSE2) && (pixel_step==3 || pixel_step==4)) {
    if (pixel_step == 4) {
      if(src_pitch_a) // move alpha channel from YUVA
        convert_yv24_to_rgb_sse2<4, true>(dstp, srcY, srcU, srcV, srcA, dst_pitch, src_pitch_y, src_pitch_uv, src_pitch_a, vi.width, vi.height, matrix);
      else
        convert_yv24_to_rgb_sse2<4, false>(dstp, srcY, srcU, srcV, srcA, dst_pitch, src_pitch_y, src_pitch_uv, src_pitch_a, vi.width, vi.height, matrix);
    } else {
      if (env->GetCPUFlags() & CPUF_SSSE3) {
        convert_yv24_to_rgb_ssse3<3, false>(dstp, srcY, srcU, srcV, srcA, dst_pitch, src_pitch_y, src_pitch_uv, src_pitch_a, vi.width, vi.height, matrix);
      } else {
        convert_yv24_to_rgb_sse2<3, false>(dstp, srcY, srcU, srcV, srcA, dst_pitch, src_pitch_y, src_pitch_uv, src_pitch_a, vi.width, vi.height, matrix);
      }
    }
    return dst;
  }

#ifdef X86_32
  // packed RGB24 and RGB32
  if ((src_pitch_a==0) && (env->GetCPUFlags() & CPUF_MMX) && (pixel_step==3 || pixel_step==4)) {
    if (pixel_step == 4) {
      convert_yv24_to_rgb_mmx<4>(dstp, srcY, srcU, srcV, dst_pitch, src_pitch_y, src_pitch_uv, vi.width, vi.height, matrix);
    } else {
      convert_yv24_to_rgb_mmx<3>(dstp, srcY, srcU, srcV, dst_pitch, src_pitch_y, src_pitch_uv, vi.width, vi.height, matrix);
    }
    return dst;
  }
#endif
#endif

  //Slow C-code.
  auto round_mask_plus_rgb_offset_i = 4096 + (matrix.offset_rgb << 13);

  dstp += dst_pitch * (vi.height-1);  // We start at last line. Not for Planar RGB
  bool srcHasAlpha = (src_pitch_a != 0);
  if (pixel_step == 4) { // RGB32
    for (int y = 0; y < vi.height; y++) {
      for (int x = 0; x < vi.width; x++) {
        int Y = srcY[x] + matrix.offset_y;
        int U = srcU[x] - 128;
        int V = srcV[x] - 128;
        uint8_t a = srcHasAlpha ? srcA[x] : 255; // YUVA aware
        int b = (((int)matrix.y_b * Y + (int)matrix.u_b * U + (int)matrix.v_b * V + round_mask_plus_rgb_offset_i)>>13);
        int g = (((int)matrix.y_g * Y + (int)matrix.u_g * U + (int)matrix.v_g * V + round_mask_plus_rgb_offset_i)>>13);
        int r = (((int)matrix.y_r * Y + (int)matrix.u_r * U + (int)matrix.v_r * V + round_mask_plus_rgb_offset_i)>>13);
        dstp[x*4+0] = PixelClip(b);  // All the safety we can wish for.
        dstp[x*4+1] = PixelClip(g);  // Probably needed here.
        dstp[x*4+2] = PixelClip(r);
        dstp[x*4+3] = a; // alpha
      }
      dstp -= dst_pitch;
      srcY += src_pitch_y;
      srcU += src_pitch_uv;
      srcV += src_pitch_uv;
      srcA += src_pitch_a;
    }
  } else if (pixel_step == 3) { // RGB24
    const int Dstep = dst_pitch + (vi.width * pixel_step);
    for (int y = 0; y < vi.height; y++) {
      for (int x = 0; x < vi.width; x++) {
        int Y = srcY[x] + matrix.offset_y;
        int U = srcU[x] - 128;
        int V = srcV[x] - 128;
        int b = (((int)matrix.y_b * Y + (int)matrix.u_b * U + (int)matrix.v_b * V + round_mask_plus_rgb_offset_i)>>13);
        int g = (((int)matrix.y_g * Y + (int)matrix.u_g * U + (int)matrix.v_g * V + round_mask_plus_rgb_offset_i)>>13);
        int r = (((int)matrix.y_r * Y + (int)matrix.u_r * U + (int)matrix.v_r * V + round_mask_plus_rgb_offset_i)>>13);
        dstp[0] = PixelClip(b);  // All the safety we can wish for.
        dstp[1] = PixelClip(g);  // Probably needed here.
        dstp[2] = PixelClip(r);
        dstp += pixel_step;
      }
      dstp -= Dstep;
      srcY += src_pitch_y;
      srcU += src_pitch_uv;
      srcV += src_pitch_uv;
    }
  } else if (pixel_step == 8) { // RGB64
    for (int y = 0; y < vi.height; y++) {
        for (int x = 0; x < vi.width; x++) {
            int Y = reinterpret_cast<const uint16_t *>(srcY)[x] + matrix.offset_y;
            int U = reinterpret_cast<const uint16_t *>(srcU)[x] - 32768;
            int V = reinterpret_cast<const uint16_t *>(srcV)[x] - 32768;
            uint16_t a = srcHasAlpha ? reinterpret_cast<const uint16_t *>(srcA)[x] : 65535; // YUVA aware
            int b = (((int64_t)matrix.y_b * Y + (int64_t)matrix.u_b * U + (int64_t)matrix.v_b * V + round_mask_plus_rgb_offset_i)>>13);
            int g = (((int64_t)matrix.y_g * Y + (int64_t)matrix.u_g * U + (int64_t)matrix.v_g * V + round_mask_plus_rgb_offset_i)>>13);
            int r = (((int64_t)matrix.y_r * Y + (int64_t)matrix.u_r * U + (int64_t)matrix.v_r * V + round_mask_plus_rgb_offset_i)>>13);
            reinterpret_cast<uint16_t *>(dstp)[x*4+0] = clamp(b,0,65535);  // All the safety we can wish for.
            reinterpret_cast<uint16_t *>(dstp)[x*4+1] = clamp(g,0,65535);  // Probably needed here.
            reinterpret_cast<uint16_t *>(dstp)[x*4+2] = clamp(r,0,65535);
            reinterpret_cast<uint16_t *>(dstp)[x*4+3] = a; // alpha
        }
        dstp -= dst_pitch;
        srcY += src_pitch_y;
        srcU += src_pitch_uv;
        srcV += src_pitch_uv;
        srcA += src_pitch_a;
    }
  } else if (pixel_step == 6) { // RGB48
    const int Dstep = dst_pitch + (vi.width * pixel_step);
    for (int y = 0; y < vi.height; y++) {
        for (int x = 0; x < vi.width; x++) {
            int Y = reinterpret_cast<const uint16_t *>(srcY)[x] + matrix.offset_y;
            int U = reinterpret_cast<const uint16_t *>(srcU)[x] - 32768;
            int V = reinterpret_cast<const uint16_t *>(srcV)[x] - 32768;
            int b = (((int64_t)matrix.y_b * Y + (int64_t)matrix.u_b * U + (int64_t)matrix.v_b * V + round_mask_plus_rgb_offset_i)>>13);
            int g = (((int64_t)matrix.y_g * Y + (int64_t)matrix.u_g * U + (int64_t)matrix.v_g * V + round_mask_plus_rgb_offset_i)>>13);
            int r = (((int64_t)matrix.y_r * Y + (int64_t)matrix.u_r * U + (int64_t)matrix.v_r * V + round_mask_plus_rgb_offset_i)>>13);
            reinterpret_cast<uint16_t *>(dstp)[0] = clamp(b,0,65535);  // All the safety we can wish for.
            reinterpret_cast<uint16_t *>(dstp)[1] = clamp(g,0,65535);  // Probably needed here.
            reinterpret_cast<uint16_t *>(dstp)[2] = clamp(r,0,65535);
            dstp += pixel_step;
        }
        dstp -= Dstep;
        srcY += src_pitch_y;
        srcU += src_pitch_uv;
        srcV += src_pitch_uv;
    }
  } else if(pixel_step < 0) // -1: RGBP  -2:RGBAP
  {
      // YUV444 -> PlanarRGB
      // YUVA444 -> PlanarRGBA
    bool targetHasAlpha = pixel_step == -2;

    BYTE *dstpG = dst->GetWritePtr(PLANAR_G);
    BYTE *dstpB = dst->GetWritePtr(PLANAR_B);
    BYTE *dstpR = dst->GetWritePtr(PLANAR_R);

    // copy or fill alpha
    BYTE *dstpA;
    if (targetHasAlpha) {
      dstpA = dst->GetWritePtr(PLANAR_A);
      int heightA = dst->GetHeight(PLANAR_A);
      int rowsizeA = dst->GetRowSize(PLANAR_A);
      int dst_pitchA = dst->GetPitch(PLANAR_A);
        // simple copy
      if(src->GetRowSize(PLANAR_A)) // vi.IsYUVA() no-no! vi is already the target video type
        env->BitBlt(dstpA, dst_pitchA, src->GetReadPtr(PLANAR_A), src->GetPitch(PLANAR_A), src->GetRowSize(PLANAR_A_ALIGNED), src->GetHeight(PLANAR_A));
      else {
        // fill default transparency
        switch (vi.ComponentSize())
        {
        case 1:
          fill_plane<BYTE>(dstpA, heightA, rowsizeA, dst_pitchA, 255);
          break;
        case 2:
          fill_plane<uint16_t>(dstpA, heightA, rowsizeA, dst_pitchA, (1 << vi.BitsPerComponent()) - 1);
          break;
        case 4:
          fill_plane<float>(dstpA, heightA, rowsizeA, dst_pitchA, 1.0f);
          break;
        }
      }
    }

    int dst_pitchG = dst->GetPitch(PLANAR_G);
    int dst_pitchB = dst->GetPitch(PLANAR_B);
    int dst_pitchR = dst->GetPitch(PLANAR_R);
    int dst_pitchA = dst->GetPitch(PLANAR_A);

    int pixelsize = vi.ComponentSize();
    int bits_per_pixel = vi.BitsPerComponent();

#ifdef INTEL_INTRINSICS
    const BYTE* srcp[3] = { src->GetReadPtr(PLANAR_Y), src->GetReadPtr(PLANAR_U), src->GetReadPtr(PLANAR_V) };
    const int srcPitch[3] = { src->GetPitch(PLANAR_Y), src->GetPitch(PLANAR_U), src->GetPitch(PLANAR_V) };

    BYTE* dstp[3] = { dstpG, dstpB, dstpR };
    int dstPitch[3] = { dst_pitchG, dst_pitchB, dst_pitchR };

    if (bits_per_pixel < 16 && (env->GetCPUFlags() & CPUF_SSE2) )
    {
      switch (bits_per_pixel) {
      case 8: convert_yuv_to_planarrgb_uint8_14_sse2<uint8_t, 8>(dstp, dstPitch, srcp, srcPitch, vi.width, vi.height, matrix); break;
      case 10: convert_yuv_to_planarrgb_uint8_14_sse2<uint16_t, 10>(dstp, dstPitch, srcp, srcPitch, vi.width, vi.height, matrix); break;
      case 12: convert_yuv_to_planarrgb_uint8_14_sse2<uint16_t, 12>(dstp, dstPitch, srcp, srcPitch, vi.width, vi.height, matrix); break;
      case 14: convert_yuv_to_planarrgb_uint8_14_sse2<uint16_t, 14>(dstp, dstPitch, srcp, srcPitch, vi.width, vi.height, matrix); break;
      }
      return dst;
    }
    if (bits_per_pixel >= 16 && (env->GetCPUFlags() & CPUF_SSE2) ) {
      if (pixelsize == 4) // float 32 bit
        convert_yuv_to_planarrgb_float_sse2(dstp, dstPitch, srcp, srcPitch, vi.width, vi.height, matrix);
      else if (env->GetCPUFlags() & CPUF_SSE4_1)
        convert_yuv_to_planarrgb_uint16_sse41<16>(dstp, dstPitch, srcp, srcPitch, vi.width, vi.height, matrix);
      else
        convert_yuv_to_planarrgb_uint16_sse2<16>(dstp, dstPitch, srcp, srcPitch, vi.width, vi.height, matrix);
      return dst;
    }
#endif

    // todo: template for integers
    if(pixelsize==1)
    {
      for (int y = 0; y < vi.height; y++) {
        for (int x = 0; x < vi.width; x++) {
          //matrix.offset_y = -16;
          int Y = reinterpret_cast<const uint8_t *>(srcY)[x] + matrix.offset_y;
          int U = reinterpret_cast<const uint8_t *>(srcU)[x] - 128;
          int V = reinterpret_cast<const uint8_t *>(srcV)[x] - 128;
          int A = 0;
          if(targetHasAlpha)
            A = srcHasAlpha ? reinterpret_cast<const uint8_t *>(srcA)[x] : 255;
          int b = (int) ((((int)matrix.y_b * Y + (int)matrix.u_b * U + (int)matrix.v_b * V + round_mask_plus_rgb_offset_i)>>13));
          int g = (int) ((((int)matrix.y_g * Y + (int)matrix.u_g * U + (int)matrix.v_g * V + round_mask_plus_rgb_offset_i)>>13));
          int r = (int) ((((int)matrix.y_r * Y + (int)matrix.u_r * U + (int)matrix.v_r * V + round_mask_plus_rgb_offset_i)>>13));
          reinterpret_cast<uint8_t *>(dstpB)[x] = clamp(b,0,255);  // All the safety we can wish for.
          reinterpret_cast<uint8_t *>(dstpG)[x] = clamp(g,0,255);  // Probably needed here.
          reinterpret_cast<uint8_t *>(dstpR)[x] = clamp(r,0,255);
          if(targetHasAlpha)
            reinterpret_cast<uint8_t *>(dstpA)[x] = A;
        }
        dstpG += dst_pitchG;
        dstpB += dst_pitchB;
        dstpR += dst_pitchR;
        if(targetHasAlpha)
          dstpA += dst_pitchA;
        srcY += src_pitch_y;
        srcU += src_pitch_uv;
        srcV += src_pitch_uv;
      }
    } else if (pixelsize==2) {
      int half_pixel_value = 1 << (bits_per_pixel - 1);
      int max_pixel_value = (1 << bits_per_pixel) - 1;
      for (int y = 0; y < vi.height; y++) {
        for (int x = 0; x < vi.width; x++) {
          int Y = reinterpret_cast<const uint16_t *>(srcY)[x] + matrix.offset_y;
          int U = reinterpret_cast<const uint16_t *>(srcU)[x] - half_pixel_value;
          int V = reinterpret_cast<const uint16_t *>(srcV)[x] - half_pixel_value;
          int A;
          if(targetHasAlpha)
            A = srcHasAlpha ? reinterpret_cast<const uint16_t *>(srcA)[x] : max_pixel_value;
          // int64_t needed for 16 bit pixels
          int b = (((int64_t)matrix.y_b * Y + (int64_t)matrix.u_b * U + (int64_t)matrix.v_b * V + round_mask_plus_rgb_offset_i)>>13);
          int g = (((int64_t)matrix.y_g * Y + (int64_t)matrix.u_g * U + (int64_t)matrix.v_g * V + round_mask_plus_rgb_offset_i)>>13);
          int r = (((int64_t)matrix.y_r * Y + (int64_t)matrix.u_r * U + (int64_t)matrix.v_r * V + round_mask_plus_rgb_offset_i)>>13);
          reinterpret_cast<uint16_t *>(dstpB)[x] = clamp(b,0,max_pixel_value);  // All the safety we can wish for.
          reinterpret_cast<uint16_t *>(dstpG)[x] = clamp(g,0,max_pixel_value);  // Probably needed here.
          reinterpret_cast<uint16_t *>(dstpR)[x] = clamp(r,0,max_pixel_value);
          if(targetHasAlpha)
            reinterpret_cast<uint16_t *>(dstpA)[x] = A;
        }
        dstpG += dst_pitchG;
        dstpB += dst_pitchB;
        dstpR += dst_pitchR;
        if(targetHasAlpha)
          dstpA += dst_pitchA;
        srcY += src_pitch_y;
        srcU += src_pitch_uv;
        srcV += src_pitch_uv;
      }
    } else { // pixelsize==4 float
      for (int y = 0; y < vi.height; y++) {
        for (int x = 0; x < vi.width; x++) {
          float Y = reinterpret_cast<const float *>(srcY)[x] + matrix.offset_y_f;
          constexpr float shift = 0.0f;
          float U = reinterpret_cast<const float *>(srcU)[x] - shift;
          float V = reinterpret_cast<const float *>(srcV)[x] - shift;
          float A;
          if(targetHasAlpha)
            A = srcHasAlpha ? reinterpret_cast<const float *>(srcA)[x] : 1.0f;
          float b = matrix.y_b_f * Y + matrix.u_b_f * U + matrix.v_b_f * V + matrix.offset_rgb_f;
          float g = matrix.y_g_f * Y + matrix.u_g_f * U + matrix.v_g_f * V + matrix.offset_rgb_f;
          float r = matrix.y_r_f * Y + matrix.u_r_f * U + matrix.v_r_f * V + matrix.offset_rgb_f;
          reinterpret_cast<float *>(dstpB)[x] = clamp(b, 0.0f, 1.0f);  // All the safety we can wish for.
          reinterpret_cast<float *>(dstpG)[x] = clamp(g, 0.0f, 1.0f);  // Probably needed here.
          reinterpret_cast<float *>(dstpR)[x] = clamp(r, 0.0f, 1.0f);
          if(targetHasAlpha)
            reinterpret_cast<float *>(dstpA)[x] = A;
        }
        dstpG += dst_pitchG;
        dstpB += dst_pitchB;
        dstpR += dst_pitchR;
        if(targetHasAlpha)
          dstpA += dst_pitchA;
        srcY += src_pitch_y;
        srcU += src_pitch_uv;
        srcV += src_pitch_uv;
      }
    }
  }
  return dst;
}

/************************************
 * YUY2 to YV16
 ************************************/

ConvertYUY2ToYV16::ConvertYUY2ToYV16(PClip src, IScriptEnvironment* env) : GenericVideoFilter(src) {

  if (!vi.IsYUY2())
    env->ThrowError("ConvertYUY2ToYV16: Only YUY2 is allowed as input");

  vi.pixel_type = VideoInfo::CS_YV16;

}


static void convert_yuy2_to_yv16_c(const BYTE *srcp, BYTE *dstp_y, BYTE *dstp_u, BYTE *dstp_v, size_t src_pitch, size_t dst_pitch_y, size_t dst_pitch_uv, size_t width, size_t height)
{
  width /= 2;

  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      dstp_y[x * 2]     = srcp[x * 4 + 0];
      dstp_y[x * 2 + 1] = srcp[x * 4 + 2];
      dstp_u[x]         = srcp[x * 4 + 1];
      dstp_v[x]         = srcp[x * 4 + 3];
    }
    srcp += src_pitch;
    dstp_y += dst_pitch_y;
    dstp_u += dst_pitch_uv;
    dstp_v += dst_pitch_uv;
  }
}

PVideoFrame __stdcall ConvertYUY2ToYV16::GetFrame(int n, IScriptEnvironment* env) {
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);

  const BYTE* srcP = src->GetReadPtr();

  BYTE* dstY = dst->GetWritePtr(PLANAR_Y);
  BYTE* dstU = dst->GetWritePtr(PLANAR_U);
  BYTE* dstV = dst->GetWritePtr(PLANAR_V);

#ifdef INTEL_INTRINSICS
  if (env->GetCPUFlags() & CPUF_SSE2) {
    convert_yuy2_to_yv16_sse2(srcP, dstY, dstU, dstV, src->GetPitch(), dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), vi.width, vi.height);
  }
  else
#ifdef X86_32
  if (env->GetCPUFlags() & CPUF_MMX) {
    convert_yuy2_to_yv16_mmx(srcP, dstY, dstU, dstV, src->GetPitch(), dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), vi.width, vi.height);
  } else
#endif
#endif
  {
    convert_yuy2_to_yv16_c(srcP, dstY, dstU, dstV, src->GetPitch(), dst->GetPitch(PLANAR_Y), dst->GetPitch(PLANAR_U), vi.width, vi.height);
  }

  return dst;
}


AVSValue __cdecl ConvertYUY2ToYV16::Create(AVSValue args, void*, IScriptEnvironment* env) {
  PClip clip = args[0].AsClip();
  if (clip->GetVideoInfo().IsYV16())
    return clip;
  return new ConvertYUY2ToYV16(clip, env);
}

/************************************
 * YV16 to YUY2
 ************************************/

ConvertYV16ToYUY2::ConvertYV16ToYUY2(PClip src, IScriptEnvironment* env) : GenericVideoFilter(src) {

  if (!vi.IsYV16())
    env->ThrowError("ConvertYV16ToYUY2: Only YV16 is allowed as input");

  vi.pixel_type = VideoInfo::CS_YUY2;
}

void convert_yv16_to_yuy2_c(const BYTE *srcp_y, const BYTE *srcp_u, const BYTE *srcp_v, BYTE *dstp, size_t src_pitch_y, size_t src_pitch_uv, size_t dst_pitch, size_t width, size_t height) {
  for (size_t y=0; y < height; y++) {
    for (size_t x=0; x < width / 2; x++) {
      dstp[x*4+0] = srcp_y[x*2];
      dstp[x*4+1] = srcp_u[x];
      dstp[x*4+2] = srcp_y[x*2+1];
      dstp[x*4+3] = srcp_v[x];
    }
    srcp_y += src_pitch_y;
    srcp_u += src_pitch_uv;
    srcp_v += src_pitch_uv;
    dstp += dst_pitch;
  }
}

PVideoFrame __stdcall ConvertYV16ToYUY2::GetFrame(int n, IScriptEnvironment* env) {
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);

  const BYTE* srcY = src->GetReadPtr(PLANAR_Y);
  const BYTE* srcU = src->GetReadPtr(PLANAR_U);
  const BYTE* srcV = src->GetReadPtr(PLANAR_V);

  BYTE* dstp = dst->GetWritePtr();

#ifdef INTEL_INTRINSICS
  if (env->GetCPUFlags() & CPUF_SSE2) {
    //U and V don't have to be aligned since we user movq to read from those
    convert_yv16_to_yuy2_sse2(srcY, srcU, srcV, dstp, src->GetPitch(PLANAR_Y), src->GetPitch(PLANAR_U), dst->GetPitch(), vi.width, vi.height);
  } else
#ifdef X86_32
  if (env->GetCPUFlags() & CPUF_MMX) {
    convert_yv16_to_yuy2_mmx(srcY, srcU, srcV, dstp, src->GetPitch(PLANAR_Y), src->GetPitch(PLANAR_U), dst->GetPitch(), vi.width, vi.height);
  } else
#endif
#endif
  {
    convert_yv16_to_yuy2_c(srcY, srcU, srcV, dstp, src->GetPitch(PLANAR_Y), src->GetPitch(PLANAR_U), dst->GetPitch(), vi.width, vi.height);
  }

  return dst;
}


AVSValue __cdecl ConvertYV16ToYUY2::Create(AVSValue args, void*, IScriptEnvironment* env) {
  PClip clip = args[0].AsClip();
  if (clip->GetVideoInfo().IsYUY2())
    return clip;
  return new ConvertYV16ToYUY2(clip, env);
}

/**********************************************
 * Converter between arbitrary planar formats
 *
 * This uses plane copy for luma, and a custom
 * resizer for chroma
 *
 * (c) Klaus Post, 2005
 * (c) Ian Brabham, 2011
 **********************************************/

ConvertToPlanarGeneric::ConvertToPlanarGeneric(
  PClip src, int dst_space, bool interlaced,
  int _ChromaLocation_In,
  const AVSValue& chromaResampler, const AVSValue& param1, const AVSValue& param2, const AVSValue& param3,
  int _ChromaLocation_Out,
  IScriptEnvironment* env) :
  GenericVideoFilter(src), ChromaLocation_In(_ChromaLocation_In), ChromaLocation_Out(_ChromaLocation_Out)
{
  Yinput = vi.NumComponents() == 1;
  pixelsize = vi.ComponentSize();

  if (Yinput) {
    vi.pixel_type = dst_space;
    if (vi.ComponentSize() != pixelsize)
      env->ThrowError("Convert: Conversion from %d to %d-byte format not supported.", pixelsize, vi.ComponentSize());
    return;
  }

  auto Is420 = [](int pix_type) {
    return pix_type == VideoInfo::CS_YV12 || pix_type == VideoInfo::CS_I420 ||
      pix_type == VideoInfo::CS_YUV420P10 || pix_type == VideoInfo::CS_YUV420P12 ||
      pix_type == VideoInfo::CS_YUV420P14 || pix_type == VideoInfo::CS_YUV420P16 ||
      pix_type == VideoInfo::CS_YUV420PS ||
        pix_type == VideoInfo::CS_YUVA420 ||
      pix_type == VideoInfo::CS_YUVA420P10 || pix_type == VideoInfo::CS_YUVA420P12 ||
      pix_type == VideoInfo::CS_YUVA420P14 || pix_type == VideoInfo::CS_YUVA420P16 ||
      pix_type == VideoInfo::CS_YUVA420PS;
  };

  if (!Is420(vi.pixel_type) && !Is420(dst_space))
    interlaced = false;  // Ignore, if YV12 is not involved.
  //if (interlaced) env->ThrowError("Convert: Interlaced only available with 4:2:0 color spaces.");

  // Describe input pixel positioning
  float xdInU = 0.0f, txdInU = 0.0f, bxdInU = 0.0f;
  float ydInU = 0.0f, tydInU = 0.0f, bydInU = 0.0f;
  float xdInV = 0.0f, txdInV = 0.0f, bxdInV = 0.0f;
  float ydInV = 0.0f, tydInV = 0.0f, bydInV = 0.0f;

  /*
    "mpeg1", "center"
      - 1-1 averaging kernel (4:2:0, 4:2:2 (horizontal only))
    "mpeg2", "left" (4:2:0, 4:2:2)
      - top-field samples are sited 1/4 sample below the luma samples (4:2:0)
      - bottom-field samples are sited 1/4 sample above the luma samples (4:2:0)
      - horizontal 1-2-1 kernel
    "dv": (4:2:0)
      Chroma samples are sited on top of luma samples, but CB and CR samples are sited on alternate lines.
      - top: V
      - bottom: U
    "top_left" (4:2:0)
      - horizontal 1-2-1 vertical 1-2-1
  */

  if (Is420(vi.pixel_type)) {
    switch (ChromaLocation_In) {
      case ChromaLocation_e::AVS_CHROMA_DV: // spec. avisynth
        xdInU = 0.0f; ydInU = 1.0f; txdInU = 0.0f; tydInU = 1.0f; bxdInU = 0.0f; bydInU = 1.0f; // Cb
        xdInV = 0.0f; ydInV = 0.0f; txdInV = 0.0f; tydInV = 0.0f; bxdInV = 0.0f; bydInV = 0.0f; // Cr
        break;
      case ChromaLocation_e::AVS_CHROMA_TOP:
        xdInU = 0.5f, ydInU = 0.0f; txdInU = 0.5f; tydInU = 0.0f; bxdInU = 0.5f; bydInU = 0.5f;
        xdInV = 0.5f, ydInV = 0.0f; txdInV = 0.5f; tydInV = 0.0f; bxdInV = 0.5f; bydInV = 0.5f;
        break;
      case ChromaLocation_e::AVS_CHROMA_CENTER: // mpeg1, center
        xdInU = 0.5f, ydInU = 0.5f; txdInU = 0.5f; tydInU = 0.25f; bxdInU = 0.5f; bydInU = 0.75f;
        xdInV = 0.5f, ydInV = 0.5f; txdInV = 0.5f; tydInV = 0.25f; bxdInV = 0.5f; bydInV = 0.75f;
        break;
      case ChromaLocation_e::AVS_CHROMA_BOTTOM:
        xdInU = 0.5f, ydInU = 1.0f; txdInU = 0.5f; tydInU = 0.5f; bxdInU = 0.5f; bydInU = 1.0f;
        xdInV = 0.5f, ydInV = 1.0f; txdInV = 0.5f; tydInV = 0.5f; bxdInV = 0.5f; bydInV = 1.0f;
        break;
      case ChromaLocation_e::AVS_CHROMA_TOP_LEFT:
        xdInU = 0.0f; ydInU = 0.0f; txdInU = 0.0f; tydInU = 0.0f; bxdInU = 0.0f; bydInU = 0.5f;
        xdInV = 0.0f; ydInV = 0.0f; txdInV = 0.0f; tydInV = 0.0f; bxdInV = 0.0f; bydInV = 0.5f;
        break;
      case ChromaLocation_e::AVS_CHROMA_LEFT: // left, mpeg2
        xdInU = 0.0f; ydInU = 0.5f; txdInU = 0.0f; tydInU = 0.25f; bxdInU = 0.0f; bydInU = 0.75f;
        xdInV = 0.0f; ydInV = 0.5f; txdInV = 0.0f; tydInV = 0.25f; bxdInV = 0.0f; bydInV = 0.75f;
        break;
      case ChromaLocation_e::AVS_CHROMA_BOTTOM_LEFT:
        xdInU = 0.0f; ydInU = 1.0f; txdInU = 0.0f; tydInU = 0.5f; bxdInU = 0.0f; bydInU = 1.0f;
        xdInV = 0.0f; ydInV = 1.0f; txdInV = 0.0f; tydInV = 0.5f; bxdInV = 0.0f; bydInV = 1.0f;
        break;
      default:
        env->ThrowError("Convert: not supported ChromaPlacement for 4:2:0 input.");
    }
  }
  else if (vi.Is422()) {
    switch (ChromaLocation_In) {
    case ChromaLocation_e::AVS_CHROMA_CENTER: // center
      xdInU = 0.5f, ydInU = 0.0f; txdInU = 0.5f; tydInU = 0.0f; bxdInU = 0.5f; bydInU = 0.0f;
      xdInV = 0.5f, ydInV = 0.0f; txdInV = 0.5f; tydInV = 0.0f; bxdInV = 0.5f; bydInV = 0.0f;
      break;
    case ChromaLocation_e::AVS_CHROMA_TOP_LEFT: // treated as left
    case ChromaLocation_e::AVS_CHROMA_LEFT: // left, mpeg2
    case ChromaLocation_e::AVS_CHROMA_BOTTOM_LEFT: // treated as left
      xdInU = 0.0f; ydInU = 0.0f; txdInU = 0.0f; tydInU = 0.0f; bxdInU = 0.0f; bydInU = 0.0f;
      xdInV = 0.0f; ydInV = 0.0f; txdInV = 0.0f; tydInV = 0.0f; bxdInV = 0.0f; bydInV = 0.0f;
      break;
    default:
      env->ThrowError("Convert: not supported ChromaPlacement for 4:2:2 input.");
    }
  }
  else if (vi.IsYV411()) {
    if (ChromaLocation_In >= 0
      && ChromaLocation_In != ChromaLocation_e::AVS_CHROMA_TOP_LEFT
      && ChromaLocation_In != ChromaLocation_e::AVS_CHROMA_LEFT
      && ChromaLocation_In != ChromaLocation_e::AVS_CHROMA_BOTTOM_LEFT
      )
    {
      // if given, only the 'left'-ish versions are accepted, this is how it is treated
      env->ThrowError("Convert: not supported ChromaPlacement for 4:1:1 input. Only 'left'-style is allowed if given.");
    }
  }
  else if (ChromaLocation_In >= 0)
    env->ThrowError("Convert: Input ChromaPlacement is invalid for this format.");

  const int xsIn = 1 << vi.GetPlaneWidthSubsampling(PLANAR_U);
  const int ysIn = 1 << vi.GetPlaneHeightSubsampling(PLANAR_U);

  // change vi to the output format
  vi.pixel_type = dst_space;

  if (vi.ComponentSize() != pixelsize)
    env->ThrowError("Convert: Conversion from %d to %d-byte format not supported.", pixelsize, vi.ComponentSize());

  // Describe output pixel positioning
  float xdOutU = 0.0f, txdOutU = 0.0f, bxdOutU = 0.0f;
  float ydOutU = 0.0f, tydOutU = 0.0f, bydOutU = 0.0f;
  float xdOutV = 0.0f, txdOutV = 0.0f, bxdOutV = 0.0f;
  float ydOutV = 0.0f, tydOutV = 0.0f, bydOutV = 0.0f;

  if (Is420(vi.pixel_type)) {
    switch (ChromaLocation_Out) {
    case ChromaLocation_e::AVS_CHROMA_DV:
      xdOutU = 0.0f; ydOutU = 1.0f; txdOutU = 0.0f; tydOutU = 1.0f; bxdOutU = 0.0f; bydOutU = 1.0f; // Cb
      xdOutV = 0.0f; ydOutV = 0.0f; txdOutV = 0.0f; tydOutV = 0.0f; bxdOutV = 0.0f; bydOutV = 0.0f; // Cr
      break;
    case ChromaLocation_e::AVS_CHROMA_TOP:
      xdOutU = 0.5f, ydOutU = 0.0f; txdOutU = 0.5f; tydOutU = 0.0f; bxdOutU = 0.5f; bydOutU = 0.5f;
      xdOutV = 0.5f, ydOutV = 0.0f; txdOutV = 0.5f; tydOutV = 0.0f; bxdOutV = 0.5f; bydOutV = 0.5f;
      break;
    case ChromaLocation_e::AVS_CHROMA_CENTER: // mpeg1, center
      xdOutU = 0.5f, ydOutU = 0.5f; txdOutU = 0.5f; tydOutU = 0.25f; bxdOutU = 0.5f; bydOutU = 0.75f;
      xdOutV = 0.5f, ydOutV = 0.5f; txdOutV = 0.5f; tydOutV = 0.25f; bxdOutV = 0.5f; bydOutV = 0.75f;
      break;
    case ChromaLocation_e::AVS_CHROMA_BOTTOM:
      xdOutU = 0.5f, ydOutU = 1.0f; txdOutU = 0.5f; tydOutU = 0.5f; bxdOutU = 0.5f; bydOutU = 1.0f;
      xdOutV = 0.5f, ydOutV = 1.0f; txdOutV = 0.5f; tydOutV = 0.5f; bxdOutV = 0.5f; bydOutV = 1.0f;
      break;
    case ChromaLocation_e::AVS_CHROMA_TOP_LEFT:
      xdOutU = 0.0f; ydOutU = 0.0f; txdOutU = 0.0f; tydOutU = 0.0f; bxdOutU = 0.0f; bydOutU = 0.5f;
      xdOutV = 0.0f; ydOutV = 0.0f; txdOutV = 0.0f; tydOutV = 0.0f; bxdOutV = 0.0f; bydOutV = 0.5f;
      break;
    case ChromaLocation_e::AVS_CHROMA_LEFT: // left, mpeg2
      xdOutU = 0.0f; ydOutU = 0.5f; txdOutU = 0.0f; tydOutU = 0.25f; bxdOutU = 0.0f; bydOutU = 0.75f;
      xdOutV = 0.0f; ydOutV = 0.5f; txdOutV = 0.0f; tydOutV = 0.25f; bxdOutV = 0.0f; bydOutV = 0.75f;
      break;
    case ChromaLocation_e::AVS_CHROMA_BOTTOM_LEFT:
      xdOutU = 0.0f; ydOutU = 1.0f; txdOutU = 0.0f; tydOutU = 0.5f; bxdOutU = 0.0f; bydOutU = 1.0f;
      xdOutV = 0.0f; ydOutV = 1.0f; txdOutV = 0.0f; tydOutV = 0.5f; bxdOutV = 0.0f; bydOutV = 1.0f;
      break;
    default:
      env->ThrowError("Convert: not supported ChromaPlacement for 4:2:0 output.");
    }
  }
  else if (vi.Is422()) {
    switch (ChromaLocation_Out) {
    case ChromaLocation_e::AVS_CHROMA_CENTER: // mpeg1, center
      xdOutU = 0.5f, ydOutU = 0.0f; txdOutU = 0.5f; tydOutU = 0.0f; bxdOutU = 0.5f; bydOutU = 0.0f;
      xdOutV = 0.5f, ydOutV = 0.0f; txdOutV = 0.5f; tydOutV = 0.0f; bxdOutV = 0.5f; bydOutV = 0.0f;
      break;
    case ChromaLocation_e::AVS_CHROMA_TOP_LEFT: // treated as left
    case ChromaLocation_e::AVS_CHROMA_LEFT: // left, mpeg2
    case ChromaLocation_e::AVS_CHROMA_BOTTOM_LEFT: // treated as left
      xdOutU = 0.0f; ydOutU = 0.0f; txdOutU = 0.0f; tydOutU = 0.0f; bxdOutU = 0.0f; bydOutU = 0.0f;
      xdOutV = 0.0f; ydOutV = 0.0f; txdOutV = 0.0f; tydOutV = 0.0f; bxdOutV = 0.0f; bydOutV = 0.0f;
      break;
    default:
      env->ThrowError("Convert: not supported ChromaPlacement for 4:2:2 output.");
    }
  }
  else if (ChromaLocation_Out >= 0)
    env->ThrowError("Convert: Output ChromaPlacement only available with 4:2:0 or 4:2:2 output.");

  const int xsOut = 1 << vi.GetPlaneWidthSubsampling(PLANAR_U);
  const int xmask = xsOut - 1;
  if (vi.width & xmask)
    env->ThrowError("Convert: Cannot convert if width isn't mod%d!", xsOut);

  const int ysOut = 1 << vi.GetPlaneHeightSubsampling(PLANAR_U);
  const int ymask = ysOut - 1;
  if (vi.height & ymask)
    env->ThrowError("Convert: Cannot convert if height isn't mod%d!", ysOut);

  int uv_width  = vi.width  >> vi.GetPlaneWidthSubsampling(PLANAR_U);
  int uv_height = vi.height >> vi.GetPlaneHeightSubsampling(PLANAR_U);

  ResamplingFunction *filter = getResampler(chromaResampler.AsString("bicubic"), param1, param2, param3, true, env);

  bool P = !lstrcmpi(chromaResampler.AsString(""), "point");

  auto ChrOffset = [P](int sIn, float dIn, int sOut, float dOut) {
    //     (1 - sOut/sIn)/2 + (dOut-dIn)/sIn; // Gavino Jan 2011
    return P ? (dOut - dIn) / sIn : 0.5f + (dOut - dIn - 0.5f*sOut) / sIn;
  };

  const int force = 0;
  const bool preserve_center = true;
  const char *placement_name_notused = nullptr; // n/a
  const int forced_chroma_placement = -1; // no force
  // chroma planes are extracted, behave like Y when resized, no chroma planes involved

  if (interlaced) {
    uv_height /=  2;

    AVSValue tUsubSampling[4] = { ChrOffset(xsIn, txdInU, xsOut, txdOutU), ChrOffset(ysIn, tydInU, ysOut, tydOutU), AVSValue(), AVSValue() };
    AVSValue bUsubSampling[4] = { ChrOffset(xsIn, bxdInU, xsOut, bxdOutU), ChrOffset(ysIn, bydInU, ysOut, bydOutU), AVSValue(), AVSValue() };
    AVSValue tVsubSampling[4] = { ChrOffset(xsIn, txdInV, xsOut, txdOutV), ChrOffset(ysIn, tydInV, ysOut, tydOutV), AVSValue(), AVSValue() };
    AVSValue bVsubSampling[4] = { ChrOffset(xsIn, bxdInV, xsOut, bxdOutV), ChrOffset(ysIn, bydInV, ysOut, bydOutV), AVSValue(), AVSValue() };

    Usource = new SeparateFields(new AssumeParity(new SwapUVToY(child, SwapUVToY::UToY8, env), true), env); // also works for Y16/Y32
    Vsource = new SeparateFields(new AssumeParity(new SwapUVToY(child, SwapUVToY::VToY8, env), true), env); // also works for Y16/Y32

    std::vector<PClip> tbUsource(2); // Interleave() will take ownership of these
    std::vector<PClip> tbVsource(2);

    tbUsource[0] = FilteredResize::CreateResize(new SelectEvery(Usource, 2, 0, env), uv_width, uv_height, tUsubSampling, force, filter, preserve_center, placement_name_notused, forced_chroma_placement, env);
    tbUsource[1] = FilteredResize::CreateResize(new SelectEvery(Usource, 2, 1, env), uv_width, uv_height, bUsubSampling, force, filter, preserve_center, placement_name_notused, forced_chroma_placement, env);
    tbVsource[0] = FilteredResize::CreateResize(new SelectEvery(Vsource, 2, 0, env), uv_width, uv_height, tVsubSampling, force, filter, preserve_center, placement_name_notused, forced_chroma_placement, env);
    tbVsource[1] = FilteredResize::CreateResize(new SelectEvery(Vsource, 2, 1, env), uv_width, uv_height, bVsubSampling, force, filter, preserve_center, placement_name_notused, forced_chroma_placement, env);

    Usource = new SelectEvery(new DoubleWeaveFields(new Interleave(std::move(tbUsource), env)), 2, 0, env);
    Vsource = new SelectEvery(new DoubleWeaveFields(new Interleave(std::move(tbVsource), env)), 2, 0, env);
  }
  else {
    AVSValue UsubSampling[4] = { ChrOffset(xsIn, xdInU, xsOut, xdOutU), ChrOffset(ysIn, ydInU, ysOut, ydOutU), AVSValue(), AVSValue() };
    AVSValue VsubSampling[4] = { ChrOffset(xsIn, xdInV, xsOut, xdOutV), ChrOffset(ysIn, ydInV, ysOut, ydOutV), AVSValue(), AVSValue() };

    Usource = FilteredResize::CreateResize(new SwapUVToY(child, SwapUVToY::UToY8, env), uv_width, uv_height, UsubSampling, force, filter, preserve_center, placement_name_notused, forced_chroma_placement, env);
    Vsource = FilteredResize::CreateResize(new SwapUVToY(child, SwapUVToY::VToY8, env), uv_width, uv_height, VsubSampling, force, filter, preserve_center, placement_name_notused, forced_chroma_placement, env);
  }
  delete filter;
}

PVideoFrame __stdcall ConvertToPlanarGeneric::GetFrame(int n, IScriptEnvironment* env) {
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame dst = env->NewVideoFrameP(vi, &src);

  auto props = env->getFramePropsRW(dst);
  update_ChromaLocation(props, ChromaLocation_Out, env);

  env->BitBlt(dst->GetWritePtr(PLANAR_Y), dst->GetPitch(PLANAR_Y), src->GetReadPtr(PLANAR_Y), src->GetPitch(PLANAR_Y),
              src->GetRowSize(PLANAR_Y_ALIGNED), src->GetHeight(PLANAR_Y));

  // alpha. if pitch is zero -> no alpha channel
  const int rowsizeA = dst->GetRowSize(PLANAR_A);
  const int dst_pitchA = dst->GetPitch(PLANAR_A);
  BYTE* dstp_a = (dst_pitchA == 0) ? nullptr : dst->GetWritePtr(PLANAR_A);
  const int heightA = dst->GetHeight(PLANAR_A);

  if (dst_pitchA != 0)
  {
    if (src->GetPitch(PLANAR_A) != 0)
      env->BitBlt(dstp_a, dst_pitchA, src->GetReadPtr(PLANAR_A), src->GetPitch(PLANAR_A),
        src->GetRowSize(PLANAR_A_ALIGNED), src->GetHeight(PLANAR_A));
    else {
      // e.g. ConvertToYUVA() case from Alpha-less formats
      switch (vi.ComponentSize())
      {
      case 1:
        fill_plane<BYTE>(dstp_a, heightA, rowsizeA, dst_pitchA, 255);
        break;
      case 2:
        fill_plane<uint16_t>(dstp_a, heightA, rowsizeA, dst_pitchA, (1 << vi.BitsPerComponent()) - 1);
        break;
      case 4:
        fill_plane<float>(dstp_a, heightA, rowsizeA, dst_pitchA, 1.0f);
        break;
      }
    }
  }

  BYTE* dstp_u = dst->GetWritePtr(PLANAR_U);
  BYTE* dstp_v = dst->GetWritePtr(PLANAR_V);
  const int height = dst->GetHeight(PLANAR_U);
  const int rowsizeUV = dst->GetRowSize(PLANAR_U);
  const int dst_pitch = dst->GetPitch(PLANAR_U);

  if (Yinput) {
    switch (vi.ComponentSize())
    {
      case 1:
        fill_chroma<BYTE>(dstp_u, dstp_v, height, rowsizeUV, dst_pitch, 0x80);
        break;
      case 2:
        fill_chroma<uint16_t>(dstp_u, dstp_v, height, rowsizeUV, dst_pitch, 1 << (vi.BitsPerComponent() - 1));
        break;
      case 4:
        const float half = 0.0f;
        fill_chroma<float>(dstp_u, dstp_v, height, rowsizeUV, dst_pitch, half);
        break;
    }
  } else {
    src = Usource->GetFrame(n, env);
    env->BitBlt(dstp_u, dst_pitch, src->GetReadPtr(PLANAR_Y), src->GetPitch(PLANAR_Y), src->GetRowSize(PLANAR_Y_ALIGNED), height);
    src = Vsource->GetFrame(n, env);
    env->BitBlt(dstp_v, dst_pitch, src->GetReadPtr(PLANAR_Y), src->GetPitch(PLANAR_Y), src->GetRowSize(PLANAR_Y_ALIGNED), height);
  }

  return dst;
}

AVSValue ConvertToPlanarGeneric::Create(AVSValue& args, const char* filter, bool strip_alpha_legacy_8bit, bool to_yuva, IScriptEnvironment* env) {
  bool converted = false;

  PClip clip = args[0].AsClip();
  VideoInfo vi = clip->GetVideoInfo();

  if (vi.IsRGB()) { // packed or planar
    const bool keep_packedrgb_alpha = to_yuva && (vi.IsRGB32() || vi.IsRGB64());
    if (vi.IsRGB48() || vi.IsRGB64()) {
      // we convert to intermediate PlanarRGB, RGB48/64->YUV444 is slow C, planarRGB  is fast
      AVSValue new_args[8] = { clip, AVSValue(), AVSValue(), AVSValue(), AVSValue(), AVSValue(), AVSValue(), AVSValue() };
      // clip, matrix,  interlaced, chromainplacement, chromaresample, param1, param2, param3
      // convert to planar RGBA only if RGB64 and target is YUVA (need to keep alpha)
      const intptr_t planar_rgb_type = keep_packedrgb_alpha ? -2 : -1;
      clip = ConvertToRGB::Create(AVSValue(new_args, 8), (void *)planar_rgb_type, env).AsClip();
      vi = clip->GetVideoInfo();
    }

    clip = new ConvertRGBToYUV444(clip, args[2].AsString(0) /* matrix_name */, keep_packedrgb_alpha, env);
    vi = clip->GetVideoInfo();
    converted = true;
  }
  else if (vi.IsYUY2()) { // 8 bit only
    clip = new ConvertYUY2ToYV16(clip, env);
    vi = clip->GetVideoInfo();
    converted = true;
  }
  else if (!vi.IsPlanar())
    env->ThrowError("%s: Can only convert from Planar YUV.", filter);

  int pixel_type = VideoInfo::CS_UNKNOWN;
  AVSValue outplacement = AVSValue(); // only for ConvertToYUV420 and ConvertToYUV422

  bool hasAlpha = vi.NumComponents() == 4 && !strip_alpha_legacy_8bit;
  bool shouldStripAlpha = vi.NumComponents() == 4 && strip_alpha_legacy_8bit;
  bool shouldAddAlpha = vi.NumComponents() != 4 && to_yuva;
  bool targethasAlpha = hasAlpha || shouldAddAlpha;

  int ChromaLocation_In = -1; // invalid. Chromalocation_e::AVS_CHROMALOCATION_UNUSED
  int ChromaLocation_Out = -1;

  const bool to_420 = strcmp(filter, "ConvertToYUV420") == 0;
  const bool to_422 = strcmp(filter, "ConvertToYUV422") == 0;
  const bool to_411 = strcmp(filter, "ConvertToYV411") == 0;
  const bool to_444 = strcmp(filter, "ConvertToYUV444") == 0;

  if (vi.IsYV411()) {
    // ChromaInPlacement parameter exists, (default none/-1) + input frame properties; 'left'-ish _ChromaLocation is allowed, checked later
    auto frame0 = clip->GetFrame(0, env);
    const AVSMap* props = env->getFramePropsRO(frame0);
    chromaloc_parse_merge_with_props(vi, args[3].AsString(nullptr), props, /* ref*/ChromaLocation_In, -1 /*default none chromaloc */, env);
  }
  else if (vi.Is420() || vi.Is422()) {
    // ChromaInPlacement parameter is valid + input frame properties
    auto frame0 = clip->GetFrame(0, env);
    const AVSMap* props = env->getFramePropsRO(frame0);
    chromaloc_parse_merge_with_props(vi, args[3].AsString(nullptr), props, /* ref*/ChromaLocation_In, ChromaLocation_e::AVS_CHROMA_LEFT /*default*/, env);
  }

  AVSValue param1;
  AVSValue param2;
  AVSValue param3;
  if (to_420 || to_422) {
    // ChromaOutPlacement parameter is valid
    chromaloc_parse_merge_with_props(vi, args[5].AsString(nullptr), nullptr, /* ref*/ChromaLocation_Out, ChromaLocation_e::AVS_CHROMA_LEFT /*default*/, env);
    param1 = args[6];
    param2 = args[7];
    param3 = args[8];
  }
  else {
    param1 = args[5];
    param2 = args[6];
    param3 = args[7];
  }


  if (to_420) {
    if (vi.Is420()) {
      // possible shortcut
      if (ChromaLocation_In == ChromaLocation_Out)
      {
        if (shouldStripAlpha)
          return new RemoveAlphaPlane(clip, env);
        if (shouldAddAlpha) {
          // create with default alpha
          clip = new AddAlphaPlane(clip, nullptr, 0.0f, false, env);
          vi = clip->GetVideoInfo();
        }
        return clip;
      }
    }

    outplacement = args[5];
    switch (vi.BitsPerComponent())
    {
    case 8 : pixel_type = targethasAlpha ? VideoInfo::CS_YUVA420 : VideoInfo::CS_YV12; break;
    case 10: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA420P10 : VideoInfo::CS_YUV420P10; break;
    case 12: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA420P12 : VideoInfo::CS_YUV420P12; break;
    case 14: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA420P14 : VideoInfo::CS_YUV420P14; break;
    case 16: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA420P16 : VideoInfo::CS_YUV420P16; break;
    case 32: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA420PS  : VideoInfo::CS_YUV420PS; break;
    }
  }
  else if (to_422) {
    if (vi.Is422()) {
      // possible shortcut
      if (ChromaLocation_In == ChromaLocation_Out)
      {
        if (shouldStripAlpha)
          return new RemoveAlphaPlane(clip, env);
        if (shouldAddAlpha) {
          // create with default alpha
          clip = new AddAlphaPlane(clip, nullptr, 0.0f, false, env);
          vi = clip->GetVideoInfo();
        }
        return clip;
      }
    }

    outplacement = args[5];
    switch (vi.BitsPerComponent())
    {
    case 8 : pixel_type = targethasAlpha ? VideoInfo::CS_YUVA422 : VideoInfo::CS_YV16; break;
    case 10: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA422P10 : VideoInfo::CS_YUV422P10; break;
    case 12: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA422P12 : VideoInfo::CS_YUV422P12; break;
    case 14: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA422P14 : VideoInfo::CS_YUV422P14; break;
    case 16: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA422P16 : VideoInfo::CS_YUV422P16; break;
    case 32: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA422PS  : VideoInfo::CS_YUV422PS; break;
    }
  }
  else if (to_444) {
    if (vi.Is444()) {
      if (shouldStripAlpha)
        return new RemoveAlphaPlane(clip, env);
      if (shouldAddAlpha) {
        // create with default alpha
        clip = new AddAlphaPlane(clip, nullptr, 0.0f, false, env);
        vi = clip->GetVideoInfo();
      }
      return clip;
    }

    switch (vi.BitsPerComponent())
    {
    case 8 : pixel_type = targethasAlpha ? VideoInfo::CS_YUVA444 : VideoInfo::CS_YV24; break;
    case 10: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA444P10 : VideoInfo::CS_YUV444P10; break;
    case 12: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA444P12 : VideoInfo::CS_YUV444P12; break;
    case 14: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA444P14 : VideoInfo::CS_YUV444P14; break;
    case 16: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA444P16 : VideoInfo::CS_YUV444P16; break;
    case 32: pixel_type = targethasAlpha ? VideoInfo::CS_YUVA444PS  : VideoInfo::CS_YUV444PS; break;
    }
  }
  else if (to_411) {
    if (vi.IsYV411()) return clip;
    if(vi.ComponentSize()!=1)
      env->ThrowError("%s: 8 bit input only", filter);

    pixel_type = VideoInfo::CS_YV411;
  }
  else env->ThrowError("Convert: unknown filter '%s'.", filter);

  if (pixel_type == VideoInfo::CS_UNKNOWN)
    env->ThrowError("%s: unsupported bit depth", filter);

  if (converted)
    clip = env->Invoke("Cache", AVSValue(clip)).AsClip();

  // ConvertToPlanarGeneric's GetFrame will recognize if alpha copy or fill-with-defaults needed
  return new ConvertToPlanarGeneric(clip, pixel_type, args[1].AsBool(false), ChromaLocation_In,
    args[4], // chromaresample
    param1, param2, param3,
    ChromaLocation_Out, env);
}

AVSValue __cdecl ConvertToPlanarGeneric::CreateYUV420(AVSValue args, void* user_data, IScriptEnvironment* env) {
  PClip clip = args[0].AsClip();
  const VideoInfo& vi = clip->GetVideoInfo();
  bool only_8bit = reinterpret_cast<intptr_t>(user_data) == 0;
  bool to_yuva = reinterpret_cast<intptr_t>(user_data) == 2;
  if (only_8bit && vi.BitsPerComponent() != 8)
    env->ThrowError("ConvertToYV12: only 8 bit sources allowed");
  return Create(args, "ConvertToYUV420", only_8bit, to_yuva, env);
}

AVSValue __cdecl ConvertToPlanarGeneric::CreateYUV422(AVSValue args, void* user_data, IScriptEnvironment* env) {
  PClip clip = args[0].AsClip();
  const VideoInfo& vi = clip->GetVideoInfo();
  bool only_8bit = reinterpret_cast<intptr_t>(user_data) == 0;
  bool to_yuva = reinterpret_cast<intptr_t>(user_data) == 2;
  if (only_8bit && vi.BitsPerComponent() != 8)
    env->ThrowError("ConvertToYV16: only 8 bit sources allowed");
  return Create(args, "ConvertToYUV422", only_8bit, to_yuva, env);
}

AVSValue __cdecl ConvertToPlanarGeneric::CreateYUV444(AVSValue args, void* user_data, IScriptEnvironment* env) {
  PClip clip = args[0].AsClip();
  const VideoInfo& vi = clip->GetVideoInfo();
  bool only_8bit = reinterpret_cast<intptr_t>(user_data) == 0;
  bool to_yuva = reinterpret_cast<intptr_t>(user_data) == 2;
  if (only_8bit && vi.BitsPerComponent() != 8)
    env->ThrowError("ConvertToYV24: only 8 bit sources allowed");
  return Create(args, "ConvertToYUV444", only_8bit, to_yuva, env);
}

AVSValue __cdecl ConvertToPlanarGeneric::CreateYV411(AVSValue args, void* user_data, IScriptEnvironment* env) {
  PClip clip = args[0].AsClip();
  const VideoInfo& vi = clip->GetVideoInfo();
  bool only_8bit = reinterpret_cast<intptr_t>(user_data) == 0;
  // though Avisynth does not have YUVA411, make similar to others
  bool to_yuva = reinterpret_cast<intptr_t>(user_data) == 2;
  if (only_8bit && vi.BitsPerComponent() != 8)
    env->ThrowError("ConvertToYV411: only 8 bit sources allowed");
  return Create(args, "ConvertToYV411", only_8bit, to_yuva, env);
}


/*
static int getPlacement(const AVSValue& _placement, IScriptEnvironment* env) {
  const char* placement = _placement.AsString(0);

  if (placement) {
    if (!lstrcmpi(placement, "mpeg2") || !lstrcmpi(placement, "left"))
      return PLACEMENT_MPEG2;

    if (!lstrcmpi(placement, "mpeg1") || !lstrcmpi(placement, "jpeg") || !lstrcmpi(placement, "center"))
      return PLACEMENT_MPEG1;

    if (!lstrcmpi(placement, "dv"))
      return PLACEMENT_DV;

    if (!lstrcmpi(placement, "top_left"))
      return PLACEMENT_TOP_LEFT;

    env->ThrowError("Convert: Unknown chromaplacement");
  }
  return PLACEMENT_MPEG2;
}
*/


ResamplingFunction* getResampler(const char* resampler, AVSValue param1, AVSValue param2, AVSValue param3, bool throw_on_error, IScriptEnvironment* env) {
  if (resampler) {
    if (!lstrcmpi(resampler, "point"))
      return new PointFilter();
    else if (!lstrcmpi(resampler, "bilinear"))
      return new TriangleFilter();
    else if (!lstrcmpi(resampler, "bicubic"))
      return new MitchellNetravaliFilter(param1.AsDblDef(1.0/3), param2.AsDblDef(1.0/3)); // optional B and C as param1 and param2
    else if (!lstrcmpi(resampler, "lanczos"))
      return new LanczosFilter((int)param1.AsFloat(3)); // optional Taps as param1
    else if (!lstrcmpi(resampler, "lanczos4"))
      return new LanczosFilter(4);
    else if (!lstrcmpi(resampler, "blackman"))
      return new BlackmanFilter((int)param1.AsFloat(4)); // optional Taps as param1
    else if (!lstrcmpi(resampler, "spline16"))
      return new Spline16Filter();
    else if (!lstrcmpi(resampler, "spline36"))
      return new Spline36Filter();
    else if (!lstrcmpi(resampler, "spline64"))
      return new Spline64Filter();
    else if (!lstrcmpi(resampler, "gauss"))
      return new GaussianFilter(param1.AsDblDef(30.0), param2.AsDblDef(2.0), param3.AsDblDef(4.0)); // optional P, B, S as param1, param2, param3
    else if (!lstrcmpi(resampler, "sinc"))
      return new SincFilter((int)param1.AsFloat(4)); // optional Taps as param1
    else if (!lstrcmpi(resampler, "sinpow"))
      return new SinPowerFilter(param1.AsDblDef(2.5)); // optional P as param1
    else if (!lstrcmpi(resampler, "sinclin2"))
      return new SincLin2Filter((int)param1.AsFloat(15)); // optional Taps= as param1
    else if (!lstrcmpi(resampler, "userdefined2"))
      return new UserDefined2Filter(param1.AsDblDef(121.0), param2.AsDblDef(19.0), param3.AsDblDef(2.3)); // optional B and C and S as param1, param2, param3
    else
      if (throw_on_error)
        env->ThrowError("Convert: Unknown chroma resampler, '%s'", resampler);
      else
        return nullptr; // e.g. from AddBorders
  }
  return new MitchellNetravaliFilter(param1.AsDblDef(1.0/3), param2.AsDblDef(1.0/3)); // Default colorspace conversion for AviSynth
}
