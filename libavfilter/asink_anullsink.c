/*
 * Copyright (c) 2010 S.N. Hemanth Meenakshisundaram <smeenaks@ucsd.edu>
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

#include "libavutil/internal.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"

static int null_filter_frame(AVFilterLink *link, AVFrame *frame)
{
    av_frame_free(&frame);
    return 0;
}

static const AVFilterPad avfilter_asink_anullsink_inputs[] = {
    {
        .name           = "default",
        .type           = AVMEDIA_TYPE_AUDIO,
        .filter_frame   = null_filter_frame,
    },
};

const FFFilter ff_asink_anullsink = {
    .p.name        = "anullsink",
    .p.description = NULL_IF_CONFIG_SMALL("Do absolutely nothing with the input audio."),
    .p.outputs     = NULL,
    .priv_size   = 0,
    FILTER_INPUTS(avfilter_asink_anullsink_inputs),
};
