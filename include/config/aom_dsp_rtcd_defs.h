##
## Copyright (c) 2017, Alliance for Open Media. All rights reserved.
##
## This source code is subject to the terms of the BSD 2 Clause License and
## the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
## was not distributed with this source code in the LICENSE file, you can
## obtain it at www.aomedia.org/license/software. If the Alliance for Open
## Media Patent License 1.0 was not distributed with this source code in the
## PATENTS file, you can obtain it at www.aomedia.org/license/patent.
##
sub aom_dsp_forward_decls() {
print <<EOF
/*
 * DSP
 */

#include "aom/aom_integer.h"
#include "aom_dsp/aom_dsp_common.h"
#include "av1/common/blockd.h"
#include "av1/common/enums.h"

EOF
}
forward_decls qw/aom_dsp_forward_decls/;

# optimizations which depend on multiple features
$avx2_ssse3 = '';
if ((aom_config("HAVE_AVX2") eq "yes") && (aom_config("HAVE_SSSE3") eq "yes")) {
  $avx2_ssse3 = 'avx2';
}

# functions that are 64 bit only.
$mmx_x86_64 = $sse2_x86_64 = $ssse3_x86_64 = $avx_x86_64 = $avx2_x86_64 = '';
if ($opts{arch} eq "x86_64") {
  $mmx_x86_64 = 'mmx';
  $sse2_x86_64 = 'sse2';
  $ssse3_x86_64 = 'ssse3';
  $avx_x86_64 = 'avx';
  $avx2_x86_64 = 'avx2';
}

@block_widths = (4, 8, 16, 32, 64, 128);

@encoder_block_sizes = ();
foreach $w (@block_widths) {
  foreach $h (@block_widths) {
    push @encoder_block_sizes, [$w, $h] if ($w <= 2*$h && $h <= 2*$w);
  }
}

if (aom_config("CONFIG_REALTIME_ONLY") ne "yes") {
  push @encoder_block_sizes, [4, 16];
  push @encoder_block_sizes, [16, 4];
  push @encoder_block_sizes, [8, 32];
  push @encoder_block_sizes, [32, 8];
  push @encoder_block_sizes, [16, 64];
  push @encoder_block_sizes, [64, 16];
}

@tx_dims = (4, 8, 16, 32, 64);
@tx_sizes = ();
foreach $w (@tx_dims) {
  push @tx_sizes, [$w, $w];
  foreach $h (@tx_dims) {
    push @tx_sizes, [$w, $h] if ($w >=4 && $h >=4 && ($w == 2*$h || $h == 2*$w));
    push @tx_sizes, [$w, $h] if ($w >=4 && $h >=4 && ($w == 4*$h || $h == 4*$w));
  }
}

@pred_names = qw/dc dc_top dc_left dc_128 v h paeth smooth smooth_v smooth_h/;

#
# Intra prediction
#

foreach (@tx_sizes) {
  ($w, $h) = @$_;
  foreach $pred_name (@pred_names) {
    add_proto "void", "aom_${pred_name}_predictor_${w}x${h}",
              "uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left";
    if (aom_config("CONFIG_AV1_HIGHBITDEPTH") eq "yes") {
        add_proto "void", "aom_highbd_${pred_name}_predictor_${w}x${h}",
                  "uint16_t *dst, ptrdiff_t y_stride, const uint16_t *above, const uint16_t *left, int bd";
    }
  }
}

specialize qw/aom_dc_top_predictor_4x4 neon sse2/;
specialize qw/aom_dc_top_predictor_4x8 neon sse2/;
specialize qw/aom_dc_top_predictor_4x16 neon sse2/;
specialize qw/aom_dc_top_predictor_8x4 neon sse2/;
specialize qw/aom_dc_top_predictor_8x8 neon sse2/;
specialize qw/aom_dc_top_predictor_8x16 neon sse2/;
specialize qw/aom_dc_top_predictor_8x32 neon sse2/;
specialize qw/aom_dc_top_predictor_16x4 neon sse2/;
specialize qw/aom_dc_top_predictor_16x8 neon sse2/;
specialize qw/aom_dc_top_predictor_16x16 neon sse2/;
specialize qw/aom_dc_top_predictor_16x32 neon sse2/;
specialize qw/aom_dc_top_predictor_16x64 neon sse2/;
specialize qw/aom_dc_top_predictor_32x8 neon sse2/;
specialize qw/aom_dc_top_predictor_32x16 neon sse2 avx2/;
specialize qw/aom_dc_top_predictor_32x32 neon sse2 avx2/;
specialize qw/aom_dc_top_predictor_32x64 neon sse2 avx2/;
specialize qw/aom_dc_top_predictor_64x16 neon sse2 avx2/;
specialize qw/aom_dc_top_predictor_64x32 neon sse2 avx2/;
specialize qw/aom_dc_top_predictor_64x64 neon sse2 avx2/;

specialize qw/aom_dc_left_predictor_4x4 neon sse2/;
specialize qw/aom_dc_left_predictor_4x8 neon sse2/;
specialize qw/aom_dc_left_predictor_4x16 neon sse2/;
specialize qw/aom_dc_left_predictor_8x4 neon sse2/;
specialize qw/aom_dc_left_predictor_8x8 neon sse2/;
specialize qw/aom_dc_left_predictor_8x16 neon sse2/;
specialize qw/aom_dc_left_predictor_8x32 neon sse2/;
specialize qw/aom_dc_left_predictor_16x4 neon sse2/;
specialize qw/aom_dc_left_predictor_16x8 neon sse2/;
specialize qw/aom_dc_left_predictor_16x16 neon sse2/;
specialize qw/aom_dc_left_predictor_16x32 neon sse2/;
specialize qw/aom_dc_left_predictor_16x64 neon sse2/;
specialize qw/aom_dc_left_predictor_32x8 neon sse2/;
specialize qw/aom_dc_left_predictor_32x16 neon sse2 avx2/;
specialize qw/aom_dc_left_predictor_32x32 neon sse2 avx2/;
specialize qw/aom_dc_left_predictor_32x64 neon sse2 avx2/;
specialize qw/aom_dc_left_predictor_64x16 neon sse2 avx2/;
specialize qw/aom_dc_left_predictor_64x32 neon sse2 avx2/;
specialize qw/aom_dc_left_predictor_64x64 neon sse2 avx2/;

specialize qw/aom_dc_128_predictor_4x4 neon sse2/;
specialize qw/aom_dc_128_predictor_4x8 neon sse2/;
specialize qw/aom_dc_128_predictor_4x16 neon sse2/;
specialize qw/aom_dc_128_predictor_8x4 neon sse2/;
specialize qw/aom_dc_128_predictor_8x8 neon sse2/;
specialize qw/aom_dc_128_predictor_8x16 neon sse2/;
specialize qw/aom_dc_128_predictor_8x32 neon sse2/;
specialize qw/aom_dc_128_predictor_16x4 neon sse2/;
specialize qw/aom_dc_128_predictor_16x8 neon sse2/;
specialize qw/aom_dc_128_predictor_16x16 neon sse2/;
specialize qw/aom_dc_128_predictor_16x32 neon sse2/;
specialize qw/aom_dc_128_predictor_16x64 neon sse2/;
specialize qw/aom_dc_128_predictor_32x8 neon sse2/;
specialize qw/aom_dc_128_predictor_32x16 neon sse2 avx2/;
specialize qw/aom_dc_128_predictor_32x32 neon sse2 avx2/;
specialize qw/aom_dc_128_predictor_32x64 neon sse2 avx2/;
specialize qw/aom_dc_128_predictor_64x16 neon sse2 avx2/;
specialize qw/aom_dc_128_predictor_64x32 neon sse2 avx2/;
specialize qw/aom_dc_128_predictor_64x64 neon sse2 avx2/;

specialize qw/aom_v_predictor_4x4 neon sse2/;
specialize qw/aom_v_predictor_4x8 neon sse2/;
specialize qw/aom_v_predictor_4x16 neon sse2/;
specialize qw/aom_v_predictor_8x4 neon sse2/;
specialize qw/aom_v_predictor_8x8 neon sse2/;
specialize qw/aom_v_predictor_8x16 neon sse2/;
specialize qw/aom_v_predictor_8x32 neon sse2/;
specialize qw/aom_v_predictor_16x4 neon sse2/;
specialize qw/aom_v_predictor_16x8 neon sse2/;
specialize qw/aom_v_predictor_16x16 neon sse2/;
specialize qw/aom_v_predictor_16x32 neon sse2/;
specialize qw/aom_v_predictor_16x64 neon sse2/;
specialize qw/aom_v_predictor_32x8 neon sse2/;
specialize qw/aom_v_predictor_32x16 neon sse2 avx2/;
specialize qw/aom_v_predictor_32x32 neon sse2 avx2/;
specialize qw/aom_v_predictor_32x64 neon sse2 avx2/;
specialize qw/aom_v_predictor_64x16 neon sse2 avx2/;
specialize qw/aom_v_predictor_64x32 neon sse2 avx2/;
specialize qw/aom_v_predictor_64x64 neon sse2 avx2/;

specialize qw/aom_h_predictor_4x4 neon sse2/;
specialize qw/aom_h_predictor_4x8 neon sse2/;
specialize qw/aom_h_predictor_4x16 neon sse2/;
specialize qw/aom_h_predictor_8x4 neon sse2/;
specialize qw/aom_h_predictor_8x8 neon sse2/;
specialize qw/aom_h_predictor_8x16 neon sse2/;
specialize qw/aom_h_predictor_8x32 neon sse2/;
specialize qw/aom_h_predictor_16x4 neon sse2/;
specialize qw/aom_h_predictor_16x8 neon sse2/;
specialize qw/aom_h_predictor_16x16 neon sse2/;
specialize qw/aom_h_predictor_16x32 neon sse2/;
specialize qw/aom_h_predictor_16x64 neon sse2/;
specialize qw/aom_h_predictor_32x8 neon sse2/;
specialize qw/aom_h_predictor_32x16 neon sse2/;
specialize qw/aom_h_predictor_32x32 neon sse2 avx2/;
specialize qw/aom_h_predictor_32x64 neon sse2/;
specialize qw/aom_h_predictor_64x16 neon sse2/;
specialize qw/aom_h_predictor_64x32 neon sse2/;
specialize qw/aom_h_predictor_64x64 neon sse2/;

specialize qw/aom_paeth_predictor_4x4 ssse3 neon/;
specialize qw/aom_paeth_predictor_4x8 ssse3 neon/;
specialize qw/aom_paeth_predictor_4x16 ssse3 neon/;
specialize qw/aom_paeth_predictor_8x4 ssse3 neon/;
specialize qw/aom_paeth_predictor_8x8 ssse3 neon/;
specialize qw/aom_paeth_predictor_8x16 ssse3 neon/;
specialize qw/aom_paeth_predictor_8x32 ssse3 neon/;
specialize qw/aom_paeth_predictor_16x4 ssse3 neon/;
specialize qw/aom_paeth_predictor_16x8 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_16x16 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_16x32 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_16x64 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_32x8 ssse3 neon/;
specialize qw/aom_paeth_predictor_32x16 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_32x32 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_32x64 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_64x16 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_64x32 ssse3 avx2 neon/;
specialize qw/aom_paeth_predictor_64x64 ssse3 avx2 neon/;

specialize qw/aom_smooth_predictor_4x4 neon ssse3/;
specialize qw/aom_smooth_predictor_4x8 neon ssse3/;
specialize qw/aom_smooth_predictor_4x16 neon ssse3/;
specialize qw/aom_smooth_predictor_8x4 neon ssse3/;
specialize qw/aom_smooth_predictor_8x8 neon ssse3/;
specialize qw/aom_smooth_predictor_8x16 neon ssse3/;
specialize qw/aom_smooth_predictor_8x32 neon ssse3/;
specialize qw/aom_smooth_predictor_16x4 neon ssse3/;
specialize qw/aom_smooth_predictor_16x8 neon ssse3/;
specialize qw/aom_smooth_predictor_16x16 neon ssse3/;
specialize qw/aom_smooth_predictor_16x32 neon ssse3/;
specialize qw/aom_smooth_predictor_16x64 neon ssse3/;
specialize qw/aom_smooth_predictor_32x8 neon ssse3/;
specialize qw/aom_smooth_predictor_32x16 neon ssse3/;
specialize qw/aom_smooth_predictor_32x32 neon ssse3/;
specialize qw/aom_smooth_predictor_32x64 neon ssse3/;
specialize qw/aom_smooth_predictor_64x16 neon ssse3/;
specialize qw/aom_smooth_predictor_64x32 neon ssse3/;
specialize qw/aom_smooth_predictor_64x64 neon ssse3/;

specialize qw/aom_smooth_v_predictor_4x4 neon ssse3/;
specialize qw/aom_smooth_v_predictor_4x8 neon ssse3/;
specialize qw/aom_smooth_v_predictor_4x16 neon ssse3/;
specialize qw/aom_smooth_v_predictor_8x4 neon ssse3/;
specialize qw/aom_smooth_v_predictor_8x8 neon ssse3/;
specialize qw/aom_smooth_v_predictor_8x16 neon ssse3/;
specialize qw/aom_smooth_v_predictor_8x32 neon ssse3/;
specialize qw/aom_smooth_v_predictor_16x4 neon ssse3/;
specialize qw/aom_smooth_v_predictor_16x8 neon ssse3/;
specialize qw/aom_smooth_v_predictor_16x16 neon ssse3/;
specialize qw/aom_smooth_v_predictor_16x32 neon ssse3/;
specialize qw/aom_smooth_v_predictor_16x64 neon ssse3/;
specialize qw/aom_smooth_v_predictor_32x8 neon ssse3/;
specialize qw/aom_smooth_v_predictor_32x16 neon ssse3/;
specialize qw/aom_smooth_v_predictor_32x32 neon ssse3/;
specialize qw/aom_smooth_v_predictor_32x64 neon ssse3/;
specialize qw/aom_smooth_v_predictor_64x16 neon ssse3/;
specialize qw/aom_smooth_v_predictor_64x32 neon ssse3/;
specialize qw/aom_smooth_v_predictor_64x64 neon ssse3/;

specialize qw/aom_smooth_h_predictor_4x4 neon ssse3/;
specialize qw/aom_smooth_h_predictor_4x8 neon ssse3/;
specialize qw/aom_smooth_h_predictor_4x16 neon ssse3/;
specialize qw/aom_smooth_h_predictor_8x4 neon ssse3/;
specialize qw/aom_smooth_h_predictor_8x8 neon ssse3/;
specialize qw/aom_smooth_h_predictor_8x16 neon ssse3/;
specialize qw/aom_smooth_h_predictor_8x32 neon ssse3/;
specialize qw/aom_smooth_h_predictor_16x4 neon ssse3/;
specialize qw/aom_smooth_h_predictor_16x8 neon ssse3/;
specialize qw/aom_smooth_h_predictor_16x16 neon ssse3/;
specialize qw/aom_smooth_h_predictor_16x32 neon ssse3/;
specialize qw/aom_smooth_h_predictor_16x64 neon ssse3/;
specialize qw/aom_smooth_h_predictor_32x8 neon ssse3/;
specialize qw/aom_smooth_h_predictor_32x16 neon ssse3/;
specialize qw/aom_smooth_h_predictor_32x32 neon ssse3/;
specialize qw/aom_smooth_h_predictor_32x64 neon ssse3/;
specialize qw/aom_smooth_h_predictor_64x16 neon ssse3/;
specialize qw/aom_smooth_h_predictor_64x32 neon ssse3/;
specialize qw/aom_smooth_h_predictor_64x64 neon ssse3/;

# TODO(yunqingwang): optimize rectangular DC_PRED to replace division
# by multiply and shift.
specialize qw/aom_dc_predictor_4x4 neon sse2/;
specialize qw/aom_dc_predictor_4x8 neon sse2/;
specialize qw/aom_dc_predictor_4x16 neon sse2/;
specialize qw/aom_dc_predictor_8x4 neon sse2/;
specialize qw/aom_dc_predictor_8x8 neon sse2/;
specialize qw/aom_dc_predictor_8x16 neon sse2/;
specialize qw/aom_dc_predictor_8x32 neon sse2/;
specialize qw/aom_dc_predictor_16x4 neon sse2/;
specialize qw/aom_dc_predictor_16x8 neon sse2/;
specialize qw/aom_dc_predictor_16x16 neon sse2/;
specialize qw/aom_dc_predictor_16x32 neon sse2/;
specialize qw/aom_dc_predictor_16x64 neon sse2/;
specialize qw/aom_dc_predictor_32x8 neon sse2/;
specialize qw/aom_dc_predictor_32x16 neon sse2 avx2/;
specialize qw/aom_dc_predictor_32x32 neon sse2 avx2/;
specialize qw/aom_dc_predictor_32x64 neon sse2 avx2/;
specialize qw/aom_dc_predictor_64x64 neon sse2 avx2/;
specialize qw/aom_dc_predictor_64x32 neon sse2 avx2/;
specialize qw/aom_dc_predictor_64x16 neon sse2 avx2/;
if (aom_config("CONFIG_AV1_HIGHBITDEPTH") eq "yes") {
  specialize qw/aom_highbd_v_predictor_4x4 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_4x8 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_4x16 neon/;
  specialize qw/aom_highbd_v_predictor_8x4 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_8x8 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_8x16 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_8x32 neon/;
  specialize qw/aom_highbd_v_predictor_16x4 neon/;
  specialize qw/aom_highbd_v_predictor_16x8 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_16x16 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_16x32 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_16x64 neon/;
  specialize qw/aom_highbd_v_predictor_32x8 neon/;
  specialize qw/aom_highbd_v_predictor_32x16 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_32x32 sse2 neon/;
  specialize qw/aom_highbd_v_predictor_32x64 neon/;
  specialize qw/aom_highbd_v_predictor_64x16 neon/;
  specialize qw/aom_highbd_v_predictor_64x32 neon/;
  specialize qw/aom_highbd_v_predictor_64x64 neon/;

  # TODO(yunqingwang): optimize rectangular DC_PRED to replace division
  # by multiply and shift.
  specialize qw/aom_highbd_dc_predictor_4x4 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_4x8 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_4x16 neon/;
  specialize qw/aom_highbd_dc_predictor_8x4 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_8x8 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_8x16 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_8x32 neon/;
  specialize qw/aom_highbd_dc_predictor_16x4 neon/;
  specialize qw/aom_highbd_dc_predictor_16x8 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_16x16 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_16x32 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_16x64 neon/;
  specialize qw/aom_highbd_dc_predictor_32x8 neon/;
  specialize qw/aom_highbd_dc_predictor_32x16 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_32x32 sse2 neon/;
  specialize qw/aom_highbd_dc_predictor_32x64 neon/;
  specialize qw/aom_highbd_dc_predictor_64x16 neon/;
  specialize qw/aom_highbd_dc_predictor_64x32 neon/;
  specialize qw/aom_highbd_dc_predictor_64x64 neon/;

  specialize qw/aom_highbd_h_predictor_4x4 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_4x8 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_4x16 neon/;
  specialize qw/aom_highbd_h_predictor_8x4 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_8x8 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_8x16 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_8x32 neon/;
  specialize qw/aom_highbd_h_predictor_16x4 neon/;
  specialize qw/aom_highbd_h_predictor_16x8 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_16x16 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_16x32 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_16x64 neon/;
  specialize qw/aom_highbd_h_predictor_32x8 neon/;
  specialize qw/aom_highbd_h_predictor_32x16 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_32x32 sse2 neon/;
  specialize qw/aom_highbd_h_predictor_32x64 neon/;
  specialize qw/aom_highbd_h_predictor_64x16 neon/;
  specialize qw/aom_highbd_h_predictor_64x32 neon/;
  specialize qw/aom_highbd_h_predictor_64x64 neon/;

  specialize qw/aom_highbd_dc_128_predictor_4x4 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_4x8 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_4x16 neon/;
  specialize qw/aom_highbd_dc_128_predictor_8x4 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_8x8 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_8x16 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_8x32 neon/;
  specialize qw/aom_highbd_dc_128_predictor_16x4 neon/;
  specialize qw/aom_highbd_dc_128_predictor_16x8 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_16x16 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_16x32 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_16x64 neon/;
  specialize qw/aom_highbd_dc_128_predictor_32x8 neon/;
  specialize qw/aom_highbd_dc_128_predictor_32x16 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_32x32 sse2 neon/;
  specialize qw/aom_highbd_dc_128_predictor_32x64 neon/;
  specialize qw/aom_highbd_dc_128_predictor_64x16 neon/;
  specialize qw/aom_highbd_dc_128_predictor_64x32 neon/;
  specialize qw/aom_highbd_dc_128_predictor_64x64 neon/;

  specialize qw/aom_highbd_dc_left_predictor_4x4 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_4x8 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_4x16 neon/;
  specialize qw/aom_highbd_dc_left_predictor_8x4 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_8x8 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_8x16 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_8x32 neon/;
  specialize qw/aom_highbd_dc_left_predictor_16x4 neon/;
  specialize qw/aom_highbd_dc_left_predictor_16x8 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_16x16 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_16x32 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_16x64 neon/;
  specialize qw/aom_highbd_dc_left_predictor_32x8 neon/;
  specialize qw/aom_highbd_dc_left_predictor_32x16 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_32x32 sse2 neon/;
  specialize qw/aom_highbd_dc_left_predictor_32x64 neon/;
  specialize qw/aom_highbd_dc_left_predictor_64x16 neon/;
  specialize qw/aom_highbd_dc_left_predictor_64x32 neon/;
  specialize qw/aom_highbd_dc_left_predictor_64x64 neon/;

  specialize qw/aom_highbd_dc_top_predictor_4x4 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_4x8 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_4x16 neon/;
  specialize qw/aom_highbd_dc_top_predictor_8x4 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_8x8 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_8x16 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_8x32 neon/;
  specialize qw/aom_highbd_dc_top_predictor_16x4 neon/;
  specialize qw/aom_highbd_dc_top_predictor_16x8 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_16x16 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_16x32 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_16x64 neon/;
  specialize qw/aom_highbd_dc_top_predictor_32x8 neon/;
  specialize qw/aom_highbd_dc_top_predictor_32x16 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_32x32 sse2 neon/;
  specialize qw/aom_highbd_dc_top_predictor_32x64 neon/;
  specialize qw/aom_highbd_dc_top_predictor_64x16 neon/;
  specialize qw/aom_highbd_dc_top_predictor_64x32 neon/;
  specialize qw/aom_highbd_dc_top_predictor_64x64 neon/;

  specialize qw/aom_highbd_paeth_predictor_4x4 neon/;
  specialize qw/aom_highbd_paeth_predictor_4x8 neon/;
  specialize qw/aom_highbd_paeth_predictor_4x16 neon/;
  specialize qw/aom_highbd_paeth_predictor_8x4 neon/;
  specialize qw/aom_highbd_paeth_predictor_8x8 neon/;
  specialize qw/aom_highbd_paeth_predictor_8x16 neon/;
  specialize qw/aom_highbd_paeth_predictor_8x32 neon/;
  specialize qw/aom_highbd_paeth_predictor_16x4 neon/;
  specialize qw/aom_highbd_paeth_predictor_16x8 neon/;
  specialize qw/aom_highbd_paeth_predictor_16x16 neon/;
  specialize qw/aom_highbd_paeth_predictor_16x32 neon/;
  specialize qw/aom_highbd_paeth_predictor_16x64 neon/;
  specialize qw/aom_highbd_paeth_predictor_32x8 neon/;
  specialize qw/aom_highbd_paeth_predictor_32x16 neon/;
  specialize qw/aom_highbd_paeth_predictor_32x32 neon/;
  specialize qw/aom_highbd_paeth_predictor_32x64 neon/;
  specialize qw/aom_highbd_paeth_predictor_64x16 neon/;
  specialize qw/aom_highbd_paeth_predictor_64x32 neon/;
  specialize qw/aom_highbd_paeth_predictor_64x64 neon/;

  specialize qw/aom_highbd_smooth_predictor_4x4 neon/;
  specialize qw/aom_highbd_smooth_predictor_4x8 neon/;
  specialize qw/aom_highbd_smooth_predictor_4x16 neon/;
  specialize qw/aom_highbd_smooth_predictor_8x4 neon/;
  specialize qw/aom_highbd_smooth_predictor_8x8 neon/;
  specialize qw/aom_highbd_smooth_predictor_8x16 neon/;
  specialize qw/aom_highbd_smooth_predictor_8x32 neon/;
  specialize qw/aom_highbd_smooth_predictor_16x4 neon/;
  specialize qw/aom_highbd_smooth_predictor_16x8 neon/;
  specialize qw/aom_highbd_smooth_predictor_16x16 neon/;
  specialize qw/aom_highbd_smooth_predictor_16x32 neon/;
  specialize qw/aom_highbd_smooth_predictor_16x64 neon/;
  specialize qw/aom_highbd_smooth_predictor_32x8 neon/;
  specialize qw/aom_highbd_smooth_predictor_32x16 neon/;
  specialize qw/aom_highbd_smooth_predictor_32x32 neon/;
  specialize qw/aom_highbd_smooth_predictor_32x64 neon/;
  specialize qw/aom_highbd_smooth_predictor_64x16 neon/;
  specialize qw/aom_highbd_smooth_predictor_64x32 neon/;
  specialize qw/aom_highbd_smooth_predictor_64x64 neon/;

  specialize qw/aom_highbd_smooth_v_predictor_4x4 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_4x8 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_4x16 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_8x4 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_8x8 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_8x16 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_8x32 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_16x4 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_16x8 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_16x16 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_16x32 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_16x64 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_32x8 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_32x16 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_32x32 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_32x64 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_64x16 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_64x32 neon/;
  specialize qw/aom_highbd_smooth_v_predictor_64x64 neon/;

  specialize qw/aom_highbd_smooth_h_predictor_4x4 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_4x8 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_4x16 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_8x4 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_8x8 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_8x16 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_8x32 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_16x4 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_16x8 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_16x16 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_16x32 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_16x64 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_32x8 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_32x16 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_32x32 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_32x64 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_64x16 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_64x32 neon/;
  specialize qw/aom_highbd_smooth_h_predictor_64x64 neon/;
}
#
# Sub Pixel Filters
#
add_proto qw/void aom_convolve8/, "const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h";
add_proto qw/void aom_convolve_copy/,             "const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, int w, int h";
add_proto qw/void aom_convolve8_horiz/,           "const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h";
add_proto qw/void aom_convolve8_vert/,            "const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h";

specialize qw/aom_convolve_copy       neon                        sse2 avx2/;
specialize qw/aom_convolve8_horiz     neon neon_dotprod neon_i8mm ssse3/, "$avx2_ssse3";
specialize qw/aom_convolve8_vert      neon neon_dotprod neon_i8mm ssse3/, "$avx2_ssse3";

add_proto qw/void aom_scaled_2d/, "const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const InterpKernel *filter, int x0_q4, int x_step_q4, int y0_q4, int y_step_q4, int w, int h";
specialize qw/aom_scaled_2d ssse3 neon neon_dotprod neon_i8mm/;

if (aom_config("CONFIG_AV1_HIGHBITDEPTH") eq "yes") {
  add_proto qw/void aom_highbd_convolve_copy/, "const uint16_t *src, ptrdiff_t src_stride, uint16_t *dst, ptrdiff_t dst_stride, int w, int h";
  specialize qw/aom_highbd_convolve_copy sse2 avx2 neon/;

  add_proto qw/void aom_highbd_convolve8_horiz/, "const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd";
  specialize qw/aom_highbd_convolve8_horiz sse2 avx2 neon sve/;

  add_proto qw/void aom_highbd_convolve8_vert/, "const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h, int bd";
  specialize qw/aom_highbd_convolve8_vert sse2 avx2 neon sve/;
}

#
# Loopfilter
#
add_proto qw/void aom_lpf_vertical_14/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_vertical_14 sse2 neon/;

add_proto qw/void aom_lpf_vertical_14_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_vertical_14_dual sse2 neon/;

add_proto qw/void aom_lpf_vertical_14_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_vertical_14_quad avx2 sse2 neon/;

add_proto qw/void aom_lpf_vertical_6/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_vertical_6 sse2 neon/;

add_proto qw/void aom_lpf_vertical_8/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_vertical_8 sse2 neon/;

add_proto qw/void aom_lpf_vertical_8_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_vertical_8_dual sse2 neon/;

add_proto qw/void aom_lpf_vertical_8_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_vertical_8_quad sse2 neon/;

add_proto qw/void aom_lpf_vertical_4/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_vertical_4 sse2 neon/;

add_proto qw/void aom_lpf_vertical_4_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_vertical_4_dual sse2 neon/;

add_proto qw/void aom_lpf_vertical_4_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_vertical_4_quad sse2 neon/;

add_proto qw/void aom_lpf_horizontal_14/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_horizontal_14 sse2 neon/;

add_proto qw/void aom_lpf_horizontal_14_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_horizontal_14_dual sse2 neon/;

add_proto qw/void aom_lpf_horizontal_14_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_horizontal_14_quad sse2 avx2 neon/;

add_proto qw/void aom_lpf_horizontal_6/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_horizontal_6 sse2 neon/;

add_proto qw/void aom_lpf_horizontal_6_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_horizontal_6_dual sse2 neon/;

add_proto qw/void aom_lpf_horizontal_6_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_horizontal_6_quad sse2 avx2 neon/;

add_proto qw/void aom_lpf_horizontal_8/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_horizontal_8 sse2 neon/;

add_proto qw/void aom_lpf_horizontal_8_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_horizontal_8_dual sse2 neon/;

add_proto qw/void aom_lpf_horizontal_8_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_horizontal_8_quad sse2 avx2 neon/;

add_proto qw/void aom_lpf_horizontal_4/, "uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh";
specialize qw/aom_lpf_horizontal_4 sse2 neon/;

add_proto qw/void aom_lpf_horizontal_4_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_horizontal_4_dual sse2 neon/;

add_proto qw/void aom_lpf_horizontal_4_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_horizontal_4_quad sse2 neon/;

add_proto qw/void aom_lpf_vertical_6_dual/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1";
specialize qw/aom_lpf_vertical_6_dual sse2 neon/;

add_proto qw/void aom_lpf_vertical_6_quad/, "uint8_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0";
specialize qw/aom_lpf_vertical_6_quad sse2 neon/;

if (aom_config("CONFIG_AV1_HIGHBITDEPTH") eq "yes") {
  add_proto qw/void aom_highbd_lpf_vertical_14/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_vertical_14 neon sse2/;

  add_proto qw/void aom_highbd_lpf_vertical_14_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1, int bd";
  specialize qw/aom_highbd_lpf_vertical_14_dual neon sse2 avx2/;

  add_proto qw/void aom_highbd_lpf_vertical_8/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_vertical_8 neon sse2/;

  add_proto qw/void aom_highbd_lpf_vertical_8_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1, int bd";
  specialize qw/aom_highbd_lpf_vertical_8_dual neon sse2 avx2/;

  add_proto qw/void aom_highbd_lpf_vertical_6/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_vertical_6 neon sse2/;

  add_proto qw/void aom_highbd_lpf_vertical_6_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1, int bd";
  specialize qw/aom_highbd_lpf_vertical_6_dual neon sse2/;

  add_proto qw/void aom_highbd_lpf_vertical_4/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_vertical_4 neon sse2/;

  add_proto qw/void aom_highbd_lpf_vertical_4_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1, int bd";
  specialize qw/aom_highbd_lpf_vertical_4_dual neon sse2 avx2/;

  add_proto qw/void aom_highbd_lpf_horizontal_14/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_horizontal_14 neon sse2/;

  add_proto qw/void aom_highbd_lpf_horizontal_14_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1,int bd";
  specialize qw/aom_highbd_lpf_horizontal_14_dual neon sse2 avx2/;

  add_proto qw/void aom_highbd_lpf_horizontal_6/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_horizontal_6 neon sse2/;

  add_proto qw/void aom_highbd_lpf_horizontal_6_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1, int bd";
  specialize qw/aom_highbd_lpf_horizontal_6_dual neon sse2/;

  add_proto qw/void aom_highbd_lpf_horizontal_8/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_horizontal_8 neon sse2/;

  add_proto qw/void aom_highbd_lpf_horizontal_8_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1, int bd";
  specialize qw/aom_highbd_lpf_horizontal_8_dual neon sse2 avx2/;

  add_proto qw/void aom_highbd_lpf_horizontal_4/, "uint16_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int bd";
  specialize qw/aom_highbd_lpf_horizontal_4 neon sse2/;

  add_proto qw/void aom_highbd_lpf_horizontal_4_dual/, "uint16_t *s, int pitch, const uint8_t *blimit0, const uint8_t *limit0, const uint8_t *thresh0, const uint8_t *blimit1, const uint8_t *limit1, const uint8_t *thresh1, int bd";
  specialize qw/aom_highbd_lpf_horizontal_4_dual neon sse2 avx2/;
}

#
# Encoder functions.
#

#
# Forward transform
#
if (aom_config("CONFIG_AV1_ENCODER") eq "yes"){
    add_proto qw/void aom_fdct4x4/, "const int16_t *input, tran_low_t *output, int stride";
    specialize qw/aom_fdct4x4 neon sse2/;

    add_proto qw/void aom_fdct4x4_lp/, "const int16_t *input, int16_t *output, int stride";
    specialize qw/aom_fdct4x4_lp neon sse2/;

    if (aom_config("CONFIG_INTERNAL_STATS") eq "yes"){
      # 8x8 DCT transform for psnr-hvs. Unlike other transforms isn't compatible
      # with av1 scan orders, because it does two transposes.
      add_proto qw/void aom_fdct8x8/, "const int16_t *input, tran_low_t *output, int stride";
      specialize qw/aom_fdct8x8 neon sse2/, "$ssse3_x86_64";
      # High bit depth
      if (aom_config("CONFIG_AV1_HIGHBITDEPTH") eq "yes") {
        add_proto qw/void aom_highbd_fdct8x8/, "const int16_t *input, tran_low_t *output, int stride";
        specialize qw/aom_highbd_fdct8x8 sse2/;
      }
    }
    # FFT/IFFT (float) only used for denoising (and noise power spectral density estimation)
    add_proto qw/void aom_fft2x2_float/, "const float *input, float *temp, float *output";

    add_proto qw/void aom_fft4x4_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_fft4x4_float                  sse2/;

    add_proto qw/void aom_fft8x8_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_fft8x8_float avx2             sse2/;

    add_proto qw/void aom_fft16x16_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_fft16x16_float avx2           sse2/;

    add_proto qw/void aom_fft32x32_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_fft32x32_float avx2           sse2/;

    add_proto qw/void aom_ifft2x2_float/, "const float *input, float *temp, float *output";

    add_proto qw/void aom_ifft4x4_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_ifft4x4_float                 sse2/;

    add_proto qw/void aom_ifft8x8_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_ifft8x8_float avx2            sse2/;

    add_proto qw/void aom_ifft16x16_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_ifft16x16_float avx2          sse2/;

    add_proto qw/void aom_ifft32x32_float/, "const float *input, float *temp, float *output";
    specialize qw/aom_ifft32x32_float avx2          sse2/;
}  # CONFIG_AV1_ENCODER

#
# Quantization
#
if (aom_config("CONFIG_AV1_ENCODER") eq "yes") {
  add_proto qw/void aom_quantize_b/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
  specialize qw/aom_quantize_b sse2 neon avx avx2/, "$ssse3_x86_64";

  add_proto qw/void aom_quantize_b_32x32/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
  specialize qw/aom_quantize_b_32x32 neon avx avx2/, "$ssse3_x86_64";

  add_proto qw/void aom_quantize_b_64x64/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
  specialize qw/aom_quantize_b_64x64 neon ssse3 avx2/;

  if (aom_config("CONFIG_REALTIME_ONLY") ne "yes") {
    add_proto qw/void aom_quantize_b_adaptive/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
    specialize qw/aom_quantize_b_adaptive sse2 avx2/;

    add_proto qw/void aom_quantize_b_32x32_adaptive/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
    specialize qw/aom_quantize_b_32x32_adaptive sse2/;

    add_proto qw/void aom_quantize_b_64x64_adaptive/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
    specialize qw/aom_quantize_b_64x64_adaptive sse2/;
  }
}  # CONFIG_AV1_ENCODER

if (aom_config("CONFIG_AV1_ENCODER") eq "yes" && aom_config("CONFIG_AV1_HIGHBITDEPTH") eq "yes") {
  add_proto qw/void aom_highbd_quantize_b/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
  specialize qw/aom_highbd_quantize_b sse2 avx2 neon/;

  add_proto qw/void aom_highbd_quantize_b_32x32/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
  specialize qw/aom_highbd_quantize_b_32x32 sse2 avx2 neon/;

  add_proto qw/void aom_highbd_quantize_b_64x64/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
  specialize qw/aom_highbd_quantize_b_64x64 sse2 avx2 neon/;

  if (aom_config("CONFIG_REALTIME_ONLY") ne "yes") {
    add_proto qw/void aom_highbd_quantize_b_adaptive/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
    specialize qw/aom_highbd_quantize_b_adaptive sse2 avx2 neon/;

    add_proto qw/void aom_highbd_quantize_b_32x32_adaptive/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
    specialize qw/aom_highbd_quantize_b_32x32_adaptive sse2 avx2 neon/;

    add_proto qw/void aom_highbd_quantize_b_64x64_adaptive/, "const tran_low_t *coeff_ptr, intptr_t n_coeffs, const int16_t *zbin_ptr, const int16_t *round_ptr, const int16_t *quant_ptr, const int16_t *quant_shift_ptr, tran_low_t *qcoeff_ptr, tran_low_t *dqcoeff_ptr, const int16_t *dequant_ptr, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan";
    specialize qw/aom_highbd_quantize_b_64x64_adaptive sse2 neon/;
  }
}  # CONFIG_AV1_ENCODER

#
# Alpha blending with mask
#
add_proto qw/void aom_lowbd_blend_a64_d16_mask/, "uint8_t *dst, uint32_t dst_stride, const CONV_BUF_TYPE *src0, uint32_t src0_stride, const CONV_BUF_TYPE *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh, ConvolveParams *conv_params";
specialize qw/aom_lowbd_blend_a64_d16_mask sse4_1 avx2 neon/;
add_proto qw/void aom_blend_a64_mask/, "uint8_t *dst, uint32_t dst_stride, const uint8_t *src0, uint32_t src0_stride, const uint8_t *src1, uint32_t src1_stride, const uint8_t *mask, uint32_t mask_stride, int w, int h, int subw, int subh";
add_proto qw/void aom_blend_a64_hmask/, "uint8_t *dst, uint32_t dst_stride, const uint8_t *src0, uint32_t src0_stride, const uint8_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h";
add_proto qw/void aom_blend_a64_vmask/, "uint8_t *dst, uint32_t dst_stride, const uint8_t *src0, uint32_t src0_stride, const uint8_t *src1, uint32_t src1_stride, const uint8_t *mask, int w, int h";
specialize "aom_blend_a64_mask", qw/sse4_1 neon avx2/;
specialize "aom_blend_a64_hmask", qw/sse4_1 neon/;
specialize "aom_blend_a64_vmask", qw/sse4_1 neon/;


1;
