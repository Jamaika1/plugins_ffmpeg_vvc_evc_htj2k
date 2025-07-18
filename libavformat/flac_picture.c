/*
 * Raw FLAC picture parser
 * Copyright (c) 2001 Fabrice Bellard
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
#include "libavcodec/bytestream.h"
#include "libavcodec/png.h"
#include "libavformat/avformat.h"
#include "libavformat/demux.h"
#include "libavformat/flac_picture.h"
#include "libavformat/id3v2.h"
#include "libavformat/internal.h"

#define MAX_TRUNC_PICTURE_SIZE (500 * 1024 * 1024)

int ff_flac_parse_picture(AVFormatContext *s, uint8_t **bufp, int buf_size,
                          int truncate_workaround)
{
    const CodecMime *mime = ff_id3v2_mime_tags;
    enum AVCodecID id = AV_CODEC_ID_NONE;
    AVBufferRef *data = NULL;
    uint8_t mimetype[64], *buf = *bufp;
    const uint8_t *desc = NULL;
    GetByteContext g;
    AVStream *st;
    int width, height, ret = 0;
    unsigned int type;
    uint32_t len, left, trunclen = 0;

    if (buf_size < 34) {
        av_log(s, AV_LOG_ERROR, "Attached picture metadata block too short\n");
        if (s->error_recognition & AV_EF_EXPLODE)
            return AVERROR_INVALIDDATA;
        return 0;
    }

    bytestream2_init(&g, buf, buf_size);

    /* read the picture type */
    type = bytestream2_get_be32u(&g);
    if (type >= FF_ARRAY_ELEMS(ff_id3v2_picture_types)) {
        av_log(s, AV_LOG_ERROR, "Invalid picture type: %d.\n", type);
        if (s->error_recognition & AV_EF_EXPLODE) {
            return AVERROR_INVALIDDATA;
        }
        type = 0;
    }

    /* picture mimetype */
    len = bytestream2_get_be32u(&g);
    if (len <= 0 || len >= sizeof(mimetype)) {
        av_log(s, AV_LOG_ERROR, "Could not read mimetype from an attached "
               "picture.\n");
        if (s->error_recognition & AV_EF_EXPLODE)
            return AVERROR_INVALIDDATA;
        return 0;
    }
    if (len + 24 > bytestream2_get_bytes_left(&g)) {
        av_log(s, AV_LOG_ERROR, "Attached picture metadata block too short\n");
        if (s->error_recognition & AV_EF_EXPLODE)
            return AVERROR_INVALIDDATA;
        return 0;
    }
    bytestream2_get_bufferu(&g, mimetype, len);
    mimetype[len] = 0;

    while (mime->id != AV_CODEC_ID_NONE) {
        if (!strncmp(mime->str, mimetype, sizeof(mimetype))) {
            id = mime->id;
            break;
        }
        mime++;
    }
    if (id == AV_CODEC_ID_NONE) {
        av_log(s, AV_LOG_WARNING, "Unknown attached picture mimetype: %s.\n",
               mimetype);
        return 0;
    }

    /* picture description */
    len = bytestream2_get_be32u(&g);
    if (len > bytestream2_get_bytes_left(&g) - 20) {
        av_log(s, AV_LOG_ERROR, "Attached picture metadata block too short\n");
        if (s->error_recognition & AV_EF_EXPLODE)
            return AVERROR_INVALIDDATA;
        return 0;
    }
    if (len > 0) {
        desc = g.buffer;
        bytestream2_skipu(&g, len);
    }

    /* picture metadata */
    width  = bytestream2_get_be32u(&g);
    ((uint8_t*)g.buffer)[-4] = '\0';   // NUL-terminate desc.
    height = bytestream2_get_be32u(&g);
    bytestream2_skipu(&g, 8);

    /* picture data */
    len = bytestream2_get_be32u(&g);

    left = bytestream2_get_bytes_left(&g);
    if (len <= 0 || len > left) {
        if (len > MAX_TRUNC_PICTURE_SIZE || len >= INT_MAX - AV_INPUT_BUFFER_PADDING_SIZE) {
            av_log(s, AV_LOG_ERROR, "Attached picture metadata block too big %u\n", len);
            if (s->error_recognition & AV_EF_EXPLODE)
                return AVERROR_INVALIDDATA;
            return 0;
        }

        // Workaround bug for flac muxers that writs truncated metadata picture block size if
        // the picture size do not fit in 24 bits. lavf flacenc used to have the issue and based
        // on existing broken files other unknown flac muxers seems to truncate also.
        if (truncate_workaround &&
            s->strict_std_compliance <= FF_COMPLIANCE_NORMAL &&
            len > left && (len & 0xffffff) == left) {
            av_log(s, AV_LOG_INFO, "Correcting truncated metadata picture size from %u to %u\n", left, len);
            trunclen = len - left;
        } else {
            av_log(s, AV_LOG_ERROR, "Attached picture metadata block too short\n");
            if (s->error_recognition & AV_EF_EXPLODE)
                return AVERROR_INVALIDDATA;
            return 0;
        }
    }
    if (trunclen == 0 && len >= buf_size - (buf_size >> 4)) {
        data = av_buffer_create(buf, buf_size + AV_INPUT_BUFFER_PADDING_SIZE,
                                av_buffer_default_free, NULL, 0);
        if (!data)
            return AVERROR(ENOMEM);
        *bufp = NULL;
        data->data += bytestream2_tell(&g);
        data->size  = len + AV_INPUT_BUFFER_PADDING_SIZE;
    } else {
        if (!(data = av_buffer_alloc(len + AV_INPUT_BUFFER_PADDING_SIZE)))
            return AVERROR(ENOMEM);

        if (trunclen == 0) {
            bytestream2_get_bufferu(&g, data->data, len);
        } else {
            // If truncation was detected copy all data from block and
            // read missing bytes not included in the block size.
            bytestream2_get_bufferu(&g, data->data, left);
            if (avio_read(s->pb, data->data + len - trunclen, trunclen) < trunclen)
                RETURN_ERROR(AVERROR_INVALIDDATA);
        }
    }
    memset(data->data + len, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    if (AV_RB64(data->data) == PNGSIG)
        id = AV_CODEC_ID_PNG;

    ret = ff_add_attached_pic(s, NULL, NULL, &data, 0);
    if (ret < 0)
        RETURN_ERROR(ret);

    st = s->streams[s->nb_streams - 1];
    st->codecpar->codec_id   = id;
    st->codecpar->width      = width;
    st->codecpar->height     = height;
    av_dict_set(&st->metadata, "comment", ff_id3v2_picture_types[type], 0);
    if (desc)
        av_dict_set(&st->metadata, "title", desc, 0);

    return 0;

fail:
    av_buffer_unref(&data);

    return ret;
}
