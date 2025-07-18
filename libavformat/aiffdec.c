/*
 * AIFF/AIFF-C demuxer
 * Copyright (c) 2006  Patrick Guimond
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
#include "libavutil/dict.h"
#include "libavutil/mem.h"
#include "libavformat/avformat.h"
#include "libavformat/demux.h"
#include "libavformat/internal.h"
#include "libavformat/pcm.h"
#include "libavformat/aiff.h"
#include "libavformat/id3v2.h"
#include "libavformat/mov_chan.h"
#include "libavformat/replaygain.h"

#define AIFF                    0
#define AIFF_C_VERSION1         0xA2805140

typedef struct AIFFInputContext {
    int64_t data_end;
    int block_duration;
} AIFFInputContext;

static enum AVCodecID aiff_codec_get_id(int bps)
{
    if (bps <= 8)
        return AV_CODEC_ID_PCM_S8;
    if (bps <= 16)
        return AV_CODEC_ID_PCM_S16BE;
    if (bps <= 24)
        return AV_CODEC_ID_PCM_S24BE;
    if (bps <= 32)
        return AV_CODEC_ID_PCM_S32BE;

    /* bigger than 32 isn't allowed  */
    return AV_CODEC_ID_NONE;
}

/* returns the size of the found tag */
static int64_t get_tag(AVIOContext *pb, uint32_t * tag)
{
    int64_t size;

    if (avio_feof(pb))
        return AVERROR(EIO);

    *tag = avio_rl32(pb);
    size = avio_rb32(pb);

    return size;
}

/* Metadata string read */
static void get_meta(AVFormatContext *s, const char *key, int64_t size)
{
    uint8_t *str = NULL;

    if (size < SIZE_MAX)
        str = av_malloc(size+1);

    if (str) {
        int res = avio_read(s->pb, str, size);
        if (res < 0){
            av_free(str);
            return;
        }
        size -= res;
        str[res] = 0;
        av_dict_set(&s->metadata, key, str, AV_DICT_DONT_STRDUP_VAL);
    }

    avio_skip(s->pb, size);
}

/* Returns the number of sound data frames or negative on error */
static int get_aiff_header(AVFormatContext *s, int64_t size,
                                    unsigned version)
{
    AVIOContext *pb        = s->pb;
    AVCodecParameters *par = s->streams[0]->codecpar;
    AIFFInputContext *aiff = s->priv_data;
    int exp;
    uint64_t val;
    int sample_rate;
    unsigned int num_frames;
    int channels;

    if (size & 1)
        size++;
    par->codec_type = AVMEDIA_TYPE_AUDIO;
    channels = avio_rb16(pb);
    if (par->ch_layout.nb_channels && par->ch_layout.nb_channels != channels)
        return AVERROR_INVALIDDATA;
    par->ch_layout.nb_channels = channels;
    num_frames = avio_rb32(pb);
    par->bits_per_coded_sample = avio_rb16(pb);

    exp = avio_rb16(pb) - 16383 - 63;
    val = avio_rb64(pb);
    if (exp <-63 || exp >63) {
        av_log(s, AV_LOG_ERROR, "exp %d is out of range\n", exp);
        return AVERROR_INVALIDDATA;
    }
    if (exp >= 0)
        sample_rate = val << exp;
    else
        sample_rate = (val + (1ULL<<(-exp-1))) >> -exp;
    if (sample_rate <= 0)
        return AVERROR_INVALIDDATA;

    par->sample_rate = sample_rate;
    if (size < 18)
        return AVERROR_INVALIDDATA;
    size -= 18;

    /* get codec id for AIFF-C */
    if (size < 4) {
        version = AIFF;
    } else if (version == AIFF_C_VERSION1) {
        par->codec_tag = avio_rl32(pb);
        par->codec_id  = ff_codec_get_id(ff_codec_aiff_tags, par->codec_tag);
        if (par->codec_id == AV_CODEC_ID_NONE)
            avpriv_request_sample(s, "unknown or unsupported codec tag: %s",
                                  av_fourcc2str(par->codec_tag));
        size -= 4;
    }

    if (version != AIFF_C_VERSION1 || par->codec_id == AV_CODEC_ID_PCM_S16BE) {
        par->codec_id = aiff_codec_get_id(par->bits_per_coded_sample);
        par->bits_per_coded_sample = av_get_bits_per_sample(par->codec_id);
        aiff->block_duration = 1;
    } else {
        switch (par->codec_id) {
        case AV_CODEC_ID_PCM_F32BE:
        case AV_CODEC_ID_PCM_F64BE:
        case AV_CODEC_ID_PCM_S16LE:
        case AV_CODEC_ID_PCM_ALAW:
        case AV_CODEC_ID_PCM_MULAW:
            aiff->block_duration = 1;
            break;
        case AV_CODEC_ID_ADPCM_IMA_QT:
            par->block_align = 34 * channels;
            break;
        case AV_CODEC_ID_MACE3:
            par->block_align = 2 * channels;
            break;
        case AV_CODEC_ID_ADPCM_G726LE:
            par->bits_per_coded_sample = 5;
        case AV_CODEC_ID_ADPCM_IMA_WS:
        case AV_CODEC_ID_ADPCM_G722:
        case AV_CODEC_ID_MACE6:
        case AV_CODEC_ID_CBD2_DPCM:
        case AV_CODEC_ID_SDX2_DPCM:
            par->block_align = 1 * channels;
            break;
        case AV_CODEC_ID_GSM:
            par->block_align = 33;
            break;
        case AV_CODEC_ID_G728:
            par->block_align = 5;
            break;
        default:
            aiff->block_duration = 1;
            break;
        }
        if (par->block_align > 0)
            aiff->block_duration = av_get_audio_frame_duration2(par,
                                                                par->block_align);
    }

    /* Block align needs to be computed in all cases, as the definition
     * is specific to applications -> here we use the WAVE format definition */
    if (!par->block_align)
        par->block_align = (av_get_bits_per_sample(par->codec_id) * channels) >> 3;

    if (aiff->block_duration) {
        par->bit_rate = av_rescale(par->sample_rate, par->block_align * 8LL,
                                   aiff->block_duration);
        if (par->bit_rate < 0)
            par->bit_rate = 0;
    }

    /* Chunk is over */
    if (size)
        avio_skip(pb, size);

    return num_frames;
}

static int aiff_probe(const AVProbeData *p)
{
    /* check file header */
    if (AV_RL32(p->buf) == MKTAG('F', 'O', 'R', 'M') &&
        AV_RB32(p->buf + 4) >= 4 &&
        p->buf[8] == 'A' && p->buf[9] == 'I' &&
        p->buf[10] == 'F' && (p->buf[11] == 'F' || p->buf[11] == 'C'))
        return AVPROBE_SCORE_MAX;
    else
        return 0;
}

/* aiff input */
static int aiff_read_header(AVFormatContext *s)
{
    int ret;
    int64_t filesize, size;
    int64_t offset = 0, position;
    uint32_t tag;
    unsigned version = AIFF_C_VERSION1;
    AVIOContext *pb = s->pb;
    AVStream * st;
    AIFFInputContext *aiff = s->priv_data;
    ID3v2ExtraMeta *id3v2_extra_meta;

    /* check FORM header */
    filesize = get_tag(pb, &tag);
    if (filesize < 4 || tag != MKTAG('F', 'O', 'R', 'M'))
        return AVERROR_INVALIDDATA;

    /* AIFF data type */
    tag = avio_rl32(pb);
    if (tag == MKTAG('A', 'I', 'F', 'F'))       /* Got an AIFF file */
        version = AIFF;
    else if (tag != MKTAG('A', 'I', 'F', 'C'))  /* An AIFF-C file then */
        return AVERROR_INVALIDDATA;

    filesize -= 4;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    while (filesize > 0) {
        /* parse different chunks */
        size = get_tag(pb, &tag);

        if (size == AVERROR_EOF && offset > 0 && st->codecpar->block_align) {
            av_log(s, AV_LOG_WARNING, "header parser hit EOF\n");
            goto got_sound;
        }
        if (size < 0)
            return size;

        filesize -= size + 8;

        switch (tag) {
        case MKTAG('C', 'O', 'M', 'M'):     /* Common chunk */
            /* Then for the complete header info */
            st->nb_frames = get_aiff_header(s, size, version);
            if (st->nb_frames < 0)
                return st->nb_frames;
            if (offset > 0) // COMM is after SSND
                goto got_sound;
            break;
        case MKTAG('I', 'D', '3', ' '):
            position = avio_tell(pb);
            ff_id3v2_read(s, ID3v2_DEFAULT_MAGIC, &id3v2_extra_meta, size);
            if (id3v2_extra_meta)
                if ((ret = ff_id3v2_parse_apic(s, id3v2_extra_meta)) < 0 ||
                    (ret = ff_id3v2_parse_chapters(s, id3v2_extra_meta)) < 0) {
                    ff_id3v2_free_extra_meta(&id3v2_extra_meta);
                    return ret;
                }
            ff_id3v2_free_extra_meta(&id3v2_extra_meta);
            if (position + size > avio_tell(pb))
                avio_skip(pb, position + size - avio_tell(pb));
            break;
        case MKTAG('F', 'V', 'E', 'R'):     /* Version chunk */
            version = avio_rb32(pb);
            break;
        case MKTAG('N', 'A', 'M', 'E'):     /* Sample name chunk */
            get_meta(s, "title"    , size);
            break;
        case MKTAG('A', 'U', 'T', 'H'):     /* Author chunk */
            get_meta(s, "author"   , size);
            break;
        case MKTAG('(', 'c', ')', ' '):     /* Copyright chunk */
            get_meta(s, "copyright", size);
            break;
        case MKTAG('A', 'N', 'N', 'O'):     /* Annotation chunk */
            get_meta(s, "comment"  , size);
            break;
        case MKTAG('S', 'S', 'N', 'D'):     /* Sampled sound chunk */
            if (size < 8)
                return AVERROR_INVALIDDATA;
            aiff->data_end = avio_tell(pb) + size;
            offset = avio_rb32(pb);      /* Offset of sound data */
            avio_rb32(pb);               /* BlockSize... don't care */
            offset += avio_tell(pb);    /* Compute absolute data offset */
            if (st->codecpar->block_align && !(pb->seekable & AVIO_SEEKABLE_NORMAL))    /* Assume COMM already parsed */
                goto got_sound;
            if (!(pb->seekable & AVIO_SEEKABLE_NORMAL)) {
                av_log(s, AV_LOG_ERROR, "file is not seekable\n");
                return -1;
            }
            avio_skip(pb, size - 8);
            break;
        case MKTAG('w', 'a', 'v', 'e'):
            if ((uint64_t)size > (1<<30))
                return AVERROR_INVALIDDATA;
            if ((ret = ff_get_extradata(s, st->codecpar, pb, size)) < 0)
                return ret;
            if (   (st->codecpar->codec_id == AV_CODEC_ID_QDMC || st->codecpar->codec_id == AV_CODEC_ID_QDM2)
                && size>=12*4 && !st->codecpar->block_align) {
                st->codecpar->block_align = AV_RB32(st->codecpar->extradata+11*4);
                aiff->block_duration = AV_RB32(st->codecpar->extradata+9*4);
            } else if (st->codecpar->codec_id == AV_CODEC_ID_QCELP) {
                char rate = 0;
                if (size >= 25)
                    rate = st->codecpar->extradata[24];
                switch (rate) {
                case 'H': // RATE_HALF
                    st->codecpar->block_align = 17;
                    break;
                case 'F': // RATE_FULL
                default:
                    st->codecpar->block_align = 35;
                }
                aiff->block_duration = 160;
                st->codecpar->bit_rate = (int64_t)st->codecpar->sample_rate * (st->codecpar->block_align << 3) /
                                         aiff->block_duration;
            }
            break;
        case MKTAG('C','H','A','N'):
            if ((ret = ff_mov_read_chan(s, pb, st, size)) < 0)
                return ret;
            break;
        case MKTAG('A','P','C','M'): /* XA ADPCM compressed sound chunk */
            st->codecpar->codec_id = AV_CODEC_ID_ADPCM_XA;
            aiff->data_end = avio_tell(pb) + size;
            offset = avio_tell(pb) + 8;
            /* This field is unknown and its data seems to be irrelevant */
            avio_rb32(pb);
            st->codecpar->block_align = avio_rb32(pb);

            goto got_sound;
            break;
        case 0:
            if (offset > 0 && st->codecpar->block_align) // COMM && SSND
                goto got_sound;
        default: /* Jump */
            avio_skip(pb, size);
        }

        /* Skip required padding byte for odd-sized chunks. */
        if (size & 1) {
            filesize--;
            avio_skip(pb, 1);
        }
    }

    ret = ff_replaygain_export(st, s->metadata);
    if (ret < 0)
        return ret;

got_sound:
    if (!st->codecpar->block_align && st->codecpar->codec_id == AV_CODEC_ID_QCELP) {
        av_log(s, AV_LOG_WARNING, "qcelp without wave chunk, assuming full rate\n");
        st->codecpar->block_align = 35;
    } else if (st->codecpar->block_align <= 0) {
        av_log(s, AV_LOG_ERROR, "could not find COMM tag or invalid block_align value\n");
        return AVERROR_INVALIDDATA;
    }
    if (aiff->block_duration < 0)
        return AVERROR_INVALIDDATA;

    /* Now positioned, get the sound data start and end */
    avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);
    st->start_time = 0;
    st->duration = st->nb_frames * aiff->block_duration;

    /* Position the stream at the first block */
    avio_seek(pb, offset, SEEK_SET);

    return 0;
}

#define MAX_SIZE 4096

static int aiff_read_packet(AVFormatContext *s,
                            AVPacket *pkt)
{
    AVStream *st = s->streams[0];
    AIFFInputContext *aiff = s->priv_data;
    int64_t max_size;
    int res, size;

    /* calculate size of remaining data */
    max_size = aiff->data_end - avio_tell(s->pb);
    if (max_size <= 0)
        return AVERROR_EOF;

    if (!st->codecpar->block_align) {
        av_log(s, AV_LOG_ERROR, "block_align not set\n");
        return AVERROR_INVALIDDATA;
    }

    /* Now for that packet */
    switch (st->codecpar->codec_id) {
    case AV_CODEC_ID_ADPCM_IMA_QT:
    case AV_CODEC_ID_GSM:
    case AV_CODEC_ID_QDM2:
    case AV_CODEC_ID_QCELP:
        size = st->codecpar->block_align;
        break;
    default:
        size = st->codecpar->block_align ? (MAX_SIZE / st->codecpar->block_align) * st->codecpar->block_align : MAX_SIZE;
        if (!size)
            return AVERROR_INVALIDDATA;
    }
    size = FFMIN(max_size, size);
    res = av_get_packet(s->pb, pkt, size);
    if (res < 0)
        return res;

    if (size >= st->codecpar->block_align)
        pkt->flags &= ~AV_PKT_FLAG_CORRUPT;
    /* Only one stream in an AIFF file */
    pkt->stream_index = 0;
    pkt->duration     = (res / st->codecpar->block_align) * (int64_t) aiff->block_duration;
    return 0;
}

const FFInputFormat ff_aiff_demuxer = {
    .p.name         = "aiff",
    .p.long_name    = NULL_IF_CONFIG_SMALL("Audio IFF"),
    .p.codec_tag    = ff_aiff_codec_tags_list,
    .priv_data_size = sizeof(AIFFInputContext),
    .read_probe     = aiff_probe,
    .read_header    = aiff_read_header,
    .read_packet    = aiff_read_packet,
    .read_seek      = ff_pcm_read_seek,
};
