#ifndef _PICTURE_X86_ASM_SAD_H_
#define _PICTURE_X86_ASM_SAD_H_
/*****************************************************************************
 * This file is part of Kvazaar HEVC encoder.
 *
 * Copyright (c) 2021, Tampere University, ITU/ISO/IEC, project contributors
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

/**
 * \ingroup Optimization
 * \file
 * Optimizations for AVX, utilizing ASM implementations.
 */

#include "../global.h" // IWYU pragma: keep
#include "../kvazaar.h"

#if KVZ_BIT_DEPTH == 8
unsigned kvz_sad_4x4_avx(const uint8_t*, const uint8_t*);
unsigned kvz_sad_8x8_avx(const uint8_t*, const uint8_t*);
unsigned kvz_sad_16x16_avx(const uint8_t*, const uint8_t*);

unsigned kvz_sad_4x4_stride_avx(const uint8_t *data1, const uint8_t *data2, unsigned stride);
unsigned kvz_sad_8x8_stride_avx(const uint8_t *data1, const uint8_t *data2, unsigned stride);
unsigned kvz_sad_16x16_stride_avx(const uint8_t *data1, const uint8_t *data2, unsigned stride);
unsigned kvz_sad_32x32_stride_avx(const uint8_t *data1, const uint8_t *data2, unsigned stride);
unsigned kvz_sad_64x64_stride_avx(const uint8_t *data1, const uint8_t *data2, unsigned stride);
#endif // KVZ_BIT_DEPTH == 8

#endif
