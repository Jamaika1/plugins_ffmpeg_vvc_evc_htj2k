/*
 * Shorten demuxer
 * Copyright (c) 2001 Fabrice Bellard
 * Copyright (c) 2005 Alex Beregszaszi
 * Copyright (c) 2015 Carl Eugen Hoyos
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
#include "libavformat/demux.h"
#include "libavformat/rawdec.h"
#include "libavcodec/golomb.h"

static int shn_probe(const AVProbeData *p)
{
    GetBitContext gb;
    int version, internal_ftype, channels, blocksize;

    if (AV_RB32(p->buf) != 0x616a6b67)
        return 0;
    version = p->buf[4];
    if (init_get_bits8(&gb, p->buf + 5, p->buf_size - 5 - AV_INPUT_BUFFER_PADDING_SIZE) < 0)
        return 0;
    if (!version) {
        internal_ftype = get_ur_golomb_shorten(&gb, 4);
        channels = get_ur_golomb_shorten(&gb, 0);
        blocksize = 256;
    } else {
        unsigned k;
        k = get_ur_golomb_shorten(&gb, 2);
        if (k > 31)
            return 0;
        internal_ftype = get_ur_golomb_shorten(&gb, k);
        k = get_ur_golomb_shorten(&gb, 2);
        if (k > 31)
            return 0;
        channels = get_ur_golomb_shorten(&gb, k);
        k = get_ur_golomb_shorten(&gb, 2);
        if (k > 31)
            return 0;
        blocksize = get_ur_golomb_shorten(&gb, k);
    }

    if (internal_ftype != 2 && internal_ftype != 3 && internal_ftype != 5)
        return 0;
    if (channels < 1 || channels > 8)
        return 0;
    if (blocksize < 1 || blocksize > 65535)
        return 0;

    return AVPROBE_SCORE_EXTENSION + 1;
}

const FFInputFormat ff_shorten_demuxer = {
    .p.name         = "shn",
    .p.long_name    = NULL_IF_CONFIG_SMALL("raw Shorten"),
    .p.flags        = AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK | AVFMT_NOTIMESTAMPS,
    .p.extensions   = "shn",
    .p.priv_class   = &ff_raw_demuxer_class,
    .read_probe     = shn_probe,
    .read_header    = ff_raw_audio_read_header,
    .read_packet    = ff_raw_read_partial_packet,
    .raw_codec_id   = AV_CODEC_ID_SHORTEN,
    .priv_data_size = sizeof(FFRawDemuxerContext),
};
