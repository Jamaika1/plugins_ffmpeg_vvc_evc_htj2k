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

// Overlay (c) 2003, 2004 by Klaus Post

#include "overlayfunctions.h"
#include "blend_common.h"
#ifdef INTEL_INTRINSICS
#include "intel/blend_common_sse.h"
#include "intel/blend_common_avx2.h"
#endif


#include <stdint.h>

void OL_BlendImage::DoBlendImageMask(ImageOverlayInternal* base, ImageOverlayInternal* overlay, ImageOverlayInternal* mask) {
  if (bits_per_pixel == 8)
    BlendImageMask<uint8_t>(base, overlay, mask);
  else if (bits_per_pixel <= 16)
    BlendImageMask<uint16_t>(base, overlay, mask);
  else if (bits_per_pixel == 32)
    BlendImageMask<float>(base, overlay, mask);
}

void OL_BlendImage::DoBlendImage(ImageOverlayInternal* base, ImageOverlayInternal* overlay) {
  if (bits_per_pixel == 8)
    BlendImage<uint8_t>(base, overlay);
  else if (bits_per_pixel <= 16)
    BlendImage<uint16_t>(base, overlay);
  else if (bits_per_pixel == 32)
    BlendImage<float>(base, overlay);
}


template<typename pixel_t>
void OL_BlendImage::BlendImageMask(ImageOverlayInternal* base, ImageOverlayInternal* overlay, ImageOverlayInternal* mask) {
  int w = base->w();
  int h = base->h();

  const int pixelsize = sizeof(pixel_t);

  int planeindex_from = 0;
  int planeindex_to = 0;

  if (of_mode == OF_Blend) {
    planeindex_from = 0;
    planeindex_to = greyscale ? 0 : 2;
  }
  else if (of_mode == OF_Luma) {
    planeindex_from = 0;
    planeindex_to = 0;
  }
  else if (of_mode == OF_Chroma) {
    if (greyscale)
      return;
    planeindex_from = 1;
    planeindex_to = 2;
  }

  overlay_blend_plane_masked_opacity_t* blend_fn = nullptr;

  // 32 bit float inside
  // independent from full/not full opacity
#ifdef INTEL_INTRINSICS
#if defined(__AVX2__)
  if (pixelsize == 4 && (env->GetCPUFlags() & CPUF_AVX2)) {
    blend_fn = overlay_blend_avx2_float<true>;
  } else
#endif
  if (pixelsize == 4 && (env->GetCPUFlags() & CPUF_SSE2)) {
    blend_fn = overlay_blend_sse2_float<true>;
  } else
#if defined(__AVX2__)
  if (env->GetCPUFlags() & CPUF_AVX2) {
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_avx2_uint<true, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_avx2_uint<true, uint16_t>; break;
    }
  } else
#endif
  if (env->GetCPUFlags() & CPUF_SSE4_1) {
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_sse41_uint<true, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_sse41_uint<true, uint16_t>; break;
    }
  }
  else if (env->GetCPUFlags() & CPUF_SSE2) {
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_sse2_uint<true, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_sse2_uint<true, uint16_t>; break;
    }
  }
  else
#endif // INTEL_INTRINSICS
  {
    // pure C
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_c_uint<true, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_c_uint<true, uint16_t>; break;
    case 32: blend_fn = overlay_blend_c_float<true>; break;
    }
  }

  // end of new, float precision inside masked overlays

  if (blend_fn == nullptr)
    env->ThrowError("Blend: no valid internal function");

  for (int p = planeindex_from; p <= planeindex_to; p++) {
    blend_fn(base->GetPtrByIndex(p), overlay->GetPtrByIndex(p), mask->GetPtrByIndex(p),
      base->GetPitchByIndex(p), overlay->GetPitchByIndex(p), mask->GetPitchByIndex(p),
      (w >> base->xSubSamplingShifts[p]), h >> base->ySubSamplingShifts[p], opacity, opacity_f,
      bits_per_pixel);
  }
}

template<typename pixel_t>
void OL_BlendImage::BlendImage(ImageOverlayInternal* base, ImageOverlayInternal* overlay) {
  int w = base->w();
  int h = base->h();

  const int pixelsize = sizeof(pixel_t);

  int planeindex_from = 0;
  int planeindex_to = 0;

  if (of_mode == OF_Blend) {
    planeindex_from = 0;
    planeindex_to = greyscale ? 0 : 2;
  }
  else if (of_mode == OF_Luma) {
    planeindex_from = 0;
    planeindex_to = 0;
  }
  else if (of_mode == OF_Chroma) {
    if (greyscale)
      return;
    planeindex_from = 1;
    planeindex_to = 2;
  }

  if (opacity == 256) {
    for (int p = planeindex_from; p <= planeindex_to; p++) {
      env->BitBlt(base->GetPtrByIndex(p), base->GetPitchByIndex(p), overlay->GetPtrByIndex(p), overlay->GetPitchByIndex(p), (w >> base->xSubSamplingShifts[p]) * pixelsize, h >> base->ySubSamplingShifts[p]);
    }
  }
  else {
    overlay_blend_plane_masked_opacity_t* blend_fn = nullptr;

#ifdef INTEL_INTRINSICS
#if defined(__AVX2__)
  if (pixelsize == 4 && (env->GetCPUFlags() & CPUF_AVX2)) {
    blend_fn = overlay_blend_avx2_float<false>;
  } else
#endif
  if (pixelsize == 4 && (env->GetCPUFlags() & CPUF_SSE2)) {
    blend_fn = overlay_blend_sse2_float<false>;
  } else
#if defined(__AVX2__)
  if (env->GetCPUFlags() & CPUF_AVX2) {
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_avx2_uint<false, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_avx2_uint<false, uint16_t>; break;
    }
  } else
#endif
  if (env->GetCPUFlags() & CPUF_SSE4_1) {
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_sse41_uint<false, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_sse41_uint<false, uint16_t>; break;
    }
  }
  else if (env->GetCPUFlags() & CPUF_SSE2) {
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_sse2_uint<false, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_sse2_uint<false, uint16_t>; break;
    }
  }
  else
#endif // INTEL_INTRINSICS
  {
    // pure C
    switch (bits_per_pixel) {
    case 8: blend_fn = overlay_blend_c_uint<false, uint8_t>; break;
    case 10:
    case 12:
    case 14:
    case 16: blend_fn = overlay_blend_c_uint<false, uint16_t>; break;
    case 32: blend_fn = overlay_blend_c_float<false>; break;
    }
  }
  // end of new, float precision inside masked overlays

    if (blend_fn == nullptr)
      env->ThrowError("Blend: no valid internal function");

    for (int p = planeindex_from; p <= planeindex_to; p++) {
      // no mask ptr
      blend_fn(
        base->GetPtrByIndex(p), overlay->GetPtrByIndex(p), nullptr,
        base->GetPitchByIndex(p), overlay->GetPitchByIndex(p), 0,
        (w >> base->xSubSamplingShifts[p]), h >> base->ySubSamplingShifts[p], opacity, opacity_f,
        bits_per_pixel);
    }

  }
}
