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

/**
 * @file
 * null video filter
 */

#include "libavutil/internal.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"
#include "libavfilter/video.h"

const FFFilter ff_vf_null = {
    .p.name        = "null",
    .p.description = NULL_IF_CONFIG_SMALL("Pass the source unchanged to the output."),
    .p.flags       = AVFILTER_FLAG_METADATA_ONLY,
    FILTER_INPUTS(ff_video_default_filterpad),
    FILTER_OUTPUTS(ff_video_default_filterpad),
};
