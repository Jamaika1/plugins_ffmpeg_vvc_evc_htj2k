/*
 * Copyright © 2024, VideoLAN and dav1d authors
 * Copyright © 2024, Two Orioles, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <string.h>

#include "src/ctx.h"

static void memset_w1(void *const ptr, const int value) {
    set_ctx1((uint8_t *) ptr, 0, value);
}

static void memset_w2(void *const ptr, const int value) {
    set_ctx2((uint8_t *) ptr, 0, value);
}

static void memset_w4(void *const ptr, const int value) {
    set_ctx4((uint8_t *) ptr, 0, value);
}

static void memset_w8(void *const ptr, const int value) {
    set_ctx8((uint8_t *) ptr, 0, value);
}

static void memset_w16(void *const ptr, const int value) {
    set_ctx16((uint8_t *) ptr, 0, value);
}

static void memset_w32(void *const ptr, const int value) {
    set_ctx32((uint8_t *) ptr, 0, value);
}

const dav1d_memset_pow2_fn dav1d_memset_pow2[6] = {
    memset_w1,
    memset_w2,
    memset_w4,
    memset_w8,
    memset_w16,
    memset_w32
};
