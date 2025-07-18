/*
 * Copyright (c) 2012 Stefano Sabatini
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
 * @file framestep filter, inspired on libmpcodecs/vf_framestep.c by
 * Daniele Fornighieri <guru AT digitalfantasy it>.
 */

#include "libavutil/opt.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"
#include "libavfilter/video.h"

typedef struct NullContext {
    const AVClass *class;
    int frame_step;
} FrameStepContext;

#define OFFSET(x) offsetof(FrameStepContext, x)
#define FLAGS AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_VIDEO_PARAM

static const AVOption framestep_options[] = {
    { "step", "set frame step",  OFFSET(frame_step), AV_OPT_TYPE_INT, {.i64=1}, 1, INT_MAX, FLAGS},
    { NULL },
};

AVFILTER_DEFINE_CLASS(framestep);

static int config_output_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FrameStepContext *framestep = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    FilterLink       *il = ff_filter_link(inlink);
    FilterLink       *ol = ff_filter_link(outlink);

    ol->frame_rate =
        av_div_q(il->frame_rate, (AVRational){framestep->frame_step, 1});

    av_log(ctx, AV_LOG_VERBOSE, "step:%d frame_rate:%d/%d(%f) -> frame_rate:%d/%d(%f)\n",
           framestep->frame_step,
           il->frame_rate.num, il->frame_rate.den, av_q2d(il->frame_rate),
           ol->frame_rate.num, ol->frame_rate.den, av_q2d(ol->frame_rate));
    return 0;
}

static int filter_frame(AVFilterLink *inlink, AVFrame *ref)
{
    FilterLink             *inl = ff_filter_link(inlink);
    FrameStepContext *framestep = inlink->dst->priv;

    if (!(inl->frame_count_out % framestep->frame_step)) {
        return ff_filter_frame(inlink->dst->outputs[0], ref);
    } else {
        av_frame_free(&ref);
        return 0;
    }
}

static const AVFilterPad framestep_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = filter_frame,
    },
};

static const AVFilterPad framestep_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_output_props,
    },
};

const FFFilter ff_vf_framestep = {
    .p.name        = "framestep",
    .p.description = NULL_IF_CONFIG_SMALL("Select one frame every N frames."),
    .p.priv_class  = &framestep_class,
    .p.flags       = AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC | AVFILTER_FLAG_METADATA_ONLY,
    .priv_size   = sizeof(FrameStepContext),
    FILTER_INPUTS(framestep_inputs),
    FILTER_OUTPUTS(framestep_outputs),
};
