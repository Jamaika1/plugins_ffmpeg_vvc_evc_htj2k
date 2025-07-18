/*
 * Bitmap Brothers JV demuxer
 * Copyright (c) 2005, 2011 Peter Ross <pross@xvid.org>
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
 * Bitmap Brothers JV demuxer
 * @author Peter Ross <pross@xvid.org>
 */

#include "libavutil/channel_layout.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/mem.h"

#include "libavformat/avformat.h"
#include "libavformat/demux.h"
#include "libavformat/internal.h"

#define JV_PREAMBLE_SIZE 5

typedef struct JVFrame {
    int audio_size;    /**< audio packet size (bytes) */
    int video_size;    /**< video packet size (bytes) */
    uint16_t palette_size;  /**< palette size (bytes) */
    uint8_t video_type;     /**< per-frame video compression type */
} JVFrame;

typedef struct JVDemuxContext {
    JVFrame *frames;
    enum {
        JV_AUDIO = 0,
        JV_VIDEO,
        JV_PADDING
    } state;
    int64_t pts;
} JVDemuxContext;

#define MAGIC " Compression by John M Phillips Copyright (C) 1995 The Bitmap Brothers Ltd."

static int read_probe(const AVProbeData *pd)
{
    if (pd->buf[0] == 'J' && pd->buf[1] == 'V' && strlen(MAGIC) + 4 <= pd->buf_size &&
        !memcmp(pd->buf + 4, MAGIC, strlen(MAGIC)))
        return AVPROBE_SCORE_MAX;
    return 0;
}

static int read_close(AVFormatContext *s)
{
    JVDemuxContext *jv = s->priv_data;

    av_freep(&jv->frames);

    return 0;
}

static int read_header(AVFormatContext *s)
{
    JVDemuxContext *jv = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *vst, *ast;
    FFStream *asti;
    int64_t audio_pts = 0;
    int64_t offset;

    avio_skip(pb, 80);

    ast = avformat_new_stream(s, NULL);
    vst = avformat_new_stream(s, NULL);
    if (!ast || !vst)
        return AVERROR(ENOMEM);
    asti = ffstream(ast);

    vst->codecpar->codec_type  = AVMEDIA_TYPE_VIDEO;
    vst->codecpar->codec_id    = AV_CODEC_ID_JV;
    vst->codecpar->codec_tag   = 0; /* no fourcc */
    vst->codecpar->width       = avio_rl16(pb);
    vst->codecpar->height      = avio_rl16(pb);
    vst->duration           =
    vst->nb_frames          =
    asti->nb_index_entries  = avio_rl16(pb);
    avpriv_set_pts_info(vst, 64, avio_rl16(pb), 1000);

    avio_skip(pb, 4);

    ast->codecpar->codec_type     = AVMEDIA_TYPE_AUDIO;
    ast->codecpar->codec_id       = AV_CODEC_ID_PCM_U8;
    ast->codecpar->codec_tag      = 0; /* no fourcc */
    ast->codecpar->sample_rate    = avio_rl16(pb);
    ast->codecpar->ch_layout      = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
    avpriv_set_pts_info(ast, 64, 1, ast->codecpar->sample_rate);

    avio_skip(pb, 10);

    asti->index_entries = av_malloc(asti->nb_index_entries *
                                    sizeof(*asti->index_entries));
    if (!asti->index_entries)
        return AVERROR(ENOMEM);

    jv->frames = av_malloc(asti->nb_index_entries * sizeof(*jv->frames));
    if (!jv->frames)
        return AVERROR(ENOMEM);
    offset = 0x68 + asti->nb_index_entries * 16;
    for (int i = 0; i < asti->nb_index_entries; i++) {
        AVIndexEntry *e   = asti->index_entries + i;
        JVFrame      *jvf = jv->frames + i;

        /* total frame size including audio, video, palette data and padding */
        e->size      = avio_rl32(pb);
        e->timestamp = i;
        e->pos       = offset;
        offset      += e->size;

        jvf->audio_size   = avio_rl32(pb);
        jvf->video_size   = avio_rl32(pb);
        jvf->palette_size = avio_r8(pb) ? 768 : 0;

        if ((jvf->video_size | jvf->audio_size) & ~0xFFFFFF ||
            e->size - jvf->audio_size
                    - jvf->video_size
                    - jvf->palette_size < 0) {
            if (s->error_recognition & AV_EF_EXPLODE)
                return AVERROR_INVALIDDATA;
            jvf->audio_size   =
            jvf->video_size   =
            jvf->palette_size = 0;
        }

        if (avio_r8(pb))
            av_log(s, AV_LOG_WARNING, "unsupported audio codec\n");

        jvf->video_type = avio_r8(pb);
        avio_skip(pb, 1);

        e->timestamp = jvf->audio_size ? audio_pts : AV_NOPTS_VALUE;
        audio_pts   += jvf->audio_size;

        e->flags = jvf->video_type != 1 ? AVINDEX_KEYFRAME : 0;
    }

    jv->state = JV_AUDIO;
    return 0;
}

static int read_packet(AVFormatContext *s, AVPacket *pkt)
{
    JVDemuxContext *jv = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *ast = s->streams[0];
    FFStream *const asti = ffstream(ast);
    int ret;

    while (!avio_feof(s->pb) && jv->pts < asti->nb_index_entries) {
        const AVIndexEntry *const e = asti->index_entries + jv->pts;
        const JVFrame      *jvf = jv->frames + jv->pts;

        switch (jv->state) {
        case JV_AUDIO:
            jv->state++;
            if (jvf->audio_size) {
                if ((ret = av_get_packet(s->pb, pkt, jvf->audio_size)) < 0)
                    return ret;
                pkt->stream_index = 0;
                pkt->pts          = e->timestamp;
                pkt->flags       |= AV_PKT_FLAG_KEY;
                return 0;
            }
        case JV_VIDEO:
            jv->state++;
            if (jvf->video_size || jvf->palette_size) {
                int size = jvf->video_size + jvf->palette_size;
                if ((ret = av_new_packet(pkt, size + JV_PREAMBLE_SIZE)) < 0)
                    return ret;

                AV_WL32(pkt->data, jvf->video_size);
                pkt->data[4] = jvf->video_type;
                ret = avio_read(pb, pkt->data + JV_PREAMBLE_SIZE, size);
                if (ret < 0)
                    return ret;
                if (ret < size) {
                    memset(pkt->data + JV_PREAMBLE_SIZE + ret, 0,
                           AV_INPUT_BUFFER_PADDING_SIZE);
                    pkt->flags |= AV_PKT_FLAG_CORRUPT;
                }
                pkt->size         = ret + JV_PREAMBLE_SIZE;
                pkt->stream_index = 1;
                pkt->pts          = jv->pts;
                if (jvf->video_type != 1)
                    pkt->flags |= AV_PKT_FLAG_KEY;
                return 0;
            }
        case JV_PADDING:
            avio_skip(pb, FFMAX(e->size - jvf->audio_size - jvf->video_size
                                        - jvf->palette_size, 0));
            jv->state = JV_AUDIO;
            jv->pts++;
        }
    }

    if (s->pb->eof_reached)
        return AVERROR_EOF;

    return AVERROR(EIO);
}

static int read_seek(AVFormatContext *s, int stream_index,
                     int64_t ts, int flags)
{
    JVDemuxContext *jv = s->priv_data;
    AVStream *ast = s->streams[0];
    FFStream *const asti = ffstream(ast);
    int i;

    if (flags & (AVSEEK_FLAG_BYTE | AVSEEK_FLAG_FRAME))
        return AVERROR(ENOSYS);

    switch (stream_index) {
    case 0:
        i = av_index_search_timestamp(ast, ts, flags);
        break;
    case 1:
        i = ts;
        break;
    default:
        return 0;
    }

    if (i < 0 || i >= asti->nb_index_entries)
        return 0;
    if (avio_seek(s->pb, asti->index_entries[i].pos, SEEK_SET) < 0)
        return -1;

    jv->state = JV_AUDIO;
    jv->pts   = i;
    return 0;
}

const FFInputFormat ff_jv_demuxer = {
    .p.name         = "jv",
    .p.long_name    = NULL_IF_CONFIG_SMALL("Bitmap Brothers JV"),
    .priv_data_size = sizeof(JVDemuxContext),
    .flags_internal = FF_INFMT_FLAG_INIT_CLEANUP,
    .read_probe     = read_probe,
    .read_header    = read_header,
    .read_packet    = read_packet,
    .read_seek      = read_seek,
    .read_close     = read_close,
};
