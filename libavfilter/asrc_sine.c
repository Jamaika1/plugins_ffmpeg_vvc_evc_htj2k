/*
 * Copyright (c) 2013 Nicolas George
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FFmpeg; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <float.h>

#include "libavutil/avassert.h"
#include "libavutil/channel_layout.h"
#include "libavutil/eval.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavfilter/audio.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"
#include "libavfilter/formats.h"

typedef struct SamplingContext {
    uint32_t phi;  ///< current phase of the sine (2pi = 1<<32)
    uint32_t dphi; ///< phase increment between two samples
    int phi_rem;   ///< current fractional phase in 1/dphi_den subfractions
    int dphi_rem;
    int dphi_den;
} SamplingContext;

typedef struct SineContext {
    const AVClass *class;
    double frequency;
    double beep_factor;
    char *samples_per_frame;
    AVExpr *samples_per_frame_expr;
    int sample_rate;
    int64_t duration;
    int16_t *sin;
    int64_t pts;
    SamplingContext signal;
    SamplingContext beep;
    unsigned beep_period;
    unsigned beep_index;
    unsigned beep_length;
} SineContext;

#define CONTEXT SineContext
#define FLAGS AV_OPT_FLAG_AUDIO_PARAM|AV_OPT_FLAG_FILTERING_PARAM

#define OPT_GENERIC(name, field, def, min, max, descr, type, deffield, ...) \
    { name, descr, offsetof(CONTEXT, field), AV_OPT_TYPE_ ## type,          \
      { .deffield = def }, min, max, FLAGS, __VA_ARGS__ }

#define OPT_INT(name, field, def, min, max, descr, ...) \
    OPT_GENERIC(name, field, def, min, max, descr, INT, i64, __VA_ARGS__)

#define OPT_DBL(name, field, def, min, max, descr, ...) \
    OPT_GENERIC(name, field, def, min, max, descr, DOUBLE, dbl, __VA_ARGS__)

#define OPT_DUR(name, field, def, min, max, descr, ...) \
    OPT_GENERIC(name, field, def, min, max, descr, DURATION, str, __VA_ARGS__)

#define OPT_STR(name, field, def, min, max, descr, ...) \
    OPT_GENERIC(name, field, def, min, max, descr, STRING, str, __VA_ARGS__)

static const AVOption sine_options[] = {
    OPT_DBL("frequency",         frequency,            440, 0, DBL_MAX,   "set the sine frequency",),
    OPT_DBL("f",                 frequency,            440, 0, DBL_MAX,   "set the sine frequency",),
    OPT_DBL("beep_factor",       beep_factor,            0, 0, DBL_MAX,   "set the beep frequency factor",),
    OPT_DBL("b",                 beep_factor,            0, 0, DBL_MAX,   "set the beep frequency factor",),
    OPT_INT("sample_rate",       sample_rate,        44100, 1, INT_MAX,   "set the sample rate",),
    OPT_INT("r",                 sample_rate,        44100, 1, INT_MAX,   "set the sample rate",),
    OPT_DUR("duration",          duration,               0, 0, INT64_MAX, "set the audio duration",),
    OPT_DUR("d",                 duration,               0, 0, INT64_MAX, "set the audio duration",),
    OPT_STR("samples_per_frame", samples_per_frame, "1024", 0, 0,         "set the number of samples per frame",),
    {NULL}
};

AVFILTER_DEFINE_CLASS(sine);

#define LOG_PERIOD 15
#define AMPLITUDE 4095
#define AMPLITUDE_SHIFT 3

static void make_sin_table(int16_t *sin)
{
    unsigned half_pi = 1 << (LOG_PERIOD - 2);
    unsigned ampls = AMPLITUDE << AMPLITUDE_SHIFT;
    uint64_t unit2 = (uint64_t)(ampls * ampls) << 32;
    unsigned step, i, c, s, k, new_k, n2;

    /* Principle: if u = exp(i*a1) and v = exp(i*a2), then
       exp(i*(a1+a2)/2) = (u+v) / length(u+v) */
    sin[0] = 0;
    sin[half_pi] = ampls;
    for (step = half_pi; step > 1; step /= 2) {
        /* k = (1 << 16) * amplitude / length(u+v)
           In exact values, k is constant at a given step */
        k = 0x10000;
        for (i = 0; i < half_pi / 2; i += step) {
            s = sin[i] + sin[i + step];
            c = sin[half_pi - i] + sin[half_pi - i - step];
            n2 = s * s + c * c;
            /* Newton's method to solve n² * k² = unit² */
            while (1) {
                new_k = (k + unit2 / ((uint64_t)k * n2) + 1) >> 1;
                if (k == new_k)
                    break;
                k = new_k;
            }
            sin[i + step / 2] = (k * s + 0x7FFF) >> 16;
            sin[half_pi - i - step / 2] = (k * c + 0x8000) >> 16;
        }
    }
    /* Unshift amplitude */
    for (i = 0; i <= half_pi; i++)
        sin[i] = (sin[i] + (1 << (AMPLITUDE_SHIFT - 1))) >> AMPLITUDE_SHIFT;
    /* Use symmetries to fill the other three quarters */
    for (i = 0; i < half_pi; i++)
        sin[half_pi * 2 - i] = sin[i];
    for (i = 0; i < 2 * half_pi; i++)
        sin[i + 2 * half_pi] = -sin[i];
}

static const char *const var_names[] = {
    "n",
    "pts",
    "t",
    "TB",
    NULL
};

enum {
    VAR_N,
    VAR_PTS,
    VAR_T,
    VAR_TB,
    VAR_VARS_NB
};

static void sampling_init(SamplingContext *c, double frequency, int sample_rate)
{
    AVRational r;
    int r_den, max_r_den;

    max_r_den   = INT_MAX / sample_rate;
    frequency   = fmod(frequency, sample_rate);
    r           = av_d2q(fmod(frequency, 1.0), max_r_den);
    r_den       = FFMIN(r.den, max_r_den);
    c->dphi     = ldexp(frequency, 32) / sample_rate;
    c->dphi_den = r_den * sample_rate;
    c->dphi_rem = round((ldexp(frequency, 32) / sample_rate - c->dphi) * c->dphi_den);
    if (c->dphi_rem >= c->dphi_den) {
        c->dphi++;
        c->dphi_rem = 0;
    }
    c->phi_rem  = (-c->dphi_den - 1) / 2;
}

static av_always_inline void sampling_advance(SamplingContext *c)
{
    c->phi += c->dphi;
    c->phi_rem += c->dphi_rem;
    if (c->phi_rem >= 0) {
        c->phi_rem -= c->dphi_den;
        c->phi++;
    }
}

static av_cold int init(AVFilterContext *ctx)
{
    int ret;
    SineContext *sine = ctx->priv;

    if (!(sine->sin = av_malloc(sizeof(*sine->sin) << LOG_PERIOD)))
        return AVERROR(ENOMEM);
    sampling_init(&sine->signal, sine->frequency, sine->sample_rate);
    make_sin_table(sine->sin);

    if (sine->beep_factor) {
        sine->beep_period = sine->sample_rate;
        sine->beep_length = sine->beep_period / 25;
        sampling_init(&sine->beep, sine->beep_factor * sine->frequency, sine->sample_rate);
    }

    ret = av_expr_parse(&sine->samples_per_frame_expr,
                        sine->samples_per_frame, var_names,
                        NULL, NULL, NULL, NULL, 0, sine);
    if (ret < 0)
        return ret;

    return 0;
}

static av_cold void uninit(AVFilterContext *ctx)
{
    SineContext *sine = ctx->priv;

    av_expr_free(sine->samples_per_frame_expr);
    sine->samples_per_frame_expr = NULL;
    av_freep(&sine->sin);
}

static av_cold int query_formats(const AVFilterContext *ctx,
                                 AVFilterFormatsConfig **cfg_in,
                                 AVFilterFormatsConfig **cfg_out)
{
    const SineContext *sine = ctx->priv;
    static const AVChannelLayout chlayouts[] = { AV_CHANNEL_LAYOUT_MONO, { 0 } };
    int sample_rates[] = { sine->sample_rate, -1 };
    static const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_S16,
                                                       AV_SAMPLE_FMT_NONE };
    int ret = ff_set_common_formats_from_list2(ctx, cfg_in, cfg_out, sample_fmts);
    if (ret < 0)
        return ret;

    ret = ff_set_common_channel_layouts_from_list2(ctx, cfg_in, cfg_out, chlayouts);
    if (ret < 0)
        return ret;

    return ff_set_common_samplerates_from_list2(ctx, cfg_in, cfg_out, sample_rates);
}

static av_cold int config_props(AVFilterLink *outlink)
{
    SineContext *sine = outlink->src->priv;
    sine->duration = av_rescale(sine->duration, sine->sample_rate, AV_TIME_BASE);
    return 0;
}

static int activate(AVFilterContext *ctx)
{
    AVFilterLink *outlink = ctx->outputs[0];
    FilterLink *outl = ff_filter_link(outlink);
    SineContext *sine = ctx->priv;
    AVFrame *frame;
    double values[VAR_VARS_NB] = {
        [VAR_N]   = outl->frame_count_in,
        [VAR_PTS] = sine->pts,
        [VAR_T]   = sine->pts * av_q2d(outlink->time_base),
        [VAR_TB]  = av_q2d(outlink->time_base),
    };
    int i, nb_samples = lrint(av_expr_eval(sine->samples_per_frame_expr, values, sine));
    int16_t *samples;

    if (!ff_outlink_frame_wanted(outlink))
        return FFERROR_NOT_READY;
    if (nb_samples <= 0) {
        av_log(sine, AV_LOG_WARNING, "nb samples expression evaluated to %d, "
               "defaulting to 1024\n", nb_samples);
        nb_samples = 1024;
    }

    if (sine->duration) {
        nb_samples = FFMIN(nb_samples, sine->duration - sine->pts);
        av_assert1(nb_samples >= 0);
        if (!nb_samples) {
            ff_outlink_set_status(outlink, AVERROR_EOF, sine->pts);
            return 0;
        }
    }
    if (!(frame = ff_get_audio_buffer(outlink, nb_samples)))
        return AVERROR(ENOMEM);
    samples = (int16_t *)frame->data[0];

    for (i = 0; i < nb_samples; i++) {
        samples[i] = sine->sin[sine->signal.phi >> (32 - LOG_PERIOD)];
        sampling_advance(&sine->signal);
        if (sine->beep_index < sine->beep_length) {
            samples[i] += sine->sin[sine->beep.phi >> (32 - LOG_PERIOD)] * 2;
            sampling_advance(&sine->beep);
        }
        if (++sine->beep_index == sine->beep_period)
            sine->beep_index = 0;
    }

    frame->pts = sine->pts;
    sine->pts += nb_samples;
    return ff_filter_frame(outlink, frame);
}

static const AVFilterPad sine_outputs[] = {
    {
        .name          = "default",
        .type          = AVMEDIA_TYPE_AUDIO,
        .config_props  = config_props,
    },
};

const FFFilter ff_asrc_sine = {
    .p.name        = "sine",
    .p.description = NULL_IF_CONFIG_SMALL("Generate sine wave audio signal."),
    .p.priv_class  = &sine_class,
    .init          = init,
    .uninit        = uninit,
    .activate      = activate,
    .priv_size     = sizeof(SineContext),
    FILTER_OUTPUTS(sine_outputs),
    FILTER_QUERY_FUNC2(query_formats),
};
