/*
 * RealAudio 2.0 (28.8K)
 * Copyright (c) 2003 The FFmpeg project
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

#include "libavutil/channel_layout.h"
#include "libavutil/float_dsp.h"
#include "libavutil/internal.h"
#include "libavutil/mem.h"
#include "libavutil/mem_internal.h"

#define BITSTREAM_READER_LE
#include "libavcodec/avcodec.h"
#include "libavcodec/celp_filters.h"
#include "libavcodec/codec_internal.h"
#include "libavcodec/decode.h"
#include "libavcodec/get_bits.h"
#include "libavcodec/lpc_functions.h"
#include "libavcodec/ra288.h"

#define MAX_BACKWARD_FILTER_ORDER  36
#define MAX_BACKWARD_FILTER_LEN    40
#define MAX_BACKWARD_FILTER_NONREC 35
#define ATTEN 0.5625
#include "extra/g728_template.c"

#define RA288_BLOCK_SIZE        5
#define RA288_BLOCKS_PER_FRAME 32

typedef struct RA288Context {
    void (*vector_fmul)(float *dst, const float *src0, const float *src1,
                        int len);
    DECLARE_ALIGNED(32, float,   sp_lpc)[FFALIGN(36, 16)];   ///< LPC coefficients for speech data (spec: A)
    DECLARE_ALIGNED(32, float, gain_lpc)[FFALIGN(10, 16)];   ///< LPC coefficients for gain        (spec: GB)

    /** speech data history                                      (spec: SB).
     *  Its first 70 coefficients are updated only at backward filtering.
     */
    float sp_hist[111];

    /// speech part of the gain autocorrelation                  (spec: REXP)
    float sp_rec[37];

    /** log-gain history                                         (spec: SBLG).
     *  Its first 28 coefficients are updated only at backward filtering.
     */
    float gain_hist[38];

    /// recursive part of the gain autocorrelation               (spec: REXPLG)
    float gain_rec[11];
} RA288Context;

static av_cold int ra288_decode_init(AVCodecContext *avctx)
{
    RA288Context *ractx = avctx->priv_data;
    AVFloatDSPContext *fdsp;

    av_channel_layout_uninit(&avctx->ch_layout);
    avctx->ch_layout      = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
    avctx->sample_fmt     = AV_SAMPLE_FMT_FLT;

    if (avctx->block_align != 38) {
        av_log(avctx, AV_LOG_ERROR, "unsupported block align\n");
        return AVERROR_PATCHWELCOME;
    }

    fdsp = avpriv_float_dsp_alloc(avctx->flags & AV_CODEC_FLAG_BITEXACT);
    if (!fdsp)
        return AVERROR(ENOMEM);
    ractx->vector_fmul = fdsp->vector_fmul;
    av_free(fdsp);

    return 0;
}

static void decode(RA288Context *ractx, float gain, int cb_coef)
{
    int i;
    double sumsum;
    float sum, buffer[5];
    float *block = ractx->sp_hist + 70 + 36; // current block
    float *gain_block = ractx->gain_hist + 28;

    memmove(ractx->sp_hist + 70, ractx->sp_hist + 75, 36*sizeof(*block));

    /* block 46 of G.728 spec */
    sum = 32.0;
    for (i=0; i < 10; i++)
        sum -= gain_block[9-i] * ractx->gain_lpc[i];

    /* block 47 of G.728 spec */
    sum = av_clipf(sum, 0, 60);

    /* block 48 of G.728 spec */
    /* exp(sum * 0.1151292546497) == pow(10.0,sum/20) */
    sumsum = exp(sum * 0.1151292546497) * gain * (1.0/(1<<23));

    for (i=0; i < 5; i++)
        buffer[i] = codetable[cb_coef][i] * sumsum;

    sum = ff_scalarproduct_float_c(buffer, buffer, 5);

    sum = FFMAX(sum, 5.0 / (1<<24));

    /* shift and store */
    memmove(gain_block, gain_block + 1, 9 * sizeof(*gain_block));

    gain_block[9] = 10 * log10(sum) + (10*log10(((1<<24)/5.)) - 32);

    ff_celp_lp_synthesis_filterf(block, ractx->sp_lpc, buffer, 5, 36);
}

/**
 * Backward synthesis filter, find the LPC coefficients from past speech data.
 */
static void backward_filter(RA288Context *ractx,
                            float *hist, float *rec, const float *window,
                            float *lpc, const float *tab,
                            int order, int n, int non_rec, int move_size)
{
    float temp[MAX_BACKWARD_FILTER_ORDER+1];

    do_hybrid_window(ractx->vector_fmul, order, n, non_rec, temp, hist, rec, window);

    if (!compute_lpc_coefs(temp, 0, order, lpc, 0, 1, 1, NULL))
        ractx->vector_fmul(lpc, lpc, tab, FFALIGN(order, 16));

    memmove(hist, hist + n, move_size*sizeof(*hist));
}

static int ra288_decode_frame(AVCodecContext * avctx, AVFrame *frame,
                              int *got_frame_ptr, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    float *out;
    int i, ret;
    RA288Context *ractx = avctx->priv_data;
    GetBitContext gb;

    if (buf_size < avctx->block_align) {
        av_log(avctx, AV_LOG_ERROR,
               "Error! Input buffer is too small [%d<%d]\n",
               buf_size, avctx->block_align);
        return AVERROR_INVALIDDATA;
    }

    ret = init_get_bits8(&gb, buf, avctx->block_align);
    if (ret < 0)
        return ret;

    /* get output buffer */
    frame->nb_samples = RA288_BLOCK_SIZE * RA288_BLOCKS_PER_FRAME;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;
    out = (float *)frame->data[0];

    for (i=0; i < RA288_BLOCKS_PER_FRAME; i++) {
        float gain = amptable[get_bits(&gb, 3)];
        int cb_coef = get_bits(&gb, 6 + (i&1));

        decode(ractx, gain, cb_coef);

        memcpy(out, &ractx->sp_hist[70 + 36], RA288_BLOCK_SIZE * sizeof(*out));
        out += RA288_BLOCK_SIZE;

        if ((i & 7) == 3) {
            backward_filter(ractx, ractx->sp_hist, ractx->sp_rec, syn_window,
                            ractx->sp_lpc, syn_bw_tab, 36, 40, 35, 70);

            backward_filter(ractx, ractx->gain_hist, ractx->gain_rec, gain_window,
                            ractx->gain_lpc, gain_bw_tab, 10, 8, 20, 28);
        }
    }

    *got_frame_ptr = 1;

    return avctx->block_align;
}

const FFCodec ff_ra_288_decoder = {
    .p.name         = "real_288",
    CODEC_LONG_NAME("RealAudio 2.0 (28.8K)"),
    .p.type         = AVMEDIA_TYPE_AUDIO,
    .p.id           = AV_CODEC_ID_RA_288,
    .priv_data_size = sizeof(RA288Context),
    .init           = ra288_decode_init,
    FF_CODEC_DECODE_CB(ra288_decode_frame),
    .p.capabilities = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_CHANNEL_CONF,
};
