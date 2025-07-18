/*
 * FLV decoding.
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

#include "libavutil/imgutils.h"

#include "libavcodec/codec_internal.h"
#include "libavcodec/flvdec.h"
#include "libavcodec/h263dec.h"
#include "libavcodec/mpegvideo.h"
#include "libavcodec/mpegvideodec.h"

int ff_flv_decode_picture_header(H263DecContext *const h)
{
    int format, width, height;

    /* picture header */
    if (get_bits(&h->gb, 17) != 1) {
        av_log(h->c.avctx, AV_LOG_ERROR, "Bad picture start code\n");
        return AVERROR_INVALIDDATA;
    }
    format = get_bits(&h->gb, 5);
    if (format != 0 && format != 1) {
        av_log(h->c.avctx, AV_LOG_ERROR, "Bad picture format\n");
        return AVERROR_INVALIDDATA;
    }
    h->flv            = format;
    h->picture_number = get_bits(&h->gb, 8); /* picture timestamp */
    format            = get_bits(&h->gb, 3);
    switch (format) {
    case 0:
        width  = get_bits(&h->gb, 8);
        height = get_bits(&h->gb, 8);
        break;
    case 1:
        width  = get_bits(&h->gb, 16);
        height = get_bits(&h->gb, 16);
        break;
    case 2:
        width  = 352;
        height = 288;
        break;
    case 3:
        width  = 176;
        height = 144;
        break;
    case 4:
        width  = 128;
        height = 96;
        break;
    case 5:
        width  = 320;
        height = 240;
        break;
    case 6:
        width  = 160;
        height = 120;
        break;
    default:
        width = height = 0;
        break;
    }
    if (av_image_check_size(width, height, 0, h->c.avctx))
        return AVERROR(EINVAL);
    h->c.width  = width;
    h->c.height = height;

    h->c.pict_type = AV_PICTURE_TYPE_I + get_bits(&h->gb, 2);
    h->c.droppable = h->c.pict_type > AV_PICTURE_TYPE_P;
    if (h->c.droppable)
        h->c.pict_type = AV_PICTURE_TYPE_P;

    skip_bits1(&h->gb); /* deblocking flag */
    h->c.chroma_qscale = h->c.qscale = get_bits(&h->gb, 5);

    h->h263_long_vectors = 0;

    /* PEI */
    if (skip_1stop_8data_bits(&h->gb) < 0)
        return AVERROR_INVALIDDATA;

    if (h->ehc_mode)
        h->c.avctx->sample_aspect_ratio= (AVRational){1,2};

    if (h->c.avctx->debug & FF_DEBUG_PICT_INFO) {
        av_log(h->c.avctx, AV_LOG_DEBUG, "%c esc_type:%d, qp:%d num:%d\n",
               h->c.droppable ? 'D' : av_get_picture_type_char(h->c.pict_type),
               h->flv, h->c.qscale, h->picture_number);
    }

    return 0;
}

const FFCodec ff_flv_decoder = {
    .p.name         = "flv",
    CODEC_LONG_NAME("FLV / Sorenson Spark / Sorenson H.263 (Flash Video)"),
    .p.type         = AVMEDIA_TYPE_VIDEO,
    .p.id           = AV_CODEC_ID_FLV1,
    .priv_data_size = sizeof(H263DecContext),
    .init           = ff_h263_decode_init,
    FF_CODEC_DECODE_CB(ff_h263_decode_frame),
    .close          = ff_mpv_decode_close,
    .p.capabilities = AV_CODEC_CAP_DRAW_HORIZ_BAND | AV_CODEC_CAP_DR1,
    .caps_internal  = FF_CODEC_CAP_INIT_CLEANUP |
                      FF_CODEC_CAP_SKIP_FRAME_FILL_PARAM,
    .p.max_lowres   = 3,
};
