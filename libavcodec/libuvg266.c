/*
 * libuvg266 encoder
 *
 * Copyright (c) 2015 Tampere University of Technology
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

#include "libuvg266/uvg266.h"
#include "libuvg266/cfg.h"
#include <stdint.h>
#include <string.h>
//#include <stdbool.h>

#include "libavutil/attributes.h"
#include "libavutil/avassert.h"
#include "libavutil/dict.h"
#include "libavutil/error.h"
#include "libavutil/imgutils.h"
#include "libavutil/internal.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include "libavutil/pixdesc.h"
#include "libavutil/opt.h"

#include "avcodec.h"
#include "codec_internal.h"
#include "encode.h"
#include "packet_internal.h"

static void parse_qp_map(uvg_config *cfg, int index) {
  int i = 0;
  for (; cfg->chroma_scale_in[index][i] != -1; i++);
  if (cfg->chroma_scale_out[index][i] != -1) return;
  assert(i < 17);

  // TODO: Move this to somewhere else when we have more than one table
  cfg->num_used_table = 1;
  cfg->qp_table_length_minus1[index] = i - 2;
  cfg->qp_table_start_minus26[index] = cfg->chroma_scale_in[index][0] - 26;
  for (i = 0; i < cfg->qp_table_length_minus1[0] + 1; i++) {
    cfg->delta_qp_in_val_minus1[index][i] = cfg->chroma_scale_in[index][i + 1] - cfg->chroma_scale_in[index][i] - (int8_t)1;
    cfg->delta_qp_out_val[index][i] = cfg->chroma_scale_out[index][i + 1] - cfg->chroma_scale_out[index][i];
  }
}

  static const char * const preset_values[11][32*2] = {
      {
        "ultrafast",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-2",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "0",
        "sao", "off",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "28",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "superfast",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-2",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "2",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "28",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "veryfast",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-3",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "2",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "28",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "faster",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-3",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "fast",
        "rd", "0",
        "pu-depth-intra", "1-3",
        "pu-depth-inter", "1-3",
        "me", "hexbs",
        "gop", "8",
        "ref", "2",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "medium",
        "rd", "0",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "hexbs",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "on",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "slow",
        "rd", "1",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "hexbs",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "on",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "slower",
        "rd", "2",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "hexbs",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "1",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "off",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "veryslow",
        "rd", "2",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "tz",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "1",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "1",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "off",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "1",
        "dual-tree", "1",
        "jccr", "1",
        "mip", "1",
        "mrl", "1",
        "mts", "both",
        "dep-quant", "1",
        NULL
      },
      {
        "placebo",
        "rd", "2",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "tz",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "1",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "1",
        "mv-rdo", "1",
        "full-intra-search", "0",
        "cu-split-termination", "off",
        "me-early-termination", "off",
        "intra-rdo-et", "0",
        "early-skip", "0",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "1",
        "dual-tree", "1",
        "jccr", "1",
        "mip", "1",
        "mrl", "1",
        "mts", "both",
        "dep-quant", "1",
        NULL
      },
      { NULL }
  };

typedef struct Libuvg266Context {
    const AVClass *class;

    const uvg_api *api;
    uvg_encoder *encoder;
    uvg_config *config;

    int preset;
    char *uvg_params;
} Libuvg266Context;

static av_cold int libuvg266_init(AVCodecContext *avctx)
{
    Libuvg266Context *const ctx = avctx->priv_data;
    const uvg_api *const api = ctx->api = uvg_api_get(8);
    uvg_config *cfg = NULL;
    uvg_encoder *enc = NULL;

    /* UVG266 requires width and height to be multiples of eight. */
    if (avctx->width % 8 || avctx->height % 8) {
        av_log(avctx, AV_LOG_ERROR,
               "Video dimensions are not a multiple of 8 (%dx%d).\n",
               avctx->width, avctx->height);
        return AVERROR(ENOSYS);
    }

    ctx->config = cfg = api->config_alloc();
    if (!cfg) {
        av_log(avctx, AV_LOG_ERROR,
               "Could not allocate uvg266 config structure.\n");
        return AVERROR(ENOMEM);
    }

    if (!api->config_init(cfg)) {
        av_log(avctx, AV_LOG_ERROR,
               "Could not initialize uvg266 config structure.\n");
        return AVERROR_BUG;
    }

    //cfg->add_encoder_info = false;
    cfg->intra_period = 256;
    //cfg->open_gop = false;
    cfg->file_format = UVG_FORMAT_AUTO;
    cfg->width  = avctx->width;
    cfg->height = avctx->height;
    //cfg->threads = 1;

    cfg->input_format   = UVG_FORMAT_P420;
    cfg->input_bitdepth = av_pix_fmt_desc_get(avctx->pix_fmt)->comp[0].depth;
    Libuvg266Context *s = avctx->priv_data;
    int preset_line = s->preset;
    fprintf(stderr, "Using preset %s: ", (preset_line == 5) ? "medium" : "?");
    for (int preset_value = 1; preset_values[preset_line][preset_value] != NULL; preset_value += 2) {
      fprintf(stderr, "%s=%s ", preset_values[preset_line][preset_value], preset_values[preset_line][preset_value + 1]);
      uvg_config_parse(cfg, preset_values[preset_line][preset_value], preset_values[preset_line][preset_value + 1]);
    }
    fprintf(stderr, "\n");

    if (avctx->framerate.num > 0 && avctx->framerate.den > 0) {
        cfg->framerate_num   = avctx->framerate.num;
        cfg->framerate_denom = avctx->framerate.den;
    } else {
        cfg->framerate_num   = avctx->time_base.den;
        cfg->framerate_denom = avctx->time_base.num;
    }
    cfg->target_bitrate = avctx->bit_rate;
    /*if (avctx->pix_fmt == AV_PIX_FMT_GRAY8 ||
        avctx->pix_fmt == AV_PIX_FMT_GRAY10LE ||
        avctx->pix_fmt == AV_PIX_FMT_GRAY12LE) {
        cfg->input_format   = UVG_FORMAT_P400;
    } else if (avctx->pix_fmt == AV_PIX_FMT_YUV420P ||
               avctx->pix_fmt == AV_PIX_FMT_YUV420P10LE ||
               avctx->pix_fmt == AV_PIX_FMT_YUV420P12LE) {
        cfg->input_format   = UVG_FORMAT_P420;
    } else if (avctx->pix_fmt == AV_PIX_FMT_YUV422P ||
               avctx->pix_fmt == AV_PIX_FMT_YUV422P10LE ||
               avctx->pix_fmt == AV_PIX_FMT_YUV422P12LE) {
        cfg->input_format   = UVG_FORMAT_P422;
    } else if (avctx->pix_fmt == AV_PIX_FMT_YUV444P ||
               avctx->pix_fmt == AV_PIX_FMT_YUV444P10LE ||
               avctx->pix_fmt == AV_PIX_FMT_YUV444P12LE) {
        cfg->input_format   = UVG_FORMAT_P444;
    } else {
        return -1;
    }*/
    cfg->vui.sar_width  = avctx->sample_aspect_ratio.num;
    cfg->vui.sar_height = avctx->sample_aspect_ratio.den;
    if (avctx->bit_rate) {
        cfg->rc_algorithm = UVG_LAMBDA;
    }

    cfg->vui.fullrange   = avctx->color_range == AVCOL_RANGE_JPEG;
    cfg->vui.colorprim   = avctx->color_primaries;
    cfg->vui.transfer    = avctx->color_trc;
    cfg->vui.colormatrix = avctx->colorspace;
    if (avctx->chroma_sample_location != AVCHROMA_LOC_UNSPECIFIED)
        cfg->vui.chroma_loc = avctx->chroma_sample_location - 1;

    if (ctx->uvg_params) {
        AVDictionary *dict = NULL;
        if (!av_dict_parse_string(&dict, ctx->uvg_params, "=", ",", 0)) {
            const AVDictionaryEntry *entry = NULL;
            while ((entry = av_dict_iterate(dict, entry))) {
                if (!api->config_parse(cfg, entry->key, entry->value)) {
                    av_log(avctx, AV_LOG_WARNING, "Invalid option: %s=%s.\n",
                           entry->key, entry->value);
                }
            }
        }
        av_dict_free(&dict);
    }

    ctx->encoder = enc = api->encoder_open(cfg);
    if (!enc) {
        av_log(avctx, AV_LOG_ERROR, "Could not open uvg266 encoder.\n");
        return AVERROR_BUG;
    }

    if (avctx->flags & AV_CODEC_FLAG_GLOBAL_HEADER) {
        uvg_data_chunk *data_out = NULL;
        uvg_data_chunk *chunk = NULL;
        uint32_t len_out;
        uint8_t *p;

        if (!api->encoder_headers(enc, &data_out, &len_out))
            return AVERROR(ENOMEM);

        avctx->extradata = p = av_mallocz(len_out + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!p) {
            ctx->api->chunk_free(data_out);
            return AVERROR(ENOMEM);
        }

        avctx->extradata_size = len_out;

        for (chunk = data_out; chunk != NULL; chunk = chunk->next) {
            memcpy(p, chunk->data, chunk->len);
            p += chunk->len;
        }

        ctx->api->chunk_free(data_out);
    }

    return 0;
}

static av_cold int libuvg266_close(AVCodecContext *avctx)
{
    Libuvg266Context *ctx = avctx->priv_data;

    if (ctx->api) {
        ctx->api->encoder_close(ctx->encoder);
        ctx->api->config_destroy(ctx->config);
    }

    return 0;
}

static int libuvg266_encode(AVCodecContext *avctx,
                             AVPacket *avpkt,
                             const AVFrame *frame,
                             int *got_packet_ptr)
{
    Libuvg266Context *ctx = avctx->priv_data;
    uvg_picture *input_pic = NULL;
    uvg_picture *recon_pic = NULL;
    uvg_frame_info frame_info;
    uvg_data_chunk *data_out = NULL;
    uint32_t len_out = 0;
    int retval = 0;
    int pict_type;

    *got_packet_ptr = 0;

    if (frame) {
        if (frame->width != ctx->config->width ||
            frame->height != ctx->config->height) {
            av_log(avctx, AV_LOG_ERROR,
                   "Changing video dimensions during encoding is not supported. "
                   "(changed from %dx%d to %dx%d)\n",
                   ctx->config->width, ctx->config->height,
                   frame->width, frame->height);
            retval = AVERROR_INVALIDDATA;
            goto done;
        }

        if (frame->format != avctx->pix_fmt) {
            av_log(avctx, AV_LOG_ERROR,
                   "Changing pixel format during encoding is not supported. "
                   "(changed from %s to %s)\n",
                   av_get_pix_fmt_name(avctx->pix_fmt),
                   av_get_pix_fmt_name(frame->format));
            retval = AVERROR_INVALIDDATA;
            goto done;
        }

        // Allocate input picture for uvg266.
        input_pic = ctx->api->picture_alloc(frame->width, frame->height);
        if (!input_pic) {
            av_log(avctx, AV_LOG_ERROR, "Failed to allocate picture.\n");
            retval = AVERROR(ENOMEM);
            goto done;
        }

        // Copy pixels from frame to input_pic.
        //{
            uint8_t *dst[4] = {
                input_pic->data[0],
                input_pic->data[1],
                input_pic->data[2],
                NULL
            };
/*if (avctx->pix_fmt == AV_PIX_FMT_YUV420P ||
               avctx->pix_fmt == AV_PIX_FMT_YUV420P10LE ||
               avctx->pix_fmt == AV_PIX_FMT_YUV420P12LE) {*/
            int dst_linesizes[4] = {
                frame->width,
                frame->width / 2,
                frame->width / 2,
                0
            };
            av_image_copy2(dst, dst_linesizes,
                           frame->data, frame->linesize,
                           frame->format, frame->width, frame->height);
/*} else if (avctx->pix_fmt == AV_PIX_FMT_YUV422P ||
               avctx->pix_fmt == AV_PIX_FMT_YUV422P10LE ||
               avctx->pix_fmt == AV_PIX_FMT_YUV422P12LE) {
            int dst_linesizes[4] = {
              frame->width,
              frame->width / 2,
              frame->width,
              0
            };
            av_image_copy2((uint8_t* const*)dst, dst_linesizes,
                           frame->data, frame->linesize,
                           frame->format, frame->width, frame->height);
} else if (avctx->pix_fmt == AV_PIX_FMT_YUV444P ||
               avctx->pix_fmt == AV_PIX_FMT_YUV444P10LE ||
               avctx->pix_fmt == AV_PIX_FMT_YUV444P12LE) {
            int dst_linesizes[4] = {
              frame->width,
              frame->width,
              frame->width,
              0
            };
            av_image_copy2((uint8_t* const*)dst, dst_linesizes,
                           frame->data, frame->linesize,
                           frame->format, frame->width, frame->height);
}*/

            //if (av_pix_fmt_desc_get(avctx->pix_fmt)->comp[0].depth == 8) {
            //av_image_copy2((uint8_t* const*)dst, dst_linesizes,
                           //frame->data, frame->linesize,
                           //frame->format, frame->width, frame->height);
            /*} else {
            av_image_copy3(dst, dst_linesizes,
                           frame->data16, frame->linesize,
                           frame->format, frame->width, frame->height);
            }*/
        }

        input_pic->pts = (int64_t)frame->pts;
    //}

    retval = ctx->api->encoder_encode(ctx->encoder,
                                      input_pic,
                                      &data_out, &len_out,
                                      &recon_pic, NULL,
                                      &frame_info);
    if (!retval) {
        av_log(avctx, AV_LOG_ERROR, "Failed to encode frame.\n");
        retval = AVERROR_INVALIDDATA;
        goto done;
    } else
        retval = 0; /* uvg266 returns 1 on success */

    if (data_out) {
        uvg_data_chunk *chunk = NULL;
        uint64_t written = 0;

        retval = ff_get_encode_buffer(avctx, avpkt, len_out, 0);
        if (retval < 0) {
            av_log(avctx, AV_LOG_ERROR, "Failed to allocate output packet.\n");
            goto done;
        }

        for (chunk = data_out; chunk != NULL; chunk = chunk->next) {
            av_assert0(written + chunk->len <= len_out);
            memcpy(avpkt->data + written, chunk->data, chunk->len);
            written += chunk->len;
        }

        avpkt->pts = recon_pic->pts;
        avpkt->dts = recon_pic->dts;
        avpkt->flags = 0;
        // IRAP VCL NAL unit types span the range
        // [BLA_W_LP (16), RSV_IRAP_VCL23 (23)].
        if (frame_info.nal_unit_type == UVG_NAL_CRA_NUT ||
            frame_info.nal_unit_type == UVG_NAL_IDR_N_LP ||
            frame_info.nal_unit_type == UVG_NAL_IDR_W_RADL ||
            frame_info.nal_unit_type == UVG_NAL_GDR_NUT) {
            avpkt->flags |= AV_PKT_FLAG_KEY;
        }

        switch (frame_info.slice_type) {
        case UVG_SLICE_I:
            pict_type = AV_PICTURE_TYPE_I;
            avpkt->flags |= AV_PKT_FLAG_KEY;
            break;
        case UVG_SLICE_P:
            pict_type = AV_PICTURE_TYPE_P;
            avpkt->flags &= ~AV_PKT_FLAG_KEY;
            break;
        case UVG_SLICE_B:
            pict_type = AV_PICTURE_TYPE_B;
            avpkt->flags &= ~AV_PKT_FLAG_KEY;
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "Unknown picture type encountered.\n");
            return AVERROR_EXTERNAL;
        }

        ff_side_data_set_encoder_stats(avpkt, frame_info.qp * FF_QP2LAMBDA, NULL, 0, pict_type);

        *got_packet_ptr = 1;
    }

done:
    ctx->api->picture_free(input_pic);
    ctx->api->picture_free(recon_pic);
    ctx->api->chunk_free(data_out);
    return retval;
}

static const enum AVPixelFormat pix_fmts[] = {
//#if !defined(UVG_BIT_DEPTH) || UVG_BIT_DEPTH == 8
    AV_PIX_FMT_GRAY8,
    AV_PIX_FMT_YUV420P,
//#endif
    //AV_PIX_FMT_YUV422P,
    //AV_PIX_FMT_YUV444P,
//#if UVG_BIT_DEPTH == 10
    //AV_PIX_FMT_YUV420P10,
//#endif
    AV_PIX_FMT_NONE
};

#define OFFSET(x) offsetof(Libuvg266Context, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "preset",       "set encoding preset", OFFSET(preset), AV_OPT_TYPE_INT, {.i64 = 5}, 0, 11, VE, .unit = "preset"},
    { "ultrafast",    "0", 0, AV_OPT_TYPE_CONST, {.i64 = 0}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "superfast",    "1", 0, AV_OPT_TYPE_CONST, {.i64 = 1}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "faster",       "2", 0, AV_OPT_TYPE_CONST, {.i64 = 2}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "fast",         "3", 0, AV_OPT_TYPE_CONST, {.i64 = 3}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "medium",       "5", 0, AV_OPT_TYPE_CONST, {.i64 = 5}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "slow",         "6", 0, AV_OPT_TYPE_CONST, {.i64 = 6}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "slower",       "7", 0, AV_OPT_TYPE_CONST, {.i64 = 7}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "veryslow",     "8", 0, AV_OPT_TYPE_CONST, {.i64 = 8}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "placebo",      "9", 0, AV_OPT_TYPE_CONST, {.i64 = 9}, INT_MIN, INT_MAX, VE, .unit = "preset" },

    { "uvg266-params", "Set uvg266 parameters as a comma-separated list of key=value pairs.",
        OFFSET(uvg_params), AV_OPT_TYPE_STRING, { .str = NULL }, 0, 0, VE },
    { NULL }
};

static const AVClass class = {
    .class_name = "libuvg266",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT
};

static const FFCodecDefault defaults[] = {
    { "b", "0" },
    { NULL }
};

const FFCodec ff_libuvg266_encoder = {
    .p.name           = "libuvg266",
    CODEC_LONG_NAME("libuvg266 H.266 / VVC"),
    .p.type           = AVMEDIA_TYPE_VIDEO,
    .p.id             = AV_CODEC_ID_VVC,
    .p.capabilities   = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_DELAY |
                        AV_CODEC_CAP_OTHER_THREADS,
    CODEC_PIXFMTS_ARRAY(pix_fmts),
    .color_ranges     = AVCOL_RANGE_MPEG | AVCOL_RANGE_JPEG,

    .p.priv_class     = &class,
    .priv_data_size   = sizeof(Libuvg266Context),
    .defaults         = defaults,

    .init             = libuvg266_init,
    FF_CODEC_ENCODE_CB(libuvg266_encode),
    .close            = libuvg266_close,

    .caps_internal  = FF_CODEC_CAP_INIT_CLEANUP | FF_CODEC_CAP_NOT_INIT_THREADSAFE |
                      FF_CODEC_CAP_AUTO_THREADS,

    .p.wrapper_name   = "libuvg266"
};
