/*
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

#include "libavutil/buffer.h"
#include "libavutil/hwcontext.h"
#include "libavutil/hwcontext_internal.h"
#include "libavutil/log.h"
#include "libavutil/pixdesc.h"
#include "libavutil/opt.h"

#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"
#include "libavfilter/formats.h"
#include "libavfilter/video.h"

typedef struct HWUploadContext {
    const AVClass *class;

    AVBufferRef       *hwdevice_ref;

    AVBufferRef       *hwframes_ref;
    AVHWFramesContext *hwframes;

    char *device_type;
} HWUploadContext;

static int hwupload_init(AVFilterContext *avctx)
{
    HWUploadContext *ctx = avctx->priv;
    int err;

    if (!avctx->hw_device_ctx) {
        av_log(ctx, AV_LOG_ERROR, "A hardware device reference is required "
               "to upload frames to.\n");
        return AVERROR(EINVAL);
    }

    if (ctx->device_type) {
        err = av_hwdevice_ctx_create_derived(
            &ctx->hwdevice_ref,
            av_hwdevice_find_type_by_name(ctx->device_type),
            avctx->hw_device_ctx, 0);
        if (err < 0)
            return err;
    } else {
        ctx->hwdevice_ref = av_buffer_ref(avctx->hw_device_ctx);
        if (!ctx->hwdevice_ref)
            return AVERROR(ENOMEM);
    }

    return 0;
}

static int hwupload_query_formats(const AVFilterContext *avctx,
                                  AVFilterFormatsConfig **cfg_in,
                                  AVFilterFormatsConfig **cfg_out)
{
    const HWUploadContext *ctx = avctx->priv;
    AVHWFramesConstraints *constraints = NULL;
    const enum AVPixelFormat *input_pix_fmts, *output_pix_fmts;
    AVFilterFormats *input_formats = NULL;
    int err, i;

    constraints = av_hwdevice_get_hwframe_constraints(ctx->hwdevice_ref, NULL);
    if (!constraints) {
        err = AVERROR(EINVAL);
        goto fail;
    }

    input_pix_fmts  = constraints->valid_sw_formats;
    output_pix_fmts = constraints->valid_hw_formats;

    input_formats = ff_make_format_list(output_pix_fmts);
    if (!input_formats) {
        err = AVERROR(ENOMEM);
        goto fail;
    }
    if (input_pix_fmts) {
        for (i = 0; input_pix_fmts[i] != AV_PIX_FMT_NONE; i++) {
            err = ff_add_format(&input_formats, input_pix_fmts[i]);
            if (err < 0)
                goto fail;
        }
    }

    if ((err = ff_formats_ref(input_formats, &cfg_in[0]->formats)) < 0 ||
        (err = ff_formats_ref(ff_make_format_list(output_pix_fmts),
                              &cfg_out[0]->formats)) < 0)
        goto fail;

    av_hwframe_constraints_free(&constraints);
    return 0;

fail:
    av_hwframe_constraints_free(&constraints);
    return err;
}

static int hwupload_config_output(AVFilterLink *outlink)
{
    FilterLink       *outl = ff_filter_link(outlink);
    AVFilterContext *avctx = outlink->src;
    AVFilterLink   *inlink = avctx->inputs[0];
    FilterLink        *inl = ff_filter_link(inlink);
    HWUploadContext   *ctx = avctx->priv;
    int err;

    av_buffer_unref(&ctx->hwframes_ref);

    if (inlink->format == outlink->format) {
        // The input is already a hardware format, so we just want to
        // pass through the input frames in their own hardware context.
        if (!inl->hw_frames_ctx) {
            av_log(ctx, AV_LOG_ERROR, "No input hwframe context.\n");
            return AVERROR(EINVAL);
        }

        outl->hw_frames_ctx = av_buffer_ref(inl->hw_frames_ctx);
        if (!outl->hw_frames_ctx)
            return AVERROR(ENOMEM);

        return 0;
    }

    ctx->hwframes_ref = av_hwframe_ctx_alloc(ctx->hwdevice_ref);
    if (!ctx->hwframes_ref)
        return AVERROR(ENOMEM);

    ctx->hwframes = (AVHWFramesContext*)ctx->hwframes_ref->data;

    av_log(ctx, AV_LOG_DEBUG, "Surface format is %s.\n",
           av_get_pix_fmt_name(inlink->format));

    ctx->hwframes->format    = outlink->format;
    if (inl->hw_frames_ctx) {
        AVHWFramesContext *in_hwframe_ctx =
            (AVHWFramesContext*)inl->hw_frames_ctx->data;
        ctx->hwframes->sw_format = in_hwframe_ctx->sw_format;
    } else {
        ctx->hwframes->sw_format = inlink->format;
    }
    ctx->hwframes->width     = inlink->w;
    ctx->hwframes->height    = inlink->h;

    if (avctx->extra_hw_frames >= 0)
        ctx->hwframes->initial_pool_size = 2 + avctx->extra_hw_frames;

    err = av_hwframe_ctx_init(ctx->hwframes_ref);
    if (err < 0)
        goto fail;

    outl->hw_frames_ctx = av_buffer_ref(ctx->hwframes_ref);
    if (!outl->hw_frames_ctx) {
        err = AVERROR(ENOMEM);
        goto fail;
    }

    return 0;

fail:
    av_buffer_unref(&ctx->hwframes_ref);
    return err;
}

static int hwupload_filter_frame(AVFilterLink *link, AVFrame *input)
{
    AVFilterContext *avctx = link->dst;
    AVFilterLink  *outlink = avctx->outputs[0];
    HWUploadContext   *ctx = avctx->priv;
    AVFrame *output = NULL;
    int err;

    if (input->format == outlink->format)
        return ff_filter_frame(outlink, input);

    output = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!output) {
        av_log(ctx, AV_LOG_ERROR, "Failed to allocate frame to upload to.\n");
        err = AVERROR(ENOMEM);
        goto fail;
    }

    output->width  = input->width;
    output->height = input->height;

    err = av_hwframe_transfer_data(output, input, 0);
    if (err < 0) {
        av_log(ctx, AV_LOG_ERROR, "Failed to upload frame: %d.\n", err);
        goto fail;
    }

    err = av_frame_copy_props(output, input);
    if (err < 0)
        goto fail;

    av_frame_free(&input);

    return ff_filter_frame(outlink, output);

fail:
    av_frame_free(&input);
    av_frame_free(&output);
    return err;
}

static av_cold void hwupload_uninit(AVFilterContext *avctx)
{
    HWUploadContext *ctx = avctx->priv;

    av_buffer_unref(&ctx->hwframes_ref);
    av_buffer_unref(&ctx->hwdevice_ref);
}

#define OFFSET(x) offsetof(HWUploadContext, x)
#define FLAGS (AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_VIDEO_PARAM)
static const AVOption hwupload_options[] = {
    {
        "derive_device", "Derive a new device of this type",
        OFFSET(device_type), AV_OPT_TYPE_STRING,
        { .str = NULL }, 0, 0, FLAGS
    },
    {
        NULL
    }
};

AVFILTER_DEFINE_CLASS(hwupload);

static const AVFilterPad hwupload_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = hwupload_filter_frame,
    },
};

static const AVFilterPad hwupload_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = hwupload_config_output,
    },
};

const FFFilter ff_vf_hwupload = {
    .p.name        = "hwupload",
    .p.description = NULL_IF_CONFIG_SMALL("Upload a normal frame to a hardware frame"),
    .p.priv_class  = &hwupload_class,
    .p.flags       = AVFILTER_FLAG_HWDEVICE,
    .init          = hwupload_init,
    .uninit        = hwupload_uninit,
    .priv_size     = sizeof(HWUploadContext),
    FILTER_INPUTS(hwupload_inputs),
    FILTER_OUTPUTS(hwupload_outputs),
    FILTER_QUERY_FUNC2(hwupload_query_formats),
    .flags_internal = FF_FILTER_FLAG_HWFRAME_AWARE,
};
