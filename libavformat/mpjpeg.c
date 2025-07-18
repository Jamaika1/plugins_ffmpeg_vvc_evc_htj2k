/*
 * Multipart JPEG format
 * Copyright (c) 2000, 2001, 2002, 2003 Fabrice Bellard
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
#include "libavutil/opt.h"
#include "libavformat/avformat.h"
#include "libavformat/mux.h"

/* Multipart JPEG */

#define BOUNDARY_TAG "ffmpeg"

typedef struct MPJPEGContext {
    AVClass *class;
    char *boundary_tag;
} MPJPEGContext;

static int mpjpeg_write_header(AVFormatContext *s)
{
    MPJPEGContext *mpj = s->priv_data;
    avio_printf(s->pb, "--%s\r\n", mpj->boundary_tag);
    return 0;
}

static int mpjpeg_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    MPJPEGContext *mpj = s->priv_data;
    avio_printf(s->pb, "Content-type: image/jpeg\r\n");
    avio_printf(s->pb, "Content-length: %d\r\n\r\n",
                pkt->size);
    avio_write(s->pb, pkt->data, pkt->size);

    avio_printf(s->pb, "\r\n--%s\r\n", mpj->boundary_tag);
    return 0;
}

static const AVOption options[] = {
    { "boundary_tag",    "Boundary tag", offsetof(MPJPEGContext, boundary_tag),   AV_OPT_TYPE_STRING, {.str = BOUNDARY_TAG}, .flags = AV_OPT_FLAG_ENCODING_PARAM },
    { NULL },
};

static const AVClass mpjpeg_muxer_class = {
    .class_name = "mpjpeg_muxer",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

const FFOutputFormat ff_mpjpeg_muxer = {
    .p.name            = "mpjpeg",
    .p.long_name       = NULL_IF_CONFIG_SMALL("MIME multipart JPEG"),
    .p.mime_type       = "multipart/x-mixed-replace;boundary=" BOUNDARY_TAG,
    .p.extensions      = "mjpg",
    .priv_data_size    = sizeof(MPJPEGContext),
    .p.audio_codec     = AV_CODEC_ID_NONE,
    .p.video_codec     = AV_CODEC_ID_MJPEG,
    .p.subtitle_codec  = AV_CODEC_ID_NONE,
    .flags_internal    = FF_OFMT_FLAG_ONLY_DEFAULT_CODECS,
    .write_header      = mpjpeg_write_header,
    .write_packet      = mpjpeg_write_packet,
    .p.flags           = AVFMT_NOTIMESTAMPS,
    .p.priv_class      = &mpjpeg_muxer_class,
};
