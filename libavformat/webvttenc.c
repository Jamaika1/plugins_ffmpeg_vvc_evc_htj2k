/*
 * Copyright (c) 2013 Matthew Heaney
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
 * WebVTT subtitle muxer
 * @see http://dev.w3.org/html5/webvtt/
 */

#include "libavformat/avformat.h"
#include "libavformat/internal.h"
#include "libavformat/mux.h"

static void webvtt_write_time(AVIOContext *pb, int64_t millisec)
{
    int64_t sec, min, hour;
    sec = millisec / 1000;
    millisec -= 1000 * sec;
    min = sec / 60;
    sec -= 60 * min;
    hour = min / 60;
    min -= 60 * hour;

    if (hour > 0)
        avio_printf(pb, "%02"PRId64":", hour);

    avio_printf(pb, "%02"PRId64":%02"PRId64".%03"PRId64"", min, sec, millisec);
}

static int webvtt_write_header(AVFormatContext *ctx)
{
    AVStream     *s = ctx->streams[0];
    AVIOContext *pb = ctx->pb;

    avpriv_set_pts_info(s, 64, 1, 1000);

    avio_printf(pb, "WEBVTT\n");

    return 0;
}

static int webvtt_write_packet(AVFormatContext *ctx, AVPacket *pkt)
{
    AVIOContext  *pb = ctx->pb;
    size_t id_size, settings_size;
    int id_size_int, settings_size_int;
    uint8_t *id, *settings;

    avio_printf(pb, "\n");

    id = av_packet_get_side_data(pkt, AV_PKT_DATA_WEBVTT_IDENTIFIER,
                                 &id_size);

    if (id_size > INT_MAX)
        return AVERROR(EINVAL);

    id_size_int = id_size;
    if (id && id_size_int > 0)
        avio_printf(pb, "%.*s\n", id_size_int, id);

    webvtt_write_time(pb, pkt->pts);
    avio_printf(pb, " --> ");
    webvtt_write_time(pb, pkt->pts + pkt->duration);

    settings = av_packet_get_side_data(pkt, AV_PKT_DATA_WEBVTT_SETTINGS,
                                       &settings_size);

    if (settings_size > INT_MAX)
        return AVERROR(EINVAL);

    settings_size_int = settings_size;
    if (settings && settings_size_int > 0)
        avio_printf(pb, " %.*s", settings_size_int, settings);

    avio_printf(pb, "\n");

    avio_write(pb, pkt->data, pkt->size);
    avio_printf(pb, "\n");

    return 0;
}

const FFOutputFormat ff_webvtt_muxer = {
    .p.name            = "webvtt",
    .p.long_name       = NULL_IF_CONFIG_SMALL("WebVTT subtitle"),
    .p.extensions      = "vtt",
    .p.mime_type       = "text/vtt",
    .p.flags           = AVFMT_VARIABLE_FPS | AVFMT_TS_NONSTRICT,
    .p.video_codec     = AV_CODEC_ID_NONE,
    .p.audio_codec     = AV_CODEC_ID_NONE,
    .p.subtitle_codec  = AV_CODEC_ID_WEBVTT,
    .write_header      = webvtt_write_header,
    .write_packet      = webvtt_write_packet,
    .flags_internal    = FF_OFMT_FLAG_MAX_ONE_OF_EACH |
                         FF_OFMT_FLAG_ONLY_DEFAULT_CODECS,
};
