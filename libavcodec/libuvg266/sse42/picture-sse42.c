/*****************************************************************************
 * This file is part of uvg266 VVC encoder.
 *
 * Copyright (c) 2023, Tampere University, ITU/ISO/IEC, project contributors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of the Tampere University or ITU/ISO/IEC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS
 ****************************************************************************/

#include "../global.h"

#if COMPILE_INTEL_SSE42
#include "../uvg266.h"

#include "picture-sse42.h"

#include <immintrin.h>
#include <stdlib.h>

#include "../strategyselector.h"



static uint32_t uvg_crc32c_4x4_8bit_sse42(const uvg_pixel *buf, uint32_t pic_stride)
{
  uint32_t crc = 0xFFFFFFFF;
  crc = _mm_crc32_u32(crc, *((uint32_t *)&buf[0 * pic_stride]));
  crc = _mm_crc32_u32(crc, *((uint32_t *)&buf[1 * pic_stride]));
  crc = _mm_crc32_u32(crc, *((uint32_t *)&buf[2 * pic_stride]));
  crc = _mm_crc32_u32(crc, *((uint32_t *)&buf[3 * pic_stride]));
  return crc ^ 0xFFFFFFFF;
}

static uint32_t uvg_crc32c_4x4_16bit_sse42(const uvg_pixel *buf, uint32_t pic_stride)
{
  uint64_t crc = 0xFFFFFFFF;
  crc = _mm_crc32_u64(crc, *((uint64_t *)&buf[0 * pic_stride]));
  crc = _mm_crc32_u64(crc, *((uint64_t *)&buf[1 * pic_stride]));
  crc = _mm_crc32_u64(crc, *((uint64_t *)&buf[2 * pic_stride]));
  crc = _mm_crc32_u64(crc, *((uint64_t *)&buf[3 * pic_stride]));
  return (uint32_t)(crc ^ 0xFFFFFFFF);
}

static uint32_t uvg_crc32c_8x8_8bit_sse42(const uvg_pixel *buf, uint32_t pic_stride)
{
  uint64_t crc = 0xFFFFFFFF;
  for (int i = 0; i < 8; i++) {
    crc = _mm_crc32_u64(crc, *((uint64_t *)&buf[i * pic_stride]));
  }
  return (uint32_t)(crc ^ 0xFFFFFFFF);
}

static uint32_t uvg_crc32c_8x8_16bit_sse42(const uvg_pixel *buf, uint32_t pic_stride)
{
  uint64_t crc = 0xFFFFFFFF;
  for (int i = 0; i < 8; i++) {
    crc = _mm_crc32_u64(crc, *((uint64_t *)&buf[i * pic_stride]));
    crc = _mm_crc32_u64(crc, *((uint64_t *)&buf[i * pic_stride + 4]));
  }

  return (uint32_t)(crc ^ 0xFFFFFFFF);
}


#endif //COMPILE_INTEL_SSE42

int uvg_strategy_register_picture_sse42(void* opaque, uint8_t bitdepth) {
  bool success = true;
#if COMPILE_INTEL_SSE42
  if (bitdepth == 8){
    success &= uvg_strategyselector_register(opaque, "crc32c_4x4", "sse42", 0, &uvg_crc32c_4x4_8bit_sse42);
    success &= uvg_strategyselector_register(opaque, "crc32c_8x8", "sse42", 0, &uvg_crc32c_8x8_8bit_sse42);
  } else {
    success &= uvg_strategyselector_register(opaque, "crc32c_4x4", "sse42", 0, &uvg_crc32c_4x4_16bit_sse42);
    success &= uvg_strategyselector_register(opaque, "crc32c_8x8", "sse42", 0, &uvg_crc32c_8x8_16bit_sse42);
  }
#endif
  return success;
}
