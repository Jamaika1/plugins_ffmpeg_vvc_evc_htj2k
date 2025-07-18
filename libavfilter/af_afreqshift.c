/*
 * Copyright (c) Paul B Mahol
 * Copyright (c) Laurent de Soras, 2005
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
#include "libavutil/ffmath.h"
#include "libavutil/opt.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/audio.h"
#include "libavfilter/filters.h"

#define MAX_NB_COEFFS 16

typedef struct AFreqShift {
    const AVClass *class;

    double shift;
    double level;
    int nb_coeffs;
    int old_nb_coeffs;

    double cd[MAX_NB_COEFFS * 2];
    float cf[MAX_NB_COEFFS * 2];

    int64_t in_samples;

    AVFrame *i1, *o1;
    AVFrame *i2, *o2;

    void (*filter_channel)(AVFilterContext *ctx,
                           int channel,
                           AVFrame *in, AVFrame *out);
} AFreqShift;

static const enum AVSampleFormat sample_fmts[] = {
    AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_DBLP, AV_SAMPLE_FMT_NONE
};

#define PFILTER(name, type, sin, cos, cc)                     \
static void pfilter_channel_## name(AVFilterContext *ctx,     \
                            int ch,                           \
                            AVFrame *in, AVFrame *out)        \
{                                                             \
    AFreqShift *s = ctx->priv;                                \
    const int nb_samples = in->nb_samples;                    \
    const type *src = (const type *)in->extended_data[ch];    \
    type *dst = (type *)out->extended_data[ch];               \
    type *i1 = (type *)s->i1->extended_data[ch];              \
    type *o1 = (type *)s->o1->extended_data[ch];              \
    type *i2 = (type *)s->i2->extended_data[ch];              \
    type *o2 = (type *)s->o2->extended_data[ch];              \
    const int nb_coeffs = s->nb_coeffs;                       \
    const type *c = s->cc;                                    \
    const type level = s->level;                              \
    type shift = s->shift * M_PI;                             \
    type cos_theta = cos(shift);                              \
    type sin_theta = sin(shift);                              \
                                                              \
    for (int n = 0; n < nb_samples; n++) {                    \
        type xn1 = src[n], xn2 = src[n];                      \
        type I, Q;                                            \
                                                              \
        for (int j = 0; j < nb_coeffs; j++) {                 \
            I = c[j] * (xn1 + o2[j]) - i2[j];                 \
            i2[j] = i1[j];                                    \
            i1[j] = xn1;                                      \
            o2[j] = o1[j];                                    \
            o1[j] = I;                                        \
            xn1 = I;                                          \
        }                                                     \
                                                              \
        for (int j = nb_coeffs; j < nb_coeffs*2; j++) {       \
            Q = c[j] * (xn2 + o2[j]) - i2[j];                 \
            i2[j] = i1[j];                                    \
            i1[j] = xn2;                                      \
            o2[j] = o1[j];                                    \
            o1[j] = Q;                                        \
            xn2 = Q;                                          \
        }                                                     \
        Q = o2[nb_coeffs * 2 - 1];                            \
                                                              \
        dst[n] = (I * cos_theta - Q * sin_theta) * level;     \
    }                                                         \
}

PFILTER(flt, float, sin, cos, cf)
PFILTER(dbl, double, sin, cos, cd)

#define FFILTER(name, type, sin, cos, fmod, cc)               \
static void ffilter_channel_## name(AVFilterContext *ctx,     \
                            int ch,                           \
                            AVFrame *in, AVFrame *out)        \
{                                                             \
    AFreqShift *s = ctx->priv;                                \
    const int nb_samples = in->nb_samples;                    \
    const type *src = (const type *)in->extended_data[ch];    \
    type *dst = (type *)out->extended_data[ch];               \
    type *i1 = (type *)s->i1->extended_data[ch];              \
    type *o1 = (type *)s->o1->extended_data[ch];              \
    type *i2 = (type *)s->i2->extended_data[ch];              \
    type *o2 = (type *)s->o2->extended_data[ch];              \
    const int nb_coeffs = s->nb_coeffs;                       \
    const type *c = s->cc;                                    \
    const type level = s->level;                              \
    type ts = 1. / in->sample_rate;                           \
    type shift = s->shift;                                    \
    int64_t N = s->in_samples;                                \
                                                              \
    for (int n = 0; n < nb_samples; n++) {                    \
        type xn1 = src[n], xn2 = src[n];                      \
        type I, Q, theta;                                     \
                                                              \
        for (int j = 0; j < nb_coeffs; j++) {                 \
            I = c[j] * (xn1 + o2[j]) - i2[j];                 \
            i2[j] = i1[j];                                    \
            i1[j] = xn1;                                      \
            o2[j] = o1[j];                                    \
            o1[j] = I;                                        \
            xn1 = I;                                          \
        }                                                     \
                                                              \
        for (int j = nb_coeffs; j < nb_coeffs*2; j++) {       \
            Q = c[j] * (xn2 + o2[j]) - i2[j];                 \
            i2[j] = i1[j];                                    \
            i1[j] = xn2;                                      \
            o2[j] = o1[j];                                    \
            o1[j] = Q;                                        \
            xn2 = Q;                                          \
        }                                                     \
        Q = o2[nb_coeffs * 2 - 1];                            \
                                                              \
        theta = 2. * M_PI * fmod(shift * (N + n) * ts, 1.);   \
        dst[n] = (I * cos(theta) - Q * sin(theta)) * level;   \
    }                                                         \
}

FFILTER(flt, float, sinf, cosf, fmodf, cf)
FFILTER(dbl, double, sin, cos, fmod, cd)

static void compute_transition_param(double *K, double *Q, double transition)
{
    double kksqrt, e, e2, e4, k, q;

    k  = tan((1. - transition * 2.) * M_PI / 4.);
    k *= k;
    kksqrt = pow(1 - k * k, 0.25);
    e = 0.5 * (1. - kksqrt) / (1. + kksqrt);
    e2 = e * e;
    e4 = e2 * e2;
    q = e * (1. + e4 * (2. + e4 * (15. + 150. * e4)));

    *Q = q;
    *K = k;
}

static double ipowp(double x, int64_t n)
{
    double z = 1.;

    while (n != 0) {
        if (n & 1)
            z *= x;
        n >>= 1;
        x *= x;
    }

    return z;
}

static double compute_acc_num(double q, int order, int c)
{
    int64_t i = 0;
    int j = 1;
    double acc = 0.;
    double q_ii1;

    do {
        q_ii1  = ipowp(q, i * (i + 1));
        q_ii1 *= sin((i * 2 + 1) * c * M_PI / order) * j;
        acc   += q_ii1;

        j = -j;
        i++;
    } while (fabs(q_ii1) > 1e-100);

    return acc;
}

static double compute_acc_den(double q, int order, int c)
{
    int64_t i = 1;
    int j = -1;
    double acc = 0.;
    double q_i2;

    do {
        q_i2  = ipowp(q, i * i);
        q_i2 *= cos(i * 2 * c * M_PI / order) * j;
        acc  += q_i2;

        j = -j;
        i++;
    } while (fabs(q_i2) > 1e-100);

    return acc;
}

static double compute_coef(int index, double k, double q, int order)
{
    const int    c    = index + 1;
    const double num  = compute_acc_num(q, order, c) * pow(q, 0.25);
    const double den  = compute_acc_den(q, order, c) + 0.5;
    const double ww   = num / den;
    const double wwsq = ww * ww;

    const double x    = sqrt((1 - wwsq * k) * (1 - wwsq / k)) / (1 + wwsq);
    const double coef = (1 - x) / (1 + x);

    return coef;
}

static void compute_coefs(double *coef_arrd, float *coef_arrf, int nbr_coefs, double transition)
{
    const int order = nbr_coefs * 2 + 1;
    double k, q;

    compute_transition_param(&k, &q, transition);

    for (int n = 0; n < nbr_coefs; n++) {
        const int idx = (n / 2) + (n & 1) * nbr_coefs / 2;

        coef_arrd[idx] = compute_coef(n, k, q, order);
        coef_arrf[idx] = coef_arrd[idx];
    }
}

static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    AFreqShift *s = ctx->priv;

    if (s->old_nb_coeffs != s->nb_coeffs)
        compute_coefs(s->cd, s->cf, s->nb_coeffs * 2, 2. * 20. / inlink->sample_rate);
    s->old_nb_coeffs = s->nb_coeffs;

    s->i1 = ff_get_audio_buffer(inlink, MAX_NB_COEFFS * 2);
    s->o1 = ff_get_audio_buffer(inlink, MAX_NB_COEFFS * 2);
    s->i2 = ff_get_audio_buffer(inlink, MAX_NB_COEFFS * 2);
    s->o2 = ff_get_audio_buffer(inlink, MAX_NB_COEFFS * 2);
    if (!s->i1 || !s->o1 || !s->i2 || !s->o2)
        return AVERROR(ENOMEM);

    if (inlink->format == AV_SAMPLE_FMT_DBLP) {
        if (!strcmp(ctx->filter->name, "afreqshift"))
            s->filter_channel = ffilter_channel_dbl;
        else
            s->filter_channel = pfilter_channel_dbl;
    } else {
        if (!strcmp(ctx->filter->name, "afreqshift"))
            s->filter_channel = ffilter_channel_flt;
        else
            s->filter_channel = pfilter_channel_flt;
    }

    return 0;
}

typedef struct ThreadData {
    AVFrame *in, *out;
} ThreadData;

static int filter_channels(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    AFreqShift *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *out = td->out;
    AVFrame *in = td->in;
    const int start = (in->ch_layout.nb_channels * jobnr) / nb_jobs;
    const int end = (in->ch_layout.nb_channels * (jobnr+1)) / nb_jobs;

    for (int ch = start; ch < end; ch++)
        s->filter_channel(ctx, ch, in, out);

    return 0;
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AFreqShift *s = ctx->priv;
    AVFrame *out;
    ThreadData td;

    if (s->old_nb_coeffs != s->nb_coeffs)
        compute_coefs(s->cd, s->cf, s->nb_coeffs * 2, 2. * 20. / inlink->sample_rate);
    s->old_nb_coeffs = s->nb_coeffs;

    if (av_frame_is_writable(in)) {
        out = in;
    } else {
        out = ff_get_audio_buffer(outlink, in->nb_samples);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }

    td.in = in; td.out = out;
    ff_filter_execute(ctx, filter_channels, &td, NULL,
                      FFMIN(inlink->ch_layout.nb_channels, ff_filter_get_nb_threads(ctx)));

    s->in_samples += in->nb_samples;

    if (out != in)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}

static av_cold void uninit(AVFilterContext *ctx)
{
    AFreqShift *s = ctx->priv;

    av_frame_free(&s->i1);
    av_frame_free(&s->o1);
    av_frame_free(&s->i2);
    av_frame_free(&s->o2);
}

#define OFFSET(x) offsetof(AFreqShift, x)
#define FLAGS AV_OPT_FLAG_AUDIO_PARAM|AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_RUNTIME_PARAM

static const AVOption afreqshift_options[] = {
    { "shift", "set frequency shift", OFFSET(shift), AV_OPT_TYPE_DOUBLE, {.dbl=0}, -INT_MAX, INT_MAX, FLAGS },
    { "level", "set output level",    OFFSET(level), AV_OPT_TYPE_DOUBLE, {.dbl=1},      0.0,     1.0, FLAGS },
    { "order", "set filter order",    OFFSET(nb_coeffs),AV_OPT_TYPE_INT, {.i64=8},  1, MAX_NB_COEFFS, FLAGS },
    { NULL }
};

AVFILTER_DEFINE_CLASS(afreqshift);

static const AVFilterPad inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_AUDIO,
        .filter_frame = filter_frame,
        .config_props = config_input,
    },
};

const FFFilter ff_af_afreqshift = {
    .p.name          = "afreqshift",
    .p.description   = NULL_IF_CONFIG_SMALL("Apply frequency shifting to input audio."),
    .p.priv_class    = &afreqshift_class,
    .p.flags         = AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC |
                       AVFILTER_FLAG_SLICE_THREADS,
    .priv_size       = sizeof(AFreqShift),
    .uninit          = uninit,
    FILTER_INPUTS(inputs),
    FILTER_OUTPUTS(ff_audio_default_filterpad),
    FILTER_SAMPLEFMTS_ARRAY(sample_fmts),
    .process_command = ff_filter_process_command,
};

static const AVOption aphaseshift_options[] = {
    { "shift", "set phase shift", OFFSET(shift), AV_OPT_TYPE_DOUBLE, {.dbl=0}, -1.0, 1.0, FLAGS },
    { "level", "set output level",OFFSET(level), AV_OPT_TYPE_DOUBLE, {.dbl=1},  0.0, 1.0, FLAGS },
    { "order", "set filter order",OFFSET(nb_coeffs), AV_OPT_TYPE_INT,{.i64=8},    1, MAX_NB_COEFFS, FLAGS },
    { NULL }
};

AVFILTER_DEFINE_CLASS(aphaseshift);

const FFFilter ff_af_aphaseshift = {
    .p.name          = "aphaseshift",
    .p.description   = NULL_IF_CONFIG_SMALL("Apply phase shifting to input audio."),
    .p.priv_class    = &aphaseshift_class,
    .p.flags         = AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC |
                       AVFILTER_FLAG_SLICE_THREADS,
    .priv_size       = sizeof(AFreqShift),
    .uninit          = uninit,
    FILTER_INPUTS(inputs),
    FILTER_OUTPUTS(ff_audio_default_filterpad),
    FILTER_SAMPLEFMTS_ARRAY(sample_fmts),
    .process_command = ff_filter_process_command,
};
