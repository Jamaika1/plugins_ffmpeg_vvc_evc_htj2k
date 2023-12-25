#include <stddef.h>
#include "opencl_source.h"

const char *ff_source_avgblur_cl = "__kernel void avgblur_horiz(__write_only image2d_t dst,\
                            __read_only  image2d_t src,\
                            int rad)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_FILTER_NEAREST);\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    int2 size = (int2)(get_global_size(0), get_global_size(1));\
\
    int count = 0;\
    float4 acc = (float4)(0,0,0,0);\
\
    for (int xx = max(0, loc.x - rad); xx < min(loc.x + rad + 1, size.x); xx++) {\
        count++;\
        acc += read_imagef(src, sampler, (int2)(xx, loc.y));\
    }\
\
    write_imagef(dst, loc, acc / count);\
}\
\
__kernel void avgblur_vert(__write_only image2d_t dst,\
                           __read_only  image2d_t src,\
                           int radv)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_FILTER_NEAREST);\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    int2 size = (int2)(get_global_size(0), get_global_size(1));\
\
    int count = 0;\
    float4 acc = (float4)(0,0,0,0);\
\
    for (int yy = max(0, loc.y - radv); yy < min(loc.y + radv + 1, size.y); yy++) {\
        count++;\
        acc += read_imagef(src, sampler, (int2)(loc.x, yy));\
    }\
\
    write_imagef(dst, loc, acc / count);\
}";

const char *ff_source_colorkey_cl = "const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |\
                          CLK_FILTER_NEAREST;\
\
__kernel void colorkey_blend(\
    __read_only  image2d_t src,\
    __write_only image2d_t dst,\
    float4 colorkey_rgba,\
    float similarity,\
    float blend\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    float4 pixel = read_imagef(src, sampler, loc);\
    float diff = distance(pixel.xyz, colorkey_rgba.xyz);\
\
    pixel.s3 = clamp((diff - similarity) / blend, 0.0f, 1.0f);\
    write_imagef(dst, loc, pixel);\
}\
\
__kernel void colorkey(\
    __read_only  image2d_t src,\
    __write_only image2d_t dst,\
    float4 colorkey_rgba,\
    float similarity\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    float4 pixel = read_imagef(src, sampler, loc);\
    float diff = distance(pixel.xyz, colorkey_rgba.xyz);\
\
    pixel.s3 = (diff > similarity) ? 1.0f : 0.0f;\
    write_imagef(dst, loc, pixel);\
}";

const char *ff_source_colorspace_common_cl = "#define ST2084_MAX_LUMINANCE 10000.0f \
#define REFERENCE_WHITE 100.0f \
\
#if chroma_loc == 1 \
    #define chroma_sample(a,b,c,d) (((a) + (c)) * 0.5f) \
#elif chroma_loc == 3 \
    #define chroma_sample(a,b,c,d) (a) \
#elif chroma_loc == 4 \
    #define chroma_sample(a,b,c,d) (((a) + (b)) * 0.5f) \
#elif chroma_loc == 5 \
    #define chroma_sample(a,b,c,d) (c) \
#elif chroma_loc == 6 \
    #define chroma_sample(a,b,c,d) (((c) + (d)) * 0.5f) \
#else \
    #define chroma_sample(a,b,c,d) (((a) + (b) + (c) + (d)) * 0.25f) \
#endif \
\
constant const float ST2084_M1 = 0.1593017578125f;\
constant const float ST2084_M2 = 78.84375f;\
constant const float ST2084_C1 = 0.8359375f;\
constant const float ST2084_C2 = 18.8515625f;\
constant const float ST2084_C3 = 18.6875f;\
\
float get_luma_dst(float3 c) {\
    return luma_dst.x * c.x + luma_dst.y * c.y + luma_dst.z * c.z;\
}\
\
float get_luma_src(float3 c) {\
    return luma_src.x * c.x + luma_src.y * c.y + luma_src.z * c.z;\
}\
\
float3 get_chroma_sample(float3 a, float3 b, float3 c, float3 d) {\
    return chroma_sample(a, b, c, d);\
}\
\
float eotf_st2084(float x) {\
    float p = powr(x, 1.0f / ST2084_M2);\
    float a = max(p -ST2084_C1, 0.0f);\
    float b = max(ST2084_C2 - ST2084_C3 * p, 1e-6f);\
    float c  = powr(a / b, 1.0f / ST2084_M1);\
    return x > 0.0f ? c * ST2084_MAX_LUMINANCE / REFERENCE_WHITE : 0.0f;\
}\
\
__constant const float HLG_A = 0.17883277f;\
__constant const float HLG_B = 0.28466892f;\
__constant const float HLG_C = 0.55991073f;\
\
float inverse_oetf_hlg(float x) {\
    float a = 4.0f * x * x;\
    float b = exp((x - HLG_C) / HLG_A) + HLG_B;\
    return x < 0.5f ? a : b;\
}\
\
float oetf_hlg(float x) {\
    float a = 0.5f * sqrt(x);\
    float b = HLG_A * log(x - HLG_B) + HLG_C;\
    return x <= 1.0f ? a : b;\
}\
\
float3 ootf_hlg(float3 c, float peak) {\
    float luma = get_luma_src(c);\
    float gamma =  1.2f + 0.42f * log10(peak * REFERENCE_WHITE / 1000.0f);\
    gamma = max(1.0f, gamma);\
    float factor = peak * powr(luma, gamma - 1.0f) / powr(12.0f, gamma);\
    return c * factor;\
}\
\
float3 inverse_ootf_hlg(float3 c, float peak) {\
    float gamma = 1.2f + 0.42f * log10(peak * REFERENCE_WHITE / 1000.0f);\
    c *=  powr(12.0f, gamma) / peak;\
    c /= powr(get_luma_dst(c), (gamma - 1.0f) / gamma);\
    return c;\
}\
\
float inverse_eotf_bt1886(float c) {\
    return c < 0.0f ? 0.0f : powr(c, 1.0f / 2.4f);\
}\
\
float oetf_bt709(float c) {\
    c = c < 0.0f ? 0.0f : c;\
    float r1 = 4.5f * c;\
    float r2 = 1.099f * powr(c, 0.45f) - 0.099f;\
    return c < 0.018f ? r1 : r2;\
}\
float inverse_oetf_bt709(float c) {\
    float r1 = c / 4.5f;\
    float r2 = powr((c + 0.099f) / 1.099f, 1.0f / 0.45f);\
    return c < 0.081f ? r1 : r2;\
}\
\
float3 yuv2rgb(float y, float u, float v) {\
#ifdef FULL_RANGE_IN \
    u -= 0.5f; v -= 0.5f;\
#else \
    y = (y * 255.0f -  16.0f) / 219.0f;\
    u = (u * 255.0f - 128.0f) / 224.0f;\
    v = (v * 255.0f - 128.0f) / 224.0f;\
#endif \
    float r = y * rgb_matrix[0] + u * rgb_matrix[1] + v * rgb_matrix[2];\
    float g = y * rgb_matrix[3] + u * rgb_matrix[4] + v * rgb_matrix[5];\
    float b = y * rgb_matrix[6] + u * rgb_matrix[7] + v * rgb_matrix[8];\
    return (float3)(r, g, b);\
}\
\
float3 yuv2lrgb(float3 yuv) {\
    float3 rgb = yuv2rgb(yuv.x, yuv.y, yuv.z);\
#ifdef linearize \
    float r = linearize(rgb.x);\
    float g = linearize(rgb.y);\
    float b = linearize(rgb.z);\
    return (float3)(r, g, b);\
#else \
    return rgb;\
#endif \
}\
\
float3 rgb2yuv(float r, float g, float b) {\
    float y = r*yuv_matrix[0] + g*yuv_matrix[1] + b*yuv_matrix[2];\
    float u = r*yuv_matrix[3] + g*yuv_matrix[4] + b*yuv_matrix[5];\
    float v = r*yuv_matrix[6] + g*yuv_matrix[7] + b*yuv_matrix[8];\
#ifdef FULL_RANGE_OUT \
    u += 0.5f; v += 0.5f;\
#else \
    y = (219.0f * y + 16.0f) / 255.0f;\
    u = (224.0f * u + 128.0f) / 255.0f;\
    v = (224.0f * v + 128.0f) / 255.0f;\
#endif \
    return (float3)(y, u, v);\
}\
\
float rgb2y(float r, float g, float b) {\
    float y = r*yuv_matrix[0] + g*yuv_matrix[1] + b*yuv_matrix[2];\
    y = (219.0f * y + 16.0f) / 255.0f;\
    return y;\
}\
\
float3 lrgb2yuv(float3 c) {\
#ifdef delinearize \
    float r = delinearize(c.x);\
    float g = delinearize(c.y);\
    float b = delinearize(c.z);\
    return rgb2yuv(r, g, b);\
#else \
    return rgb2yuv(c.x, c.y, c.z);\
#endif \
}\
\
float lrgb2y(float3 c) {\
#ifdef delinearize \
    float r = delinearize(c.x);\
    float g = delinearize(c.y);\
    float b = delinearize(c.z);\
    return rgb2y(r, g, b);\
#else \
    return rgb2y(c.x, c.y, c.z);\
#endif \
}\
\
float3 lrgb2lrgb(float3 c) {\
#ifdef RGB2RGB_PASSTHROUGH \
    return c;\
#else \
    float r = c.x, g = c.y, b = c.z;\
    float rr = rgb2rgb[0] * r + rgb2rgb[1] * g + rgb2rgb[2] * b;\
    float gg = rgb2rgb[3] * r + rgb2rgb[4] * g + rgb2rgb[5] * b;\
    float bb = rgb2rgb[6] * r + rgb2rgb[7] * g + rgb2rgb[8] * b;\
    return (float3)(rr, gg, bb);\
#endif \
}\
\
float3 ootf(float3 c, float peak) {\
#ifdef ootf_impl \
    return ootf_impl(c, peak);\
#else \
    return c;\
#endif \
}\
\
float3 inverse_ootf(float3 c, float peak) {\
#ifdef inverse_ootf_impl \
    return inverse_ootf_impl(c, peak);\
#else \
    return c;\
#endif \
}";

const char *ff_source_convolution_cl = "__kernel void convolution_global(__write_only image2d_t dst,\
                                 __read_only  image2d_t src,\
                                 int coef_matrix_dim,\
                                 __constant float *coef_matrix,\
                                 float div,\
                                 float bias)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
\
    const int half_matrix_dim = (coef_matrix_dim / 2);\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    float4 convPix = (float4)(0.0f, 0.0f, 0.0f, 0.0f);\
\
    for (int conv_i = -half_matrix_dim; conv_i <= half_matrix_dim; conv_i++) {\
        for (int conv_j = -half_matrix_dim; conv_j <= half_matrix_dim; conv_j++) {\
            float4 px = read_imagef(src, sampler, loc + (int2)(conv_j, conv_i));\
            convPix += px * coef_matrix[(conv_i + half_matrix_dim) * coef_matrix_dim +\
                                        (conv_j + half_matrix_dim)];\
        }\
     }\
     float4 dstPix = convPix * div + bias;\
     write_imagef(dst, loc, dstPix);\
}\
\
\
__kernel void sobel_global(__write_only image2d_t dst,\
                           __read_only  image2d_t src,\
                             float div,\
                             float bias)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 sum1 = read_imagef(src, sampler, loc + (int2)(-1,-1)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)( 0,-1)) * -2 +\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)( 0, 1)) *  2 +\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) *  1;\
\
    float4 sum2 = read_imagef(src, sampler, loc + (int2)(-1,-1)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)(-1, 0)) * -2 +\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)( 1, 0)) *  2 +\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) *  1;\
\
    float4 dstPix = hypot(sum1, sum2) * div + bias;\
    write_imagef(dst, loc, dstPix);\
}\
\
__kernel void prewitt_global(__write_only image2d_t dst,\
                             __read_only  image2d_t src,\
                             float div,\
                             float bias)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 sum1 = read_imagef(src, sampler, loc + (int2)(-1,-1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)( 0,-1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)( 0, 1)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) * -1;\
\
    float4 sum2 = read_imagef(src, sampler, loc + (int2)(-1,-1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)(-1, 0)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)( 1, 0)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) * -1;\
\
    float4 dstPix = hypot(sum1, sum2) * div + bias;\
    write_imagef(dst, loc, dstPix);\
}\
\
__kernel void roberts_global(__write_only image2d_t dst,\
                             __read_only  image2d_t src,\
                             float div,\
                             float bias)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 sum1 = read_imagef(src, sampler, loc + (int2)(-1,-1)) *  1 +\
                  read_imagef(src, sampler, loc + (int2)( 0,-1)) * -1;\
\
\
    float4 sum2 = read_imagef(src, sampler, loc + (int2)(-1, 0)) * -1 +\
                  read_imagef(src, sampler, loc + (int2)( 0, 0)) *  1;\
\
\
    float4 dstPix = hypot(sum1, sum2) * div + bias;\
    write_imagef(dst, loc, dstPix);\
}";

const char *ff_source_deshake_cl = "#define HARRIS_THRESHOLD 3.0f \
#define HARRIS_RADIUS 2 \
#define DISTANCE_THRESHOLD 80 \
\
#define REFINE_WIN_HALF_W 5 \
#define REFINE_WIN_HALF_H 5 \
#define REFINE_WIN_W 11 \
#define REFINE_WIN_H 11 \
\
#define NONMAX_WIN 30\
#define NONMAX_WIN_HALF 15 \
\
typedef struct PointPair {\
    float2 p1;\
    float2 p2;\
} PointPair;\
\
typedef struct SmoothedPointPair {\
    int2 p1;\
    float2 p2;\
} SmoothedPointPair;\
\
typedef struct MotionVector {\
    PointPair p;\
    int should_consider;\
} MotionVector;\
\
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |\
                          CLK_ADDRESS_CLAMP_TO_EDGE |\
                          CLK_FILTER_NEAREST;\
\
const sampler_t sampler_linear = CLK_NORMALIZED_COORDS_FALSE |\
                          CLK_ADDRESS_CLAMP_TO_EDGE |\
                          CLK_FILTER_LINEAR;\
\
const sampler_t sampler_linear_mirror = CLK_NORMALIZED_COORDS_TRUE |\
                          CLK_ADDRESS_MIRRORED_REPEAT |\
                          CLK_FILTER_LINEAR;\
\
static void write_to_1d_arrf(__global float *buf, int2 loc, float val) {\
    buf[loc.x + loc.y * get_global_size(0)] = val;\
}\
\
static void write_to_1d_arrul8(__global ulong8 *buf, int2 loc, ulong8 val) {\
    buf[loc.x + loc.y * get_global_size(0)] = val;\
}\
\
static void write_to_1d_arrvec(__global MotionVector *buf, int2 loc, MotionVector val) {\
    buf[loc.x + loc.y * get_global_size(0)] = val;\
}\
\
static void write_to_1d_arrf2(__global float2 *buf, int2 loc, float2 val) {\
    buf[loc.x + loc.y * get_global_size(0)] = val;\
}\
\
static ulong8 read_from_1d_arrul8(__global const ulong8 *buf, int2 loc) {\
    return buf[loc.x + loc.y * get_global_size(0)];\
}\
\
static float2 read_from_1d_arrf2(__global const float2 *buf, int2 loc) {\
    return buf[loc.x + loc.y * get_global_size(0)];\
}\
\
static float pixel_grayscale(__read_only image2d_t src, int2 loc) {\
    float4 pixel = read_imagef(src, sampler, loc);\
    return (pixel.x + pixel.y + pixel.z) / 3.0f;\
}\
\
static float convolve(\
    __local const float *grayscale,\
    int local_idx_x,\
    int local_idx_y,\
    float mask[3][3]\
) {\
    float ret = 0;\
\
    for (int i = 1, i2 = 0; i >= -1; --i, ++i2) {\
        for (int j = -1, j2 = 0; j <= 1; ++j, ++j2) {\
            ret += mask[i2][j2] * grayscale[(local_idx_x + 3 + j) + (local_idx_y + 3 + i) * 14];\
        }\
    }\
\
    return ret;\
}\
\
static float sum_deriv_prod(\
    __local const float *grayscale,\
    float mask_x[3][3],\
    float mask_y[3][3]\
) {\
    float ret = 0;\
\
    for (int i = HARRIS_RADIUS; i >= -HARRIS_RADIUS; --i) {\
        for (int j = -HARRIS_RADIUS; j <= HARRIS_RADIUS; ++j) {\
            ret += convolve(grayscale, get_local_id(0) + j, get_local_id(1) + i, mask_x) *\
                   convolve(grayscale, get_local_id(0) + j, get_local_id(1) + i, mask_y);\
        }\
    }\
\
    return ret;\
}\
\
static float sum_deriv_pow(__local const float *grayscale, float mask[3][3])\
{\
    float ret = 0;\
\
    for (int i = HARRIS_RADIUS; i >= -HARRIS_RADIUS; --i) {\
        for (int j = -HARRIS_RADIUS; j <= HARRIS_RADIUS; ++j) {\
            float deriv = convolve(grayscale, get_local_id(0) + j, get_local_id(1) + i, mask);\
            ret += deriv * deriv;\
        }\
    }\
\
    return ret;\
}\
\
static void draw_box(__write_only image2d_t dst, int2 loc, float4 pixel, int radius)\
{\
    for (int i = -radius; i <= radius; ++i) {\
        for (int j = -radius; j <= radius; ++j) {\
            write_imagef(\
                dst,\
                (int2)(\
                    clamp(loc.x + i, 0, get_image_dim(dst).x - 1),\
                    clamp(loc.y + j, 0, get_image_dim(dst).y - 1)\
                ),\
                pixel\
            );\
        }\
    }\
}\
\
__kernel void grayscale(\
    __read_only image2d_t src,\
    __write_only image2d_t grayscale\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    write_imagef(grayscale, loc, (float4)(pixel_grayscale(src, loc), 0.0f, 0.0f, 1.0f));\
}\
\
__kernel void harris_response(\
    __read_only image2d_t grayscale,\
    __global float *harris_buf\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    if (loc.x > get_image_width(grayscale) - 1 || loc.y > get_image_height(grayscale) - 1) {\
        write_to_1d_arrf(harris_buf, loc, 0);\
        return;\
    }\
\
    float scale = 1.0f / ((1 << 2) * HARRIS_RADIUS * 255.0f);\
\
    float sobel_mask_x[3][3] = {\
        {-1, 0, 1},\
        {-2, 0, 2},\
        {-1, 0, 1}\
    };\
\
    float sobel_mask_y[3][3] = {\
        { 1,  2,  1},\
        { 0,  0,  0},\
        {-1, -2, -1}\
    };\
\
    __local float grayscale_data[196];\
\
    int idx = get_group_id(0) * get_local_size(0);\
    int idy = get_group_id(1) * get_local_size(1);\
\
    for (int i = idy - 3, it = 0; i < idy + (int)get_local_size(1) + 3; i++, it++) {\
        for (int j = idx - 3, jt = 0; j < idx + (int)get_local_size(0) + 3; j++, jt++) {\
            grayscale_data[jt + it * 14] = read_imagef(grayscale, sampler, (int2)(j, i)).x;\
        }\
    }\
\
    barrier(CLK_LOCAL_MEM_FENCE);\
\
    float sumdxdy = sum_deriv_prod(grayscale_data, sobel_mask_x, sobel_mask_y);\
    float sumdx2 = sum_deriv_pow(grayscale_data, sobel_mask_x);\
    float sumdy2 = sum_deriv_pow(grayscale_data, sobel_mask_y);\
\
    float trace = sumdx2 + sumdy2;\
    float r = (sumdx2 * sumdy2 - sumdxdy * sumdxdy) - 0.04f * (trace * trace) * pown(scale, 4);\
\
    harris_buf[loc.x + loc.y * get_image_width(grayscale)] = r * step(HARRIS_THRESHOLD, r);\
}\
\
static void get_rect_sub_pix(\
    __read_only image2d_t grayscale,\
    float *buffer,\
    int size_x,\
    int size_y,\
    float2 center\
) {\
    float2 offset = ((float2)(size_x, size_y) - 1.0f) * 0.5f;\
\
    for (int i = 0; i < size_y; i++) {\
        for (int j = 0; j < size_x; j++) {\
            buffer[i * size_x + j] = read_imagef(\
                grayscale,\
                sampler_linear,\
                (float2)(j, i) + center - offset\
            ).x * 255.0f;\
        }\
    }\
}\
\
static float2 corner_sub_pix(\
    __read_only image2d_t grayscale,\
    float2 feature,\
    float *mask\
) {\
    float2 init = feature;\
    int src_width = get_global_size(0);\
    int src_height = get_global_size(1);\
\
    const int max_iters = 40;\
    const float eps = 0.001f * 0.001f;\
    int i, j, k;\
\
    int iter = 0;\
    float err = 0;\
    float subpix[(REFINE_WIN_W + 2) * (REFINE_WIN_H + 2)];\
    const float flt_epsilon = 0x1.0p-23f;\
\
    do {\
        float2 feature_tmp;\
        float a = 0, b = 0, c = 0, bb1 = 0, bb2 = 0;\
\
        get_rect_sub_pix(grayscale, subpix, REFINE_WIN_W + 2, REFINE_WIN_H + 2, feature);\
        float *subpix_ptr = subpix;\
        subpix_ptr += REFINE_WIN_W + 2 + 1;\
\
        for (i = 0, k = 0; i < REFINE_WIN_H; i++, subpix_ptr += REFINE_WIN_W + 2) {\
            float py = i - REFINE_WIN_HALF_H;\
\
            for (j = 0; j < REFINE_WIN_W; j++, k++) {\
                float m = mask[k];\
                float tgx = subpix_ptr[j + 1] - subpix_ptr[j - 1];\
                float tgy = subpix_ptr[j + REFINE_WIN_W + 2] - subpix_ptr[j - REFINE_WIN_W - 2];\
                float gxx = tgx * tgx * m;\
                float gxy = tgx * tgy * m;\
                float gyy = tgy * tgy * m;\
                float px = j - REFINE_WIN_HALF_W;\
\
                a += gxx;\
                b += gxy;\
                c += gyy;\
\
                bb1 += gxx * px + gxy * py;\
                bb2 += gxy * px + gyy * py;\
            }\
        }\
\
        float det = a * c - b * b;\
        if (fabs(det) <= flt_epsilon * flt_epsilon) {\
            break;\
        }\
\
        float scale = 1.0f / det;\
        feature_tmp.x = (float)(feature.x + (c * scale * bb1) - (b * scale * bb2));\
        feature_tmp.y = (float)(feature.y - (b * scale * bb1) + (a * scale * bb2));\
        err = dot(feature_tmp - feature, feature_tmp - feature);\
\
        feature = feature_tmp;\
        if (feature.x < 0 || feature.x >= src_width || feature.y < 0 || feature.y >= src_height) {\
            break;\
        }\
    } while (++iter < max_iters && err > eps);\
\
    if (fabs(feature.x - init.x) > REFINE_WIN_HALF_W || fabs(feature.y - init.y) > REFINE_WIN_HALF_H) {\
        feature = init;\
    }\
\
    return feature;\
}\
\
__kernel void refine_features(\
    __read_only image2d_t grayscale,\
    __global const float *harris_buf,\
    __global float2 *refined_features,\
    int subpixel_refine\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    int2 loc_i = (int2)(loc.x * 32, loc.y * 32);\
\
    float new_val;\
    float max_val = 0;\
    float2 loc_max = (float2)(-1, -1);\
\
    int end_x = min(loc_i.x + 32, (int)get_image_dim(grayscale).x - 1);\
    int end_y = min(loc_i.y + 32, (int)get_image_dim(grayscale).y - 1);\
\
    for (int i = loc_i.x; i < end_x; ++i) {\
        for (int j = loc_i.y; j < end_y; ++j) {\
            new_val = harris_buf[i + j * get_image_dim(grayscale).x];\
\
            if (new_val > max_val) {\
                max_val = new_val;\
                loc_max = (float2)(i, j);\
            }\
        }\
    }\
\
    if (max_val == 0) {\
        write_to_1d_arrf2(refined_features, loc, loc_max);\
        return;\
    }\
\
    if (subpixel_refine) {\
        float mask[REFINE_WIN_H * REFINE_WIN_W];\
        for (int i = 0; i < REFINE_WIN_H; i++) {\
            float y = (float)(i - REFINE_WIN_HALF_H) / REFINE_WIN_HALF_H;\
            float vy = exp(-y * y);\
\
            for (int j = 0; j < REFINE_WIN_W; j++) {\
                float x = (float)(j - REFINE_WIN_HALF_W) / REFINE_WIN_HALF_W;\
                mask[i * REFINE_WIN_W + j] = (float)(vy * exp(-x * x));\
            }\
        }\
\
        loc_max = corner_sub_pix(grayscale, loc_max, mask);\
    }\
\
    write_to_1d_arrf2(refined_features, loc, loc_max);\
}\
\
__kernel void brief_descriptors(\
    __read_only image2d_t grayscale,\
    __global const float2 *refined_features,\
    __global ulong8 *desc_buf,\
    __global const PointPair *brief_pattern\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    float2 feature = read_from_1d_arrf2(refined_features, loc);\
\
    if (feature.x == -1) {\
        write_to_1d_arrul8(desc_buf, loc, (ulong8)(0));\
        return;\
    }\
\
    ulong8 desc = 0;\
    ulong *p = &desc;\
\
    for (int i = 0; i < 8; ++i) {\
        for (int j = 0; j < 64; ++j) {\
            PointPair pair = brief_pattern[j * (i + 1)];\
            float l1 = read_imagef(grayscale, sampler_linear, feature + pair.p1).x;\
            float l2 = read_imagef(grayscale, sampler_linear, feature + pair.p2).x;\
\
            if (l1 < l2) {\
                p[i] |= 1UL << j;\
            }\
        }\
    }\
\
    write_to_1d_arrul8(desc_buf, loc, desc);\
}\
\
__kernel void match_descriptors(\
    __global const float2 *prev_refined_features,\
    __global const float2 *refined_features,\
    __global const ulong8 *desc_buf,\
    __global const ulong8 *prev_desc_buf,\
    __global MotionVector *matches_buf\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    ulong8 desc = read_from_1d_arrul8(desc_buf, loc);\
    const int search_radius = 3;\
\
    MotionVector invalid_vector = (MotionVector) {\
        (PointPair) {\
            (float2)(-1, -1),\
            (float2)(-1, -1)\
        },\
        0\
    };\
\
    if (desc.s0 == 0 && desc.s1 == 0) {\
        write_to_1d_arrvec(\
            matches_buf,\
            loc,\
            invalid_vector\
        );\
        return;\
    }\
\
    int2 start = max(loc - search_radius, 0);\
    int2 end = min(loc + search_radius, (int2)(get_global_size(0) - 1, get_global_size(1) - 1));\
\
    for (int i = start.x; i < end.x; ++i) {\
        for (int j = start.y; j < end.y; ++j) {\
            int2 prev_point = (int2)(i, j);\
            int total_dist = 0;\
\
            ulong8 prev_desc = read_from_1d_arrul8(prev_desc_buf, prev_point);\
\
            if (prev_desc.s0 == 0 && prev_desc.s1 == 0) {\
                continue;\
            }\
\
            ulong *prev_desc_p = &prev_desc;\
            ulong *desc_p = &desc;\
\
            for (int i = 0; i < 8; i++) {\
                total_dist += popcount(desc_p[i] ^ prev_desc_p[i]);\
            }\
\
            if (total_dist < DISTANCE_THRESHOLD) {\
                write_to_1d_arrvec(\
                    matches_buf,\
                    loc,\
                    (MotionVector) {\
                        (PointPair) {\
                            read_from_1d_arrf2(prev_refined_features, prev_point),\
                            read_from_1d_arrf2(refined_features, loc)\
                        },\
                        1\
                    }\
                );\
\
                return;\
            }\
        }\
    }\
\
    write_to_1d_arrvec(\
        matches_buf,\
        loc,\
        invalid_vector\
    );\
}\
\
static float2 transformed_point(float2 p, __global const float *transform) {\
    float2 ret;\
\
    ret.x = p.x * transform[0] + p.y * transform[1] + transform[2];\
    ret.y = p.x * transform[3] + p.y * transform[4] + transform[5];\
\
    return ret;\
}\
\
__kernel void transform(\
    __read_only image2d_t src,\
    __write_only image2d_t dst,\
    __global const float *transform\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    float2 norm = convert_float2(get_image_dim(src));\
\
    write_imagef(\
        dst,\
        loc,\
        read_imagef(\
            src,\
            sampler_linear_mirror,\
            transformed_point((float2)(loc.x, loc.y), transform) / norm\
        )\
    );\
}\
\
static float2 cropped_point(\
    float2 p,\
    float2 top_left,\
    float2 bottom_right,\
    int2 orig_dim\
) {\
    float2 ret;\
\
    float crop_width  = bottom_right.x - top_left.x;\
    float crop_height = bottom_right.y - top_left.y;\
\
    float width_norm = p.x / (float)orig_dim.x;\
    float height_norm = p.y / (float)orig_dim.y;\
\
    ret.x = (width_norm * crop_width) + top_left.x;\
    ret.y = (height_norm * crop_height) + ((float)orig_dim.y - bottom_right.y);\
\
    return ret;\
}\
\
__kernel void crop_upscale(\
    __read_only image2d_t src,\
    __write_only image2d_t dst,\
    float2 top_left,\
    float2 bottom_right\
) {\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    write_imagef(\
        dst,\
        loc,\
        read_imagef(\
            src,\
            sampler_linear,\
            cropped_point((float2)(loc.x, loc.y), top_left, bottom_right, get_image_dim(dst))\
        )\
    );\
}\
\
__kernel void draw_debug_info(\
    __write_only image2d_t dst,\
    __global const MotionVector *matches,\
    __global const MotionVector *model_matches,\
    int num_model_matches,\
    __global const float *transform\
) {\
    int loc = get_global_id(0);\
    MotionVector vec = matches[loc];\
    float4 big_rect_color = (float4)(0.1f, 0.1f, 0.1f, 1.0f);\
\
    if (vec.should_consider) {\
        big_rect_color = (float4)(0.0f, 1.0f, 0.0f, 1.0f);\
    }\
\
    for (int i = 0; i < num_model_matches; i++) {\
        if (vec.p.p2.x == model_matches[i].p.p2.x && vec.p.p2.y == model_matches[i].p.p2.y) {\
            big_rect_color = (float4)(1.0f, 0.5f, 0.0f, 1.0f);\
        }\
    }\
\
    float2 transformed_p1 = transformed_point(vec.p.p1, transform);\
    float2 transformed_p2 = transformed_point(vec.p.p2, transform);\
\
    draw_box(dst, (int2)(transformed_p2.x, transformed_p2.y), big_rect_color, 5);\
    draw_box(dst, (int2)(transformed_p1.x, transformed_p1.y), (float4)(0.0f, 0.3f, 0.7f, 1.0f), 3);\
}";

const char *ff_source_neighbor_cl = "__kernel void erosion_global(__write_only image2d_t dst,\
                             __read_only  image2d_t src,\
                             float threshold,\
                             __constant int *coord)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 px = read_imagef(src, sampler, loc);\
    float limit = px.x - threshold;\
    if (limit < 0) {\
        limit = 0;\
    }\
\
    for (int i = -1; i <= 1; i++) {\
        for (int j = -1; j <= 1; j++) {\
            if (coord[(j + 1) * 3 + (i + 1)] == 1) {\
                float4 cur = read_imagef(src, sampler, loc + (int2)(i, j));\
                if (cur.x < px.x) {\
                    px = cur;\
                }\
            }\
        }\
    }\
    if (limit > px.x) {\
        px = (float4)(limit);\
    }\
    write_imagef(dst, loc, px);\
}\
\
\
__kernel void dilation_global(__write_only image2d_t dst,\
                              __read_only  image2d_t src,\
                              float threshold,\
                              __constant int *coord)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 px = read_imagef(src, sampler, loc);\
    float limit = px.x + threshold;\
    if (limit > 1) {\
        limit = 1;\
    }\
\
    for (int i = -1; i <= 1; i++) {\
        for (int j = -1; j <= 1; j++) {\
            if (coord[(j + 1) * 3 + (i + 1)] == 1) {\
                float4 cur = read_imagef(src, sampler, loc + (int2)(i, j));\
                if (cur.x > px.x) {\
                    px = cur;\
                }\
            }\
        }\
    }\
    if (limit < px.x) {\
        px = (float4)(limit);\
    }\
    write_imagef(dst, loc, px);\
}";

const char *ff_source_nlmeans_cl = "const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                           CLK_ADDRESS_CLAMP_TO_EDGE   |\
                           CLK_FILTER_NEAREST);\
\
kernel void horiz_sum(__global uint4 *integral_img,\
                      __read_only image2d_t src,\
                      int width,\
                      int height,\
                      int4 dx,\
                      int4 dy)\
{\
\
    int y = get_global_id(0);\
    int work_size = get_global_size(0);\
\
    uint4 sum = (uint4)(0);\
    float4 s2;\
    for (int i = 0; i < width; i++) {\
        float s1 = read_imagef(src, sampler, (int2)(i, y)).x;\
        s2.x = read_imagef(src, sampler, (int2)(i + dx.x, y + dy.x)).x;\
        s2.y = read_imagef(src, sampler, (int2)(i + dx.y, y + dy.y)).x;\
        s2.z = read_imagef(src, sampler, (int2)(i + dx.z, y + dy.z)).x;\
        s2.w = read_imagef(src, sampler, (int2)(i + dx.w, y + dy.w)).x;\
        sum += convert_uint4((s1 - s2) * (s1 - s2) * 255 * 255);\
        integral_img[y * width + i] = sum;\
    }\
}\
\
kernel void vert_sum(__global uint4 *integral_img,\
                     __global int *overflow,\
                     int width,\
                     int height)\
{\
    int x = get_global_id(0);\
    uint4 sum = 0;\
    for (int i = 0; i < height; i++) {\
        if (any((uint4)UINT_MAX - integral_img[i * width + x] < sum))\
            atomic_inc(overflow);\
        integral_img[i * width + x] += sum;\
        sum = integral_img[i * width + x];\
    }\
}\
\
kernel void weight_accum(global float *sum, global float *weight,\
                         global uint4 *integral_img, __read_only image2d_t src,\
                         int width, int height, int p, float h,\
                         int4 dx, int4 dy)\
{\
    int x = get_global_id(0);\
    int y = get_global_id(1);\
    int4 xoff = x + dx;\
    int4 yoff = y + dy;\
    uint4 a = 0, b = 0, c = 0, d = 0;\
    uint4 src_pix = 0;\
\
    int oobb = (x - p) < 0 || (y - p) < 0 || (y + p) >= height || (x + p) >= width;\
\
    src_pix.x = (int)(255 * read_imagef(src, sampler, (int2)(xoff.x, yoff.x)).x);\
    src_pix.y = (int)(255 * read_imagef(src, sampler, (int2)(xoff.y, yoff.y)).x);\
    src_pix.z = (int)(255 * read_imagef(src, sampler, (int2)(xoff.z, yoff.z)).x);\
    src_pix.w = (int)(255 * read_imagef(src, sampler, (int2)(xoff.w, yoff.w)).x);\
    if (!oobb) {\
        a = integral_img[(y - p) * width + x - p];\
        b = integral_img[(y + p) * width + x - p];\
        c = integral_img[(y - p) * width + x + p];\
        d = integral_img[(y + p) * width + x + p];\
    }\
\
    float4 patch_diff = convert_float4(d + a - c - b);\
    float4 w = native_exp(-patch_diff / (h * h));\
    float w_sum = w.x + w.y + w.z + w.w;\
    weight[y * width + x] += w_sum;\
    sum[y * width + x] += dot(w, convert_float4(src_pix));\
}\
\
kernel void average(__write_only image2d_t dst,\
                    __read_only image2d_t src,\
                    global float *sum, global float *weight) {\
    int x = get_global_id(0);\
    int y = get_global_id(1);\
    int2 dim = get_image_dim(dst);\
\
    float w = weight[y * dim.x + x];\
    float s = sum[y * dim.x + x];\
    float src_pix = read_imagef(src, sampler, (int2)(x, y)).x;\
    float r = (s + src_pix * 255) / (1.0f + w) / 255.0f;\
    if (x < dim.x && y < dim.y)\
        write_imagef(dst, (int2)(x, y), (float4)(r, 0.0f, 0.0f, 1.0f));\
}";

const char *ff_source_overlay_cl = "__kernel void overlay_no_alpha(__write_only image2d_t dst,\
                               __read_only  image2d_t main,\
                               __read_only  image2d_t overlay,\
                               int x_position,\
                               int y_position)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_FILTER_NEAREST);\
\
    int2 overlay_size = get_image_dim(overlay);\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    if (loc.x <  x_position ||\
        loc.y <  y_position ||\
        loc.x >= overlay_size.x + x_position ||\
        loc.y >= overlay_size.y + y_position) {\
        float4 val = read_imagef(main, sampler, loc);\
        write_imagef(dst, loc, val);\
    } else {\
        int2 loc_overlay = (int2)(x_position, y_position);\
        float4 val       = read_imagef(overlay, sampler, loc - loc_overlay);\
        write_imagef(dst, loc, val);\
    }\
}\
\
__kernel void overlay_internal_alpha(__write_only image2d_t dst,\
                                     __read_only  image2d_t main,\
                                     __read_only  image2d_t overlay,\
                                     int x_position,\
                                     int y_position)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_FILTER_NEAREST);\
\
    int2 overlay_size = get_image_dim(overlay);\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    if (loc.x <  x_position ||\
        loc.y <  y_position ||\
        loc.x >= overlay_size.x + x_position ||\
        loc.y >= overlay_size.y + y_position) {\
        float4 val = read_imagef(main, sampler, loc);\
        write_imagef(dst, loc, val);\
    } else {\
        int2 loc_overlay  = (int2)(x_position, y_position);\
        float4 in_main    = read_imagef(main,    sampler, loc);\
        float4 in_overlay = read_imagef(overlay, sampler, loc - loc_overlay);\
        float4 val        = in_overlay * in_overlay.w + in_main * (1.0f - in_overlay.w);\
        write_imagef(dst, loc, val);\
    }\
}\
\
__kernel void overlay_external_alpha(__write_only image2d_t dst,\
                                     __read_only  image2d_t main,\
                                     __read_only  image2d_t overlay,\
                                     __read_only  image2d_t alpha,\
                                     int x_position,\
                                     int y_position,\
                                     int alpha_adj_x,\
                                     int alpha_adj_y)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_FILTER_NEAREST);\
\
    int2 overlay_size = get_image_dim(overlay);\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
\
    if (loc.x <  x_position ||\
        loc.y <  y_position ||\
        loc.x >= overlay_size.x + x_position ||\
        loc.y >= overlay_size.y + y_position) {\
        float4 val = read_imagef(main, sampler, loc);\
        write_imagef(dst, loc, val);\
    } else {\
        int2 loc_overlay  = (int2)(x_position, y_position);\
        float4 in_main    = read_imagef(main,    sampler, loc);\
        float4 in_overlay = read_imagef(overlay, sampler, loc - loc_overlay);\
\
        int2 loc_alpha    = (int2)(loc.x * alpha_adj_x,\
                                   loc.y * alpha_adj_y) - loc_overlay;\
        float4 in_alpha   = read_imagef(alpha,   sampler, loc_alpha);\
\
        float4 val = in_overlay * in_alpha.x + in_main * (1.0f - in_alpha.x);\
        write_imagef(dst, loc, val);\
    }\
}";

const char *ff_source_pad_cl = "const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |\
                          CLK_FILTER_NEAREST;\
\
__kernel void pad (\
    __read_only  image2d_t src,\
    __write_only image2d_t dst,\
    float4 color,\
    int2 xy)\
{\
    int2 size_src = get_image_dim(src);\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\
    int2 src_pos = (int2)(get_global_id(0) - xy.x, get_global_id(1) - xy.y);\
    float4 pixel = loc.x >= size_src.x + xy.x ||\
                   loc.y >= size_src.y + xy.y ||\
                   loc.x < xy.x ||\
                   loc.y < xy.y ? color : read_imagef(src, sampler, src_pos);\
    write_imagef(dst, loc, pixel);\
}";

const char *ff_source_remap_cl = "const sampler_t linear_sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                                  CLK_FILTER_LINEAR);\
\
const sampler_t nearest_sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                                   CLK_FILTER_NEAREST);\
\
__kernel void remap_near(__write_only image2d_t dst,\
                         __read_only  image2d_t src,\
                         __read_only  image2d_t xmapi,\
                         __read_only  image2d_t ymapi,\
                         float4 fill_color)\
{\
    int2 p = (int2)(get_global_id(0), get_global_id(1));\
    int2 dimi = get_image_dim(src);\
    float2 dimf = (float2)(dimi.x, dimi.y);\
    float4 val;\
    int2 mi;\
    float m;\
\
    float4 xmap = read_imagef(xmapi, nearest_sampler, p);\
    float4 ymap = read_imagef(ymapi, nearest_sampler, p);\
    float2 pos  = (float2)(xmap.x, ymap.x);\
    pos.xy = pos.xy * 65535.f;\
\
    mi = ((pos >= (float2)(0.f, 0.f)) * (pos < dimf) * (p <= dimi));\
    m = mi.x && mi.y;\
    val = mix(fill_color, read_imagef(src, nearest_sampler, pos), m);\
\
    write_imagef(dst, p, val);\
}\
\
__kernel void remap_linear(__write_only image2d_t dst,\
                           __read_only  image2d_t src,\
                           __read_only  image2d_t xmapi,\
                           __read_only  image2d_t ymapi,\
                           float4 fill_color)\
{\
    int2 p = (int2)(get_global_id(0), get_global_id(1));\
    int2 dimi = get_image_dim(src);\
    float2 dimf = (float2)(dimi.x, dimi.y);\
    float4 val;\
    int2 mi;\
    float m;\
\
    float4 xmap = read_imagef(xmapi, nearest_sampler, p);\
    float4 ymap = read_imagef(ymapi, nearest_sampler, p);\
    float2 pos  = (float2)(xmap.x, ymap.x);\
    pos.xy = pos.xy * 65535.f;\
\
    mi = ((pos >= (float2)(0.f, 0.f)) * (pos < dimf) * (p <= dimi));\
    m = mi.x && mi.y;\
    val = mix(fill_color, read_imagef(src, linear_sampler, pos), m);\
\
    write_imagef(dst, p, val);\
}";

const char *ff_source_tonemap_cl = "#define REFERENCE_WHITE 100.0f \
extern float3 lrgb2yuv(float3);\
extern float  lrgb2y(float3);\
extern float3 yuv2lrgb(float3);\
extern float3 lrgb2lrgb(float3);\
extern float  get_luma_src(float3);\
extern float  get_luma_dst(float3);\
extern float3 ootf(float3 c, float peak);\
extern float3 inverse_ootf(float3 c, float peak);\
extern float3 get_chroma_sample(float3, float3, float3, float3);\
\
struct detection_result {\
    float peak;\
    float average;\
};\
\
float hable_f(float in) {\
    float a = 0.15f, b = 0.50f, c = 0.10f, d = 0.20f, e = 0.02f, f = 0.30f;\
    return (in * (in * a + b * c) + d * e) / (in * (in * a + b) + d * f) - e / f;\
}\
\
float direct(float s, float peak) {\
    return s;\
}\
\
float linear(float s, float peak) {\
    return s * tone_param / peak;\
}\
\
float gamma(float s, float peak) {\
    float p = s > 0.05f ? s /peak : 0.05f / peak;\
    float v = powr(p, 1.0f / tone_param);\
    return s > 0.05f ? v : (s * v /0.05f);\
}\
\
float clip(float s, float peak) {\
    return clamp(s * tone_param, 0.0f, 1.0f);\
}\
\
float reinhard(float s, float peak) {\
    return s / (s + tone_param) * (peak + tone_param) / peak;\
}\
\
float hable(float s, float peak) {\
    return hable_f(s)/hable_f(peak);\
}\
\
float mobius(float s, float peak) {\
    float j = tone_param;\
    float a, b;\
\
    if (s <= j)\
        return s;\
\
    a = -j * j * (peak - 1.0f) / (j * j - 2.0f * j + peak);\
    b = (j * j - 2.0f * j * peak + peak) / max(peak - 1.0f, 1e-6f);\
\
    return (b * b + 2.0f * b * j + j * j) / (b - a) * (s + a) / (s + b);\
}\
\
struct detection_result\
detect_peak_avg(global uint *util_buf, __local uint *sum_wg,\
            float signal, float peak) {\
    global uint *avg_buf = util_buf;\
    global uint *peak_buf = avg_buf + DETECTION_FRAMES + 1;\
    global uint *counter_wg_p = peak_buf + DETECTION_FRAMES + 1;\
    global uint *max_total_p = counter_wg_p + 1;\
    global uint *avg_total_p = max_total_p + 1;\
    global uint *frame_idx_p = avg_total_p + 1;\
    global uint *scene_frame_num_p = frame_idx_p + 1;\
\
    uint frame_idx = *frame_idx_p;\
    uint scene_frame_num = *scene_frame_num_p;\
\
    size_t lidx = get_local_id(0);\
    size_t lidy = get_local_id(1);\
    size_t lsizex = get_local_size(0);\
    size_t lsizey = get_local_size(1);\
    uint num_wg = get_num_groups(0) * get_num_groups(1);\
    size_t group_idx = get_group_id(0);\
    size_t group_idy = get_group_id(1);\
    struct detection_result r = {peak, sdr_avg};\
    if (lidx == 0 && lidy == 0)\
        *sum_wg = 0;\
    barrier(CLK_LOCAL_MEM_FENCE);\
\
    atomic_add(sum_wg, (uint)(signal * REFERENCE_WHITE));\
    barrier(CLK_LOCAL_MEM_FENCE);\
\
    if (lidx == 0 && lidy == 0) {\
        uint avg_wg = *sum_wg / (lsizex * lsizey);\
        atomic_max(&peak_buf[frame_idx], avg_wg);\
        atomic_add(&avg_buf[frame_idx], avg_wg);\
    }\
\
    if (scene_frame_num > 0) {\
        float peak = (float)*max_total_p / (REFERENCE_WHITE * scene_frame_num);\
        float avg = (float)*avg_total_p / (REFERENCE_WHITE * scene_frame_num);\
        r.peak = max(1.0f, peak);\
        r.average = max(0.25f, avg);\
    }\
\
    if (lidx == 0 && lidy == 0 && atomic_add(counter_wg_p, 1) == num_wg - 1) {\
        *counter_wg_p = 0;\
        avg_buf[frame_idx] /= num_wg;\
\
        if (scene_threshold > 0.0f) {\
            uint cur_max = peak_buf[frame_idx];\
            uint cur_avg = avg_buf[frame_idx];\
            int diff = (int)(scene_frame_num * cur_avg) - (int)*avg_total_p;\
\
            if (abs(diff) > scene_frame_num * scene_threshold * REFERENCE_WHITE) {\
                for (uint i = 0; i < DETECTION_FRAMES + 1; i++)\
                  avg_buf[i] = 0;\
                for (uint i = 0; i < DETECTION_FRAMES + 1; i++)\
                  peak_buf[i] = 0;\
                *avg_total_p = *max_total_p = 0;\
                *scene_frame_num_p = 0;\
                avg_buf[frame_idx] = cur_avg;\
                peak_buf[frame_idx] = cur_max;\
            }\
        }\
        uint next = (frame_idx + 1) % (DETECTION_FRAMES + 1);\
        *max_total_p += peak_buf[frame_idx] - peak_buf[next];\
        *avg_total_p += avg_buf[frame_idx] - avg_buf[next];\
        peak_buf[next] = avg_buf[next] = 0;\
        *frame_idx_p = next;\
        *scene_frame_num_p = min(*scene_frame_num_p + 1,\
                                 (uint)DETECTION_FRAMES);\
    }\
    return r;\
}\
\
float3 map_one_pixel_rgb(float3 rgb, float peak, float average) {\
    float sig = max(max(rgb.x, max(rgb.y, rgb.z)), 1e-6f);\
\
    if (target_peak > 1.0f) {\
        sig *= 1.0f / target_peak;\
        peak *= 1.0f / target_peak;\
    }\
\
    float sig_old = sig;\
\
    float slope = min(1.0f, sdr_avg / average);\
    sig *= slope;\
    peak *= slope;\
\
    if (desat_param > 0.0f) {\
        float luma = get_luma_dst(rgb);\
        float coeff = max(sig - 0.18f, 1e-6f) / max(sig, 1e-6f);\
        coeff = native_powr(coeff, 10.0f / desat_param);\
        rgb = mix(rgb, (float3)luma, (float3)coeff);\
        sig = mix(sig, luma * slope, coeff);\
    }\
\
    sig = TONE_FUNC(sig, peak);\
\
    sig = min(sig, 1.0f);\
    rgb *= (sig/sig_old);\
    return rgb;\
}\
\
float3 map_to_dst_space_from_yuv(float3 yuv, float peak) {\
    float3 c = yuv2lrgb(yuv);\
    c = ootf(c, peak);\
    c = lrgb2lrgb(c);\
    return c;\
}\
\
__kernel void tonemap(__write_only image2d_t dst1,\
                      __read_only  image2d_t src1,\
                      __write_only image2d_t dst2,\
                      __read_only  image2d_t src2,\
                      global uint *util_buf,\
                      float peak\
                      )\
{\
    __local uint sum_wg;\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
    int xi = get_global_id(0);\
    int yi = get_global_id(1);\
    int x = 2 * xi;\
    int y = 2 * yi;\
\
    float y0 = read_imagef(src1, sampler, (int2)(x,     y)).x;\
    float y1 = read_imagef(src1, sampler, (int2)(x + 1, y)).x;\
    float y2 = read_imagef(src1, sampler, (int2)(x,     y + 1)).x;\
    float y3 = read_imagef(src1, sampler, (int2)(x + 1, y + 1)).x;\
    float2 uv = read_imagef(src2, sampler, (int2)(xi,     yi)).xy;\
\
    float3 c0 = map_to_dst_space_from_yuv((float3)(y0, uv.x, uv.y), peak);\
    float3 c1 = map_to_dst_space_from_yuv((float3)(y1, uv.x, uv.y), peak);\
    float3 c2 = map_to_dst_space_from_yuv((float3)(y2, uv.x, uv.y), peak);\
    float3 c3 = map_to_dst_space_from_yuv((float3)(y3, uv.x, uv.y), peak);\
\
    float sig0 = max(c0.x, max(c0.y, c0.z));\
    float sig1 = max(c1.x, max(c1.y, c1.z));\
    float sig2 = max(c2.x, max(c2.y, c2.z));\
    float sig3 = max(c3.x, max(c3.y, c3.z));\
    float sig = max(sig0, max(sig1, max(sig2, sig3)));\
\
    struct detection_result r = detect_peak_avg(util_buf, &sum_wg, sig, peak);\
\
    float3 c0_old = c0, c1_old = c1, c2_old = c2;\
    c0 = map_one_pixel_rgb(c0, r.peak, r.average);\
    c1 = map_one_pixel_rgb(c1, r.peak, r.average);\
    c2 = map_one_pixel_rgb(c2, r.peak, r.average);\
    c3 = map_one_pixel_rgb(c3, r.peak, r.average);\
\
    c0 = inverse_ootf(c0, target_peak);\
    c1 = inverse_ootf(c1, target_peak);\
    c2 = inverse_ootf(c2, target_peak);\
    c3 = inverse_ootf(c3, target_peak);\
\
    y0 = lrgb2y(c0);\
    y1 = lrgb2y(c1);\
    y2 = lrgb2y(c2);\
    y3 = lrgb2y(c3);\
    float3 chroma_c = get_chroma_sample(c0, c1, c2, c3);\
    float3 chroma = lrgb2yuv(chroma_c);\
\
    if (xi < get_image_width(dst2) && yi < get_image_height(dst2)) {\
        write_imagef(dst1, (int2)(x, y), (float4)(y0, 0.0f, 0.0f, 1.0f));\
        write_imagef(dst1, (int2)(x+1, y), (float4)(y1, 0.0f, 0.0f, 1.0f));\
        write_imagef(dst1, (int2)(x, y+1), (float4)(y2, 0.0f, 0.0f, 1.0f));\
        write_imagef(dst1, (int2)(x+1, y+1), (float4)(y3, 0.0f, 0.0f, 1.0f));\
        write_imagef(dst2, (int2)(xi, yi),\
                     (float4)(chroma.y, chroma.z, 0.0f, 1.0f));\
    }\
}";

const char *ff_source_transpose_cl = "kernel void transpose(__write_only image2d_t dst,\
                      __read_only image2d_t src,\
                      int dir) {\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\
                               CLK_FILTER_NEAREST);\
\
    int2 size = get_image_dim(dst);\
    int x = get_global_id(0);\
    int y = get_global_id(1);\
\
    int xin = (dir & 2) ? (size.y - 1 - y) : y;\
    int yin = (dir & 1) ? (size.x - 1 - x) : x;\
    float4 data = read_imagef(src, sampler, (int2)(xin, yin));\
\
    if (x < size.x && y < size.y)\
        write_imagef(dst, (int2)(x, y), data);\
}";

const char *ff_source_unsharp_cl = "__kernel void unsharp_global(__write_only image2d_t dst,\
                             __read_only  image2d_t src,\
                             int size_x,\
                             int size_y,\
                             float amount,\
                             __constant float *coef_matrix)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_FILTER_NEAREST);\
    int2 loc    = (int2)(get_global_id(0), get_global_id(1));\
    int2 centre = (int2)(size_x / 2, size_y / 2);\
\
    float4 val = read_imagef(src, sampler, loc);\
    float4 sum = 0.0f;\
    int x, y;\
\
    for (y = 0; y < size_y; y++) {\
        for (x = 0; x < size_x; x++) {\
            int2 pos = loc + (int2)(x, y) - centre;\
            sum += coef_matrix[y * size_x + x] *\
                read_imagef(src, sampler, pos);\
        }\
    }\
\
    write_imagef(dst, loc, val + (val - sum) * amount);\
}\
\
__kernel void unsharp_local(__write_only image2d_t dst,\
                            __read_only  image2d_t src,\
                            int size_x,\
                            int size_y,\
                            float amount,\
                            __constant float *coef_x,\
                            __constant float *coef_y)\
{\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                               CLK_ADDRESS_CLAMP_TO_EDGE |\
                               CLK_FILTER_NEAREST);\
    int2 block = (int2)(get_group_id(0), get_group_id(1)) * 16;\
    int2 pos   = (int2)(get_local_id(0), get_local_id(1));\
\
    __local float4 tmp[32][32];\
\
    int rad_x = size_x / 2;\
    int rad_y = size_y / 2;\
    int x, y;\
\
    for (y = 0; y <= 1; y++) {\
        for (x = 0; x <= 1; x++) {\
            tmp[pos.y + 16 * y][pos.x + 16 * x] =\
                read_imagef(src, sampler, block + pos + (int2)(16 * x - 8, 16 * y - 8));\
        }\
    }\
\
    barrier(CLK_LOCAL_MEM_FENCE);\
\
    float4 val = tmp[pos.y + 8][pos.x + 8];\
\
    float4 horiz[2];\
    for (y = 0; y <= 1; y++) {\
        horiz[y] = 0.0f;\
        for (x = 0; x < size_x; x++)\
            horiz[y] += coef_x[x] * tmp[pos.y + y * 16][pos.x + 8 + x - rad_x];\
    }\
\
    barrier(CLK_LOCAL_MEM_FENCE);\
\
    for (y = 0; y <= 1; y++) {\
        tmp[pos.y + y * 16][pos.x + 8] = horiz[y];\
    }\
\
    barrier(CLK_LOCAL_MEM_FENCE);\
\
    float4 sum = 0.0f;\
    for (y = 0; y < size_y; y++)\
        sum += coef_y[y] * tmp[pos.y + 8 + y - rad_y][pos.x + 8];\
\
    if (block.x + pos.x < get_image_width(dst) &&\
        block.y + pos.y < get_image_height(dst))\
        write_imagef(dst, block + pos, val + (val - sum) * amount);\
}";

const char *ff_source_xfade_cl = "const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\
                           CLK_FILTER_NEAREST);\
\
__kernel void fade(__write_only image2d_t dst,\
                   __read_only  image2d_t src1,\
                   __read_only  image2d_t src2,\
                   float progress)\
{\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 val1 = read_imagef(src1, sampler, p);\
    float4 val2 = read_imagef(src2, sampler, p);\
\
    write_imagef(dst, p, mix(val2, val1, progress));\
}\
\
__kernel void wipeleft(__write_only image2d_t dst,\
                       __read_only  image2d_t src1,\
                       __read_only  image2d_t src2,\
                       float progress)\
{\
    int   s = (int)(get_image_dim(src1).x * progress);\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 val1 = read_imagef(src1, sampler, p);\
    float4 val2 = read_imagef(src2, sampler, p);\
\
    write_imagef(dst, p, p.x > s ? val2 : val1);\
}\
\
__kernel void wiperight(__write_only image2d_t dst,\
                        __read_only  image2d_t src1,\
                        __read_only  image2d_t src2,\
                        float progress)\
{\
    int   s = (int)(get_image_dim(src1).x * (1.f - progress));\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 val1 = read_imagef(src1, sampler, p);\
    float4 val2 = read_imagef(src2, sampler, p);\
\
    write_imagef(dst, p, p.x > s ? val1 : val2);\
}\
\
__kernel void wipeup(__write_only image2d_t dst,\
                     __read_only  image2d_t src1,\
                     __read_only  image2d_t src2,\
                     float progress)\
{\
    int   s = (int)(get_image_dim(src1).y * progress);\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 val1 = read_imagef(src1, sampler, p);\
    float4 val2 = read_imagef(src2, sampler, p);\
\
    write_imagef(dst, p, p.y > s ? val2 : val1);\
}\
\
__kernel void wipedown(__write_only image2d_t dst,\
                       __read_only  image2d_t src1,\
                       __read_only  image2d_t src2,\
                       float progress)\
{\
    int   s = (int)(get_image_dim(src1).y * (1.f - progress));\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\
\
    float4 val1 = read_imagef(src1, sampler, p);\
    float4 val2 = read_imagef(src2, sampler, p);\
\
    write_imagef(dst, p, p.y > s ? val1 : val2);\
}\
\
void slide(__write_only image2d_t dst,\
           __read_only  image2d_t src1,\
           __read_only  image2d_t src2,\
           float progress,\
           int2 direction)\
{\
    int   w = get_image_dim(src1).x;\
    int   h = get_image_dim(src1).y;\
    int2 wh = (int2)(w, h);\
    int2 uv = (int2)(get_global_id(0), get_global_id(1));\
    int2 pi = (int2)(progress * w, progress * h);\
    int2 p = uv + pi * direction;\
    int2 f = p % wh;\
\
    f = f + (int2)(w, h) * (int2)(f.x < 0, f.y < 0);\
    float4 val1 = read_imagef(src1, sampler, f);\
    float4 val2 = read_imagef(src2, sampler, f);\
    write_imagef(dst, uv, mix(val1, val2, (p.y >= 0) * (h > p.y) * (p.x >= 0) * (w > p.x)));\
}\
\
__kernel void slidedown(__write_only image2d_t dst,\
                        __read_only  image2d_t src1,\
                        __read_only  image2d_t src2,\
                        float progress)\
{\
    int2 direction = (int2)(0, 1);\
    slide(dst, src1, src2, progress, direction);\
}\
\
__kernel void slideup(__write_only image2d_t dst,\
                      __read_only  image2d_t src1,\
                      __read_only  image2d_t src2,\
                      float progress)\
{\
    int2 direction = (int2)(0, -1);\
    slide(dst, src1, src2, progress, direction);\
}\
\
__kernel void slideleft(__write_only image2d_t dst,\
                        __read_only  image2d_t src1,\
                        __read_only  image2d_t src2,\
                        float progress)\
{\
    int2 direction = (int2)(-1, 0);\
    slide(dst, src1, src2, progress, direction);\
}\
\
__kernel void slideright(__write_only image2d_t dst,\
                         __read_only  image2d_t src1,\
                         __read_only  image2d_t src2,\
                         float progress)\
{\
    int2 direction = (int2)(1, 0);\
    slide(dst, src1, src2, progress, direction);\
}";
