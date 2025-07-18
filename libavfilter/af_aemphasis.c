/*
 * Copyright (c) 2001-2010 Krzysztof Foltman, Markus Schmidt, Thor Harald Johansen, Damien Zammit and others
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

#include "libavutil/opt.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"
#include "libavfilter/audio.h"

typedef struct BiquadCoeffs {
    double a0, a1, a2, b1, b2;
} BiquadCoeffs;

typedef struct RIAACurve {
    BiquadCoeffs r1;
    BiquadCoeffs brickw;
    int use_brickw;
} RIAACurve;

typedef struct AudioEmphasisContext {
    const AVClass *class;
    int mode, type;
    double level_in, level_out;

    RIAACurve rc;

    AVFrame *w;
} AudioEmphasisContext;

#define OFFSET(x) offsetof(AudioEmphasisContext, x)
#define FLAGS AV_OPT_FLAG_AUDIO_PARAM|AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_RUNTIME_PARAM

static const AVOption aemphasis_options[] = {
    { "level_in",      "set input gain", OFFSET(level_in),  AV_OPT_TYPE_DOUBLE, {.dbl=1}, 0, 64, FLAGS },
    { "level_out",    "set output gain", OFFSET(level_out), AV_OPT_TYPE_DOUBLE, {.dbl=1}, 0, 64, FLAGS },
    { "mode",         "set filter mode", OFFSET(mode), AV_OPT_TYPE_INT,   {.i64=0}, 0, 1, FLAGS, .unit = "mode" },
    { "reproduction",              NULL,            0, AV_OPT_TYPE_CONST, {.i64=0}, 0, 0, FLAGS, .unit = "mode" },
    { "production",                NULL,            0, AV_OPT_TYPE_CONST, {.i64=1}, 0, 0, FLAGS, .unit = "mode" },
    { "type",         "set filter type", OFFSET(type), AV_OPT_TYPE_INT,   {.i64=4}, 0, 8, FLAGS, .unit = "type" },
    { "col",                 "Columbia",            0, AV_OPT_TYPE_CONST, {.i64=0}, 0, 0, FLAGS, .unit = "type" },
    { "emi",                      "EMI",            0, AV_OPT_TYPE_CONST, {.i64=1}, 0, 0, FLAGS, .unit = "type" },
    { "bsi",              "BSI (78RPM)",            0, AV_OPT_TYPE_CONST, {.i64=2}, 0, 0, FLAGS, .unit = "type" },
    { "riaa",                    "RIAA",            0, AV_OPT_TYPE_CONST, {.i64=3}, 0, 0, FLAGS, .unit = "type" },
    { "cd",         "Compact Disc (CD)",            0, AV_OPT_TYPE_CONST, {.i64=4}, 0, 0, FLAGS, .unit = "type" },
    { "50fm",               "50µs (FM)",            0, AV_OPT_TYPE_CONST, {.i64=5}, 0, 0, FLAGS, .unit = "type" },
    { "75fm",               "75µs (FM)",            0, AV_OPT_TYPE_CONST, {.i64=6}, 0, 0, FLAGS, .unit = "type" },
    { "50kf",            "50µs (FM-KF)",            0, AV_OPT_TYPE_CONST, {.i64=7}, 0, 0, FLAGS, .unit = "type" },
    { "75kf",            "75µs (FM-KF)",            0, AV_OPT_TYPE_CONST, {.i64=8}, 0, 0, FLAGS, .unit = "type" },
    { NULL }
};

AVFILTER_DEFINE_CLASS(aemphasis);

static inline void biquad_process(BiquadCoeffs *bq, double *dst, const double *src, int nb_samples,
                                  double *w, double level_in, double level_out)
{
    const double a0 = bq->a0;
    const double a1 = bq->a1;
    const double a2 = bq->a2;
    const double b1 = bq->b1;
    const double b2 = bq->b2;
    double w1 = w[0];
    double w2 = w[1];

    for (int i = 0; i < nb_samples; i++) {
        double n = src[i] * level_in;
        double tmp = n - w1 * b1 - w2 * b2;
        double out = tmp * a0 + w1 * a1 + w2 * a2;

        w2 = w1;
        w1 = tmp;

        dst[i] = out * level_out;
    }

    w[0] = w1;
    w[1] = w2;
}

typedef struct ThreadData {
    AVFrame *in, *out;
} ThreadData;

static int filter_channels(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    AudioEmphasisContext *s = ctx->priv;
    const double level_out = s->level_out;
    const double level_in = s->level_in;
    ThreadData *td = arg;
    AVFrame *out = td->out;
    AVFrame *in = td->in;
    const int start = (in->ch_layout.nb_channels * jobnr) / nb_jobs;
    const int end = (in->ch_layout.nb_channels * (jobnr+1)) / nb_jobs;

    for (int ch = start; ch < end; ch++) {
        const double *src = (const double *)in->extended_data[ch];
        double *w = (double *)s->w->extended_data[ch];
        double *dst = (double *)out->extended_data[ch];

        if (s->rc.use_brickw) {
            biquad_process(&s->rc.brickw, dst, src, in->nb_samples, w + 2, level_in, 1.);
            biquad_process(&s->rc.r1, dst, dst, in->nb_samples, w, 1., level_out);
        } else {
            biquad_process(&s->rc.r1, dst, src, in->nb_samples, w, level_in, level_out);
        }
    }

    return 0;
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ThreadData td;
    AVFrame *out;

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

    if (in != out)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}

static inline void set_highshelf_rbj(BiquadCoeffs *bq, double freq, double q, double peak, double sr)
{
    double A = sqrt(peak);
    double w0 = freq * 2 * M_PI / sr;
    double alpha = sin(w0) / (2 * q);
    double cw0 = cos(w0);
    double tmp = 2 * sqrt(A) * alpha;
    double b0 = 0, ib0 = 0;

    bq->a0 =    A*( (A+1) + (A-1)*cw0 + tmp);
    bq->a1 = -2*A*( (A-1) + (A+1)*cw0);
    bq->a2 =    A*( (A+1) + (A-1)*cw0 - tmp);
        b0 =        (A+1) - (A-1)*cw0 + tmp;
    bq->b1 =    2*( (A-1) - (A+1)*cw0);
    bq->b2 =        (A+1) - (A-1)*cw0 - tmp;

    ib0     = 1 / b0;
    bq->b1 *= ib0;
    bq->b2 *= ib0;
    bq->a0 *= ib0;
    bq->a1 *= ib0;
    bq->a2 *= ib0;
}

static inline void set_lp_rbj(BiquadCoeffs *bq, double fc, double q, double sr, double gain)
{
    double omega = 2.0 * M_PI * fc / sr;
    double sn = sin(omega);
    double cs = cos(omega);
    double alpha = sn/(2 * q);
    double inv = 1.0/(1.0 + alpha);

    bq->a2 = bq->a0 = gain * inv * (1.0 - cs) * 0.5;
    bq->a1 = bq->a0 + bq->a0;
    bq->b1 = (-2.0 * cs * inv);
    bq->b2 = ((1.0 - alpha) * inv);
}

static double freq_gain(BiquadCoeffs *c, double freq, double sr)
{
    double zr, zi;

    freq *= 2.0 * M_PI / sr;
    zr = cos(freq);
    zi = -sin(freq);

    /* |(a0 + a1*z + a2*z^2)/(1 + b1*z + b2*z^2)| */
    return hypot(c->a0 + c->a1*zr + c->a2*(zr*zr-zi*zi), c->a1*zi + 2*c->a2*zr*zi) /
           hypot(1 + c->b1*zr + c->b2*(zr*zr-zi*zi), c->b1*zi + 2*c->b2*zr*zi);
}

static int config_input(AVFilterLink *inlink)
{
    double i, j, k, g, t, a0, a1, a2, b1, b2, tau1, tau2, tau3;
    double cutfreq, gain1kHz, gc, sr = inlink->sample_rate;
    AVFilterContext *ctx = inlink->dst;
    AudioEmphasisContext *s = ctx->priv;
    BiquadCoeffs coeffs;

    if (!s->w)
        s->w = ff_get_audio_buffer(inlink, 4);
    if (!s->w)
        return AVERROR(ENOMEM);

    switch (s->type) {
    case 0: //"Columbia"
        i = 100.;
        j = 500.;
        k = 1590.;
        break;
    case 1: //"EMI"
        i = 70.;
        j = 500.;
        k = 2500.;
        break;
    case 2: //"BSI(78rpm)"
        i = 50.;
        j = 353.;
        k = 3180.;
        break;
    case 3: //"RIAA"
    default:
        tau1 = 0.003180;
        tau2 = 0.000318;
        tau3 = 0.000075;
        i = 1. / (2. * M_PI * tau1);
        j = 1. / (2. * M_PI * tau2);
        k = 1. / (2. * M_PI * tau3);
        break;
    case 4: //"CD Mastering"
        tau1 = 0.000050;
        tau2 = 0.000015;
        tau3 = 0.0000001;// 1.6MHz out of audible range for null impact
        i = 1. / (2. * M_PI * tau1);
        j = 1. / (2. * M_PI * tau2);
        k = 1. / (2. * M_PI * tau3);
        break;
    case 5: //"50µs FM (Europe)"
        tau1 = 0.000050;
        tau2 = tau1 / 20;// not used
        tau3 = tau1 / 50;//
        i = 1. / (2. * M_PI * tau1);
        j = 1. / (2. * M_PI * tau2);
        k = 1. / (2. * M_PI * tau3);
        break;
    case 6: //"75µs FM (US)"
        tau1 = 0.000075;
        tau2 = tau1 / 20;// not used
        tau3 = tau1 / 50;//
        i = 1. / (2. * M_PI * tau1);
        j = 1. / (2. * M_PI * tau2);
        k = 1. / (2. * M_PI * tau3);
        break;
    }

    i *= 2 * M_PI;
    j *= 2 * M_PI;
    k *= 2 * M_PI;

    t = 1. / sr;

    //swap a1 b1, a2 b2
    if (s->type == 7 || s->type == 8) {
        double tau = (s->type == 7 ? 0.000050 : 0.000075);
        double f = 1.0 / (2 * M_PI * tau);
        double nyq = sr * 0.5;
        double gain = sqrt(1.0 + nyq * nyq / (f * f)); // gain at Nyquist
        double cfreq = sqrt((gain - 1.0) * f * f); // frequency
        double q = 1.0;

        if (s->type == 8)
            q = pow((sr / 3269.0) + 19.5, -0.25); // somewhat poor curve-fit
        if (s->type == 7)
            q = pow((sr / 4750.0) + 19.5, -0.25);
        if (s->mode == 0)
            set_highshelf_rbj(&s->rc.r1, cfreq, q, 1. / gain, sr);
        else
            set_highshelf_rbj(&s->rc.r1, cfreq, q, gain, sr);
        s->rc.use_brickw = 0;
    } else {
        s->rc.use_brickw = 1;
        if (s->mode == 0) { // Reproduction
            g  = 1. / (4.+2.*i*t+2.*k*t+i*k*t*t);
            a0 = (2.*t+j*t*t)*g;
            a1 = (2.*j*t*t)*g;
            a2 = (-2.*t+j*t*t)*g;
            b1 = (-8.+2.*i*k*t*t)*g;
            b2 = (4.-2.*i*t-2.*k*t+i*k*t*t)*g;
        } else {  // Production
            g  = 1. / (2.*t+j*t*t);
            a0 = (4.+2.*i*t+2.*k*t+i*k*t*t)*g;
            a1 = (-8.+2.*i*k*t*t)*g;
            a2 = (4.-2.*i*t-2.*k*t+i*k*t*t)*g;
            b1 = (2.*j*t*t)*g;
            b2 = (-2.*t+j*t*t)*g;
        }

        coeffs.a0 = a0;
        coeffs.a1 = a1;
        coeffs.a2 = a2;
        coeffs.b1 = b1;
        coeffs.b2 = b2;

        // the coeffs above give non-normalized value, so it should be normalized to produce 0dB at 1 kHz
        // find actual gain
        // Note: for FM emphasis, use 100 Hz for normalization instead
        gain1kHz = freq_gain(&coeffs, 1000.0, sr);
        // divide one filter's x[n-m] coefficients by that value
        gc = 1.0 / gain1kHz;
        s->rc.r1.a0 = coeffs.a0 * gc;
        s->rc.r1.a1 = coeffs.a1 * gc;
        s->rc.r1.a2 = coeffs.a2 * gc;
        s->rc.r1.b1 = coeffs.b1;
        s->rc.r1.b2 = coeffs.b2;
    }

    cutfreq = FFMIN(0.45 * sr, 21000.);
    set_lp_rbj(&s->rc.brickw, cutfreq, 0.707, sr, 1.);

    return 0;
}

static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    int ret;

    ret = ff_filter_process_command(ctx, cmd, args, res, res_len, flags);
    if (ret < 0)
        return ret;

    return config_input(ctx->inputs[0]);
}

static av_cold void uninit(AVFilterContext *ctx)
{
    AudioEmphasisContext *s = ctx->priv;

    av_frame_free(&s->w);
}

static const AVFilterPad avfilter_af_aemphasis_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_AUDIO,
        .config_props = config_input,
        .filter_frame = filter_frame,
    },
};

const FFFilter ff_af_aemphasis = {
    .p.name        = "aemphasis",
    .p.description = NULL_IF_CONFIG_SMALL("Audio emphasis."),
    .p.priv_class  = &aemphasis_class,
    .p.flags       = AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC |
                     AVFILTER_FLAG_SLICE_THREADS,
    .priv_size     = sizeof(AudioEmphasisContext),
    .uninit        = uninit,
    FILTER_INPUTS(avfilter_af_aemphasis_inputs),
    FILTER_OUTPUTS(ff_audio_default_filterpad),
    FILTER_SINGLE_SAMPLEFMT(AV_SAMPLE_FMT_DBLP),
    .process_command = process_command,
};
