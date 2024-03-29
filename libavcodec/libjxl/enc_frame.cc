// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "lib/jxl/enc_frame.h"

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#include "lib/jxl/ac_context.h"
#include "lib/jxl/ac_strategy.h"
#include "lib/jxl/ans_params.h"
#include "lib/jxl/base/bits.h"
#include "lib/jxl/base/common.h"
#include "lib/jxl/base/compiler_specific.h"
#include "lib/jxl/base/data_parallel.h"
#include "lib/jxl/base/override.h"
#include "lib/jxl/base/printf_macros.h"
#include "lib/jxl/base/status.h"
#include "lib/jxl/chroma_from_luma.h"
#include "lib/jxl/coeff_order.h"
#include "lib/jxl/coeff_order_fwd.h"
#include "lib/jxl/color_encoding_internal.h"
#include "lib/jxl/common.h"  // kMaxNumPasses
#include "lib/jxl/compressed_dc.h"
#include "lib/jxl/dct_util.h"
#include "lib/jxl/dec_external_image.h"
#include "lib/jxl/enc_ac_strategy.h"
#include "lib/jxl/enc_adaptive_quantization.h"
#include "lib/jxl/enc_ans.h"
#include "lib/jxl/enc_ar_control_field.h"
#include "lib/jxl/enc_aux_out.h"
#include "lib/jxl/enc_bit_writer.h"
#include "lib/jxl/enc_cache.h"
#include "lib/jxl/enc_chroma_from_luma.h"
#include "lib/jxl/enc_coeff_order.h"
#include "lib/jxl/enc_context_map.h"
#include "lib/jxl/enc_entropy_coder.h"
#include "lib/jxl/enc_external_image.h"
#include "lib/jxl/enc_fields.h"
#include "lib/jxl/enc_gaborish.h"
#include "lib/jxl/enc_group.h"
#include "lib/jxl/enc_heuristics.h"
#include "lib/jxl/enc_modular.h"
#include "lib/jxl/enc_noise.h"
#include "lib/jxl/enc_params.h"
#include "lib/jxl/enc_patch_dictionary.h"
#include "lib/jxl/enc_photon_noise.h"
#include "lib/jxl/enc_quant_weights.h"
#include "lib/jxl/enc_splines.h"
#include "lib/jxl/enc_toc.h"
#include "lib/jxl/enc_xyb.h"
#include "lib/jxl/fields.h"
#include "lib/jxl/frame_dimensions.h"
#include "lib/jxl/frame_header.h"
#include "lib/jxl/image.h"
#include "lib/jxl/image_bundle.h"
#include "lib/jxl/image_ops.h"
#include "lib/jxl/jpeg/enc_jpeg_data.h"
#include "lib/jxl/loop_filter.h"
#include "lib/jxl/modular/options.h"
#include "lib/jxl/quant_weights.h"
#include "lib/jxl/quantizer.h"
#include "lib/jxl/splines.h"
#include "lib/jxl/toc.h"

namespace jxl {

Status ParamsPostInit(CompressParams* p) {
  if (!p->manual_noise.empty() &&
      p->manual_noise.size() != NoiseParams::kNumNoisePoints) {
    return JXL_FAILURE("Invalid number of noise lut entries");
  }
  if (!p->manual_xyb_factors.empty() && p->manual_xyb_factors.size() != 3) {
    return JXL_FAILURE("Invalid number of XYB quantization factors");
  }
  if (!p->modular_mode && p->butteraugli_distance == 0.0) {
    p->butteraugli_distance = kMinButteraugliDistance;
  }
  if (p->original_butteraugli_distance == -1.0) {
    p->original_butteraugli_distance = p->butteraugli_distance;
  }
  if (p->resampling <= 0) {
    p->resampling = 1;
    // For very low bit rates, using 2x2 resampling gives better results on
    // most photographic images, with an adjusted butteraugli score chosen to
    // give roughly the same amount of bits per pixel.
    if (!p->already_downsampled && p->butteraugli_distance >= 20) {
      p->resampling = 2;
      p->butteraugli_distance = 6 + ((p->butteraugli_distance - 20) * 0.25);
    }
  }
  if (p->ec_resampling <= 0) {
    p->ec_resampling = p->resampling;
  }
  return true;
}

namespace {

template <typename T>
uint32_t GetBitDepth(JxlBitDepth bit_depth, const T& metadata,
                     JxlPixelFormat format) {
  if (bit_depth.type == JXL_BIT_DEPTH_FROM_PIXEL_FORMAT) {
    return BitsPerChannel(format.data_type);
  } else if (bit_depth.type == JXL_BIT_DEPTH_FROM_CODESTREAM) {
    return metadata.bit_depth.bits_per_sample;
  } else if (bit_depth.type == JXL_BIT_DEPTH_CUSTOM) {
    return bit_depth.bits_per_sample;
  } else {
    return 0;
  }
}

Status CopyColorChannels(JxlChunkedFrameInputSource input, size_t x0, size_t y0,
                         size_t xsize, size_t ysize,
                         const FrameInfo& frame_info,
                         const ImageMetadata& metadata, ThreadPool* pool,
                         Image3F* color, ImageF* alpha,
                         bool* has_interleaved_alpha) {
  JxlPixelFormat format = {4, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
  input.get_color_channels_pixel_format(input.opaque, &format);
  *has_interleaved_alpha = format.num_channels == 2 || format.num_channels == 4;
  size_t bits_per_sample =
      GetBitDepth(frame_info.image_bit_depth, metadata, format);
  size_t row_offset;
  auto buffer = GetColorBuffer(input, x0, y0, xsize, ysize, &row_offset);
  if (!buffer) {
    return JXL_FAILURE("no buffer for color channels given");
  }
  size_t color_channels = frame_info.ib_needs_color_transform
                              ? metadata.color_encoding.Channels()
                              : 3;
  if (format.num_channels < color_channels) {
    return JXL_FAILURE("Expected %" PRIuS
                       " color channels, received only %u channels",
                       color_channels, format.num_channels);
  }
  *color = Image3F(xsize, ysize);
  const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.get());
  for (size_t c = 0; c < color_channels; ++c) {
    JXL_RETURN_IF_ERROR(ConvertFromExternalNoSizeCheck(
        data, xsize, ysize, row_offset, bits_per_sample, format, c, pool,
        &color->Plane(c)));
  }
  if (color_channels == 1) {
    CopyImageTo(color->Plane(0), &color->Plane(1));
    CopyImageTo(color->Plane(0), &color->Plane(2));
  }
  if (alpha) {
    if (*has_interleaved_alpha) {
      JXL_RETURN_IF_ERROR(ConvertFromExternalNoSizeCheck(
          data, xsize, ysize, row_offset, bits_per_sample, format,
          format.num_channels - 1, pool, alpha));
    } else {
      // if alpha is not passed, but it is expected, then assume
      // it is all-opaque
      FillImage(1.0f, alpha);
    }
  }
  return true;
}

Status CopyExtraChannels(JxlChunkedFrameInputSource input, size_t x0, size_t y0,
                         size_t xsize, size_t ysize,
                         const FrameInfo& frame_info,
                         const ImageMetadata& metadata,
                         bool has_interleaved_alpha, ThreadPool* pool,
                         std::vector<ImageF>* extra_channels) {
  for (size_t ec = 0; ec < metadata.num_extra_channels; ec++) {
    if (has_interleaved_alpha &&
        metadata.extra_channel_info[ec].type == ExtraChannel::kAlpha) {
      // Skip this alpha channel, but still request additional alpha channels
      // if they exist.
      has_interleaved_alpha = false;
      continue;
    }
    JxlPixelFormat ec_format = {1, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
    input.get_extra_channel_pixel_format(input.opaque, ec, &ec_format);
    ec_format.num_channels = 1;
    size_t row_offset;
    auto buffer =
        GetExtraChannelBuffer(input, ec, x0, y0, xsize, ysize, &row_offset);
    if (!buffer) {
      return JXL_FAILURE("no buffer for extra channel given");
    }
    size_t bits_per_sample = GetBitDepth(
        frame_info.image_bit_depth, metadata.extra_channel_info[ec], ec_format);
    if (!ConvertFromExternalNoSizeCheck(
            reinterpret_cast<const uint8_t*>(buffer.get()), xsize, ysize,
            row_offset, bits_per_sample, ec_format, 0, pool,
            &(*extra_channels)[ec])) {
      return JXL_FAILURE("Failed to set buffer for extra channel");
    }
  }
  return true;
}

void SetProgressiveMode(const CompressParams& cparams,
                        ProgressiveSplitter* progressive_splitter) {
  constexpr PassDefinition progressive_passes_dc_vlf_lf_full_ac[] = {
      {/*num_coefficients=*/2, /*shift=*/0,
       /*suitable_for_downsampling_of_at_least=*/4},
      {/*num_coefficients=*/3, /*shift=*/0,
       /*suitable_for_downsampling_of_at_least=*/2},
      {/*num_coefficients=*/8, /*shift=*/0,
       /*suitable_for_downsampling_of_at_least=*/0},
  };
  constexpr PassDefinition progressive_passes_dc_quant_ac_full_ac[] = {
      {/*num_coefficients=*/8, /*shift=*/1,
       /*suitable_for_downsampling_of_at_least=*/2},
      {/*num_coefficients=*/8, /*shift=*/0,
       /*suitable_for_downsampling_of_at_least=*/0},
  };
  if (cparams.custom_progressive_mode) {
    progressive_splitter->SetProgressiveMode(*cparams.custom_progressive_mode);
  } else if (cparams.qprogressive_mode) {
    progressive_splitter->SetProgressiveMode(
        ProgressiveMode{progressive_passes_dc_quant_ac_full_ac});
  } else if (cparams.progressive_mode) {
    progressive_splitter->SetProgressiveMode(
        ProgressiveMode{progressive_passes_dc_vlf_lf_full_ac});
  }
}

uint64_t FrameFlagsFromParams(const CompressParams& cparams) {
  uint64_t flags = 0;

  const float dist = cparams.butteraugli_distance;

  // We don't add noise at low butteraugli distances because the original
  // noise is stored within the compressed image and adding noise makes things
  // worse.
  if (ApplyOverride(cparams.noise, dist >= kMinButteraugliForNoise) ||
      cparams.photon_noise_iso > 0 ||
      cparams.manual_noise.size() == NoiseParams::kNumNoisePoints) {
    flags |= FrameHeader::kNoise;
  }

  if (cparams.progressive_dc > 0 && cparams.modular_mode == false) {
    flags |= FrameHeader::kUseDcFrame;
  }

  return flags;
}

Status LoopFilterFromParams(const CompressParams& cparams,
                            FrameHeader* JXL_RESTRICT frame_header) {
  LoopFilter* loop_filter = &frame_header->loop_filter;

  // Gaborish defaults to enabled in Hare or slower.
  loop_filter->gab = ApplyOverride(
      cparams.gaborish, cparams.speed_tier <= SpeedTier::kHare &&
                            frame_header->encoding == FrameEncoding::kVarDCT &&
                            cparams.decoding_speed_tier < 4);

  if (cparams.epf != -1) {
    loop_filter->epf_iters = cparams.epf;
  } else {
    if (frame_header->encoding == FrameEncoding::kModular) {
      loop_filter->epf_iters = 0;
    } else {
      constexpr float kThresholds[3] = {0.7, 1.5, 4.0};
      loop_filter->epf_iters = 0;
      if (cparams.decoding_speed_tier < 3) {
        for (size_t i = cparams.decoding_speed_tier == 2 ? 1 : 0; i < 3; i++) {
          if (cparams.butteraugli_distance >= kThresholds[i]) {
            loop_filter->epf_iters++;
          }
        }
      }
    }
  }
  // Strength of EPF in modular mode.
  if (frame_header->encoding == FrameEncoding::kModular &&
      !cparams.IsLossless()) {
    // TODO(veluca): this formula is nonsense.
    loop_filter->epf_sigma_for_modular = cparams.butteraugli_distance;
  }
  if (frame_header->encoding == FrameEncoding::kModular &&
      cparams.lossy_palette) {
    loop_filter->epf_sigma_for_modular = 1.0f;
  }

  return true;
}

Status MakeFrameHeader(size_t xsize, size_t ysize,
                       const CompressParams& cparams,
                       const ProgressiveSplitter& progressive_splitter,
                       const FrameInfo& frame_info,
                       const jpeg::JPEGData* jpeg_data,
                       FrameHeader* JXL_RESTRICT frame_header) {
  frame_header->nonserialized_is_preview = frame_info.is_preview;
  frame_header->is_last = frame_info.is_last;
  frame_header->save_before_color_transform =
      frame_info.save_before_color_transform;
  frame_header->frame_type = frame_info.frame_type;
  frame_header->name = frame_info.name;

  progressive_splitter.InitPasses(&frame_header->passes);

  if (cparams.modular_mode) {
    frame_header->encoding = FrameEncoding::kModular;
    if (cparams.modular_group_size_shift == -1) {
      frame_header->group_size_shift = 1;
      // no point using groups when only one group is full and the others are
      // less than half full: multithreading will not really help much, while
      // compression does suffer
      if (xsize <= 400 && ysize <= 400) {
        frame_header->group_size_shift = 2;
      }
    } else {
      frame_header->group_size_shift = cparams.modular_group_size_shift;
    }
  }

  if (jpeg_data) {
    // we are transcoding a JPEG, so we don't get to choose
    frame_header->encoding = FrameEncoding::kVarDCT;
    JXL_RETURN_IF_ERROR(SetChromaSubsamplingFromJpegData(
        *jpeg_data, &frame_header->chroma_subsampling));
    JXL_RETURN_IF_ERROR(SetColorTransformFromJpegData(
        *jpeg_data, &frame_header->color_transform));
  } else {
    frame_header->color_transform = cparams.color_transform;
    if (!cparams.modular_mode &&
        (frame_header->chroma_subsampling.MaxHShift() != 0 ||
         frame_header->chroma_subsampling.MaxVShift() != 0)) {
      return JXL_FAILURE(
          "Chroma subsampling is not supported in VarDCT mode when not "
          "recompressing JPEGs");
    }
  }
  if (frame_header->color_transform != ColorTransform::kYCbCr &&
      (frame_header->chroma_subsampling.MaxHShift() != 0 ||
       frame_header->chroma_subsampling.MaxVShift() != 0)) {
    return JXL_FAILURE(
        "Chroma subsampling is not supported when color transform is not "
        "YCbCr");
  }

  frame_header->flags = FrameFlagsFromParams(cparams);
  // Non-photon noise is not supported in the Modular encoder for now.
  if (frame_header->encoding != FrameEncoding::kVarDCT &&
      cparams.photon_noise_iso == 0 && cparams.manual_noise.empty()) {
    frame_header->UpdateFlag(false, FrameHeader::Flags::kNoise);
  }

  JXL_RETURN_IF_ERROR(LoopFilterFromParams(cparams, frame_header));

  frame_header->dc_level = frame_info.dc_level;
  if (frame_header->dc_level > 2) {
    // With 3 or more progressive_dc frames, the implementation does not yet
    // work, see enc_cache.cc.
    return JXL_FAILURE("progressive_dc > 2 is not yet supported");
  }
  if (cparams.progressive_dc > 0 &&
      (cparams.ec_resampling != 1 || cparams.resampling != 1)) {
    return JXL_FAILURE("Resampling not supported with DC frames");
  }
  if (cparams.resampling != 1 && cparams.resampling != 2 &&
      cparams.resampling != 4 && cparams.resampling != 8) {
    return JXL_FAILURE("Invalid resampling factor");
  }
  if (cparams.ec_resampling != 1 && cparams.ec_resampling != 2 &&
      cparams.ec_resampling != 4 && cparams.ec_resampling != 8) {
    return JXL_FAILURE("Invalid ec_resampling factor");
  }
  // Resized frames.
  if (frame_info.frame_type != FrameType::kDCFrame) {
    frame_header->frame_origin = frame_info.origin;
    size_t ups = 1;
    if (cparams.already_downsampled) ups = cparams.resampling;

    // TODO(lode): this is not correct in case of odd original image sizes in
    // combination with cparams.already_downsampled. Likely these values should
    // be set to respectively frame_header->default_xsize() and
    // frame_header->default_ysize() instead, the original (non downsampled)
    // intended decoded image dimensions. But it may be more subtle than that
    // if combined with crop. This issue causes custom_size_or_origin to be
    // incorrectly set to true in case of already_downsampled with odd output
    // image size when no cropping is used.
    frame_header->frame_size.xsize = xsize * ups;
    frame_header->frame_size.ysize = ysize * ups;
    if (frame_info.origin.x0 != 0 || frame_info.origin.y0 != 0 ||
        frame_header->frame_size.xsize != frame_header->default_xsize() ||
        frame_header->frame_size.ysize != frame_header->default_ysize()) {
      frame_header->custom_size_or_origin = true;
    }
  }
  // Upsampling.
  frame_header->upsampling = cparams.resampling;
  const std::vector<ExtraChannelInfo>& extra_channels =
      frame_header->nonserialized_metadata->m.extra_channel_info;
  frame_header->extra_channel_upsampling.clear();
  frame_header->extra_channel_upsampling.resize(extra_channels.size(),
                                                cparams.ec_resampling);
  frame_header->save_as_reference = frame_info.save_as_reference;

  // Set blending-related information.
  if (frame_info.blend || frame_header->custom_size_or_origin) {
    // Set blend_channel to the first alpha channel. These values are only
    // encoded in case a blend mode involving alpha is used and there are more
    // than one extra channels.
    size_t index = 0;
    if (frame_info.alpha_channel == -1) {
      if (extra_channels.size() > 1) {
        for (size_t i = 0; i < extra_channels.size(); i++) {
          if (extra_channels[i].type == ExtraChannel::kAlpha) {
            index = i;
            break;
          }
        }
      }
    } else {
      index = static_cast<size_t>(frame_info.alpha_channel);
      JXL_ASSERT(index == 0 || index < extra_channels.size());
    }
    frame_header->blending_info.alpha_channel = index;
    frame_header->blending_info.mode =
        frame_info.blend ? frame_info.blendmode : BlendMode::kReplace;
    frame_header->blending_info.source = frame_info.source;
    frame_header->blending_info.clamp = frame_info.clamp;
    const auto& extra_channel_info = frame_info.extra_channel_blending_info;
    for (size_t i = 0; i < extra_channels.size(); i++) {
      if (i < extra_channel_info.size()) {
        frame_header->extra_channel_blending_info[i] = extra_channel_info[i];
      } else {
        frame_header->extra_channel_blending_info[i].alpha_channel = index;
        BlendMode default_blend = frame_info.blendmode;
        if (extra_channels[i].type != ExtraChannel::kBlack && i != index) {
          // K needs to be blended, spot colors and other stuff gets added
          default_blend = BlendMode::kAdd;
        }
        frame_header->extra_channel_blending_info[i].mode =
            frame_info.blend ? default_blend : BlendMode::kReplace;
        frame_header->extra_channel_blending_info[i].source = 1;
      }
    }
  }

  frame_header->animation_frame.duration = frame_info.duration;
  frame_header->animation_frame.timecode = frame_info.timecode;

  return true;
}

// Invisible (alpha = 0) pixels tend to be a mess in optimized PNGs.
// Since they have no visual impact whatsoever, we can replace them with
// something that compresses better and reduces artifacts near the edges. This
// does some kind of smooth stuff that seems to work.
// Replace invisible pixels with a weighted average of the pixel to the left,
// the pixel to the topright, and non-invisible neighbours.
// Produces downward-blurry smears, with in the upwards direction only a 1px
// edge duplication but not more. It would probably be better to smear in all
// directions. That requires an alpha-weighed convolution with a large enough
// kernel though, which might be overkill...
void SimplifyInvisible(Image3F* image, const ImageF& alpha, bool lossless) {
  for (size_t c = 0; c < 3; ++c) {
    for (size_t y = 0; y < image->ysize(); ++y) {
      float* JXL_RESTRICT row = image->PlaneRow(c, y);
      const float* JXL_RESTRICT prow =
          (y > 0 ? image->PlaneRow(c, y - 1) : nullptr);
      const float* JXL_RESTRICT nrow =
          (y + 1 < image->ysize() ? image->PlaneRow(c, y + 1) : nullptr);
      const float* JXL_RESTRICT a = alpha.Row(y);
      const float* JXL_RESTRICT pa = (y > 0 ? alpha.Row(y - 1) : nullptr);
      const float* JXL_RESTRICT na =
          (y + 1 < image->ysize() ? alpha.Row(y + 1) : nullptr);
      for (size_t x = 0; x < image->xsize(); ++x) {
        if (a[x] == 0) {
          if (lossless) {
            row[x] = 0;
            continue;
          }
          float d = 0.f;
          row[x] = 0;
          if (x > 0) {
            row[x] += row[x - 1];
            d++;
            if (a[x - 1] > 0.f) {
              row[x] += row[x - 1];
              d++;
            }
          }
          if (x + 1 < image->xsize()) {
            if (y > 0) {
              row[x] += prow[x + 1];
              d++;
            }
            if (a[x + 1] > 0.f) {
              row[x] += 2.f * row[x + 1];
              d += 2.f;
            }
            if (y > 0 && pa[x + 1] > 0.f) {
              row[x] += 2.f * prow[x + 1];
              d += 2.f;
            }
            if (y + 1 < image->ysize() && na[x + 1] > 0.f) {
              row[x] += 2.f * nrow[x + 1];
              d += 2.f;
            }
          }
          if (y > 0 && pa[x] > 0.f) {
            row[x] += 2.f * prow[x];
            d += 2.f;
          }
          if (y + 1 < image->ysize() && na[x] > 0.f) {
            row[x] += 2.f * nrow[x];
            d += 2.f;
          }
          if (d > 1.f) row[x] /= d;
        }
      }
    }
  }
}

struct PixelStatsForChromacityAdjustment {
  float dx = 0;
  float db = 0;
  float exposed_blue = 0;
  float CalcPlane(const ImageF* JXL_RESTRICT plane) const {
    float xmax = 0;
    float ymax = 0;
    for (size_t ty = 1; ty < plane->ysize(); ++ty) {
      for (size_t tx = 1; tx < plane->xsize(); ++tx) {
        float cur = plane->Row(ty)[tx];
        float prev_row = plane->Row(ty - 1)[tx];
        float prev = plane->Row(ty)[tx - 1];
        xmax = std::max(xmax, std::abs(cur - prev));
        ymax = std::max(ymax, std::abs(cur - prev_row));
      }
    }
    return std::max(xmax, ymax);
  }
  void CalcExposedBlue(const ImageF* JXL_RESTRICT plane_y,
                       const ImageF* JXL_RESTRICT plane_b) {
    float eb = 0;
    float xmax = 0;
    float ymax = 0;
    for (size_t ty = 1; ty < plane_y->ysize(); ++ty) {
      for (size_t tx = 1; tx < plane_y->xsize(); ++tx) {
        float cur_y = plane_y->Row(ty)[tx];
        float cur_b = plane_b->Row(ty)[tx];
        float exposed_b = cur_b - cur_y * 1.2;
        float diff_b = cur_b - cur_y;
        float prev_row = plane_b->Row(ty - 1)[tx];
        float prev = plane_b->Row(ty)[tx - 1];
        float diff_prev_row = prev_row - plane_y->Row(ty - 1)[tx];
        float diff_prev = prev - plane_y->Row(ty)[tx - 1];
        xmax = std::max(xmax, std::abs(diff_b - diff_prev));
        ymax = std::max(ymax, std::abs(diff_b - diff_prev_row));
        if (exposed_b >= 0) {
          exposed_b *= fabs(cur_b - prev) + fabs(cur_b - prev_row);
          eb = std::max(eb, exposed_b);
        }
      }
    }
    exposed_blue = eb;
    db = std::max(xmax, ymax);
  }
  void Calc(const Image3F* JXL_RESTRICT opsin) {
    dx = CalcPlane(&opsin->Plane(0));
    CalcExposedBlue(&opsin->Plane(1), &opsin->Plane(2));
  }
  int HowMuchIsXChannelPixelized() {
    if (dx >= 0.03) {
      return 2;
    }
    if (dx >= 0.017) {
      return 1;
    }
    return 0;
  }
  int HowMuchIsBChannelPixelized() {
    int add = exposed_blue >= 0.13 ? 1 : 0;
    if (db > 0.38) {
      return 2 + add;
    }
    if (db > 0.33) {
      return 1 + add;
    }
    if (db > 0.28) {
      return add;
    }
    return 0;
  }
};

void ComputeChromacityAdjustments(const CompressParams& cparams,
                                  bool color_is_jpeg, const Image3F& opsin,
                                  FrameHeader* frame_header) {
  if (frame_header->encoding != FrameEncoding::kVarDCT) {
    return;
  }
  if (color_is_jpeg) {
    frame_header->x_qm_scale = 2;
    frame_header->b_qm_scale = 2;
    return;
  }
  if (cparams.max_error_mode) {
    return;
  }
  // 1) Distance based approach for chromacity adjustment:
  float x_qm_scale_steps[4] = {1.25f, 7.0f, 15.0f, 24.0f};
  frame_header->x_qm_scale = 2;
  for (float x_qm_scale_step : x_qm_scale_steps) {
    if (cparams.original_butteraugli_distance > x_qm_scale_step) {
      frame_header->x_qm_scale++;
    }
  }
  if (cparams.butteraugli_distance < 0.299f) {
    // Favor chromacity preservation for making images appear more
    // faithful to original even with extreme (5-10x) zooming.
    frame_header->x_qm_scale++;
  }
  // 2) Pixel-based approach for chromacity adjustment:
  // look at the individual pixels and make a guess how difficult
  // the image would be based on the worst case pixel.
  PixelStatsForChromacityAdjustment pixel_stats;
  if (cparams.speed_tier <= SpeedTier::kWombat) {
    pixel_stats.Calc(&opsin);
  }
  // For X take the most severe adjustment.
  frame_header->x_qm_scale = std::max<int>(
      frame_header->x_qm_scale, 2 + pixel_stats.HowMuchIsXChannelPixelized());
  // B only adjusted by pixel-based approach.
  frame_header->b_qm_scale = 2 + pixel_stats.HowMuchIsBChannelPixelized();
}

void ComputeNoiseParams(const CompressParams& cparams, bool color_is_jpeg,
                        const Image3F& opsin, const FrameDimensions& frame_dim,
                        FrameHeader* frame_header, NoiseParams* noise_params) {
  if (cparams.photon_noise_iso > 0) {
    *noise_params = SimulatePhotonNoise(frame_dim.xsize, frame_dim.ysize,
                                        cparams.photon_noise_iso);
  } else if (cparams.manual_noise.size() == NoiseParams::kNumNoisePoints) {
    for (size_t i = 0; i < NoiseParams::kNumNoisePoints; i++) {
      noise_params->lut[i] = cparams.manual_noise[i];
    }
  } else if (frame_header->encoding == FrameEncoding::kVarDCT &&
             frame_header->flags & FrameHeader::kNoise && !color_is_jpeg) {
    // Don't start at zero amplitude since adding noise is expensive -- it
    // significantly slows down decoding, and this is unlikely to
    // completely go away even with advanced optimizations. After the
    // kNoiseModelingRampUpDistanceRange we have reached the full level,
    // i.e. noise is no longer represented by the compressed image, so we
    // can add full noise by the noise modeling itself.
    static const float kNoiseModelingRampUpDistanceRange = 0.6;
    static const float kNoiseLevelAtStartOfRampUp = 0.25;
    static const float kNoiseRampupStart = 1.0;
    // TODO(user) test and properly select quality_coef with smooth
    // filter
    float quality_coef = 1.0f;
    const float rampup = (cparams.butteraugli_distance - kNoiseRampupStart) /
                         kNoiseModelingRampUpDistanceRange;
    if (rampup < 1.0f) {
      quality_coef = kNoiseLevelAtStartOfRampUp +
                     (1.0f - kNoiseLevelAtStartOfRampUp) * rampup;
    }
    if (rampup < 0.0f) {
      quality_coef = kNoiseRampupStart;
    }
    if (!GetNoiseParameter(opsin, noise_params, quality_coef)) {
      frame_header->flags &= ~FrameHeader::kNoise;
    }
  }
}

void DownsampleColorChannels(const CompressParams& cparams,
                             const FrameHeader& frame_header,
                             bool color_is_jpeg, Image3F* opsin) {
  if (color_is_jpeg || frame_header.upsampling == 1 ||
      cparams.already_downsampled) {
    return;
  }
  if (frame_header.encoding == FrameEncoding::kVarDCT &&
      frame_header.upsampling == 2) {
    // TODO(lode): use the regular DownsampleImage, or adapt to the custom
    // coefficients, if there is are custom upscaling coefficients in
    // CustomTransformData
    if (cparams.speed_tier <= SpeedTier::kSquirrel) {
      // TODO(lode): DownsampleImage2_Iterative is currently too slow to
      // be used for squirrel, make it faster, and / or enable it only for
      // kitten.
      DownsampleImage2_Iterative(opsin);
    } else {
      DownsampleImage2_Sharper(opsin);
    }
  } else {
    DownsampleImage(opsin, frame_header.upsampling);
  }
  if (frame_header.encoding == FrameEncoding::kVarDCT) {
    PadImageToBlockMultipleInPlace(opsin);
  }
}

template <typename V, typename R>
void FindIndexOfSumMaximum(const V* array, const size_t len, R* idx, V* sum) {
  JXL_ASSERT(len > 0);
  V maxval = 0;
  V val = 0;
  R maxidx = 0;
  for (size_t i = 0; i < len; ++i) {
    val += array[i];
    if (val > maxval) {
      maxval = val;
      maxidx = i;
    }
  }
  *idx = maxidx;
  *sum = maxval;
}

Status ComputeJPEGTranscodingData(const jpeg::JPEGData& jpeg_data,
                                  ThreadPool* pool,
                                  ModularFrameEncoder* modular_frame_encoder,
                                  PassesEncoderState* enc_state) {
  PassesSharedState& shared = enc_state->shared;
  FrameHeader& frame_header = shared.frame_header;
  const FrameDimensions& frame_dim = shared.frame_dim;

  const size_t xsize = frame_dim.xsize_padded;
  const size_t ysize = frame_dim.ysize_padded;
  const size_t xsize_blocks = frame_dim.xsize_blocks;
  const size_t ysize_blocks = frame_dim.ysize_blocks;

  // no-op chroma from luma
  shared.cmap = ColorCorrelationMap(xsize, ysize, false);
  shared.ac_strategy.FillDCT8();
  FillImage(uint8_t(0), &shared.epf_sharpness);

  enc_state->coeffs.clear();
  while (enc_state->coeffs.size() < enc_state->passes.size()) {
    enc_state->coeffs.emplace_back(make_unique<ACImageT<int32_t>>(
        kGroupDim * kGroupDim, frame_dim.num_groups));
  }

  // convert JPEG quantization table to a Quantizer object
  float dcquantization[3];
  std::vector<QuantEncoding> qe(DequantMatrices::kNum,
                                QuantEncoding::Library(0));

  auto jpeg_c_map =
      JpegOrder(frame_header.color_transform, jpeg_data.components.size() == 1);

  std::vector<int> qt(192);
  for (size_t c = 0; c < 3; c++) {
    size_t jpeg_c = jpeg_c_map[c];
    const int32_t* quant =
        jpeg_data.quant[jpeg_data.components[jpeg_c].quant_idx].values.data();

    dcquantization[c] = 255 * 8.0f / quant[0];
    for (size_t y = 0; y < 8; y++) {
      for (size_t x = 0; x < 8; x++) {
        // JPEG XL transposes the DCT, JPEG doesn't.
        qt[c * 64 + 8 * x + y] = quant[8 * y + x];
      }
    }
  }
  DequantMatricesSetCustomDC(&shared.matrices, dcquantization);
  float dcquantization_r[3] = {1.0f / dcquantization[0],
                               1.0f / dcquantization[1],
                               1.0f / dcquantization[2]};

  qe[AcStrategy::Type::DCT] = QuantEncoding::RAW(qt);
  DequantMatricesSetCustom(&shared.matrices, qe, modular_frame_encoder);

  // Ensure that InvGlobalScale() is 1.
  shared.quantizer = Quantizer(&shared.matrices, 1, kGlobalScaleDenom);
  // Recompute MulDC() and InvMulDC().
  shared.quantizer.RecomputeFromGlobalScale();

  // Per-block dequant scaling should be 1.
  FillImage(static_cast<int32_t>(shared.quantizer.InvGlobalScale()),
            &shared.raw_quant_field);

  std::vector<int32_t> scaled_qtable(192);
  for (size_t c = 0; c < 3; c++) {
    for (size_t i = 0; i < 64; i++) {
      scaled_qtable[64 * c + i] =
          (1 << kCFLFixedPointPrecision) * qt[64 + i] / qt[64 * c + i];
    }
  }

  auto jpeg_row = [&](size_t c, size_t y) {
    return jpeg_data.components[jpeg_c_map[c]].coeffs.data() +
           jpeg_data.components[jpeg_c_map[c]].width_in_blocks * kDCTBlockSize *
               y;
  };

  bool DCzero = (frame_header.color_transform == ColorTransform::kYCbCr);
  // Compute chroma-from-luma for AC (doesn't seem to be useful for DC)
  if (frame_header.chroma_subsampling.Is444() &&
      enc_state->cparams.force_cfl_jpeg_recompression &&
      jpeg_data.components.size() == 3) {
    for (size_t c : {0, 2}) {
      ImageSB* map = (c == 0 ? &shared.cmap.ytox_map : &shared.cmap.ytob_map);
      const float kScale = kDefaultColorFactor;
      const int kOffset = 127;
      const float kBase =
          c == 0 ? shared.cmap.YtoXRatio(0) : shared.cmap.YtoBRatio(0);
      const float kZeroThresh =
          kScale * kZeroBiasDefault[c] *
          0.9999f;  // just epsilon less for better rounding

      auto process_row = [&](const uint32_t task, const size_t thread) {
        size_t ty = task;
        int8_t* JXL_RESTRICT row_out = map->Row(ty);
        for (size_t tx = 0; tx < map->xsize(); ++tx) {
          const size_t y0 = ty * kColorTileDimInBlocks;
          const size_t x0 = tx * kColorTileDimInBlocks;
          const size_t y1 = std::min(frame_dim.ysize_blocks,
                                     (ty + 1) * kColorTileDimInBlocks);
          const size_t x1 = std::min(frame_dim.xsize_blocks,
                                     (tx + 1) * kColorTileDimInBlocks);
          int32_t d_num_zeros[257] = {0};
          // TODO(veluca): this needs SIMD + fixed point adaptation, and/or
          // conversion to the new CfL algorithm.
          for (size_t y = y0; y < y1; ++y) {
            const int16_t* JXL_RESTRICT row_m = jpeg_row(1, y);
            const int16_t* JXL_RESTRICT row_s = jpeg_row(c, y);
            for (size_t x = x0; x < x1; ++x) {
              for (size_t coeffpos = 1; coeffpos < kDCTBlockSize; coeffpos++) {
                const float scaled_m = row_m[x * kDCTBlockSize + coeffpos] *
                                       scaled_qtable[64 * c + coeffpos] *
                                       (1.0f / (1 << kCFLFixedPointPrecision));
                const float scaled_s =
                    kScale * row_s[x * kDCTBlockSize + coeffpos] +
                    (kOffset - kBase * kScale) * scaled_m;
                if (std::abs(scaled_m) > 1e-8f) {
                  float from, to;
                  if (scaled_m > 0) {
                    from = (scaled_s - kZeroThresh) / scaled_m;
                    to = (scaled_s + kZeroThresh) / scaled_m;
                  } else {
                    from = (scaled_s + kZeroThresh) / scaled_m;
                    to = (scaled_s - kZeroThresh) / scaled_m;
                  }
                  if (from < 0.0f) {
                    from = 0.0f;
                  }
                  if (to > 255.0f) {
                    to = 255.0f;
                  }
                  // Instead of clamping the both values
                  // we just check that range is sane.
                  if (from <= to) {
                    d_num_zeros[static_cast<int>(std::ceil(from))]++;
                    d_num_zeros[static_cast<int>(std::floor(to + 1))]--;
                  }
                }
              }
            }
          }
          int best = 0;
          int32_t best_sum = 0;
          FindIndexOfSumMaximum(d_num_zeros, 256, &best, &best_sum);
          int32_t offset_sum = 0;
          for (int i = 0; i < 256; ++i) {
            if (i <= kOffset) {
              offset_sum += d_num_zeros[i];
            }
          }
          row_out[tx] = 0;
          if (best_sum > offset_sum + 1) {
            row_out[tx] = best - kOffset;
          }
        }
      };

      JXL_RETURN_IF_ERROR(RunOnPool(pool, 0, map->ysize(), ThreadPool::NoInit,
                                    process_row, "FindCorrelation"));
    }
  }

  Image3F dc = Image3F(xsize_blocks, ysize_blocks);
  if (!frame_header.chroma_subsampling.Is444()) {
    ZeroFillImage(&dc);
    for (auto& coeff : enc_state->coeffs) {
      coeff->ZeroFill();
    }
  }
  // JPEG DC is from -1024 to 1023.
  std::vector<size_t> dc_counts[3] = {};
  dc_counts[0].resize(2048);
  dc_counts[1].resize(2048);
  dc_counts[2].resize(2048);
  size_t total_dc[3] = {};
  for (size_t c : {1, 0, 2}) {
    if (jpeg_data.components.size() == 1 && c != 1) {
      for (auto& coeff : enc_state->coeffs) {
        coeff->ZeroFillPlane(c);
      }
      ZeroFillImage(&dc.Plane(c));
      // Ensure no division by 0.
      dc_counts[c][1024] = 1;
      total_dc[c] = 1;
      continue;
    }
    size_t hshift = frame_header.chroma_subsampling.HShift(c);
    size_t vshift = frame_header.chroma_subsampling.VShift(c);
    ImageSB& map = (c == 0 ? shared.cmap.ytox_map : shared.cmap.ytob_map);
    for (size_t group_index = 0; group_index < frame_dim.num_groups;
         group_index++) {
      const size_t gx = group_index % frame_dim.xsize_groups;
      const size_t gy = group_index / frame_dim.xsize_groups;
      int32_t* coeffs[kMaxNumPasses];
      for (size_t i = 0; i < enc_state->coeffs.size(); i++) {
        coeffs[i] = enc_state->coeffs[i]->PlaneRow(c, group_index, 0).ptr32;
      }
      int32_t block[64];
      for (size_t by = gy * kGroupDimInBlocks;
           by < ysize_blocks && by < (gy + 1) * kGroupDimInBlocks; ++by) {
        if ((by >> vshift) << vshift != by) continue;
        const int16_t* JXL_RESTRICT inputjpeg = jpeg_row(c, by >> vshift);
        const int16_t* JXL_RESTRICT inputjpegY = jpeg_row(1, by);
        float* JXL_RESTRICT fdc = dc.PlaneRow(c, by >> vshift);
        const int8_t* JXL_RESTRICT cm =
            map.ConstRow(by / kColorTileDimInBlocks);
        for (size_t bx = gx * kGroupDimInBlocks;
             bx < xsize_blocks && bx < (gx + 1) * kGroupDimInBlocks; ++bx) {
          if ((bx >> hshift) << hshift != bx) continue;
          size_t base = (bx >> hshift) * kDCTBlockSize;
          int idc;
          if (DCzero) {
            idc = inputjpeg[base];
          } else {
            idc = inputjpeg[base] + 1024 / qt[c * 64];
          }
          dc_counts[c][std::min(static_cast<uint32_t>(idc + 1024),
                                uint32_t(2047))]++;
          total_dc[c]++;
          fdc[bx >> hshift] = idc * dcquantization_r[c];
          if (c == 1 || !enc_state->cparams.force_cfl_jpeg_recompression ||
              !frame_header.chroma_subsampling.Is444()) {
            for (size_t y = 0; y < 8; y++) {
              for (size_t x = 0; x < 8; x++) {
                block[y * 8 + x] = inputjpeg[base + x * 8 + y];
              }
            }
          } else {
            const int32_t scale =
                shared.cmap.RatioJPEG(cm[bx / kColorTileDimInBlocks]);

            for (size_t y = 0; y < 8; y++) {
              for (size_t x = 0; x < 8; x++) {
                int Y = inputjpegY[kDCTBlockSize * bx + x * 8 + y];
                int QChroma = inputjpeg[kDCTBlockSize * bx + x * 8 + y];
                // Fixed-point multiply of CfL scale with quant table ratio
                // first, and Y value second.
                int coeff_scale = (scale * scaled_qtable[64 * c + y * 8 + x] +
                                   (1 << (kCFLFixedPointPrecision - 1))) >>
                                  kCFLFixedPointPrecision;
                int cfl_factor =
                    (Y * coeff_scale + (1 << (kCFLFixedPointPrecision - 1))) >>
                    kCFLFixedPointPrecision;
                int QCR = QChroma - cfl_factor;
                block[y * 8 + x] = QCR;
              }
            }
          }
          enc_state->progressive_splitter.SplitACCoefficients(
              block, AcStrategy::FromRawStrategy(AcStrategy::Type::DCT), bx, by,
              coeffs);
          for (size_t i = 0; i < enc_state->coeffs.size(); i++) {
            coeffs[i] += kDCTBlockSize;
          }
        }
      }
    }
  }

  auto& dct = enc_state->shared.block_ctx_map.dc_thresholds;
  auto& num_dc_ctxs = enc_state->shared.block_ctx_map.num_dc_ctxs;
  num_dc_ctxs = 1;
  for (size_t i = 0; i < 3; i++) {
    dct[i].clear();
    int num_thresholds = (CeilLog2Nonzero(total_dc[i]) - 12) / 2;
    // up to 3 buckets per channel:
    // dark/medium/bright, yellow/unsat/blue, green/unsat/red
    num_thresholds = std::min(std::max(num_thresholds, 0), 2);
    size_t cumsum = 0;
    size_t cut = total_dc[i] / (num_thresholds + 1);
    for (int j = 0; j < 2048; j++) {
      cumsum += dc_counts[i][j];
      if (cumsum > cut) {
        dct[i].push_back(j - 1025);
        cut = total_dc[i] * (dct[i].size() + 1) / (num_thresholds + 1);
      }
    }
    num_dc_ctxs *= dct[i].size() + 1;
  }

  auto& ctx_map = enc_state->shared.block_ctx_map.ctx_map;
  ctx_map.clear();
  ctx_map.resize(3 * kNumOrders * num_dc_ctxs, 0);

  int lbuckets = (dct[1].size() + 1);
  for (size_t i = 0; i < num_dc_ctxs; i++) {
    // up to 9 contexts for luma
    ctx_map[i] = i / lbuckets;
    // up to 3 contexts for chroma
    ctx_map[kNumOrders * num_dc_ctxs + i] =
        ctx_map[2 * kNumOrders * num_dc_ctxs + i] =
            num_dc_ctxs / lbuckets + (i % lbuckets);
  }
  enc_state->shared.block_ctx_map.num_ctxs =
      *std::max_element(ctx_map.begin(), ctx_map.end()) + 1;

  enc_state->histogram_idx.resize(shared.frame_dim.num_groups);

  // disable DC frame for now
  frame_header.UpdateFlag(false, FrameHeader::kUseDcFrame);
  auto compute_dc_coeffs = [&](const uint32_t group_index,
                               size_t /* thread */) {
    modular_frame_encoder->AddVarDCTDC(dc, group_index, /*nl_dc=*/false,
                                       enc_state, /*jpeg_transcode=*/true);
    modular_frame_encoder->AddACMetadata(group_index, /*jpeg_transcode=*/true,
                                         enc_state);
  };
  JXL_RETURN_IF_ERROR(RunOnPool(pool, 0, shared.frame_dim.num_dc_groups,
                                ThreadPool::NoInit, compute_dc_coeffs,
                                "Compute DC coeffs"));

  frame_header.UpdateFlag(true, FrameHeader::kSkipAdaptiveDCSmoothing);

  return true;
}

Status ComputeVarDCTEncodingData(const Image3F* linear,
                                 Image3F* JXL_RESTRICT opsin,
                                 const JxlCmsInterface& cms, ThreadPool* pool,
                                 ModularFrameEncoder* modular_frame_encoder,
                                 PassesEncoderState* enc_state,
                                 AuxOut* aux_out) {
  JXL_ASSERT((opsin->xsize() % kBlockDim) == 0 &&
             (opsin->ysize() % kBlockDim) == 0);
  JXL_RETURN_IF_ERROR(LossyFrameHeuristics(enc_state, modular_frame_encoder,
                                           linear, opsin, cms, pool, aux_out));

  JXL_RETURN_IF_ERROR(InitializePassesEncoder(*opsin, cms, pool, enc_state,
                                              modular_frame_encoder, aux_out));

  return true;
}

void ComputeAllCoeffOrders(PassesEncoderState& enc_state,
                           const FrameDimensions& frame_dim) {
  auto used_orders_info = ComputeUsedOrders(
      enc_state.cparams.speed_tier, enc_state.shared.ac_strategy,
      Rect(enc_state.shared.raw_quant_field));
  enc_state.used_orders.clear();
  enc_state.used_orders.resize(enc_state.progressive_splitter.GetNumPasses(),
                               used_orders_info.second);
  for (size_t i = 0; i < enc_state.progressive_splitter.GetNumPasses(); i++) {
    ComputeCoeffOrder(
        enc_state.cparams.speed_tier, *enc_state.coeffs[i],
        enc_state.shared.ac_strategy, frame_dim, enc_state.used_orders[i],
        used_orders_info.first,
        &enc_state.shared.coeff_orders[i * enc_state.shared.coeff_order_size]);
  }
}

// Working area for TokenizeCoefficients (per-group!)
struct EncCache {
  // Allocates memory when first called.
  void InitOnce() {
    if (num_nzeroes.xsize() == 0) {
      num_nzeroes = Image3I(kGroupDimInBlocks, kGroupDimInBlocks);
    }
  }
  // TokenizeCoefficients
  Image3I num_nzeroes;
};

Status TokenizeAllCoefficients(ThreadPool* pool,
                               PassesEncoderState* enc_state) {
  PassesSharedState& shared = enc_state->shared;
  std::vector<EncCache> group_caches;
  const auto tokenize_group_init = [&](const size_t num_threads) {
    group_caches.resize(num_threads);
    return true;
  };
  const auto tokenize_group = [&](const uint32_t group_index,
                                  const size_t thread) {
    // Tokenize coefficients.
    const Rect rect = shared.BlockGroupRect(group_index);
    for (size_t idx_pass = 0; idx_pass < enc_state->passes.size(); idx_pass++) {
      JXL_ASSERT(enc_state->coeffs[idx_pass]->Type() == ACType::k32);
      const int32_t* JXL_RESTRICT ac_rows[3] = {
          enc_state->coeffs[idx_pass]->PlaneRow(0, group_index, 0).ptr32,
          enc_state->coeffs[idx_pass]->PlaneRow(1, group_index, 0).ptr32,
          enc_state->coeffs[idx_pass]->PlaneRow(2, group_index, 0).ptr32,
      };
      // Ensure group cache is initialized.
      group_caches[thread].InitOnce();
      TokenizeCoefficients(
          &shared.coeff_orders[idx_pass * shared.coeff_order_size], rect,
          ac_rows, shared.ac_strategy, shared.frame_header.chroma_subsampling,
          &group_caches[thread].num_nzeroes,
          &enc_state->passes[idx_pass].ac_tokens[group_index], shared.quant_dc,
          shared.raw_quant_field, shared.block_ctx_map);
    }
  };
  return RunOnPool(pool, 0, shared.frame_dim.num_groups, tokenize_group_init,
                   tokenize_group, "TokenizeGroup");
}

Status EncodeGlobalDCInfo(const PassesSharedState& shared, BitWriter* writer,
                          AuxOut* aux_out) {
  // Encode quantizer DC and global scale.
  QuantizerParams params = shared.quantizer.GetParams();
  JXL_RETURN_IF_ERROR(
      WriteQuantizerParams(params, writer, kLayerQuant, aux_out));
  EncodeBlockCtxMap(shared.block_ctx_map, writer, aux_out);
  ColorCorrelationMapEncodeDC(shared.cmap, writer, kLayerDC, aux_out);
  return true;
}

Status EncodeGlobalACInfo(PassesEncoderState* enc_state, BitWriter* writer,
                          ModularFrameEncoder* modular_frame_encoder,
                          AuxOut* aux_out) {
  PassesSharedState& shared = enc_state->shared;
  JXL_RETURN_IF_ERROR(DequantMatricesEncode(
      shared.matrices, writer, kLayerQuant, aux_out, modular_frame_encoder));
  size_t num_histo_bits = CeilLog2Nonzero(shared.frame_dim.num_groups);
  if (num_histo_bits != 0) {
    BitWriter::Allotment allotment(writer, num_histo_bits);
    writer->Write(num_histo_bits, shared.num_histograms - 1);
    allotment.ReclaimAndCharge(writer, kLayerAC, aux_out);
  }

  for (size_t i = 0; i < enc_state->progressive_splitter.GetNumPasses(); i++) {
    // Encode coefficient orders.
    size_t order_bits = 0;
    JXL_RETURN_IF_ERROR(
        U32Coder::CanEncode(kOrderEnc, enc_state->used_orders[i], &order_bits));
    BitWriter::Allotment allotment(writer, order_bits);
    JXL_CHECK(U32Coder::Write(kOrderEnc, enc_state->used_orders[i], writer));
    allotment.ReclaimAndCharge(writer, kLayerOrder, aux_out);
    EncodeCoeffOrders(enc_state->used_orders[i],
                      &shared.coeff_orders[i * shared.coeff_order_size], writer,
                      kLayerOrder, aux_out);

    // Encode histograms.
    HistogramParams hist_params(enc_state->cparams.speed_tier,
                                shared.block_ctx_map.NumACContexts());
    if (enc_state->cparams.speed_tier > SpeedTier::kTortoise) {
      hist_params.lz77_method = HistogramParams::LZ77Method::kNone;
    }
    if (enc_state->cparams.decoding_speed_tier >= 1) {
      hist_params.max_histograms = 6;
    }
    BuildAndEncodeHistograms(
        hist_params,
        shared.num_histograms * shared.block_ctx_map.NumACContexts(),
        enc_state->passes[i].ac_tokens, &enc_state->passes[i].codes,
        &enc_state->passes[i].context_map, writer, kLayerAC, aux_out);
  }

  return true;
}

Status EncodeGroups(PassesEncoderState* enc_state,
                    ModularFrameEncoder* enc_modular, ThreadPool* pool,
                    std::vector<BitWriter>* group_codes, AuxOut* aux_out) {
  const PassesSharedState& shared = enc_state->shared;
  const FrameDimensions& frame_dim = shared.frame_dim;
  const FrameHeader& frame_header = shared.frame_header;
  const size_t num_groups = frame_dim.num_groups;
  const size_t num_passes = enc_state->progressive_splitter.GetNumPasses();
  const size_t global_ac_index = frame_dim.num_dc_groups + 1;
  const bool is_small_image = frame_dim.num_groups == 1 && num_passes == 1;

  group_codes->resize(
      NumTocEntries(num_groups, frame_dim.num_dc_groups, num_passes));

  const auto get_output = [&](const size_t index) {
    return &(*group_codes)[is_small_image ? 0 : index];
  };
  auto ac_group_code = [&](size_t pass, size_t group) {
    return get_output(AcGroupIndex(pass, group, frame_dim.num_groups,
                                   frame_dim.num_dc_groups));
  };

  if (frame_header.flags & FrameHeader::kPatches) {
    PatchDictionaryEncoder::Encode(shared.image_features.patches, get_output(0),
                                   kLayerDictionary, aux_out);
  }

  if (frame_header.flags & FrameHeader::kSplines) {
    EncodeSplines(shared.image_features.splines, get_output(0), kLayerSplines,
                  HistogramParams(), aux_out);
  }
  if (frame_header.flags & FrameHeader::kNoise) {
    EncodeNoise(shared.image_features.noise_params, get_output(0), kLayerNoise,
                aux_out);
  }

  JXL_RETURN_IF_ERROR(DequantMatricesEncodeDC(shared.matrices, get_output(0),
                                              kLayerQuant, aux_out));
  if (frame_header.encoding == FrameEncoding::kVarDCT) {
    JXL_RETURN_IF_ERROR(EncodeGlobalDCInfo(shared, get_output(0), aux_out));
  }
  JXL_RETURN_IF_ERROR(enc_modular->EncodeGlobalInfo(get_output(0), aux_out));
  JXL_RETURN_IF_ERROR(enc_modular->EncodeStream(
      get_output(0), aux_out, kLayerModularGlobal, ModularStreamId::Global()));

  std::vector<std::unique_ptr<AuxOut>> aux_outs;
  auto resize_aux_outs = [&aux_outs,
                          aux_out](const size_t num_threads) -> Status {
    if (aux_out == nullptr) {
      aux_outs.resize(num_threads);
    } else {
      while (aux_outs.size() > num_threads) {
        aux_out->Assimilate(*aux_outs.back());
        aux_outs.pop_back();
      }
      while (num_threads > aux_outs.size()) {
        aux_outs.emplace_back(jxl::make_unique<AuxOut>());
      }
    }
    return true;
  };

  const auto process_dc_group = [&](const uint32_t group_index,
                                    const size_t thread) {
    AuxOut* my_aux_out = aux_outs[thread].get();
    BitWriter* output = get_output(group_index + 1);
    if (frame_header.encoding == FrameEncoding::kVarDCT &&
        !(frame_header.flags & FrameHeader::kUseDcFrame)) {
      BitWriter::Allotment allotment(output, 2);
      output->Write(2, enc_modular->extra_dc_precision[group_index]);
      allotment.ReclaimAndCharge(output, kLayerDC, my_aux_out);
      JXL_CHECK(
          enc_modular->EncodeStream(output, my_aux_out, kLayerDC,
                                    ModularStreamId::VarDCTDC(group_index)));
    }
    JXL_CHECK(
        enc_modular->EncodeStream(output, my_aux_out, kLayerModularDcGroup,
                                  ModularStreamId::ModularDC(group_index)));
    if (frame_header.encoding == FrameEncoding::kVarDCT) {
      const Rect& rect = enc_state->shared.DCGroupRect(group_index);
      size_t nb_bits = CeilLog2Nonzero(rect.xsize() * rect.ysize());
      if (nb_bits != 0) {
        BitWriter::Allotment allotment(output, nb_bits);
        output->Write(nb_bits, enc_modular->ac_metadata_size[group_index] - 1);
        allotment.ReclaimAndCharge(output, kLayerControlFields, my_aux_out);
      }
      JXL_CHECK(
          enc_modular->EncodeStream(output, my_aux_out, kLayerControlFields,
                                    ModularStreamId::ACMetadata(group_index)));
    }
  };
  JXL_RETURN_IF_ERROR(RunOnPool(pool, 0, frame_dim.num_dc_groups,
                                resize_aux_outs, process_dc_group,
                                "EncodeDCGroup"));

  if (frame_header.encoding == FrameEncoding::kVarDCT) {
    JXL_RETURN_IF_ERROR(EncodeGlobalACInfo(
        enc_state, get_output(global_ac_index), enc_modular, aux_out));
  }

  std::atomic<int> num_errors{0};
  const auto process_group = [&](const uint32_t group_index,
                                 const size_t thread) {
    AuxOut* my_aux_out = aux_outs[thread].get();

    for (size_t i = 0; i < num_passes; i++) {
      if (frame_header.encoding == FrameEncoding::kVarDCT) {
        if (!EncodeGroupTokenizedCoefficients(
                group_index, i, enc_state->histogram_idx[group_index],
                *enc_state, ac_group_code(i, group_index), my_aux_out)) {
          num_errors.fetch_add(1, std::memory_order_relaxed);
          return;
        }
      }
      // Write all modular encoded data (color?, alpha, depth, extra channels)
      if (!enc_modular->EncodeStream(
              ac_group_code(i, group_index), my_aux_out, kLayerModularAcGroup,
              ModularStreamId::ModularAC(group_index, i))) {
        num_errors.fetch_add(1, std::memory_order_relaxed);
        return;
      }
    }
  };
  JXL_RETURN_IF_ERROR(RunOnPool(pool, 0, num_groups, resize_aux_outs,
                                process_group, "EncodeGroupCoefficients"));

  // Resizing aux_outs to 0 also Assimilates the array.
  static_cast<void>(resize_aux_outs(0));
  JXL_RETURN_IF_ERROR(num_errors.load(std::memory_order_relaxed) == 0);

  for (BitWriter& bw : *group_codes) {
    BitWriter::Allotment allotment(&bw, 8);
    bw.ZeroPadToByte();  // end of group.
    allotment.ReclaimAndCharge(&bw, kLayerAC, aux_out);
  }
  return true;
}

Status ComputeEncodingData(
    const CompressParams& cparams, const FrameInfo& frame_info,
    const CodecMetadata* metadata, JxlEncoderChunkedFrameAdapter& frame_data,
    size_t x0, size_t y0, size_t xsize, size_t ysize,
    const JxlCmsInterface& cms, ThreadPool* pool, PassesEncoderState& enc_state,
    std::vector<BitWriter>* group_codes, AuxOut* aux_out) {
  JXL_ASSERT(x0 + xsize <= frame_data.xsize);
  JXL_ASSERT(y0 + ysize <= frame_data.ysize);
  const size_t num_extra_channels = metadata->m.num_extra_channels;
  const ExtraChannelInfo* alpha_eci = metadata->m.Find(ExtraChannel::kAlpha);
  const ExtraChannelInfo* black_eci = metadata->m.Find(ExtraChannel::kBlack);
  const size_t alpha_idx = alpha_eci - metadata->m.extra_channel_info.data();
  const size_t black_idx = black_eci - metadata->m.extra_channel_info.data();
  const ColorEncoding c_enc = metadata->m.color_encoding;

  Image3F color;
  std::vector<ImageF> extra_channels(num_extra_channels);
  for (auto& extra_channel : extra_channels) {
    extra_channel = jxl::ImageF(xsize, ysize);
  }
  ImageF* alpha = alpha_eci ? &extra_channels[alpha_idx] : nullptr;
  ImageF* black = black_eci ? &extra_channels[black_idx] : nullptr;
  bool has_interleaved_alpha = false;
  JxlChunkedFrameInputSource input = frame_data.GetInputSource();
  std::unique_ptr<jpeg::JPEGData> jpeg_data;
  if (frame_data.IsJPEG()) {
    jpeg_data = make_unique<jpeg::JPEGData>(frame_data.TakeJPEGData());
  } else {
    JXL_RETURN_IF_ERROR(CopyColorChannels(input, x0, y0, xsize, ysize,
                                          frame_info, metadata->m, pool, &color,
                                          alpha, &has_interleaved_alpha));
  }
  JXL_RETURN_IF_ERROR(CopyExtraChannels(input, x0, y0, xsize, ysize, frame_info,
                                        metadata->m, has_interleaved_alpha,
                                        pool, &extra_channels));

  PassesSharedState& shared = enc_state.shared;
  shared.image_features.patches.SetPassesSharedState(&shared);
  enc_state.cparams = cparams;
  SetProgressiveMode(cparams, &enc_state.progressive_splitter);

  {
    FrameHeader frame_header(metadata);
    JXL_RETURN_IF_ERROR(
        MakeFrameHeader(xsize, ysize, cparams, enc_state.progressive_splitter,
                        frame_info, jpeg_data.get(), &frame_header));
    JXL_RETURN_IF_ERROR(InitializePassesSharedState(frame_header, &shared,
                                                    /*encoder=*/true));
  }
  FrameHeader& frame_header = shared.frame_header;
  const FrameDimensions& frame_dim = shared.frame_dim;
  ModularFrameEncoder modular_frame_encoder(frame_header, cparams);

  Image3F linear_storage;
  Image3F* linear = nullptr;

  Image3F opsin;
  if (!jpeg_data) {
    // Allocating a large enough image avoids a copy when padding.
    opsin = Image3F(RoundUpToBlockDim(xsize), RoundUpToBlockDim(ysize));
    opsin.ShrinkTo(xsize, ysize);
    if (frame_header.color_transform == ColorTransform::kXYB &&
        frame_info.ib_needs_color_transform) {
      if (frame_header.encoding == FrameEncoding::kVarDCT &&
          cparams.speed_tier <= SpeedTier::kKitten) {
        linear_storage = Image3F(xsize, ysize);
        linear = &linear_storage;
      }
      ToXYB(color, c_enc, metadata->m.IntensityTarget(), black, pool, &opsin,
            cms, linear);
    } else {  // RGB or YCbCr: don't do anything (forward YCbCr is not
              // implemented, this is only used when the input is already in
              // YCbCr)
              // If encoding a special DC or reference frame, don't do anything:
              // input is already in XYB.
      CopyImageTo(color, &opsin);
    }
    bool lossless = cparams.IsLossless();
    if (alpha && !alpha_eci->alpha_associated &&
        frame_header.frame_type == FrameType::kRegularFrame &&
        !ApplyOverride(cparams.keep_invisible, lossless) &&
        cparams.ec_resampling == cparams.resampling) {
      // simplify invisible pixels
      SimplifyInvisible(&opsin, *alpha, lossless);
      if (linear) {
        SimplifyInvisible(linear, *alpha, lossless);
      }
    }
    PadImageToBlockMultipleInPlace(&opsin);
  }

  ComputeChromacityAdjustments(cparams, !!jpeg_data, opsin, &frame_header);

  ComputeNoiseParams(cparams, !!jpeg_data, opsin, frame_dim, &frame_header,
                     &shared.image_features.noise_params);

  DownsampleColorChannels(cparams, frame_header, !!jpeg_data, &opsin);

  if (cparams.ec_resampling != 1 && !cparams.already_downsampled) {
    for (ImageF& ec : extra_channels) {
      DownsampleImage(&ec, cparams.ec_resampling);
    }
  }

  if (frame_header.encoding == FrameEncoding::kVarDCT) {
    enc_state.passes.resize(enc_state.progressive_splitter.GetNumPasses());
    for (PassesEncoderState::PassData& pass : enc_state.passes) {
      pass.ac_tokens.resize(shared.frame_dim.num_groups);
    }
    if (jpeg_data) {
      JXL_RETURN_IF_ERROR(ComputeJPEGTranscodingData(
          *jpeg_data, pool, &modular_frame_encoder, &enc_state));
    } else {
      JXL_RETURN_IF_ERROR(ComputeVarDCTEncodingData(linear, &opsin, cms, pool,
                                                    &modular_frame_encoder,
                                                    &enc_state, aux_out));
    }
    ComputeAllCoeffOrders(enc_state, frame_dim);
    shared.num_histograms = 1;
    JXL_RETURN_IF_ERROR(TokenizeAllCoefficients(pool, &enc_state));
  }

  JXL_RETURN_IF_ERROR(modular_frame_encoder.ComputeEncodingData(
      frame_header, metadata->m, &opsin, extra_channels, &enc_state, cms, pool,
      aux_out,
      /* do_color=*/frame_header.encoding == FrameEncoding::kModular));

  frame_header.UpdateFlag(shared.image_features.patches.HasAny(),
                          FrameHeader::kPatches);
  frame_header.UpdateFlag(shared.image_features.splines.HasAny(),
                          FrameHeader::kSplines);

  JXL_RETURN_IF_ERROR(EncodeGroups(&enc_state, &modular_frame_encoder, pool,
                                   group_codes, aux_out));
  return true;
}

Status PermuteGroups(const CompressParams& cparams,
                     const FrameDimensions& frame_dim, size_t num_passes,
                     std::vector<coeff_order_t>* permutation,
                     std::vector<BitWriter>* group_codes) {
  const size_t num_groups = frame_dim.num_groups;
  if (!cparams.centerfirst || (num_passes == 1 && num_groups == 1)) {
    return true;
  }
  // Don't permute global DC/AC or DC.
  permutation->resize(frame_dim.num_dc_groups + 2);
  std::iota(permutation->begin(), permutation->end(), 0);
  std::vector<coeff_order_t> ac_group_order(num_groups);
  std::iota(ac_group_order.begin(), ac_group_order.end(), 0);
  size_t group_dim = frame_dim.group_dim;

  // The center of the image is either given by parameters or chosen
  // to be the middle of the image by default if center_x, center_y resp.
  // are not provided.

  int64_t imag_cx;
  if (cparams.center_x != static_cast<size_t>(-1)) {
    JXL_RETURN_IF_ERROR(cparams.center_x < frame_dim.xsize);
    imag_cx = cparams.center_x;
  } else {
    imag_cx = frame_dim.xsize / 2;
  }

  int64_t imag_cy;
  if (cparams.center_y != static_cast<size_t>(-1)) {
    JXL_RETURN_IF_ERROR(cparams.center_y < frame_dim.ysize);
    imag_cy = cparams.center_y;
  } else {
    imag_cy = frame_dim.ysize / 2;
  }

  // The center of the group containing the center of the image.
  int64_t cx = (imag_cx / group_dim) * group_dim + group_dim / 2;
  int64_t cy = (imag_cy / group_dim) * group_dim + group_dim / 2;
  // This identifies in what area of the central group the center of the image
  // lies in.
  double direction = -std::atan2(imag_cy - cy, imag_cx - cx);
  // This identifies the side of the central group the center of the image
  // lies closest to. This can take values 0, 1, 2, 3 corresponding to left,
  // bottom, right, top.
  int64_t side = std::fmod((direction + 5 * kPi / 4), 2 * kPi) * 2 / kPi;
  auto get_distance_from_center = [&](size_t gid) {
    Rect r = frame_dim.GroupRect(gid);
    int64_t gcx = r.x0() + group_dim / 2;
    int64_t gcy = r.y0() + group_dim / 2;
    int64_t dx = gcx - cx;
    int64_t dy = gcy - cy;
    // The angle is determined by taking atan2 and adding an appropriate
    // starting point depending on the side we want to start on.
    double angle = std::remainder(
        std::atan2(dy, dx) + kPi / 4 + side * (kPi / 2), 2 * kPi);
    // Concentric squares in clockwise order.
    return std::make_pair(std::max(std::abs(dx), std::abs(dy)), angle);
  };
  std::sort(ac_group_order.begin(), ac_group_order.end(),
            [&](coeff_order_t a, coeff_order_t b) {
              return get_distance_from_center(a) < get_distance_from_center(b);
            });
  std::vector<coeff_order_t> inv_ac_group_order(ac_group_order.size(), 0);
  for (size_t i = 0; i < ac_group_order.size(); i++) {
    inv_ac_group_order[ac_group_order[i]] = i;
  }
  for (size_t i = 0; i < num_passes; i++) {
    size_t pass_start = permutation->size();
    for (coeff_order_t v : inv_ac_group_order) {
      permutation->push_back(pass_start + v);
    }
  }
  std::vector<BitWriter> new_group_codes(group_codes->size());
  for (size_t i = 0; i < permutation->size(); i++) {
    new_group_codes[(*permutation)[i]] = std::move((*group_codes)[i]);
  }
  *group_codes = std::move(new_group_codes);
  return true;
}

bool CanDoStreamingEncoding(const CompressParams& cparams,
                            const FrameInfo& frame_info,
                            const CodecMetadata& metadata,
                            const JxlEncoderChunkedFrameAdapter& frame_data) {
  if (frame_data.IsJPEG()) {
    return false;
  }
  if (cparams.noise == Override::kOn || cparams.patches == Override::kOn) {
    return false;
  }
  if (cparams.progressive_dc != 0 || frame_info.dc_level != 0) {
    return false;
  }
  if (cparams.resampling != 1 || cparams.ec_resampling != 1) {
    return false;
  }
  if (cparams.max_error_mode) {
    return false;
  }
  if (cparams.color_transform != ColorTransform::kXYB) {
    return false;
  }
  if (cparams.modular_mode) {
    return false;
  }
  if (metadata.m.num_extra_channels > 0) {
    return false;
  }
  if (cparams.buffering == 0) {
    return false;
  }
  if (cparams.buffering == 1 && frame_data.xsize <= 2048 &&
      frame_data.ysize <= 2048) {
    return false;
  }
  if (frame_data.xsize <= 256 && frame_data.ysize <= 256) {
    return false;
  }
  return true;
}

Status EncodeFrameStreaming(const CompressParams& cparams,
                            const FrameInfo& frame_info,
                            const CodecMetadata* metadata,
                            JxlEncoderChunkedFrameAdapter& frame_data,
                            const JxlCmsInterface& cms, ThreadPool* pool,
                            JxlEncoderOutputProcessorWrapper* output_processor,
                            AuxOut* aux_out) {
  // TODO(szabadka): Call ComputeEncodingData for one DC group at a time and
  // write the output at a precomputed position.
  PassesEncoderState enc_state;
  std::vector<BitWriter> group_codes;
  JXL_RETURN_IF_ERROR(ComputeEncodingData(
      cparams, frame_info, metadata, frame_data, 0, 0, frame_data.xsize,
      frame_data.ysize, cms, pool, enc_state, &group_codes, aux_out));

  BitWriter writer;
  writer.AppendByteAligned(enc_state.special_frames);
  JXL_RETURN_IF_ERROR(
      WriteFrameHeader(enc_state.shared.frame_header, &writer, aux_out));

  const size_t num_passes = enc_state.progressive_splitter.GetNumPasses();
  std::vector<coeff_order_t> permutation;
  JXL_RETURN_IF_ERROR(PermuteGroups(cparams, enc_state.shared.frame_dim,
                                    num_passes, &permutation, &group_codes));

  JXL_RETURN_IF_ERROR(
      WriteGroupOffsets(group_codes, permutation, &writer, aux_out));

  writer.AppendByteAligned(group_codes);
  PaddedBytes frame_bytes = std::move(writer).TakeBytes();
  JXL_RETURN_IF_ERROR(AppendData(*output_processor, frame_bytes));

  return true;
}

Status EncodeFrameOneShot(const CompressParams& cparams,
                          const FrameInfo& frame_info,
                          const CodecMetadata* metadata,
                          JxlEncoderChunkedFrameAdapter& frame_data,
                          const JxlCmsInterface& cms, ThreadPool* pool,
                          JxlEncoderOutputProcessorWrapper* output_processor,
                          AuxOut* aux_out) {
  PassesEncoderState enc_state;
  std::vector<BitWriter> group_codes;
  JXL_RETURN_IF_ERROR(ComputeEncodingData(
      cparams, frame_info, metadata, frame_data, 0, 0, frame_data.xsize,
      frame_data.ysize, cms, pool, enc_state, &group_codes, aux_out));

  BitWriter writer;
  writer.AppendByteAligned(enc_state.special_frames);
  JXL_RETURN_IF_ERROR(
      WriteFrameHeader(enc_state.shared.frame_header, &writer, aux_out));

  const size_t num_passes = enc_state.progressive_splitter.GetNumPasses();
  std::vector<coeff_order_t> permutation;
  JXL_RETURN_IF_ERROR(PermuteGroups(cparams, enc_state.shared.frame_dim,
                                    num_passes, &permutation, &group_codes));

  JXL_RETURN_IF_ERROR(
      WriteGroupOffsets(group_codes, permutation, &writer, aux_out));

  writer.AppendByteAligned(group_codes);
  PaddedBytes frame_bytes = std::move(writer).TakeBytes();
  JXL_RETURN_IF_ERROR(AppendData(*output_processor, frame_bytes));

  return true;
}

}  // namespace

Status EncodeFrame(const CompressParams& cparams_orig,
                   const FrameInfo& frame_info, const CodecMetadata* metadata,
                   JxlEncoderChunkedFrameAdapter& frame_data,
                   const JxlCmsInterface& cms, ThreadPool* pool,
                   JxlEncoderOutputProcessorWrapper* output_processor,
                   AuxOut* aux_out) {
  CompressParams cparams = cparams_orig;
  if (cparams.speed_tier == SpeedTier::kGlacier && !cparams.IsLossless()) {
    cparams.speed_tier = SpeedTier::kTortoise;
  }
  if (cparams.speed_tier == SpeedTier::kGlacier) {
    std::vector<CompressParams> all_params;
    std::vector<size_t> size;

    CompressParams cparams_attempt = cparams_orig;
    cparams_attempt.speed_tier = SpeedTier::kTortoise;
    cparams_attempt.options.max_properties = 4;

    for (float x : {0.0f, 80.f}) {
      cparams_attempt.channel_colors_percent = x;
      for (float y : {0.0f, 95.0f}) {
        cparams_attempt.channel_colors_pre_transform_percent = y;
        // 70000 ensures that the number of palette colors is representable in
        // modular headers.
        for (int K : {0, 1 << 10, 70000}) {
          cparams_attempt.palette_colors = K;
          for (int tree_mode : {-1, (int)ModularOptions::TreeMode::kNoWP,
                                (int)ModularOptions::TreeMode::kDefault}) {
            if (tree_mode == -1) {
              // LZ77 only
              cparams_attempt.options.nb_repeats = 0;
            } else {
              cparams_attempt.options.nb_repeats = 1;
              cparams_attempt.options.wp_tree_mode =
                  static_cast<ModularOptions::TreeMode>(tree_mode);
            }
            for (Predictor pred : {Predictor::Zero, Predictor::Variable}) {
              cparams_attempt.options.predictor = pred;
              for (int g : {0, -1, 3}) {
                cparams_attempt.modular_group_size_shift = g;
                for (Override patches : {Override::kDefault, Override::kOff}) {
                  cparams_attempt.patches = patches;
                  all_params.push_back(cparams_attempt);
                }
              }
            }
          }
        }
      }
    }

    size.resize(all_params.size());

    std::atomic<int> num_errors{0};

    JXL_RETURN_IF_ERROR(RunOnPool(
        pool, 0, all_params.size(), ThreadPool::NoInit,
        [&](size_t task, size_t) {
          std::vector<uint8_t> output(64);
          uint8_t* next_out = output.data();
          size_t avail_out = output.size();
          JxlEncoderOutputProcessorWrapper local_output;
          local_output.SetAvailOut(&next_out, &avail_out);
          if (!EncodeFrame(all_params[task], frame_info, metadata, frame_data,
                           cms, nullptr, &local_output, aux_out)) {
            num_errors.fetch_add(1, std::memory_order_relaxed);
            return;
          }
          size[task] = local_output.CurrentPosition();
        },
        "Compress kGlacier"));
    JXL_RETURN_IF_ERROR(num_errors.load(std::memory_order_relaxed) == 0);

    size_t best_idx = 0;
    for (size_t i = 1; i < all_params.size(); i++) {
      if (size[best_idx] > size[i]) {
        best_idx = i;
      }
    }
    cparams = all_params[best_idx];
  }

  JXL_RETURN_IF_ERROR(ParamsPostInit(&cparams));

  if (cparams.butteraugli_distance < 0) {
    return JXL_FAILURE("Expected non-negative distance");
  }

  if (cparams.progressive_dc < 0) {
    if (cparams.progressive_dc != -1) {
      return JXL_FAILURE("Invalid progressive DC setting value (%d)",
                         cparams.progressive_dc);
    }
    cparams.progressive_dc = 0;
  }
  if (cparams.ec_resampling < cparams.resampling) {
    cparams.ec_resampling = cparams.resampling;
  }
  if (cparams.resampling > 1 || frame_info.is_preview) {
    cparams.progressive_dc = 0;
  }

  if (frame_info.dc_level + cparams.progressive_dc > 4) {
    return JXL_FAILURE("Too many levels of progressive DC");
  }

  if (cparams.butteraugli_distance != 0 &&
      cparams.butteraugli_distance < kMinButteraugliDistance) {
    return JXL_FAILURE("Butteraugli distance is too low (%f)",
                       cparams.butteraugli_distance);
  }

  if (frame_data.IsJPEG()) {
    cparams.gaborish = Override::kOff;
    cparams.epf = 0;
    cparams.modular_mode = false;
  }

  if (frame_data.xsize == 0 || frame_data.ysize == 0) {
    return JXL_FAILURE("Empty image");
  }

  // Assert that this metadata is correctly set up for the compression params,
  // this should have been done by enc_file.cc
  JXL_ASSERT(metadata->m.xyb_encoded ==
             (cparams.color_transform == ColorTransform::kXYB));

  if (frame_data.IsJPEG() && cparams.color_transform == ColorTransform::kXYB) {
    return JXL_FAILURE("Can't add JPEG frame to XYB codestream");
  }

  if (CanDoStreamingEncoding(cparams, frame_info, *metadata, frame_data)) {
    return EncodeFrameStreaming(cparams, frame_info, metadata, frame_data, cms,
                                pool, output_processor, aux_out);
  } else {
    return EncodeFrameOneShot(cparams, frame_info, metadata, frame_data, cms,
                              pool, output_processor, aux_out);
  }
}

Status EncodeFrame(const CompressParams& cparams_orig,
                   const FrameInfo& frame_info, const CodecMetadata* metadata,
                   const ImageBundle& ib, const JxlCmsInterface& cms,
                   ThreadPool* pool, BitWriter* writer, AuxOut* aux_out) {
  JxlEncoderChunkedFrameAdapter frame_data(ib.xsize(), ib.ysize(),
                                           ib.extra_channels().size());
  std::vector<uint8_t> color;
  if (ib.IsJPEG()) {
    frame_data.SetJPEGData(*ib.jpeg_data);
  } else {
    uint32_t num_channels =
        ib.IsGray() && frame_info.ib_needs_color_transform ? 1 : 3;
    size_t stride = ib.xsize() * num_channels * 4;
    color.resize(ib.ysize() * stride);
    JXL_RETURN_IF_ERROR(ConvertToExternal(
        ib, /*bites_per_sample=*/32, /*float_out=*/true, num_channels,
        JXL_NATIVE_ENDIAN, stride, pool, color.data(), color.size(),
        /*out_callback=*/{}, Orientation::kIdentity));
    JxlPixelFormat format{num_channels, JXL_TYPE_FLOAT, JXL_NATIVE_ENDIAN, 0};
    frame_data.SetFromBuffer(0, color.data(), color.size(), format);
  }
  for (size_t ec = 0; ec < ib.extra_channels().size(); ++ec) {
    JxlPixelFormat ec_format{1, JXL_TYPE_FLOAT, JXL_NATIVE_ENDIAN, 0};
    size_t ec_stride = ib.xsize() * 4;
    std::vector<uint8_t> ec_data(ib.ysize() * ec_stride);
    const ImageF* channel = &ib.extra_channels()[ec];
    JXL_RETURN_IF_ERROR(ConvertChannelsToExternal(
        &channel, 1,
        /*bites_per_sample=*/32,
        /*float_out=*/true, JXL_NATIVE_ENDIAN, ec_stride, pool, ec_data.data(),
        ec_data.size(), /*out_callback=*/{}, Orientation::kIdentity));
    frame_data.SetFromBuffer(1 + ec, ec_data.data(), ec_data.size(), ec_format);
  }
  FrameInfo fi = frame_info;
  fi.origin = ib.origin;
  fi.blend = ib.blend;
  fi.blendmode = ib.blendmode;
  fi.duration = ib.duration;
  fi.timecode = ib.timecode;
  fi.name = ib.name;
  std::vector<uint8_t> output(64);
  uint8_t* next_out = output.data();
  size_t avail_out = output.size();
  JxlEncoderOutputProcessorWrapper output_processor;
  output_processor.SetAvailOut(&next_out, &avail_out);
  JXL_RETURN_IF_ERROR(EncodeFrame(cparams_orig, fi, metadata, frame_data, cms,
                                  pool, &output_processor, aux_out));
  output_processor.SetFinalizedPosition();
  output_processor.CopyOutput(output, next_out, avail_out);
  writer->AppendByteAligned(Bytes(output));
  return true;
}

}  // namespace jxl
