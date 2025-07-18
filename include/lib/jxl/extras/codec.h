// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef LIB_EXTRAS_CODEC_H_
#define LIB_EXTRAS_CODEC_H_

// Facade for image encoders/decoders (PNG, PNM, ...).

#include <cstdint>
#include <string>
#include <vector>

#include "lib/jxl/extras/codec_in_out.h"
#include "lib/jxl/extras/dec/color_hints.h"
#include "lib/jxl/extras/dec/decode.h"
#include "lib/jxl/extras/packed_image.h"
#include "lib/jxl/base/compiler_specific.h"
#include "lib/jxl/base/data_parallel.h"
#include "lib/jxl/base/span.h"
#include "lib/jxl/base/status.h"

namespace jxl {

struct SizeConstraints;

// Decodes "bytes" and sets io->metadata.m.
// color_space_hint may specify the color space, otherwise, defaults to sRGB.
Status SetFromBytes(Span<const uint8_t> bytes,
                    const extras::ColorHints& color_hints, CodecInOut* io,
                    ThreadPool* pool = nullptr,
                    const SizeConstraints* constraints = nullptr,
                    extras::Codec* orig_codec = nullptr);
// Helper function to use no color_space_hint.
JXL_INLINE Status SetFromBytes(const Span<const uint8_t> bytes, CodecInOut* io,
                               ThreadPool* pool = nullptr,
                               const SizeConstraints* constraints = nullptr,
                               extras::Codec* orig_codec = nullptr) {
  return SetFromBytes(bytes, extras::ColorHints(), io, pool, constraints,
                      orig_codec);
}

Status Encode(const extras::PackedPixelFile& ppf, extras::Codec codec,
              std::vector<uint8_t>* bytes, ThreadPool* pool);

Status Encode(const extras::PackedPixelFile& ppf, const std::string& pathname,
              std::vector<uint8_t>* bytes, ThreadPool* pool = nullptr);

}  // namespace jxl

#endif  // LIB_EXTRAS_CODEC_H_
