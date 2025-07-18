/*
 * OpenH264 shared utils
 * Copyright (C) 2014 Martin Storsjo
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

#include <string.h>
#include "libopenh264/decoder/codec_api.h"
#include "libopenh264/decoder/codec_ver.h"

#include "libavutil/error.h"
#include "libavutil/log.h"

#include "libavcodec/libopenh264.h"

// Convert libopenh264 log level to equivalent ffmpeg log level.
static int libopenh264_to_ffmpeg_log_level(int libopenh264_log_level)
{
    if      (libopenh264_log_level >= WELS_LOG_DETAIL)  return AV_LOG_TRACE;
    else if (libopenh264_log_level >= WELS_LOG_DEBUG)   return AV_LOG_DEBUG;
    else if (libopenh264_log_level >= WELS_LOG_INFO)    return AV_LOG_VERBOSE;
    else if (libopenh264_log_level >= WELS_LOG_WARNING) return AV_LOG_WARNING;
    else if (libopenh264_log_level >= WELS_LOG_ERROR)   return AV_LOG_ERROR;
    else                                                return AV_LOG_QUIET;
}

void ff_libopenh264_trace_callback(void *ctx, int level, const char *msg)
{
    // The message will be logged only if the requested EQUIVALENT ffmpeg log level is
    // less than or equal to the current ffmpeg log level.
    int equiv_ffmpeg_log_level = libopenh264_to_ffmpeg_log_level(level);
    av_log(ctx, equiv_ffmpeg_log_level, "%s\n", msg);
}
