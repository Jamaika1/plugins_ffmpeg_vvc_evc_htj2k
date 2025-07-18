/*
 * MPEG-4 Parametric Stereo decoding functions
 * Copyright (c) 2010 Alex Converse <alex.converse@gmail.com>
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
 *
 * Note: Rounding-to-nearest used unless otherwise stated
 *
 */

#include <stdint.h>
#include "libavutil/common.h"
#include "libavutil/mathematics.h"
#include "libavutil/mem_internal.h"
#include "libavcodec/aacps.h"
#if USE_FIXED
#include "libavcodec/aacps_fixed_tablegen.h"
#else
#include "libavutil/internal.h"
#include "libavcodec/aacps_tablegen.h"
#endif /* USE_FIXED */

static const INTFLOAT g1_Q2[] = {
    Q31(0.0f),  Q31(0.01899487526049f), Q31(0.0f), Q31(-0.07293139167538f),
    Q31(0.0f),  Q31(0.30596630545168f), Q31(0.5f)
};

static void ipdopd_reset(int8_t *ipd_hist, int8_t *opd_hist)
{
    int i;
    for (i = 0; i < PS_MAX_NR_IPDOPD; i++) {
        opd_hist[i] = 0;
        ipd_hist[i] = 0;
    }
}

/** Split one subband into 2 subsubbands with a symmetric real filter.
 * The filter must have its non-center even coefficients equal to zero. */
static void hybrid2_re(INTFLOAT (*in)[2], INTFLOAT (*out)[32][2],
                       const INTFLOAT filter[7], int len, int reverse)
{
    int i, j;
    for (i = 0; i < len; i++, in++) {
        INT64FLOAT re_in = AAC_MUL31(filter[6], in[6][0]); //real inphase
        INT64FLOAT re_op = 0.0f;                          //real out of phase
        INT64FLOAT im_in = AAC_MUL31(filter[6], in[6][1]); //imag inphase
        INT64FLOAT im_op = 0.0f;                          //imag out of phase
        for (j = 0; j < 6; j += 2) {
            re_op += (INT64FLOAT)filter[j+1] * (in[j+1][0] + in[12-j-1][0]);
            im_op += (INT64FLOAT)filter[j+1] * (in[j+1][1] + in[12-j-1][1]);
        }

#if USE_FIXED
        re_op = (re_op + 0x40000000) >> 31;
        im_op = (im_op + 0x40000000) >> 31;
#endif /* USE_FIXED */

        out[ reverse][i][0] = (INTFLOAT)(re_in + re_op);
        out[ reverse][i][1] = (INTFLOAT)(im_in + im_op);
        out[!reverse][i][0] = (INTFLOAT)(re_in - re_op);
        out[!reverse][i][1] = (INTFLOAT)(im_in - im_op);
    }
}

/** Split one subband into 6 subsubbands with a complex filter */
static void hybrid6_cx(PSDSPContext *dsp, INTFLOAT (*in)[2], INTFLOAT (*out)[32][2],
                       TABLE_CONST INTFLOAT (*filter)[8][2], int len)
{
    int i;
    int N = 8;
    LOCAL_ALIGNED_16(INTFLOAT, temp, [8], [2]);

    for (i = 0; i < len; i++, in++) {
        dsp->hybrid_analysis(temp, in, (const INTFLOAT (*)[8][2]) filter, 1, N);
        out[0][i][0] = temp[6][0];
        out[0][i][1] = temp[6][1];
        out[1][i][0] = temp[7][0];
        out[1][i][1] = temp[7][1];
        out[2][i][0] = temp[0][0];
        out[2][i][1] = temp[0][1];
        out[3][i][0] = temp[1][0];
        out[3][i][1] = temp[1][1];
        out[4][i][0] = temp[2][0] + temp[5][0];
        out[4][i][1] = temp[2][1] + temp[5][1];
        out[5][i][0] = temp[3][0] + temp[4][0];
        out[5][i][1] = temp[3][1] + temp[4][1];
    }
}

static void hybrid4_8_12_cx(PSDSPContext *dsp,
                            INTFLOAT (*in)[2], INTFLOAT (*out)[32][2],
                            TABLE_CONST INTFLOAT (*filter)[8][2], int N, int len)
{
    int i;

    for (i = 0; i < len; i++, in++) {
        dsp->hybrid_analysis(out[0] + i, in, (const INTFLOAT (*)[8][2]) filter, 32, N);
    }
}

static void hybrid_analysis(PSDSPContext *dsp, INTFLOAT out[91][32][2],
                            INTFLOAT in[5][44][2], INTFLOAT L[2][38][64],
                            int is34, int len)
{
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 38; j++) {
            in[i][j+6][0] = L[0][j][i];
            in[i][j+6][1] = L[1][j][i];
        }
    }
    if (is34) {
        hybrid4_8_12_cx(dsp, in[0], out,    f34_0_12, 12, len);
        hybrid4_8_12_cx(dsp, in[1], out+12, f34_1_8,   8, len);
        hybrid4_8_12_cx(dsp, in[2], out+20, f34_2_4,   4, len);
        hybrid4_8_12_cx(dsp, in[3], out+24, f34_2_4,   4, len);
        hybrid4_8_12_cx(dsp, in[4], out+28, f34_2_4,   4, len);
        dsp->hybrid_analysis_ileave(out + 27, L, 5, len);
    } else {
        hybrid6_cx(dsp, in[0], out, f20_0_8, len);
        hybrid2_re(in[1], out+6, g1_Q2, len, 1);
        hybrid2_re(in[2], out+8, g1_Q2, len, 0);
        dsp->hybrid_analysis_ileave(out + 7, L, 3, len);
    }
    //update in_buf
    for (i = 0; i < 5; i++) {
        memcpy(in[i], in[i]+32, 6 * sizeof(in[i][0]));
    }
}

static void hybrid_synthesis(PSDSPContext *dsp, INTFLOAT out[2][38][64],
                             INTFLOAT in[91][32][2], int is34, int len)
{
    int i, n;
    if (is34) {
        for (n = 0; n < len; n++) {
            memset(out[0][n], 0, 5*sizeof(out[0][n][0]));
            memset(out[1][n], 0, 5*sizeof(out[1][n][0]));
            for (i = 0; i < 12; i++) {
                out[0][n][0] += (UINTFLOAT)in[   i][n][0];
                out[1][n][0] += (UINTFLOAT)in[   i][n][1];
            }
            for (i = 0; i < 8; i++) {
                out[0][n][1] += (UINTFLOAT)in[12+i][n][0];
                out[1][n][1] += (UINTFLOAT)in[12+i][n][1];
            }
            for (i = 0; i < 4; i++) {
                out[0][n][2] += (UINTFLOAT)in[20+i][n][0];
                out[1][n][2] += (UINTFLOAT)in[20+i][n][1];
                out[0][n][3] += (UINTFLOAT)in[24+i][n][0];
                out[1][n][3] += (UINTFLOAT)in[24+i][n][1];
                out[0][n][4] += (UINTFLOAT)in[28+i][n][0];
                out[1][n][4] += (UINTFLOAT)in[28+i][n][1];
            }
        }
        dsp->hybrid_synthesis_deint(out, in + 27, 5, len);
    } else {
        for (n = 0; n < len; n++) {
            out[0][n][0] = (UINTFLOAT)in[0][n][0] + in[1][n][0] + in[2][n][0] +
                           (UINTFLOAT)in[3][n][0] + in[4][n][0] + in[5][n][0];
            out[1][n][0] = (UINTFLOAT)in[0][n][1] + in[1][n][1] + in[2][n][1] +
                           (UINTFLOAT)in[3][n][1] + in[4][n][1] + in[5][n][1];
            out[0][n][1] = (UINTFLOAT)in[6][n][0] + in[7][n][0];
            out[1][n][1] = (UINTFLOAT)in[6][n][1] + in[7][n][1];
            out[0][n][2] = (UINTFLOAT)in[8][n][0] + in[9][n][0];
            out[1][n][2] = (UINTFLOAT)in[8][n][1] + in[9][n][1];
        }
        dsp->hybrid_synthesis_deint(out, in + 7, 3, len);
    }
}

/// All-pass filter decay slope
#define DECAY_SLOPE      Q30(0.05f)
/// Number of frequency bands that can be addressed by the parameter index, b(k)
static const int   NR_PAR_BANDS[]      = { 20, 34 };
static const int   NR_IPDOPD_BANDS[]   = { 11, 17 };
/// Number of frequency bands that can be addressed by the sub subband index, k
static const int   NR_BANDS[]          = { 71, 91 };
/// Start frequency band for the all-pass filter decay slope
static const int   DECAY_CUTOFF[]      = { 10, 32 };
/// Number of all-pass filer bands
static const int   NR_ALLPASS_BANDS[]  = { 30, 50 };
/// First stereo band using the short one sample delay
static const int   SHORT_DELAY_BAND[]  = { 42, 62 };

/** Table 8.46 */
static void map_idx_10_to_20(int8_t *par_mapped, const int8_t *par, int full)
{
    int b;
    if (full)
        b = 9;
    else {
        b = 4;
        par_mapped[10] = 0;
    }
    for (; b >= 0; b--) {
        par_mapped[2*b+1] = par_mapped[2*b] = par[b];
    }
}

static void map_idx_34_to_20(int8_t *par_mapped, const int8_t *par, int full)
{
    par_mapped[ 0] = (2*par[ 0] +   par[ 1]) / 3;
    par_mapped[ 1] = (  par[ 1] + 2*par[ 2]) / 3;
    par_mapped[ 2] = (2*par[ 3] +   par[ 4]) / 3;
    par_mapped[ 3] = (  par[ 4] + 2*par[ 5]) / 3;
    par_mapped[ 4] = (  par[ 6] +   par[ 7]) / 2;
    par_mapped[ 5] = (  par[ 8] +   par[ 9]) / 2;
    par_mapped[ 6] =    par[10];
    par_mapped[ 7] =    par[11];
    par_mapped[ 8] = (  par[12] +   par[13]) / 2;
    par_mapped[ 9] = (  par[14] +   par[15]) / 2;
    par_mapped[10] =    par[16];
    if (full) {
        par_mapped[11] =    par[17];
        par_mapped[12] =    par[18];
        par_mapped[13] =    par[19];
        par_mapped[14] = (  par[20] +   par[21]) / 2;
        par_mapped[15] = (  par[22] +   par[23]) / 2;
        par_mapped[16] = (  par[24] +   par[25]) / 2;
        par_mapped[17] = (  par[26] +   par[27]) / 2;
        par_mapped[18] = (  par[28] +   par[29] +   par[30] +   par[31]) / 4;
        par_mapped[19] = (  par[32] +   par[33]) / 2;
    }
}

static void map_val_34_to_20(INTFLOAT par[PS_MAX_NR_IIDICC])
{
#if USE_FIXED
    par[ 0] = (int)(((int64_t)(par[ 0] + (unsigned)(par[ 1]>>1)) * 1431655765 + \
                      0x40000000) >> 31);
    par[ 1] = (int)(((int64_t)((par[ 1]>>1) + (unsigned)par[ 2]) * 1431655765 + \
                      0x40000000) >> 31);
    par[ 2] = (int)(((int64_t)(par[ 3] + (unsigned)(par[ 4]>>1)) * 1431655765 + \
                      0x40000000) >> 31);
    par[ 3] = (int)(((int64_t)((par[ 4]>>1) + (unsigned)par[ 5]) * 1431655765 + \
                      0x40000000) >> 31);
#else
    par[ 0] = (2*par[ 0] +   par[ 1]) * 0.33333333f;
    par[ 1] = (  par[ 1] + 2*par[ 2]) * 0.33333333f;
    par[ 2] = (2*par[ 3] +   par[ 4]) * 0.33333333f;
    par[ 3] = (  par[ 4] + 2*par[ 5]) * 0.33333333f;
#endif /* USE_FIXED */
    par[ 4] = AAC_HALF_SUM(par[ 6], par[ 7]);
    par[ 5] = AAC_HALF_SUM(par[ 8], par[ 9]);
    par[ 6] =    par[10];
    par[ 7] =    par[11];
    par[ 8] = AAC_HALF_SUM(par[12], par[13]);
    par[ 9] = AAC_HALF_SUM(par[14], par[15]);
    par[10] =    par[16];
    par[11] =    par[17];
    par[12] =    par[18];
    par[13] =    par[19];
    par[14] = AAC_HALF_SUM(par[20], par[21]);
    par[15] = AAC_HALF_SUM(par[22], par[23]);
    par[16] = AAC_HALF_SUM(par[24], par[25]);
    par[17] = AAC_HALF_SUM(par[26], par[27]);
#if USE_FIXED
    par[18] = (((par[28]+2)>>2) + ((par[29]+2)>>2) + ((par[30]+2)>>2) + ((par[31]+2)>>2));
#else
    par[18] = (  par[28] +   par[29] +   par[30] +   par[31]) * 0.25f;
#endif /* USE_FIXED */
    par[19] = AAC_HALF_SUM(par[32], par[33]);
}

static void map_idx_10_to_34(int8_t *par_mapped, const int8_t *par, int full)
{
    if (full) {
        par_mapped[33] = par[9];
        par_mapped[32] = par[9];
        par_mapped[31] = par[9];
        par_mapped[30] = par[9];
        par_mapped[29] = par[9];
        par_mapped[28] = par[9];
        par_mapped[27] = par[8];
        par_mapped[26] = par[8];
        par_mapped[25] = par[8];
        par_mapped[24] = par[8];
        par_mapped[23] = par[7];
        par_mapped[22] = par[7];
        par_mapped[21] = par[7];
        par_mapped[20] = par[7];
        par_mapped[19] = par[6];
        par_mapped[18] = par[6];
        par_mapped[17] = par[5];
        par_mapped[16] = par[5];
    } else {
        par_mapped[16] =      0;
    }
    par_mapped[15] = par[4];
    par_mapped[14] = par[4];
    par_mapped[13] = par[4];
    par_mapped[12] = par[4];
    par_mapped[11] = par[3];
    par_mapped[10] = par[3];
    par_mapped[ 9] = par[2];
    par_mapped[ 8] = par[2];
    par_mapped[ 7] = par[2];
    par_mapped[ 6] = par[2];
    par_mapped[ 5] = par[1];
    par_mapped[ 4] = par[1];
    par_mapped[ 3] = par[1];
    par_mapped[ 2] = par[0];
    par_mapped[ 1] = par[0];
    par_mapped[ 0] = par[0];
}

static void map_idx_20_to_34(int8_t *par_mapped, const int8_t *par, int full)
{
    if (full) {
        par_mapped[33] =  par[19];
        par_mapped[32] =  par[19];
        par_mapped[31] =  par[18];
        par_mapped[30] =  par[18];
        par_mapped[29] =  par[18];
        par_mapped[28] =  par[18];
        par_mapped[27] =  par[17];
        par_mapped[26] =  par[17];
        par_mapped[25] =  par[16];
        par_mapped[24] =  par[16];
        par_mapped[23] =  par[15];
        par_mapped[22] =  par[15];
        par_mapped[21] =  par[14];
        par_mapped[20] =  par[14];
        par_mapped[19] =  par[13];
        par_mapped[18] =  par[12];
        par_mapped[17] =  par[11];
    }
    par_mapped[16] =  par[10];
    par_mapped[15] =  par[ 9];
    par_mapped[14] =  par[ 9];
    par_mapped[13] =  par[ 8];
    par_mapped[12] =  par[ 8];
    par_mapped[11] =  par[ 7];
    par_mapped[10] =  par[ 6];
    par_mapped[ 9] =  par[ 5];
    par_mapped[ 8] =  par[ 5];
    par_mapped[ 7] =  par[ 4];
    par_mapped[ 6] =  par[ 4];
    par_mapped[ 5] =  par[ 3];
    par_mapped[ 4] = (par[ 2] + par[ 3]) / 2;
    par_mapped[ 3] =  par[ 2];
    par_mapped[ 2] =  par[ 1];
    par_mapped[ 1] = (par[ 0] + par[ 1]) / 2;
    par_mapped[ 0] =  par[ 0];
}

static void map_val_20_to_34(INTFLOAT par[PS_MAX_NR_IIDICC])
{
    par[33] =  par[19];
    par[32] =  par[19];
    par[31] =  par[18];
    par[30] =  par[18];
    par[29] =  par[18];
    par[28] =  par[18];
    par[27] =  par[17];
    par[26] =  par[17];
    par[25] =  par[16];
    par[24] =  par[16];
    par[23] =  par[15];
    par[22] =  par[15];
    par[21] =  par[14];
    par[20] =  par[14];
    par[19] =  par[13];
    par[18] =  par[12];
    par[17] =  par[11];
    par[16] =  par[10];
    par[15] =  par[ 9];
    par[14] =  par[ 9];
    par[13] =  par[ 8];
    par[12] =  par[ 8];
    par[11] =  par[ 7];
    par[10] =  par[ 6];
    par[ 9] =  par[ 5];
    par[ 8] =  par[ 5];
    par[ 7] =  par[ 4];
    par[ 6] =  par[ 4];
    par[ 5] =  par[ 3];
    par[ 4] = AAC_HALF_SUM(par[ 2], par[ 3]);
    par[ 3] =  par[ 2];
    par[ 2] =  par[ 1];
    par[ 1] = AAC_HALF_SUM(par[ 0], par[ 1]);
}

static void decorrelation(PSContext *ps, INTFLOAT (*out)[32][2], const INTFLOAT (*s)[32][2], int is34)
{
    LOCAL_ALIGNED_16(INTFLOAT, power, [34], [PS_QMF_TIME_SLOTS]);
    LOCAL_ALIGNED_16(INTFLOAT, transient_gain, [34], [PS_QMF_TIME_SLOTS]);
    INTFLOAT *peak_decay_nrg = ps->peak_decay_nrg;
    INTFLOAT *power_smooth = ps->power_smooth;
    INTFLOAT *peak_decay_diff_smooth = ps->peak_decay_diff_smooth;
    INTFLOAT (*delay)[PS_QMF_TIME_SLOTS + PS_MAX_DELAY][2] = ps->delay;
    INTFLOAT (*ap_delay)[PS_AP_LINKS][PS_QMF_TIME_SLOTS + PS_MAX_AP_DELAY][2] = ps->ap_delay;
#if !USE_FIXED
    const float transient_impact  = 1.5f;
    const float a_smooth          = 0.25f; ///< Smoothing coefficient
#endif /* USE_FIXED */
    const int8_t *const k_to_i = is34 ? ff_k_to_i_34 : ff_k_to_i_20;
    int i, k, m, n;
    int n0 = 0, nL = 32;
    const INTFLOAT peak_decay_factor = Q31(0.76592833836465f);

    memset(power, 0, 34 * sizeof(*power));

    if (is34 != ps->common.is34bands_old) {
        memset(ps->peak_decay_nrg,         0, sizeof(ps->peak_decay_nrg));
        memset(ps->power_smooth,           0, sizeof(ps->power_smooth));
        memset(ps->peak_decay_diff_smooth, 0, sizeof(ps->peak_decay_diff_smooth));
        memset(ps->delay,                  0, sizeof(ps->delay));
        memset(ps->ap_delay,               0, sizeof(ps->ap_delay));
    }

    for (k = 0; k < NR_BANDS[is34]; k++) {
        int i = k_to_i[k];
        ps->dsp.add_squares(power[i], s[k], nL - n0);
    }

    //Transient detection
#if USE_FIXED
    for (i = 0; i < NR_PAR_BANDS[is34]; i++) {
        for (n = n0; n < nL; n++) {
            int decayed_peak;
            decayed_peak = (int)(((int64_t)peak_decay_factor * \
                                           peak_decay_nrg[i] + 0x40000000) >> 31);
            peak_decay_nrg[i] = FFMAX(decayed_peak, power[i][n]);
            power_smooth[i] += (power[i][n] + 2LL - power_smooth[i]) >> 2;
            peak_decay_diff_smooth[i] += (peak_decay_nrg[i] + 2LL - power[i][n] - \
                                          peak_decay_diff_smooth[i]) >> 2;

            if (peak_decay_diff_smooth[i]) {
                transient_gain[i][n] = FFMIN(power_smooth[i]*43691LL / peak_decay_diff_smooth[i], 1<<16);
            } else
                transient_gain[i][n] = 1 << 16;
        }
    }
#else
    for (i = 0; i < NR_PAR_BANDS[is34]; i++) {
        for (n = n0; n < nL; n++) {
            float decayed_peak = peak_decay_factor * peak_decay_nrg[i];
            float denom;
            peak_decay_nrg[i] = FFMAX(decayed_peak, power[i][n]);
            power_smooth[i] += a_smooth * (power[i][n] - power_smooth[i]);
            peak_decay_diff_smooth[i] += a_smooth * (peak_decay_nrg[i] - power[i][n] - peak_decay_diff_smooth[i]);
            denom = transient_impact * peak_decay_diff_smooth[i];
            transient_gain[i][n]   = (denom > power_smooth[i]) ?
                                         power_smooth[i] / denom : 1.0f;
        }
    }

#endif /* USE_FIXED */
    //Decorrelation and transient reduction
    //                         PS_AP_LINKS - 1
    //                               -----
    //                                | |  Q_fract_allpass[k][m]*z^-link_delay[m] - a[m]*g_decay_slope[k]
    //H[k][z] = z^-2 * phi_fract[k] * | | ----------------------------------------------------------------
    //                                | | 1 - a[m]*g_decay_slope[k]*Q_fract_allpass[k][m]*z^-link_delay[m]
    //                               m = 0
    //d[k][z] (out) = transient_gain_mapped[k][z] * H[k][z] * s[k][z]
    for (k = 0; k < NR_ALLPASS_BANDS[is34]; k++) {
        int b = k_to_i[k];
#if USE_FIXED
        int g_decay_slope;

        if (k - DECAY_CUTOFF[is34] <= 0) {
          g_decay_slope = 1 << 30;
        }
        else if (k - DECAY_CUTOFF[is34] >= 20) {
          g_decay_slope = 0;
        }
        else {
          g_decay_slope = (1 << 30) - DECAY_SLOPE * (k - DECAY_CUTOFF[is34]);
        }
#else
        float g_decay_slope = 1.f - DECAY_SLOPE * (k - DECAY_CUTOFF[is34]);
        g_decay_slope = av_clipf(g_decay_slope, 0.f, 1.f);
#endif /* USE_FIXED */
        memcpy(delay[k], delay[k]+nL, PS_MAX_DELAY*sizeof(delay[k][0]));
        memcpy(delay[k]+PS_MAX_DELAY, s[k], numQMFSlots*sizeof(delay[k][0]));
        for (m = 0; m < PS_AP_LINKS; m++) {
            memcpy(ap_delay[k][m],   ap_delay[k][m]+numQMFSlots,           5*sizeof(ap_delay[k][m][0]));
        }
        ps->dsp.decorrelate(out[k], delay[k] + PS_MAX_DELAY - 2, ap_delay[k],
                            phi_fract[is34][k],
                            (const INTFLOAT (*)[2]) Q_fract_allpass[is34][k],
                            transient_gain[b], g_decay_slope, nL - n0);
    }
    for (; k < SHORT_DELAY_BAND[is34]; k++) {
        int i = k_to_i[k];
        memcpy(delay[k], delay[k]+nL, PS_MAX_DELAY*sizeof(delay[k][0]));
        memcpy(delay[k]+PS_MAX_DELAY, s[k], numQMFSlots*sizeof(delay[k][0]));
        //H = delay 14
        ps->dsp.mul_pair_single(out[k], delay[k] + PS_MAX_DELAY - 14,
                                transient_gain[i], nL - n0);
    }
    for (; k < NR_BANDS[is34]; k++) {
        int i = k_to_i[k];
        memcpy(delay[k], delay[k]+nL, PS_MAX_DELAY*sizeof(delay[k][0]));
        memcpy(delay[k]+PS_MAX_DELAY, s[k], numQMFSlots*sizeof(delay[k][0]));
        //H = delay 1
        ps->dsp.mul_pair_single(out[k], delay[k] + PS_MAX_DELAY - 1,
                                transient_gain[i], nL - n0);
    }
}

static void remap34(int8_t (**p_par_mapped)[PS_MAX_NR_IIDICC],
                    int8_t           (*par)[PS_MAX_NR_IIDICC],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[PS_MAX_NR_IIDICC] = *p_par_mapped;
    int e;
    if (num_par == 20 || num_par == 11) {
        for (e = 0; e < num_env; e++) {
            map_idx_20_to_34(par_mapped[e], par[e], full);
        }
    } else if (num_par == 10 || num_par == 5) {
        for (e = 0; e < num_env; e++) {
            map_idx_10_to_34(par_mapped[e], par[e], full);
        }
    } else {
        *p_par_mapped = par;
    }
}

static void remap20(int8_t (**p_par_mapped)[PS_MAX_NR_IIDICC],
                    int8_t           (*par)[PS_MAX_NR_IIDICC],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[PS_MAX_NR_IIDICC] = *p_par_mapped;
    int e;
    if (num_par == 34 || num_par == 17) {
        for (e = 0; e < num_env; e++) {
            map_idx_34_to_20(par_mapped[e], par[e], full);
        }
    } else if (num_par == 10 || num_par == 5) {
        for (e = 0; e < num_env; e++) {
            map_idx_10_to_20(par_mapped[e], par[e], full);
        }
    } else {
        *p_par_mapped = par;
    }
}

static void stereo_processing(PSContext *ps, INTFLOAT (*l)[32][2], INTFLOAT (*r)[32][2], int is34)
{
    int e, b, k;

    PSCommonContext *const ps2 = &ps->common;
    INTFLOAT (*H11)[PS_MAX_NUM_ENV+1][PS_MAX_NR_IIDICC] = ps->H11;
    INTFLOAT (*H12)[PS_MAX_NUM_ENV+1][PS_MAX_NR_IIDICC] = ps->H12;
    INTFLOAT (*H21)[PS_MAX_NUM_ENV+1][PS_MAX_NR_IIDICC] = ps->H21;
    INTFLOAT (*H22)[PS_MAX_NUM_ENV+1][PS_MAX_NR_IIDICC] = ps->H22;
    int8_t *opd_hist = ps->opd_hist;
    int8_t *ipd_hist = ps->ipd_hist;
    int8_t iid_mapped_buf[PS_MAX_NUM_ENV][PS_MAX_NR_IIDICC];
    int8_t icc_mapped_buf[PS_MAX_NUM_ENV][PS_MAX_NR_IIDICC];
    int8_t ipd_mapped_buf[PS_MAX_NUM_ENV][PS_MAX_NR_IIDICC];
    int8_t opd_mapped_buf[PS_MAX_NUM_ENV][PS_MAX_NR_IIDICC];
    int8_t (*iid_mapped)[PS_MAX_NR_IIDICC] = iid_mapped_buf;
    int8_t (*icc_mapped)[PS_MAX_NR_IIDICC] = icc_mapped_buf;
    int8_t (*ipd_mapped)[PS_MAX_NR_IIDICC] = ipd_mapped_buf;
    int8_t (*opd_mapped)[PS_MAX_NR_IIDICC] = opd_mapped_buf;
    const int8_t *const k_to_i = is34 ? ff_k_to_i_34 : ff_k_to_i_20;
    TABLE_CONST INTFLOAT (*H_LUT)[8][4] = (PS_BASELINE || ps2->icc_mode < 3) ? HA : HB;

    //Remapping
    if (ps2->num_env_old) {
        memcpy(H11[0][0], H11[0][ps2->num_env_old], sizeof(H11[0][0]));
        memcpy(H11[1][0], H11[1][ps2->num_env_old], sizeof(H11[1][0]));
        memcpy(H12[0][0], H12[0][ps2->num_env_old], sizeof(H12[0][0]));
        memcpy(H12[1][0], H12[1][ps2->num_env_old], sizeof(H12[1][0]));
        memcpy(H21[0][0], H21[0][ps2->num_env_old], sizeof(H21[0][0]));
        memcpy(H21[1][0], H21[1][ps2->num_env_old], sizeof(H21[1][0]));
        memcpy(H22[0][0], H22[0][ps2->num_env_old], sizeof(H22[0][0]));
        memcpy(H22[1][0], H22[1][ps2->num_env_old], sizeof(H22[1][0]));
    }

    if (is34) {
        remap34(&iid_mapped, ps2->iid_par, ps2->nr_iid_par, ps2->num_env, 1);
        remap34(&icc_mapped, ps2->icc_par, ps2->nr_icc_par, ps2->num_env, 1);
        if (ps2->enable_ipdopd) {
            remap34(&ipd_mapped, ps2->ipd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
            remap34(&opd_mapped, ps2->opd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
        }
        if (!ps2->is34bands_old) {
            map_val_20_to_34(H11[0][0]);
            map_val_20_to_34(H11[1][0]);
            map_val_20_to_34(H12[0][0]);
            map_val_20_to_34(H12[1][0]);
            map_val_20_to_34(H21[0][0]);
            map_val_20_to_34(H21[1][0]);
            map_val_20_to_34(H22[0][0]);
            map_val_20_to_34(H22[1][0]);
            ipdopd_reset(ipd_hist, opd_hist);
        }
    } else {
        remap20(&iid_mapped, ps2->iid_par, ps2->nr_iid_par, ps2->num_env, 1);
        remap20(&icc_mapped, ps2->icc_par, ps2->nr_icc_par, ps2->num_env, 1);
        if (ps2->enable_ipdopd) {
            remap20(&ipd_mapped, ps2->ipd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
            remap20(&opd_mapped, ps2->opd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
        }
        if (ps2->is34bands_old) {
            map_val_34_to_20(H11[0][0]);
            map_val_34_to_20(H11[1][0]);
            map_val_34_to_20(H12[0][0]);
            map_val_34_to_20(H12[1][0]);
            map_val_34_to_20(H21[0][0]);
            map_val_34_to_20(H21[1][0]);
            map_val_34_to_20(H22[0][0]);
            map_val_34_to_20(H22[1][0]);
            ipdopd_reset(ipd_hist, opd_hist);
        }
    }

    //Mixing
    for (e = 0; e < ps2->num_env; e++) {
        for (b = 0; b < NR_PAR_BANDS[is34]; b++) {
            INTFLOAT h11, h12, h21, h22;
            h11 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][0];
            h12 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][1];
            h21 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][2];
            h22 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][3];

            if (!PS_BASELINE && ps2->enable_ipdopd && b < NR_IPDOPD_BANDS[is34]) {
                //The spec say says to only run this smoother when enable_ipdopd
                //is set but the reference decoder appears to run it constantly
                INTFLOAT h11i, h12i, h21i, h22i;
                INTFLOAT ipd_adj_re, ipd_adj_im;
                int opd_idx = opd_hist[b] * 8 + opd_mapped[e][b];
                int ipd_idx = ipd_hist[b] * 8 + ipd_mapped[e][b];
                INTFLOAT opd_re = pd_re_smooth[opd_idx];
                INTFLOAT opd_im = pd_im_smooth[opd_idx];
                INTFLOAT ipd_re = pd_re_smooth[ipd_idx];
                INTFLOAT ipd_im = pd_im_smooth[ipd_idx];
                opd_hist[b] = opd_idx & 0x3F;
                ipd_hist[b] = ipd_idx & 0x3F;

                ipd_adj_re = AAC_MADD30(opd_re, ipd_re, opd_im, ipd_im);
                ipd_adj_im = AAC_MSUB30(opd_im, ipd_re, opd_re, ipd_im);
                h11i = AAC_MUL30(h11,  opd_im);
                h11  = AAC_MUL30(h11,  opd_re);
                h12i = AAC_MUL30(h12,  ipd_adj_im);
                h12  = AAC_MUL30(h12,  ipd_adj_re);
                h21i = AAC_MUL30(h21,  opd_im);
                h21  = AAC_MUL30(h21,  opd_re);
                h22i = AAC_MUL30(h22,  ipd_adj_im);
                h22  = AAC_MUL30(h22,  ipd_adj_re);
                H11[1][e+1][b] = h11i;
                H12[1][e+1][b] = h12i;
                H21[1][e+1][b] = h21i;
                H22[1][e+1][b] = h22i;
            }
            H11[0][e+1][b] = h11;
            H12[0][e+1][b] = h12;
            H21[0][e+1][b] = h21;
            H22[0][e+1][b] = h22;
        }
        for (k = 0; k < NR_BANDS[is34]; k++) {
            LOCAL_ALIGNED_16(INTFLOAT, h, [2], [4]);
            LOCAL_ALIGNED_16(INTFLOAT, h_step, [2], [4]);
            int start = ps2->border_position[e];
            int stop  = ps2->border_position[e+1];
            INTFLOAT width = Q30(1.f) / ((stop - start) ? (stop - start) : 1);
#if USE_FIXED
            width = FFMIN(2U*width, INT_MAX);
#endif
            b = k_to_i[k];
            h[0][0] = H11[0][e][b];
            h[0][1] = H12[0][e][b];
            h[0][2] = H21[0][e][b];
            h[0][3] = H22[0][e][b];
            if (!PS_BASELINE && ps2->enable_ipdopd) {
            //Is this necessary? ps_04_new seems unchanged
            if ((is34 && k <= 13 && k >= 9) || (!is34 && k <= 1)) {
                h[1][0] = -H11[1][e][b];
                h[1][1] = -H12[1][e][b];
                h[1][2] = -H21[1][e][b];
                h[1][3] = -H22[1][e][b];
            } else {
                h[1][0] = H11[1][e][b];
                h[1][1] = H12[1][e][b];
                h[1][2] = H21[1][e][b];
                h[1][3] = H22[1][e][b];
            }
            }
            //Interpolation
            h_step[0][0] = AAC_MSUB31_V3(H11[0][e+1][b], h[0][0], width);
            h_step[0][1] = AAC_MSUB31_V3(H12[0][e+1][b], h[0][1], width);
            h_step[0][2] = AAC_MSUB31_V3(H21[0][e+1][b], h[0][2], width);
            h_step[0][3] = AAC_MSUB31_V3(H22[0][e+1][b], h[0][3], width);
            if (!PS_BASELINE && ps2->enable_ipdopd) {
                h_step[1][0] = AAC_MSUB31_V3(H11[1][e+1][b], h[1][0], width);
                h_step[1][1] = AAC_MSUB31_V3(H12[1][e+1][b], h[1][1], width);
                h_step[1][2] = AAC_MSUB31_V3(H21[1][e+1][b], h[1][2], width);
                h_step[1][3] = AAC_MSUB31_V3(H22[1][e+1][b], h[1][3], width);
            }
            if (stop - start)
                ps->dsp.stereo_interpolate[!PS_BASELINE && ps2->enable_ipdopd](
                    l[k] + 1 + start, r[k] + 1 + start,
                    h, h_step, stop - start);
        }
    }
}

int AAC_RENAME(ff_ps_apply)(PSContext *ps, INTFLOAT L[2][38][64], INTFLOAT R[2][38][64], int top)
{
    INTFLOAT (*Lbuf)[32][2] = ps->Lbuf;
    INTFLOAT (*Rbuf)[32][2] = ps->Rbuf;
    const int len = 32;
    int is34 = ps->common.is34bands;

    top += NR_BANDS[is34] - 64;
    memset(ps->delay+top, 0, (NR_BANDS[is34] - top)*sizeof(ps->delay[0]));
    if (top < NR_ALLPASS_BANDS[is34])
        memset(ps->ap_delay + top, 0, (NR_ALLPASS_BANDS[is34] - top)*sizeof(ps->ap_delay[0]));

    hybrid_analysis(&ps->dsp, Lbuf, ps->in_buf, L, is34, len);
    decorrelation(ps, Rbuf, (const INTFLOAT (*)[32][2]) Lbuf, is34);
    stereo_processing(ps, Lbuf, Rbuf, is34);
    hybrid_synthesis(&ps->dsp, L, Lbuf, is34, len);
    hybrid_synthesis(&ps->dsp, R, Rbuf, is34, len);

    return 0;
}

av_cold void AAC_RENAME(ff_ps_init)(void) {
    ps_tableinit();
}
