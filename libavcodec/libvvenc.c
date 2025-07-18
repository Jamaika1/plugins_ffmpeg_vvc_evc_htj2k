/*
 * H.266 encoding using the VVenC library
 *
 * Copyright (C) 2022, Thomas Siedel
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

#include "libvvenc/vvenc/vvenc.h"
#include "libvvenc/vvenc/vvencCfg.h"
#include "libvvenc/vvenc/version.h"

#include "libavutil/avassert.h" //
#include "libavutil/avstring.h"
#include "libavutil/avutil.h"
#include "libavutil/common.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include "libavutil/pixdesc.h"
#include "libavutil/opt.h"

#include "libavcodec/avcodec.h"
#include "libavcodec/codec_internal.h"
#include "libavcodec/encode.h"
#include "libavcodec/frame_thread_encoder.h" //
#include "libavcodec/internal.h"
#include "libavcodec/packet_internal.h"
#include "libavcodec/profiles.h"

#define VVENC_VERSION_INT  AV_VERSION_INT(VVENC_VERSION_MAJOR, \
                                          VVENC_VERSION_MINOR, \
                                          VVENC_VERSION_PATCH)

typedef struct VVenCContext {
    AVClass         *class;
    vvenc_config    params;
    vvencEncoder    *encoder;
    vvencAccessUnit *au;
    bool             encode_done;
    int   preset;
    int   qp;
    bool  qpa;
    char* stats;
    int   refreshsec;
    int   intraperiod;
    char* level;
    int   passes;
    int   pass;
    int   threads;
    int   tier;
    AVDictionary *vvenc_opts;
} VVenCContext;

static void vvenc_log_callback(void *ctx, int level,
                               const char *fmt, va_list args)
{
    vvenc_config params;
    vvencEncoder *encoder = ctx;
    if (encoder) {
        vvenc_config_default(&params);
        vvenc_get_config(encoder, &params);
        if ((int)params.m_verbosity >= level)
            vfprintf(level == 1 ? stderr : stdout, fmt, args);
    }
}

static void vvenc_set_verbository(vvenc_config *params)
{
    int loglevel = av_log_get_level();
    params->m_verbosity = VVENC_SILENT;
    if (loglevel >= AV_LOG_DEBUG)
        params->m_verbosity = VVENC_DETAILS;
    else if (loglevel >= AV_LOG_VERBOSE)
        params->m_verbosity = VVENC_NOTICE;
    else if (loglevel >= AV_LOG_INFO)
        params->m_verbosity = VVENC_WARNING;
}

static void vvenc_set_pic_format(AVCodecContext *avctx, vvenc_config *params)
{
    switch (avctx->pix_fmt) {
        case AV_PIX_FMT_GRAY10:
            params->m_internChromaFormat = VVENC_CHROMA_400;
            //params->m_useIdentityTableForNon420Chroma = false;
            break;
        case AV_PIX_FMT_YUV420P10:
            params->m_internChromaFormat = VVENC_CHROMA_420;
            //params->m_useIdentityTableForNon420Chroma = false;
            break;
        case AV_PIX_FMT_YUV422P10:
            params->m_internChromaFormat = VVENC_CHROMA_422;
            //params->m_useIdentityTableForNon420Chroma = true;
            //params->m_hrdParametersPresent = false;
            break;
        case AV_PIX_FMT_YUV444P10:
            params->m_internChromaFormat = VVENC_CHROMA_444;
            //params->m_useIdentityTableForNon420Chroma = true;
            //params->m_hrdParametersPresent = false;
            break;
    };
    //params->m_internChromaFormat = VVENC_CHROMA_420;
    params->m_inputBitDepth[0] = params->m_inputBitDepth[1] = params->m_inputBitDepth[2] = 10;
    params->m_internalBitDepth[0] = params->m_internalBitDepth[1] = params->m_internalBitDepth[2] = 10;
}

static void vvenc_set_color_format(AVCodecContext *avctx, vvenc_config *params)
{
    VVenCContext *s = avctx->priv_data;
    const AVFrame   *frame;
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    params->m_HdrMode = VVENC_HDR_OFF;
    if (avctx->color_primaries != AVCOL_PRI_UNSPECIFIED)
        params->m_colourPrimaries = (int) avctx->color_primaries;
    if (avctx->colorspace != AVCOL_SPC_UNSPECIFIED)
        params->m_matrixCoefficients = (int) avctx->colorspace;
    if (avctx->color_trc != AVCOL_TRC_UNSPECIFIED) {
        params->m_transferCharacteristics = (int) avctx->color_trc;

        if (avctx->color_primaries == AVCOL_PRI_BT2020) {
            if (avctx->color_trc == AVCOL_TRC_SMPTE2084)
                params->m_HdrMode = ((avctx->color_primaries == AVCOL_PRI_BT2020) ? VVENC_HDR_PQ_BT2020 : VVENC_HDR_PQ);
            else if (avctx->color_trc == AVCOL_TRC_BT2020_10 ||
                     avctx->colorspace == AVCOL_SPC_BT2020_NCL || avctx->colorspace == AVCOL_SPC_BT2020_CL)
                params->m_HdrMode = VVENC_SDR_BT2020;
            else if (avctx->color_trc == AVCOL_TRC_ARIB_STD_B67)
                params->m_HdrMode = ((avctx->color_primaries == AVCOL_PRI_BT2020) ? VVENC_HDR_HLG_BT2020 : VVENC_HDR_HLG);
        }
    }

    if (avctx->colorspace == AVCOL_SPC_BT709)
        params->m_HdrMode = VVENC_SDR_BT709;

    if (params->m_HdrMode == VVENC_HDR_OFF &&
        (avctx->color_primaries != AVCOL_PRI_UNSPECIFIED || avctx->colorspace != AVCOL_SPC_UNSPECIFIED)) {
        params->m_vuiParametersPresent = 1;
        params->m_colourDescriptionPresent = true;
    }

    if (avctx->color_range == AVCOL_RANGE_JPEG || frame->color_range == AVCOL_RANGE_JPEG)
        params->m_videoFullRangeFlag = true;
    else {
        params->m_videoFullRangeFlag = false;
    }
}

static void vvenc_set_framerate(AVCodecContext *avctx, vvenc_config *params)
{
    if (avctx->framerate.num > 0 && avctx->framerate.den > 0) {
        params->m_FrameRate = avctx->framerate.num;
        params->m_FrameScale = avctx->framerate.den;
    } else {
        params->m_FrameRate = avctx->time_base.den;
        params->m_FrameScale = avctx->time_base.num;
    }

    params->m_TicksPerSecond = -1;   /* auto mode for ticks per frame = 1 */
}

static int vvenc_parse_vvenc_params(AVCodecContext *avctx, vvenc_config *params)
{
    VVenCContext *s = avctx->priv_data;
    const AVDictionaryEntry *en = NULL;
    int parse_ret;
    int ret = 0;

    while ((en = av_dict_iterate(s->vvenc_opts, en))) {
        av_log(avctx, AV_LOG_DEBUG, "vvenc_set_param: '%s:%s'\n", en->key,
               en->value);
        parse_ret = vvenc_set_param(params, en->key, en->value);
        switch (parse_ret) {
        case VVENC_PARAM_BAD_NAME:
            av_log(avctx, AV_LOG_ERROR, "Unknown vvenc option: %s.\n", en->key);
            ret = AVERROR(EINVAL);
            break;
        case VVENC_PARAM_BAD_VALUE:
            av_log(avctx, AV_LOG_ERROR, "Invalid vvenc value for %s: %s.\n", en->key, en->value);
            ret = AVERROR(EINVAL);
            break;
        default:
            break;
        }
    }
    return ret;
}

static int vvenc_set_rc_mode(AVCodecContext *avctx, vvenc_config *params)
{
    VVenCContext *s = avctx->priv_data;
    if (!avctx->bit_rate) {
    av_log(avctx, AV_LOG_ERROR, "A bitrate must be set to use two pass mode.\n");
        return AVERROR(EINVAL);
    }

    if (s->pass == 1)
        s->pass = s->params.m_RCNumPasses;
    if ((avctx->flags & AV_CODEC_FLAG_PASS2 || s->pass == 2) && s->params.m_RCNumPasses == 2) {
        s->params.m_RCPass = 1; // <--first
        s->pass = 10;
    } else {
        if (s->pass == 10 && s->params.m_RCNumPasses == 2) {
            s->params.m_RCPass = 2;
        } else {
            s->params.m_RCPass = 1;
            s->pass = 10;
        }
    }

    if (avctx->rc_max_rate) {
#if VVENC_VERSION_INT >= AV_VERSION_INT(1,8,0)
        s->params.m_RCMaxBitrate = avctx->rc_max_rate;
#endif

#if VVENC_VERSION_INT < AV_VERSION_INT(1,11,0)
        /* rc_max_rate without a bit_rate enables capped CQF mode.
           (QP + subj. optimization + max. bitrate) */
        if (!avctx->bit_rate) {
            av_log(avctx, AV_LOG_ERROR, "Capped Constant Quality Factor mode (capped CQF) "
                   "needs at least vvenc version >= 1.11.0 (current version %s)\n", vvenc_get_version());
            return AVERROR(EINVAL);
        }
#endif
    }
    return 0;
}

static int vvenc_init_extradata(AVCodecContext *avctx, VVenCContext *s)
{
    int ret;
    if (avctx->flags & AV_CODEC_FLAG_GLOBAL_HEADER) {
        ret = vvenc_get_headers(s->encoder, s->au);
        if (0 != ret) {
            av_log(avctx, AV_LOG_ERROR, "cannot get (SPS,PPS) headers: %s\n",
                   vvenc_get_last_error(s->encoder));
            return AVERROR(EINVAL);
        }

        if (s->au->payloadUsedSize <= 0) {
            return AVERROR_INVALIDDATA;
        }

        avctx->extradata_size = s->au->payloadUsedSize;
        avctx->extradata = av_mallocz(avctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!avctx->extradata) {
            return AVERROR(ENOMEM);
        }

        memcpy(avctx->extradata, s->au->payload, avctx->extradata_size);
    }
    return 0;
}

static av_cold int vvenc_init(AVCodecContext *avctx)
{
    int ret;
    int framerate;
    VVenCContext *s = avctx->priv_data;
    vvencPresetMode preset = (vvencPresetMode) s->preset;

    if (avctx->flags & AV_CODEC_FLAG_INTERLACED_DCT) {
        av_log(avctx, AV_LOG_ERROR, "interlaced not supported\n");
        return AVERROR(EINVAL);
    }

    vvenc_config_default(&s->params);

    if (avctx->framerate.num > 0 && avctx->framerate.den > 0)
        framerate = avctx->framerate.num / avctx->framerate.den;
    else
        framerate = avctx->time_base.den / avctx->time_base.num;

    vvenc_init_default(&s->params, avctx->width, avctx->height, framerate,
                       avctx->bit_rate, s->qp, preset);

    vvenc_set_verbository(&s->params);

    s->params.m_numThreads = ((avctx->thread_count == 0 && s->threads == -1) ? -1 : ((s->threads != -1) ? s->threads : avctx->thread_count));
    s->params.m_GOPSize = (avctx->gop_size == 1) ? 1 : 32;
    s->params.m_IntraPeriod = ((avctx->gop_size == 1) ? 1 : s->intraperiod);
    s->params.m_IntraPeriodSec = s->refreshsec;

    /* GOP settings (IDR/CRA) */
    if (avctx->flags & AV_CODEC_FLAG_CLOSED_GOP)
        s->params.m_DecodingRefreshType = VVENC_DRT_IDR;

    /*if (avctx->gop_size == 1) {
        s->params.m_GOPSize = 1;
        s->params.m_IntraPeriod = 1;
    } else {
        s->params.m_IntraPeriodSec = s->refreshsec;
        s->params.m_IntraPeriod = s->intraperiot;
    }*/

    s->params.m_AccessUnitDelimiter = true;
    s->params.m_usePerceptQPA = s->qpa;
    s->params.m_levelTier     = (vvencTier) s->tier;

    if (avctx->level > 0)
        s->params.m_level = (vvencLevel)avctx->level;

    if (s->level) {
        if (VVENC_PARAM_BAD_VALUE == vvenc_set_param(&s->params, "level", s->level)) {
            av_log(avctx, AV_LOG_ERROR, "Invalid level_idc: %s.\n", s->level);
            return AVERROR(EINVAL);
        }
    }

    vvenc_set_framerate(avctx, &s->params);

    vvenc_set_pic_format(avctx, &s->params);

    vvenc_set_color_format(avctx, &s->params);

    ret = vvenc_parse_vvenc_params(avctx, &s->params);
    if (ret != 0)
        return ret;

    ret = vvenc_set_rc_mode(avctx, &s->params);
    if (ret != 0)
        return ret;

    s->encoder = vvenc_encoder_create();
    if (!s->encoder) {
        av_log(avctx, AV_LOG_ERROR, "cannot create libvvenc encoder\n");
        return AVERROR(ENOMEM);
    }

    vvenc_set_msg_callback(&s->params, s->encoder, vvenc_log_callback);
    ret = vvenc_encoder_open(s->encoder, &s->params);
    if (ret != 0) {
        av_log(avctx, AV_LOG_ERROR, "cannot open libvvenc encoder: %s\n",
               vvenc_get_last_error(s->encoder));
        return AVERROR_EXTERNAL;
    }

    vvenc_get_config(s->encoder, &s->params);     /* get the adapted config */

//pass2:
    av_log(avctx, AV_LOG_INFO, "libvvenc version: %s\n", vvenc_get_version());
    if (av_log_get_level() >= AV_LOG_VERBOSE)
        av_log(avctx, AV_LOG_INFO, "%s\n", vvenc_get_config_as_string(&s->params, s->params.m_verbosity));

    if (s->params.m_RCNumPasses == 2) {
        ret = vvenc_init_pass(s->encoder, s->params.m_RCPass - 1, "videopass.json");
        if (ret != 0) {
            av_log(avctx, AV_LOG_ERROR, "cannot init pass %d: %s\n",  s->params.m_RCPass,
                   vvenc_get_last_error(s->encoder));
            return AVERROR_EXTERNAL;
        }
    }

    /*if (s->params.m_RCPass == 1 && s->params.m_RCNumPasses == 2 ){
        s->params.m_RCPass = 2;
        goto pass2;
    }*/

    s->au = vvenc_accessUnit_alloc();
    if (!s->au) {
        av_log(avctx, AV_LOG_FATAL, "cannot allocate memory for AU payload\n");
        return AVERROR(ENOMEM);
    }
    vvenc_accessUnit_alloc_payload(s->au, avctx->width * avctx->height);
    if (!s->au->payload) {
        av_log(avctx, AV_LOG_FATAL, "cannot allocate payload memory of size %d\n",
               avctx->width * avctx->height);
        return AVERROR(ENOMEM);
    }

    ret = vvenc_init_extradata(avctx, s);
    if (ret != 0)
        return ret;

    s->encode_done = false;
    /*if (s->params.m_RCPass == 1 && s->params.m_RCNumPasses == 2 ){
        s->params.m_RCPass = 2;
        while (!s->encode_done) {
            ff_encode_encode_cb(avctx, pkt, frame, &got_packet);
        }
        //ff_encode_preinit(avctx);
        goto pass2;
    }*/
    return 0;
}

int vvenc_frame(AVCodecContext *avctx, AVPacket *pkt, const AVFrame *frame,
                               int *got_packet)
{
    VVenCContext *s = avctx->priv_data;
    vvencYUVBuffer *pyuvbuf;
    vvencYUVBuffer yuvbuf;
    int ret;

    pyuvbuf = NULL;
    if (frame) {
        vvenc_YUVBuffer_default(&yuvbuf);
        yuvbuf.planes[0].ptr = (int16_t *) frame->data[0];
        yuvbuf.planes[1].ptr = (int16_t *) frame->data[1];
        yuvbuf.planes[2].ptr = (int16_t *) frame->data[2];

        yuvbuf.planes[0].width  = frame->width;
        yuvbuf.planes[0].height = frame->height;
        yuvbuf.planes[0].stride = frame->linesize[0] >> 1; /* stride is used in 16bit samples in vvenc */

        yuvbuf.planes[1].width  = frame->width >> 1;
        yuvbuf.planes[1].height = frame->height >> 1;
        yuvbuf.planes[1].stride = frame->linesize[1] >> 1;

        yuvbuf.planes[2].width  = frame->width >> 1;
        yuvbuf.planes[2].height = frame->height >> 1;
        yuvbuf.planes[2].stride = frame->linesize[2] >> 1;

        yuvbuf.cts = frame->pts;
        yuvbuf.ctsValid = true;
        pyuvbuf = &yuvbuf;
    }

    if (!s->encode_done) {
        if (vvenc_encode(s->encoder, pyuvbuf, s->au, &s->encode_done) != 0)
            return AVERROR_EXTERNAL;
    } else
        return 0;

    if (s->au->payloadUsedSize > 0) {
        ret = ff_get_encode_buffer(avctx, pkt, s->au->payloadUsedSize, 0);
        if (ret < 0)
            return ret;

        memcpy(pkt->data, s->au->payload, s->au->payloadUsedSize);

        if (s->au->ctsValid)
            pkt->pts = s->au->cts;
        if (s->au->dtsValid)
            pkt->dts = s->au->dts;
        pkt->flags |= AV_PKT_FLAG_KEY * s->au->rap;

        *got_packet = 1;
        return 0;
    }

    return 0;
}

static av_cold int vvenc_close(AVCodecContext *avctx)
{
    VVenCContext *s = avctx->priv_data;

    if (s->au)
        vvenc_accessUnit_free_payload(s->au);

    if (s->encoder) {
        vvenc_print_summary(s->encoder);

        if (0 != vvenc_encoder_close(s->encoder))
            return AVERROR_EXTERNAL;
    }

    /*if (s->pass == 10 && s->params.m_RCNumPasses == 2 ){

    int ret;
    ret = vvenc_init(avctx);
    if (ret != 0)
        return ret;

    ret = avcodec_open2(avctx, avctx->codec, NULL);
    if (ret != 0)
        return ret;*/

    /*AVCodecInternal  *avci = avctx->internal;
    AVFrame          *frame = avci->in_frame;
    AVPacket         *pkt;
    const FFCodec *const codec = ffcodec(avctx->codec);
    int got_packet;

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    if (avci->draining_done)
        return AVERROR_EOF;

    if (!frame->buf[0] && !avci->draining) {
        av_frame_unref(frame);
        ret = ff_encode_get_frame(avctx, frame);
        if (ret < 0 && ret != AVERROR_EOF)
            return ret;
    }

    if (!frame->buf[0]) {
        if (!(avctx->codec->capabilities & AV_CODEC_CAP_DELAY ||
              avci->frame_thread_encoder))
            return AVERROR_EOF;

        // Flushing is signaled with a NULL frame
        frame = NULL;
    }

    got_packet = 0;

    av_assert0(codec->cb_type == FF_CODEC_CB_TYPE_ENCODE);

    if (CONFIG_FRAME_THREAD_ENCODER && avci->frame_thread_encoder)

        ret = ff_thread_video_encode_frame(avctx, pkt, frame, &got_packet);
    else {
        ret = ff_encode_encode_cb(avctx, pkt, frame, &got_packet);
    }

    if (avci->draining && !got_packet)
        avci->draining_done = 1;

    return ret;*/

        //if (av_codec_is_encoder(avctx->codec))
            //ret = ff_encode_preinit(avctx);
        /*ret = vvenc_init(avctx);
        if (ret != 0)
            return ret;

        pkt = av_packet_alloc();
        if (!pkt)
            exit(1);
        frame = av_frame_alloc();
        if (!frame) {
            fprintf(stderr, "Could not allocate video frame\n");
            exit(1);
        }

        while (ret >= 0) {
        ret = vvenc_frame(avctx, pkt, frame, &got_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return ret;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }
        }*/

        /*while (!s->encode_done) {
            ff_encode_encode_cb(avctx, pkt, frame, &got_packet);
            ff_encode_preinit
        }*/

        /*if (s->au)
            vvenc_accessUnit_free_payload(s->au);

        if (s->encoder) {
            vvenc_print_summary(s->encoder);

        if (0 != vvenc_encoder_close(s->encoder))
            return AVERROR_EXTERNAL;
        }
    }*/

    return 0;
}

static const enum AVPixelFormat pix_fmts_vvenc[] = {
    AV_PIX_FMT_YUV420P10,
    AV_PIX_FMT_YUV422P10,
    AV_PIX_FMT_YUV444P10,
    AV_PIX_FMT_NONE
};

#define OFFSET(x) offsetof(VVenCContext, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "preset",       "set encoding preset", OFFSET(preset), AV_OPT_TYPE_INT, {.i64 = 2}, 0, 4, VE, .unit = "preset"},
    { "faster",       "0", 0, AV_OPT_TYPE_CONST, {.i64 = VVENC_FASTER}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "fast",         "1", 0, AV_OPT_TYPE_CONST, {.i64 = VVENC_FAST},   INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "medium",       "2", 0, AV_OPT_TYPE_CONST, {.i64 = VVENC_MEDIUM}, INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "slow",         "3", 0, AV_OPT_TYPE_CONST, {.i64 = VVENC_SLOW},   INT_MIN, INT_MAX, VE, .unit = "preset" },
    { "slower",       "4", 0, AV_OPT_TYPE_CONST, {.i64 = VVENC_SLOWER}, INT_MIN, INT_MAX, VE, .unit = "preset" },

    { "qp",           "set quantization",          OFFSET(qp), AV_OPT_TYPE_INT,  {.i64 = 32}, -1, 63, VE },
    { "qpa",          "set subjective (perceptually motivated) optimization", OFFSET(qpa), AV_OPT_TYPE_BOOL, {.i64 = 1},  0, 1, VE },
    //{ "passlogfile",  "Filename for 2 pass stats", OFFSET(stats), AV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, VE},
    { "rcstatsfile",  "Filename for 2 pass stats", OFFSET(stats), AV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, VE},
    { "refreshsec",   "set (intra) refresh period in seconds", OFFSET(refreshsec), AV_OPT_TYPE_INT, {.i64 = 1},  1, INT_MAX, VE },
    { "intraperiod",  "intra period in frames (0: specify intra period in seconds instead, see 'refreshsec')", OFFSET(intraperiod), AV_OPT_TYPE_INT, {.i64 = 256}, 0, INT_MAX, VE },
    { "level",        "Specify level (as defined by Annex A)", OFFSET(level), AV_OPT_TYPE_STRING, {.str = "auto"}, 0, 0, VE },
    { "passes",       "number of rc passes (default: -1, if not set and bitrate > 0 2-pass rc will be used)", OFFSET(passes), AV_OPT_TYPE_INT,  {.i64 = 1}, -1, 2, VE },
    { "pass",         "current pass (0,1) for rc (only needed for 2-pass rc)", OFFSET(pass), AV_OPT_TYPE_INT, {.i64 = 1}, 0, 2, VE },

    { "threads",      "number of threads (multithreading; -1: resolution < 720p: 4, < 5K 2880p: 8, >= 5K 2880p: 12 threads)", OFFSET(threads), AV_OPT_TYPE_INT, {.i64 = -1}, -1, INT_MAX, VE, .unit = "threads" },
    { "auto",         "-1", 0, AV_OPT_TYPE_CONST, {.i64 = -1}, INT_MIN, INT_MAX, VE, .unit = "threads" },

    { "tier",         "set vvc tier", OFFSET(tier), AV_OPT_TYPE_INT, {.i64 = 0},  0, 1, VE, .unit = "tier" },
    { "main",         "0", 0, AV_OPT_TYPE_CONST, {.i64 = 0}, INT_MIN, INT_MAX, VE, .unit = "tier" },
    { "high",         "1", 0, AV_OPT_TYPE_CONST, {.i64 = 1}, INT_MIN, INT_MAX, VE, .unit = "tier" },

    { "vvenc-params", "set the vvenc configuration using a :-separated list of key=value parameters", OFFSET(vvenc_opts), AV_OPT_TYPE_DICT, { 0 }, 0, 0, VE },
    { NULL }
};

static const AVClass class = {
    .class_name = "libvvenc",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT
};

static const FFCodecDefault vvenc_defaults[] = {
    { "b", "0" },
    { "g", "-1" },
    { NULL }
};


const FFCodec ff_libvvenc_encoder = {
    .p.name         = "libvvenc",
    CODEC_LONG_NAME("libvvenc H.266 / VVC"),
    .p.type         = AVMEDIA_TYPE_VIDEO,
    .p.id           = AV_CODEC_ID_VVC,
    .p.capabilities = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_DELAY |
                      AV_CODEC_CAP_OTHER_THREADS,
    .p.profiles     = NULL_IF_CONFIG_SMALL(ff_vvc_profiles),
    .p.priv_class   = &class,
    .p.wrapper_name = "libvvenc",
    .priv_data_size = sizeof(VVenCContext),
    CODEC_PIXFMTS_ARRAY(pix_fmts_vvenc),
    .color_ranges   = AVCOL_RANGE_MPEG | AVCOL_RANGE_JPEG,
    .init           = vvenc_init,
    FF_CODEC_ENCODE_CB(vvenc_frame),
    .close          = vvenc_close,
    .defaults       = vvenc_defaults,
    .caps_internal  = FF_CODEC_CAP_INIT_CLEANUP | FF_CODEC_CAP_NOT_INIT_THREADSAFE |
                      FF_CODEC_CAP_AUTO_THREADS
};
