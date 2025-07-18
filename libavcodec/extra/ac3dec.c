/*
 * AC-3 Audio Decoder
 * This code was developed as part of Google Summer of Code 2006.
 * E-AC-3 support was added as part of Google Summer of Code 2007.
 *
 * Copyright (c) 2006 Kartikey Mahendra BHATT (bhattkm at gmail dot com)
 * Copyright (c) 2007-2008 Bartlomiej Wolowiec <bartek.wolowiec@gmail.com>
 * Copyright (c) 2007 Justin Ruggles <justin.ruggles@gmail.com>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavcodec/config_components.h"

#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <string.h>

#include "libavutil/channel_layout.h"
#include "libavutil/crc.h"
#include "libavutil/downmix_info.h"
#include "libavutil/intmath.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavutil/thread.h"
#include "libavcodec/bswapdsp.h"
#include "libavcodec/ac3_parser_internal.h"
#include "libavcodec/ac3dec.h"
#include "libavcodec/ac3dec_data.h"
#include "libavcodec/ac3defs.h"
#include "libavcodec/decode.h"
#include "libavcodec/kbdwin.h"

#if (!USE_FIXED)
/** dynamic range table. converts codes to scale factors. */
static float dynamic_range_tab[256];
float ff_ac3_heavy_dynamic_range_tab[256];

/*
 * Initialize tables at runtime.
 */
static av_cold void ac3_float_tables_init(void)
{
    /* generate dynamic range table
       reference: Section 7.7.1 Dynamic Range Control */
    for (int i = 0; i < 256; i++) {
        int v = (i >> 5) - ((i >> 7) << 3) - 5;
        dynamic_range_tab[i] = powf(2.0f, v) * ((i & 0x1F) | 0x20);
    }

    /* generate compr dynamic range table
       reference: Section 7.7.2 Heavy Compression */
    for (int i = 0; i < 256; i++) {
        int v = (i >> 4) - ((i >> 7) << 4) - 4;
        ff_ac3_heavy_dynamic_range_tab[i] = powf(2.0f, v) * ((i & 0xF) | 0x10);
    }
    ff_ac3_init_static();
}
#endif

static void ac3_downmix(AVCodecContext *avctx)
{
    AC3DecodeContext *s = avctx->priv_data;
    const AVChannelLayout mono   = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
    const AVChannelLayout stereo = (AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO;

    /* allow downmixing to stereo or mono */
    if (avctx->ch_layout.nb_channels > 1 &&
        !av_channel_layout_compare(&s->downmix_layout, &mono)) {
        av_channel_layout_uninit(&avctx->ch_layout);
        avctx->ch_layout = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
    } else if (avctx->ch_layout.nb_channels > 2 &&
             !av_channel_layout_compare(&s->downmix_layout, &stereo)) {
        av_channel_layout_uninit(&avctx->ch_layout);
        avctx->ch_layout = (AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO;
    }
}

/**
 * AVCodec initialization
 */
static av_cold int ac3_decode_init(AVCodecContext *avctx)
{
    AC3DecodeContext *s = avctx->priv_data;
    const float scale = 1.0f;
    int i, ret;

    s->avctx = avctx;

    if ((ret = av_tx_init(&s->tx_128, &s->tx_fn_128, IMDCT_TYPE, 1, 128, &scale, 0)))
        return ret;

    if ((ret = av_tx_init(&s->tx_256, &s->tx_fn_256, IMDCT_TYPE, 1, 256, &scale, 0)))
        return ret;

    AC3_RENAME(ff_kbd_window_init)(s->window, 5.0, 256);
    ff_bswapdsp_init(&s->bdsp);

#if (USE_FIXED)
    s->fdsp = avpriv_alloc_fixed_dsp(avctx->flags & AV_CODEC_FLAG_BITEXACT);
#else
    ff_fmt_convert_init(&s->fmt_conv);
    s->fdsp = avpriv_float_dsp_alloc(avctx->flags & AV_CODEC_FLAG_BITEXACT);
#endif
    if (!s->fdsp)
        return AVERROR(ENOMEM);

    ff_ac3dsp_init(&s->ac3dsp);
    av_lfg_init(&s->dith_state, 0);

    if (USE_FIXED)
        avctx->sample_fmt = AV_SAMPLE_FMT_S16P;
    else
        avctx->sample_fmt = AV_SAMPLE_FMT_FLTP;

    ac3_downmix(avctx);
    s->downmixed = 1;

    for (i = 0; i < AC3_MAX_CHANNELS; i++) {
        s->xcfptr[i] = s->transform_coeffs[i];
        s->dlyptr[i] = s->delay[i];
    }

#if USE_FIXED
    ff_ac3_init_static();
#else
    static AVOnce init_static_once = AV_ONCE_INIT;
    ff_thread_once(&init_static_once, ac3_float_tables_init);
#endif

    return 0;
}

static av_cold void ac3_decode_flush(AVCodecContext *avctx)
{
    AC3DecodeContext *s = avctx->priv_data;

    memset(&s->frame_type, 0, sizeof(*s) - offsetof(AC3DecodeContext, frame_type));

    AC3_RENAME(ff_kbd_window_init)(s->window, 5.0, 256);
    av_lfg_init(&s->dith_state, 0);
}

/**
 * Common function to parse AC-3 or E-AC-3 frame header
 */
static int parse_frame_header(AC3DecodeContext *s)
{
    AC3HeaderInfo hdr;
    int err;

    err = ff_ac3_parse_header(&s->gbc, &hdr);
    if (err)
        return err;

    /* get decoding parameters from header info */
    s->bit_alloc_params.sr_code     = hdr.sr_code;
    s->bitstream_id                 = hdr.bitstream_id;
    s->bitstream_mode               = hdr.bitstream_mode;
    s->channel_mode                 = hdr.channel_mode;
    s->lfe_on                       = hdr.lfe_on;
    s->bit_alloc_params.sr_shift    = hdr.sr_shift;
    s->sample_rate                  = hdr.sample_rate;
    s->bit_rate                     = hdr.bit_rate;
    s->channels                     = hdr.channels;
    s->fbw_channels                 = s->channels - s->lfe_on;
    s->lfe_ch                       = s->fbw_channels + 1;
    s->frame_size                   = hdr.frame_size;
    s->superframe_size             += hdr.frame_size;
    s->preferred_downmix            = AC3_DMIXMOD_NOTINDICATED;
    if (hdr.bitstream_id <= 10) {
        s->center_mix_level         = hdr.center_mix_level;
        s->surround_mix_level       = hdr.surround_mix_level;
    }
    s->center_mix_level_ltrt        = 4; // -3.0dB
    s->surround_mix_level_ltrt      = 4; // -3.0dB
    s->lfe_mix_level_exists         = 0;
    s->num_blocks                   = hdr.num_blocks;
    s->frame_type                   = hdr.frame_type;
    s->substreamid                  = hdr.substreamid;
    s->dolby_surround_mode          = hdr.dolby_surround_mode;
    s->dolby_surround_ex_mode       = AC3_DSUREXMOD_NOTINDICATED;
    s->dolby_headphone_mode         = AC3_DHEADPHONMOD_NOTINDICATED;

    if (s->lfe_on) {
        s->start_freq[s->lfe_ch]     = 0;
        s->end_freq[s->lfe_ch]       = 7;
        s->num_exp_groups[s->lfe_ch] = 2;
        s->channel_in_cpl[s->lfe_ch] = 0;
    }

    if (s->bitstream_id <= 10) {
        s->eac3                  = 0;
        s->snr_offset_strategy   = 2;
        s->block_switch_syntax   = 1;
        s->dither_flag_syntax    = 1;
        s->bit_allocation_syntax = 1;
        s->fast_gain_syntax      = 0;
        s->first_cpl_leak        = 0;
        s->dba_syntax            = 1;
        s->skip_syntax           = 1;
        memset(s->channel_uses_aht, 0, sizeof(s->channel_uses_aht));
        /* volume control params */
        for (int i = 0; i < (s->channel_mode ? 1 : 2); i++) {
            s->dialog_normalization[i] = hdr.dialog_normalization[i];
            if (s->dialog_normalization[i] == 0) {
                s->dialog_normalization[i] = -31;
            }
            if (s->target_level != 0) {
                s->level_gain[i] = powf(2.0f,
                    (float)(s->target_level - s->dialog_normalization[i])/6.0f);
            }
            s->compression_exists[i] = hdr.compression_exists[i];
            if (s->compression_exists[i]) {
                s->heavy_dynamic_range[i] = AC3_HEAVY_RANGE(hdr.heavy_dynamic_range[i]);
            }
        }
        return 0;
    } else if (CONFIG_EAC3_DECODER) {
        s->eac3 = 1;
        return ff_eac3_parse_header(s, &hdr);
    } else {
        av_log(s->avctx, AV_LOG_ERROR, "E-AC-3 support not compiled in\n");
        return AVERROR(ENOSYS);
    }
}

/**
 * Set stereo downmixing coefficients based on frame header info.
 * reference: Section 7.8.2 Downmixing Into Two Channels
 */
static int set_downmix_coeffs(AC3DecodeContext *s)
{
    int i;
    float cmix = ff_ac3_gain_levels[s->  center_mix_level];
    float smix = ff_ac3_gain_levels[s->surround_mix_level];
    float norm0, norm1;
    float downmix_coeffs[2][AC3_MAX_CHANNELS];

    if (!s->downmix_coeffs[0]) {
        s->downmix_coeffs[0] = av_malloc_array(2 * AC3_MAX_CHANNELS,
                                               sizeof(**s->downmix_coeffs));
        if (!s->downmix_coeffs[0])
            return AVERROR(ENOMEM);
        s->downmix_coeffs[1] = s->downmix_coeffs[0] + AC3_MAX_CHANNELS;
    }

    for (i = 0; i < s->fbw_channels; i++) {
        downmix_coeffs[0][i] = ff_ac3_gain_levels[ff_ac3_default_coeffs[s->channel_mode][i][0]];
        downmix_coeffs[1][i] = ff_ac3_gain_levels[ff_ac3_default_coeffs[s->channel_mode][i][1]];
    }
    if (s->channel_mode > 1 && s->channel_mode & 1) {
        downmix_coeffs[0][1] = downmix_coeffs[1][1] = cmix;
    }
    if (s->channel_mode == AC3_CHMODE_2F1R || s->channel_mode == AC3_CHMODE_3F1R) {
        int nf = s->channel_mode - 2;
        downmix_coeffs[0][nf] = downmix_coeffs[1][nf] = smix * LEVEL_MINUS_3DB;
    }
    if (s->channel_mode == AC3_CHMODE_2F2R || s->channel_mode == AC3_CHMODE_3F2R) {
        int nf = s->channel_mode - 4;
        downmix_coeffs[0][nf] = downmix_coeffs[1][nf+1] = smix;
    }

    /* renormalize */
    norm0 = norm1 = 0.0;
    for (i = 0; i < s->fbw_channels; i++) {
        norm0 += downmix_coeffs[0][i];
        norm1 += downmix_coeffs[1][i];
    }
    norm0 = 1.0f / norm0;
    norm1 = 1.0f / norm1;
    for (i = 0; i < s->fbw_channels; i++) {
        downmix_coeffs[0][i] *= norm0;
        downmix_coeffs[1][i] *= norm1;
    }

    if (s->output_mode == AC3_CHMODE_MONO) {
        for (i = 0; i < s->fbw_channels; i++)
            downmix_coeffs[0][i] = (downmix_coeffs[0][i] +
                                    downmix_coeffs[1][i]) * LEVEL_MINUS_3DB;
    }
    for (i = 0; i < s->fbw_channels; i++) {
        s->downmix_coeffs[0][i] = FIXR12(downmix_coeffs[0][i]);
        s->downmix_coeffs[1][i] = FIXR12(downmix_coeffs[1][i]);
    }

    return 0;
}

/**
 * Decode the grouped exponents according to exponent strategy.
 * reference: Section 7.1.3 Exponent Decoding
 */
static int decode_exponents(AC3DecodeContext *s,
                            GetBitContext *gbc, int exp_strategy, int ngrps,
                            uint8_t absexp, int8_t *dexps)
{
    int i, j, grp, group_size;
    int dexp[256];
    int expacc, prevexp;

    /* unpack groups */
    group_size = exp_strategy + (exp_strategy == EXP_D45);
    for (grp = 0, i = 0; grp < ngrps; grp++) {
        expacc = get_bits(gbc, 7);
        if (expacc >= 125) {
            av_log(s->avctx, AV_LOG_ERROR, "expacc %d is out-of-range\n", expacc);
            return AVERROR_INVALIDDATA;
        }
        dexp[i++] = ff_ac3_ungroup_3_in_7_bits_tab[expacc][0];
        dexp[i++] = ff_ac3_ungroup_3_in_7_bits_tab[expacc][1];
        dexp[i++] = ff_ac3_ungroup_3_in_7_bits_tab[expacc][2];
    }

    /* convert to absolute exps and expand groups */
    prevexp = absexp;
    for (i = 0, j = 0; i < ngrps * 3; i++) {
        prevexp += dexp[i] - 2;
        if (prevexp > 24U) {
            av_log(s->avctx, AV_LOG_ERROR, "exponent %d is out-of-range\n", prevexp);
            return AVERROR_INVALIDDATA;
        }
        switch (group_size) {
        case 4: dexps[j++] = prevexp;
                dexps[j++] = prevexp;
        case 2: dexps[j++] = prevexp;
        case 1: dexps[j++] = prevexp;
        }
    }
    return 0;
}

/**
 * Generate transform coefficients for each coupled channel in the coupling
 * range using the coupling coefficients and coupling coordinates.
 * reference: Section 7.4.3 Coupling Coordinate Format
 */
static void calc_transform_coeffs_cpl(AC3DecodeContext *s)
{
    int bin, band, ch;

    bin = s->start_freq[CPL_CH];
    for (band = 0; band < s->num_cpl_bands; band++) {
        int band_start = bin;
        int band_end = bin + s->cpl_band_sizes[band];
        for (ch = 1; ch <= s->fbw_channels; ch++) {
            if (s->channel_in_cpl[ch]) {
                int cpl_coord = s->cpl_coords[ch][band] << 5;
                for (bin = band_start; bin < band_end; bin++) {
                    s->fixed_coeffs[ch][bin] =
                        MULH(s->fixed_coeffs[CPL_CH][bin] * (1 << 4), cpl_coord);
                }
                if (ch == 2 && s->phase_flags[band]) {
                    for (bin = band_start; bin < band_end; bin++)
                        s->fixed_coeffs[2][bin] = -s->fixed_coeffs[2][bin];
                }
            }
        }
        bin = band_end;
    }
}

/**
 * Grouped mantissas for 3-level 5-level and 11-level quantization
 */
typedef struct mant_groups {
    int b1_mant[2];
    int b2_mant[2];
    int b4_mant;
    int b1;
    int b2;
    int b4;
} mant_groups;

/**
 * Decode the transform coefficients for a particular channel
 * reference: Section 7.3 Quantization and Decoding of Mantissas
 */
static void ac3_decode_transform_coeffs_ch(AC3DecodeContext *s, int ch_index, mant_groups *m)
{
    int start_freq = s->start_freq[ch_index];
    int end_freq   = s->end_freq[ch_index];
    uint8_t *baps  = s->bap[ch_index];
    int8_t *exps   = s->dexps[ch_index];
    int32_t *coeffs = s->fixed_coeffs[ch_index];
    int dither     = (ch_index == CPL_CH) || s->dither_flag[ch_index];
    GetBitContext *gbc = &s->gbc;
    int freq;

    for (freq = start_freq; freq < end_freq; freq++) {
        int bap = baps[freq];
        int mantissa;
        switch (bap) {
        case 0:
            /* random noise with approximate range of -0.707 to 0.707 */
            if (dither)
                mantissa = (((av_lfg_get(&s->dith_state)>>8)*181)>>8) - 5931008;
            else
                mantissa = 0;
            break;
        case 1:
            if (m->b1) {
                m->b1--;
                mantissa = m->b1_mant[m->b1];
            } else {
                int bits      = get_bits(gbc, 5);
                mantissa      = ff_ac3_bap1_mantissas[bits][0];
                m->b1_mant[1] = ff_ac3_bap1_mantissas[bits][1];
                m->b1_mant[0] = ff_ac3_bap1_mantissas[bits][2];
                m->b1         = 2;
            }
            break;
        case 2:
            if (m->b2) {
                m->b2--;
                mantissa = m->b2_mant[m->b2];
            } else {
                int bits      = get_bits(gbc, 7);
                mantissa      = ff_ac3_bap2_mantissas[bits][0];
                m->b2_mant[1] = ff_ac3_bap2_mantissas[bits][1];
                m->b2_mant[0] = ff_ac3_bap2_mantissas[bits][2];
                m->b2         = 2;
            }
            break;
        case 3:
            mantissa = ff_ac3_bap3_mantissas[get_bits(gbc, 3)];
            break;
        case 4:
            if (m->b4) {
                m->b4 = 0;
                mantissa = m->b4_mant;
            } else {
                int bits   = get_bits(gbc, 7);
                mantissa   = ff_ac3_bap4_mantissas[bits][0];
                m->b4_mant = ff_ac3_bap4_mantissas[bits][1];
                m->b4      = 1;
            }
            break;
        case 5:
            mantissa = ff_ac3_bap5_mantissas[get_bits(gbc, 4)];
            break;
        default: /* 6 to 15 */
            /* Shift mantissa and sign-extend it. */
            if (bap > 15) {
                av_log(s->avctx, AV_LOG_ERROR, "bap %d is invalid in plain AC-3\n", bap);
                bap = 15;
            }
            mantissa = (unsigned)get_sbits(gbc, ff_ac3_quantization_tab[bap]) << (24 - ff_ac3_quantization_tab[bap]);
            break;
        }
        coeffs[freq] = mantissa >> exps[freq];
    }
}

/**
 * Remove random dithering from coupling range coefficients with zero-bit
 * mantissas for coupled channels which do not use dithering.
 * reference: Section 7.3.4 Dither for Zero Bit Mantissas (bap=0)
 */
static void remove_dithering(AC3DecodeContext *s) {
    int ch, i;

    for (ch = 1; ch <= s->fbw_channels; ch++) {
        if (!s->dither_flag[ch] && s->channel_in_cpl[ch]) {
            for (i = s->start_freq[CPL_CH]; i < s->end_freq[CPL_CH]; i++) {
                if (!s->bap[CPL_CH][i])
                    s->fixed_coeffs[ch][i] = 0;
            }
        }
    }
}

static inline void decode_transform_coeffs_ch(AC3DecodeContext *s, int blk,
                                              int ch, mant_groups *m)
{
    if (!s->channel_uses_aht[ch]) {
        ac3_decode_transform_coeffs_ch(s, ch, m);
    } else {
        /* if AHT is used, mantissas for all blocks are encoded in the first
           block of the frame. */
        int bin;
        if (CONFIG_EAC3_DECODER && !blk)
            ff_eac3_decode_transform_coeffs_aht_ch(s, ch);
        for (bin = s->start_freq[ch]; bin < s->end_freq[ch]; bin++) {
            s->fixed_coeffs[ch][bin] = s->pre_mantissa[ch][bin][blk] >> s->dexps[ch][bin];
        }
    }
}

/**
 * Decode the transform coefficients.
 */
static inline void decode_transform_coeffs(AC3DecodeContext *s, int blk)
{
    int ch, end;
    int got_cplchan = 0;
    mant_groups m;

    m.b1 = m.b2 = m.b4 = 0;

    for (ch = 1; ch <= s->channels; ch++) {
        /* transform coefficients for full-bandwidth channel */
        decode_transform_coeffs_ch(s, blk, ch, &m);
        /* transform coefficients for coupling channel come right after the
           coefficients for the first coupled channel*/
        if (s->channel_in_cpl[ch])  {
            if (!got_cplchan) {
                decode_transform_coeffs_ch(s, blk, CPL_CH, &m);
                calc_transform_coeffs_cpl(s);
                got_cplchan = 1;
            }
            end = s->end_freq[CPL_CH];
        } else {
            end = s->end_freq[ch];
        }
        do
            s->fixed_coeffs[ch][end] = 0;
        while (++end < 256);
    }

    /* zero the dithered coefficients for appropriate channels */
    remove_dithering(s);
}

/**
 * Stereo rematrixing.
 * reference: Section 7.5.4 Rematrixing : Decoding Technique
 */
static void do_rematrixing(AC3DecodeContext *s)
{
    int bnd, i;
    int end, bndend;

    end = FFMIN(s->end_freq[1], s->end_freq[2]);

    for (bnd = 0; bnd < s->num_rematrixing_bands; bnd++) {
        if (s->rematrixing_flags[bnd]) {
            bndend = FFMIN(end, ff_ac3_rematrix_band_tab[bnd + 1]);
            for (i = ff_ac3_rematrix_band_tab[bnd]; i < bndend; i++) {
                int tmp0 = s->fixed_coeffs[1][i];
                s->fixed_coeffs[1][i] += s->fixed_coeffs[2][i];
                s->fixed_coeffs[2][i]  = tmp0 - s->fixed_coeffs[2][i];
            }
        }
    }
}

/**
 * Inverse MDCT Transform.
 * Convert frequency domain coefficients to time-domain audio samples.
 * reference: Section 7.9.4 Transformation Equations
 */
static inline void do_imdct(AC3DecodeContext *s, int channels, int offset)
{
    int ch;

    for (ch = 1; ch <= channels; ch++) {
        if (s->block_switch[ch]) {
            int i;
            INTFLOAT *x = s->tmp_output + 128;
            for (i = 0; i < 128; i++)
                x[i] = s->transform_coeffs[ch][2 * i];
            s->tx_fn_128(s->tx_128, s->tmp_output, x, sizeof(INTFLOAT));
#if USE_FIXED
            s->fdsp->vector_fmul_window_scaled(s->outptr[ch - 1], s->delay[ch - 1 + offset],
                                       s->tmp_output, s->window, 128, 8);
#else
            s->fdsp->vector_fmul_window(s->outptr[ch - 1], s->delay[ch - 1 + offset],
                                       s->tmp_output, s->window, 128);
#endif
            for (i = 0; i < 128; i++)
                x[i] = s->transform_coeffs[ch][2 * i + 1];
            s->tx_fn_128(s->tx_128, s->delay[ch - 1 + offset], x, sizeof(INTFLOAT));
        } else {
            s->tx_fn_256(s->tx_256, s->tmp_output, s->transform_coeffs[ch], sizeof(INTFLOAT));
#if USE_FIXED
            s->fdsp->vector_fmul_window_scaled(s->outptr[ch - 1], s->delay[ch - 1 + offset],
                                       s->tmp_output, s->window, 128, 8);
#else
            s->fdsp->vector_fmul_window(s->outptr[ch - 1], s->delay[ch - 1 + offset],
                                       s->tmp_output, s->window, 128);
#endif
            memcpy(s->delay[ch - 1 + offset], s->tmp_output + 128, 128 * sizeof(INTFLOAT));
        }
    }
}

/**
 * Upmix delay samples from stereo to original channel layout.
 */
static void ac3_upmix_delay(AC3DecodeContext *s)
{
    int channel_data_size = sizeof(s->delay[0]);
    switch (s->channel_mode) {
    case AC3_CHMODE_DUALMONO:
    case AC3_CHMODE_STEREO:
        /* upmix mono to stereo */
        memcpy(s->delay[1], s->delay[0], channel_data_size);
        break;
    case AC3_CHMODE_2F2R:
        memset(s->delay[3], 0, channel_data_size);
    case AC3_CHMODE_2F1R:
        memset(s->delay[2], 0, channel_data_size);
        break;
    case AC3_CHMODE_3F2R:
        memset(s->delay[4], 0, channel_data_size);
    case AC3_CHMODE_3F1R:
        memset(s->delay[3], 0, channel_data_size);
    case AC3_CHMODE_3F:
        memcpy(s->delay[2], s->delay[1], channel_data_size);
        memset(s->delay[1], 0, channel_data_size);
        break;
    }
}

/**
 * Decode band structure for coupling, spectral extension, or enhanced coupling.
 * The band structure defines how many subbands are in each band.  For each
 * subband in the range, 1 means it is combined with the previous band, and 0
 * means that it starts a new band.
 *
 * @param[in] gbc bit reader context
 * @param[in] blk block number
 * @param[in] eac3 flag to indicate E-AC-3
 * @param[in] ecpl flag to indicate enhanced coupling
 * @param[in] start_subband subband number for start of range
 * @param[in] end_subband subband number for end of range
 * @param[in] default_band_struct default band structure table
 * @param[out] num_bands number of bands (optionally NULL)
 * @param[out] band_sizes array containing the number of bins in each band (optionally NULL)
 * @param[in,out] band_struct current band structure
 */
static void decode_band_structure(GetBitContext *gbc, int blk, int eac3,
                                  int ecpl, int start_subband, int end_subband,
                                  const uint8_t *default_band_struct,
                                  int *num_bands, uint8_t *band_sizes,
                                  uint8_t *band_struct, int band_struct_size)
{
    int subbnd, bnd, n_subbands, n_bands=0;
    uint8_t bnd_sz[22];

    n_subbands = end_subband - start_subband;

    if (!blk)
        memcpy(band_struct, default_band_struct, band_struct_size);

    av_assert0(band_struct_size >= start_subband + n_subbands);

    band_struct += start_subband + 1;

    /* decode band structure from bitstream or use default */
    if (!eac3 || get_bits1(gbc)) {
        for (subbnd = 0; subbnd < n_subbands - 1; subbnd++) {
            band_struct[subbnd] = get_bits1(gbc);
        }
    }

    /* calculate number of bands and band sizes based on band structure.
       note that the first 4 subbands in enhanced coupling span only 6 bins
       instead of 12. */
    if (num_bands || band_sizes ) {
        n_bands = n_subbands;
        bnd_sz[0] = ecpl ? 6 : 12;
        for (bnd = 0, subbnd = 1; subbnd < n_subbands; subbnd++) {
            int subbnd_size = (ecpl && subbnd < 4) ? 6 : 12;
            if (band_struct[subbnd - 1]) {
                n_bands--;
                bnd_sz[bnd] += subbnd_size;
            } else {
                bnd_sz[++bnd] = subbnd_size;
            }
        }
    }

    /* set optional output params */
    if (num_bands)
        *num_bands = n_bands;
    if (band_sizes)
        memcpy(band_sizes, bnd_sz, n_bands);
}

static inline int spx_strategy(AC3DecodeContext *s, int blk)
{
    GetBitContext *bc = &s->gbc;
    int dst_start_freq, dst_end_freq, src_start_freq,
        start_subband, end_subband;

    /* determine which channels use spx */
    if (s->channel_mode == AC3_CHMODE_MONO) {
        s->channel_uses_spx[1] = 1;
    } else {
        unsigned channel_uses_spx = get_bits(bc, s->fbw_channels);
        for (int ch = s->fbw_channels; ch >= 1; --ch) {
            s->channel_uses_spx[ch] = channel_uses_spx & 1;
            channel_uses_spx      >>= 1;
        }
    }

    /* get the frequency bins of the spx copy region and the spx start
       and end subbands */
    dst_start_freq = get_bits(bc, 2);
    start_subband  = get_bits(bc, 3) + 2;
    if (start_subband > 7)
        start_subband += start_subband - 7;
    end_subband    = get_bits(bc, 3) + 5;
#if USE_FIXED
    s->spx_dst_end_freq = end_freq_inv_tab[end_subband-5];
#endif
    if (end_subband   > 7)
        end_subband   += end_subband   - 7;
    dst_start_freq = dst_start_freq * 12 + 25;
    src_start_freq = start_subband  * 12 + 25;
    dst_end_freq   = end_subband    * 12 + 25;

    /* check validity of spx ranges */
    if (start_subband >= end_subband) {
        av_log(s->avctx, AV_LOG_ERROR, "invalid spectral extension "
               "range (%d >= %d)\n", start_subband, end_subband);
        return AVERROR_INVALIDDATA;
    }
    if (dst_start_freq >= src_start_freq) {
        av_log(s->avctx, AV_LOG_ERROR, "invalid spectral extension "
               "copy start bin (%d >= %d)\n", dst_start_freq, src_start_freq);
        return AVERROR_INVALIDDATA;
    }

    s->spx_dst_start_freq = dst_start_freq;
    s->spx_src_start_freq = src_start_freq;
    if (!USE_FIXED)
        s->spx_dst_end_freq   = dst_end_freq;

    decode_band_structure(bc, blk, s->eac3, 0,
                          start_subband, end_subband,
                          ff_eac3_default_spx_band_struct,
                          &s->num_spx_bands,
                          s->spx_band_sizes,
                          s->spx_band_struct, sizeof(s->spx_band_struct));
    return 0;
}

static inline void spx_coordinates(AC3DecodeContext *s)
{
    GetBitContext *bc = &s->gbc;
    int fbw_channels = s->fbw_channels;
    int ch, bnd;

    for (ch = 1; ch <= fbw_channels; ch++) {
        if (s->channel_uses_spx[ch]) {
            if (s->first_spx_coords[ch] || get_bits1(bc)) {
                INTFLOAT spx_blend;
                int bin, master_spx_coord;

                s->first_spx_coords[ch] = 0;
                spx_blend = AC3_SPX_BLEND(get_bits(bc, 5));
                master_spx_coord = get_bits(bc, 2) * 3;

                bin = s->spx_src_start_freq;
                for (bnd = 0; bnd < s->num_spx_bands; bnd++) {
                    int bandsize = s->spx_band_sizes[bnd];
                    int spx_coord_exp, spx_coord_mant;
                    INTFLOAT nratio, sblend, nblend;
#if USE_FIXED
                    /* calculate blending factors */
                    int64_t accu = ((bin << 23) + (bandsize << 22))
                                 * (int64_t)s->spx_dst_end_freq;
                    nratio = (int)(accu >> 32);
                    nratio -= spx_blend << 18;

                    if (nratio < 0) {
                        nblend = 0;
                        sblend = 0x800000;
                    } else if (nratio > 0x7fffff) {
                        nblend = 14529495; // sqrt(3) in FP.23
                        sblend = 0;
                    } else {
                        nblend = fixed_sqrt(nratio, 23);
                        accu = (int64_t)nblend * 1859775393;
                        nblend = (int)((accu + (1<<29)) >> 30);
                        sblend = fixed_sqrt(0x800000 - nratio, 23);
                    }
#else
                    float spx_coord;

                    /* calculate blending factors */
                    nratio = ((float)((bin + (bandsize >> 1))) / s->spx_dst_end_freq) - spx_blend;
                    nratio = av_clipf(nratio, 0.0f, 1.0f);
                    nblend = sqrtf(3.0f * nratio); // noise is scaled by sqrt(3)
                                                   // to give unity variance
                    sblend = sqrtf(1.0f - nratio);
#endif
                    bin += bandsize;

                    /* decode spx coordinates */
                    spx_coord_exp  = get_bits(bc, 4);
                    spx_coord_mant = get_bits(bc, 2);
                    if (spx_coord_exp == 15) spx_coord_mant <<= 1;
                    else                     spx_coord_mant += 4;
                    spx_coord_mant <<= (25 - spx_coord_exp - master_spx_coord);

                    /* multiply noise and signal blending factors by spx coordinate */
#if USE_FIXED
                    accu = (int64_t)nblend * spx_coord_mant;
                    s->spx_noise_blend[ch][bnd]  = (int)((accu + (1<<22)) >> 23);
                    accu = (int64_t)sblend * spx_coord_mant;
                    s->spx_signal_blend[ch][bnd] = (int)((accu + (1<<22)) >> 23);
#else
                    spx_coord = spx_coord_mant * (1.0f / (1 << 23));
                    s->spx_noise_blend [ch][bnd] = nblend * spx_coord;
                    s->spx_signal_blend[ch][bnd] = sblend * spx_coord;
#endif
                }
            }
        } else {
            s->first_spx_coords[ch] = 1;
        }
    }
}

static inline int coupling_strategy(AC3DecodeContext *s, int blk,
                                    uint8_t *bit_alloc_stages)
{
    GetBitContext *bc = &s->gbc;
    int fbw_channels = s->fbw_channels;
    int channel_mode = s->channel_mode;
    int ch;

    memset(bit_alloc_stages, 3, AC3_MAX_CHANNELS);
    if (!s->eac3)
        s->cpl_in_use[blk] = get_bits1(bc);
    if (s->cpl_in_use[blk]) {
        /* coupling in use */
        int cpl_start_subband, cpl_end_subband;

        if (channel_mode < AC3_CHMODE_STEREO) {
            av_log(s->avctx, AV_LOG_ERROR, "coupling not allowed in mono or dual-mono\n");
            return AVERROR_INVALIDDATA;
        }

        /* check for enhanced coupling */
        if (s->eac3 && get_bits1(bc)) {
            /* TODO: parse enhanced coupling strategy info */
            avpriv_request_sample(s->avctx, "Enhanced coupling");
            return AVERROR_PATCHWELCOME;
        }

        /* determine which channels are coupled */
        if (s->eac3 && s->channel_mode == AC3_CHMODE_STEREO) {
            s->channel_in_cpl[1] = 1;
            s->channel_in_cpl[2] = 1;
        } else {
            for (ch = 1; ch <= fbw_channels; ch++)
                s->channel_in_cpl[ch] = get_bits1(bc);
        }

        /* phase flags in use */
        if (channel_mode == AC3_CHMODE_STEREO)
            s->phase_flags_in_use = get_bits1(bc);

        /* coupling frequency range */
        cpl_start_subband = get_bits(bc, 4);
        cpl_end_subband = s->spx_in_use ? (s->spx_src_start_freq - 37) / 12 :
                                          get_bits(bc, 4) + 3;
        if (cpl_start_subband >= cpl_end_subband) {
            av_log(s->avctx, AV_LOG_ERROR, "invalid coupling range (%d >= %d)\n",
                   cpl_start_subband, cpl_end_subband);
            return AVERROR_INVALIDDATA;
        }
        s->start_freq[CPL_CH] = cpl_start_subband * 12 + 37;
        s->end_freq[CPL_CH]   = cpl_end_subband   * 12 + 37;

        decode_band_structure(bc, blk, s->eac3, 0, cpl_start_subband,
                              cpl_end_subband,
                              ff_eac3_default_cpl_band_struct,
                              &s->num_cpl_bands, s->cpl_band_sizes,
                              s->cpl_band_struct, sizeof(s->cpl_band_struct));
    } else {
        /* coupling not in use */
        for (ch = 1; ch <= fbw_channels; ch++) {
            s->channel_in_cpl[ch] = 0;
            s->first_cpl_coords[ch] = 1;
        }
        s->first_cpl_leak = s->eac3;
        s->phase_flags_in_use = 0;
    }

    return 0;
}

static inline int coupling_coordinates(AC3DecodeContext *s, int blk)
{
    GetBitContext *bc = &s->gbc;
    int fbw_channels = s->fbw_channels;
    int ch, bnd;
    int cpl_coords_exist = 0;

    for (ch = 1; ch <= fbw_channels; ch++) {
        if (s->channel_in_cpl[ch]) {
            if ((s->eac3 && s->first_cpl_coords[ch]) || get_bits1(bc)) {
                int master_cpl_coord, cpl_coord_exp, cpl_coord_mant;
                s->first_cpl_coords[ch] = 0;
                cpl_coords_exist = 1;
                master_cpl_coord = 3 * get_bits(bc, 2);
                for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
                    cpl_coord_exp  = get_bits(bc, 4);
                    cpl_coord_mant = get_bits(bc, 4);
                    if (cpl_coord_exp == 15)
                        s->cpl_coords[ch][bnd] = cpl_coord_mant << 22;
                    else
                        s->cpl_coords[ch][bnd] = (cpl_coord_mant + 16) << 21;
                    s->cpl_coords[ch][bnd] >>= (cpl_coord_exp + master_cpl_coord);
                }
            } else if (!blk) {
                av_log(s->avctx, AV_LOG_ERROR, "new coupling coordinates must "
                       "be present in block 0\n");
                return AVERROR_INVALIDDATA;
            }
        } else {
            /* channel not in coupling */
            s->first_cpl_coords[ch] = 1;
        }
    }
    /* phase flags */
    if (s->channel_mode == AC3_CHMODE_STEREO && cpl_coords_exist) {
        for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
            s->phase_flags[bnd] = s->phase_flags_in_use ? get_bits1(bc) : 0;
        }
    }

    return 0;
}

/**
 * Decode a single audio block from the AC-3 bitstream.
 */
static int decode_audio_block(AC3DecodeContext *s, int blk, int offset)
{
    int fbw_channels = s->fbw_channels;
    int channel_mode = s->channel_mode;
    int i, bnd, seg, ch, ret;
    int different_transforms;
    int downmix_output;
    int cpl_in_use;
    GetBitContext *gbc = &s->gbc;
    uint8_t bit_alloc_stages[AC3_MAX_CHANNELS] = { 0 };

    /* block switch flags */
    different_transforms = 0;
    if (s->block_switch_syntax) {
        for (ch = 1; ch <= fbw_channels; ch++) {
            s->block_switch[ch] = get_bits1(gbc);
            if (ch > 1 && s->block_switch[ch] != s->block_switch[1])
                different_transforms = 1;
        }
    }

    /* dithering flags */
    if (s->dither_flag_syntax) {
        for (ch = 1; ch <= fbw_channels; ch++) {
            s->dither_flag[ch] = get_bits1(gbc);
        }
    }

    /* dynamic range */
    i = !s->channel_mode;
    do {
        if (get_bits1(gbc)) {
            /* Allow asymmetric application of DRC when drc_scale > 1.
               Amplification of quiet sounds is enhanced */
            int range_bits = get_bits(gbc, 8);
            INTFLOAT range = AC3_RANGE(range_bits);
            if (range_bits <= 127 || s->drc_scale <= 1.0)
                s->dynamic_range[i] = AC3_DYNAMIC_RANGE(range);
            else
                s->dynamic_range[i] = range;
        } else if (blk == 0) {
            s->dynamic_range[i] = AC3_DYNAMIC_RANGE1;
        }
    } while (i--);

    /* spectral extension strategy */
    if (s->eac3 && (!blk || get_bits1(gbc))) {
        s->spx_in_use = get_bits1(gbc);
        if (s->spx_in_use) {
            if ((ret = spx_strategy(s, blk)) < 0)
                return ret;
        }
    }
    if (!s->eac3 || !s->spx_in_use) {
        s->spx_in_use = 0;
        for (ch = 1; ch <= fbw_channels; ch++) {
            s->channel_uses_spx[ch] = 0;
            s->first_spx_coords[ch] = 1;
        }
    }

    /* spectral extension coordinates */
    if (s->spx_in_use)
        spx_coordinates(s);

    /* coupling strategy */
    if (s->eac3 ? s->cpl_strategy_exists[blk] : get_bits1(gbc)) {
        if ((ret = coupling_strategy(s, blk, bit_alloc_stages)) < 0)
            return ret;
    } else if (!s->eac3) {
        if (!blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new coupling strategy must "
                   "be present in block 0\n");
            return AVERROR_INVALIDDATA;
        } else {
            s->cpl_in_use[blk] = s->cpl_in_use[blk-1];
        }
    }
    cpl_in_use = s->cpl_in_use[blk];

    /* coupling coordinates */
    if (cpl_in_use) {
        if ((ret = coupling_coordinates(s, blk)) < 0)
            return ret;
    }

    /* stereo rematrixing strategy and band structure */
    if (channel_mode == AC3_CHMODE_STEREO) {
        if ((s->eac3 && !blk) || get_bits1(gbc)) {
            s->num_rematrixing_bands = 4;
            if (cpl_in_use && s->start_freq[CPL_CH] <= 61) {
                s->num_rematrixing_bands -= 1 + (s->start_freq[CPL_CH] == 37);
            } else if (s->spx_in_use && s->spx_src_start_freq <= 61) {
                s->num_rematrixing_bands--;
            }
            for (bnd = 0; bnd < s->num_rematrixing_bands; bnd++)
                s->rematrixing_flags[bnd] = get_bits1(gbc);
        } else if (!blk) {
            av_log(s->avctx, AV_LOG_WARNING, "Warning: "
                   "new rematrixing strategy not present in block 0\n");
            s->num_rematrixing_bands = 0;
        }
    }

    /* exponent strategies for each channel */
    for (ch = !cpl_in_use; ch <= s->channels; ch++) {
        if (!s->eac3)
            s->exp_strategy[blk][ch] = get_bits(gbc, 2 - (ch == s->lfe_ch));
        if (s->exp_strategy[blk][ch] != EXP_REUSE)
            bit_alloc_stages[ch] = 3;
    }

    /* channel bandwidth */
    for (ch = 1; ch <= fbw_channels; ch++) {
        s->start_freq[ch] = 0;
        if (s->exp_strategy[blk][ch] != EXP_REUSE) {
            int group_size;
            int prev = s->end_freq[ch];
            if (s->channel_in_cpl[ch])
                s->end_freq[ch] = s->start_freq[CPL_CH];
            else if (s->channel_uses_spx[ch])
                s->end_freq[ch] = s->spx_src_start_freq;
            else {
                int bandwidth_code = get_bits(gbc, 6);
                if (bandwidth_code > 60) {
                    av_log(s->avctx, AV_LOG_ERROR, "bandwidth code = %d > 60\n", bandwidth_code);
                    return AVERROR_INVALIDDATA;
                }
                s->end_freq[ch] = bandwidth_code * 3 + 73;
            }
            group_size = 3 << (s->exp_strategy[blk][ch] - 1);
            s->num_exp_groups[ch] = (s->end_freq[ch] + group_size-4) / group_size;
            if (blk > 0 && s->end_freq[ch] != prev)
                memset(bit_alloc_stages, 3, AC3_MAX_CHANNELS);
        }
    }
    if (cpl_in_use && s->exp_strategy[blk][CPL_CH] != EXP_REUSE) {
        s->num_exp_groups[CPL_CH] = (s->end_freq[CPL_CH] - s->start_freq[CPL_CH]) /
                                    (3 << (s->exp_strategy[blk][CPL_CH] - 1));
    }

    /* decode exponents for each channel */
    for (ch = !cpl_in_use; ch <= s->channels; ch++) {
        if (s->exp_strategy[blk][ch] != EXP_REUSE) {
            s->dexps[ch][0] = get_bits(gbc, 4) << !ch;
            if (decode_exponents(s, gbc, s->exp_strategy[blk][ch],
                                 s->num_exp_groups[ch], s->dexps[ch][0],
                                 &s->dexps[ch][s->start_freq[ch]+!!ch])) {
                return AVERROR_INVALIDDATA;
            }
            if (ch != CPL_CH && ch != s->lfe_ch)
                skip_bits(gbc, 2); /* skip gainrng */
        }
    }

    /* bit allocation information */
    if (s->bit_allocation_syntax) {
        if (get_bits1(gbc)) {
            s->bit_alloc_params.slow_decay = ff_ac3_slow_decay_tab[get_bits(gbc, 2)] >> s->bit_alloc_params.sr_shift;
            s->bit_alloc_params.fast_decay = ff_ac3_fast_decay_tab[get_bits(gbc, 2)] >> s->bit_alloc_params.sr_shift;
            s->bit_alloc_params.slow_gain  = ff_ac3_slow_gain_tab[get_bits(gbc, 2)];
            s->bit_alloc_params.db_per_bit = ff_ac3_db_per_bit_tab[get_bits(gbc, 2)];
            s->bit_alloc_params.floor  = ff_ac3_floor_tab[get_bits(gbc, 3)];
            for (ch = !cpl_in_use; ch <= s->channels; ch++)
                bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
        } else if (!blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new bit allocation info must "
                   "be present in block 0\n");
            return AVERROR_INVALIDDATA;
        }
    }

    /* signal-to-noise ratio offsets and fast gains (signal-to-mask ratios) */
    if (!s->eac3 || !blk) {
        if (s->snr_offset_strategy && get_bits1(gbc)) {
            int snr = 0;
            int csnr;
            csnr = (get_bits(gbc, 6) - 15) << 4;
            for (i = ch = !cpl_in_use; ch <= s->channels; ch++) {
                /* snr offset */
                if (ch == i || s->snr_offset_strategy == 2)
                    snr = (csnr + get_bits(gbc, 4)) << 2;
                /* run at least last bit allocation stage if snr offset changes */
                if (blk && s->snr_offset[ch] != snr) {
                    bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 1);
                }
                s->snr_offset[ch] = snr;

                /* fast gain (normal AC-3 only) */
                if (!s->eac3) {
                    int prev = s->fast_gain[ch];
                    s->fast_gain[ch] = ff_ac3_fast_gain_tab[get_bits(gbc, 3)];
                    /* run last 2 bit allocation stages if fast gain changes */
                    if (blk && prev != s->fast_gain[ch])
                        bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
                }
            }
        } else if (!s->eac3 && !blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new snr offsets must be present in block 0\n");
            return AVERROR_INVALIDDATA;
        }
    }

    /* fast gain (E-AC-3 only) */
    if (s->fast_gain_syntax && get_bits1(gbc)) {
        for (ch = !cpl_in_use; ch <= s->channels; ch++) {
            int prev = s->fast_gain[ch];
            s->fast_gain[ch] = ff_ac3_fast_gain_tab[get_bits(gbc, 3)];
            /* run last 2 bit allocation stages if fast gain changes */
            if (blk && prev != s->fast_gain[ch])
                bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
        }
    } else if (s->eac3 && !blk) {
        for (ch = !cpl_in_use; ch <= s->channels; ch++)
            s->fast_gain[ch] = ff_ac3_fast_gain_tab[4];
    }

    /* E-AC-3 to AC-3 converter SNR offset */
    if (s->frame_type == EAC3_FRAME_TYPE_INDEPENDENT && get_bits1(gbc)) {
        skip_bits(gbc, 10); // skip converter snr offset
    }

    /* coupling leak information */
    if (cpl_in_use) {
        if (s->first_cpl_leak || get_bits1(gbc)) {
            int fl = get_bits(gbc, 3);
            int sl = get_bits(gbc, 3);
            /* run last 2 bit allocation stages for coupling channel if
               coupling leak changes */
            if (blk && (fl != s->bit_alloc_params.cpl_fast_leak ||
                sl != s->bit_alloc_params.cpl_slow_leak)) {
                bit_alloc_stages[CPL_CH] = FFMAX(bit_alloc_stages[CPL_CH], 2);
            }
            s->bit_alloc_params.cpl_fast_leak = fl;
            s->bit_alloc_params.cpl_slow_leak = sl;
        } else if (!s->eac3 && !blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new coupling leak info must "
                   "be present in block 0\n");
            return AVERROR_INVALIDDATA;
        }
        s->first_cpl_leak = 0;
    }

    /* delta bit allocation information */
    if (s->dba_syntax && get_bits1(gbc)) {
        /* delta bit allocation exists (strategy) */
        for (ch = !cpl_in_use; ch <= fbw_channels; ch++) {
            s->dba_mode[ch] = get_bits(gbc, 2);
            if (s->dba_mode[ch] == DBA_RESERVED) {
                av_log(s->avctx, AV_LOG_ERROR, "delta bit allocation strategy reserved\n");
                return AVERROR_INVALIDDATA;
            }
            bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
        }
        /* channel delta offset, len and bit allocation */
        for (ch = !cpl_in_use; ch <= fbw_channels; ch++) {
            if (s->dba_mode[ch] == DBA_NEW) {
                s->dba_nsegs[ch] = get_bits(gbc, 3) + 1;
                for (seg = 0; seg < s->dba_nsegs[ch]; seg++) {
                    s->dba_offsets[ch][seg] = get_bits(gbc, 5);
                    s->dba_lengths[ch][seg] = get_bits(gbc, 4);
                    s->dba_values[ch][seg]  = get_bits(gbc, 3);
                }
                /* run last 2 bit allocation stages if new dba values */
                bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
            }
        }
    } else if (blk == 0) {
        for (ch = 0; ch <= s->channels; ch++) {
            s->dba_mode[ch] = DBA_NONE;
        }
    }

    /* Bit allocation */
    for (ch = !cpl_in_use; ch <= s->channels; ch++) {
        if (bit_alloc_stages[ch] > 2) {
            /* Exponent mapping into PSD and PSD integration */
            ff_ac3_bit_alloc_calc_psd(s->dexps[ch],
                                      s->start_freq[ch], s->end_freq[ch],
                                      s->psd[ch], s->band_psd[ch]);
        }
        if (bit_alloc_stages[ch] > 1) {
            /* Compute excitation function, Compute masking curve, and
               Apply delta bit allocation */
            if (ff_ac3_bit_alloc_calc_mask(&s->bit_alloc_params, s->band_psd[ch],
                                           s->start_freq[ch],  s->end_freq[ch],
                                           s->fast_gain[ch],   (ch == s->lfe_ch),
                                           s->dba_mode[ch],    s->dba_nsegs[ch],
                                           s->dba_offsets[ch], s->dba_lengths[ch],
                                           s->dba_values[ch],  s->mask[ch])) {
                av_log(s->avctx, AV_LOG_ERROR, "error in bit allocation\n");
                return AVERROR_INVALIDDATA;
            }
        }
        if (bit_alloc_stages[ch] > 0) {
            /* Compute bit allocation */
            const uint8_t *bap_tab = s->channel_uses_aht[ch] ?
                                     ff_eac3_hebap_tab : ff_ac3_bap_tab;
            s->ac3dsp.bit_alloc_calc_bap(s->mask[ch], s->psd[ch],
                                      s->start_freq[ch], s->end_freq[ch],
                                      s->snr_offset[ch],
                                      s->bit_alloc_params.floor,
                                      bap_tab, s->bap[ch]);
        }
    }

    /* unused dummy data */
    if (s->skip_syntax && get_bits1(gbc)) {
        int skipl = get_bits(gbc, 9);
        skip_bits_long(gbc, 8 * skipl);
    }

    /* unpack the transform coefficients
       this also uncouples channels if coupling is in use. */
    decode_transform_coeffs(s, blk);

    /* TODO: generate enhanced coupling coordinates and uncouple */

    /* recover coefficients if rematrixing is in use */
    if (s->channel_mode == AC3_CHMODE_STEREO)
        do_rematrixing(s);

    /* apply scaling to coefficients (headroom, dynrng) */
    for (ch = 1; ch <= s->channels; ch++) {
        int audio_channel = 0;
        INTFLOAT gain;
        if (s->channel_mode == AC3_CHMODE_DUALMONO && ch <= 2)
            audio_channel = 2-ch;
        if (s->heavy_compression && s->compression_exists[audio_channel])
            gain = s->heavy_dynamic_range[audio_channel];
        else
            gain = s->dynamic_range[audio_channel];

#if USE_FIXED
        scale_coefs(s->transform_coeffs[ch], s->fixed_coeffs[ch], gain, 256);
#else
        if (s->target_level != 0)
          gain = gain * s->level_gain[audio_channel];
        gain *= 1.0 / 4194304.0f;
        s->fmt_conv.int32_to_float_fmul_scalar(s->transform_coeffs[ch],
                                               s->fixed_coeffs[ch], gain, 256);
#endif
    }

    /* apply spectral extension to high frequency bins */
    if (CONFIG_EAC3_DECODER && s->spx_in_use) {
        ff_eac3_apply_spectral_extension(s);
    }

    /* downmix and MDCT. order depends on whether block switching is used for
       any channel in this block. this is because coefficients for the long
       and short transforms cannot be mixed. */
    downmix_output = s->channels != s->out_channels &&
                     !((s->output_mode & AC3_OUTPUT_LFEON) &&
                     s->fbw_channels == s->out_channels);
    if (different_transforms) {
        /* the delay samples have already been downmixed, so we upmix the delay
           samples in order to reconstruct all channels before downmixing. */
        if (s->downmixed) {
            s->downmixed = 0;
            ac3_upmix_delay(s);
        }

        do_imdct(s, s->channels, offset);

        if (downmix_output) {
#if USE_FIXED
            ac3_downmix_c_fixed16(s->outptr, s->downmix_coeffs,
                              s->out_channels, s->fbw_channels, 256);
#else
            ff_ac3dsp_downmix(&s->ac3dsp, s->outptr, s->downmix_coeffs,
                              s->out_channels, s->fbw_channels, 256);
#endif
        }
    } else {
        if (downmix_output) {
            AC3_RENAME(ff_ac3dsp_downmix)(&s->ac3dsp, s->xcfptr + 1, s->downmix_coeffs,
                                          s->out_channels, s->fbw_channels, 256);
        }

        if (downmix_output && !s->downmixed) {
            s->downmixed = 1;
            AC3_RENAME(ff_ac3dsp_downmix)(&s->ac3dsp, s->dlyptr, s->downmix_coeffs,
                                          s->out_channels, s->fbw_channels, 128);
        }

        do_imdct(s, s->out_channels, offset);
    }

    return 0;
}

/**
 * Decode a single AC-3 frame.
 */
static int ac3_decode_frame(AVCodecContext *avctx, AVFrame *frame,
                            int *got_frame_ptr, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size, full_buf_size = avpkt->size;
    AC3DecodeContext *s = avctx->priv_data;
    int blk, ch, err, offset, ret;
    int i;
    int skip = 0, got_independent_frame = 0;
    const uint8_t *channel_map;
    uint8_t extended_channel_map[EAC3_MAX_CHANNELS];
    const SHORTFLOAT *output[AC3_MAX_CHANNELS];
    enum AVMatrixEncoding matrix_encoding;
    uint64_t mask;

    s->superframe_size = 0;

    buf_size = full_buf_size;
    i = ff_ac3_find_syncword(buf, buf_size);
    if (i < 0 || i > 10)
        return i;
    buf += i;
    buf_size -= i;

    /* copy input buffer to decoder context to avoid reading past the end
       of the buffer, which can be caused by a damaged input stream. */
    if (buf_size >= 2 && AV_RB16(buf) == 0x770B) {
        // seems to be byte-swapped AC-3
        int cnt = FFMIN(buf_size, AC3_FRAME_BUFFER_SIZE) >> 1;
        s->bdsp.bswap16_buf((uint16_t *) s->input_buffer,
                            (const uint16_t *) buf, cnt);
    } else
        memcpy(s->input_buffer, buf, FFMIN(buf_size, AC3_FRAME_BUFFER_SIZE));

    /* if consistent noise generation is enabled, seed the linear feedback generator
     * with the contents of the AC-3 frame so that the noise is identical across
     * decodes given the same AC-3 frame data, for use with non-linear edititing software. */
    if (s->consistent_noise_generation)
        av_lfg_init_from_data(&s->dith_state, s->input_buffer, FFMIN(buf_size, AC3_FRAME_BUFFER_SIZE));

    buf = s->input_buffer;
dependent_frame:
    /* initialize the GetBitContext with the start of valid AC-3 Frame */
    if ((ret = init_get_bits8(&s->gbc, buf, buf_size)) < 0)
        return ret;

    /* parse the syncinfo */
    err = parse_frame_header(s);

    if (err) {
        switch (err) {
        case AC3_PARSE_ERROR_SYNC:
            av_log(avctx, AV_LOG_ERROR, "frame sync error\n");
            return AVERROR_INVALIDDATA;
        case AC3_PARSE_ERROR_BSID:
            av_log(avctx, AV_LOG_ERROR, "invalid bitstream id\n");
            break;
        case AC3_PARSE_ERROR_SAMPLE_RATE:
            av_log(avctx, AV_LOG_ERROR, "invalid sample rate\n");
            break;
        case AC3_PARSE_ERROR_FRAME_SIZE:
            av_log(avctx, AV_LOG_ERROR, "invalid frame size\n");
            break;
        case AC3_PARSE_ERROR_FRAME_TYPE:
            /* skip frame if CRC is ok. otherwise use error concealment. */
            /* TODO: add support for substreams */
            if (s->substreamid) {
                av_log(avctx, AV_LOG_DEBUG,
                       "unsupported substream %d: skipping frame\n",
                       s->substreamid);
                *got_frame_ptr = 0;
                return buf_size;
            } else {
                av_log(avctx, AV_LOG_ERROR, "invalid frame type\n");
            }
            break;
        case AC3_PARSE_ERROR_CHANNEL_MAP:
            av_log(avctx, AV_LOG_ERROR, "invalid channel map\n");
            return AVERROR_INVALIDDATA;
        case AC3_PARSE_ERROR_CRC:
            break;
        default: // Normal AVERROR do not try to recover.
            *got_frame_ptr = 0;
            return err;
        }
    } else {
        /* check that reported frame size fits in input buffer */
        if (s->frame_size > buf_size) {
            av_log(avctx, AV_LOG_ERROR, "incomplete frame\n");
            err = AC3_PARSE_ERROR_FRAME_SIZE;
        } else if (avctx->err_recognition & (AV_EF_CRCCHECK|AV_EF_CAREFUL)) {
            /* check for crc mismatch */
            if (av_crc(av_crc_get_table(AV_CRC_16_ANSI), 0, &buf[2],
                       s->frame_size - 2)) {
                av_log(avctx, AV_LOG_ERROR, "frame CRC mismatch\n");
                if (avctx->err_recognition & AV_EF_EXPLODE)
                    return AVERROR_INVALIDDATA;
                err = AC3_PARSE_ERROR_CRC;
            }
        }
    }

    if (s->frame_type == EAC3_FRAME_TYPE_DEPENDENT && !got_independent_frame) {
        av_log(avctx, AV_LOG_WARNING, "Ignoring dependent frame without independent frame.\n");
        *got_frame_ptr = 0;
        return FFMIN(full_buf_size, s->frame_size);
    }

    /* channel config */
    if (!err || (s->channels && s->out_channels != s->channels)) {
        s->out_channels = s->channels;
        s->output_mode  = s->channel_mode;
        if (s->lfe_on)
            s->output_mode |= AC3_OUTPUT_LFEON;
        if (s->channels > 1 &&
            !av_channel_layout_compare(&s->downmix_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_MONO)) {
            s->out_channels = 1;
            s->output_mode  = AC3_CHMODE_MONO;
        } else if (s->channels > 2 &&
                   !av_channel_layout_compare(&s->downmix_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO)) {
            s->out_channels = 2;
            s->output_mode  = AC3_CHMODE_STEREO;
        }

        s->loro_center_mix_level   = ff_ac3_gain_levels[s->  center_mix_level];
        s->loro_surround_mix_level = ff_ac3_gain_levels[s->surround_mix_level];
        s->ltrt_center_mix_level   = ff_ac3_gain_levels[s->  center_mix_level_ltrt];
        s->ltrt_surround_mix_level = ff_ac3_gain_levels[s->surround_mix_level_ltrt];
        switch (s->preferred_downmix) {
        case AC3_DMIXMOD_LTRT:
            s->preferred_stereo_downmix = AV_DOWNMIX_TYPE_LTRT;
            break;
        case AC3_DMIXMOD_LORO:
            s->preferred_stereo_downmix = AV_DOWNMIX_TYPE_LORO;
            break;
        case AC3_DMIXMOD_DPLII:
            s->preferred_stereo_downmix = AV_DOWNMIX_TYPE_DPLII;
            break;
        default:
            s->preferred_stereo_downmix = AV_DOWNMIX_TYPE_UNKNOWN;
            break;
        }
        /* set downmixing coefficients if needed */
        if (s->channels != s->out_channels && !((s->output_mode & AC3_OUTPUT_LFEON) &&
                s->fbw_channels == s->out_channels)) {
            if ((ret = set_downmix_coeffs(s)) < 0) {
                av_log(avctx, AV_LOG_ERROR, "error setting downmix coeffs\n");
                return ret;
            }
        }
    } else if (!s->channels) {
        av_log(avctx, AV_LOG_ERROR, "unable to determine channel mode\n");
        return AVERROR_INVALIDDATA;
    }

    mask = ff_ac3_channel_layout_tab[s->output_mode & ~AC3_OUTPUT_LFEON];
    if (s->output_mode & AC3_OUTPUT_LFEON)
        mask |= AV_CH_LOW_FREQUENCY;

    av_channel_layout_uninit(&avctx->ch_layout);
    av_channel_layout_from_mask(&avctx->ch_layout, mask);

    /* set audio service type based on bitstream mode for AC-3 */
    avctx->audio_service_type = s->bitstream_mode;
    if (s->bitstream_mode == 0x7 && s->channels > 1)
        avctx->audio_service_type = AV_AUDIO_SERVICE_TYPE_KARAOKE;

    /* decode the audio blocks */
    channel_map = ff_ac3_dec_channel_map[s->output_mode & ~AC3_OUTPUT_LFEON][s->lfe_on];
    offset = s->frame_type == EAC3_FRAME_TYPE_DEPENDENT ? AC3_MAX_CHANNELS : 0;
    for (ch = 0; ch < AC3_MAX_CHANNELS; ch++) {
        output[ch] = s->output[ch + offset];
        s->outptr[ch] = s->output[ch + offset];
    }
    for (ch = 0; ch < s->channels; ch++) {
        if (ch < s->out_channels)
            s->outptr[channel_map[ch]] = s->output_buffer[ch + offset];
    }
    for (blk = 0; blk < s->num_blocks; blk++) {
        if (!err && decode_audio_block(s, blk, offset)) {
            av_log(avctx, AV_LOG_ERROR, "error decoding the audio block\n");
            err = 1;
        }
        if (err)
            for (ch = 0; ch < s->out_channels; ch++)
                memcpy(s->output_buffer[ch + offset] + AC3_BLOCK_SIZE*blk, output[ch], AC3_BLOCK_SIZE*sizeof(SHORTFLOAT));
        for (ch = 0; ch < s->out_channels; ch++)
            output[ch] = s->outptr[channel_map[ch]];
        for (ch = 0; ch < s->out_channels; ch++) {
            if (!ch || channel_map[ch])
                s->outptr[channel_map[ch]] += AC3_BLOCK_SIZE;
        }
    }

    /* keep last block for error concealment in next frame */
    for (ch = 0; ch < s->out_channels; ch++)
        memcpy(s->output[ch + offset], output[ch], AC3_BLOCK_SIZE*sizeof(SHORTFLOAT));

    /* check if there is dependent frame */
    if (buf_size > s->frame_size) {
        AC3HeaderInfo hdr;
        int err;

        if (buf_size - s->frame_size <= 16) {
            skip = buf_size - s->frame_size;
            goto skip;
        }

        if ((ret = init_get_bits8(&s->gbc, buf + s->frame_size, buf_size - s->frame_size)) < 0)
            return ret;

        err = ff_ac3_parse_header(&s->gbc, &hdr);
        if (err)
            return err;

        if (hdr.frame_type == EAC3_FRAME_TYPE_DEPENDENT) {
            if (hdr.num_blocks != s->num_blocks || s->sample_rate != hdr.sample_rate) {
                av_log(avctx, AV_LOG_WARNING, "Ignoring non-compatible dependent frame.\n");
            } else {
                buf += s->frame_size;
                buf_size -= s->frame_size;
                s->prev_output_mode = s->output_mode;
                s->prev_bit_rate = s->bit_rate;
                got_independent_frame = 1;
                goto dependent_frame;
            }
        }
    }
skip:

    frame->decode_error_flags = err ? FF_DECODE_ERROR_INVALID_BITSTREAM : 0;

    /* if frame is ok, set audio parameters */
    if (!err) {
        avctx->sample_rate = s->sample_rate;
        avctx->bit_rate    = s->bit_rate + s->prev_bit_rate;
        avctx->profile     = s->eac3_extension_type_a == 1 ? AV_PROFILE_EAC3_DDP_ATMOS : AV_PROFILE_UNKNOWN;
    }

    if (!avctx->sample_rate) {
        av_log(avctx, AV_LOG_ERROR, "Could not determine the sample rate\n");
        return AVERROR_INVALIDDATA;
    }

    for (ch = 0; ch < EAC3_MAX_CHANNELS; ch++)
        extended_channel_map[ch] = ch;

    if (s->frame_type == EAC3_FRAME_TYPE_DEPENDENT) {
        uint64_t ich_layout = ff_ac3_channel_layout_tab[s->prev_output_mode & ~AC3_OUTPUT_LFEON];
        int channel_map_size = ff_ac3_channels_tab[s->output_mode & ~AC3_OUTPUT_LFEON] + s->lfe_on;
        uint64_t channel_layout;
        int extend = 0;

        if (s->prev_output_mode & AC3_OUTPUT_LFEON)
            ich_layout |= AV_CH_LOW_FREQUENCY;

        channel_layout = ich_layout;
        for (ch = 0; ch < 16; ch++) {
            if (s->channel_map & (1 << (EAC3_MAX_CHANNELS - ch - 1))) {
                channel_layout |= ff_eac3_custom_channel_map_locations[ch][1];
            }
        }
        if (av_popcount64(channel_layout) > EAC3_MAX_CHANNELS) {
            av_log(avctx, AV_LOG_ERROR, "Too many channels (%d) coded\n",
                   av_popcount64(channel_layout));
            return AVERROR_INVALIDDATA;
        }

        av_channel_layout_uninit(&avctx->ch_layout);
        av_channel_layout_from_mask(&avctx->ch_layout, channel_layout);

        for (ch = 0; ch < EAC3_MAX_CHANNELS; ch++) {
            if (s->channel_map & (1 << (EAC3_MAX_CHANNELS - ch - 1))) {
                if (ff_eac3_custom_channel_map_locations[ch][0]) {
                    int index = av_channel_layout_index_from_channel(&avctx->ch_layout,
                                                                     ff_ctzll(ff_eac3_custom_channel_map_locations[ch][1]));
                    if (index < 0)
                        return AVERROR_INVALIDDATA;
                    if (extend >= channel_map_size)
                        break;

                    extended_channel_map[index] = offset + channel_map[extend++];
                } else {
                    int i;

                    for (i = 0; i < 64; i++) {
                        if ((1ULL << i) & ff_eac3_custom_channel_map_locations[ch][1]) {
                            int index = av_channel_layout_index_from_channel(&avctx->ch_layout, i);
                            if (index < 0)
                                return AVERROR_INVALIDDATA;
                            if (extend >= channel_map_size)
                                break;

                            extended_channel_map[index] = offset + channel_map[extend++];
                        }
                    }
                }
            }
        }

        ac3_downmix(avctx);
    }

    /* get output buffer */
    frame->nb_samples = s->num_blocks * AC3_BLOCK_SIZE;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    for (ch = 0; ch < avctx->ch_layout.nb_channels; ch++) {
        int map = extended_channel_map[ch];
        av_assert0(ch>=AV_NUM_DATA_POINTERS || frame->extended_data[ch] == frame->data[ch]);
        memcpy((SHORTFLOAT *)frame->extended_data[ch],
               s->output_buffer[map],
               s->num_blocks * AC3_BLOCK_SIZE * sizeof(SHORTFLOAT));
    }

    /*
     * AVMatrixEncoding
     *
     * Check whether the input layout is compatible, and make sure we're not
     * downmixing (else the matrix encoding is no longer applicable).
     */
    matrix_encoding = AV_MATRIX_ENCODING_NONE;
    if (s->channel_mode == AC3_CHMODE_STEREO &&
        s->channel_mode == (s->output_mode & ~AC3_OUTPUT_LFEON)) {
        if (s->dolby_surround_mode == AC3_DSURMOD_ON)
            matrix_encoding = AV_MATRIX_ENCODING_DOLBY;
        else if (s->dolby_headphone_mode == AC3_DHEADPHONMOD_ON)
            matrix_encoding = AV_MATRIX_ENCODING_DOLBYHEADPHONE;
    } else if (s->channel_mode >= AC3_CHMODE_2F2R &&
               s->channel_mode == (s->output_mode & ~AC3_OUTPUT_LFEON)) {
        switch (s->dolby_surround_ex_mode) {
        case AC3_DSUREXMOD_ON: // EX or PLIIx
            matrix_encoding = AV_MATRIX_ENCODING_DOLBYEX;
            break;
        case AC3_DSUREXMOD_PLIIZ:
            matrix_encoding = AV_MATRIX_ENCODING_DPLIIZ;
            break;
        default: // not indicated or off
            break;
        }
    }
    if (matrix_encoding != AV_MATRIX_ENCODING_NONE &&
        (ret = ff_side_data_update_matrix_encoding(frame, matrix_encoding)) < 0)
        return ret;

    /* AVDownmixInfo */
    if ( (s->channel_mode                     > AC3_CHMODE_STEREO) &&
        ((s->output_mode & ~AC3_OUTPUT_LFEON) > AC3_CHMODE_STEREO)) {
        AVDownmixInfo *downmix_info = av_downmix_info_update_side_data(frame);
        if (!downmix_info)
            return AVERROR(ENOMEM);
        switch (s->preferred_downmix) {
        case AC3_DMIXMOD_LTRT:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_LTRT;
            break;
        case AC3_DMIXMOD_LORO:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_LORO;
            break;
        case AC3_DMIXMOD_DPLII:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_DPLII;
            break;
        default:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_UNKNOWN;
            break;
        }
        downmix_info->center_mix_level        = ff_ac3_gain_levels[s->       center_mix_level];
        downmix_info->center_mix_level_ltrt   = ff_ac3_gain_levels[s->  center_mix_level_ltrt];
        downmix_info->surround_mix_level      = ff_ac3_gain_levels[s->     surround_mix_level];
        downmix_info->surround_mix_level_ltrt = ff_ac3_gain_levels[s->surround_mix_level_ltrt];
        if (s->lfe_mix_level_exists)
            downmix_info->lfe_mix_level       = ff_eac3_gain_levels_lfe[s->lfe_mix_level];
        else
            downmix_info->lfe_mix_level       = 0.0; // -inf dB
    }

    *got_frame_ptr = 1;

    if (!s->superframe_size)
        return FFMIN(full_buf_size, s->frame_size + skip);

    return FFMIN(full_buf_size, s->superframe_size + skip);
}

/**
 * Uninitialize the AC-3 decoder.
 */
static av_cold int ac3_decode_end(AVCodecContext *avctx)
{
    AC3DecodeContext *s = avctx->priv_data;
    av_tx_uninit(&s->tx_256);
    av_tx_uninit(&s->tx_128);
    av_freep(&s->fdsp);
    av_freep(&s->downmix_coeffs[0]);

    return 0;
}

#define OFFSET(x) offsetof(AC3DecodeContext, x)
#define PAR (AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM)
