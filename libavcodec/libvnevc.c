/* The copyright in this software is being made available under the BSD
*  License, included below. This software may be subject to other third party
*  and contributor rights, including patent rights, and no such rights are
*  granted under this license.
*
*  Copyright (c) 2019-2020, ISO/IEC
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*   * Neither the name of the ISO/IEC nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
*  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <float.h>
#include <stdlib.h>

#include "libvnevc/evc.h"
#include "libvnevc/evc_def.h"

#include "libavutil/internal.h"
#include "libavutil/common.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/pixfmt.h"
#include "libavutil/time.h"
#include "libavutil/cpu.h"
#include "libavutil/avstring.h"

#include "libavcodec/avcodec.h"
#include "libavcodec/internal.h"
#include "libavcodec/packet_internal.h"
#include "libavcodec/codec_internal.h"
#include "libavcodec/profiles.h"
#include "libavcodec/encode.h"

#define MAX_BS_BUF (16*1024*1024)

/**
 * Error codes
 */
#define XEVE_PARAM_BAD_NAME -100
#define XEVE_PARAM_BAD_VALUE -200

/**
 * Encoder states
 *
 * STATE_ENCODING - the encoder receives and processes input frames
 * STATE_BUMPING  - there are no more input frames, however the encoder still processes previously received data
 */
typedef enum State {
    STATE_ENCODING,
    STATE_BUMPING,
} State;

/**
 * The structure stores all the states associated with the instance of V-Nova MPEG-5 EVC encoder
 */
typedef struct VNeveContext {
    const AVClass *class;

    EVCE id;            // XEVE instance identifier
    EVCE_CDSC cdsc;     // coding parameters i.e profile, width & height of input frame, num of therads, frame rate ...
    EVC_BITB bitb;     // bitstream buffer (output)
    EVCE_STAT stat;     // encoding status (output)
    EVC_IMGB imgb;     // image buffer (input)

    State state;        // encoder state (skipping, encoding, bumping)

    int profile_id;     // encoder profile (main, baseline)
    int preset_id;      // preset of vneve ( fast, medium, slow, placebo)
    //int tune_id;        // tune of vneve (psnr, zerolatency)

    // variables for rate control modes
    //int rc_mode;        // Rate control mode [ 0(CQP) / 1(ABR) / 2(CRF) ]
    int qp;             // quantization parameter (QP) [0,51]
    //int crf;            // constant rate factor (CRF) [10,49]

    int hash;           // embed picture signature (HASH) for conformance checking in decoding
    //int sei_info;       // embed Supplemental enhancement information while encoding

    int color_format;   // input data color format: currently only XEVE_CF_YCBCR420 is supported

    AVDictionary *vneve_params;
} VNeveContext;

/**
 * Convert FFmpeg pixel format (AVPixelFormat) to XEVE pre-defined color format
 *
 * @param[in]  av_pix_fmt pixel format (@see https://ffmpeg.org/doxygen/trunk/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5)
 * @param[out] vneve_col_fmt XEVE pre-defined color format (@see vneve.h)
 *
 * @return 0 on success, negative value on failure
 */
static int libvneve_color_fmt(enum AVPixelFormat av_pix_fmt, int *vneve_col_fmt)
{
    switch (av_pix_fmt) {
    case AV_PIX_FMT_YUV420P:
        *vneve_col_fmt = CHROMA_FORMAT_420;
        break;
    case AV_PIX_FMT_YUV420P10:
        *vneve_col_fmt = CHROMA_FORMAT_420;
        break;
    default:
        *vneve_col_fmt = EVC_COLORSPACE_UNKNOWN;
        return AVERROR_INVALIDDATA;
    }

    return 0;
}

/**
 * Convert FFmpeg pixel format (AVPixelFormat) into XEVE pre-defined color space
 *
 * @param[in] px_fmt pixel format (@see https://ffmpeg.org/doxygen/trunk/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5)
 *
 * @return XEVE pre-defined color space (@see vneve.h) on success, EVC_COLORSPACE_UNKNOWN on failure
 */
static int libvneve_color_space(enum AVPixelFormat av_pix_fmt)
{
    /* color space of input image */
    int cs = EVC_COLORSPACE_UNKNOWN;

    switch (av_pix_fmt) {
    case AV_PIX_FMT_YUV420P:
        cs = EVC_COLORSPACE_YUV420;
        break;
    case AV_PIX_FMT_YUV420P10:
#if AV_HAVE_BIGENDIAN
        cs = EVC_COLORSPACE_YUV420_10BE;
#else
        cs = EVC_COLORSPACE_YUV420_10LE;
#endif

        break;
    default:
        cs = EVC_COLORSPACE_UNKNOWN;
        break;
    }

    return cs;
}

/**
 * The function returns a pointer to the object of the EVCE_CDSC type.
 * EVCE_CDSC contains all encoder parameters that should be initialized before the encoder is used.
 *
 * The field values of the EVCE_CDSC structure are populated based on:
 * - the corresponding field values of the AvCodecConetxt structure,
 * - the vneve encoder specific option values,
 *   (the full list of options available for vneve encoder is displayed after executing the command ./ffmpeg --help encoder = libvneve)
 *
 * The order of processing input data and populating the EVCE_CDSC structure
 * 1) first, the fields of the AVCodecContext structure corresponding to the provided input options are processed,
 *    (i.e -pix_fmt yuv420p -s:v 1920x1080 -r 30 -profile:v 0)
 * 2) then vneve-specific options added as AVOption to the vneve AVCodec implementation
 *    (i.e -preset 0)
 *
 * Keep in mind that, there are options that can be set in different ways.
 * In this case, please follow the above-mentioned order of processing.
 * The most recent assignments overwrite the previous values.
 *
 * @param[in] avctx codec context (AVCodecContext)
 * @param[out] cdsc contains all V-Nova MPEG-5 EVC encoder encoder parameters that should be initialized before the encoder is use
 *
 * @return 0 on success, negative error code on failure
 */
static int get_conf(AVCodecContext *avctx, EVCE_CDSC *cdsc)
{
    VNeveContext *xectx = NULL;
    int ret;

    xectx = avctx->priv_data;

    /* initialize vneve_param struct with default values */
    /*ret = vneve_param_default(&cdsc->param);
    if (EVC_FAILED(ret)) {
        av_log(avctx, AV_LOG_ERROR, "Cannot set_default parameter\n");
        return AVERROR_EXTERNAL;
    }*/

    /* read options from AVCodecContext */
    if (avctx->width > 0)
        cdsc->w = avctx->width;

    if (avctx->height > 0)
        cdsc->h = avctx->height;

    if (avctx->framerate.num > 0) {
        // fps can be float number, but vneve API doesn't support it
        cdsc->fps = lrintf(av_q2d(avctx->framerate));
    }

    // GOP size (key-frame interval, I-picture period)
    cdsc->closed_gop = avctx->gop_size; // 0: only one I-frame at the first time; 1: every frame is coded in I-frame

    if (avctx->max_b_frames == 0 || avctx->max_b_frames == 1 || avctx->max_b_frames == 3 ||
        avctx->max_b_frames == 7 || avctx->max_b_frames == 15)   // number of b-frames
        cdsc->max_b_frames = avctx->max_b_frames;
    else {
        av_log(avctx, AV_LOG_ERROR, "Incorrect value for maximum number of B frames: (%d) \n"
               "Acceptable values for bf option (maximum number of B frames) are 0,1,3,7 or 15\n", avctx->max_b_frames);
        return AVERROR_INVALIDDATA;
    }

    cdsc->level = avctx->level;

    /*if (avctx->rc_buffer_size)   // VBV buf size
        cdsc->param.vbv_bufsize = (int)(avctx->rc_buffer_size / 1000);

    cdsc->param.rc_type = xectx->rc_mode;

    if (xectx->rc_mode == XEVE_RC_CQP)*/
        cdsc->qp = xectx->qp;
    /*else if (xectx->rc_mode == XEVE_RC_ABR) {
        if (avctx->bit_rate / 1000 > INT_MAX || avctx->rc_max_rate / 1000 > INT_MAX) {
            av_log(avctx, AV_LOG_ERROR, "Not supported bitrate bit_rate and rc_max_rate > %d000\n", INT_MAX);
            return AVERROR_INVALIDDATA;
        }
        cdsc->param.bitrate = (int)(avctx->bit_rate / 1000);
    } else if (xectx->rc_mode == XEVE_RC_CRF)
        cdsc->param.crf = xectx->crf;
    else {
        av_log(avctx, AV_LOG_ERROR, "Not supported rate control type: %d\n", xectx->rc_mode);
        return AVERROR_INVALIDDATA;
    }

    if (avctx->thread_count <= 0) {
        int cpu_count = av_cpu_count();
        cdsc->param.threads = (cpu_count < XEVE_MAX_THREADS) ? cpu_count : XEVE_MAX_THREADS;
    } else if (avctx->thread_count > XEVE_MAX_THREADS)
        cdsc->param.threads = XEVE_MAX_THREADS;
    else
        cdsc->param.threads = avctx->thread_count;*/


    libvneve_color_fmt(avctx->pix_fmt, &xectx->color_format);

    cdsc->cs = CS_FROM_BD_420(cdsc->codec_bit_depth);

    //cdsc->max_bs_buf_size = MAX_BS_BUF;

    /*ret = vneve_param_ppt(&cdsc->param, xectx->profile_id, xectx->preset_id, xectx->tune_id);
    if (EVC_FAILED(ret)) {
        av_log(avctx, AV_LOG_ERROR, "Cannot set profile(%d), preset(%d), tune(%d)\n", xectx->profile_id, xectx->preset_id, xectx->tune_id);
        return AVERROR_EXTERNAL;
    }*/

    return 0;
}

/**
 * Set XEVE_CFG_SET_USE_PIC_SIGNATURE for encoder
 *
 * @param[in] logger context
 * @param[in] id XEVE encodec instance identifier
 * @param[in] ctx the structure stores all the states associated with the instance of V-Nova MPEG-5 EVC encoder
 *
 * @return 0 on success, negative error code on failure
 */
static int set_extra_config(AVCodecContext *avctx, EVCE id, VNeveContext *ctx)
{
    int ret, size;
    size = 4;

    // embed SEI messages identifying encoder parameters and command line arguments
    // - 0: off\n"
    // - 1: emit sei info"
    //
    // SEI - Supplemental enhancement information contains information
    // that is not necessary to decode the samples of coded pictures from VCL NAL units.
    // Some SEI message information is required to check bitstream conformance
    // and for output timing decoder conformance.
    // @see ISO_IEC_23094-1_2020 7.4.3.5
    // @see ISO_IEC_23094-1_2020 Annex D
    /*ret = evce_config(id, EVCE_CFG_SET_SEI_CMD, &ctx->sei_info, &size); // sei_cmd_info
    if (EVC_FAILED(ret)) {
        av_log(avctx, AV_LOG_ERROR, "Failed to set config for sei command info messages\n");
        return AVERROR_EXTERNAL;
    }*/

    ret = evce_config(id, EVCE_CFG_SET_USE_PIC_SIGNATURE, &ctx->hash, &size);
    if (EVC_FAILED(ret)) {
        av_log(avctx, AV_LOG_ERROR, "Failed to set config for picture signature\n");
        return AVERROR_EXTERNAL;
    }

    return 0;
}

/**
 * @brief Switch encoder to bumping mode
 *
 * @param id XEVE encodec instance identifier
 * @return 0 on success, negative error code on failure
 */
static int setup_bumping(EVCE id)
{
    int val = 1;
    int size = sizeof(int);
    if (EVC_FAILED(evce_config(id, EVCE_CFG_SET_FORCE_OUT, (void *)(&val), &size)))
        return AVERROR_EXTERNAL;

    return 0;
}

/**
 * @brief Initialize eXtra-fast Essential Video Encoder codec
 * Create an encoder instance and allocate all the needed resources
 *
 * @param avctx codec context
 * @return 0 on success, negative error code on failure
 */
static av_cold int libvneve_init(AVCodecContext *avctx)
{
    VNeveContext *xectx = avctx->priv_data;
    unsigned char *bs_buf = NULL;
    int i;
    int shift_h = 0;
    int shift_v = 0;
    int width_chroma = 0;
    int height_chroma = 0;
    EVC_IMGB *imgb = NULL;
    int ret = 0;

    EVCE_CDSC *cdsc = &(xectx->cdsc);

    /* allocate bitstream buffer */
    bs_buf = av_malloc(MAX_BS_BUF);
    if (bs_buf == NULL) {
        av_log(avctx, AV_LOG_ERROR, "Cannot allocate bitstream buffer\n");
        return AVERROR(ENOMEM);
    }
    xectx->bitb.addr = bs_buf;
    xectx->bitb.bsize = MAX_BS_BUF;

    /* read configurations and set values for created descriptor (XEVE_CDSC) */
    if ((ret = get_conf(avctx, cdsc)) != 0) {
        av_log(avctx, AV_LOG_ERROR, "Cannot get configuration\n");
        return AVERROR(EINVAL);
    }

    /*if ((ret = vneve_param_check(&cdsc->param)) != 0) {
        av_log(avctx, AV_LOG_ERROR, "Invalid configuration\n");
        return AVERROR(EINVAL);
    }

    {
        AVDictionaryEntry *en = NULL;
        while (en = av_dict_get(xectx->vneve_params, "", en, AV_DICT_IGNORE_SUFFIX)) {
            if ((ret = vneve_param_parse(&cdsc->param, en->key, en->value)) < 0) {
                av_log(avctx, AV_LOG_WARNING,
                       "Error parsing option '%s = %s'.\n",
                       en->key, en->value);
            }
        }
    }*/

    /* create encoder */
    xectx->id = evce_create(cdsc, NULL);
    if (xectx->id == NULL) {
        av_log(avctx, AV_LOG_ERROR, "Cannot create XEVE encoder\n");
        return AVERROR_EXTERNAL;
    }

    if ((ret = set_extra_config(avctx, xectx->id, xectx)) != 0) {
        av_log(avctx, AV_LOG_ERROR, "Cannot set extra configuration\n");
        return AVERROR(EINVAL);
    }

    if ((ret = av_pix_fmt_get_chroma_sub_sample(avctx->pix_fmt, &shift_h, &shift_v)) != 0) {
        av_log(avctx, AV_LOG_ERROR, "Failed to get  chroma shift\n");
        return AVERROR(EINVAL);
    }

    // Chroma subsampling
    //
    // YUV format explanation
    // shift_h == 1 && shift_v == 1 : YUV420
    // shift_h == 1 && shift_v == 0 : YUV422
    // shift_h == 0 && shift_v == 0 : YUV444
    //
    width_chroma = AV_CEIL_RSHIFT(avctx->width, shift_h);
    height_chroma = AV_CEIL_RSHIFT(avctx->height, shift_v);

    /* set default values for input image buffer */
    imgb = &xectx->imgb;
    imgb->cs = libvneve_color_space(avctx->pix_fmt);
    imgb->np = 3; /* only for yuv420p, yuv420ple */

    for (i = 0; i < imgb->np; i++)
        imgb->x[i] = imgb->y[i] = 0;

    imgb->w[0] = imgb->aw[0] = avctx->width; // width luma
    imgb->w[1] = imgb->w[2] = imgb->aw[1] = imgb->aw[2] = width_chroma;
    imgb->h[0] = imgb->ah[0] = avctx->height; // height luma
    imgb->h[1] = imgb->h[2] = imgb->ah[1] = imgb->ah[2] = height_chroma;

    xectx->state = STATE_ENCODING;

    return 0;
}

/**
  * Encode raw data frame into EVC packet
  *
  * @param[in]  avctx codec context
  * @param[out] avpkt output AVPacket containing encoded data
  * @param[in]  frame AVFrame containing the raw data to be encoded
  * @param[out] got_packet encoder sets to 0 or 1 to indicate that a
  *                         non-empty packet was returned in pkt
  *
  * @return 0 on success, negative error code on failure
  */
static int libvneve_encode(AVCodecContext *avctx, AVPacket *avpkt,
                          const AVFrame *frame, int *got_packet)
{
    VNeveContext *xectx =  avctx->priv_data;
    int  ret = -1;

    // No more input frames are available but encoder still can have some data in its internal buffer to process
    // and some frames to dump.
    if (xectx->state == STATE_ENCODING && frame == NULL) {
        if (setup_bumping(xectx->id) == 0)
            xectx->state = STATE_BUMPING;  // Entering bumping process
        else {
            av_log(avctx, AV_LOG_ERROR, "Failed to setup bumping\n");
            return 0;
        }
    }

    if (xectx->state == STATE_ENCODING) {
        int i;
        EVC_IMGB *imgb = NULL;

        imgb = &xectx->imgb;

        for (i = 0; i < imgb->np; i++) {
            imgb->a[i] = frame->data[i];
            imgb->s[i] = frame->linesize[i];
        }

        //imgb->ts[XEVE_TS_PTS] = frame->pts;

        /* push image to encoder */
        ret = evce_push(xectx->id, (EVC_IMGB **)imgb);
        if (EVC_FAILED(ret)) {
            av_log(avctx, AV_LOG_ERROR, "vneve_push() failed\n");
            return AVERROR_EXTERNAL;
        }
    }
    if (xectx->state == STATE_ENCODING || xectx->state == STATE_BUMPING) {
        /* encoding */
        ret = evce_encode(xectx->id, &(xectx->bitb), &(xectx->stat));
        if (EVC_FAILED(ret)) {
            av_log(avctx, AV_LOG_ERROR, "vneve_encode() failed\n");
            return AVERROR_EXTERNAL;
        }

        /* store bitstream */
        if (ret == EVC_OK_OUT_NOT_AVAILABLE) { // Return OK but picture is not available yet
            *got_packet = 0;
            return 0;
        } else if (ret == EVC_OK) {
            int av_pic_type;

            if (xectx->stat.write > 0) {

                ret = ff_get_encode_buffer(avctx, avpkt, xectx->stat.write, 0);
                if (ret < 0)
                    return ret;

                memcpy(avpkt->data, xectx->bitb.addr, xectx->stat.write);

                avpkt->time_base.num = 1;
                avpkt->time_base.den = xectx->cdsc.fps;

                //avpkt->pts = xectx->bitb.ts[XEVE_TS_PTS];
                //avpkt->dts = xectx->bitb.ts[XEVE_TS_DTS];

                switch(xectx->stat.stype) {
                case EVC_ST_I:
                    av_pic_type = AV_PICTURE_TYPE_I;
                    avpkt->flags |= AV_PKT_FLAG_KEY;
                    break;
                case EVC_ST_P:
                    av_pic_type = AV_PICTURE_TYPE_P;
                    break;
                case EVC_ST_B:
                    av_pic_type = AV_PICTURE_TYPE_B;
                    break;
                case EVC_ST_UNKNOWN:
                    av_log(avctx, AV_LOG_ERROR, "Unknown slice type\n");
                    return AVERROR_INVALIDDATA;
                }

                ff_side_data_set_encoder_stats(avpkt, xectx->stat.qp * FF_QP2LAMBDA, NULL, 0, av_pic_type);

                *got_packet = 1;
            }
        } else if (ret == EVC_OK_NO_MORE_FRM) {
            // Return OK but no more frames
            return 0;
        } else {
            av_log(avctx, AV_LOG_ERROR, "Invalid return value: %d\n", ret);
            return AVERROR_EXTERNAL;
        }
    } else {
        av_log(avctx, AV_LOG_ERROR, "Udefined encoder state\n");
        return AVERROR_INVALIDDATA;
    }

    return 0;
}

/**
 * Destroy the encoder and release all the allocated resources
 *
 * @param avctx codec context
 * @return 0 on success, negative error code on failure
 */
static av_cold int libvneve_close(AVCodecContext *avctx)
{
    VNeveContext *xectx = avctx->priv_data;

    if (xectx->id) {
        evce_delete(xectx->id);
        xectx->id = NULL;
    }

    av_free(xectx->bitb.addr); /* release bitstream buffer */

    return 0;
}

#define OFFSET(x) offsetof(VNeveContext, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM

static const enum AVPixelFormat supported_pixel_formats[] = {
    AV_PIX_FMT_YUV420P,
    AV_PIX_FMT_YUV420P10,
    AV_PIX_FMT_NONE
};

// Consider using following options (./ffmpeg --help encoder=libvneve)
//
static const AVOption libvneve_options[] = {
    { "preset", "Encoding preset for setting encoding speed", OFFSET(preset_id), AV_OPT_TYPE_INT, { .i64 = ENC_SUCO_FAST_CONFIG }, 1,  4, VE, "preset" },
    { "default", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = ENC_SUCO_FAST_CONFIG }, INT_MIN, INT_MAX, VE, "preset" },
    { "fast",    NULL, 0, AV_OPT_TYPE_CONST, { .i64 = 1 },    INT_MIN, INT_MAX, VE, "preset" },
    { "medium",  NULL, 0, AV_OPT_TYPE_CONST, { .i64 = 2 },  INT_MIN, INT_MAX, VE, "preset" },
    { "slow",    NULL, 0, AV_OPT_TYPE_CONST, { .i64 = 3 },    INT_MIN, INT_MAX, VE, "preset" },
    { "placebo", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = 4 }, INT_MIN, INT_MAX, VE, "preset" },
    //{ "tune", "Tuning parameter for special purpose operation", OFFSET(tune_id), AV_OPT_TYPE_INT, { .i64 = XEVE_TUNE_NONE }, XEVE_TUNE_NONE, XEVE_TUNE_PSNR, VE, "tune"},
    //{ "none",        NULL, 0, AV_OPT_TYPE_CONST, { .i64 = XEVE_TUNE_NONE },        INT_MIN, INT_MAX, VE, "tune" },
    //{ "zerolatency", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = XEVE_TUNE_ZEROLATENCY }, INT_MIN, INT_MAX, VE, "tune" },
    //{ "psnr",        NULL, 0, AV_OPT_TYPE_CONST, { .i64 = XEVE_TUNE_PSNR },        INT_MIN, INT_MAX, VE, "tune" },
    { "profile", "Encoding profile", OFFSET(profile_id), AV_OPT_TYPE_INT, { .i64 = PROFILE_BASELINE }, PROFILE_BASELINE,  PROFILE_MAIN, VE, "profile" },
    { "baseline", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = PROFILE_BASELINE }, INT_MIN, INT_MAX, VE, "profile" },
    { "main",     NULL, 0, AV_OPT_TYPE_CONST, { .i64 = PROFILE_MAIN },     INT_MIN, INT_MAX, VE, "profile" },
    //{ "rc_mode", "Rate control mode", OFFSET(rc_mode), AV_OPT_TYPE_INT, { .i64 = XEVE_RC_CQP }, XEVE_RC_CQP,  XEVE_RC_CRF, VE, "rc_mode" },
    //{ "CQP", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = XEVE_RC_CQP }, INT_MIN, INT_MAX, VE, "rc_mode" },
    //{ "ABR", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = XEVE_RC_ABR }, INT_MIN, INT_MAX, VE, "rc_mode" },
    //{ "CRF", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = XEVE_RC_CRF }, INT_MIN, INT_MAX, VE, "rc_mode" },
    { "qp", "Quantization parameter value for CQP rate control mode", OFFSET(qp), AV_OPT_TYPE_INT, { .i64 = 32 }, 0, 51, VE },
    //{ "crf", "Constant rate factor value for CRF rate control mode", OFFSET(crf), AV_OPT_TYPE_INT, { .i64 = 32 }, 10, 49, VE },
    { "hash", "Embed picture signature (HASH) for conformance checking in decoding", OFFSET(hash), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE },
    //{ "sei_info", "Embed SEI messages identifying encoder parameters and command line arguments", OFFSET(sei_info), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE },
    { "vneve-params",  "Override the vneve configuration using a :-separated list of key=value parameters", OFFSET(vneve_params), AV_OPT_TYPE_DICT, { 0 }, 0, 0, VE },
    { NULL }
};

static const AVClass libvneve_class = {
    .class_name = "libvneve",
    .option     = libvneve_options,
    .version    = LIBAVUTIL_VERSION_INT,
};

/**
 *  libavcodec generic global options, which can be set on all the encoders and decoders
 *  @see https://www.ffmpeg.org/ffmpeg-codecs.html#Codec-Options
 */
static const FFCodecDefault libvneve_defaults[] = {
    { "b", "0" },       // bitrate in terms of kilo-bits per second
    { "g", "0" },       // gop_size (key-frame interval 0: only one I-frame at the first time; 1: every frame is coded in I-frame)
    { "bf", "15"},      // maximum number of B frames (0: no B-frames, 1,3,7,15)
    { "threads", "0"},  // number of threads to be used (0: automatically select the number of threads to set)
    { NULL },
};

const FFCodec ff_libvneve_encoder = {
    .p.name             = "libvneve",
    .p.long_name        = NULL_IF_CONFIG_SMALL("libvneve V-NOVA MPEG-5 EVC"),
    .p.type             = AVMEDIA_TYPE_VIDEO,
    .p.id               = AV_CODEC_ID_EVC,
    .init               = libvneve_init,
    FF_CODEC_ENCODE_CB(libvneve_encode),
    .close              = libvneve_close,
    .priv_data_size     = sizeof(VNeveContext),
    .p.priv_class       = &libvneve_class,
    .defaults           = libvneve_defaults,
    .p.capabilities     = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_OTHER_THREADS | AV_CODEC_CAP_DR1,
    .p.profiles         = NULL_IF_CONFIG_SMALL(ff_evc_profiles),
    .p.wrapper_name     = "libvneve",
    .p.pix_fmts         = supported_pixel_formats,
    .caps_internal      = FF_CODEC_CAP_INIT_CLEANUP | FF_CODEC_CAP_NOT_INIT_THREADSAFE,
};
