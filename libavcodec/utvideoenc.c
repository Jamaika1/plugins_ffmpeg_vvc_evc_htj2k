/*
 * Ut Video encoder
 * Copyright (c) 2012 Jan Ekström
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
 * Ut Video encoder
 */

#include "libavutil/avassert.h"
#include "libavutil/imgutils.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"

#include "libavcodec/avcodec.h"
#include "libavcodec/codec_internal.h"
#include "libavcodec/encode.h"
#include "libavcodec/bswapdsp.h"
#include "libavcodec/bytestream.h"
#include "libavcodec/lossless_videoencdsp.h"
#include "libavcodec/put_bits.h"
#include "libavcodec/utvideo.h"
#include "libavcodec/huffman.h"

typedef struct UtvideoContext {
    const AVClass *class;
    BswapDSPContext bdsp;
    LLVidEncDSPContext llvidencdsp;

    uint32_t frame_info_size, flags;
    int      planes;
    int      slices;
    int      compression;
    int      frame_pred;

    ptrdiff_t slice_stride;
    uint8_t  *slice_bits, *slice_buffer[4];
    int       slice_bits_size;
} UtvideoContext;

typedef struct HuffEntry {
    uint16_t sym;
    uint8_t  len;
    uint32_t code;
} HuffEntry;

/* Compare huffman tree nodes */
static int ut_huff_cmp_len(const void *a, const void *b)
{
    const HuffEntry *aa = a, *bb = b;
    return (aa->len - bb->len)*256 + aa->sym - bb->sym;
}

/* Compare huffentry symbols */
static int huff_cmp_sym(const void *a, const void *b)
{
    const HuffEntry *aa = a, *bb = b;
    return aa->sym - bb->sym;
}

static av_cold int utvideo_encode_close(AVCodecContext *avctx)
{
    UtvideoContext *c = avctx->priv_data;
    int i;

    av_freep(&c->slice_bits);
    for (i = 0; i < 4; i++)
        av_freep(&c->slice_buffer[i]);

    return 0;
}

static av_cold int utvideo_encode_init(AVCodecContext *avctx)
{
    UtvideoContext *c = avctx->priv_data;
    int i, subsampled_height;
    uint32_t original_format;

    c->frame_info_size = 4;
    c->slice_stride    = FFALIGN(avctx->width, 32);

    switch (avctx->pix_fmt) {
    case AV_PIX_FMT_GBRP:
        c->planes        = 3;
        avctx->codec_tag = MKTAG('U', 'L', 'R', 'G');
        original_format  = UTVIDEO_RGB;
        break;
    case AV_PIX_FMT_GBRAP:
        c->planes        = 4;
        avctx->codec_tag = MKTAG('U', 'L', 'R', 'A');
        original_format  = UTVIDEO_RGBA;
        avctx->bits_per_coded_sample = 32;
        break;
    case AV_PIX_FMT_YUV420P:
        if (avctx->width & 1 || avctx->height & 1) {
            av_log(avctx, AV_LOG_ERROR,
                   "4:2:0 video requires even width and height.\n");
            return AVERROR_INVALIDDATA;
        }
        c->planes        = 3;
        if (avctx->colorspace == AVCOL_SPC_BT709)
            avctx->codec_tag = MKTAG('U', 'L', 'H', '0');
        else
            avctx->codec_tag = MKTAG('U', 'L', 'Y', '0');
        original_format  = UTVIDEO_420;
        break;
    case AV_PIX_FMT_YUV422P:
        if (avctx->width & 1) {
            av_log(avctx, AV_LOG_ERROR,
                   "4:2:2 video requires even width.\n");
            return AVERROR_INVALIDDATA;
        }
        c->planes        = 3;
        if (avctx->colorspace == AVCOL_SPC_BT709)
            avctx->codec_tag = MKTAG('U', 'L', 'H', '2');
        else
            avctx->codec_tag = MKTAG('U', 'L', 'Y', '2');
        original_format  = UTVIDEO_422;
        break;
    case AV_PIX_FMT_YUV444P:
        c->planes        = 3;
        if (avctx->colorspace == AVCOL_SPC_BT709)
            avctx->codec_tag = MKTAG('U', 'L', 'H', '4');
        else
            avctx->codec_tag = MKTAG('U', 'L', 'Y', '4');
        original_format  = UTVIDEO_444;
        break;
    default:
        av_unreachable("Already checked via CODEC_PIXFMTS");
    }

    ff_bswapdsp_init(&c->bdsp);
    ff_llvidencdsp_init(&c->llvidencdsp);

    if (c->frame_pred == PRED_GRADIENT) {
        av_log(avctx, AV_LOG_ERROR, "Gradient prediction is not supported.\n");
        return AVERROR_PATCHWELCOME;
    }

    /*
     * Check the asked slice count for obviously invalid
     * values (> 256 or negative).
     */
    if (avctx->slices > 256 || avctx->slices < 0) {
        av_log(avctx, AV_LOG_ERROR,
               "Slice count %d is not supported in Ut Video (theoretical range is 0-256).\n",
               avctx->slices);
        return AVERROR(EINVAL);
    }

    /* Check that the slice count is not larger than the subsampled height */
    subsampled_height = avctx->height >> av_pix_fmt_desc_get(avctx->pix_fmt)->log2_chroma_h;
    if (avctx->slices > subsampled_height) {
        av_log(avctx, AV_LOG_ERROR,
               "Slice count %d is larger than the subsampling-applied height %d.\n",
               avctx->slices, subsampled_height);
        return AVERROR(EINVAL);
    }

    /* extradata size is 4 * 32 bits */
    avctx->extradata_size = 16;

    avctx->extradata = av_mallocz(avctx->extradata_size +
                                  AV_INPUT_BUFFER_PADDING_SIZE);

    if (!avctx->extradata) {
        av_log(avctx, AV_LOG_ERROR, "Could not allocate extradata.\n");
        return AVERROR(ENOMEM);
    }

    for (i = 0; i < c->planes; i++) {
        c->slice_buffer[i] = av_malloc(c->slice_stride * (avctx->height + 2) +
                                       AV_INPUT_BUFFER_PADDING_SIZE);
        if (!c->slice_buffer[i]) {
            av_log(avctx, AV_LOG_ERROR, "Cannot allocate temporary buffer 1.\n");
            return AVERROR(ENOMEM);
        }
    }

    /*
     * Set the version of the encoder.
     * Last byte is "implementation ID", which is
     * obtained from the creator of the format.
     * Libavcodec has been assigned with the ID 0xF0.
     */
    AV_WB32(avctx->extradata, MKTAG(1, 0, 0, 0xF0));

    /*
     * Set the "original format"
     * Not used for anything during decoding.
     */
    AV_WL32(avctx->extradata + 4, original_format);

    /* Write 4 as the 'frame info size' */
    AV_WL32(avctx->extradata + 8, c->frame_info_size);

    /*
     * Set how many slices are going to be used.
     * By default uses multiple slices depending on the subsampled height.
     * This enables multithreading in the official decoder.
     */
    if (!avctx->slices) {
        c->slices = subsampled_height / 120;

        if (!c->slices)
            c->slices = 1;
        else if (c->slices > 256)
            c->slices = 256;
    } else {
        c->slices = avctx->slices;
    }

    /* Set compression mode */
    c->compression = COMP_HUFF;

    /*
     * Set the encoding flags:
     * - Slice count minus 1
     * - Interlaced encoding mode flag, set to zero for now.
     * - Compression mode (none/huff)
     * And write the flags.
     */
    c->flags  = (c->slices - 1U) << 24;
    c->flags |= 0 << 11; // bit field to signal interlaced encoding mode
    c->flags |= c->compression;

    AV_WL32(avctx->extradata + 12, c->flags);

    return 0;
}

static void mangle_rgb_planes(uint8_t *dst[4], ptrdiff_t dst_stride,
                              uint8_t *const src[4], int planes, const int stride[4],
                              int width, int height)
{
    int i, j;
    int k = 2 * dst_stride;
    const uint8_t *sg = src[0];
    const uint8_t *sb = src[1];
    const uint8_t *sr = src[2];
    const uint8_t *sa = src[3];
    unsigned int g;

    for (j = 0; j < height; j++) {
        if (planes == 3) {
            for (i = 0; i < width; i++) {
                g         = sg[i];
                dst[0][k] = g;
                g        += 0x80;
                dst[1][k] = sb[i] - g;
                dst[2][k] = sr[i] - g;
                k++;
            }
        } else {
            for (i = 0; i < width; i++) {
                g         = sg[i];
                dst[0][k] = g;
                g        += 0x80;
                dst[1][k] = sb[i] - g;
                dst[2][k] = sr[i] - g;
                dst[3][k] = sa[i];
                k++;
            }
            sa += stride[3];
        }
        k += dst_stride - width;
        sg += stride[0];
        sb += stride[1];
        sr += stride[2];
    }
}

#undef A
#undef B

/* Write data to a plane with median prediction */
static void median_predict(UtvideoContext *c, const uint8_t *src, uint8_t *dst,
                           ptrdiff_t stride, int width, int height)
{
    int i, j;
    int A, B;
    uint8_t prev;

    /* First line uses left neighbour prediction */
    prev = 0x80; /* Set the initial value */
    for (i = 0; i < width; i++) {
        *dst++ = src[i] - prev;
        prev   = src[i];
    }

    if (height == 1)
        return;

    src += stride;

    /*
     * Second line uses top prediction for the first sample,
     * and median for the rest.
     */
    A = B = 0;

    /* Rest of the coded part uses median prediction */
    for (j = 1; j < height; j++) {
        c->llvidencdsp.sub_median_pred(dst, src - stride, src, width, &A, &B);
        dst += width;
        src += stride;
    }
}

/* Count the usage of values in a plane */
static void count_usage(uint8_t *src, int width,
                        int height, uint64_t *counts)
{
    int i, j;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            counts[src[i]]++;
        }
        src += width;
    }
}

/* Calculate the actual huffman codes from the code lengths */
static void calculate_codes(HuffEntry *he)
{
    int last, i;
    uint32_t code;

    qsort(he, 256, sizeof(*he), ut_huff_cmp_len);

    last = 255;
    while (he[last].len == 255 && last)
        last--;

    code = 0;
    for (i = last; i >= 0; i--) {
        he[i].code  = code >> (32 - he[i].len);
        code       += 0x80000000u >> (he[i].len - 1);
    }

    qsort(he, 256, sizeof(*he), huff_cmp_sym);
}

/* Write huffman bit codes to a memory block */
static int write_huff_codes(uint8_t *src, uint8_t *dst, int dst_size,
                            int width, int height, HuffEntry *he)
{
    PutBitContext pb;
    int i, j;
    int count;

    init_put_bits(&pb, dst, dst_size);

    /* Write the codes */
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++)
            put_bits(&pb, he[src[i]].len, he[src[i]].code);

        src += width;
    }

    /* Pad output to a 32-bit boundary */
    count = put_bits_count(&pb) & 0x1F;

    if (count)
        put_bits(&pb, 32 - count, 0);

    /* Flush the rest with zeroes */
    flush_put_bits(&pb);

    /* Return the amount of bytes written */
    return put_bytes_output(&pb);
}

static int encode_plane(AVCodecContext *avctx, const uint8_t *src,
                        uint8_t *dst, ptrdiff_t stride, int plane_no,
                        int width, int height, PutByteContext *pb)
{
    UtvideoContext *c        = avctx->priv_data;
    uint8_t  lengths[256];
    uint64_t counts[256]     = { 0 };

    HuffEntry he[256];

    uint32_t offset = 0, slice_len = 0;
    const int cmask = ~(!plane_no && avctx->pix_fmt == AV_PIX_FMT_YUV420P);
    int      i, sstart, send = 0;
    int      symbol;
    int      ret;

    /* Do prediction / make planes */
    switch (c->frame_pred) {
    case PRED_NONE:
        for (i = 0; i < c->slices; i++) {
            sstart = send;
            send   = height * (i + 1) / c->slices & cmask;
            av_image_copy_plane(dst + sstart * width, width,
                                src + sstart * stride, stride,
                                width, send - sstart);
        }
        break;
    case PRED_LEFT:
        for (i = 0; i < c->slices; i++) {
            sstart = send;
            send   = height * (i + 1) / c->slices & cmask;
            c->llvidencdsp.sub_left_predict(dst + sstart * width, src + sstart * stride, stride, width, send - sstart);
        }
        break;
    case PRED_MEDIAN:
        for (i = 0; i < c->slices; i++) {
            sstart = send;
            send   = height * (i + 1) / c->slices & cmask;
            median_predict(c, src + sstart * stride, dst + sstart * width,
                           stride, width, send - sstart);
        }
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unknown prediction mode: %d\n",
               c->frame_pred);
        return AVERROR_OPTION_NOT_FOUND;
    }

    /* Count the usage of values */
    count_usage(dst, width, height, counts);

    /* Check for a special case where only one symbol was used */
    for (symbol = 0; symbol < 256; symbol++) {
        /* If non-zero count is found, see if it matches width * height */
        if (counts[symbol]) {
            /* Special case if only one symbol was used */
            if (counts[symbol] == width * (int64_t)height) {
                /*
                 * Write a zero for the single symbol
                 * used in the plane, else 0xFF.
                 */
                for (i = 0; i < 256; i++) {
                    if (i == symbol)
                        bytestream2_put_byte(pb, 0);
                    else
                        bytestream2_put_byte(pb, 0xFF);
                }

                /* Write zeroes for lengths */
                for (i = 0; i < c->slices; i++)
                    bytestream2_put_le32(pb, 0);

                /* And that's all for that plane folks */
                return 0;
            }
            break;
        }
    }

    /* Calculate huffman lengths */
    if ((ret = ff_huff_gen_len_table(lengths, counts, 256, 1)) < 0)
        return ret;

    /*
     * Write the plane's header into the output packet:
     * - huffman code lengths (256 bytes)
     * - slice end offsets (gotten from the slice lengths)
     */
    for (i = 0; i < 256; i++) {
        bytestream2_put_byte(pb, lengths[i]);

        he[i].len = lengths[i];
        he[i].sym = i;
    }

    /* Calculate the huffman codes themselves */
    calculate_codes(he);

    send = 0;
    for (i = 0; i < c->slices; i++) {
        sstart  = send;
        send    = height * (i + 1) / c->slices & cmask;

        /*
         * Write the huffman codes to a buffer,
         * get the offset in bytes.
         */
        offset += write_huff_codes(dst + sstart * width, c->slice_bits,
                                   width * height + 4, width,
                                   send - sstart, he);

        slice_len = offset - slice_len;

        /* Byteswap the written huffman codes */
        c->bdsp.bswap_buf((uint32_t *) c->slice_bits,
                          (uint32_t *) c->slice_bits,
                          slice_len >> 2);

        /* Write the offset to the stream */
        bytestream2_put_le32(pb, offset);

        /* Seek to the data part of the packet */
        bytestream2_seek_p(pb, 4 * (c->slices - i - 1) +
                           offset - slice_len, SEEK_CUR);

        /* Write the slices' data into the output packet */
        bytestream2_put_buffer(pb, c->slice_bits, slice_len);

        /* Seek back to the slice offsets */
        bytestream2_seek_p(pb, -4 * (c->slices - i - 1) - offset,
                           SEEK_CUR);

        slice_len = offset;
    }

    /* And at the end seek to the end of written slice(s) */
    bytestream2_seek_p(pb, offset, SEEK_CUR);

    return 0;
}

static int utvideo_encode_frame(AVCodecContext *avctx, AVPacket *pkt,
                                const AVFrame *pic, int *got_packet)
{
    UtvideoContext *c = avctx->priv_data;
    PutByteContext pb;

    uint32_t frame_info;

    uint8_t *dst;

    int width = avctx->width, height = avctx->height;
    int i, ret = 0;

    /* Allocate a new packet if needed, and set it to the pointer dst */
    ret = ff_alloc_packet(avctx, pkt, (256 + 4 * c->slices + width * height)
                                      * c->planes + 4);

    if (ret < 0)
        return ret;

    dst = pkt->data;

    bytestream2_init_writer(&pb, dst, pkt->size);

    av_fast_padded_malloc(&c->slice_bits, &c->slice_bits_size, width * height + 4);

    if (!c->slice_bits) {
        av_log(avctx, AV_LOG_ERROR, "Cannot allocate temporary buffer 2.\n");
        return AVERROR(ENOMEM);
    }

    /* In case of RGB, mangle the planes to Ut Video's format */
    if (avctx->pix_fmt == AV_PIX_FMT_GBRAP || avctx->pix_fmt == AV_PIX_FMT_GBRP)
        mangle_rgb_planes(c->slice_buffer, c->slice_stride, pic->data,
                          c->planes, pic->linesize, width, height);

    /* Deal with the planes */
    switch (avctx->pix_fmt) {
    case AV_PIX_FMT_GBRP:
    case AV_PIX_FMT_GBRAP:
        for (i = 0; i < c->planes; i++) {
            ret = encode_plane(avctx, c->slice_buffer[i] + 2 * c->slice_stride,
                               c->slice_buffer[i], c->slice_stride, i,
                               width, height, &pb);

            if (ret) {
                av_log(avctx, AV_LOG_ERROR, "Error encoding plane %d.\n", i);
                return ret;
            }
        }
        break;
    case AV_PIX_FMT_YUV444P:
        for (i = 0; i < c->planes; i++) {
            ret = encode_plane(avctx, pic->data[i], c->slice_buffer[0],
                               pic->linesize[i], i, width, height, &pb);

            if (ret) {
                av_log(avctx, AV_LOG_ERROR, "Error encoding plane %d.\n", i);
                return ret;
            }
        }
        break;
    case AV_PIX_FMT_YUV422P:
        for (i = 0; i < c->planes; i++) {
            ret = encode_plane(avctx, pic->data[i], c->slice_buffer[0],
                               pic->linesize[i], i, width >> !!i, height, &pb);

            if (ret) {
                av_log(avctx, AV_LOG_ERROR, "Error encoding plane %d.\n", i);
                return ret;
            }
        }
        break;
    case AV_PIX_FMT_YUV420P:
        for (i = 0; i < c->planes; i++) {
            ret = encode_plane(avctx, pic->data[i], c->slice_buffer[0],
                               pic->linesize[i], i, width >> !!i, height >> !!i,
                               &pb);

            if (ret) {
                av_log(avctx, AV_LOG_ERROR, "Error encoding plane %d.\n", i);
                return ret;
            }
        }
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unknown pixel format: %d\n",
               avctx->pix_fmt);
        return AVERROR_INVALIDDATA;
    }

    /*
     * Write frame information (LE 32-bit unsigned)
     * into the output packet.
     * Contains the prediction method.
     */
    frame_info = c->frame_pred << 8;
    bytestream2_put_le32(&pb, frame_info);

    pkt->size   = bytestream2_tell_p(&pb);

    /* Packet should be done */
    *got_packet = 1;

    return 0;
}

#define OFFSET(x) offsetof(UtvideoContext, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
{ "pred", "Prediction method", OFFSET(frame_pred), AV_OPT_TYPE_INT, { .i64 = PRED_LEFT }, PRED_NONE, PRED_MEDIAN, VE, .unit = "pred" },
    { "none",     NULL, 0, AV_OPT_TYPE_CONST, { .i64 = PRED_NONE }, INT_MIN, INT_MAX, VE, .unit = "pred" },
    { "left",     NULL, 0, AV_OPT_TYPE_CONST, { .i64 = PRED_LEFT }, INT_MIN, INT_MAX, VE, .unit = "pred" },
    { "median",   NULL, 0, AV_OPT_TYPE_CONST, { .i64 = PRED_MEDIAN }, INT_MIN, INT_MAX, VE, .unit = "pred" },

    { NULL},
};

static const AVClass utvideo_class = {
    .class_name = "utvideo",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

const FFCodec ff_utvideo_encoder = {
    .p.name         = "utvideo",
    CODEC_LONG_NAME("Ut Video"),
    .p.type         = AVMEDIA_TYPE_VIDEO,
    .p.id           = AV_CODEC_ID_UTVIDEO,
    .p.capabilities = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_FRAME_THREADS |
                      AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE,
    .priv_data_size = sizeof(UtvideoContext),
    .p.priv_class   = &utvideo_class,
    .init           = utvideo_encode_init,
    FF_CODEC_ENCODE_CB(utvideo_encode_frame),
    .close          = utvideo_encode_close,
    CODEC_PIXFMTS(AV_PIX_FMT_GBRP, AV_PIX_FMT_GBRAP,
                  AV_PIX_FMT_YUV422P, AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUV444P),
    .color_ranges   = AVCOL_RANGE_MPEG,
    .caps_internal  = FF_CODEC_CAP_INIT_CLEANUP,
};
