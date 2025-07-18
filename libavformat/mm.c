/*
 * American Laser Games MM Format Demuxer
 * Copyright (c) 2006 Peter Ross
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
 * American Laser Games MM Format Demuxer
 * by Peter Ross (pross@xvid.org)
 *
 * The MM format was used by IBM-PC ports of ALG's "arcade shooter" games,
 * including Mad Dog McCree and Crime Patrol.
 *
 * Technical details here:
 *  http://wiki.multimedia.cx/index.php?title=American_Laser_Games_MM
 */

#include "libavutil/channel_layout.h"
#include "libavutil/intreadwrite.h"
#include "libavformat/avformat.h"
#include "libavformat/avio_internal.h"
#include "libavformat/demux.h"
#include "libavformat/internal.h"

#define MM_PREAMBLE_SIZE    6

#define MM_TYPE_HEADER      0x0
#define MM_TYPE_RAW         0x2
#define MM_TYPE_INTER       0x5
#define MM_TYPE_INTRA       0x8
#define MM_TYPE_INTRA_HH    0xc
#define MM_TYPE_INTER_HH    0xd
#define MM_TYPE_INTRA_HHV   0xe
#define MM_TYPE_INTER_HHV   0xf
#define MM_TYPE_AUDIO2      0x14
#define MM_TYPE_AUDIO       0x15
#define MM_TYPE_PALETTE     0x31

#define MM_HEADER_LEN_V     0x16    /* video only */
#define MM_HEADER_LEN_AV    0x18    /* video + audio */
#define MM_HEADER_LEN_AV2   0x1a

#define MM_PALETTE_COUNT    128
#define MM_PALETTE_SIZE     (MM_PALETTE_COUNT*3)

static int probe(const AVProbeData *p)
{
    int len, type, fps, w, h;
    if (p->buf_size < MM_HEADER_LEN_AV + MM_PREAMBLE_SIZE)
        return 0;
    /* the first chunk is always the header */
    if (AV_RL16(&p->buf[0]) != MM_TYPE_HEADER)
        return 0;
    len = AV_RL32(&p->buf[2]);
    if (len != MM_HEADER_LEN_V && len != MM_HEADER_LEN_AV && len != MM_HEADER_LEN_AV2)
        return 0;
    fps = AV_RL16(&p->buf[8]);
    w = AV_RL16(&p->buf[12]);
    h = AV_RL16(&p->buf[14]);
    if (!fps || fps > 60 || !w || w > 2048 || !h || h > 2048)
        return 0;
    type = AV_RL16(&p->buf[len]);
    if (!type || type > 0x31)
        return 0;

    /* only return half certainty since this check is a bit sketchy */
    return AVPROBE_SCORE_EXTENSION;
}

static int read_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    AVStream *st;

    unsigned int type, length;
    unsigned int frame_rate, width, height;

    type = avio_rl16(pb);
    length = avio_rl32(pb);

    if (type != MM_TYPE_HEADER || length < 10)
        return AVERROR_INVALIDDATA;

    /* read header */
    avio_rl16(pb);   /* total number of chunks */
    frame_rate = avio_rl16(pb);
    avio_rl16(pb);   /* ibm-pc video bios mode */
    width = avio_rl16(pb);
    height = avio_rl16(pb);
    avio_skip(pb, length - 10);  /* unknown data */

    /* video stream */
    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);
    st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codecpar->codec_id = AV_CODEC_ID_MMVIDEO;
    st->codecpar->codec_tag = 0;  /* no fourcc */
    st->codecpar->width = width;
    st->codecpar->height = height;
    avpriv_set_pts_info(st, 64, 1, frame_rate);

    /* audio stream */
    if (length >= MM_HEADER_LEN_AV) {
        st = avformat_new_stream(s, NULL);
        if (!st)
            return AVERROR(ENOMEM);
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_tag = 0; /* no fourcc */
        st->codecpar->codec_id = AV_CODEC_ID_PCM_U8;
        st->codecpar->ch_layout = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
        st->codecpar->sample_rate = 8000;
        avpriv_set_pts_info(st, 64, 1, 8000); /* 8000 hz */
    }

    return 0;
}

static int read_packet(AVFormatContext *s,
                           AVPacket *pkt)
{
    AVIOContext *pb = s->pb;
    unsigned char preamble[MM_PREAMBLE_SIZE];
    unsigned int type, length;
    int64_t pos = avio_tell(pb);
    int ret;

    while (1) {
        if (avio_feof(pb))
            return AVERROR_EOF;

        ret = ffio_read_size(pb, preamble, MM_PREAMBLE_SIZE);
        if (ret < 0)
            return ret;

        type = AV_RL16(&preamble[0]);
        length = AV_RL16(&preamble[2]);

        switch (type) {
        case MM_TYPE_RAW :
        case MM_TYPE_PALETTE :
        case MM_TYPE_INTER :
        case MM_TYPE_INTRA :
        case MM_TYPE_INTRA_HH :
        case MM_TYPE_INTER_HH :
        case MM_TYPE_INTRA_HHV :
        case MM_TYPE_INTER_HHV :
            /* output preamble + data */
            if ((ret = av_new_packet(pkt, length + MM_PREAMBLE_SIZE)) < 0)
                return ret;
            memcpy(pkt->data, preamble, MM_PREAMBLE_SIZE);
            ret = ffio_read_size(pb, pkt->data + MM_PREAMBLE_SIZE, length);
            if (ret < 0)
                return ret;
            pkt->size = length + MM_PREAMBLE_SIZE;
            pkt->stream_index = 0;
            if (type!=MM_TYPE_PALETTE)
                pkt->duration = 1;
            if (type == MM_TYPE_RAW ||
                type == MM_TYPE_INTRA)
                pkt->flags |= AV_PKT_FLAG_KEY;
            pkt->pos = pos;
            return 0;

        case MM_TYPE_AUDIO :
            if (s->nb_streams < 2)
                return AVERROR_INVALIDDATA;
            if ((ret = av_get_packet(s->pb, pkt, length)) < 0)
                return ret;
            pkt->stream_index = 1;
            pkt->duration = length;
            pkt->pos = pos;
            return 0;

        default :
            av_log(s, AV_LOG_INFO, "unknown chunk type 0x%x\n", type);
            /* fallthrough */
        case MM_TYPE_AUDIO2 :
            avio_skip(pb, length);
        }
    }
}

const FFInputFormat ff_mm_demuxer = {
    .p.name         = "mm",
    .p.long_name    = NULL_IF_CONFIG_SMALL("American Laser Games MM"),
    .p.flags        = AVFMT_GENERIC_INDEX,
    .read_probe     = probe,
    .read_header    = read_header,
    .read_packet    = read_packet,
};
