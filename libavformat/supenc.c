/*
 * SUP muxer
 * Copyright (c) 2014 Petri Hintukainen <phintuka@users.sourceforge.net>
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

#include "libavformat/avformat.h"
#include "libavformat/internal.h"
#include "libavformat/mux.h"
#include "libavutil/intreadwrite.h"

#define SUP_PGS_MAGIC 0x5047 /* "PG", big endian */

static int sup_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    const uint8_t *data = pkt->data;
    size_t size = pkt->size;
    uint32_t pts = 0, dts = 0;

    if (pkt->pts != AV_NOPTS_VALUE) {
        pts = pkt->pts;
    }
    if (pkt->dts != AV_NOPTS_VALUE) {
        dts = pkt->dts;
    }

    /*
      Split frame to segments.
      mkvmerge stores multiple segments in one frame.
    */
    while (size > 2) {
        size_t len = AV_RB16(data + 1) + 3;

        if (len > size) {
            av_log(s, AV_LOG_ERROR, "Not enough data, skipping %"SIZE_SPECIFIER" bytes\n",
                   size);
            return AVERROR_INVALIDDATA;
        }

        /* header */
        avio_wb16(s->pb, SUP_PGS_MAGIC);
        avio_wb32(s->pb, pts);
        avio_wb32(s->pb, dts);

        avio_write(s->pb, data, len);

        data += len;
        size -= len;
    }

    if (size > 0) {
        av_log(s, AV_LOG_ERROR, "Skipping %"SIZE_SPECIFIER" bytes after last segment in frame\n",
               size);
        return AVERROR_INVALIDDATA;
    }

    return 0;
}

static av_cold int sup_init(AVFormatContext *s)
{
    avpriv_set_pts_info(s->streams[0], 32, 1, 90000);

    return 0;
}

const FFOutputFormat ff_sup_muxer = {
    .p.name           = "sup",
    .p.long_name      = NULL_IF_CONFIG_SMALL("raw HDMV Presentation Graphic Stream subtitles"),
    .p.extensions     = "sup",
    .p.mime_type      = "application/x-pgs",
    .p.video_codec    = AV_CODEC_ID_NONE,
    .p.audio_codec    = AV_CODEC_ID_NONE,
    .p.subtitle_codec = AV_CODEC_ID_HDMV_PGS_SUBTITLE,
    .p.flags          = AVFMT_VARIABLE_FPS | AVFMT_TS_NONSTRICT,
    .flags_internal   = FF_OFMT_FLAG_MAX_ONE_OF_EACH,
    .init             = sup_init,
    .write_packet   = sup_write_packet,
};
