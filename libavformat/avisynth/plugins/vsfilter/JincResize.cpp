#include <cmath>
#include <cstring>

#include "JincResize.h"

static AVS_FORCEINLINE unsigned portable_clz(size_t x)
 {
 #if defined(_WIN32) && (defined(_M_IX86) || defined(_M_X64))
     unsigned long index;
     return (_BitScanReverse(&index, static_cast<unsigned long>(x))) ? (31 - index) : 32;
 #elif defined(_WIN32) && defined(_M_ARM64)
     return static_cast<unsigned>(__clz(static_cast<unsigned long>(x)));
 #else
     return (x == 0) ? 32 : __builtin_clz(static_cast<unsigned>(x));
 #endif
 }

#ifndef M_PI // GCC seems to have it
static constexpr double M_PI = 3.14159265358979323846;
#endif

// Taylor series coefficients of 2*BesselJ1(pi*x)/(pi*x) as (x^2) -> 0
static constexpr double jinc_taylor_series[31] =
{
    1.0,
    -1.23370055013616982735431137,
    0.507339015802096027273126733,
    -0.104317403816764804365258186,
    0.0128696438477519721233840271,
    -0.00105848577966854543020422691,
    6.21835470803998638484476598e-05,
    -2.73985272294670461142756204e-06,
    9.38932725442064547796003405e-08,
    -2.57413737759717407304931036e-09,
    5.77402672521402031756429343e-11,
    -1.07930605263598241754572977e-12,
    1.70710316782347356046974552e-14,
    -2.31434518382749184406648762e-16,
    2.71924659665997312120515390e-18,
    -2.79561335187943028518083529e-20,
    2.53599244866299622352138464e-22,
    -2.04487273140961494085786452e-24,
    1.47529860450204338866792475e-26,
    -9.57935105257523453155043307e-29,
    5.62764317309979254140393917e-31,
    -3.00555258814860366342363867e-33,
    1.46559362903641161989338221e-35,
    -6.55110024064596600335624426e-38,
    2.69403199029404093412381643e-40,
    -1.02265499954159964097119923e-42,
    3.59444454568084324694180635e-45,
    -1.17313973900539982313119019e-47,
    3.56478606255557746426034301e-50,
    -1.01100655781438313239513538e-52,
    2.68232117541264485328658605e-55
};

static constexpr double jinc_zeros[16] =
{
    1.2196698912665045,
    2.2331305943815286,
    3.2383154841662362,
    4.2410628637960699,
    5.2427643768701817,
    6.2439216898644877,
    7.2447598687199570,
    8.2453949139520427,
    9.2458926849494673,
    10.246293348754916,
    11.246622794877883,
    12.246898461138105,
    13.247132522181061,
    14.247333735806849,
    15.247508563037300,
    16.247661874700962
};

//  Modified from boost package math/tools/`rational.hpp`
//
//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
static double evaluate_rational(const double* num, const double* denom, double z, int count)
{
    double s1, s2;
    if (z <= 1.0)
    {
        s1 = num[count - 1];
        s2 = denom[count - 1];
        for (auto i = count - 2; i >= 0; --i)
        {
            s1 *= z;
            s2 *= z;
            s1 += num[i];
            s2 += denom[i];
        }
    }
    else
    {
        z = 1.0f / z;
        s1 = num[0];
        s2 = denom[0];
        for (auto i = 1; i < count; ++i)
        {
            s1 *= z;
            s2 *= z;
            s1 += num[i];
            s2 += denom[i];
        }
    }

    return s1 / s2;
}

//  Modified from boost package `BesselJ1.hpp`
//
//  Copyright (c) 2006 Xiaogang Zhang
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
static double jinc_sqr_boost_l(double x2)
{
    constexpr double bPC[7] =
    {
        -4.4357578167941278571e+06,
        -9.9422465050776411957e+06,
        -6.6033732483649391093e+06,
        -1.5235293511811373833e+06,
        -1.0982405543459346727e+05,
        -1.6116166443246101165e+03,
        0.0
    };
    constexpr double bQC[7] =
    {
        -4.4357578167941278568e+06,
        -9.9341243899345856590e+06,
        -6.5853394797230870728e+06,
        -1.5118095066341608816e+06,
        -1.0726385991103820119e+05,
        -1.4550094401904961825e+03,
        1.0
    };
    constexpr double bPS[7] =
    {
        3.3220913409857223519e+04,
        8.5145160675335701966e+04,
        6.6178836581270835179e+04,
        1.8494262873223866797e+04,
        1.7063754290207680021e+03,
        3.5265133846636032186e+01,
        0.0
    };
    constexpr double bQS[7] =
    {
        7.0871281941028743574e+05,
        1.8194580422439972989e+06,
        1.4194606696037208929e+06,
        4.0029443582266975117e+05,
        3.7890229745772202641e+04,
        8.6383677696049909675e+02,
        1.0
    };

    const auto y2 = M_PI * M_PI * x2;
    const auto xp = sqrt(y2);
    const auto y2p = 64.0 / y2;
    const auto sx = sin(xp);
    const auto cx = cos(xp);

    return (sqrt(xp / M_PI) * 2.0 / y2) * (evaluate_rational(bPC, bQC, y2p, 7) * (sx - cx) + (8.0 / xp) * evaluate_rational(bPS, bQS, y2p, 7) * (sx + cx));
}

// jinc(sqrt(x2))
static double jinc_sqr(double x2)
{
    if (x2 < 1.49)        // the 1-tap radius
    {
        double res = 0.0;
        for (auto j = 16; j > 0; --j)
            res = res * x2 + jinc_taylor_series[j - 1];
        return res;
    }
    else if (x2 < 4.97)   // the 2-tap radius
    {
        double res = 0.0;
        for (auto j = 21; j > 0; --j)
            res = res * x2 + jinc_taylor_series[j - 1];
        return res;
    }
    else if (x2 < 10.49)  // the 3-tap radius
    {
        double res = 0.0;
        for (auto j = 26; j > 0; --j)
            res = res * x2 + jinc_taylor_series[j - 1];
        return res;
    }
    else if (x2 < 17.99)  // the 4-tap radius
    {
        double res = 0.0;
        for (auto j = 31; j > 0; --j)
            res = res * x2 + jinc_taylor_series[j - 1];
        return res;
    }
    else if (x2 < 52.57)  // the 5~7-tap radius
    {
        const auto x = M_PI * sqrt(x2);
        return 2.0 * std::cyl_bessel_j(1, x) / x;
    }
    else if (x2 < 68.07)  // the 8-tap radius // Modify from pull request #4
    {
        return jinc_sqr_boost_l(x2);
    }
    else                  // the 9~16-tap radius
    {
        const auto x = M_PI * sqrt(x2);
        return 2.0 * std::cyl_bessel_j(1, x) / x;
    }
}

static double sample_sqr(double (*filter)(double), double x2, double blur2, double radius2)
{
    if (blur2 > 0.0)
        x2 /= blur2;

    if (x2 < radius2)
        return filter(x2);

    return 0.0;
}

constexpr double JINC_ZERO_SQR = 1.48759464366204680005356;

Lut::Lut()
{
    lut = new double[lut_size];
}

void Lut::InitLut(int lut_size, double radius, double blur)
{
    const auto radius2 = radius * radius;
    const auto blur2 = blur * blur;

    for (auto i = 0; i < lut_size; ++i)
    {
        const auto t2 = i / (lut_size - 1.0);
        lut[i] = sample_sqr(jinc_sqr, radius2 * t2, blur2, radius2) * sample_sqr(jinc_sqr, JINC_ZERO_SQR * t2, 1.0, radius2);
    }
}

float Lut::GetFactor(int index)
{
    if (index >= lut_size)
        return 0.f;
    return static_cast<float>(lut[index]);
}

constexpr double DOUBLE_ROUND_MAGIC_NUMBER = 6755399441055744.0;

static void init_coeff_table(EWAPixelCoeff* out, int quantize_x, int quantize_y,
    int filter_size, int dst_width, int dst_height)
{
    out->filter_size = filter_size;
    out->coeff_stride = (filter_size + 15) & ~15;

    // Allocate metadata
    out->meta = new EWAPixelCoeffMeta[static_cast<int64_t>(dst_width) * dst_height];

    // Alocate factor map
    out->factor_map = new int[static_cast<int64_t>(quantize_x) * quantize_y];

    // This will be reserved to exact size in coff generating procedure
    out->factor = nullptr;

    // Zeroed memory
    if (out->factor_map != nullptr)
        memset(out->factor_map, 0, static_cast<int64_t>(quantize_x) * quantize_y * sizeof(int));

    memset(out->meta, 0, static_cast<int64_t>(dst_width) * dst_height * sizeof(EWAPixelCoeffMeta));
}

static void delete_coeff_table(EWAPixelCoeff* out)
{
    _aligned_free(out->factor);
    delete[] out->meta;
    delete[] out->factor_map;
}

struct generate_coeff_params
{
    Lut* func;
    EWAPixelCoeff* out;
    int quantize_x;
    int quantize_y;
    int samples;
    int src_width;
    int src_height;
    int dst_width;
    int dst_height;
    double radius;
    double crop_left;
    double crop_top;
    double crop_width;
    double crop_height;
    int initial_capacity;
    double initial_factor;
};

/* Coefficient table generation */
static void generate_coeff_table_c(const generate_coeff_params& params)
{
    Lut* func = params.func;
    EWAPixelCoeff* out = params.out;
    int quantize_x = params.quantize_x;
    int quantize_y = params.quantize_y;
    int samples = params.samples;
    int src_width = params.src_width;
    int src_height = params.src_height;
    int dst_width = params.dst_width;
    int dst_height = params.dst_height;
    double radius = params.radius;

    const double filter_step_x = std::min(static_cast<double>(dst_width) / params.crop_width, 1.0);
    const double filter_step_y = std::min(static_cast<double>(dst_height) / params.crop_height, 1.0);

    const float filter_support_x = static_cast<float>(radius / filter_step_x);
    const float filter_support_y = static_cast<float>(radius / filter_step_y);

    const float filter_support = std::max(filter_support_x, filter_support_y);
    const int filter_size = std::max(static_cast<int>(ceil(filter_support_x * 2.0)), static_cast<int>(ceil(filter_support_y * 2.0)));

    const float start_x = static_cast<float>(params.crop_left + (params.crop_width / dst_width - 1.0) / 2.0);

    const float x_step = static_cast<float>(params.crop_width / dst_width);
    const float y_step = static_cast<float>(params.crop_height / dst_height);

    float xpos = start_x;
    float ypos = static_cast<float>(params.crop_top + (params.crop_height - dst_height) / (dst_height * static_cast<int64_t>(2)));

    // Initialize EWAPixelCoeff data structure
    init_coeff_table(out, quantize_x, quantize_y, filter_size, dst_width, dst_height);

    size_t tmp_array_capacity = params.initial_capacity;
    float* tmp_array = static_cast<float*>(_aligned_malloc(tmp_array_capacity * sizeof(float), 64));
    if (!tmp_array)
        throw "JincResize: failed to allocate tmp_array.";
    size_t tmp_array_size = 0;
    int tmp_array_top = 0;
    unsigned base_clz = portable_clz(tmp_array_capacity);
    const double initial_growth_factor = params.initial_factor;
    const double radius2 = radius * radius;

    // Use to advance the coeff pointer
    const int coeff_per_pixel = out->coeff_stride * filter_size;

    for (int y = 0; y < dst_height; ++y)
    {
        for (int x = 0; x < dst_width; ++x)
        {
            bool is_border = false;

            EWAPixelCoeffMeta* meta = &out->meta[y * dst_width + x];

            // Here, the window_*** variable specified a begin/size/end
            // of EWA window to process.
            int window_end_x = static_cast<int>(xpos + filter_support);
            int window_end_y = static_cast<int>(ypos + filter_support);

            if (window_end_x >= src_width)
            {
                window_end_x = src_width - 1;
                is_border = true;
            }
            if (window_end_y >= src_height)
            {
                window_end_y = src_height - 1;
                is_border = true;
            }

            int window_begin_x = window_end_x - filter_size + 1;
            int window_begin_y = window_end_y - filter_size + 1;

            if (window_begin_x < 0)
            {
                window_begin_x = 0;
                is_border = true;
            }
            if (window_begin_y < 0)
            {
                window_begin_y = 0;
                is_border = true;
            }

            meta->start_x = window_begin_x;
            meta->start_y = window_begin_y;

            // Quantize xpos and ypos
            const int quantized_x_int = static_cast<int>(xpos * quantize_x);
            const int quantized_y_int = static_cast<int>(ypos * quantize_y);
            const int quantized_x_value = quantized_x_int % quantize_x;
            const int quantized_y_value = quantized_y_int % quantize_y;
            const float quantized_xpos = static_cast<float>(quantized_x_int) / quantize_x;
            const float quantized_ypos = static_cast<float>(quantized_y_int) / quantize_y;

            if (!is_border && out->factor_map[quantized_y_value * quantize_x + quantized_x_value] != 0)
            {
                // Not border pixel and already have coefficient calculated at this quantized position
                meta->coeff_meta = out->factor_map[quantized_y_value * quantize_x + quantized_x_value] - 1;
            }
            else
            {
                // then need computation
                float divider = 0.f;

                // This is the location of current target pixel in source pixel
                // Quantized
                //const float current_x = clamp(is_border ? xpos : quantized_xpos, 0.f, src_width - 1.f);
                //const float current_y = clamp(is_border ? ypos : quantized_ypos, 0.f, src_height - 1.f);

                if (!is_border)
                {
                    // Change window position to quantized position
                    window_begin_x = static_cast<int>(quantized_xpos + filter_support) - filter_size + 1;
                    window_begin_y = static_cast<int>(quantized_ypos + filter_support) - filter_size + 1;
                }

                // Windowing positon
                int window_x = window_begin_x;
                int window_y = window_begin_y;

                // First loop calcuate coeff
                const size_t new_size = tmp_array_size + coeff_per_pixel;
                if (new_size > tmp_array_capacity)
                {
                    size_t new_capacity = tmp_array_capacity * (1.0 + (initial_growth_factor - 1.0)
                        * (1.0 - static_cast<double>(std::max(0, static_cast<int>(base_clz - portable_clz(tmp_array_capacity)))) / 32.0));
                    if (new_capacity < new_size)
                        new_capacity = new_size;
                    float* new_tmp = static_cast<float*>(_aligned_malloc(new_capacity * sizeof(float), 64));
                    if (!new_tmp)
                    {
                        _aligned_free(tmp_array);
                        throw "JincResize: failed to allocate new_tmp.";
                    }
                    memcpy(new_tmp, tmp_array, tmp_array_size * sizeof(float));
                    _aligned_free(tmp_array);
                    tmp_array = new_tmp;
                    tmp_array_capacity = new_capacity;
                }
                memset(tmp_array + tmp_array_size, 0, coeff_per_pixel * sizeof(float));
                int curr_factor_ptr = tmp_array_top;
                tmp_array_size = new_size;

                for (int ly = 0; ly < filter_size; ++ly)
                {
                    for (int lx = 0; lx < filter_size; ++lx)
                    {
                        // Euclidean distance to sampling pixel
                        const double dx = (clamp(is_border ? xpos : quantized_xpos, 0.f, static_cast<float>(src_width - 1)) - window_x) * filter_step_x;
                        const double dy = (clamp(is_border ? ypos : quantized_ypos, 0.f, static_cast<float>(src_height - 1)) - window_y) * filter_step_y;

                        int index = static_cast<int>(llround((samples - 1) * (dx * dx + dy * dy) / radius2 + DOUBLE_ROUND_MAGIC_NUMBER));

                        const float factor = func->GetFactor(index);

                        tmp_array[curr_factor_ptr + static_cast<int64_t>(lx)] = factor;
                        divider += factor;

                        ++window_x;
                    }

                    curr_factor_ptr += out->coeff_stride;

                    window_x = window_begin_x;
                    ++window_y;
                }

                // Second loop to divide the coeff
                curr_factor_ptr = tmp_array_top;
                for (int ly = 0; ly < filter_size; ++ly)
                {
                    for (int lx = 0; lx < filter_size; ++lx)
                    {
                        tmp_array[curr_factor_ptr + static_cast<int64_t>(lx)] /= divider;
                    }

                    curr_factor_ptr += out->coeff_stride;
                }

                // Save factor to table
                if (!is_border)
                    out->factor_map[quantized_y_value * quantize_x + quantized_x_value] = tmp_array_top + 1;

                meta->coeff_meta = tmp_array_top;
                tmp_array_top += coeff_per_pixel;
            }

            xpos += x_step;
        }

        ypos += y_step;
        xpos = start_x;
    }

    // Copy from tmp_array to real array
    out->factor = tmp_array;
}

/* Planar resampling with coeff table */
template<typename T, int thr, int subsampled>
void JincResize::resize_plane_c(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env)
{
    const int planes_y[4] = { PLANAR_Y, PLANAR_U, PLANAR_V, PLANAR_A };
    const int planes_r[4] = { PLANAR_G, PLANAR_B, PLANAR_R, PLANAR_A };
    const int* current_planes = (vi.IsRGB()) ? planes_r : planes_y;
    for (int i = 0; i < planecount; ++i)
    {
        const int plane = current_planes[i];

        const int src_stride = src->GetPitch(plane) / sizeof(T);
        const int dst_stride = dst->GetPitch(plane) / sizeof(T);
        const int dst_width = dst->GetRowSize(plane) / sizeof(T);
        const int dst_height = dst->GetHeight(plane);
        const T* srcp = reinterpret_cast<const T*>(src->GetReadPtr(plane));

        EWAPixelCoeff* out = [&]()
        {
            if constexpr (subsampled)
                return (i) ? (i == 3) ? JincResize::out1[0] : JincResize::out1[1] : JincResize::out1[0];
            else
                return JincResize::out1[0];
        }();

        auto loop = [&](int y)
        {
            T* __restrict dstp = reinterpret_cast<T*>(dst->GetWritePtr(plane)) + static_cast<int64_t>(y) * dst_stride;

            for (int x = 0; x < dst_width; ++x)
            {
                EWAPixelCoeffMeta* meta = out->meta + static_cast<int64_t>(y) * dst_width + x;
                const T* src_ptr = srcp + meta->start_y * static_cast<int64_t>(src_stride) + meta->start_x;
                const float* coeff_ptr = out->factor + meta->coeff_meta;

                float result = 0.f;

                for (int ly = 0; ly < out->filter_size; ++ly)
                {
                    for (int lx = 0; lx < out->filter_size; ++lx)
                        result += src_ptr[lx] * coeff_ptr[lx];

                    coeff_ptr += out->coeff_stride;
                    src_ptr += src_stride;
                }

                if constexpr (std::is_integral_v<T>)
                    dstp[x] = static_cast<T>(lrintf(clamp(result, 0.f, peak)));
                else
                    dstp[x] = result;

            }
        };

        if constexpr (thr)
        {
            for (intptr_t i = 0; i < dst_height; ++i)
                loop(i);
        }
        else
        {
            std::vector<int> l(dst_height);
            std::iota(std::begin(l), std::end(l), 0);
            std::for_each(std::execution::par, std::begin(l), std::end(l), loop);
        }
    }
}

/* Planar resampling with single point of coeff table for all planes */
template<typename T, int thr, int subsampled>
void JincResize::resize_eqplanes_c(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env)
{
	const int planes_y[4] = { PLANAR_Y, PLANAR_U, PLANAR_V, PLANAR_A };
	const int planes_r[4] = { PLANAR_G, PLANAR_B, PLANAR_R, PLANAR_A };
	const int* current_planes = (vi.IsRGB()) ? planes_r : planes_y;

	int src_stride[4];
	int dst_stride[4];
	T* srcp[4];

	for (int i = 0; i < planecount; ++i)
	{
		const int plane = current_planes[i];

		src_stride[i] = src->GetPitch(plane) / sizeof(T);
		dst_stride[i] = dst->GetPitch(plane) / sizeof(T);
		srcp[i] = (T*)(src->GetReadPtr(plane));
	}

	const int dst_width = dst->GetRowSize(current_planes[0]) / sizeof(T); // must be equal for all planes
	const int dst_height = dst->GetHeight(current_planes[0]); // must be equal for all planes

	EWAPixelCoeff* out = JincResize::out1[0];

	auto loop = [&](int y)
	{
		T* dstp_planes[4];

		for (int i = 0; i < planecount; ++i)
		{
			const int plane = current_planes[i];
			dstp_planes[i] = reinterpret_cast<T*>(dst->GetWritePtr(plane)) + static_cast<int64_t>(y)* dst_stride[i];
		}

		for (int x = 0; x < dst_width; ++x)
		{
			EWAPixelCoeffMeta* meta = out->meta + static_cast<int64_t>(y)* dst_width + x; // one for all planes
			const float* coeff_ptr = out->factor + meta->coeff_meta; // one for all planes

			for (int i = 0; i < planecount; ++i)
			{
				const T* src_ptr = srcp[i] + meta->start_y * static_cast<int64_t>(src_stride[i]) + meta->start_x;
				T* dstp = dstp_planes[i];

				float result = 0.f;

				for (int ly = 0; ly < out->filter_size; ++ly)
				{
					for (int lx = 0; lx < out->filter_size; ++lx)
						result += src_ptr[lx] * coeff_ptr[lx];

					coeff_ptr += out->coeff_stride;
					src_ptr += src_stride[i];
				}

				if constexpr (std::is_integral_v<T>)
					dstp[x] = static_cast<T>(lrintf(clamp(result, 0.f, peak)));
				else
					dstp[x] = result;
			}
		}

	};

	if constexpr (thr)
	{
		for (intptr_t i = 0; i < dst_height; ++i)
			loop(i);
	}
	else
	{
		std::vector<int> l(dst_height);
		std::iota(std::begin(l), std::end(l), 0);
		std::for_each(std::execution::par, std::begin(l), std::end(l), loop);
	}
}

JincResize::JincResize(PClip _child, int target_width, int target_height, double crop_left, double crop_top, double crop_width, double crop_height, int quant_x, int quant_y, int tap, double blur,
    std::string cplace_, int threads, int opt, int initial_capacity, double initial_factor, IScriptEnvironment* env)
    : GenericVideoFilter(_child), cplace(cplace_)
{
    if (!vi.IsPlanar())
        env->ThrowError("JincResize: clip must be in planar format.");
    if (tap < 1 || tap > 16)
        env->ThrowError("JincResize: tap must be between 1..16.");
    if (quant_x < 1 || quant_x > 256)
        env->ThrowError("JincResize: quant_x must be between 1..256.");
    if (quant_y < 1 || quant_y > 256)
        env->ThrowError("JincResize: quant_y must be between 1..256.");

    if (!cplace.empty())
    {
        for (auto& c : cplace)
            c = tolower(c);

        if (cplace != "mpeg2" && cplace != "mpeg1" && cplace != "topleft")
            env->ThrowError("JincResize: cplace must be MPEG2, MPEG1 or topleft.");
    }
    else
    {
        PVideoFrame frame0 = child->GetFrame(0, env);
        const AVSMap* props = env->getFramePropsRO(frame0);

        if (env->propGetType(props, "_ChromaLocation") == 'i')
        {
            switch (env->propGetInt(props, "_ChromaLocation", 0, nullptr))
            {
                case 0: cplace = "mpeg2"; break;
                case 1: cplace = "mpeg1"; break;
                case 2: cplace = "topleft"; break;
                default: env->ThrowError((std::string("JincResize: _ChromaLocation ") + std::to_string(env->propGetInt(props, "_ChromaLocation", 0, nullptr)) + " is not supported.").c_str()); break;
            }
        }
            else
                cplace = "mpeg2";
    }

    if (cplace == "topleft" && !vi.Is420())
        env->ThrowError("JincResize: topleft must be used only for 4:2:0 chroma subsampling.");
    if (opt > 3)
        env->ThrowError("JincResize: opt higher than 3 is not allowed.");
    if (opt == 3 && !(env->GetCPUFlags() & CPUF_AVX512F))
        env->ThrowError("JincResize: opt=3 requires AVX-512F.");
    if (opt == 2 && !(env->GetCPUFlags() & CPUF_AVX2))
        env->ThrowError("JincResize: opt=2 requires AVX2.");
    if (opt == 1 && !(env->GetCPUFlags() & CPUF_SSE4_1))
        env->ThrowError("JincResize: opt=1 requires SSE4.1.");
    if (threads < 0 || threads > 1)
        env->ThrowError("JincResize: threads must be either 0 or 1.");

    if (crop_width <= 0.0)
        crop_width = vi.width - crop_left + crop_width;
    if (crop_height <= 0.0)
        crop_height = vi.height - crop_top + crop_height;

    if (!blur)
        blur = 1.0;

    if (initial_factor < 1.0)
        env->ThrowError("JincResize: initial_factor must be eqaul to or greater than 1.0.");

    const int src_width = vi.width;
    const int src_height = vi.height;

    if (initial_capacity <= 0)
        env->ThrowError("JincResize: initial_capacity must be greater than 0.");

    vi.width = target_width;
    vi.height = target_height;
    peak = static_cast<float>((1 << vi.BitsPerComponent()) - 1);
    const double radius = jinc_zeros[tap - 1];
    constexpr int samples = 1024;  // should be a multiple of 4
    init_lut = new Lut();
    init_lut->InitLut(samples, radius, blur);
    planecount = vi.NumComponents();
    bool subsampled = false;
    std::vector<EWAPixelCoeff*>* out = &out1;
    out->emplace_back(new EWAPixelCoeff());
    generate_coeff_params params =
    {
        init_lut,
        out1[0],
        quant_x,
        quant_y,
        samples,
        src_width,
        src_height,
        target_width,
        target_height,
        radius,
        crop_left,
        crop_top,
        crop_width,
        crop_height,
        initial_capacity,
        initial_factor
    };

    try
    {
        if (planecount > 1)
        {
            if (vi.Is444() || vi.IsRGB())
                generate_coeff_table_c(params);
            else
            {
                out->emplace_back(new EWAPixelCoeff());

                subsampled = true;
                const int sub_w = vi.GetPlaneWidthSubsampling(PLANAR_U);
                const int sub_h = vi.GetPlaneHeightSubsampling(PLANAR_U);
                const double div_w = 1 << sub_w;
                const double div_h = 1 << sub_h;

                const double crop_left_uv = (cplace == "mpeg2" || cplace == "topleft") ?
                    (0.5 * (1.0 - static_cast<double>(src_width) / target_width) + crop_left) / div_w : crop_left / div_w;
                const double crop_top_uv = (cplace == "topleft") ?
                    (0.5 * (1.0 - static_cast<double>(src_height) / target_height) + crop_top) / div_h : crop_top / div_h;

                generate_coeff_table_c(params);
                 params = {
                     init_lut,
                     (*out)[1],
                     quant_x,
                     quant_y,
                     samples,
                     src_width >> sub_w,
                     src_height >> sub_h,
                     target_width >> sub_w,
                     target_height >> sub_h,
                     radius,
                     crop_left_uv,
                     crop_top_uv,
                     crop_width / div_w,
                     crop_height / div_h,
                     initial_capacity / (static_cast<int>(div_w) * static_cast<int>(div_h)),
                     initial_factor
                 };
                 generate_coeff_table_c(params);
            }
        }
        else
            generate_coeff_table_c(params);
    }
    catch (const std::exception& e)
    {
        std::vector<EWAPixelCoeff*>* out = &out1;
        for (int i = 0; i < static_cast<int>(out1.size()); ++i)
        {
            delete_coeff_table((*out)[i]);
            delete (*out)[i];
        }

        delete[] init_lut->lut;
        delete init_lut;

        env->ThrowError(e.what());
    }
    catch (const char* e)
    {
        std::vector<EWAPixelCoeff*>* out = &out1;
        for (int i = 0; i < static_cast<int>(out1.size()); ++i)
        {
            delete_coeff_table((*out)[i]);
            delete (*out)[i];
        }

        delete[] init_lut->lut;
        delete init_lut;

        env->ThrowError(e);
    }


#if defined(__AVX512F__)
    const bool avx512 = (opt == 3);
#else
    const bool avx512 = false;
#endif
#if defined(__AVX2__)
    const bool avx2 = (!!(env->GetCPUFlags() & CPUF_AVX2) && opt < 0) || opt == 2;
#else
    const bool avx2 = false;
#endif
    const bool sse41 = (!!(env->GetCPUFlags() & CPUF_SSE4_1) && opt < 0) || opt == 1;

    if (threads)
    {
        switch (vi.ComponentSize())
        {
            case 1:
#if defined(__AVX512F__)
                if (avx512)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx512<uint8_t, 1, 1> : &JincResize::resize_plane_avx512<uint8_t, 1, 0>;
                else
#endif
#if defined(__AVX2__)
                if (avx2)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx2<uint8_t, 1, 1> : &JincResize::resize_plane_avx2<uint8_t, 1, 0>;
                else
#endif
                if (sse41)
                    process_frame = (subsampled) ? &JincResize::resize_plane_sse41<uint8_t, 1, 1> : &JincResize::resize_eqplanes_sse41<uint8_t, 1, 0>;
                else
                    process_frame = (subsampled) ? &JincResize::resize_plane_c<uint8_t, 1, 1> : &JincResize::resize_eqplanes_c<uint8_t, 1, 0>;
                break;
            case 2:
#if defined(__AVX512F__)
                if (avx512)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx512<uint16_t, 1, 1> : &JincResize::resize_plane_avx512<uint16_t, 1, 0>;
                else
#endif
#if defined(__AVX2__)
                if (avx2)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx2<uint16_t, 1, 1> : &JincResize::resize_plane_avx2<uint16_t, 1, 0>;
                else
#endif
                if (sse41)
                    process_frame = (subsampled) ? &JincResize::resize_plane_sse41<uint16_t, 1, 1> : &JincResize::resize_eqplanes_sse41<uint16_t, 1, 0>;
                else
                    process_frame = (subsampled) ? &JincResize::resize_plane_c<uint16_t, 1, 1> : &JincResize::resize_eqplanes_c<uint16_t, 1, 0>;
                break;
            default:
#if defined(__AVX512F__)
                if (avx512)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx512<float, 1, 1> : &JincResize::resize_plane_avx512<float, 1, 0>;
                else
#endif
#if defined(__AVX2__)
                if (avx2)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx2<float, 1, 1> : &JincResize::resize_plane_avx2<float, 1, 0>;
                else
#endif
                if (sse41)
                    process_frame = (subsampled) ? &JincResize::resize_plane_sse41<float, 1, 1> : &JincResize::resize_eqplanes_sse41<float, 1, 0>;
                else
                    process_frame = (subsampled) ? &JincResize::resize_plane_c<float, 1, 1> : &JincResize::resize_eqplanes_c<float, 1, 0>;
                break;
        }
    }
    else
    {
        switch (vi.ComponentSize())
        {
            case 1:
#if defined(__AVX512F__)
                if (avx512)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx512<uint8_t, 0, 1> : &JincResize::resize_plane_avx512<uint8_t, 0, 0>;
                else
#endif
#if defined(__AVX2__)
                if (avx2)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx2<uint8_t, 0, 1> : &JincResize::resize_plane_avx2<uint8_t, 0, 0>;
                else
#endif
                if (sse41)
                    process_frame = (subsampled) ? &JincResize::resize_plane_sse41<uint8_t, 0, 1> : &JincResize::resize_eqplanes_sse41<uint8_t, 0, 0>;
                else
                    process_frame = (subsampled) ? &JincResize::resize_plane_c<uint8_t, 0, 1> : &JincResize::resize_eqplanes_c<uint8_t, 0, 0>;
                break;
            case 2:
#if defined(__AVX512F__)
                if (avx512)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx512<uint16_t, 0, 1> : &JincResize::resize_plane_avx512<uint16_t, 0, 0>;
                else
#endif
#if defined(__AVX2__)
                if (avx2)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx2<uint16_t, 0, 1> : &JincResize::resize_plane_avx2<uint16_t, 0, 0>;
                else
#endif
                if (sse41)
                    process_frame = (subsampled) ? &JincResize::resize_plane_sse41<uint16_t, 0, 1> : &JincResize::resize_eqplanes_sse41<uint16_t, 0, 0>;
                else
                    process_frame = (subsampled) ? &JincResize::resize_plane_c<uint16_t, 0, 1> : &JincResize::resize_eqplanes_c<uint16_t, 0, 0>;
                break;
            default:
#if defined(__AVX512F__)
                if (avx512)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx512<float, 0, 1> : &JincResize::resize_plane_avx512<float, 0, 0>;
                else
#endif
#if defined(__AVX2__)
                if (avx2)
                    process_frame = (subsampled) ? &JincResize::resize_plane_avx2<float, 0, 1> : &JincResize::resize_plane_avx2<float, 0, 0>;
                else
#endif
                if (sse41)
                    process_frame = (subsampled) ? &JincResize::resize_plane_sse41<float, 0, 1> : &JincResize::resize_eqplanes_sse41<float, 0, 0>;
                else
                    process_frame = (subsampled) ? &JincResize::resize_plane_c<float, 0, 1> : &JincResize::resize_eqplanes_c<float, 0, 0>;
                break;
        }
    }
}

JincResize::~JincResize()
{
    for (int i = 0; i < static_cast<int>(out1.size()); ++i)
    {
        delete_coeff_table(out1[i]);
        delete out1[i];
    }

    delete[] init_lut->lut;
    delete init_lut;
}

PVideoFrame JincResize::GetFrame(int n, IScriptEnvironment* env)
{
    PVideoFrame src = child->GetFrame(n, env);
    PVideoFrame dst = env->NewVideoFrameP(vi, &src);

    (this->*process_frame)(src, dst, env);

    if (vi.Is420() || vi.Is422() || vi.IsYV411())
    {
        if (cplace == "mpeg2")
            env->propSetInt(env->getFramePropsRW(dst), "_ChromaLocation", 0, 0);
        else if (cplace == "mpeg1")
            env->propSetInt(env->getFramePropsRW(dst), "_ChromaLocation", 1, 0);
        else
            env->propSetInt(env->getFramePropsRW(dst), "_ChromaLocation", 2, 0);
    }

    return dst;
}

AVSValue __cdecl Create_JincResize(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    const VideoInfo& vi = args[0].AsClip()->GetVideoInfo();

    return new JincResize(
        args[0].AsClip(),
        args[1].AsInt(),
        args[2].AsInt(),
        args[3].AsFloatf(0.0f),
        args[4].AsFloatf(0.0f),
        args[5].AsFloatf(static_cast<float>(vi.width)),
        args[6].AsFloatf(static_cast<float>(vi.height)),
        args[7].AsInt(256),
        args[8].AsInt(256),
        args[9].AsInt(3),
        args[10].AsFloatf(0.0f),
        args[11].AsString(""),
        args[12].AsInt(0),
        args[13].AsInt(-1),
        args[14].AsInt(vi.width*vi.height),
        args[15].AsFloatf(1.5f),
        env);
}

static void resizer(const AVSValue& args, Arguments* out_args, int src_left_idx = 3)
{
    out_args->add(args[0]);
    out_args->add(args[1]);
    out_args->add(args[2]);

    if (args[src_left_idx + 0].Defined())
        out_args->add(args[src_left_idx + 0], "src_left");
    if (args[src_left_idx + 1].Defined())
        out_args->add(args[src_left_idx + 1], "src_top");
    if (args[src_left_idx + 2].Defined())
        out_args->add(args[src_left_idx + 2], "src_width");
    if (args[src_left_idx + 3].Defined())
        out_args->add(args[src_left_idx + 3], "src_height");
    if (args[src_left_idx + 4].Defined())
        out_args->add(args[src_left_idx + 4], "quant_x");
    if (args[src_left_idx + 5].Defined())
        out_args->add(args[src_left_idx + 5], "quant_y");
    if (args[src_left_idx + 6].Defined())
        out_args->add(args[src_left_idx + 6], "cplace");
    if (args[src_left_idx + 7].Defined())
        out_args->add(args[src_left_idx + 7], "threads");
}

template <int taps>
AVSValue __cdecl resizer_jincresize(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    Arguments mapped_args;

    resizer(args, &mapped_args);
    mapped_args.add(taps, "tap");

    return env->Invoke("JincResize", mapped_args.args(), mapped_args.arg_names()).AsClip();
}

/*const AVS_Linkage* AVS_linkage;

extern "C" __declspec(dllexport)
const char* __stdcall AvisynthPluginInit3(IScriptEnvironment * env, const AVS_Linkage* const vectors)
{
    AVS_linkage = vectors;

    env->AddFunction("JincResize", "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[tap]i[blur]f[cplace]s[threads]i[opt]i[initial_capacity]i[initial_factor]f", Create_JincResize, 0);

    env->AddFunction("Jinc36Resize", "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<3>, 0);
    env->AddFunction("Jinc64Resize", "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<4>, 0);
    env->AddFunction("Jinc144Resize", "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<6>, 0);
    env->AddFunction("Jinc256Resize", "cii[src_left]f[src_top]f[src_width]f[src_height]f[quant_x]i[quant_y]i[cplace]s[threads]i", resizer_jincresize<8>, 0);

    return "JincResize";
}*/
