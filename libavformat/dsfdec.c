/*
 * DSD Stream File (DSF) demuxer
 * Copyright (c) 2014 Peter Ross
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
#include "libavformat/id3v2.h"

typedef struct {
    uint64_t data_end;
    uint64_t audio_size;
    uint64_t data_size;
} DSFContext;

static int dsf_probe(const AVProbeData *p)
{
    if (p->buf_size < 12 || memcmp(p->buf, "DSD ", 4) || AV_RL64(p->buf + 4) != 28)
        return 0;
    return AVPROBE_SCORE_MAX;
}

static const AVChannelLayout dsf_channel_layout[] = {
    { .order = AV_CHANNEL_ORDER_UNSPEC },
    AV_CHANNEL_LAYOUT_MONO,
    AV_CHANNEL_LAYOUT_STEREO,
    AV_CHANNEL_LAYOUT_SURROUND,
    AV_CHANNEL_LAYOUT_QUAD,
    AV_CHANNEL_LAYOUT_4POINT0,
    AV_CHANNEL_LAYOUT_5POINT0_BACK,
    AV_CHANNEL_LAYOUT_5POINT1_BACK,
};

static void read_id3(AVFormatContext *s, uint64_t id3pos)
{
    ID3v2ExtraMeta *id3v2_extra_meta;
    if (avio_seek(s->pb, id3pos, SEEK_SET) < 0)
        return;

    ff_id3v2_read(s, ID3v2_DEFAULT_MAGIC, &id3v2_extra_meta, 0);
    if (id3v2_extra_meta) {
        ff_id3v2_parse_apic(s, id3v2_extra_meta);
        ff_id3v2_parse_chapters(s, id3v2_extra_meta);
    }
    ff_id3v2_free_extra_meta(&id3v2_extra_meta);
}

static int dsf_read_header(AVFormatContext *s)
{
    DSFContext *dsf = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st;
    uint64_t id3pos;
    unsigned int channel_type;
    int channels;

    avio_skip(pb, 4);
    if (avio_rl64(pb) != 28)
        return AVERROR_INVALIDDATA;

    /* create primary stream before any id3 coverart streams */
    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    avio_skip(pb, 8);
    id3pos = avio_rl64(pb);
    if (pb->seekable & AVIO_SEEKABLE_NORMAL) {
        read_id3(s, id3pos);
        avio_seek(pb, 28, SEEK_SET);
    }

    /* fmt chunk */

    if (avio_rl32(pb) != MKTAG('f', 'm', 't', ' ') || avio_rl64(pb) != 52)
        return AVERROR_INVALIDDATA;

    if (avio_rl32(pb) != 1) {
        avpriv_request_sample(s, "unknown format version");
        return AVERROR_INVALIDDATA;
    }

    if (avio_rl32(pb)) {
        avpriv_request_sample(s, "unknown format id");
        return AVERROR_INVALIDDATA;
    }

    channel_type = avio_rl32(pb);
    if (channel_type < FF_ARRAY_ELEMS(dsf_channel_layout))
        st->codecpar->ch_layout = dsf_channel_layout[channel_type];
    if (!st->codecpar->ch_layout.nb_channels)
        avpriv_request_sample(s, "channel type %i", channel_type);

    st->codecpar->codec_type   = AVMEDIA_TYPE_AUDIO;
    channels = avio_rl32(pb);
    if (!st->codecpar->ch_layout.nb_channels) {
        st->codecpar->ch_layout.nb_channels = channels;
    } else if (channels != st->codecpar->ch_layout.nb_channels) {
        av_log(s, AV_LOG_ERROR, "Channel count mismatch\n");
        return AVERROR(EINVAL);
    }
    st->codecpar->sample_rate  = avio_rl32(pb) / 8;

    if (st->codecpar->ch_layout.nb_channels <= 0)
        return AVERROR_INVALIDDATA;

    switch(avio_rl32(pb)) {
    case 1: st->codecpar->codec_id = AV_CODEC_ID_DSD_LSBF_PLANAR; break;
    case 8: st->codecpar->codec_id = AV_CODEC_ID_DSD_MSBF_PLANAR; break;
    default:
        avpriv_request_sample(s, "unknown most significant bit");
        return AVERROR_INVALIDDATA;
    }

    dsf->audio_size = avio_rl64(pb) / 8 * st->codecpar->ch_layout.nb_channels;
    st->codecpar->block_align = avio_rl32(pb);
    if (st->codecpar->block_align > INT_MAX / st->codecpar->ch_layout.nb_channels ||
        st->codecpar->block_align <= 0) {
        avpriv_request_sample(s, "block_align invalid");
        return AVERROR_INVALIDDATA;
    }
    st->codecpar->block_align *= st->codecpar->ch_layout.nb_channels;
    st->codecpar->bit_rate = st->codecpar->ch_layout.nb_channels * 8LL * st->codecpar->sample_rate;
    avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);
    avio_skip(pb, 4);

    /* data chunk */

    dsf->data_end = avio_tell(pb);
    if (avio_rl32(pb) != MKTAG('d', 'a', 't', 'a'))
        return AVERROR_INVALIDDATA;
    dsf->data_size = avio_rl64(pb) - 12;
    dsf->data_end += dsf->data_size + 12;

    return 0;
}

static int dsf_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    FFFormatContext *const si = ffformatcontext(s);
    DSFContext *dsf = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st = s->streams[0];
    int64_t pos = avio_tell(pb);
    int channels = st->codecpar->ch_layout.nb_channels;
    int ret;

    if (pos >= dsf->data_end)
        return AVERROR_EOF;

    if (dsf->data_size > dsf->audio_size) {
        int last_packet = pos == (dsf->data_end - st->codecpar->block_align);

        if (last_packet) {
            int64_t data_pos = pos - si->data_offset;
            int64_t packet_size = dsf->audio_size - data_pos;
            int64_t skip_size = dsf->data_size - data_pos - packet_size;
            uint8_t *dst;
            int ch, ret;

            if (packet_size <= 0 || skip_size <= 0)
                return AVERROR_INVALIDDATA;

            if ((ret = av_new_packet(pkt, packet_size)) < 0)
                return ret;
            dst = pkt->data;
            for (ch = 0; ch < st->codecpar->ch_layout.nb_channels; ch++) {
                ret = avio_read(pb, dst,  packet_size / st->codecpar->ch_layout.nb_channels);
                if (ret < packet_size / st->codecpar->ch_layout.nb_channels)
                    return AVERROR_EOF;

                dst += ret;
                avio_skip(pb, skip_size / st->codecpar->ch_layout.nb_channels);
            }

            pkt->pos = pos;
            pkt->stream_index = 0;
            pkt->pts = (pos - si->data_offset) / channels;
            pkt->duration = packet_size / channels;
            return 0;
        }
    }
    ret = av_get_packet(pb, pkt, FFMIN(dsf->data_end - pos, st->codecpar->block_align));
    if (ret < 0)
        return ret;

    pkt->stream_index = 0;
    pkt->pts = (pos - si->data_offset) / channels;
    pkt->duration = st->codecpar->block_align / channels;

    return 0;
}

const FFInputFormat ff_dsf_demuxer = {
    .p.name         = "dsf",
    .p.long_name    = NULL_IF_CONFIG_SMALL("DSD Stream File (DSF)"),
    .p.flags        = AVFMT_GENERIC_INDEX | AVFMT_NO_BYTE_SEEK,
    .priv_data_size = sizeof(DSFContext),
    .read_probe     = dsf_probe,
    .read_header    = dsf_read_header,
    .read_packet    = dsf_read_packet,
};
