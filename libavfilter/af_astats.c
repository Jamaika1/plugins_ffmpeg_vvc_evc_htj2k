/*
 * Copyright (c) 2009 Rob Sykes <robs@users.sourceforge.net>
 * Copyright (c) 2013 Paul B Mahol
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

#include <float.h>
#include <math.h>

#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavfilter/audio.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/filters.h"

#define HISTOGRAM_SIZE                  8192
#define HISTOGRAM_MAX                   (HISTOGRAM_SIZE-1)

#define MEASURE_ALL                     UINT_MAX
#define MEASURE_NONE                           0

#define MEASURE_DC_OFFSET               (1 <<  0)
#define MEASURE_MIN_LEVEL               (1 <<  1)
#define MEASURE_MAX_LEVEL               (1 <<  2)
#define MEASURE_MIN_DIFFERENCE          (1 <<  3)
#define MEASURE_MAX_DIFFERENCE          (1 <<  4)
#define MEASURE_MEAN_DIFFERENCE         (1 <<  5)
#define MEASURE_RMS_DIFFERENCE          (1 <<  6)
#define MEASURE_PEAK_LEVEL              (1 <<  7)
#define MEASURE_RMS_LEVEL               (1 <<  8)
#define MEASURE_RMS_PEAK                (1 <<  9)
#define MEASURE_RMS_TROUGH              (1 << 10)
#define MEASURE_CREST_FACTOR            (1 << 11)
#define MEASURE_FLAT_FACTOR             (1 << 12)
#define MEASURE_PEAK_COUNT              (1 << 13)
#define MEASURE_BIT_DEPTH               (1 << 14)
#define MEASURE_DYNAMIC_RANGE           (1 << 15)
#define MEASURE_ZERO_CROSSINGS          (1 << 16)
#define MEASURE_ZERO_CROSSINGS_RATE     (1 << 17)
#define MEASURE_NUMBER_OF_SAMPLES       (1 << 18)
#define MEASURE_NUMBER_OF_NANS          (1 << 19)
#define MEASURE_NUMBER_OF_INFS          (1 << 20)
#define MEASURE_NUMBER_OF_DENORMALS     (1 << 21)
#define MEASURE_NOISE_FLOOR             (1 << 22)
#define MEASURE_NOISE_FLOOR_COUNT       (1 << 23)
#define MEASURE_ENTROPY                 (1 << 24)
#define MEASURE_ABS_PEAK_COUNT          (1 << 25)

#define MEASURE_MINMAXPEAK              (MEASURE_MIN_LEVEL | MEASURE_MAX_LEVEL | MEASURE_PEAK_LEVEL)

typedef struct ChannelStats {
    double last;
    double last_non_zero;
    double min_non_zero;
    double sigma_x, sigma_x2;
    double avg_sigma_x2, min_sigma_x2, max_sigma_x2;
    double min, max;
    double nmin, nmax;
    double min_run, max_run;
    double min_runs, max_runs;
    double min_diff, max_diff;
    double diff1_sum;
    double diff1_sum_x2;
    double abs_peak;
    uint64_t mask[4];
    uint64_t min_count, max_count;
    uint64_t abs_peak_count;
    uint64_t noise_floor_count;
    uint64_t zero_runs;
    uint64_t nb_samples;
    uint64_t nb_nans;
    uint64_t nb_infs;
    uint64_t nb_denormals;
    double *win_samples;
    double *sorted_samples;
    uint64_t ehistogram[HISTOGRAM_SIZE];
    int64_t lasti;
    int sorted_front;
    int sorted_back;
    int win_pos;
    int max_index;
    double noise_floor;
    double entropy;
} ChannelStats;

typedef struct AudioStatsContext {
    const AVClass *class;
    ChannelStats *chstats;
    int nb_channels;
    uint64_t tc_samples;
    double time_constant;
    double mult;
    int metadata;
    int used;
    int reset_count;
    int nb_frames;
    int maxbitdepth;
    int measure_perchannel;
    int measure_overall;
    int is_float;
    int is_double;
} AudioStatsContext;

#define OFFSET(x) offsetof(AudioStatsContext, x)
#define FLAGS AV_OPT_FLAG_AUDIO_PARAM|AV_OPT_FLAG_FILTERING_PARAM

static const AVOption astats_options[] = {
    { "length", "set the window length", OFFSET(time_constant), AV_OPT_TYPE_DOUBLE, {.dbl=.05}, 0, 10, FLAGS },
    { "metadata", "inject metadata in the filtergraph", OFFSET(metadata), AV_OPT_TYPE_BOOL, {.i64=0}, 0, 1, FLAGS },
    { "reset", "Set the number of frames over which cumulative stats are calculated before being reset", OFFSET(reset_count), AV_OPT_TYPE_INT, {.i64=0}, 0, INT_MAX, FLAGS },
    { "measure_perchannel", "Select the parameters which are measured per channel", OFFSET(measure_perchannel), AV_OPT_TYPE_FLAGS, {.i64=MEASURE_ALL}, 0, UINT_MAX, FLAGS, .unit = "measure" },
      { "none"                      , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_NONE                }, 0, 0, FLAGS, .unit = "measure" },
      { "all"                       , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_ALL                 }, 0, 0, FLAGS, .unit = "measure" },
      { "Bit_depth"                 , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_BIT_DEPTH           }, 0, 0, FLAGS, .unit = "measure" },
      { "Crest_factor"              , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_CREST_FACTOR        }, 0, 0, FLAGS, .unit = "measure" },
      { "DC_offset"                 , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_DC_OFFSET           }, 0, 0, FLAGS, .unit = "measure" },
      { "Dynamic_range"             , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_DYNAMIC_RANGE       }, 0, 0, FLAGS, .unit = "measure" },
      { "Entropy"                   , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_ENTROPY             }, 0, 0, FLAGS, .unit = "measure" },
      { "Flat_factor"               , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_FLAT_FACTOR         }, 0, 0, FLAGS, .unit = "measure" },
      { "Max_difference"            , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_MAX_DIFFERENCE      }, 0, 0, FLAGS, .unit = "measure" },
      { "Max_level"                 , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_MAX_LEVEL           }, 0, 0, FLAGS, .unit = "measure" },
      { "Mean_difference"           , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_MEAN_DIFFERENCE     }, 0, 0, FLAGS, .unit = "measure" },
      { "Min_difference"            , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_MIN_DIFFERENCE      }, 0, 0, FLAGS, .unit = "measure" },
      { "Min_level"                 , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_MIN_LEVEL           }, 0, 0, FLAGS, .unit = "measure" },
      { "Noise_floor"               , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_NOISE_FLOOR         }, 0, 0, FLAGS, .unit = "measure" },
      { "Noise_floor_count"         , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_NOISE_FLOOR_COUNT   }, 0, 0, FLAGS, .unit = "measure" },
      { "Number_of_Infs"            , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_NUMBER_OF_INFS      }, 0, 0, FLAGS, .unit = "measure" },
      { "Number_of_NaNs"            , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_NUMBER_OF_NANS      }, 0, 0, FLAGS, .unit = "measure" },
      { "Number_of_denormals"       , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_NUMBER_OF_DENORMALS }, 0, 0, FLAGS, .unit = "measure" },
      { "Number_of_samples"         , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_NUMBER_OF_SAMPLES   }, 0, 0, FLAGS, .unit = "measure" },
      { "Peak_count"                , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_PEAK_COUNT          }, 0, 0, FLAGS, .unit = "measure" },
      { "Peak_level"                , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_PEAK_LEVEL          }, 0, 0, FLAGS, .unit = "measure" },
      { "RMS_difference"            , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_RMS_DIFFERENCE      }, 0, 0, FLAGS, .unit = "measure" },
      { "RMS_level"                 , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_RMS_LEVEL           }, 0, 0, FLAGS, .unit = "measure" },
      { "RMS_peak"                  , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_RMS_PEAK            }, 0, 0, FLAGS, .unit = "measure" },
      { "RMS_trough"                , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_RMS_TROUGH          }, 0, 0, FLAGS, .unit = "measure" },
      { "Zero_crossings"            , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_ZERO_CROSSINGS      }, 0, 0, FLAGS, .unit = "measure" },
      { "Zero_crossings_rate"       , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_ZERO_CROSSINGS_RATE }, 0, 0, FLAGS, .unit = "measure" },
      { "Abs_Peak_count"            , "", 0, AV_OPT_TYPE_CONST, {.i64=MEASURE_ABS_PEAK_COUNT      }, 0, 0, FLAGS, .unit = "measure" },
    { "measure_overall", "Select the parameters which are measured overall", OFFSET(measure_overall), AV_OPT_TYPE_FLAGS, {.i64=MEASURE_ALL}, 0, UINT_MAX, FLAGS, .unit = "measure" },
    { NULL }
};

AVFILTER_DEFINE_CLASS(astats);

static void reset_stats(AudioStatsContext *s)
{
    int c;

    for (c = 0; c < s->nb_channels; c++) {
        ChannelStats *p = &s->chstats[c];

        p->min = p->nmin = p->min_sigma_x2 = DBL_MAX;
        p->max = p->nmax = p->max_sigma_x2 =-DBL_MAX;
        p->abs_peak = 0;
        p->min_non_zero = DBL_MAX;
        p->min_diff = DBL_MAX;
        p->max_diff = 0;
        p->sigma_x = 0;
        p->sigma_x2 = 0;
        p->avg_sigma_x2 = 0;
        p->min_run = 0;
        p->max_run = 0;
        p->min_runs = 0;
        p->max_runs = 0;
        p->diff1_sum = 0;
        p->diff1_sum_x2 = 0;
        p->mask[0] = 0;
        p->mask[1] = 0;
        p->mask[2] =~0;
        p->mask[3] = 0;
        p->min_count = 0;
        p->max_count = 0;
        p->abs_peak_count = 0;
        p->zero_runs = 0;
        p->nb_samples = 0;
        p->nb_nans = 0;
        p->nb_infs = 0;
        p->nb_denormals = 0;
        p->last = NAN;
        p->noise_floor = NAN;
        p->noise_floor_count = 0;
        p->entropy = 0;
        p->win_pos = 0;
        p->sorted_front = 0;
        p->sorted_back = 0;
        memset(p->win_samples, 0, s->tc_samples * sizeof(*p->win_samples));
        memset(p->ehistogram, 0, sizeof(p->ehistogram));
        for (int n = 0; n < s->tc_samples; n++)
            p->sorted_samples[n] = -1.0;
    }
}

static int config_output(AVFilterLink *outlink)
{
    AudioStatsContext *s = outlink->src->priv;

    s->chstats = av_calloc(sizeof(*s->chstats), outlink->ch_layout.nb_channels);
    if (!s->chstats)
        return AVERROR(ENOMEM);

    s->tc_samples = FFMAX(s->time_constant * outlink->sample_rate + .5, 1);
    s->nb_channels = outlink->ch_layout.nb_channels;

    for (int i = 0; i < s->nb_channels; i++) {
        ChannelStats *p = &s->chstats[i];

        p->win_samples = av_calloc(s->tc_samples, sizeof(*p->win_samples));
        if (!p->win_samples)
            return AVERROR(ENOMEM);

        p->sorted_samples = av_calloc(s->tc_samples, sizeof(*p->sorted_samples));
        if (!p->sorted_samples)
            return AVERROR(ENOMEM);
    }

    s->mult = exp((-1 / s->time_constant / outlink->sample_rate));
    s->nb_frames = 0;
    s->maxbitdepth = av_get_bytes_per_sample(outlink->format) * 8;
    s->is_double = outlink->format == AV_SAMPLE_FMT_DBL  ||
                   outlink->format == AV_SAMPLE_FMT_DBLP;

    s->is_float = outlink->format == AV_SAMPLE_FMT_FLT  ||
                  outlink->format == AV_SAMPLE_FMT_FLTP;

    reset_stats(s);

    return 0;
}

static void bit_depth(AudioStatsContext *s, const uint64_t *const mask, uint8_t *depth)
{
    unsigned result = s->maxbitdepth;
    uint64_t amask = mask[1] & (~mask[2]);

    depth[0] = 0;
    for (int i = 0; i < result; i++)
        depth[0] += !!(mask[0] & (1ULL << i));

    depth[1] = 0;
    for (int i = 0; i < result; i++)
        depth[1] += !!(mask[1] & (1ULL << i));

    depth[2] = result;
    for (int i = 0; i < result && !(amask & 1); i++) {
        depth[2]--;
        amask >>= 1;
    }

    depth[3] = 0;
    for (int i = 0; i < result; i++)
        depth[3] += !!(mask[3] & (1ULL << i));
}

static double calc_entropy(AudioStatsContext *s, ChannelStats *p)
{
    double entropy = 0.;

    for (int i = 0; i < HISTOGRAM_SIZE; i++) {
        double entry = p->ehistogram[i] / ((double)p->nb_samples);

        if (entry > 1e-8)
            entropy += entry * log2(entry);
    }

    return -entropy / log2(HISTOGRAM_SIZE);
}

static double calc_noise_floor(double *ss, double x, double px,
                               int n, int *ffront, int *bback)
{
    double r, ax = fabs(x);
    int front = *ffront;
    int back = *bback;
    int empty = front == back && ss[front] == -1.0;

    if (!empty && fabs(px) == ss[front]) {
        ss[front] = -1.0;
        if (back != front) {
            front--;
            if (front < 0)
                front = n - 1;
        }
        empty = front == back;
    }

    if (!empty && ax >= ss[front]) {
        while (1) {
            ss[front] = -1.0;
            if (back == front) {
                empty = 1;
                break;
            }
            front--;
            if (front < 0)
                front = n - 1;
        }
    }

    while (!empty && ax >= ss[back]) {
        ss[back] = -1.0;
        if (back == front) {
            empty = 1;
            break;
        }
        back++;
        if (back >= n)
            back = 0;
    }

    if (!empty) {
        back--;
        if (back < 0)
            back = n - 1;
    }

    ss[back] = ax;
    r = ss[front];

    *ffront = front;
    *bback = back;

    return r;
}

static inline void update_minmax(AudioStatsContext *s, ChannelStats *p, double d)
{
    if (d < p->min)
        p->min = d;
    if (d > p->max)
        p->max = d;
}

static inline void update_stat(AudioStatsContext *s, ChannelStats *p, double d, double nd, int64_t i)
{
    double abs_d = FFABS(d);
    double drop, noise_floor;
    int index;

    if (p->abs_peak < abs_d) {
        p->abs_peak = abs_d;
        p->abs_peak_count = 1;
    } else if (p->abs_peak == abs_d) {
        p->abs_peak_count++;
    }
    if (d < p->min) {
        p->min = d;
        p->nmin = nd;
        p->min_run = 1;
        p->min_runs = 0;
        p->min_count = 1;
    } else if (d == p->min) {
        p->min_count++;
        p->min_run = d == p->last ? p->min_run + 1 : 1;
    } else if (p->last == p->min) {
        p->min_runs += p->min_run * p->min_run;
    }

    if (d != 0 && FFABS(d) < p->min_non_zero)
        p->min_non_zero = FFABS(d);

    if (d > p->max) {
        p->max = d;
        p->nmax = nd;
        p->max_run = 1;
        p->max_runs = 0;
        p->max_count = 1;
    } else if (d == p->max) {
        p->max_count++;
        p->max_run = d == p->last ? p->max_run + 1 : 1;
    } else if (p->last == p->max) {
        p->max_runs += p->max_run * p->max_run;
    }

    if (d != 0) {
        p->zero_runs += FFSIGN(d) != FFSIGN(p->last_non_zero);
        p->last_non_zero = d;
    }

    p->sigma_x += nd;
    p->sigma_x2 += nd * nd;
    p->avg_sigma_x2 = p->avg_sigma_x2 * s->mult + (1.0 - s->mult) * nd * nd;
    if (!isnan(p->last)) {
        p->min_diff = FFMIN(p->min_diff, fabs(d - p->last));
        p->max_diff = FFMAX(p->max_diff, fabs(d - p->last));
        p->diff1_sum += fabs(d - p->last);
        p->diff1_sum_x2 += (d - p->last) * (d - p->last);
    }
    p->mask[0] |= (i < 0) ? -i : i;
    p->mask[1] |= i;
    p->mask[2] &= i;
    if (!isnan(p->last))
        p->mask[3] |= i ^ p->lasti;
    p->lasti = i;
    p->last = d;

    drop = p->win_samples[p->win_pos];
    p->win_samples[p->win_pos] = nd;
    index = av_clip(lrint(av_clipd(FFABS(nd), 0.0, 1.0) * HISTOGRAM_MAX), 0, HISTOGRAM_MAX);
    p->max_index = FFMAX(p->max_index, index);
    p->ehistogram[index]++;
    p->win_pos++;

    if (p->win_pos >= s->tc_samples)
        p->win_pos = 0;

    if (p->nb_samples >= s->tc_samples) {
        p->max_sigma_x2 = FFMAX(p->max_sigma_x2, p->avg_sigma_x2);
        p->min_sigma_x2 = FFMIN(p->min_sigma_x2, p->avg_sigma_x2);
    }
    p->nb_samples++;

    noise_floor = calc_noise_floor(p->sorted_samples, nd, drop,
                                   s->tc_samples, &p->sorted_front, &p->sorted_back);
    if (p->nb_samples >= s->tc_samples) {
        if (isnan(p->noise_floor)) {
            p->noise_floor = noise_floor;
            p->noise_floor_count = 1;
        } else {
            if (noise_floor < p->noise_floor) {
                p->noise_floor = noise_floor;
                p->noise_floor_count = 1;
            } else if (noise_floor == p->noise_floor) {
                p->noise_floor_count++;
            }
        }
    }
}

static inline void update_float_stat(AudioStatsContext *s, ChannelStats *p, float d)
{
    int type = fpclassify(d);

    p->nb_nans      += type == FP_NAN;
    p->nb_infs      += type == FP_INFINITE;
    p->nb_denormals += type == FP_SUBNORMAL;
}

static inline void update_double_stat(AudioStatsContext *s, ChannelStats *p, double d)
{
    int type = fpclassify(d);

    p->nb_nans      += type == FP_NAN;
    p->nb_infs      += type == FP_INFINITE;
    p->nb_denormals += type == FP_SUBNORMAL;
}

static void set_meta(AVDictionary **metadata, int chan, const char *key,
                     const char *fmt, double val)
{
    uint8_t value[128];
    uint8_t key2[128];

    snprintf(value, sizeof(value), fmt, val);
    if (chan)
        snprintf(key2, sizeof(key2), "lavfi.astats.%d.%s", chan, key);
    else
        snprintf(key2, sizeof(key2), "lavfi.astats.%s", key);
    av_dict_set(metadata, key2, value, 0);
}

#define LINEAR_TO_DB(x) (log10(x) * 20)

static void set_metadata(AudioStatsContext *s, AVDictionary **metadata)
{
    uint64_t mask[4], min_count = 0, max_count = 0, nb_samples = 0, noise_floor_count = 0;
    uint64_t nb_nans = 0, nb_infs = 0, nb_denormals = 0;
    uint64_t abs_peak_count = 0;
    double min_runs = 0, max_runs = 0,
           min = DBL_MAX, max =-DBL_MAX, min_diff = DBL_MAX, max_diff = 0,
           nmin = DBL_MAX, nmax =-DBL_MAX,
           max_sigma_x = 0,
           diff1_sum = 0,
           diff1_sum_x2 = 0,
           sigma_x2 = 0,
           noise_floor = 0,
           entropy = 0,
           min_sigma_x2 = DBL_MAX,
           max_sigma_x2 =-DBL_MAX;
    uint8_t depth[4];
    int c;

    mask[0] = 0;
    mask[1] = 0;
    mask[2] =~0;
    mask[3] = 0;

    for (c = 0; c < s->nb_channels; c++) {
        ChannelStats *p = &s->chstats[c];

        if (p->nb_samples < s->tc_samples)
            p->min_sigma_x2 = p->max_sigma_x2 = p->sigma_x2 / p->nb_samples;

        min = FFMIN(min, p->min);
        max = FFMAX(max, p->max);
        nmin = FFMIN(nmin, p->nmin);
        nmax = FFMAX(nmax, p->nmax);
        min_diff = FFMIN(min_diff, p->min_diff);
        max_diff = FFMAX(max_diff, p->max_diff);
        diff1_sum += p->diff1_sum;
        diff1_sum_x2 += p->diff1_sum_x2;
        min_sigma_x2 = FFMIN(min_sigma_x2, p->min_sigma_x2);
        max_sigma_x2 = FFMAX(max_sigma_x2, p->max_sigma_x2);
        sigma_x2 += p->sigma_x2;
        noise_floor = FFMAX(noise_floor, p->noise_floor);
        noise_floor_count += p->noise_floor_count;
        p->entropy = calc_entropy(s, p);
        entropy += p->entropy;
        min_count += p->min_count;
        max_count += p->max_count;
        abs_peak_count += p->abs_peak_count;
        min_runs += p->min_runs;
        max_runs += p->max_runs;
        mask[0] |= p->mask[0];
        mask[1] |= p->mask[1];
        mask[2] &= p->mask[2];
        mask[3] |= p->mask[3];
        nb_samples += p->nb_samples;
        nb_nans += p->nb_nans;
        nb_infs += p->nb_infs;
        nb_denormals += p->nb_denormals;
        if (fabs(p->sigma_x) > fabs(max_sigma_x))
            max_sigma_x = p->sigma_x;

        if (s->measure_perchannel & MEASURE_DC_OFFSET)
            set_meta(metadata, c + 1, "DC_offset", "%f", p->sigma_x / p->nb_samples);
        if (s->measure_perchannel & MEASURE_MIN_LEVEL)
            set_meta(metadata, c + 1, "Min_level", "%f", p->min);
        if (s->measure_perchannel & MEASURE_MAX_LEVEL)
            set_meta(metadata, c + 1, "Max_level", "%f", p->max);
        if (s->measure_perchannel & MEASURE_MIN_DIFFERENCE)
            set_meta(metadata, c + 1, "Min_difference", "%f", p->min_diff);
        if (s->measure_perchannel & MEASURE_MAX_DIFFERENCE)
            set_meta(metadata, c + 1, "Max_difference", "%f", p->max_diff);
        if (s->measure_perchannel & MEASURE_MEAN_DIFFERENCE)
            set_meta(metadata, c + 1, "Mean_difference", "%f", p->diff1_sum / (p->nb_samples - 1));
        if (s->measure_perchannel & MEASURE_RMS_DIFFERENCE)
            set_meta(metadata, c + 1, "RMS_difference", "%f", sqrt(p->diff1_sum_x2 / (p->nb_samples - 1)));
        if (s->measure_perchannel & MEASURE_PEAK_LEVEL)
            set_meta(metadata, c + 1, "Peak_level", "%f", LINEAR_TO_DB(FFMAX(-p->nmin, p->nmax)));
        if (s->measure_perchannel & MEASURE_RMS_LEVEL)
            set_meta(metadata, c + 1, "RMS_level", "%f", LINEAR_TO_DB(sqrt(p->sigma_x2 / p->nb_samples)));
        if (s->measure_perchannel & MEASURE_RMS_PEAK)
            set_meta(metadata, c + 1, "RMS_peak", "%f", LINEAR_TO_DB(sqrt(p->max_sigma_x2)));
        if (s->measure_perchannel & MEASURE_RMS_TROUGH)
            set_meta(metadata, c + 1, "RMS_trough", "%f", LINEAR_TO_DB(sqrt(p->min_sigma_x2)));
        if (s->measure_perchannel & MEASURE_CREST_FACTOR)
            set_meta(metadata, c + 1, "Crest_factor", "%f", p->sigma_x2 ? FFMAX(-p->min, p->max) / sqrt(p->sigma_x2 / p->nb_samples) : 1);
        if (s->measure_perchannel & MEASURE_FLAT_FACTOR)
            set_meta(metadata, c + 1, "Flat_factor", "%f", LINEAR_TO_DB((p->min_runs + p->max_runs) / (p->min_count + p->max_count)));
        if (s->measure_perchannel & MEASURE_PEAK_COUNT)
            set_meta(metadata, c + 1, "Peak_count", "%f", (float)(p->min_count + p->max_count));
        if (s->measure_perchannel & MEASURE_ABS_PEAK_COUNT)
            set_meta(metadata, c + 1, "Peak_count", "%f", p->abs_peak_count);
        if (s->measure_perchannel & MEASURE_NOISE_FLOOR)
            set_meta(metadata, c + 1, "Noise_floor", "%f", LINEAR_TO_DB(p->noise_floor));
        if (s->measure_perchannel & MEASURE_NOISE_FLOOR_COUNT)
            set_meta(metadata, c + 1, "Noise_floor_count", "%f", p->noise_floor_count);
        if (s->measure_perchannel & MEASURE_ENTROPY)
            set_meta(metadata, c + 1, "Entropy", "%f", p->entropy);
        if (s->measure_perchannel & MEASURE_BIT_DEPTH) {
            bit_depth(s, p->mask, depth);
            set_meta(metadata, c + 1, "Bit_depth", "%f", depth[0]);
            set_meta(metadata, c + 1, "Bit_depth2", "%f", depth[1]);
            set_meta(metadata, c + 1, "Bit_depth3", "%f", depth[2]);
            set_meta(metadata, c + 1, "Bit_depth4", "%f", depth[3]);
        }
        if (s->measure_perchannel & MEASURE_DYNAMIC_RANGE)
            set_meta(metadata, c + 1, "Dynamic_range", "%f", LINEAR_TO_DB(2 * FFMAX(FFABS(p->min), FFABS(p->max))/ p->min_non_zero));
        if (s->measure_perchannel & MEASURE_ZERO_CROSSINGS)
            set_meta(metadata, c + 1, "Zero_crossings", "%f", p->zero_runs);
        if (s->measure_perchannel & MEASURE_ZERO_CROSSINGS_RATE)
            set_meta(metadata, c + 1, "Zero_crossings_rate", "%f", p->zero_runs/(double)p->nb_samples);
        if ((s->is_float || s->is_double) && s->measure_perchannel & MEASURE_NUMBER_OF_NANS)
            set_meta(metadata, c + 1, "Number of NaNs", "%f", p->nb_nans);
        if ((s->is_float || s->is_double) && s->measure_perchannel & MEASURE_NUMBER_OF_INFS)
            set_meta(metadata, c + 1, "Number of Infs", "%f", p->nb_infs);
        if ((s->is_float || s->is_double) && s->measure_perchannel & MEASURE_NUMBER_OF_DENORMALS)
            set_meta(metadata, c + 1, "Number of denormals", "%f", p->nb_denormals);
    }

    if (s->measure_overall & MEASURE_DC_OFFSET)
        set_meta(metadata, 0, "Overall.DC_offset", "%f", max_sigma_x / (nb_samples / s->nb_channels));
    if (s->measure_overall & MEASURE_MIN_LEVEL)
        set_meta(metadata, 0, "Overall.Min_level", "%f", min);
    if (s->measure_overall & MEASURE_MAX_LEVEL)
        set_meta(metadata, 0, "Overall.Max_level", "%f", max);
    if (s->measure_overall & MEASURE_MIN_DIFFERENCE)
        set_meta(metadata, 0, "Overall.Min_difference", "%f", min_diff);
    if (s->measure_overall & MEASURE_MAX_DIFFERENCE)
        set_meta(metadata, 0, "Overall.Max_difference", "%f", max_diff);
    if (s->measure_overall & MEASURE_MEAN_DIFFERENCE)
        set_meta(metadata, 0, "Overall.Mean_difference", "%f", diff1_sum / (nb_samples - s->nb_channels));
    if (s->measure_overall & MEASURE_RMS_DIFFERENCE)
        set_meta(metadata, 0, "Overall.RMS_difference", "%f", sqrt(diff1_sum_x2 / (nb_samples - s->nb_channels)));
    if (s->measure_overall & MEASURE_PEAK_LEVEL)
        set_meta(metadata, 0, "Overall.Peak_level", "%f", LINEAR_TO_DB(FFMAX(-nmin, nmax)));
    if (s->measure_overall & MEASURE_RMS_LEVEL)
        set_meta(metadata, 0, "Overall.RMS_level", "%f", LINEAR_TO_DB(sqrt(sigma_x2 / nb_samples)));
    if (s->measure_overall & MEASURE_RMS_PEAK)
        set_meta(metadata, 0, "Overall.RMS_peak", "%f", LINEAR_TO_DB(sqrt(max_sigma_x2)));
    if (s->measure_overall & MEASURE_RMS_TROUGH)
        set_meta(metadata, 0, "Overall.RMS_trough", "%f", LINEAR_TO_DB(sqrt(min_sigma_x2)));
    if (s->measure_overall & MEASURE_FLAT_FACTOR)
        set_meta(metadata, 0, "Overall.Flat_factor", "%f", LINEAR_TO_DB((min_runs + max_runs) / (min_count + max_count)));
    if (s->measure_overall & MEASURE_PEAK_COUNT)
        set_meta(metadata, 0, "Overall.Peak_count", "%f", (float)(min_count + max_count) / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_ABS_PEAK_COUNT)
        set_meta(metadata, 0, "Overall.Abs_Peak_count", "%f", (float)(abs_peak_count) / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_NOISE_FLOOR)
        set_meta(metadata, 0, "Overall.Noise_floor", "%f", LINEAR_TO_DB(noise_floor));
    if (s->measure_overall & MEASURE_NOISE_FLOOR_COUNT)
        set_meta(metadata, 0, "Overall.Noise_floor_count", "%f", noise_floor_count / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_ENTROPY)
        set_meta(metadata, 0, "Overall.Entropy", "%f", entropy / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_BIT_DEPTH) {
        bit_depth(s, mask, depth);
        set_meta(metadata, 0, "Overall.Bit_depth", "%f", depth[0]);
        set_meta(metadata, 0, "Overall.Bit_depth2", "%f", depth[1]);
        set_meta(metadata, 0, "Overall.Bit_depth3", "%f", depth[2]);
        set_meta(metadata, 0, "Overall.Bit_depth4", "%f", depth[3]);
    }
    if (s->measure_overall & MEASURE_NUMBER_OF_SAMPLES)
        set_meta(metadata, 0, "Overall.Number_of_samples", "%f", nb_samples / s->nb_channels);
    if ((s->is_float || s->is_double) && s->measure_overall & MEASURE_NUMBER_OF_NANS)
        set_meta(metadata, 0, "Number of NaNs", "%f", nb_nans / (float)s->nb_channels);
    if ((s->is_float || s->is_double) && s->measure_overall & MEASURE_NUMBER_OF_INFS)
        set_meta(metadata, 0, "Number of Infs", "%f", nb_infs / (float)s->nb_channels);
    if ((s->is_float || s->is_double) && s->measure_overall & MEASURE_NUMBER_OF_DENORMALS)
        set_meta(metadata, 0, "Number of denormals", "%f", nb_denormals / (float)s->nb_channels);
}

#define UPDATE_STATS_P(type, update_func, update_float, channel_func)           \
    for (int c = start; c < end; c++) {                                         \
        ChannelStats *p = &s->chstats[c];                                       \
        const type *src = (const type *)data[c];                                \
        const type * const srcend = src + samples;                              \
        for (; src < srcend; src++) {                                           \
            update_func;                                                        \
            update_float;                                                       \
        }                                                                       \
        channel_func;                                                           \
    }

#define UPDATE_STATS_I(type, update_func, update_float, channel_func)           \
    for (int c = start; c < end; c++) {                                         \
        ChannelStats *p = &s->chstats[c];                                       \
        const type *src = (const type *)data[0];                                \
        const type * const srcend = src + samples * channels;                   \
        for (src += c; src < srcend; src += channels) {                         \
            update_func;                                                        \
            update_float;                                                       \
        }                                                                       \
        channel_func;                                                           \
    }

#define UPDATE_STATS(planar, type, sample, normalizer_suffix, int_sample) \
    if ((s->measure_overall | s->measure_perchannel) & ~MEASURE_MINMAXPEAK) {                          \
        UPDATE_STATS_##planar(type, update_stat(s, p, sample, sample normalizer_suffix, int_sample), s->is_float ? update_float_stat(s, p, sample) : s->is_double ? update_double_stat(s, p, sample) : (void)NULL, ); \
    } else {                                                                                           \
        UPDATE_STATS_##planar(type, update_minmax(s, p, sample), , p->nmin = p->min normalizer_suffix; p->nmax = p->max normalizer_suffix;); \
    }

static int filter_channel(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    AudioStatsContext *s = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    AVFrame *buf = arg;
    const uint8_t * const * const data = (const uint8_t * const *)buf->extended_data;
    const int channels = s->nb_channels;
    const int samples = buf->nb_samples;
    const int start = (buf->ch_layout.nb_channels * jobnr) / nb_jobs;
    const int end = (buf->ch_layout.nb_channels * (jobnr+1)) / nb_jobs;

    switch (inlink->format) {
    case AV_SAMPLE_FMT_DBLP:
        UPDATE_STATS(P, double, *src, , llrint(*src * (UINT64_C(1) << 63)));
        break;
    case AV_SAMPLE_FMT_DBL:
        UPDATE_STATS(I, double, *src, , llrint(*src * (UINT64_C(1) << 63)));
        break;
    case AV_SAMPLE_FMT_FLTP:
        UPDATE_STATS(P, float, *src, , llrint(*src * (UINT64_C(1) << 31)));
        break;
    case AV_SAMPLE_FMT_FLT:
        UPDATE_STATS(I, float, *src, , llrint(*src * (UINT64_C(1) << 31)));
        break;
    case AV_SAMPLE_FMT_S64P:
        UPDATE_STATS(P, int64_t, *src, / (double)INT64_MAX, *src);
        break;
    case AV_SAMPLE_FMT_S64:
        UPDATE_STATS(I, int64_t, *src, / (double)INT64_MAX, *src);
        break;
    case AV_SAMPLE_FMT_S32P:
        UPDATE_STATS(P, int32_t, *src, / (double)INT32_MAX, *src);
        break;
    case AV_SAMPLE_FMT_S32:
        UPDATE_STATS(I, int32_t, *src, / (double)INT32_MAX, *src);
        break;
    case AV_SAMPLE_FMT_S16P:
        UPDATE_STATS(P, int16_t, *src, / (double)INT16_MAX, *src);
        break;
    case AV_SAMPLE_FMT_S16:
        UPDATE_STATS(I, int16_t, *src, / (double)INT16_MAX, *src);
        break;
    }

    return 0;
}

static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext *ctx = inlink->dst;
    AudioStatsContext *s = ctx->priv;
    AVDictionary **metadata = &buf->metadata;

    if (s->reset_count > 0) {
        if (s->nb_frames >= s->reset_count) {
            reset_stats(s);
            s->nb_frames = 0;
        }
        s->nb_frames++;
    }

    if (s->used == 0)
        s->used = buf->nb_samples > 0;
    ff_filter_execute(ctx, filter_channel, buf, NULL,
                      FFMIN(inlink->ch_layout.nb_channels, ff_filter_get_nb_threads(ctx)));

    if (s->metadata)
        set_metadata(s, metadata);

    return ff_filter_frame(inlink->dst->outputs[0], buf);
}

static void print_stats(AVFilterContext *ctx)
{
    AudioStatsContext *s = ctx->priv;
    uint64_t mask[4], min_count = 0, max_count = 0, nb_samples = 0, noise_floor_count = 0;
    uint64_t nb_nans = 0, nb_infs = 0, nb_denormals = 0, abs_peak_count = 0;
    double min_runs = 0, max_runs = 0,
           min = DBL_MAX, max =-DBL_MAX, min_diff = DBL_MAX, max_diff = 0,
           nmin = DBL_MAX, nmax =-DBL_MAX,
           max_sigma_x = 0,
           diff1_sum_x2 = 0,
           diff1_sum = 0,
           sigma_x2 = 0,
           noise_floor = 0,
           entropy = 0,
           min_sigma_x2 = DBL_MAX,
           max_sigma_x2 =-DBL_MAX;
    uint8_t depth[4];
    int c;

    mask[0] = 0;
    mask[1] = 0;
    mask[2] =~0;
    mask[3] = 0;

    for (c = 0; c < s->nb_channels; c++) {
        ChannelStats *p = &s->chstats[c];

        if (p->nb_samples == 0 && !s->used)
            continue;

        if (p->nb_samples < s->tc_samples)
            p->min_sigma_x2 = p->max_sigma_x2 = p->sigma_x2 / p->nb_samples;

        min = FFMIN(min, p->min);
        max = FFMAX(max, p->max);
        nmin = FFMIN(nmin, p->nmin);
        nmax = FFMAX(nmax, p->nmax);
        min_diff = FFMIN(min_diff, p->min_diff);
        max_diff = FFMAX(max_diff, p->max_diff);
        diff1_sum_x2 += p->diff1_sum_x2;
        diff1_sum += p->diff1_sum;
        min_sigma_x2 = FFMIN(min_sigma_x2, p->min_sigma_x2);
        max_sigma_x2 = FFMAX(max_sigma_x2, p->max_sigma_x2);
        sigma_x2 += p->sigma_x2;
        noise_floor = FFMAX(noise_floor, p->noise_floor);
        p->entropy = calc_entropy(s, p);
        entropy += p->entropy;
        min_count += p->min_count;
        max_count += p->max_count;
        abs_peak_count += p->abs_peak_count;
        noise_floor_count += p->noise_floor_count;
        min_runs += p->min_runs;
        max_runs += p->max_runs;
        mask[0] |= p->mask[0];
        mask[1] |= p->mask[1];
        mask[2] &= p->mask[2];
        mask[3] |= p->mask[3];
        nb_samples += p->nb_samples;
        nb_nans += p->nb_nans;
        nb_infs += p->nb_infs;
        nb_denormals += p->nb_denormals;
        if (fabs(p->sigma_x) > fabs(max_sigma_x))
            max_sigma_x = p->sigma_x;

        if (s->measure_perchannel != MEASURE_NONE)
            av_log(ctx, AV_LOG_INFO, "Channel: %d\n", c + 1);
        if (s->measure_perchannel & MEASURE_DC_OFFSET)
            av_log(ctx, AV_LOG_INFO, "DC offset: %f\n", p->sigma_x / p->nb_samples);
        if (s->measure_perchannel & MEASURE_MIN_LEVEL)
            av_log(ctx, AV_LOG_INFO, "Min level: %f\n", p->min);
        if (s->measure_perchannel & MEASURE_MAX_LEVEL)
            av_log(ctx, AV_LOG_INFO, "Max level: %f\n", p->max);
        if (s->measure_perchannel & MEASURE_MIN_DIFFERENCE)
            av_log(ctx, AV_LOG_INFO, "Min difference: %f\n", p->min_diff);
        if (s->measure_perchannel & MEASURE_MAX_DIFFERENCE)
            av_log(ctx, AV_LOG_INFO, "Max difference: %f\n", p->max_diff);
        if (s->measure_perchannel & MEASURE_MEAN_DIFFERENCE)
            av_log(ctx, AV_LOG_INFO, "Mean difference: %f\n", p->diff1_sum / (p->nb_samples - 1));
        if (s->measure_perchannel & MEASURE_RMS_DIFFERENCE)
            av_log(ctx, AV_LOG_INFO, "RMS difference: %f\n", sqrt(p->diff1_sum_x2 / (p->nb_samples - 1)));
        if (s->measure_perchannel & MEASURE_PEAK_LEVEL)
            av_log(ctx, AV_LOG_INFO, "Peak level dB: %f\n", LINEAR_TO_DB(FFMAX(-p->nmin, p->nmax)));
        if (s->measure_perchannel & MEASURE_RMS_LEVEL)
            av_log(ctx, AV_LOG_INFO, "RMS level dB: %f\n", LINEAR_TO_DB(sqrt(p->sigma_x2 / p->nb_samples)));
        if (s->measure_perchannel & MEASURE_RMS_PEAK)
            av_log(ctx, AV_LOG_INFO, "RMS peak dB: %f\n", LINEAR_TO_DB(sqrt(p->max_sigma_x2)));
        if (s->measure_perchannel & MEASURE_RMS_TROUGH)
            if (p->min_sigma_x2 != 1)
                av_log(ctx, AV_LOG_INFO, "RMS trough dB: %f\n",LINEAR_TO_DB(sqrt(p->min_sigma_x2)));
        if (s->measure_perchannel & MEASURE_CREST_FACTOR)
            av_log(ctx, AV_LOG_INFO, "Crest factor: %f\n", p->sigma_x2 ? FFMAX(-p->nmin, p->nmax) / sqrt(p->sigma_x2 / p->nb_samples) : 1);
        if (s->measure_perchannel & MEASURE_FLAT_FACTOR)
            av_log(ctx, AV_LOG_INFO, "Flat factor: %f\n", LINEAR_TO_DB((p->min_runs + p->max_runs) / (p->min_count + p->max_count)));
        if (s->measure_perchannel & MEASURE_PEAK_COUNT)
            av_log(ctx, AV_LOG_INFO, "Peak count: %"PRId64"\n", p->min_count + p->max_count);
        if (s->measure_perchannel & MEASURE_ABS_PEAK_COUNT)
            av_log(ctx, AV_LOG_INFO, "Abs Peak count: %"PRId64"\n", p->abs_peak_count);
        if (s->measure_perchannel & MEASURE_NOISE_FLOOR)
            av_log(ctx, AV_LOG_INFO, "Noise floor dB: %f\n", LINEAR_TO_DB(p->noise_floor));
        if (s->measure_perchannel & MEASURE_NOISE_FLOOR_COUNT)
            av_log(ctx, AV_LOG_INFO, "Noise floor count: %"PRId64"\n", p->noise_floor_count);
        if (s->measure_perchannel & MEASURE_ENTROPY)
            av_log(ctx, AV_LOG_INFO, "Entropy: %f\n", p->entropy);
        if (s->measure_perchannel & MEASURE_BIT_DEPTH) {
            bit_depth(s, p->mask, depth);
            av_log(ctx, AV_LOG_INFO, "Bit depth: %u/%u/%u/%u\n", depth[0], depth[1], depth[2], depth[3]);
        }
        if (s->measure_perchannel & MEASURE_DYNAMIC_RANGE)
            av_log(ctx, AV_LOG_INFO, "Dynamic range: %f\n", LINEAR_TO_DB(2 * FFMAX(FFABS(p->min), FFABS(p->max))/ p->min_non_zero));
        if (s->measure_perchannel & MEASURE_ZERO_CROSSINGS)
            av_log(ctx, AV_LOG_INFO, "Zero crossings: %"PRId64"\n", p->zero_runs);
        if (s->measure_perchannel & MEASURE_ZERO_CROSSINGS_RATE)
            av_log(ctx, AV_LOG_INFO, "Zero crossings rate: %f\n", p->zero_runs/(double)p->nb_samples);
        if ((s->is_float || s->is_double) && s->measure_perchannel & MEASURE_NUMBER_OF_NANS)
            av_log(ctx, AV_LOG_INFO, "Number of NaNs: %"PRId64"\n", p->nb_nans);
        if ((s->is_float || s->is_double) && s->measure_perchannel & MEASURE_NUMBER_OF_INFS)
            av_log(ctx, AV_LOG_INFO, "Number of Infs: %"PRId64"\n", p->nb_infs);
        if ((s->is_float || s->is_double) && s->measure_perchannel & MEASURE_NUMBER_OF_DENORMALS)
            av_log(ctx, AV_LOG_INFO, "Number of denormals: %"PRId64"\n", p->nb_denormals);
    }

    if (nb_samples == 0 && !s->used)
        return;

    if (s->measure_overall != MEASURE_NONE)
        av_log(ctx, AV_LOG_INFO, "Overall\n");
    if (s->measure_overall & MEASURE_DC_OFFSET)
        av_log(ctx, AV_LOG_INFO, "DC offset: %f\n", max_sigma_x / (nb_samples / s->nb_channels));
    if (s->measure_overall & MEASURE_MIN_LEVEL)
        av_log(ctx, AV_LOG_INFO, "Min level: %f\n", min);
    if (s->measure_overall & MEASURE_MAX_LEVEL)
        av_log(ctx, AV_LOG_INFO, "Max level: %f\n", max);
    if (s->measure_overall & MEASURE_MIN_DIFFERENCE)
        av_log(ctx, AV_LOG_INFO, "Min difference: %f\n", min_diff);
    if (s->measure_overall & MEASURE_MAX_DIFFERENCE)
        av_log(ctx, AV_LOG_INFO, "Max difference: %f\n", max_diff);
    if (s->measure_overall & MEASURE_MEAN_DIFFERENCE)
        av_log(ctx, AV_LOG_INFO, "Mean difference: %f\n", diff1_sum / (nb_samples - s->nb_channels));
    if (s->measure_overall & MEASURE_RMS_DIFFERENCE)
        av_log(ctx, AV_LOG_INFO, "RMS difference: %f\n", sqrt(diff1_sum_x2 / (nb_samples - s->nb_channels)));
    if (s->measure_overall & MEASURE_PEAK_LEVEL)
        av_log(ctx, AV_LOG_INFO, "Peak level dB: %f\n", LINEAR_TO_DB(FFMAX(-nmin, nmax)));
    if (s->measure_overall & MEASURE_RMS_LEVEL)
        av_log(ctx, AV_LOG_INFO, "RMS level dB: %f\n", LINEAR_TO_DB(sqrt(sigma_x2 / nb_samples)));
    if (s->measure_overall & MEASURE_RMS_PEAK)
        av_log(ctx, AV_LOG_INFO, "RMS peak dB: %f\n", LINEAR_TO_DB(sqrt(max_sigma_x2)));
    if (s->measure_overall & MEASURE_RMS_TROUGH)
        if (min_sigma_x2 != 1)
            av_log(ctx, AV_LOG_INFO, "RMS trough dB: %f\n", LINEAR_TO_DB(sqrt(min_sigma_x2)));
    if (s->measure_overall & MEASURE_FLAT_FACTOR)
        av_log(ctx, AV_LOG_INFO, "Flat factor: %f\n", LINEAR_TO_DB((min_runs + max_runs) / (min_count + max_count)));
    if (s->measure_overall & MEASURE_PEAK_COUNT)
        av_log(ctx, AV_LOG_INFO, "Peak count: %f\n", (min_count + max_count) / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_ABS_PEAK_COUNT)
        av_log(ctx, AV_LOG_INFO, "Abs Peak count: %f\n", abs_peak_count / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_NOISE_FLOOR)
        av_log(ctx, AV_LOG_INFO, "Noise floor dB: %f\n", LINEAR_TO_DB(noise_floor));
    if (s->measure_overall & MEASURE_NOISE_FLOOR_COUNT)
        av_log(ctx, AV_LOG_INFO, "Noise floor count: %f\n", noise_floor_count / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_ENTROPY)
        av_log(ctx, AV_LOG_INFO, "Entropy: %f\n", entropy / (double)s->nb_channels);
    if (s->measure_overall & MEASURE_BIT_DEPTH) {
        bit_depth(s, mask, depth);
        av_log(ctx, AV_LOG_INFO, "Bit depth: %u/%u/%u/%u\n", depth[0], depth[1], depth[2], depth[3]);
    }
    if (s->measure_overall & MEASURE_NUMBER_OF_SAMPLES)
        av_log(ctx, AV_LOG_INFO, "Number of samples: %"PRId64"\n", nb_samples / s->nb_channels);
    if ((s->is_float || s->is_double) && s->measure_overall & MEASURE_NUMBER_OF_NANS)
        av_log(ctx, AV_LOG_INFO, "Number of NaNs: %f\n", nb_nans / (float)s->nb_channels);
    if ((s->is_float || s->is_double) && s->measure_overall & MEASURE_NUMBER_OF_INFS)
        av_log(ctx, AV_LOG_INFO, "Number of Infs: %f\n", nb_infs / (float)s->nb_channels);
    if ((s->is_float || s->is_double) && s->measure_overall & MEASURE_NUMBER_OF_DENORMALS)
        av_log(ctx, AV_LOG_INFO, "Number of denormals: %f\n", nb_denormals / (float)s->nb_channels);
}

static av_cold void uninit(AVFilterContext *ctx)
{
    AudioStatsContext *s = ctx->priv;

    if (s->nb_channels)
        print_stats(ctx);
    if (s->chstats) {
        for (int i = 0; i < s->nb_channels; i++) {
            ChannelStats *p = &s->chstats[i];

            av_freep(&p->win_samples);
            av_freep(&p->sorted_samples);
        }
    }
    av_freep(&s->chstats);
}

static const AVFilterPad astats_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_AUDIO,
        .filter_frame = filter_frame,
    },
};

static const AVFilterPad astats_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_AUDIO,
        .config_props = config_output,
    },
};

const FFFilter ff_af_astats = {
    .p.name        = "astats",
    .p.description = NULL_IF_CONFIG_SMALL("Show time domain statistics about audio frames."),
    .p.priv_class  = &astats_class,
    .p.flags       = AVFILTER_FLAG_SLICE_THREADS | AVFILTER_FLAG_METADATA_ONLY,
    .priv_size     = sizeof(AudioStatsContext),
    .uninit        = uninit,
    FILTER_INPUTS(astats_inputs),
    FILTER_OUTPUTS(astats_outputs),
    FILTER_SAMPLEFMTS(AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
                      AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
                      AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
                      AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
                      AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP),
};
