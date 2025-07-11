#include <stddef.h>
#include "opencl_source.h"

const char *ff_source_avgblur_cl = "\
__kernel void avgblur_horiz(__write_only image2d_t dst,\n\
                            __read_only  image2d_t src,\n\
                            int rad)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_FILTER_NEAREST);\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    int2 size = (int2)(get_global_size(0), get_global_size(1));\n\
\n\
    int count = 0;\n\
    float4 acc = (float4)(0,0,0,0);\n\
\n\
    for (int xx = max(0, loc.x - rad); xx < min(loc.x + rad + 1, size.x); xx++) {\n\
        count++;\n\
        acc += read_imagef(src, sampler, (int2)(xx, loc.y));\n\
    }\n\
\n\
    write_imagef(dst, loc, acc / count);\n\
}\n\
\n\
__kernel void avgblur_vert(__write_only image2d_t dst,\n\
                           __read_only  image2d_t src,\n\
                           int radv)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_FILTER_NEAREST);\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    int2 size = (int2)(get_global_size(0), get_global_size(1));\n\
\n\
    int count = 0;\n\
    float4 acc = (float4)(0,0,0,0);\n\
\n\
    for (int yy = max(0, loc.y - radv); yy < min(loc.y + radv + 1, size.y); yy++) {\n\
        count++;\n\
        acc += read_imagef(src, sampler, (int2)(loc.x, yy));\n\
    }\n\
\n\
    write_imagef(dst, loc, acc / count);\n\
}";

const char *ff_source_colorkey_cl = "\
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |\n\
                          CLK_FILTER_NEAREST;\n\
\n\
__kernel void colorkey_blend(\n\
    __read_only  image2d_t src,\n\
    __write_only image2d_t dst,\n\
    float4 colorkey_rgba,\n\
    float similarity,\n\
    float blend\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    float4 pixel = read_imagef(src, sampler, loc);\n\
    float diff = distance(pixel.xyz, colorkey_rgba.xyz);\n\
\n\
    pixel.s3 = clamp((diff - similarity) / blend, 0.0f, 1.0f);\n\
    write_imagef(dst, loc, pixel);\n\
}\n\
\n\
__kernel void colorkey(\n\
    __read_only  image2d_t src,\n\
    __write_only image2d_t dst,\n\
    float4 colorkey_rgba,\n\
    float similarity\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    float4 pixel = read_imagef(src, sampler, loc);\n\
    float diff = distance(pixel.xyz, colorkey_rgba.xyz);\n\
\n\
    pixel.s3 = (diff > similarity) ? 1.0f : 0.0f;\n\
    write_imagef(dst, loc, pixel);\n\
}";

const char *ff_source_colorspace_common_cl = "\
#define ST2084_MAX_LUMINANCE 10000.0f\n\
#define REFERENCE_WHITE 100.0f\n\
\n\
#if chroma_loc == 1\n\
    #define chroma_sample(a,b,c,d) (((a) + (c)) * 0.5f)\n\
#elif chroma_loc == 3\n\
    #define chroma_sample(a,b,c,d) (a)\n\
#elif chroma_loc == 4\n\
    #define chroma_sample(a,b,c,d) (((a) + (b)) * 0.5f)\n\
#elif chroma_loc == 5\n\
    #define chroma_sample(a,b,c,d) (c)\n\
#elif chroma_loc == 6\n\
    #define chroma_sample(a,b,c,d) (((c) + (d)) * 0.5f)\n\
#else\n\
    #define chroma_sample(a,b,c,d) (((a) + (b) + (c) + (d)) * 0.25f)\n\
#endif\n\
\n\
constant const float ST2084_M1 = 0.1593017578125f;\n\
constant const float ST2084_M2 = 78.84375f;\n\
constant const float ST2084_C1 = 0.8359375f;\n\
constant const float ST2084_C2 = 18.8515625f;\n\
constant const float ST2084_C3 = 18.6875f;\n\
\n\
float get_luma_dst(float3 c) {\n\
    return luma_dst.x * c.x + luma_dst.y * c.y + luma_dst.z * c.z;\n\
}\n\
\n\
float get_luma_src(float3 c) {\n\
    return luma_src.x * c.x + luma_src.y * c.y + luma_src.z * c.z;\n\
}\n\
\n\
float3 get_chroma_sample(float3 a, float3 b, float3 c, float3 d) {\n\
    return chroma_sample(a, b, c, d);\n\
}\n\
\n\
float eotf_st2084(float x) {\n\
    float p = powr(x, 1.0f / ST2084_M2);\n\
    float a = max(p -ST2084_C1, 0.0f);\n\
    float b = max(ST2084_C2 - ST2084_C3 * p, 1e-6f);\n\
    float c  = powr(a / b, 1.0f / ST2084_M1);\n\
    return x > 0.0f ? c * ST2084_MAX_LUMINANCE / REFERENCE_WHITE : 0.0f;\n\
}\n\
\n\
__constant const float HLG_A = 0.17883277f;\n\
__constant const float HLG_B = 0.28466892f;\n\
__constant const float HLG_C = 0.55991073f;\n\
\n\
// linearizer for HLG\n\
float inverse_oetf_hlg(float x) {\n\
    float a = 4.0f * x * x;\n\
    float b = exp((x - HLG_C) / HLG_A) + HLG_B;\n\
    return x < 0.5f ? a : b;\n\
}\n\
\n\
// delinearizer for HLG\n\
float oetf_hlg(float x) {\n\
    float a = 0.5f * sqrt(x);\n\
    float b = HLG_A * log(x - HLG_B) + HLG_C;\n\
    return x <= 1.0f ? a : b;\n\
}\n\
\n\
float3 ootf_hlg(float3 c, float peak) {\n\
    float luma = get_luma_src(c);\n\
    float gamma =  1.2f + 0.42f * log10(peak * REFERENCE_WHITE / 1000.0f);\n\
    gamma = max(1.0f, gamma);\n\
    float factor = peak * powr(luma, gamma - 1.0f) / powr(12.0f, gamma);\n\
    return c * factor;\n\
}\n\
\n\
float3 inverse_ootf_hlg(float3 c, float peak) {\n\
    float gamma = 1.2f + 0.42f * log10(peak * REFERENCE_WHITE / 1000.0f);\n\
    c *=  powr(12.0f, gamma) / peak;\n\
    c /= powr(get_luma_dst(c), (gamma - 1.0f) / gamma);\n\
    return c;\n\
}\n\
\n\
float inverse_eotf_bt1886(float c) {\n\
    return c < 0.0f ? 0.0f : powr(c, 1.0f / 2.4f);\n\
}\n\
\n\
float oetf_bt709(float c) {\n\
    c = c < 0.0f ? 0.0f : c;\n\
    float r1 = 4.5f * c;\n\
    float r2 = 1.099f * powr(c, 0.45f) - 0.099f;\n\
    return c < 0.018f ? r1 : r2;\n\
}\n\
float inverse_oetf_bt709(float c) {\n\
    float r1 = c / 4.5f;\n\
    float r2 = powr((c + 0.099f) / 1.099f, 1.0f / 0.45f);\n\
    return c < 0.081f ? r1 : r2;\n\
}\n\
\n\
float3 yuv2rgb(float y, float u, float v) {\n\
#ifdef FULL_RANGE_IN\n\
    u -= 0.5f; v -= 0.5f;\n\
#else\n\
    y = (y * 255.0f -  16.0f) / 219.0f;\n\
    u = (u * 255.0f - 128.0f) / 224.0f;\n\
    v = (v * 255.0f - 128.0f) / 224.0f;\n\
#endif\n\
    float r = y * rgb_matrix[0] + u * rgb_matrix[1] + v * rgb_matrix[2];\n\
    float g = y * rgb_matrix[3] + u * rgb_matrix[4] + v * rgb_matrix[5];\n\
    float b = y * rgb_matrix[6] + u * rgb_matrix[7] + v * rgb_matrix[8];\n\
    return (float3)(r, g, b);\n\
}\n\
\n\
float3 yuv2lrgb(float3 yuv) {\n\
    float3 rgb = yuv2rgb(yuv.x, yuv.y, yuv.z);\n\
#ifdef linearize\n\
    float r = linearize(rgb.x);\n\
    float g = linearize(rgb.y);\n\
    float b = linearize(rgb.z);\n\
    return (float3)(r, g, b);\n\
#else\n\
    return rgb;\n\
#endif\n\
}\n\
\n\
float3 rgb2yuv(float r, float g, float b) {\n\
    float y = r*yuv_matrix[0] + g*yuv_matrix[1] + b*yuv_matrix[2];\n\
    float u = r*yuv_matrix[3] + g*yuv_matrix[4] + b*yuv_matrix[5];\n\
    float v = r*yuv_matrix[6] + g*yuv_matrix[7] + b*yuv_matrix[8];\n\
#ifdef FULL_RANGE_OUT\n\
    u += 0.5f; v += 0.5f;\n\
#else\n\
    y = (219.0f * y + 16.0f) / 255.0f;\n\
    u = (224.0f * u + 128.0f) / 255.0f;\n\
    v = (224.0f * v + 128.0f) / 255.0f;\n\
#endif\n\
    return (float3)(y, u, v);\n\
}\n\
\n\
float rgb2y(float r, float g, float b) {\n\
    float y = r*yuv_matrix[0] + g*yuv_matrix[1] + b*yuv_matrix[2];\n\
    y = (219.0f * y + 16.0f) / 255.0f;\n\
    return y;\n\
}\n\
\n\
float3 lrgb2yuv(float3 c) {\n\
#ifdef delinearize\n\
    float r = delinearize(c.x);\n\
    float g = delinearize(c.y);\n\
    float b = delinearize(c.z);\n\
    return rgb2yuv(r, g, b);\n\
#else\n\
    return rgb2yuv(c.x, c.y, c.z);\n\
#endif\n\
}\n\
\n\
float lrgb2y(float3 c) {\n\
#ifdef delinearize\n\
    float r = delinearize(c.x);\n\
    float g = delinearize(c.y);\n\
    float b = delinearize(c.z);\n\
    return rgb2y(r, g, b);\n\
#else\n\
    return rgb2y(c.x, c.y, c.z);\n\
#endif\n\
}\n\
\n\
float3 lrgb2lrgb(float3 c) {\n\
#ifdef RGB2RGB_PASSTHROUGH\n\
    return c;\n\
#else\n\
    float r = c.x, g = c.y, b = c.z;\n\
    float rr = rgb2rgb[0] * r + rgb2rgb[1] * g + rgb2rgb[2] * b;\n\
    float gg = rgb2rgb[3] * r + rgb2rgb[4] * g + rgb2rgb[5] * b;\n\
    float bb = rgb2rgb[6] * r + rgb2rgb[7] * g + rgb2rgb[8] * b;\n\
    return (float3)(rr, gg, bb);\n\
#endif\n\
}\n\
\n\
float3 ootf(float3 c, float peak) {\n\
#ifdef ootf_impl\n\
    return ootf_impl(c, peak);\n\
#else\n\
    return c;\n\
#endif\n\
}\n\
\n\
float3 inverse_ootf(float3 c, float peak) {\n\
#ifdef inverse_ootf_impl\n\
    return inverse_ootf_impl(c, peak);\n\
#else\n\
    return c;\n\
#endif\n\
}";

const char *ff_source_convolution_cl = "\
__kernel void convolution_global(__write_only image2d_t dst,\n\
                                 __read_only  image2d_t src,\n\
                                 int coef_matrix_dim,\n\
                                 __constant float *coef_matrix,\n\
                                 float div,\n\
                                 float bias)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    const int half_matrix_dim = (coef_matrix_dim / 2);\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    float4 convPix = (float4)(0.0f, 0.0f, 0.0f, 0.0f);\n\
\n\
    for (int conv_i = -half_matrix_dim; conv_i <= half_matrix_dim; conv_i++) {\n\
        for (int conv_j = -half_matrix_dim; conv_j <= half_matrix_dim; conv_j++) {\n\
            float4 px = read_imagef(src, sampler, loc + (int2)(conv_j, conv_i));\n\
            convPix += px * coef_matrix[(conv_i + half_matrix_dim) * coef_matrix_dim +\n\
                                        (conv_j + half_matrix_dim)];\n\
        }\n\
     }\n\
     float4 dstPix = convPix * div + bias;\n\
     write_imagef(dst, loc, dstPix);\n\
}\n\
\n\
__kernel void sobel_global(__write_only image2d_t dst,\n\
                           __read_only  image2d_t src,\n\
                             float div,\n\
                             float bias)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 sum1 = read_imagef(src, sampler, loc + (int2)(-1,-1)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 0,-1)) * -2 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 0, 1)) *  2 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) *  1;\n\
\n\
    float4 sum2 = read_imagef(src, sampler, loc + (int2)(-1,-1)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)(-1, 0)) * -2 +\n\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1, 0)) *  2 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) *  1;\n\
\n\
    float4 dstPix = hypot(sum1, sum2) * div + bias;\n\
    write_imagef(dst, loc, dstPix);\n\
}\n\
\n\
__kernel void prewitt_global(__write_only image2d_t dst,\n\
                             __read_only  image2d_t src,\n\
                             float div,\n\
                             float bias)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 sum1 = read_imagef(src, sampler, loc + (int2)(-1,-1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 0,-1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 0, 1)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) * -1;\n\
\n\
    float4 sum2 = read_imagef(src, sampler, loc + (int2)(-1,-1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)(-1, 0)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)(-1, 1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1,-1)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1, 0)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 1, 1)) * -1;\n\
\n\
    float4 dstPix = hypot(sum1, sum2) * div + bias;\n\
    write_imagef(dst, loc, dstPix);\n\
}\n\
\n\
__kernel void roberts_global(__write_only image2d_t dst,\n\
                             __read_only  image2d_t src,\n\
                             float div,\n\
                             float bias)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\
\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 sum1 = read_imagef(src, sampler, loc + (int2)(-1,-1)) *  1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 0,-1)) * -1;\n\
\n\
    float4 sum2 = read_imagef(src, sampler, loc + (int2)(-1, 0)) * -1 +\n\
                  read_imagef(src, sampler, loc + (int2)( 0, 0)) *  1;\n\
\n\
    float4 dstPix = hypot(sum1, sum2) * div + bias;\n\
    write_imagef(dst, loc, dstPix);\n\
}";

const char *ff_source_deshake_cl = "\
#define HARRIS_THRESHOLD 3.0f\n\
// Block size over which to compute harris response\n\
//\n\
// Note that changing this will require fiddling with the local array sizes in\n\
// harris_response\n\
#define HARRIS_RADIUS 2\n\
#define DISTANCE_THRESHOLD 80\n\
\n\
// Sub-pixel refinement window for feature points\n\
#define REFINE_WIN_HALF_W 5\n\
#define REFINE_WIN_HALF_H 5\n\
#define REFINE_WIN_W 11 // REFINE_WIN_HALF_W * 2 + 1\n\
#define REFINE_WIN_H 11\n\
\n\
// Non-maximum suppression window size\n\
#define NONMAX_WIN 30\n\
#define NONMAX_WIN_HALF 15 // NONMAX_WIN / 2\n\
\n\
typedef struct PointPair {\n\
    // Previous frame\n\
    float2 p1;\n\
    // Current frame\n\
    float2 p2;\n\
} PointPair;\n\
\n\
typedef struct SmoothedPointPair {\n\
    // Non-smoothed point in current frame\n\
    int2 p1;\n\
    // Smoothed point in current frame\n\
    float2 p2;\n\
} SmoothedPointPair;\n\
\n\
typedef struct MotionVector {\n\
    PointPair p;\n\
    // Used to mark vectors as potential outliers\n\
    int should_consider;\n\
} MotionVector;\n\
\n\
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |\n\
                          CLK_ADDRESS_CLAMP_TO_EDGE |\n\
                          CLK_FILTER_NEAREST;\n\
\n\
const sampler_t sampler_linear = CLK_NORMALIZED_COORDS_FALSE |\n\
                          CLK_ADDRESS_CLAMP_TO_EDGE |\n\
                          CLK_FILTER_LINEAR;\n\
\n\
const sampler_t sampler_linear_mirror = CLK_NORMALIZED_COORDS_TRUE |\n\
                          CLK_ADDRESS_MIRRORED_REPEAT |\n\
                          CLK_FILTER_LINEAR;\n\
\n\
// Writes to a 1D array at loc, treating it as a 2D array with the same\n\
// dimensions as the global work size.\n\
static void write_to_1d_arrf(__global float *buf, int2 loc, float val) {\n\
    buf[loc.x + loc.y * get_global_size(0)] = val;\n\
}\n\
\n\
static void write_to_1d_arrul8(__global ulong8 *buf, int2 loc, ulong8 val) {\n\
    buf[loc.x + loc.y * get_global_size(0)] = val;\n\
}\n\
\n\
static void write_to_1d_arrvec(__global MotionVector *buf, int2 loc, MotionVector val) {\n\
    buf[loc.x + loc.y * get_global_size(0)] = val;\n\
}\n\
\n\
static void write_to_1d_arrf2(__global float2 *buf, int2 loc, float2 val) {\n\
    buf[loc.x + loc.y * get_global_size(0)] = val;\n\
}\n\
\n\
static ulong8 read_from_1d_arrul8(__global const ulong8 *buf, int2 loc) {\n\
    return buf[loc.x + loc.y * get_global_size(0)];\n\
}\n\
\n\
static float2 read_from_1d_arrf2(__global const float2 *buf, int2 loc) {\n\
    return buf[loc.x + loc.y * get_global_size(0)];\n\
}\n\
\n\
// Returns the grayscale value at the given point.\n\
static float pixel_grayscale(__read_only image2d_t src, int2 loc) {\n\
    float4 pixel = read_imagef(src, sampler, loc);\n\
    return (pixel.x + pixel.y + pixel.z) / 3.0f;\n\
}\n\
\n\
static float convolve(\n\
    __local const float *grayscale,\n\
    int local_idx_x,\n\
    int local_idx_y,\n\
    float mask[3][3]\n\
) {\n\
    float ret = 0;\n\
\n\
    // These loops touch each pixel surrounding loc as well as loc itself\n\
    for (int i = 1, i2 = 0; i >= -1; --i, ++i2) {\n\
        for (int j = -1, j2 = 0; j <= 1; ++j, ++j2) {\n\
            ret += mask[i2][j2] * grayscale[(local_idx_x + 3 + j) + (local_idx_y + 3 + i) * 14];\n\
        }\n\
    }\n\
\n\
    return ret;\n\
}\n\
\n\
// Sums dx * dy for all pixels within radius of loc\n\
static float sum_deriv_prod(\n\
    __local const float *grayscale,\n\
    float mask_x[3][3],\n\
    float mask_y[3][3]\n\
) {\n\
    float ret = 0;\n\
\n\
    for (int i = HARRIS_RADIUS; i >= -HARRIS_RADIUS; --i) {\n\
        for (int j = -HARRIS_RADIUS; j <= HARRIS_RADIUS; ++j) {\n\
            ret += convolve(grayscale, get_local_id(0) + j, get_local_id(1) + i, mask_x) *\n\
                   convolve(grayscale, get_local_id(0) + j, get_local_id(1) + i, mask_y);\n\
        }\n\
    }\n\
\n\
    return ret;\n\
}\n\
\n\
// Sums d<>^2 (determined by mask) for all pixels within radius of loc\n\
static float sum_deriv_pow(__local const float *grayscale, float mask[3][3])\n\
{\n\
    float ret = 0;\n\
\n\
    for (int i = HARRIS_RADIUS; i >= -HARRIS_RADIUS; --i) {\n\
        for (int j = -HARRIS_RADIUS; j <= HARRIS_RADIUS; ++j) {\n\
            float deriv = convolve(grayscale, get_local_id(0) + j, get_local_id(1) + i, mask);\n\
            ret += deriv * deriv;\n\
        }\n\
    }\n\
\n\
    return ret;\n\
}\n\
\n\
// Fills a box with the given radius and pixel around loc\n\
static void draw_box(__write_only image2d_t dst, int2 loc, float4 pixel, int radius)\n\
{\n\
    for (int i = -radius; i <= radius; ++i) {\n\
        for (int j = -radius; j <= radius; ++j) {\n\
            write_imagef(\n\
                dst,\n\
                (int2)(\n\
                    // Clamp to avoid writing outside image bounds\n\
                    clamp(loc.x + i, 0, get_image_dim(dst).x - 1),\n\
                    clamp(loc.y + j, 0, get_image_dim(dst).y - 1)\n\
                ),\n\
                pixel\n\
            );\n\
        }\n\
    }\n\
}\n\
\n\
// Converts the src image to grayscale\n\
__kernel void grayscale(\n\
    __read_only image2d_t src,\n\
    __write_only image2d_t grayscale\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    write_imagef(grayscale, loc, (float4)(pixel_grayscale(src, loc), 0.0f, 0.0f, 1.0f));\n\
}\n\
\n\
// This kernel computes the harris response for the given grayscale src image\n\
// within the given radius and writes it to harris_buf\n\
__kernel void harris_response(\n\
    __read_only image2d_t grayscale,\n\
    __global float *harris_buf\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    if (loc.x > get_image_width(grayscale) - 1 || loc.y > get_image_height(grayscale) - 1) {\n\
        write_to_1d_arrf(harris_buf, loc, 0);\n\
        return;\n\
    }\n\
\n\
    float scale = 1.0f / ((1 << 2) * HARRIS_RADIUS * 255.0f);\n\
\n\
    float sobel_mask_x[3][3] = {\n\
        {-1, 0, 1},\n\
        {-2, 0, 2},\n\
        {-1, 0, 1}\n\
    };\n\
\n\
    float sobel_mask_y[3][3] = {\n\
        { 1,  2,  1},\n\
        { 0,  0,  0},\n\
        {-1, -2, -1}\n\
    };\n\
\n\
    // 8 x 8 local work + 3 pixels around each side (needed to accommodate for the\n\
    // block size radius of 2)\n\
    __local float grayscale_data[196];\n\
\n\
    int idx = get_group_id(0) * get_local_size(0);\n\
    int idy = get_group_id(1) * get_local_size(1);\n\
\n\
    for (int i = idy - 3, it = 0; i < idy + (int)get_local_size(1) + 3; i++, it++) {\n\
        for (int j = idx - 3, jt = 0; j < idx + (int)get_local_size(0) + 3; j++, jt++) {\n\
            grayscale_data[jt + it * 14] = read_imagef(grayscale, sampler, (int2)(j, i)).x;\n\
        }\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    float sumdxdy = sum_deriv_prod(grayscale_data, sobel_mask_x, sobel_mask_y);\n\
    float sumdx2 = sum_deriv_pow(grayscale_data, sobel_mask_x);\n\
    float sumdy2 = sum_deriv_pow(grayscale_data, sobel_mask_y);\n\
\n\
    float trace = sumdx2 + sumdy2;\n\
    // r = det(M) - k(trace(M))^2\n\
    // k usually between 0.04 to 0.06\n\
    float r = (sumdx2 * sumdy2 - sumdxdy * sumdxdy) - 0.04f * (trace * trace) * pown(scale, 4);\n\
\n\
    // Threshold the r value\n\
    harris_buf[loc.x + loc.y * get_image_width(grayscale)] = r * step(HARRIS_THRESHOLD, r);\n\
}\n\
\n\
// Gets a patch centered around a float coordinate from a grayscale image using\n\
// bilinear interpolation\n\
static void get_rect_sub_pix(\n\
    __read_only image2d_t grayscale,\n\
    float *buffer,\n\
    int size_x,\n\
    int size_y,\n\
    float2 center\n\
) {\n\
    float2 offset = ((float2)(size_x, size_y) - 1.0f) * 0.5f;\n\
\n\
    for (int i = 0; i < size_y; i++) {\n\
        for (int j = 0; j < size_x; j++) {\n\
            buffer[i * size_x + j] = read_imagef(\n\
                grayscale,\n\
                sampler_linear,\n\
                (float2)(j, i) + center - offset\n\
            ).x * 255.0f;\n\
        }\n\
    }\n\
}\n\
\n\
// Refines detected features at a sub-pixel level\n\
//\n\
// This function is ported from OpenCV\n\
static float2 corner_sub_pix(\n\
    __read_only image2d_t grayscale,\n\
    float2 feature,\n\
    float *mask\n\
) {\n\
    float2 init = feature;\n\
    int src_width = get_global_size(0);\n\
    int src_height = get_global_size(1);\n\
\n\
    const int max_iters = 40;\n\
    const float eps = 0.001f * 0.001f;\n\
    int i, j, k;\n\
\n\
    int iter = 0;\n\
    float err = 0;\n\
    float subpix[(REFINE_WIN_W + 2) * (REFINE_WIN_H + 2)];\n\
    const float flt_epsilon = 0x1.0p-23f;\n\
\n\
    do {\n\
        float2 feature_tmp;\n\
        float a = 0, b = 0, c = 0, bb1 = 0, bb2 = 0;\n\
\n\
        get_rect_sub_pix(grayscale, subpix, REFINE_WIN_W + 2, REFINE_WIN_H + 2, feature);\n\
        float *subpix_ptr = subpix;\n\
        subpix_ptr += REFINE_WIN_W + 2 + 1;\n\
\n\
        // process gradient\n\
        for (i = 0, k = 0; i < REFINE_WIN_H; i++, subpix_ptr += REFINE_WIN_W + 2) {\n\
            float py = i - REFINE_WIN_HALF_H;\n\
\n\
            for (j = 0; j < REFINE_WIN_W; j++, k++) {\n\
                float m = mask[k];\n\
                float tgx = subpix_ptr[j + 1] - subpix_ptr[j - 1];\n\
                float tgy = subpix_ptr[j + REFINE_WIN_W + 2] - subpix_ptr[j - REFINE_WIN_W - 2];\n\
                float gxx = tgx * tgx * m;\n\
                float gxy = tgx * tgy * m;\n\
                float gyy = tgy * tgy * m;\n\
                float px = j - REFINE_WIN_HALF_W;\n\
\n\
                a += gxx;\n\
                b += gxy;\n\
                c += gyy;\n\
\n\
                bb1 += gxx * px + gxy * py;\n\
                bb2 += gxy * px + gyy * py;\n\
            }\n\
        }\n\
\n\
        float det = a * c - b * b;\n\
        if (fabs(det) <= flt_epsilon * flt_epsilon) {\n\
            break;\n\
        }\n\
\n\
        // 2x2 matrix inversion\n\
        float scale = 1.0f / det;\n\
        feature_tmp.x = (float)(feature.x + (c * scale * bb1) - (b * scale * bb2));\n\
        feature_tmp.y = (float)(feature.y - (b * scale * bb1) + (a * scale * bb2));\n\
        err = dot(feature_tmp - feature, feature_tmp - feature);\n\
\n\
        feature = feature_tmp;\n\
        if (feature.x < 0 || feature.x >= src_width || feature.y < 0 || feature.y >= src_height) {\n\
            break;\n\
        }\n\
    } while (++iter < max_iters && err > eps);\n\
\n\
    // Make sure new point isn't too far from the initial point (indicates poor convergence)\n\
    if (fabs(feature.x - init.x) > REFINE_WIN_HALF_W || fabs(feature.y - init.y) > REFINE_WIN_HALF_H) {\n\
        feature = init;\n\
    }\n\
\n\
    return feature;\n\
}\n\
\n\
// Performs non-maximum suppression on the harris response and writes the resulting\n\
// feature locations to refined_features.\n\
//\n\
// Assumes that refined_features and the global work sizes are set up such that the image\n\
// is split up into a grid of 32x32 blocks where each block has a single slot in the\n\
// refined_features buffer. This kernel finds the best corner in each block (if the\n\
// block has any) and writes it to the corresponding slot in the buffer.\n\
//\n\
// If subpixel_refine is true, the features are additionally refined at a sub-pixel\n\
// level for increased precision.\n\
__kernel void refine_features(\n\
    __read_only image2d_t grayscale,\n\
    __global const float *harris_buf,\n\
    __global float2 *refined_features,\n\
    int subpixel_refine\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    // The location in the grayscale buffer rather than the compacted grid\n\
    int2 loc_i = (int2)(loc.x * 32, loc.y * 32);\n\
\n\
    float new_val;\n\
    float max_val = 0;\n\
    float2 loc_max = (float2)(-1, -1);\n\
\n\
    int end_x = min(loc_i.x + 32, (int)get_image_dim(grayscale).x - 1);\n\
    int end_y = min(loc_i.y + 32, (int)get_image_dim(grayscale).y - 1);\n\
\n\
    for (int i = loc_i.x; i < end_x; ++i) {\n\
        for (int j = loc_i.y; j < end_y; ++j) {\n\
            new_val = harris_buf[i + j * get_image_dim(grayscale).x];\n\
\n\
            if (new_val > max_val) {\n\
                max_val = new_val;\n\
                loc_max = (float2)(i, j);\n\
            }\n\
        }\n\
    }\n\
\n\
    if (max_val == 0) {\n\
        // There are no features in this part of the frame\n\
        write_to_1d_arrf2(refined_features, loc, loc_max);\n\
        return;\n\
    }\n\
\n\
    if (subpixel_refine) {\n\
        float mask[REFINE_WIN_H * REFINE_WIN_W];\n\
        for (int i = 0; i < REFINE_WIN_H; i++) {\n\
            float y = (float)(i - REFINE_WIN_HALF_H) / REFINE_WIN_HALF_H;\n\
            float vy = exp(-y * y);\n\
\n\
            for (int j = 0; j < REFINE_WIN_W; j++) {\n\
                float x = (float)(j - REFINE_WIN_HALF_W) / REFINE_WIN_HALF_W;\n\
                mask[i * REFINE_WIN_W + j] = (float)(vy * exp(-x * x));\n\
            }\n\
        }\n\
\n\
        loc_max = corner_sub_pix(grayscale, loc_max, mask);\n\
    }\n\
\n\
    write_to_1d_arrf2(refined_features, loc, loc_max);\n\
}\n\
\n\
// Extracts BRIEF descriptors from the grayscale src image for the given features\n\
// using the provided sampler.\n\
__kernel void brief_descriptors(\n\
    __read_only image2d_t grayscale,\n\
    __global const float2 *refined_features,\n\
    // for 512 bit descriptors\n\
    __global ulong8 *desc_buf,\n\
    __global const PointPair *brief_pattern\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    float2 feature = read_from_1d_arrf2(refined_features, loc);\n\
\n\
    // There was no feature in this part of the frame\n\
    if (feature.x == -1) {\n\
        write_to_1d_arrul8(desc_buf, loc, (ulong8)(0));\n\
        return;\n\
    }\n\
\n\
    ulong8 desc = 0;\n\
    ulong *p = &desc;\n\
\n\
    for (int i = 0; i < 8; ++i) {\n\
        for (int j = 0; j < 64; ++j) {\n\
            PointPair pair = brief_pattern[j * (i + 1)];\n\
            float l1 = read_imagef(grayscale, sampler_linear, feature + pair.p1).x;\n\
            float l2 = read_imagef(grayscale, sampler_linear, feature + pair.p2).x;\n\
\n\
            if (l1 < l2) {\n\
                p[i] |= 1UL << j;\n\
            }\n\
        }\n\
    }\n\
\n\
    write_to_1d_arrul8(desc_buf, loc, desc);\n\
}\n\
\n\
// Given buffers with descriptors for the current and previous frame, determines\n\
// which ones match, writing correspondences to matches_buf.\n\
//\n\
// Feature and descriptor buffers are assumed to be compacted (each element sourced\n\
// from a 32x32 block in the frame being processed).\n\
__kernel void match_descriptors(\n\
    __global const float2 *prev_refined_features,\n\
    __global const float2 *refined_features,\n\
    __global const ulong8 *desc_buf,\n\
    __global const ulong8 *prev_desc_buf,\n\
    __global MotionVector *matches_buf\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    ulong8 desc = read_from_1d_arrul8(desc_buf, loc);\n\
    const int search_radius = 3;\n\
\n\
    MotionVector invalid_vector = (MotionVector) {\n\
        (PointPair) {\n\
            (float2)(-1, -1),\n\
            (float2)(-1, -1)\n\
        },\n\
        0\n\
    };\n\
\n\
    if (desc.s0 == 0 && desc.s1 == 0) {\n\
        // There was no feature in this part of the frame\n\
        write_to_1d_arrvec(\n\
            matches_buf,\n\
            loc,\n\
            invalid_vector\n\
        );\n\
        return;\n\
    }\n\
\n\
    int2 start = max(loc - search_radius, 0);\n\
    int2 end = min(loc + search_radius, (int2)(get_global_size(0) - 1, get_global_size(1) - 1));\n\
\n\
    for (int i = start.x; i < end.x; ++i) {\n\
        for (int j = start.y; j < end.y; ++j) {\n\
            int2 prev_point = (int2)(i, j);\n\
            int total_dist = 0;\n\
\n\
            ulong8 prev_desc = read_from_1d_arrul8(prev_desc_buf, prev_point);\n\
\n\
            if (prev_desc.s0 == 0 && prev_desc.s1 == 0) {\n\
                continue;\n\
            }\n\
\n\
            ulong *prev_desc_p = &prev_desc;\n\
            ulong *desc_p = &desc;\n\
\n\
            for (int i = 0; i < 8; i++) {\n\
                total_dist += popcount(desc_p[i] ^ prev_desc_p[i]);\n\
            }\n\
\n\
            if (total_dist < DISTANCE_THRESHOLD) {\n\
                write_to_1d_arrvec(\n\
                    matches_buf,\n\
                    loc,\n\
                    (MotionVector) {\n\
                        (PointPair) {\n\
                            read_from_1d_arrf2(prev_refined_features, prev_point),\n\
                            read_from_1d_arrf2(refined_features, loc)\n\
                        },\n\
                        1\n\
                    }\n\
                );\n\
\n\
                return;\n\
            }\n\
        }\n\
    }\n\
\n\
    // There is no found match for this point\n\
    write_to_1d_arrvec(\n\
        matches_buf,\n\
        loc,\n\
        invalid_vector\n\
    );\n\
}\n\
\n\
// Returns the position of the given point after the transform is applied\n\
static float2 transformed_point(float2 p, __global const float *transform) {\n\
    float2 ret;\n\
\n\
    ret.x = p.x * transform[0] + p.y * transform[1] + transform[2];\n\
    ret.y = p.x * transform[3] + p.y * transform[4] + transform[5];\n\
\n\
    return ret;\n\
}\n\
\n\
// Performs the given transform on the src image\n\
__kernel void transform(\n\
    __read_only image2d_t src,\n\
    __write_only image2d_t dst,\n\
    __global const float *transform\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    float2 norm = convert_float2(get_image_dim(src));\n\
\n\
    write_imagef(\n\
        dst,\n\
        loc,\n\
        read_imagef(\n\
            src,\n\
            sampler_linear_mirror,\n\
            transformed_point((float2)(loc.x, loc.y), transform) / norm\n\
        )\n\
    );\n\
}\n\
\n\
// Returns the new location of the given point using the given crop bounding box\n\
// and the width and height of the original frame.\n\
static float2 cropped_point(\n\
    float2 p,\n\
    float2 top_left,\n\
    float2 bottom_right,\n\
    int2 orig_dim\n\
) {\n\
    float2 ret;\n\
\n\
    float crop_width  = bottom_right.x - top_left.x;\n\
    float crop_height = bottom_right.y - top_left.y;\n\
\n\
    float width_norm = p.x / (float)orig_dim.x;\n\
    float height_norm = p.y / (float)orig_dim.y;\n\
\n\
    ret.x = (width_norm * crop_width) + top_left.x;\n\
    ret.y = (height_norm * crop_height) + ((float)orig_dim.y - bottom_right.y);\n\
\n\
    return ret;\n\
}\n\
\n\
// Upscales the given cropped region to the size of the original frame\n\
__kernel void crop_upscale(\n\
    __read_only image2d_t src,\n\
    __write_only image2d_t dst,\n\
    float2 top_left,\n\
    float2 bottom_right\n\
) {\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    write_imagef(\n\
        dst,\n\
        loc,\n\
        read_imagef(\n\
            src,\n\
            sampler_linear,\n\
            cropped_point((float2)(loc.x, loc.y), top_left, bottom_right, get_image_dim(dst))\n\
        )\n\
    );\n\
}\n\
\n\
// Draws boxes to represent the given point matches and uses the given transform\n\
// and crop info to make sure their positions are accurate on the transformed frame.\n\
//\n\
// model_matches is an array of three points that were used by the RANSAC process\n\
// to generate the given transform\n\
__kernel void draw_debug_info(\n\
    __write_only image2d_t dst,\n\
    __global const MotionVector *matches,\n\
    __global const MotionVector *model_matches,\n\
    int num_model_matches,\n\
    __global const float *transform\n\
) {\n\
    int loc = get_global_id(0);\n\
    MotionVector vec = matches[loc];\n\
    // Black box: matched point that RANSAC considered an outlier\n\
    float4 big_rect_color = (float4)(0.1f, 0.1f, 0.1f, 1.0f);\n\
\n\
    if (vec.should_consider) {\n\
        // Green box: matched point that RANSAC considered an inlier\n\
        big_rect_color = (float4)(0.0f, 1.0f, 0.0f, 1.0f);\n\
    }\n\
\n\
    for (int i = 0; i < num_model_matches; i++) {\n\
        if (vec.p.p2.x == model_matches[i].p.p2.x && vec.p.p2.y == model_matches[i].p.p2.y) {\n\
            // Orange box: point used to calculate model\n\
            big_rect_color = (float4)(1.0f, 0.5f, 0.0f, 1.0f);\n\
        }\n\
    }\n\
\n\
    float2 transformed_p1 = transformed_point(vec.p.p1, transform);\n\
    float2 transformed_p2 = transformed_point(vec.p.p2, transform);\n\
\n\
    draw_box(dst, (int2)(transformed_p2.x, transformed_p2.y), big_rect_color, 5);\n\
    // Small light blue box: the point in the previous frame\n\
    draw_box(dst, (int2)(transformed_p1.x, transformed_p1.y), (float4)(0.0f, 0.3f, 0.7f, 1.0f), 3);\n\
}";

const char *ff_source_neighbor_cl = "\
__kernel void erosion_global(__write_only image2d_t dst,\n\
                             __read_only  image2d_t src,\n\
                             float threshold,\n\
                             __constant int *coord)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 px = read_imagef(src, sampler, loc);\n\
    float limit = px.x - threshold;\n\
    if (limit < 0) {\n\
        limit = 0;\n\
    }\n\
\n\
    for (int i = -1; i <= 1; i++) {\n\
        for (int j = -1; j <= 1; j++) {\n\
            if (coord[(j + 1) * 3 + (i + 1)] == 1) {\n\
                float4 cur = read_imagef(src, sampler, loc + (int2)(i, j));\n\
                if (cur.x < px.x) {\n\
                    px = cur;\n\
                }\n\
            }\n\
        }\n\
    }\n\
    if (limit > px.x) {\n\
        px = (float4)(limit);\n\
    }\n\
    write_imagef(dst, loc, px);\n\
}\n\
\n\
__kernel void dilation_global(__write_only image2d_t dst,\n\
                              __read_only  image2d_t src,\n\
                              float threshold,\n\
                              __constant int *coord)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 px = read_imagef(src, sampler, loc);\n\
    float limit = px.x + threshold;\n\
    if (limit > 1) {\n\
        limit = 1;\n\
    }\n\
\n\
    for (int i = -1; i <= 1; i++) {\n\
        for (int j = -1; j <= 1; j++) {\n\
            if (coord[(j + 1) * 3 + (i + 1)] == 1) {\n\
                float4 cur = read_imagef(src, sampler, loc + (int2)(i, j));\n\
                if (cur.x > px.x) {\n\
                    px = cur;\n\
                }\n\
            }\n\
        }\n\
    }\n\
    if (limit < px.x) {\n\
        px = (float4)(limit);\n\
    }\n\
    write_imagef(dst, loc, px);\n\
}";

const char *ff_source_nlmeans_cl = "\
const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                           CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                           CLK_FILTER_NEAREST);\n\
\n\
__kernel void horiz_sum(__global uint4 *integral_img,\n\
                        __read_only image2d_t src,\n\
                        int width,\n\
                        int height,\n\
                        int4 dx,\n\
                        int4 dy)\n\
{\n\
\n\
    int y = get_global_id(0);\n\
    int work_size = get_global_size(0);\n\
\n\
    uint4 sum = (uint4)(0);\n\
    float4 s2;\n\
    for (int i = 0; i < width; i++) {\n\
        float s1 = read_imagef(src, sampler, (int2)(i, y)).x;\n\
        s2.x = read_imagef(src, sampler, (int2)(i + dx.x, y + dy.x)).x;\n\
        s2.y = read_imagef(src, sampler, (int2)(i + dx.y, y + dy.y)).x;\n\
        s2.z = read_imagef(src, sampler, (int2)(i + dx.z, y + dy.z)).x;\n\
        s2.w = read_imagef(src, sampler, (int2)(i + dx.w, y + dy.w)).x;\n\
        sum += convert_uint4((s1 - s2) * (s1 - s2) * 255 * 255);\n\
        integral_img[y * width + i] = sum;\n\
    }\n\
}\n\
\n\
__kernel void vert_sum(__global uint4 *integral_img,\n\
                     __global int *overflow,\n\
                     int width,\n\
                     int height)\n\
{\n\
    int x = get_global_id(0);\n\
    uint4 sum = 0;\n\
    for (int i = 0; i < height; i++) {\n\
        if (any((uint4)UINT_MAX - integral_img[i * width + x] < sum))\n\
            atomic_inc(overflow);\n\
        integral_img[i * width + x] += sum;\n\
        sum = integral_img[i * width + x];\n\
    }\n\
}\n\
\n\
__kernel void weight_accum(__global float *sum, __global float *weight,\n\
                           __global uint4 *integral_img, __read_only image2d_t src,\n\
                           int width, int height, int p, float h,\n\
                           int4 dx, int4 dy)\n\
{\n\
    // w(x) = integral_img(x-p, y-p) +\n\
    //        integral_img(x+p, y+p) -\n\
    //        integral_img(x+p, y-p) -\n\
    //        integral_img(x-p, y+p)\n\
    // total_sum[x] += w(x, y) * src(x + dx, y + dy)\n\
    // total_weight += w(x, y)\n\
\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
    int4 xoff = x + dx;\n\
    int4 yoff = y + dy;\n\
    uint4 a = 0, b = 0, c = 0, d = 0;\n\
    uint4 src_pix = 0;\n\
\n\
    // out-of-bounding-box?\n\
    int oobb = (x - p) < 0 || (y - p) < 0 || (y + p) >= height || (x + p) >= width;\n\
\n\
    src_pix.x = (int)(255 * read_imagef(src, sampler, (int2)(xoff.x, yoff.x)).x);\n\
    src_pix.y = (int)(255 * read_imagef(src, sampler, (int2)(xoff.y, yoff.y)).x);\n\
    src_pix.z = (int)(255 * read_imagef(src, sampler, (int2)(xoff.z, yoff.z)).x);\n\
    src_pix.w = (int)(255 * read_imagef(src, sampler, (int2)(xoff.w, yoff.w)).x);\n\
    if (!oobb) {\n\
        a = integral_img[(y - p) * width + x - p];\n\
        b = integral_img[(y + p) * width + x - p];\n\
        c = integral_img[(y - p) * width + x + p];\n\
        d = integral_img[(y + p) * width + x + p];\n\
    }\n\
\n\
    float4 patch_diff = convert_float4(d + a - c - b);\n\
    float4 w = native_exp(-patch_diff / (h * h));\n\
    float w_sum = w.x + w.y + w.z + w.w;\n\
    weight[y * width + x] += w_sum;\n\
    sum[y * width + x] += dot(w, convert_float4(src_pix));\n\
}\n\
\n\
__kernel void average(__write_only image2d_t dst,\n\
                      __read_only image2d_t src,\n\
                      __global float *sum, __global float *weight) {\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
    int2 dim = get_image_dim(dst);\n\
\n\
    float w = weight[y * dim.x + x];\n\
    float s = sum[y * dim.x + x];\n\
    float src_pix = read_imagef(src, sampler, (int2)(x, y)).x;\n\
    float r = (s + src_pix * 255) / (1.0f + w) / 255.0f;\n\
    if (x < dim.x && y < dim.y)\n\
        write_imagef(dst, (int2)(x, y), (float4)(r, 0.0f, 0.0f, 1.0f));\n\
}";

const char *ff_source_overlay_cl = "\
__kernel void overlay_no_alpha(__write_only image2d_t dst,\n\
                               __read_only  image2d_t main,\n\
                               __read_only  image2d_t overlay,\n\
                               int x_position,\n\
                               int y_position)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 overlay_size = get_image_dim(overlay);\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    if (loc.x <  x_position ||\n\
        loc.y <  y_position ||\n\
        loc.x >= overlay_size.x + x_position ||\n\
        loc.y >= overlay_size.y + y_position) {\n\
        float4 val = read_imagef(main, sampler, loc);\n\
        write_imagef(dst, loc, val);\n\
    } else {\n\
        int2 loc_overlay = (int2)(x_position, y_position);\n\
        float4 val       = read_imagef(overlay, sampler, loc - loc_overlay);\n\
        write_imagef(dst, loc, val);\n\
    }\n\
}\n\
\n\
__kernel void overlay_internal_alpha(__write_only image2d_t dst,\n\
                                     __read_only  image2d_t main,\n\
                                     __read_only  image2d_t overlay,\n\
                                     int x_position,\n\
                                     int y_position)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 overlay_size = get_image_dim(overlay);\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    if (loc.x <  x_position ||\n\
        loc.y <  y_position ||\n\
        loc.x >= overlay_size.x + x_position ||\n\
        loc.y >= overlay_size.y + y_position) {\n\
        float4 val = read_imagef(main, sampler, loc);\n\
        write_imagef(dst, loc, val);\n\
    } else {\n\
        int2 loc_overlay  = (int2)(x_position, y_position);\n\
        float4 in_main    = read_imagef(main,    sampler, loc);\n\
        float4 in_overlay = read_imagef(overlay, sampler, loc - loc_overlay);\n\
        float4 val        = in_overlay * in_overlay.w + in_main * (1.0f - in_overlay.w);\n\
        write_imagef(dst, loc, val);\n\
    }\n\
}\n\
\n\
__kernel void overlay_external_alpha(__write_only image2d_t dst,\n\
                                     __read_only  image2d_t main,\n\
                                     __read_only  image2d_t overlay,\n\
                                     __read_only  image2d_t alpha,\n\
                                     int x_position,\n\
                                     int y_position,\n\
                                     int alpha_adj_x,\n\
                                     int alpha_adj_y)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 overlay_size = get_image_dim(overlay);\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    if (loc.x <  x_position ||\n\
        loc.y <  y_position ||\n\
        loc.x >= overlay_size.x + x_position ||\n\
        loc.y >= overlay_size.y + y_position) {\n\
        float4 val = read_imagef(main, sampler, loc);\n\
        write_imagef(dst, loc, val);\n\
    } else {\n\
        int2 loc_overlay  = (int2)(x_position, y_position);\n\
        float4 in_main    = read_imagef(main,    sampler, loc);\n\
        float4 in_overlay = read_imagef(overlay, sampler, loc - loc_overlay);\n\
\n\
        int2 loc_alpha    = (int2)(loc.x * alpha_adj_x,\n\
                                   loc.y * alpha_adj_y) - loc_overlay;\n\
        float4 in_alpha   = read_imagef(alpha,   sampler, loc_alpha);\n\
\n\
        float4 val = in_overlay * in_alpha.x + in_main * (1.0f - in_alpha.x);\n\
        write_imagef(dst, loc, val);\n\
    }\n\
}";

const char *ff_source_pad_cl = "\
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |\n\
                          CLK_FILTER_NEAREST;\n\
\n\
__kernel void pad (\n\
    __read_only  image2d_t src,\n\
    __write_only image2d_t dst,\n\
    float4 color,\n\
    int2 xy)\n\
{\n\
    int2 size_src = get_image_dim(src);\n\
    int2 loc = (int2)(get_global_id(0), get_global_id(1));\n\
    int2 src_pos = (int2)(get_global_id(0) - xy.x, get_global_id(1) - xy.y);\n\
    float4 pixel = loc.x >= size_src.x + xy.x ||\n\
                   loc.y >= size_src.y + xy.y ||\n\
                   loc.x < xy.x ||\n\
                   loc.y < xy.y ? color : read_imagef(src, sampler, src_pos);\n\
    write_imagef(dst, loc, pixel);\n\
}";

const char *ff_source_remap_cl = "\
const sampler_t linear_sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                                  CLK_FILTER_LINEAR);\n\
\n\
const sampler_t nearest_sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                                   CLK_FILTER_NEAREST);\n\
\n\
__kernel void remap_near(__write_only image2d_t dst,\n\
                         __read_only  image2d_t src,\n\
                         __read_only  image2d_t xmapi,\n\
                         __read_only  image2d_t ymapi,\n\
                         float4 fill_color)\n\
{\n\
    int2 p = (int2)(get_global_id(0), get_global_id(1));\n\
    int2 dimi = get_image_dim(src);\n\
    float2 dimf = (float2)(dimi.x, dimi.y);\n\
    float4 val;\n\
    int2 mi;\n\
    float m;\n\
\n\
    float4 xmap = read_imagef(xmapi, nearest_sampler, p);\n\
    float4 ymap = read_imagef(ymapi, nearest_sampler, p);\n\
    float2 pos  = (float2)(xmap.x, ymap.x);\n\
    pos.xy = pos.xy * 65535.f;\n\
\n\
    mi = ((pos >= (float2)(0.f, 0.f)) * (pos < dimf) * (p <= dimi));\n\
    m = mi.x && mi.y;\n\
    val = mix(fill_color, read_imagef(src, nearest_sampler, pos), m);\n\
\n\
    write_imagef(dst, p, val);\n\
}\n\
\n\
__kernel void remap_linear(__write_only image2d_t dst,\n\
                           __read_only  image2d_t src,\n\
                           __read_only  image2d_t xmapi,\n\
                           __read_only  image2d_t ymapi,\n\
                           float4 fill_color)\n\
{\n\
    int2 p = (int2)(get_global_id(0), get_global_id(1));\n\
    int2 dimi = get_image_dim(src);\n\
    float2 dimf = (float2)(dimi.x, dimi.y);\n\
    float4 val;\n\
    int2 mi;\n\
    float m;\n\
\n\
    float4 xmap = read_imagef(xmapi, nearest_sampler, p);\n\
    float4 ymap = read_imagef(ymapi, nearest_sampler, p);\n\
    float2 pos  = (float2)(xmap.x, ymap.x);\n\
    pos.xy = pos.xy * 65535.f;\n\
\n\
    mi = ((pos >= (float2)(0.f, 0.f)) * (pos < dimf) * (p <= dimi));\n\
    m = mi.x && mi.y;\n\
    val = mix(fill_color, read_imagef(src, linear_sampler, pos), m);\n\
\n\
    write_imagef(dst, p, val);\n\
}";

const char *ff_source_tonemap_cl = "\
#define REFERENCE_WHITE 100.0f\n\
extern float3 lrgb2yuv(float3);\n\
extern float  lrgb2y(float3);\n\
extern float3 yuv2lrgb(float3);\n\
extern float3 lrgb2lrgb(float3);\n\
extern float  get_luma_src(float3);\n\
extern float  get_luma_dst(float3);\n\
extern float3 ootf(float3 c, float peak);\n\
extern float3 inverse_ootf(float3 c, float peak);\n\
extern float3 get_chroma_sample(float3, float3, float3, float3);\n\
\n\
struct detection_result {\n\
    float peak;\n\
    float average;\n\
};\n\
\n\
float hable_f(float in) {\n\
    float a = 0.15f, b = 0.50f, c = 0.10f, d = 0.20f, e = 0.02f, f = 0.30f;\n\
    return (in * (in * a + b * c) + d * e) / (in * (in * a + b) + d * f) - e / f;\n\
}\n\
\n\
float direct(float s, float peak) {\n\
    return s;\n\
}\n\
\n\
float linear(float s, float peak) {\n\
    return s * tone_param / peak;\n\
}\n\
\n\
float gamma(float s, float peak) {\n\
    float p = s > 0.05f ? s /peak : 0.05f / peak;\n\
    float v = powr(p, 1.0f / tone_param);\n\
    return s > 0.05f ? v : (s * v /0.05f);\n\
}\n\
\n\
float clip(float s, float peak) {\n\
    return clamp(s * tone_param, 0.0f, 1.0f);\n\
}\n\
\n\
float reinhard(float s, float peak) {\n\
    return s / (s + tone_param) * (peak + tone_param) / peak;\n\
}\n\
\n\
float hable(float s, float peak) {\n\
    return hable_f(s)/hable_f(peak);\n\
}\n\
\n\
float mobius(float s, float peak) {\n\
    float j = tone_param;\n\
    float a, b;\n\
\n\
    if (s <= j)\n\
        return s;\n\
\n\
    a = -j * j * (peak - 1.0f) / (j * j - 2.0f * j + peak);\n\
    b = (j * j - 2.0f * j * peak + peak) / max(peak - 1.0f, 1e-6f);\n\
\n\
    return (b * b + 2.0f * b * j + j * j) / (b - a) * (s + a) / (s + b);\n\
}\n\
\n\
// detect peak/average signal of a frame, the algorithm was ported from:\n\
// libplacebo (https://github.com/haasn/libplacebo)\n\
struct detection_result\n\
detect_peak_avg(__global uint *util_buf, __local uint *sum_wg,\n\
            float signal, float peak) {\n\
// layout of the util buffer\n\
//\n\
// Name:             : Size (units of 4-bytes)\n\
// average buffer    : detection_frames + 1\n\
// peak buffer       : detection_frames + 1\n\
// workgroup counter : 1\n\
// total of peak     : 1\n\
// total of average  : 1\n\
// frame index       : 1\n\
// frame number      : 1\n\
    __global uint *avg_buf = util_buf;\n\
    __global uint *peak_buf = avg_buf + DETECTION_FRAMES + 1;\n\
    __global uint *counter_wg_p = peak_buf + DETECTION_FRAMES + 1;\n\
    __global uint *max_total_p = counter_wg_p + 1;\n\
    __global uint *avg_total_p = max_total_p + 1;\n\
    __global uint *frame_idx_p = avg_total_p + 1;\n\
    __global uint *scene_frame_num_p = frame_idx_p + 1;\n\
\n\
    uint frame_idx = *frame_idx_p;\n\
    uint scene_frame_num = *scene_frame_num_p;\n\
\n\
    size_t lidx = get_local_id(0);\n\
    size_t lidy = get_local_id(1);\n\
    size_t lsizex = get_local_size(0);\n\
    size_t lsizey = get_local_size(1);\n\
    uint num_wg = get_num_groups(0) * get_num_groups(1);\n\
    size_t group_idx = get_group_id(0);\n\
    size_t group_idy = get_group_id(1);\n\
    struct detection_result r = {peak, sdr_avg};\n\
    if (lidx == 0 && lidy == 0)\n\
        *sum_wg = 0;\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    // update workgroup sum\n\
    atomic_add(sum_wg, (uint)(signal * REFERENCE_WHITE));\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    // update frame peak/avg using work-group-average.\n\
    if (lidx == 0 && lidy == 0) {\n\
        uint avg_wg = *sum_wg / (lsizex * lsizey);\n\
        atomic_max(&peak_buf[frame_idx], avg_wg);\n\
        atomic_add(&avg_buf[frame_idx], avg_wg);\n\
    }\n\
\n\
    if (scene_frame_num > 0) {\n\
        float peak = (float)*max_total_p / (REFERENCE_WHITE * scene_frame_num);\n\
        float avg = (float)*avg_total_p / (REFERENCE_WHITE * scene_frame_num);\n\
        r.peak = max(1.0f, peak);\n\
        r.average = max(0.25f, avg);\n\
    }\n\
\n\
    if (lidx == 0 && lidy == 0 && atomic_add(counter_wg_p, 1) == num_wg - 1) {\n\
        *counter_wg_p = 0;\n\
        avg_buf[frame_idx] /= num_wg;\n\
\n\
        if (scene_threshold > 0.0f) {\n\
            uint cur_max = peak_buf[frame_idx];\n\
            uint cur_avg = avg_buf[frame_idx];\n\
            int diff = (int)(scene_frame_num * cur_avg) - (int)*avg_total_p;\n\
\n\
            if (abs(diff) > scene_frame_num * scene_threshold * REFERENCE_WHITE) {\n\
                for (uint i = 0; i < DETECTION_FRAMES + 1; i++)\n\
                  avg_buf[i] = 0;\n\
                for (uint i = 0; i < DETECTION_FRAMES + 1; i++)\n\
                  peak_buf[i] = 0;\n\
                *avg_total_p = *max_total_p = 0;\n\
                *scene_frame_num_p = 0;\n\
                avg_buf[frame_idx] = cur_avg;\n\
                peak_buf[frame_idx] = cur_max;\n\
            }\n\
        }\n\
        uint next = (frame_idx + 1) % (DETECTION_FRAMES + 1);\n\
        // add current frame, subtract next frame\n\
        *max_total_p += peak_buf[frame_idx] - peak_buf[next];\n\
        *avg_total_p += avg_buf[frame_idx] - avg_buf[next];\n\
        // reset next frame\n\
        peak_buf[next] = avg_buf[next] = 0;\n\
        *frame_idx_p = next;\n\
        *scene_frame_num_p = min(*scene_frame_num_p + 1,\n\
                                 (uint)DETECTION_FRAMES);\n\
    }\n\
    return r;\n\
}\n\
\n\
float3 map_one_pixel_rgb(float3 rgb, float peak, float average) {\n\
    float sig = max(max(rgb.x, max(rgb.y, rgb.z)), 1e-6f);\n\
\n\
    // Rescale the variables in order to bring it into a representation where\n\
    // 1.0 represents the dst_peak. This is because all of the tone mapping\n\
    // algorithms are defined in such a way that they map to the range [0.0, 1.0].\n\
    if (target_peak > 1.0f) {\n\
        sig *= 1.0f / target_peak;\n\
        peak *= 1.0f / target_peak;\n\
    }\n\
\n\
    float sig_old = sig;\n\
\n\
    // Scale the signal to compensate for differences in the average brightness\n\
    float slope = min(1.0f, sdr_avg / average);\n\
    sig *= slope;\n\
    peak *= slope;\n\
\n\
    // Desaturate the color using a coefficient dependent on the signal level\n\
    if (desat_param > 0.0f) {\n\
        float luma = get_luma_dst(rgb);\n\
        float coeff = max(sig - 0.18f, 1e-6f) / max(sig, 1e-6f);\n\
        coeff = native_powr(coeff, 10.0f / desat_param);\n\
        rgb = mix(rgb, (float3)luma, (float3)coeff);\n\
        sig = mix(sig, luma * slope, coeff);\n\
    }\n\
\n\
    sig = TONE_FUNC(sig, peak);\n\
\n\
    sig = min(sig, 1.0f);\n\
    rgb *= (sig/sig_old);\n\
    return rgb;\n\
}\n\
\n\
// map from source space YUV to destination space RGB\n\
float3 map_to_dst_space_from_yuv(float3 yuv, float peak) {\n\
    float3 c = yuv2lrgb(yuv);\n\
    c = ootf(c, peak);\n\
    c = lrgb2lrgb(c);\n\
    return c;\n\
}\n\
\n\
__kernel void tonemap(__write_only image2d_t dst1,\n\
                      __read_only  image2d_t src1,\n\
                      __write_only image2d_t dst2,\n\
                      __read_only  image2d_t src2,\n\
                      __global uint *util_buf,\n\
                      float peak\n\
                      )\n\
{\n\
    __local uint sum_wg;\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\n\
    int xi = get_global_id(0);\n\
    int yi = get_global_id(1);\n\
    // each work item process four pixels\n\
    int x = 2 * xi;\n\
    int y = 2 * yi;\n\
\n\
    float y0 = read_imagef(src1, sampler, (int2)(x,     y)).x;\n\
    float y1 = read_imagef(src1, sampler, (int2)(x + 1, y)).x;\n\
    float y2 = read_imagef(src1, sampler, (int2)(x,     y + 1)).x;\n\
    float y3 = read_imagef(src1, sampler, (int2)(x + 1, y + 1)).x;\n\
    float2 uv = read_imagef(src2, sampler, (int2)(xi,     yi)).xy;\n\
\n\
    float3 c0 = map_to_dst_space_from_yuv((float3)(y0, uv.x, uv.y), peak);\n\
    float3 c1 = map_to_dst_space_from_yuv((float3)(y1, uv.x, uv.y), peak);\n\
    float3 c2 = map_to_dst_space_from_yuv((float3)(y2, uv.x, uv.y), peak);\n\
    float3 c3 = map_to_dst_space_from_yuv((float3)(y3, uv.x, uv.y), peak);\n\
\n\
    float sig0 = max(c0.x, max(c0.y, c0.z));\n\
    float sig1 = max(c1.x, max(c1.y, c1.z));\n\
    float sig2 = max(c2.x, max(c2.y, c2.z));\n\
    float sig3 = max(c3.x, max(c3.y, c3.z));\n\
    float sig = max(sig0, max(sig1, max(sig2, sig3)));\n\
\n\
    struct detection_result r = detect_peak_avg(util_buf, &sum_wg, sig, peak);\n\
\n\
    float3 c0_old = c0, c1_old = c1, c2_old = c2;\n\
    c0 = map_one_pixel_rgb(c0, r.peak, r.average);\n\
    c1 = map_one_pixel_rgb(c1, r.peak, r.average);\n\
    c2 = map_one_pixel_rgb(c2, r.peak, r.average);\n\
    c3 = map_one_pixel_rgb(c3, r.peak, r.average);\n\
\n\
    c0 = inverse_ootf(c0, target_peak);\n\
    c1 = inverse_ootf(c1, target_peak);\n\
    c2 = inverse_ootf(c2, target_peak);\n\
    c3 = inverse_ootf(c3, target_peak);\n\
\n\
    y0 = lrgb2y(c0);\n\
    y1 = lrgb2y(c1);\n\
    y2 = lrgb2y(c2);\n\
    y3 = lrgb2y(c3);\n\
    float3 chroma_c = get_chroma_sample(c0, c1, c2, c3);\n\
    float3 chroma = lrgb2yuv(chroma_c);\n\
\n\
    if (xi < get_image_width(dst2) && yi < get_image_height(dst2)) {\n\
        write_imagef(dst1, (int2)(x, y), (float4)(y0, 0.0f, 0.0f, 1.0f));\n\
        write_imagef(dst1, (int2)(x+1, y), (float4)(y1, 0.0f, 0.0f, 1.0f));\n\
        write_imagef(dst1, (int2)(x, y+1), (float4)(y2, 0.0f, 0.0f, 1.0f));\n\
        write_imagef(dst1, (int2)(x+1, y+1), (float4)(y3, 0.0f, 0.0f, 1.0f));\n\
        write_imagef(dst2, (int2)(xi, yi),\n\
                     (float4)(chroma.y, chroma.z, 0.0f, 1.0f));\n\
    }\n\
}";

const char *ff_source_transpose_cl = "\
__kernel void transpose(__write_only image2d_t dst,\n\
                        __read_only image2d_t src,\n\
                        int dir) {\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE   |\n\
                               CLK_FILTER_NEAREST);\n\
\n\
    int2 size = get_image_dim(dst);\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
\n\
    int xin = (dir & 2) ? (size.y - 1 - y) : y;\n\
    int yin = (dir & 1) ? (size.x - 1 - x) : x;\n\
    float4 data = read_imagef(src, sampler, (int2)(xin, yin));\n\
\n\
    if (x < size.x && y < size.y)\n\
        write_imagef(dst, (int2)(x, y), data);\n\
}";

const char *ff_source_unsharp_cl = "\
__kernel void unsharp_global(__write_only image2d_t dst,\n\
                             __read_only  image2d_t src,\n\
                             int size_x,\n\
                             int size_y,\n\
                             float amount,\n\
                             __constant float *coef_matrix)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_FILTER_NEAREST);\n\
    int2 loc    = (int2)(get_global_id(0), get_global_id(1));\n\
    int2 centre = (int2)(size_x / 2, size_y / 2);\n\
\n\
    float4 val = read_imagef(src, sampler, loc);\n\
    float4 sum = 0.0f;\n\
    int x, y;\n\
\n\
    for (y = 0; y < size_y; y++) {\n\
        for (x = 0; x < size_x; x++) {\n\
            int2 pos = loc + (int2)(x, y) - centre;\n\
            sum += coef_matrix[y * size_x + x] *\n\
                read_imagef(src, sampler, pos);\n\
        }\n\
    }\n\
\n\
    write_imagef(dst, loc, val + (val - sum) * amount);\n\
}\n\
\n\
__kernel void unsharp_local(__write_only image2d_t dst,\n\
                            __read_only  image2d_t src,\n\
                            int size_x,\n\
                            int size_y,\n\
                            float amount,\n\
                            __constant float *coef_x,\n\
                            __constant float *coef_y)\n\
{\n\
    const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                               CLK_ADDRESS_CLAMP_TO_EDGE |\n\
                               CLK_FILTER_NEAREST);\n\
    int2 block = (int2)(get_group_id(0), get_group_id(1)) * 16;\n\
    int2 pos   = (int2)(get_local_id(0), get_local_id(1));\n\
\n\
    __local float4 tmp[32][32];\n\
\n\
    int rad_x = size_x / 2;\n\
    int rad_y = size_y / 2;\n\
    int x, y;\n\
\n\
    for (y = 0; y <= 1; y++) {\n\
        for (x = 0; x <= 1; x++) {\n\
            tmp[pos.y + 16 * y][pos.x + 16 * x] =\n\
                read_imagef(src, sampler, block + pos + (int2)(16 * x - 8, 16 * y - 8));\n\
        }\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    float4 val = tmp[pos.y + 8][pos.x + 8];\n\
\n\
    float4 horiz[2];\n\
    for (y = 0; y <= 1; y++) {\n\
        horiz[y] = 0.0f;\n\
        for (x = 0; x < size_x; x++)\n\
            horiz[y] += coef_x[x] * tmp[pos.y + y * 16][pos.x + 8 + x - rad_x];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    for (y = 0; y <= 1; y++) {\n\
        tmp[pos.y + y * 16][pos.x + 8] = horiz[y];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    float4 sum = 0.0f;\n\
    for (y = 0; y < size_y; y++)\n\
        sum += coef_y[y] * tmp[pos.y + 8 + y - rad_y][pos.x + 8];\n\
\n\
    if (block.x + pos.x < get_image_width(dst) &&\n\
        block.y + pos.y < get_image_height(dst))\n\
        write_imagef(dst, block + pos, val + (val - sum) * amount);\n\
}";

const char *ff_source_xfade_cl = "\
const sampler_t sampler = (CLK_NORMALIZED_COORDS_FALSE |\n\
                           CLK_FILTER_NEAREST);\n\
\n\
__kernel void fade(__write_only image2d_t dst,\n\
                   __read_only  image2d_t src1,\n\
                   __read_only  image2d_t src2,\n\
                   float progress)\n\
{\n\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 val1 = read_imagef(src1, sampler, p);\n\
    float4 val2 = read_imagef(src2, sampler, p);\n\
\n\
    write_imagef(dst, p, mix(val2, val1, progress));\n\
}\n\
\n\
__kernel void wipeleft(__write_only image2d_t dst,\n\
                       __read_only  image2d_t src1,\n\
                       __read_only  image2d_t src2,\n\
                       float progress)\n\
{\n\
    int   s = (int)(get_image_dim(src1).x * progress);\n\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 val1 = read_imagef(src1, sampler, p);\n\
    float4 val2 = read_imagef(src2, sampler, p);\n\
\n\
    write_imagef(dst, p, p.x > s ? val2 : val1);\n\
}\n\
\n\
__kernel void wiperight(__write_only image2d_t dst,\n\
                        __read_only  image2d_t src1,\n\
                        __read_only  image2d_t src2,\n\
                        float progress)\n\
{\n\
    int   s = (int)(get_image_dim(src1).x * (1.f - progress));\n\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 val1 = read_imagef(src1, sampler, p);\n\
    float4 val2 = read_imagef(src2, sampler, p);\n\
\n\
    write_imagef(dst, p, p.x > s ? val1 : val2);\n\
}\n\
\n\
__kernel void wipeup(__write_only image2d_t dst,\n\
                     __read_only  image2d_t src1,\n\
                     __read_only  image2d_t src2,\n\
                     float progress)\n\
{\n\
    int   s = (int)(get_image_dim(src1).y * progress);\n\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 val1 = read_imagef(src1, sampler, p);\n\
    float4 val2 = read_imagef(src2, sampler, p);\n\
\n\
    write_imagef(dst, p, p.y > s ? val2 : val1);\n\
}\n\
\n\
__kernel void wipedown(__write_only image2d_t dst,\n\
                       __read_only  image2d_t src1,\n\
                       __read_only  image2d_t src2,\n\
                       float progress)\n\
{\n\
    int   s = (int)(get_image_dim(src1).y * (1.f - progress));\n\
    int2  p = (int2)(get_global_id(0), get_global_id(1));\n\
\n\
    float4 val1 = read_imagef(src1, sampler, p);\n\
    float4 val2 = read_imagef(src2, sampler, p);\n\
\n\
    write_imagef(dst, p, p.y > s ? val1 : val2);\n\
}\n\
\n\
void slide(__write_only image2d_t dst,\n\
           __read_only  image2d_t src1,\n\
           __read_only  image2d_t src2,\n\
           float progress,\n\
           int2 direction)\n\
{\n\
    int   w = get_image_dim(src1).x;\n\
    int   h = get_image_dim(src1).y;\n\
    int2 wh = (int2)(w, h);\n\
    int2 uv = (int2)(get_global_id(0), get_global_id(1));\n\
    int2 pi = (int2)(progress * w, progress * h);\n\
    int2 p = uv + pi * direction;\n\
    int2 f = p % wh;\n\
\n\
    f = f + (int2)(w, h) * (int2)(f.x < 0, f.y < 0);\n\
    float4 val1 = read_imagef(src1, sampler, f);\n\
    float4 val2 = read_imagef(src2, sampler, f);\n\
    write_imagef(dst, uv, mix(val1, val2, (p.y >= 0) * (h > p.y) * (p.x >= 0) * (w > p.x)));\n\
}\n\
\n\
__kernel void slidedown(__write_only image2d_t dst,\n\
                        __read_only  image2d_t src1,\n\
                        __read_only  image2d_t src2,\n\
                        float progress)\n\
{\n\
    int2 direction = (int2)(0, 1);\n\
    slide(dst, src1, src2, progress, direction);\n\
}\n\
\n\
__kernel void slideup(__write_only image2d_t dst,\n\
                      __read_only  image2d_t src1,\n\
                      __read_only  image2d_t src2,\n\
                      float progress)\n\
{\n\
    int2 direction = (int2)(0, -1);\n\
    slide(dst, src1, src2, progress, direction);\n\
}\n\
\n\
__kernel void slideleft(__write_only image2d_t dst,\n\
                        __read_only  image2d_t src1,\n\
                        __read_only  image2d_t src2,\n\
                        float progress)\n\
{\n\
    int2 direction = (int2)(-1, 0);\n\
    slide(dst, src1, src2, progress, direction);\n\
}\n\
\n\
__kernel void slideright(__write_only image2d_t dst,\n\
                         __read_only  image2d_t src1,\n\
                         __read_only  image2d_t src2,\n\
                         float progress)\n\
{\n\
    int2 direction = (int2)(1, 0);\n\
    slide(dst, src1, src2, progress, direction);\n\
}";
