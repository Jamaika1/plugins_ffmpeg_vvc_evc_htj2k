/*
 * Copyright (c) 2019 Xuewei Meng
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

/**
 * @file
 * Filter implementing image derain filter using deep convolutional networks.
 * http://openaccess.thecvf.com/content_ECCV_2018/html/Xia_Li_Recurrent_Squeeze-and-Excitation_Context_ECCV_2018_paper.html
 */

#include "libavutil/opt.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/dnn_filter_common.h"
#include "libavfilter/filters.h"
#include "libavfilter/video.h"

typedef struct DRContext {
    const AVClass *class;
    DnnContext dnnctx;
    int                filter_type;
} DRContext;

#define OFFSET(x) offsetof(DRContext, x)
#define FLAGS AV_OPT_FLAG_FILTERING_PARAM | AV_OPT_FLAG_VIDEO_PARAM
static const AVOption derain_options[] = {
    { "filter_type", "filter type(derain/dehaze)",  OFFSET(filter_type),    AV_OPT_TYPE_INT,    { .i64 = 0 },    0, 1, FLAGS, .unit = "type" },
    { "derain",      "derain filter flag",          0,                      AV_OPT_TYPE_CONST,  { .i64 = 0 },    0, 0, FLAGS, .unit = "type" },
    { "dehaze",      "dehaze filter flag",          0,                      AV_OPT_TYPE_CONST,  { .i64 = 1 },    0, 0, FLAGS, .unit = "type" },
    { "dnn_backend", "DNN backend",                 OFFSET(dnnctx.backend_type),   AV_OPT_TYPE_INT,    { .i64 = 1 },    0, 1, FLAGS, .unit = "backend" },
#if (CONFIG_LIBTENSORFLOW == 1)
    { "tensorflow",  "tensorflow backend flag",     0,                      AV_OPT_TYPE_CONST,  { .i64 = 1 },    0, 0, FLAGS, .unit = "backend" },
#endif
    { NULL }
};

AVFILTER_DNN_DEFINE_CLASS(derain, DNN_TF);

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    DNNAsyncStatusType async_state = 0;
    AVFilterContext *ctx  = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    DRContext *dr_context = ctx->priv;
    int dnn_result;
    AVFrame *out;

    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_log(ctx, AV_LOG_ERROR, "could not allocate memory for output frame\n");
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);

    dnn_result = ff_dnn_execute_model(&dr_context->dnnctx, in, out);
    if (dnn_result != 0){
        av_log(ctx, AV_LOG_ERROR, "failed to execute model\n");
        av_frame_free(&in);
        return dnn_result;
    }
    do {
        async_state = ff_dnn_get_result(&dr_context->dnnctx, &in, &out);
    } while (async_state == DAST_NOT_READY);

    if (async_state != DAST_SUCCESS)
        return AVERROR(EINVAL);

    av_frame_free(&in);

    return ff_filter_frame(outlink, out);
}

static av_cold int init(AVFilterContext *ctx)
{
    DRContext *dr_context = ctx->priv;
    return ff_dnn_init(&dr_context->dnnctx, DFT_PROCESS_FRAME, ctx);
}

static av_cold void uninit(AVFilterContext *ctx)
{
    DRContext *dr_context = ctx->priv;
    ff_dnn_uninit(&dr_context->dnnctx);
}

static const AVFilterPad derain_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = filter_frame,
    },
};

const FFFilter ff_vf_derain = {
    .p.name        = "derain",
    .p.description = NULL_IF_CONFIG_SMALL("Apply derain filter to the input."),
    .p.priv_class  = &derain_class,
    .p.flags       = AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC,
    .priv_size     = sizeof(DRContext),
    .preinit       = ff_dnn_filter_init_child_class,
    .init          = init,
    .uninit        = uninit,
    FILTER_INPUTS(derain_inputs),
    FILTER_OUTPUTS(ff_video_default_filterpad),
    FILTER_SINGLE_PIXFMT(AV_PIX_FMT_RGB24),
};
