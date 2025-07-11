// Avisynth v2.5.  Copyright 2002 Ben Rudiak-Gould et al.
// http://www.avisynth.org

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


#include "greyscale.h"
#ifdef INTEL_INTRINSICS
#include "intel/greyscale_sse.h"
#endif
#include "internal.h"
#include "avs/alignment.h"
#include "avs/minmax.h"

#ifdef AVS_WINDOWS
    #include "avs/win.h"
#else
    #include "avs/posix.h"
#endif

#include <stdint.h>
#include "convert_planar.h"
#include "convert.h"
#include "convert_helper.h"


/*************************************
 *******   Convert to Greyscale ******
 ************************************/

extern const AVSFunction Greyscale_filters[] = {
  { "Greyscale", BUILTIN_FUNC_PREFIX, "c[matrix]s", Greyscale::Create },       // matrix can be "rec601", "rec709" or "Average" or "rec2020"
  { "Grayscale", BUILTIN_FUNC_PREFIX, "c[matrix]s", Greyscale::Create },
  { 0 }
};

Greyscale::Greyscale(PClip _child, const char* matrix_name, IScriptEnvironment* env)
 : GenericVideoFilter(_child)
{
  if (matrix_name && !vi.IsRGB())
    env->ThrowError("GreyScale: invalid \"matrix\" parameter (RGB data only)");

  // originally there was no PC range here
  pixelsize = vi.ComponentSize();
  bits_per_pixel = vi.BitsPerComponent();

  // In all cases, Luma range is not changed(0-255 in -> 0-255 out; 16-235 in -> 16-235 out)

  if (vi.IsRGB()) {
    auto frame0 = _child->GetFrame(0, env);
    const AVSMap* props = env->getFramePropsRO(frame0);
    // input _ColorRange frame property can appear for RGB source (studio range limited rgb)
    matrix_parse_merge_with_props(true /*in rgb*/, true /*out rgb, same range*/, matrix_name, props, theMatrix, theColorRange, theOutColorRange, env);
    /*if (theColorRange == ColorRange_e::AVS_RANGE_FULL && theMatrix != Matrix_e::AVS_MATRIX_AVERAGE)
      env->ThrowError("GreyScale: only limited range matrix definition or \"Average\" is allowed.");
    */

    const int shift = 15; // internally 15 bits precision, still no overflow in calculations

    // input _ColorRange frame property can appear for RGB source (studio range limited rgb)
    if (!do_BuildMatrix_Rgb2Yuv(theMatrix, theColorRange, theOutColorRange, shift, bits_per_pixel, /*ref*/greyMatrix))
      env->ThrowError("GreyScale: Unknown matrix.");
  }
  // greyscale does not change color space, rgb remains rgb
  // Leave matrix and range frame properties as is.
}

template<typename pixel_t, int pixel_step>
static void greyscale_packed_rgb_c(BYTE *srcp8, int src_pitch, int width, int height, ConversionMatrix& m) {
  const bool has_offset_rgb = 0 != m.offset_rgb;

  pixel_t *srcp = reinterpret_cast<pixel_t *>(srcp8);
  src_pitch /= sizeof(pixel_t);

  // .15 bit frac integer arithmetic
  const int rounder_and_luma_offset = (1 << 14) + (m.offset_y << 15);
  // greyscale RGB is putting pack the calculated pixels to rgb
  // Limited range input remains limited range output (-offset_rgb is the same as offset_y)

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int b = srcp[x * pixel_step + 0];
      int g = srcp[x * pixel_step + 1];
      int r = srcp[x * pixel_step + 2];
      if (has_offset_rgb) {
        b = b + m.offset_rgb;
        g = g + m.offset_rgb;
        r = r + m.offset_rgb;
      }
      srcp[x * pixel_step + 0] = srcp[x * pixel_step + 1] = srcp[x * pixel_step + 2] =
        (m.y_b * b + m.y_g * g + m.y_r * r + rounder_and_luma_offset) >> 15;
    }
    srcp += src_pitch;
  }
}

template<typename pixel_t>
static void greyscale_planar_rgb_c(BYTE *srcp_r8, BYTE *srcp_g8, BYTE *srcp_b8, int src_pitch, int width, int height, ConversionMatrix& m) {
  const bool has_offset_rgb = 0 != m.offset_rgb;

  pixel_t *srcp_r = reinterpret_cast<pixel_t *>(srcp_r8);
  pixel_t *srcp_g = reinterpret_cast<pixel_t *>(srcp_g8);
  pixel_t *srcp_b = reinterpret_cast<pixel_t *>(srcp_b8);
  src_pitch /= sizeof(pixel_t);

  // .15 bit frac integer arithmetic
  const int rounder_and_luma_offset = (1 << 14) + (m.offset_y << 15);
  // greyscale RGB is putting pack the calculated pixels to rgb
  // Limited range input remains limited range output (-offset_rgb is the same as offset_y)

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int b = srcp_b[x];
      int g = srcp_g[x];
      int r = srcp_r[x];
      if (has_offset_rgb) {
        b = b + m.offset_rgb;
        g = g + m.offset_rgb;
        r = r + m.offset_rgb;
      }

      srcp_b[x] = srcp_g[x] = srcp_r[x] =
        (m.y_b * b + m.y_g * g + m.y_r * r + rounder_and_luma_offset) >> 15;
    }
    srcp_r += src_pitch;
    srcp_g += src_pitch;
    srcp_b += src_pitch;
  }
}

static void greyscale_planar_rgb_float_c(BYTE *srcp_r8, BYTE *srcp_g8, BYTE *srcp_b8, int src_pitch, int width, int height, ConversionMatrix& m) {
  const bool has_offset_rgb = 0 != m.offset_rgb_f;

  float *srcp_r = reinterpret_cast<float *>(srcp_r8);
  float *srcp_g = reinterpret_cast<float *>(srcp_g8);
  float *srcp_b = reinterpret_cast<float *>(srcp_b8);
  src_pitch /= sizeof(float);

  const float luma_offset = m.offset_y_f;
  // greyscale RGB is putting pack the calculated pixels to rgb
  // Limited range input remains limited range output (-offset_rgb is the same as offset_y)


  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      float b = srcp_b[x];
      float g = srcp_g[x];
      float r = srcp_r[x];
      if (has_offset_rgb) {
        b = b + m.offset_rgb_f;
        g = g + m.offset_rgb_f;
        r = r + m.offset_rgb_f;
      }
      srcp_b[x] = srcp_g[x] = srcp_r[x] =
        m.y_b_f * b + m.y_g_f * g + m.y_r_f * r + luma_offset;
    }
    srcp_r += src_pitch;
    srcp_g += src_pitch;
    srcp_b += src_pitch;
  }
}


PVideoFrame Greyscale::GetFrame(int n, IScriptEnvironment* env)
{
  PVideoFrame frame = child->GetFrame(n, env);
  if (vi.NumComponents() == 1)
    return frame;

  env->MakeWritable(&frame);
  BYTE* srcp = frame->GetWritePtr();
  int pitch = frame->GetPitch();
  int height = vi.height;
  int width = vi.width;

  // greyscale does not change color space, rgb remains rgb
  // Leave matrix and range frame properties as is.

  if (vi.IsPlanar() && (vi.IsYUV() || vi.IsYUVA())) {
    // planar YUV, set UV plane to neutral
    BYTE* dstp_u = frame->GetWritePtr(PLANAR_U);
    BYTE* dstp_v = frame->GetWritePtr(PLANAR_V);
    const int height = frame->GetHeight(PLANAR_U);
    const int rowsizeUV = frame->GetRowSize(PLANAR_U);
    const int dst_pitch = frame->GetPitch(PLANAR_U);
    switch (vi.ComponentSize())
    {
    case 1:
      fill_chroma<BYTE>(dstp_u, dstp_v, height, rowsizeUV, dst_pitch, 0x80);
      break;
    case 2:
      fill_chroma<uint16_t>(dstp_u, dstp_v, height, rowsizeUV, dst_pitch, 1 << (vi.BitsPerComponent() - 1));
      break;
    case 4:
      const float shift = 0.0f;
      fill_chroma<float>(dstp_u, dstp_v, height, rowsizeUV, dst_pitch, shift);
      break;
    }
    return frame;
  }

  if (vi.IsYUY2()) {
#ifdef INTEL_INTRINSICS
    if ((env->GetCPUFlags() & CPUF_SSE2) && width > 4) {
      greyscale_yuy2_sse2(srcp, width, height, pitch);
    } else
#ifdef X86_32
      if ((env->GetCPUFlags() & CPUF_MMX) && width > 2) {
        greyscale_yuy2_mmx(srcp, width, height, pitch);
      } else
#endif
#endif
      {
        for (int y = 0; y<height; ++y)
        {
          for (int x = 0; x<width; x++)
            srcp[x*2+1] = 128;
          srcp += pitch;
        }
      }

      return frame;
  }
#ifdef INTEL_INTRINSICS
  if(vi.IsRGB64()) {
    if (env->GetCPUFlags() & CPUF_SSE4_1) {
      greyscale_rgb64_sse41(srcp, width, height, pitch, greyMatrix);
      return frame;
    }
  }

  if (vi.IsRGB32()) {
    if (env->GetCPUFlags() & CPUF_SSE2) {
      greyscale_rgb32_sse2(srcp, width, height, pitch, greyMatrix);
      return frame;
    }
#ifdef X86_32
    else if (env->GetCPUFlags() & CPUF_MMX) {
      greyscale_rgb32_mmx(srcp, width, height, pitch, greyMatrix);
      return frame;
    }
#endif
  }
#endif

  if (vi.IsRGB())
  {  // RGB C.
    if (vi.IsPlanarRGB() || vi.IsPlanarRGBA())
    {
      BYTE* srcp_g = frame->GetWritePtr(PLANAR_G);
      BYTE* srcp_b = frame->GetWritePtr(PLANAR_B);
      BYTE* srcp_r = frame->GetWritePtr(PLANAR_R);

      const int src_pitch = frame->GetPitch(); // same for all planes

      if (pixelsize == 1)
        greyscale_planar_rgb_c<uint8_t>(srcp_r, srcp_g, srcp_b, src_pitch, vi.width, vi.height, greyMatrix);
      else if (pixelsize == 2)
        greyscale_planar_rgb_c<uint16_t>(srcp_r, srcp_g, srcp_b, src_pitch, vi.width, vi.height, greyMatrix);
      else
        greyscale_planar_rgb_float_c(srcp_r, srcp_g, srcp_b, src_pitch, vi.width, vi.height, greyMatrix);

      return frame;
    }
    // packed RGB

    const int rgb_inc = vi.IsRGB32() || vi.IsRGB64() ? 4 : 3;

    if (pixelsize == 1) { // rgb24/32
      if (rgb_inc == 3)
        greyscale_packed_rgb_c<uint8_t, 3>(srcp, pitch, vi.width, vi.height, greyMatrix);
      else
        greyscale_packed_rgb_c<uint8_t, 4>(srcp, pitch, vi.width, vi.height, greyMatrix);
    }
    else { // rgb48/64
      if (rgb_inc == 3)
        greyscale_packed_rgb_c<uint16_t, 3>(srcp, pitch, vi.width, vi.height, greyMatrix);
      else
        greyscale_packed_rgb_c<uint16_t, 4>(srcp, pitch, vi.width, vi.height, greyMatrix);
    }

  }
  return frame;
}


AVSValue __cdecl Greyscale::Create(AVSValue args, void*, IScriptEnvironment* env)
{
  PClip clip = args[0].AsClip();
  const VideoInfo& vi = clip->GetVideoInfo();

  if (vi.NumComponents() == 1)
    return clip;

  return new Greyscale(clip, args[1].AsString(0), env);
}
