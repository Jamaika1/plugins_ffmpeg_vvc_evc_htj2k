/*
 * THP Demuxer
 * Copyright (c) 2007 Marco Gerards
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

#include "libavutil/intreadwrite.h"
#include "libavutil/intfloat.h"
#include "libavformat/avformat.h"
#include "libavformat/avio_internal.h"
#include "libavformat/demux.h"
#include "libavformat/internal.h"

typedef struct ThpDemuxContext {
    int              version;
    unsigned         first_frame;
    unsigned         first_framesz;
    unsigned         last_frame;
    int              compoff;
    unsigned         framecnt;
    AVRational       fps;
    unsigned         frame;
    int64_t          next_frame;
    unsigned         next_framesz;
    int              video_stream_index;
    int              audio_stream_index;
    int              compcount;
    unsigned char    components[16];
    AVStream*        vst;
    int              has_audio;
    unsigned         audiosize;
} ThpDemuxContext;


static int thp_probe(const AVProbeData *p)
{
    double d;
    /* check file header */
    if (AV_RL32(p->buf) != MKTAG('T', 'H', 'P', '\0'))
        return 0;

    d = av_int2float(AV_RB32(p->buf + 16));
    if (d < 0.1 || d > 1000 || isnan(d))
        return AVPROBE_SCORE_MAX/4;

    return AVPROBE_SCORE_MAX;
}

static int thp_read_header(AVFormatContext *s)
{
    ThpDemuxContext *thp = s->priv_data;
    AVStream *st;
    AVIOContext *pb = s->pb;
    int64_t fsize= avio_size(pb);
    uint32_t maxsize;
    int i;

    /* Read the file header.  */
                           avio_rb32(pb); /* Skip Magic.  */
    thp->version         = avio_rb32(pb);

                           avio_rb32(pb); /* Max buf size.  */
                           avio_rb32(pb); /* Max samples.  */

    thp->fps             = av_d2q(av_int2float(avio_rb32(pb)), INT_MAX);
    if (thp->fps.den <= 0 || thp->fps.num < 0)
        return AVERROR_INVALIDDATA;
    thp->framecnt        = avio_rb32(pb);
    thp->first_framesz   = avio_rb32(pb);
    maxsize              = avio_rb32(pb);
    if (fsize > 0 && (!maxsize || fsize < maxsize))
        maxsize = fsize;
    ffiocontext(pb)->maxsize = fsize;

    thp->compoff         = avio_rb32(pb);
                           avio_rb32(pb); /* offsetDataOffset.  */
    thp->first_frame     = avio_rb32(pb);
    thp->last_frame      = avio_rb32(pb);

    thp->next_framesz    = thp->first_framesz;
    thp->next_frame      = thp->first_frame;

    /* Read the component structure.  */
    avio_seek (pb, thp->compoff, SEEK_SET);
    thp->compcount       = avio_rb32(pb);

    if (thp->compcount > FF_ARRAY_ELEMS(thp->components))
        return AVERROR_INVALIDDATA;

    /* Read the list of component types.  */
    avio_read(pb, thp->components, 16);

    for (i = 0; i < thp->compcount; i++) {
        if (thp->components[i] == 0) {
            if (thp->vst)
                break;

            /* Video component.  */
            st = avformat_new_stream(s, NULL);
            if (!st)
                return AVERROR(ENOMEM);

            /* The denominator and numerator are switched because 1/fps
               is required.  */
            avpriv_set_pts_info(st, 64, thp->fps.den, thp->fps.num);
            st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
            st->codecpar->codec_id = AV_CODEC_ID_THP;
            st->codecpar->codec_tag = 0;  /* no fourcc */
            st->codecpar->width = avio_rb32(pb);
            st->codecpar->height = avio_rb32(pb);
            st->codecpar->sample_rate = av_q2d(thp->fps);
            st->nb_frames =
            st->duration = thp->framecnt;
            thp->vst = st;
            thp->video_stream_index = st->index;

            if (thp->version == 0x11000)
                avio_rb32(pb); /* Unknown.  */
        } else if (thp->components[i] == 1) {
            if (thp->has_audio != 0)
                break;

            /* Audio component.  */
            st = avformat_new_stream(s, NULL);
            if (!st)
                return AVERROR(ENOMEM);

            st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
            st->codecpar->codec_id = AV_CODEC_ID_ADPCM_THP;
            st->codecpar->codec_tag = 0;  /* no fourcc */
            st->codecpar->ch_layout.nb_channels = avio_rb32(pb);
            st->codecpar->sample_rate = avio_rb32(pb); /* Frequency.  */
            st->duration           = avio_rb32(pb);

            avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);

            thp->audio_stream_index = st->index;
            thp->has_audio = 1;
        }
    }

    if (!thp->vst)
        return AVERROR_INVALIDDATA;

    return 0;
}

static int thp_read_packet(AVFormatContext *s,
                            AVPacket *pkt)
{
    ThpDemuxContext *thp = s->priv_data;
    AVIOContext *pb = s->pb;
    unsigned int size;
    int ret;

    if (thp->audiosize == 0) {
        /* Terminate when last frame is reached.  */
        if (thp->frame >= thp->framecnt)
            return AVERROR_EOF;

        avio_seek(pb, thp->next_frame, SEEK_SET);

        /* Locate the next frame and read out its size.  */
        thp->next_frame += FFMAX(thp->next_framesz, 1);
        thp->next_framesz = avio_rb32(pb);

                        avio_rb32(pb); /* Previous total size.  */
        size          = avio_rb32(pb); /* Total size of this frame.  */

        /* Store the audiosize so the next time this function is called,
           the audio can be read.  */
        if (thp->has_audio)
            thp->audiosize = avio_rb32(pb); /* Audio size.  */
        else
            thp->frame++;

        ret = av_get_packet(pb, pkt, size);
        if (ret < 0)
            return ret;
        if (ret != size) {
            return AVERROR(EIO);
        }

        pkt->stream_index = thp->video_stream_index;
    } else {
        ret = av_get_packet(pb, pkt, thp->audiosize);
        if (ret < 0)
            return ret;
        if (ret != thp->audiosize) {
            return AVERROR(EIO);
        }

        pkt->stream_index = thp->audio_stream_index;
        if (thp->audiosize >= 8)
            pkt->duration = AV_RB32(&pkt->data[4]);

        thp->audiosize = 0;
        thp->frame++;
    }

    return 0;
}

const FFInputFormat ff_thp_demuxer = {
    .p.name         = "thp",
    .p.long_name    = NULL_IF_CONFIG_SMALL("THP"),
    .priv_data_size = sizeof(ThpDemuxContext),
    .read_probe     = thp_probe,
    .read_header    = thp_read_header,
    .read_packet    = thp_read_packet
};
