/*
 * Copyright (c) 2010 Niel van der Westhuizen <nielkie@gmail.com>
 * Copyright (c) 2002 A'rpi
 * Copyright (c) 1997-2001 ZSNES Team ( zsknight@zsnes.com / _demo_@zsnes.com )
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with FFmpeg; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * @file
 * Super 2xSaI video filter
 * Ported from MPlayer libmpcodecs/vf_2xsai.c.
 */

#include "libavutil/pixdesc.h"
#include "libavutil/intreadwrite.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"
#include "libavfilter/video.h"

typedef struct Super2xSaIContext {
    /* masks used for two pixels interpolation */
    uint32_t hi_pixel_mask;
    uint32_t lo_pixel_mask;

    /* masks used for four pixels interpolation */
    uint32_t q_hi_pixel_mask;
    uint32_t q_lo_pixel_mask;

    int bpp; ///< bytes per pixel, pixel stride for each (packed) pixel
    int is_be;
} Super2xSaIContext;

typedef struct ThreadData {
    AVFrame *in, *out;
} ThreadData;

#define GET_RESULT(A, B, C, D) ((A != C || A != D) - (B != C || B != D))

#define INTERPOLATE(A, B) (((A & hi_pixel_mask) >> 1) + ((B & hi_pixel_mask) >> 1) + (A & B & lo_pixel_mask))

#define Q_INTERPOLATE(A, B, C, D) ((A & q_hi_pixel_mask) >> 2) + ((B & q_hi_pixel_mask) >> 2) + ((C & q_hi_pixel_mask) >> 2) + ((D & q_hi_pixel_mask) >> 2) \
    + ((((A & q_lo_pixel_mask) + (B & q_lo_pixel_mask) + (C & q_lo_pixel_mask) + (D & q_lo_pixel_mask)) >> 2) & q_lo_pixel_mask)

static int super2xsai(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    Super2xSaIContext *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *in = td->in;
    AVFrame *out = td->out;
    const uint8_t *src = in->data[0];
    uint8_t *dst = out->data[0];
    const int src_linesize = in->linesize[0];
    const int dst_linesize = out->linesize[0];
    const int width = in->width;
    const int height = in->height;
    unsigned int x, y;
    uint32_t color[4][4];
    const uint8_t *src_line[4];
    const int bpp = s->bpp;
    const uint32_t hi_pixel_mask = s->hi_pixel_mask;
    const uint32_t lo_pixel_mask = s->lo_pixel_mask;
    const uint32_t q_hi_pixel_mask = s->q_hi_pixel_mask;
    const uint32_t q_lo_pixel_mask = s->q_lo_pixel_mask;
    const int slice_start = (height * jobnr) / nb_jobs;
    const int slice_end = (height * (jobnr+1)) / nb_jobs;

    /* Point to the first 4 lines, first line is duplicated */
    src_line[0] = src + src_linesize*FFMAX(slice_start - 1, 0);
    src_line[1] = src + src_linesize*slice_start;
    src_line[2] = src + src_linesize*FFMIN(slice_start + 1, height-1);
    src_line[3] = src + src_linesize*FFMIN(slice_start + 2, height-1);

#define READ_COLOR4(dst, src_line, off) dst = *((const uint32_t *)src_line + off)
#define READ_COLOR3(dst, src_line, off) dst = AV_RL24 (src_line + 3*off)
#define READ_COLOR2(dst, src_line, off) dst = s->is_be ? AV_RB16(src_line + 2 * off) : AV_RL16(src_line + 2 * off)

    for (y = slice_start; y < slice_end; y++) {
        uint8_t *dst_line[2];

        dst_line[0] = dst + dst_linesize*2*y;
        dst_line[1] = dst + dst_linesize*(2*y+1);

        switch (bpp) {
        case 4:
            READ_COLOR4(color[0][0], src_line[0], 0); color[0][1] = color[0][0]; READ_COLOR4(color[0][2], src_line[0], 1); READ_COLOR4(color[0][3], src_line[0], 2);
            READ_COLOR4(color[1][0], src_line[1], 0); color[1][1] = color[1][0]; READ_COLOR4(color[1][2], src_line[1], 1); READ_COLOR4(color[1][3], src_line[1], 2);
            READ_COLOR4(color[2][0], src_line[2], 0); color[2][1] = color[2][0]; READ_COLOR4(color[2][2], src_line[2], 1); READ_COLOR4(color[2][3], src_line[2], 2);
            READ_COLOR4(color[3][0], src_line[3], 0); color[3][1] = color[3][0]; READ_COLOR4(color[3][2], src_line[3], 1); READ_COLOR4(color[3][3], src_line[3], 2);
            break;
        case 3:
            READ_COLOR3(color[0][0], src_line[0], 0); color[0][1] = color[0][0]; READ_COLOR3(color[0][2], src_line[0], 1); READ_COLOR3(color[0][3], src_line[0], 2);
            READ_COLOR3(color[1][0], src_line[1], 0); color[1][1] = color[1][0]; READ_COLOR3(color[1][2], src_line[1], 1); READ_COLOR3(color[1][3], src_line[1], 2);
            READ_COLOR3(color[2][0], src_line[2], 0); color[2][1] = color[2][0]; READ_COLOR3(color[2][2], src_line[2], 1); READ_COLOR3(color[2][3], src_line[2], 2);
            READ_COLOR3(color[3][0], src_line[3], 0); color[3][1] = color[3][0]; READ_COLOR3(color[3][2], src_line[3], 1); READ_COLOR3(color[3][3], src_line[3], 2);
            break;
        default:
            READ_COLOR2(color[0][0], src_line[0], 0); color[0][1] = color[0][0]; READ_COLOR2(color[0][2], src_line[0], 1); READ_COLOR2(color[0][3], src_line[0], 2);
            READ_COLOR2(color[1][0], src_line[1], 0); color[1][1] = color[1][0]; READ_COLOR2(color[1][2], src_line[1], 1); READ_COLOR2(color[1][3], src_line[1], 2);
            READ_COLOR2(color[2][0], src_line[2], 0); color[2][1] = color[2][0]; READ_COLOR2(color[2][2], src_line[2], 1); READ_COLOR2(color[2][3], src_line[2], 2);
            READ_COLOR2(color[3][0], src_line[3], 0); color[3][1] = color[3][0]; READ_COLOR2(color[3][2], src_line[3], 1); READ_COLOR2(color[3][3], src_line[3], 2);
        }

        for (x = 0; x < width; x++) {
            uint32_t product1a, product1b, product2a, product2b;

//---------------------------------------  B0 B1 B2 B3    0  1  2  3
//                                         4  5* 6  S2 -> 4  5* 6  7
//                                         1  2  3  S1    8  9 10 11
//                                         A0 A1 A2 A3   12 13 14 15
//--------------------------------------
            if (color[2][1] == color[1][2] && color[1][1] != color[2][2]) {
                product2b = color[2][1];
                product1b = product2b;
            } else if (color[1][1] == color[2][2] && color[2][1] != color[1][2]) {
                product2b = color[1][1];
                product1b = product2b;
            } else if (color[1][1] == color[2][2] && color[2][1] == color[1][2]) {
                int r = 0;

                r += GET_RESULT(color[1][2], color[1][1], color[1][0], color[3][1]);
                r += GET_RESULT(color[1][2], color[1][1], color[2][0], color[0][1]);
                r += GET_RESULT(color[1][2], color[1][1], color[3][2], color[2][3]);
                r += GET_RESULT(color[1][2], color[1][1], color[0][2], color[1][3]);

                if (r > 0)
                    product1b = color[1][2];
                else if (r < 0)
                    product1b = color[1][1];
                else
                    product1b = INTERPOLATE(color[1][1], color[1][2]);

                product2b = product1b;
            } else {
                if (color[1][2] == color[2][2] && color[2][2] == color[3][1] && color[2][1] != color[3][2] && color[2][2] != color[3][0])
                    product2b = Q_INTERPOLATE(color[2][2], color[2][2], color[2][2], color[2][1]);
                else if (color[1][1] == color[2][1] && color[2][1] == color[3][2] && color[3][1] != color[2][2] && color[2][1] != color[3][3])
                    product2b = Q_INTERPOLATE(color[2][1], color[2][1], color[2][1], color[2][2]);
                else
                    product2b = INTERPOLATE(color[2][1], color[2][2]);

                if (color[1][2] == color[2][2] && color[1][2] == color[0][1] && color[1][1] != color[0][2] && color[1][2] != color[0][0])
                    product1b = Q_INTERPOLATE(color[1][2], color[1][2], color[1][2], color[1][1]);
                else if (color[1][1] == color[2][1] && color[1][1] == color[0][2] && color[0][1] != color[1][2] && color[1][1] != color[0][3])
                    product1b = Q_INTERPOLATE(color[1][2], color[1][1], color[1][1], color[1][1]);
                else
                    product1b = INTERPOLATE(color[1][1], color[1][2]);
            }

            if (color[1][1] == color[2][2] && color[2][1] != color[1][2] && color[1][0] == color[1][1] && color[1][1] != color[3][2])
                product2a = INTERPOLATE(color[2][1], color[1][1]);
            else if (color[1][1] == color[2][0] && color[1][2] == color[1][1] && color[1][0] != color[2][1] && color[1][1] != color[3][0])
                product2a = INTERPOLATE(color[2][1], color[1][1]);
            else
                product2a = color[2][1];

            if (color[2][1] == color[1][2] && color[1][1] != color[2][2] && color[2][0] == color[2][1] && color[2][1] != color[0][2])
                product1a = INTERPOLATE(color[2][1], color[1][1]);
            else if (color[1][0] == color[2][1] && color[2][2] == color[2][1] && color[2][0] != color[1][1] && color[2][1] != color[0][0])
                product1a = INTERPOLATE(color[2][1], color[1][1]);
            else
                product1a = color[1][1];

            /* Set the calculated pixels */
            switch (bpp) {
            case 4:
                AV_WN32A(dst_line[0] + x * 8,     product1a);
                AV_WN32A(dst_line[0] + x * 8 + 4, product1b);
                AV_WN32A(dst_line[1] + x * 8,     product2a);
                AV_WN32A(dst_line[1] + x * 8 + 4, product2b);
                break;
            case 3:
                AV_WL24(dst_line[0] + x * 6,     product1a);
                AV_WL24(dst_line[0] + x * 6 + 3, product1b);
                AV_WL24(dst_line[1] + x * 6,     product2a);
                AV_WL24(dst_line[1] + x * 6 + 3, product2b);
                break;
            default: // bpp = 2
                if (s->is_be) {
                    AV_WB32(dst_line[0] + x * 4, product1a | (product1b << 16));
                    AV_WB32(dst_line[1] + x * 4, product2a | (product2b << 16));
                } else {
                    AV_WL32(dst_line[0] + x * 4, product1a | (product1b << 16));
                    AV_WL32(dst_line[1] + x * 4, product2a | (product2b << 16));
                }
            }

            /* Move color matrix forward */
            color[0][0] = color[0][1]; color[0][1] = color[0][2]; color[0][2] = color[0][3];
            color[1][0] = color[1][1]; color[1][1] = color[1][2]; color[1][2] = color[1][3];
            color[2][0] = color[2][1]; color[2][1] = color[2][2]; color[2][2] = color[2][3];
            color[3][0] = color[3][1]; color[3][1] = color[3][2]; color[3][2] = color[3][3];

            if (x < width - 3) {
                x += 3;
                switch (bpp) {
                case 4:
                    READ_COLOR4(color[0][3], src_line[0], x);
                    READ_COLOR4(color[1][3], src_line[1], x);
                    READ_COLOR4(color[2][3], src_line[2], x);
                    READ_COLOR4(color[3][3], src_line[3], x);
                    break;
                case 3:
                    READ_COLOR3(color[0][3], src_line[0], x);
                    READ_COLOR3(color[1][3], src_line[1], x);
                    READ_COLOR3(color[2][3], src_line[2], x);
                    READ_COLOR3(color[3][3], src_line[3], x);
                    break;
                default:        /* case 2 */
                    READ_COLOR2(color[0][3], src_line[0], x);
                    READ_COLOR2(color[1][3], src_line[1], x);
                    READ_COLOR2(color[2][3], src_line[2], x);
                    READ_COLOR2(color[3][3], src_line[3], x);
                }
                x -= 3;
            }
        }

        /* We're done with one line, so we shift the source lines up */
        src_line[0] = src_line[1];
        src_line[1] = src_line[2];
        src_line[2] = src_line[3];

        /* Read next line */
        src_line[3] = src_line[2];
        if (y < height - 3)
            src_line[3] += src_linesize;
    } // y loop

    return 0;
}

static const enum AVPixelFormat pix_fmts[] = {
    AV_PIX_FMT_RGBA, AV_PIX_FMT_BGRA, AV_PIX_FMT_ARGB, AV_PIX_FMT_ABGR,
    AV_PIX_FMT_RGB24, AV_PIX_FMT_BGR24,
    AV_PIX_FMT_RGB565BE, AV_PIX_FMT_BGR565BE, AV_PIX_FMT_RGB555BE, AV_PIX_FMT_BGR555BE,
    AV_PIX_FMT_RGB565LE, AV_PIX_FMT_BGR565LE, AV_PIX_FMT_RGB555LE, AV_PIX_FMT_BGR555LE,
    AV_PIX_FMT_NONE
};

static int config_input(AVFilterLink *inlink)
{
    Super2xSaIContext *s = inlink->dst->priv;

    s->hi_pixel_mask   = 0xFEFEFEFE;
    s->lo_pixel_mask   = 0x01010101;
    s->q_hi_pixel_mask = 0xFCFCFCFC;
    s->q_lo_pixel_mask = 0x03030303;
    s->bpp  = 4;

    switch (inlink->format) {
    case AV_PIX_FMT_RGB24:
    case AV_PIX_FMT_BGR24:
        s->bpp = 3;
        break;

    case AV_PIX_FMT_RGB565BE:
    case AV_PIX_FMT_BGR565BE:
        s->is_be = 1;
    case AV_PIX_FMT_RGB565LE:
    case AV_PIX_FMT_BGR565LE:
        s->hi_pixel_mask   = 0xF7DEF7DE;
        s->lo_pixel_mask   = 0x08210821;
        s->q_hi_pixel_mask = 0xE79CE79C;
        s->q_lo_pixel_mask = 0x18631863;
        s->bpp = 2;
        break;

    case AV_PIX_FMT_BGR555BE:
    case AV_PIX_FMT_RGB555BE:
        s->is_be = 1;
    case AV_PIX_FMT_BGR555LE:
    case AV_PIX_FMT_RGB555LE:
        s->hi_pixel_mask   = 0x7BDE7BDE;
        s->lo_pixel_mask   = 0x04210421;
        s->q_hi_pixel_mask = 0x739C739C;
        s->q_lo_pixel_mask = 0x0C630C63;
        s->bpp = 2;
        break;
    }

    return 0;
}

static int config_output(AVFilterLink *outlink)
{
    AVFilterLink *inlink = outlink->src->inputs[0];

    outlink->w = inlink->w*2;
    outlink->h = inlink->h*2;

    av_log(inlink->dst, AV_LOG_VERBOSE, "fmt:%s size:%dx%d -> size:%dx%d\n",
           av_get_pix_fmt_name(inlink->format),
           inlink->w, inlink->h, outlink->w, outlink->h);

    return 0;
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ThreadData td;
    AVFrame *out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    out->width  = outlink->w;
    out->height = outlink->h;

    td.in = in, td.out = out;
    ff_filter_execute(ctx, super2xsai, &td, NULL,
                      FFMIN(in->height, ff_filter_get_nb_threads(ctx)));

    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}

static const AVFilterPad super2xsai_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_input,
        .filter_frame = filter_frame,
    },
};

static const AVFilterPad super2xsai_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_output,
    },
};

const FFFilter ff_vf_super2xsai = {
    .p.name        = "super2xsai",
    .p.description = NULL_IF_CONFIG_SMALL("Scale the input by 2x using the Super2xSaI pixel art algorithm."),
    .p.flags       = AVFILTER_FLAG_SLICE_THREADS,
    .priv_size     = sizeof(Super2xSaIContext),
    FILTER_INPUTS(super2xsai_inputs),
    FILTER_OUTPUTS(super2xsai_outputs),
    FILTER_PIXFMTS_ARRAY(pix_fmts),
};
