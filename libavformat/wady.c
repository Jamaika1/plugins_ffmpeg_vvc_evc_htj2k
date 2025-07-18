/*
 * WADY demuxer
 * Copyright (c) 2023 Paul B Mahol
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

#include "libavutil/channel_layout.h"
#include "libavutil/intreadwrite.h"
#include "libavformat/avformat.h"
#include "libavformat/demux.h"
#include "libavformat/internal.h"
#include "libavformat/pcm.h"

static int wady_probe(const AVProbeData *p)
{
    if (AV_RL32(p->buf) != MKTAG('W','A','D','Y'))
        return 0;
    if (p->buf[4] != 0 || p->buf[5] == 0 ||
        AV_RL16(p->buf+6) == 0 ||
        AV_RL16(p->buf+6) > 2 ||
        (int32_t)AV_RL32(p->buf+8) <= 0)
        return 0;

    return AVPROBE_SCORE_MAX / 3 * 2;
}

static int wady_read_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    AVCodecParameters *par;
    int channels, ret;
    AVStream *st;

    avio_skip(pb, 4 + 1);

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    par              = st->codecpar;
    par->codec_type  = AVMEDIA_TYPE_AUDIO;
    par->codec_id    = AV_CODEC_ID_WADY_DPCM;
    par->format      = AV_SAMPLE_FMT_S16;
    if ((ret = ff_get_extradata(s, par, pb, 1)) < 0)
        return ret;
    channels         = avio_rl16(pb);
    if (channels == 0)
        return AVERROR_INVALIDDATA;
    av_channel_layout_default(&par->ch_layout, channels);
    par->sample_rate = avio_rl32(pb);
    if (par->sample_rate <= 0)
        return AVERROR_INVALIDDATA;
    avio_skip(pb, 4);
    st->duration = avio_rl32(pb);
    par->block_align = channels;
    avpriv_set_pts_info(st, 64, 1, par->sample_rate);

    avio_seek(pb, 0x30, SEEK_SET);

    return 0;
}

const FFInputFormat ff_wady_demuxer = {
    .p.name         = "wady",
    .p.long_name    = NULL_IF_CONFIG_SMALL("Marble WADY"),
    .p.flags        = AVFMT_GENERIC_INDEX,
    .p.extensions   = "way",
    .read_probe     = wady_probe,
    .read_header    = wady_read_header,
    .read_packet    = ff_pcm_read_packet,
    .read_seek      = ff_pcm_read_seek,
};
