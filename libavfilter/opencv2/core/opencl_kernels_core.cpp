//#include <stddef.h>
#include "precomp.hpp"
#include "opencl_kernels_core.hpp"

namespace cv::ocl::core
{

ProgramSource arithm_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined cl_khr_fp64\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#ifdef INTEL_DEVICE\n\
#pragma OPENCL FP_CONTRACT ON\n\
#pragma OPENCL FP_FAST_FMAF ON\n\
#pragma OPENCL FP_FAST_FMA ON\n\
#endif\n\
\n\
#if !defined(DEPTH_dst)\n\
#error \"Kernel configuration error: DEPTH_dst value is required\"\n\
#elif !(DEPTH_dst >= 0 && DEPTH_dst <= 7)\n\
#error \"Kernel configuration error: invalid DEPTH_dst value\"\n\
#endif\n\
#if defined(depth)\n\
#error \"Kernel configuration error: ambiguous 'depth' value is defined, use 'DEPTH_dst' instead\"\n\
#endif\n\
\n\
#define CAT__(x, y) x ## y\n\
#define CAT_(x, y) CAT__(x, y)\n\
#define CAT(x, y) CAT_(x, y)\n\
\n\
\n\
#if DEPTH_dst < 5 /* CV_32F */\n\
#define CV_DST_TYPE_IS_INTEGER\n\
#else\n\
#define CV_DST_TYPE_IS_FP\n\
#endif\n\
\n\
#if DEPTH_dst != 6 /* CV_64F */\n\
#define CV_DST_TYPE_FIT_32F 1\n\
#else\n\
#define CV_DST_TYPE_FIT_32F 0\n\
#endif\n\
\n\
\n\
#if CV_DST_TYPE_FIT_32F\n\
#define CV_PI M_PI_F\n\
#else\n\
#define CV_PI M_PI\n\
#endif\n\
\n\
#ifndef cn\n\
#define cn 1\n\
#endif\n\
\n\
#if cn == 1\n\
#undef srcT1_C1\n\
#undef srcT2_C1\n\
#undef dstT_C1\n\
#define srcT1_C1 srcT1\n\
#define srcT2_C1 srcT2\n\
#define dstT_C1 dstT\n\
#endif\n\
\n\
#if cn != 3\n\
    #define storedst(val) *(__global dstT *)(dstptr + dst_index) = val\n\
    #define storedst2(val) *(__global dstT *)(dstptr2 + dst_index2) = val\n\
#else\n\
    #define storedst(val) vstore3(val, 0, (__global dstT_C1 *)(dstptr + dst_index))\n\
    #define storedst2(val) vstore3(val, 0, (__global dstT_C1 *)(dstptr2 + dst_index2))\n\
#endif\n\
\n\
#define noconvert\n\
\n\
#ifndef workT\n\
\n\
    #ifndef srcT1\n\
    #define srcT1 dstT\n\
    #endif\n\
\n\
    #ifndef srcT1_C1\n\
    #define srcT1_C1 dstT_C1\n\
    #endif\n\
\n\
    #ifndef srcT2\n\
    #define srcT2 dstT\n\
    #endif\n\
\n\
    #ifndef srcT2_C1\n\
    #define srcT2_C1 dstT_C1\n\
    #endif\n\
\n\
    #define workT dstT\n\
    #if cn != 3\n\
        #define srcelem1 *(__global srcT1 *)(srcptr1 + src1_index)\n\
        #define srcelem2 *(__global srcT2 *)(srcptr2 + src2_index)\n\
    #else\n\
        #define srcelem1 vload3(0, (__global srcT1_C1 *)(srcptr1 + src1_index))\n\
        #define srcelem2 vload3(0, (__global srcT2_C1 *)(srcptr2 + src2_index))\n\
    #endif\n\
    #ifndef convertToDT\n\
    #define convertToDT noconvert\n\
    #endif\n\
\n\
#else\n\
\n\
    #ifndef convertToWT2\n\
    #define convertToWT2 convertToWT1\n\
    #endif\n\
    #if cn != 3\n\
        #define srcelem1 convertToWT1(*(__global srcT1 *)(srcptr1 + src1_index))\n\
        #define srcelem2 convertToWT2(*(__global srcT2 *)(srcptr2 + src2_index))\n\
    #else\n\
        #define srcelem1 convertToWT1(vload3(0, (__global srcT1_C1 *)(srcptr1 + src1_index)))\n\
        #define srcelem2 convertToWT2(vload3(0, (__global srcT2_C1 *)(srcptr2 + src2_index)))\n\
    #endif\n\
\n\
#endif\n\
\n\
#ifndef workST\n\
#define workST workT\n\
#endif\n\
\n\
#define EXTRA_PARAMS\n\
#define EXTRA_INDEX\n\
#define EXTRA_INDEX_ADD\n\
\n\
#if defined OP_ADD\n\
#define PROCESS_ELEM storedst(convertToDT(srcelem1 + srcelem2))\n\
\n\
#elif defined OP_SUB\n\
#define PROCESS_ELEM storedst(convertToDT(srcelem1 - srcelem2))\n\
\n\
#elif defined OP_RSUB\n\
#define PROCESS_ELEM storedst(convertToDT(srcelem2 - srcelem1))\n\
\n\
#elif defined OP_ABSDIFF\n\
#if wdepth <= 4\n\
#define PROCESS_ELEM \\\n\
    storedst(convertToDT(convertFromU(abs_diff(srcelem1, srcelem2))))\n\
#else\n\
#define PROCESS_ELEM \\\n\
    storedst(convertToDT(fabs(srcelem1 - srcelem2)))\n\
#endif\n\
\n\
#elif defined OP_AND\n\
#define PROCESS_ELEM storedst(srcelem1 & srcelem2)\n\
\n\
#elif defined OP_OR\n\
#define PROCESS_ELEM storedst(srcelem1 | srcelem2)\n\
\n\
#elif defined OP_XOR\n\
#define PROCESS_ELEM storedst(srcelem1 ^ srcelem2)\n\
\n\
#elif defined OP_NOT\n\
#define PROCESS_ELEM storedst(~srcelem1)\n\
\n\
#elif defined OP_MIN\n\
#define PROCESS_ELEM storedst(min(srcelem1, srcelem2))\n\
\n\
#elif defined OP_MAX\n\
#define PROCESS_ELEM storedst(max(srcelem1, srcelem2))\n\
\n\
#elif defined OP_MUL\n\
#define PROCESS_ELEM storedst(convertToDT(srcelem1 * srcelem2))\n\
\n\
#elif defined OP_MUL_SCALE\n\
#undef EXTRA_PARAMS\n\
#ifdef UNARY_OP\n\
#define EXTRA_PARAMS , workST srcelem2_, scaleT scale\n\
#undef srcelem2\n\
#define srcelem2 srcelem2_\n\
#else\n\
#define EXTRA_PARAMS , scaleT scale\n\
#endif\n\
#define PROCESS_ELEM storedst(convertToDT(srcelem1 * scale * srcelem2))\n\
\n\
#elif defined OP_DIV\n\
#ifdef CV_DST_TYPE_IS_INTEGER\n\
#define PROCESS_ELEM \\\n\
        workT e2 = srcelem2, zero = (workT)(0); \\\n\
        storedst(convertToDT(e2 != zero ? srcelem1 / e2 : zero))\n\
#else\n\
#define PROCESS_ELEM \\\n\
        workT e2 = srcelem2; \\\n\
        storedst(convertToDT(srcelem1 / e2))\n\
#endif\n\
\n\
#elif defined OP_DIV_SCALE\n\
#undef EXTRA_PARAMS\n\
#ifdef UNARY_OP\n\
#define EXTRA_PARAMS , workST srcelem2_, scaleT scale\n\
#undef srcelem2\n\
#define srcelem2 srcelem2_\n\
#else\n\
#define EXTRA_PARAMS , scaleT scale\n\
#endif\n\
#ifdef CV_DST_TYPE_IS_INTEGER\n\
#define PROCESS_ELEM \\\n\
        workT e2 = srcelem2, zero = (workT)(0); \\\n\
        storedst(convertToDT(e2 == zero ? zero : (srcelem1 * (workT)(scale) / e2)))\n\
#else\n\
#define PROCESS_ELEM \\\n\
        workT e2 = srcelem2; \\\n\
        storedst(convertToDT(srcelem1 * (workT)(scale) / e2))\n\
#endif\n\
\n\
#elif defined OP_RDIV_SCALE\n\
#undef EXTRA_PARAMS\n\
#ifdef UNARY_OP\n\
#define EXTRA_PARAMS , workST srcelem2_, scaleT scale\n\
#undef srcelem2\n\
#define srcelem2 srcelem2_\n\
#else\n\
#define EXTRA_PARAMS , scaleT scale\n\
#endif\n\
#ifdef CV_DST_TYPE_IS_INTEGER\n\
#define PROCESS_ELEM \\\n\
        workT e1 = srcelem1, zero = (workT)(0); \\\n\
        storedst(convertToDT(e1 == zero ? zero : (srcelem2 * (workT)(scale) / e1)))\n\
#else\n\
#define PROCESS_ELEM \\\n\
        workT e1 = srcelem1; \\\n\
        storedst(convertToDT(srcelem2 * (workT)(scale) / e1))\n\
#endif\n\
\n\
#elif defined OP_RECIP_SCALE\n\
#undef EXTRA_PARAMS\n\
#define EXTRA_PARAMS , scaleT scale\n\
#ifdef CV_DST_TYPE_IS_INTEGER\n\
#define PROCESS_ELEM \\\n\
        workT e1 = srcelem1, zero = (workT)(0); \\\n\
        storedst(convertToDT(e1 != zero ? scale / e1 : zero))\n\
#else\n\
#define PROCESS_ELEM \\\n\
        workT e1 = srcelem1; \\\n\
        storedst(convertToDT(scale / e1))\n\
#endif\n\
\n\
#elif defined OP_ADDW\n\
#undef EXTRA_PARAMS\n\
#define EXTRA_PARAMS , scaleT alpha, scaleT beta, scaleT gamma\n\
#if wdepth <= 4\n\
#define PROCESS_ELEM storedst(convertToDT(mad24(srcelem1, alpha, mad24(srcelem2, beta, gamma))))\n\
#else\n\
#define PROCESS_ELEM storedst(convertToDT(fma(srcelem1, alpha, fma(srcelem2, beta, gamma))))\n\
#endif\n\
\n\
#elif defined OP_MAG\n\
#define PROCESS_ELEM storedst(hypot(srcelem1, srcelem2))\n\
\n\
#elif defined OP_PHASE_RADIANS\n\
#define PROCESS_ELEM \\\n\
    workT tmp = atan2(srcelem2, srcelem1); \\\n\
    if (tmp < 0) \\\n\
        tmp += 2 * CV_PI; \\\n\
    storedst(tmp)\n\
\n\
#elif defined OP_PHASE_DEGREES\n\
    #define PROCESS_ELEM \\\n\
    workT tmp = degrees(atan2(srcelem2, srcelem1)); \\\n\
    if (tmp < 0) \\\n\
        tmp += 360; \\\n\
    storedst(tmp)\n\
\n\
#elif defined OP_EXP\n\
#if wdepth == 5\n\
#define PROCESS_ELEM storedst(native_exp(srcelem1))\n\
#else\n\
#define PROCESS_ELEM storedst(exp(srcelem1))\n\
#endif\n\
\n\
#elif defined OP_POW\n\
#define PROCESS_ELEM storedst(pow(srcelem1, srcelem2))\n\
\n\
#elif defined OP_POWN\n\
#if cn > 1\n\
#define PROCESS_INIT CAT(int, cn) powi = (CAT(int, cn))srcelem2;\n\
#else // cn\n\
#define PROCESS_INIT int powi = srcelem2;\n\
#endif\n\
#define PROCESS_ELEM storedst(convertToDT(pown(srcelem1, powi)))\n\
\n\
#elif defined OP_SQRT\n\
#if CV_DST_TYPE_FIT_32F\n\
#define PROCESS_ELEM storedst(native_sqrt(srcelem1))\n\
#else\n\
#define PROCESS_ELEM storedst(sqrt(srcelem1))\n\
#endif\n\
\n\
#elif defined OP_LOG\n\
#define PROCESS_ELEM \\\n\
    storedst(log(fabs(srcelem1)))\n\
\n\
#elif defined OP_CMP\n\
#define srcT2 srcT1\n\
#ifndef convertToWT1\n\
#define convertToWT1\n\
#endif\n\
#define PROCESS_ELEM \\\n\
    storedst(srcelem1 CMP_OPERATOR srcelem2 ? (dstT)(255) : (dstT)(0))\n\
\n\
#elif defined OP_CONVERT_SCALE_ABS\n\
#undef EXTRA_PARAMS\n\
#define EXTRA_PARAMS , workT1 alpha, workT1 beta\n\
#if wdepth <= 4\n\
#define PROCESS_ELEM \\\n\
    workT value = mad24(srcelem1, (workT)(alpha), (workT)(beta)); \\\n\
    storedst(convertToDT(abs(value)))\n\
#else\n\
#define PROCESS_ELEM \\\n\
    workT value = fma(srcelem1, (workT)(alpha), (workT)(beta)); \\\n\
    storedst(convertToDT(fabs(value)))\n\
#endif\n\
\n\
#elif defined OP_SCALE_ADD\n\
#undef EXTRA_PARAMS\n\
#define EXTRA_PARAMS , workT1 alpha\n\
#if wdepth <= 4\n\
#define PROCESS_ELEM storedst(convertToDT(mad24(srcelem1, (workT)(alpha), srcelem2)))\n\
#else\n\
#define PROCESS_ELEM storedst(convertToDT(fma(srcelem1, (workT)(alpha), srcelem2)))\n\
#endif\n\
\n\
#elif defined OP_CTP_AD || defined OP_CTP_AR\n\
#if CV_DST_TYPE_FIT_32F\n\
#define CV_EPSILON FLT_EPSILON\n\
#else\n\
#define CV_EPSILON DBL_EPSILON\n\
#endif\n\
#ifdef OP_CTP_AD\n\
#define TO_DEGREE cartToPolar = degrees(cartToPolar);\n\
#elif defined OP_CTP_AR\n\
#define TO_DEGREE\n\
#endif\n\
#ifdef SRC1_IS_DST_MAG\n\
#define ADAPT_SRC1 dstptr = srcptr1;\n\
#elif SRC1_IS_DST_ANGLE\n\
#define ADAPT_SRC1 dstptr2 = srcptr1;\n\
#else\n\
#define ADAPT_SRC1\n\
#endif\n\
#ifdef SRC2_IS_DST_MAG\n\
#define ADAPT_SRC2 dstptr = srcptr2;\n\
#elif SRC2_IS_DST_ANGLE\n\
#define ADAPT_SRC2 dstptr2 = srcptr2;\n\
#else\n\
#define ADAPT_SRC2\n\
#endif\n\
#define PROCESS_ELEM \\\n\
    dstT x = srcelem1, y = srcelem2; \\\n\
    dstT x2 = x * x, y2 = y * y; \\\n\
    dstT magnitude = sqrt(x2 + y2); \\\n\
    dstT tmp = y >= 0 ? 0 : CV_PI * 2; \\\n\
    tmp = x < 0 ? CV_PI : tmp; \\\n\
    dstT tmp1 = y >= 0 ? CV_PI * 0.5f : CV_PI * 1.5f; \\\n\
    dstT cartToPolar = y2 <= x2 ? x * y / mad((dstT)(0.28f), y2, x2 + CV_EPSILON) + tmp : (tmp1 - x * y / mad((dstT)(0.28f), x2, y2 + CV_EPSILON)); \\\n\
    TO_DEGREE \\\n\
    ADAPT_SRC1 \\\n\
    ADAPT_SRC2 \\\n\
    storedst(magnitude); \\\n\
    storedst2(cartToPolar)\n\
\n\
#elif defined OP_PTC_AD || defined OP_PTC_AR\n\
#ifdef OP_PTC_AD\n\
#define FROM_DEGREE y = radians(y)\n\
#else\n\
#define FROM_DEGREE\n\
#endif\n\
#ifdef SRC1_IS_DST_X\n\
#define ADAPT_SRC1 dstptr = srcptr1;\n\
#elif SRC1_IS_DST_Y\n\
#define ADAPT_SRC1 dstptr2 = srcptr1;\n\
#else\n\
#define ADAPT_SRC1\n\
#endif\n\
#ifdef SRC2_IS_DST_X\n\
#define ADAPT_SRC2 dstptr = srcptr2;\n\
#elif SRC2_IS_DST_Y\n\
#define ADAPT_SRC2 dstptr2 = srcptr2;\n\
#else\n\
#define ADAPT_SRC2\n\
#endif\n\
#define PROCESS_ELEM \\\n\
    dstT x = srcelem1, y = srcelem2, cosval; \\\n\
    FROM_DEGREE; \\\n\
    ADAPT_SRC1; \\\n\
    ADAPT_SRC2; \\\n\
    storedst2(sincos(y, &cosval) * x); \\\n\
    storedst(cosval * x);\n\
\n\
#elif defined OP_PATCH_NANS\n\
#undef EXTRA_PARAMS\n\
#define EXTRA_PARAMS , dstT val\n\
#define PROCESS_ELEM \\\n\
    if (isnan(srcelem1)) \\\n\
        storedst(val)\n\
\n\
#else\n\
#error \"unknown op type\"\n\
#endif\n\
\n\
#if defined OP_CTP_AD || defined OP_CTP_AR || defined OP_PTC_AD || defined OP_PTC_AR\n\
    #undef EXTRA_PARAMS\n\
    #define EXTRA_PARAMS , __global uchar* dstptr2, int dststep2, int dstoffset2\n\
    #undef EXTRA_INDEX\n\
    #define EXTRA_INDEX int dst_index2 = mad24(y0, dststep2, mad24(x, (int)sizeof(dstT_C1) * cn, dstoffset2))\n\
    #undef EXTRA_INDEX_ADD\n\
    #define EXTRA_INDEX_ADD dst_index2 += dststep2\n\
#endif\n\
\n\
#if defined UNARY_OP || defined MASK_UNARY_OP\n\
\n\
#if defined OP_AND || defined OP_OR || defined OP_XOR || defined OP_ADD || defined OP_SAT_ADD || \\\n\
    defined OP_SUB || defined OP_SAT_SUB || defined OP_RSUB || defined OP_SAT_RSUB || \\\n\
    defined OP_ABSDIFF || defined OP_CMP || defined OP_MIN || defined OP_MAX || defined OP_POW || \\\n\
    defined OP_MUL || defined OP_DIV || defined OP_POWN || defined OP_POWR || defined OP_ROOTN\n\
    #undef EXTRA_PARAMS\n\
    #define EXTRA_PARAMS , workST srcelem2_\n\
    #undef srcelem2\n\
    #define srcelem2 srcelem2_\n\
#endif\n\
\n\
#if !defined(PROCESS_INIT) && cn == 3\n\
#undef srcelem2\n\
#define srcelem2 (workT)(srcelem2_.x, srcelem2_.y, srcelem2_.z)\n\
#endif\n\
\n\
#endif\n\
\n\
#if defined BINARY_OP\n\
\n\
__kernel void KF(__global const uchar * srcptr1, int srcstep1, int srcoffset1,\n\
                 __global const uchar * srcptr2, int srcstep2, int srcoffset2,\n\
                 __global uchar * dstptr, int dststep, int dstoffset,\n\
                 int rows, int cols EXTRA_PARAMS )\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < cols)\n\
    {\n\
        int src1_index = mad24(y0, srcstep1, mad24(x, (int)sizeof(srcT1_C1) * cn, srcoffset1));\n\
#if !(defined(OP_RECIP_SCALE) || defined(OP_NOT))\n\
        int src2_index = mad24(y0, srcstep2, mad24(x, (int)sizeof(srcT2_C1) * cn, srcoffset2));\n\
#endif\n\
        int dst_index  = mad24(y0, dststep, mad24(x, (int)sizeof(dstT_C1) * cn, dstoffset));\n\
        EXTRA_INDEX;\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y, src1_index += srcstep1, dst_index += dststep)\n\
        {\n\
            PROCESS_ELEM;\n\
#if !(defined(OP_RECIP_SCALE) || defined(OP_NOT))\n\
            src2_index += srcstep2;\n\
#endif\n\
            EXTRA_INDEX_ADD;\n\
        }\n\
    }\n\
}\n\
\n\
#elif defined MASK_BINARY_OP\n\
\n\
__kernel void KF(__global const uchar * srcptr1, int srcstep1, int srcoffset1,\n\
                 __global const uchar * srcptr2, int srcstep2, int srcoffset2,\n\
                 __global const uchar * mask, int maskstep, int maskoffset,\n\
                 __global uchar * dstptr, int dststep, int dstoffset,\n\
                 int rows, int cols EXTRA_PARAMS )\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
#ifdef PROCESS_INIT\n\
    PROCESS_INIT\n\
#endif\n\
\n\
    if (x < cols)\n\
    {\n\
        int mask_index = mad24(y0, maskstep, x + maskoffset);\n\
        int src1_index = mad24(y0, srcstep1, mad24(x, (int)sizeof(srcT1_C1) * cn, srcoffset1));\n\
        int src2_index = mad24(y0, srcstep2, mad24(x, (int)sizeof(srcT2_C1) * cn, srcoffset2));\n\
        int dst_index  = mad24(y0, dststep, mad24(x, (int)sizeof(dstT_C1) * cn, dstoffset));\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y, src1_index += srcstep1, src2_index += srcstep2,\n\
                                                                mask_index += maskstep, dst_index += dststep)\n\
            if (mask[mask_index])\n\
            {\n\
                PROCESS_ELEM;\n\
            }\n\
    }\n\
}\n\
\n\
#elif defined UNARY_OP\n\
\n\
__kernel void KF(__global const uchar * srcptr1, int srcstep1, int srcoffset1,\n\
                 __global uchar * dstptr, int dststep, int dstoffset,\n\
                 int rows, int cols EXTRA_PARAMS )\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
#ifdef PROCESS_INIT\n\
    PROCESS_INIT\n\
#endif\n\
\n\
    if (x < cols)\n\
    {\n\
        int src1_index = mad24(y0, srcstep1, mad24(x, (int)sizeof(srcT1_C1) * cn, srcoffset1));\n\
        int dst_index  = mad24(y0, dststep, mad24(x, (int)sizeof(dstT_C1) * cn, dstoffset));\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y, src1_index += srcstep1, dst_index += dststep)\n\
        {\n\
            PROCESS_ELEM;\n\
        }\n\
    }\n\
}\n\
\n\
#elif defined MASK_UNARY_OP\n\
\n\
__kernel void KF(__global const uchar * srcptr1, int srcstep1, int srcoffset1,\n\
                 __global const uchar * mask, int maskstep, int maskoffset,\n\
                 __global uchar * dstptr, int dststep, int dstoffset,\n\
                 int rows, int cols EXTRA_PARAMS )\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
#ifdef PROCESS_INIT\n\
    PROCESS_INIT\n\
#endif\n\
\n\
    if (x < cols)\n\
    {\n\
        int mask_index = mad24(y0, maskstep, x + maskoffset);\n\
        int src1_index = mad24(y0, srcstep1, mad24(x, (int)sizeof(srcT1_C1) * cn, srcoffset1));\n\
        int dst_index  = mad24(y0, dststep, mad24(x, (int)sizeof(dstT_C1) * cn, dstoffset));\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y, src1_index += srcstep1, mask_index += maskstep, dst_index += dststep)\n\
            if (mask[mask_index])\n\
            {\n\
                PROCESS_ELEM;\n\
            }\n\
    }\n\
}\n\
\n\
#else\n\
\n\
#error \"Unknown operation type\"\n\
\n\
#endif");

ProgramSource convert_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#ifdef HALF_SUPPORT\n\
#ifdef cl_khr_fp16\n\
#pragma OPENCL EXTENSION cl_khr_fp16:enable\n\
#endif\n\
#endif\n\
\n\
#define noconvert\n\
\n\
__kernel void convertTo(__global const uchar * srcptr, int src_step, int src_offset,\n\
                        __global uchar * dstptr, int dst_step, int dst_offset, int dst_rows, int dst_cols\n\
#ifndef NO_SCALE\n\
                        , WT alpha, WT beta\n\
#endif\n\
)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < dst_cols)\n\
    {\n\
        int src_index = mad24(y0, src_step, mad24(x, (int)sizeof(srcT), src_offset));\n\
        int dst_index = mad24(y0, dst_step, mad24(x, (int)sizeof(dstT), dst_offset));\n\
\n\
        for (int y = y0, y1 = min(dst_rows, y0 + rowsPerWI); y < y1; ++y, src_index += src_step, dst_index += dst_step)\n\
        {\n\
            __global const srcT * src = (__global const srcT *)(srcptr + src_index);\n\
            __global dstT * dst = (__global dstT *)(dstptr + dst_index);\n\
\n\
#ifdef NO_SCALE\n\
            dst[0] = convertToDT(src[0]);\n\
#else\n\
            dst[0] = convertToDT(fma(convertToWT(src[0]), alpha, beta));\n\
#endif\n\
        }\n\
    }\n\
}");

ProgramSource copymakeborder_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#if cn != 3\n\
#define loadpix(addr)  *(__global const T*)(addr)\n\
#define storepix(val, addr)  *(__global T*)(addr) = val\n\
#define TSIZE ((int)sizeof(T))\n\
#define convertScalar(a) (a)\n\
#else\n\
#define loadpix(addr)  vload3(0, (__global const T1*)(addr))\n\
#define storepix(val, addr) vstore3(val, 0, (__global T1*)(addr))\n\
#define TSIZE ((int)sizeof(T1)*3)\n\
#define convertScalar(a) (T)(a.x, a.y, a.z)\n\
#endif\n\
\n\
#ifdef BORDER_CONSTANT\n\
#define EXTRAPOLATE(x, cols) \\\n\
    ;\n\
#elif defined BORDER_REPLICATE\n\
#define EXTRAPOLATE(x, cols) \\\n\
    x = clamp(x, 0, cols - 1);\n\
#elif defined BORDER_WRAP\n\
#define EXTRAPOLATE(x, cols) \\\n\
    { \\\n\
        if (x < 0) \\\n\
            x -= ((x - cols + 1) / cols) * cols; \\\n\
        if (x >= cols) \\\n\
            x %= cols; \\\n\
    }\n\
#elif defined(BORDER_REFLECT) || defined(BORDER_REFLECT_101)\n\
#ifdef BORDER_REFLECT\n\
#define DELTA int delta = 0\n\
#else\n\
#define DELTA int delta = 1\n\
#endif\n\
#define EXTRAPOLATE(x, cols) \\\n\
    { \\\n\
        DELTA; \\\n\
        if (cols == 1) \\\n\
            x = 0; \\\n\
        else \\\n\
            do \\\n\
            { \\\n\
                if( x < 0 ) \\\n\
                    x = -x - 1 + delta; \\\n\
                else \\\n\
                    x = cols - 1 - (x - cols) - delta; \\\n\
            } \\\n\
            while (x >= cols || x < 0); \\\n\
    }\n\
#else\n\
#error \"No extrapolation method\"\n\
#endif\n\
\n\
#define NEED_EXTRAPOLATION(x, cols) (x >= cols || x < 0)\n\
\n\
__kernel void copyMakeBorder(__global const uchar * srcptr, int src_step, int src_offset, int src_rows, int src_cols,\n\
                             __global uchar * dstptr, int dst_step, int dst_offset, int dst_rows, int dst_cols,\n\
                             int top, int left, ST nVal)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
#ifdef BORDER_CONSTANT\n\
    T scalar = convertScalar(nVal);\n\
#endif\n\
\n\
    if (x < dst_cols)\n\
    {\n\
        int src_x = x - left, src_y;\n\
        int dst_index = mad24(y0, dst_step, mad24(x, (int)TSIZE, dst_offset));\n\
\n\
        if (NEED_EXTRAPOLATION(src_x, src_cols))\n\
        {\n\
#ifdef BORDER_CONSTANT\n\
            for (int y = y0, y1 = min(y0 + rowsPerWI, dst_rows); y < y1; ++y, dst_index += dst_step)\n\
                storepix(scalar, dstptr + dst_index);\n\
            return;\n\
#endif\n\
            EXTRAPOLATE(src_x, src_cols)\n\
        }\n\
        src_x = mad24(src_x, TSIZE, src_offset);\n\
\n\
        for (int y = y0, y1 = min(y0 + rowsPerWI, dst_rows); y < y1; ++y, dst_index += dst_step)\n\
        {\n\
            src_y = y - top;\n\
            if (NEED_EXTRAPOLATION(src_y, src_rows))\n\
            {\n\
                EXTRAPOLATE(src_y, src_rows)\n\
#ifdef BORDER_CONSTANT\n\
                storepix(scalar, dstptr + dst_index);\n\
                continue;\n\
#endif\n\
            }\n\
            int src_index = mad24(src_y, src_step, src_x);\n\
            storepix(loadpix(srcptr + src_index), dstptr + dst_index);\n\
        }\n\
    }\n\
}");

ProgramSource copyset_oclsrc = ProgramSource("\
#ifdef COPY_TO_MASK\n\
#define DEFINE_DATA \\\n\
    int src_index = mad24(y, src_step, mad24(x, (int)sizeof(T1) * scn, src_offset)); \\\n\
    int dst_index = mad24(y, dst_step, mad24(x, (int)sizeof(T1) * scn, dst_offset)); \\\n\
     \\\n\
    __global const T1 * src = (__global const T1 *)(srcptr + src_index); \\\n\
    __global T1 * dst = (__global T1 *)(dstptr + dst_index)\n\
\n\
__kernel void copyToMask(__global const uchar * srcptr, int src_step, int src_offset,\n\
                         __global const uchar * mask, int mask_step, int mask_offset,\n\
                         __global uchar * dstptr, int dst_step, int dst_offset,\n\
                         int dst_rows, int dst_cols)\n\
{\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
\n\
    if (x < dst_cols && y < dst_rows)\n\
    {\n\
        mask += mad24(y, mask_step, mad24(x, mcn, mask_offset));\n\
\n\
#if mcn == 1\n\
        if (mask[0])\n\
        {\n\
            DEFINE_DATA;\n\
\n\
            #pragma unroll\n\
            for (int c = 0; c < scn; ++c)\n\
                dst[c] = src[c];\n\
        }\n\
#ifdef HAVE_DST_UNINIT\n\
        else\n\
        {\n\
            DEFINE_DATA;\n\
\n\
            #pragma unroll\n\
            for (int c = 0; c < scn; ++c)\n\
                dst[c] = (T1)(0);\n\
        }\n\
#endif\n\
#elif scn == mcn\n\
        DEFINE_DATA;\n\
\n\
        #pragma unroll\n\
        for (int c = 0; c < scn; ++c)\n\
            if (mask[c])\n\
                dst[c] = src[c];\n\
#ifdef HAVE_DST_UNINIT\n\
            else\n\
                dst[c] = (T1)(0);\n\
#endif\n\
#else\n\
#error \"(mcn == 1 || mcn == scn) should be true\"\n\
#endif\n\
    }\n\
}\n\
\n\
#else\n\
\n\
#ifndef dstST\n\
#define dstST dstT\n\
#endif\n\
\n\
#if cn != 3\n\
#define value value_\n\
#define storedst(val) *(__global dstT *)(dstptr + dst_index) = val\n\
#else\n\
#define value (dstT)(value_.x, value_.y, value_.z)\n\
#define storedst(val) vstore3(val, 0, (__global dstT1 *)(dstptr + dst_index))\n\
#endif\n\
\n\
__kernel void setMask(__global const uchar* mask, int maskstep, int maskoffset,\n\
                      __global uchar* dstptr, int dststep, int dstoffset,\n\
                      int rows, int cols, dstST value_)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < cols)\n\
    {\n\
        int mask_index = mad24(y0, maskstep, x + maskoffset);\n\
        int dst_index  = mad24(y0, dststep, mad24(x, (int)sizeof(dstT1) * cn, dstoffset));\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y)\n\
        {\n\
            if( mask[mask_index] )\n\
                storedst(value);\n\
\n\
            mask_index += maskstep;\n\
            dst_index += dststep;\n\
        }\n\
    }\n\
}\n\
\n\
__kernel void set(__global uchar* dstptr, int dststep, int dstoffset,\n\
                  int rows, int cols, dstST value_)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < cols)\n\
    {\n\
        int dst_index  = mad24(y0, dststep, mad24(x, (int)sizeof(dstT1) * cn, dstoffset));\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y, dst_index += dststep)\n\
            storedst(value);\n\
    }\n\
}\n\
#endif");

ProgramSource cvtclr_dx_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined cl_khr_fp64\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#ifdef INTEL_DEVICE\n\
#pragma OPENCL FP_CONTRACT ON\n\
#pragma OPENCL FP_FAST_FMAF ON\n\
#pragma OPENCL FP_FAST_FMA ON\n\
#endif\n\
\n\
static\n\
__constant\n\
float c_YUV2RGBCoeffs_420[5] =\n\
{\n\
     1.163999557f,\n\
     2.017999649f,\n\
    -0.390999794f,\n\
    -0.812999725f,\n\
     1.5959997177f\n\
};\n\
\n\
static const __constant float CV_8U_MAX         = 255.0f;\n\
static const __constant float CV_8U_HALF        = 128.0f;\n\
static const __constant float BT601_BLACK_RANGE = 16.0f;\n\
static const __constant float CV_8U_SCALE       = 1.0f / 255.0f;\n\
static const __constant float d1                = BT601_BLACK_RANGE / CV_8U_MAX;\n\
static const __constant float d2                = CV_8U_HALF / CV_8U_MAX;\n\
\n\
#define NCHANNELS 3\n\
\n\
__kernel\n\
void YUV2BGR_NV12_8u(\n\
    read_only image2d_t imgY,\n\
    read_only image2d_t imgUV,\n\
    __global unsigned char* pBGR,\n\
   int bgrStep,\n\
   int cols,\n\
   int rows)\n\
{\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
    // each iteration computes 2*2=4 pixels\n\
    int x2 = x*2;\n\
    int y2 = y*2;\n\
\n\
    if (x2 + 1 < cols) {\n\
        if (y2 + 1 < rows) {\n\
            __global uchar *pDstRow1 = pBGR + mad24(y2, bgrStep, mad24(x2, NCHANNELS, 0));\n\
            __global uchar *pDstRow2 = pDstRow1 + bgrStep;\n\
\n\
            float4 Y1 = read_imagef(imgY, (int2)(x2 + 0, y2 + 0));\n\
            float4 Y2 = read_imagef(imgY, (int2)(x2 + 1, y2 + 0));\n\
            float4 Y3 = read_imagef(imgY, (int2)(x2 + 0, y2 + 1));\n\
            float4 Y4 = read_imagef(imgY, (int2)(x2 + 1, y2 + 1));\n\
            float4 Y = (float4)(Y1.x, Y2.x, Y3.x, Y4.x);\n\
\n\
            float4 UV = read_imagef(imgUV, (int2)(x, y)) - d2;\n\
\n\
            __constant float *coeffs = c_YUV2RGBCoeffs_420;\n\
\n\
            Y = max(0.f, Y - d1) * coeffs[0];\n\
\n\
            float ruv = fma(coeffs[4], UV.y, 0.0f);\n\
            float guv = fma(coeffs[3], UV.y, fma(coeffs[2], UV.x, 0.0f));\n\
            float buv = fma(coeffs[1], UV.x, 0.0f);\n\
\n\
            float4 R = (Y + ruv) * CV_8U_MAX;\n\
            float4 G = (Y + guv) * CV_8U_MAX;\n\
            float4 B = (Y + buv) * CV_8U_MAX;\n\
\n\
            pDstRow1[0*NCHANNELS + 0] = convert_uchar_sat(B.x);\n\
            pDstRow1[0*NCHANNELS + 1] = convert_uchar_sat(G.x);\n\
            pDstRow1[0*NCHANNELS + 2] = convert_uchar_sat(R.x);\n\
\n\
            pDstRow1[1*NCHANNELS + 0] = convert_uchar_sat(B.y);\n\
            pDstRow1[1*NCHANNELS + 1] = convert_uchar_sat(G.y);\n\
            pDstRow1[1*NCHANNELS + 2] = convert_uchar_sat(R.y);\n\
\n\
            pDstRow2[0*NCHANNELS + 0] = convert_uchar_sat(B.z);\n\
            pDstRow2[0*NCHANNELS + 1] = convert_uchar_sat(G.z);\n\
            pDstRow2[0*NCHANNELS + 2] = convert_uchar_sat(R.z);\n\
\n\
            pDstRow2[1*NCHANNELS + 0] = convert_uchar_sat(B.w);\n\
            pDstRow2[1*NCHANNELS + 1] = convert_uchar_sat(G.w);\n\
            pDstRow2[1*NCHANNELS + 2] = convert_uchar_sat(R.w);\n\
        }\n\
    }\n\
}\n\
\n\
static\n\
__constant float c_RGB2YUVCoeffs_420[8] =\n\
{\n\
     0.256999969f,  0.50399971f,   0.09799957f,   -0.1479988098f,\n\
    -0.2909994125f, 0.438999176f, -0.3679990768f, -0.0709991455f\n\
};\n\
\n\
__kernel\n\
void BGR2YUV_NV12_8u(\n\
    __global unsigned char* pBGR,\n\
    int bgrStep,\n\
    int cols,\n\
    int rows,\n\
    write_only image2d_t imgY,\n\
    write_only image2d_t imgUV)\n\
{\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
    // each iteration computes 2*2=4 pixels\n\
    int x2 = x*2;\n\
    int y2 = y*2;\n\
\n\
    if (x2 + 1 < cols)\n\
    {\n\
        if (y2 + 1 < rows)\n\
        {\n\
            __global const uchar* pSrcRow1 = pBGR + mad24(y2, bgrStep, mad24(x2, NCHANNELS, 0));\n\
            __global const uchar* pSrcRow2 = pSrcRow1 + bgrStep;\n\
\n\
            float4 src_pix1 = convert_float4(vload4(0, pSrcRow1 + 0*NCHANNELS)) * CV_8U_SCALE;\n\
            float4 src_pix2 = convert_float4(vload4(0, pSrcRow1 + 1*NCHANNELS)) * CV_8U_SCALE;\n\
            float4 src_pix3 = convert_float4(vload4(0, pSrcRow2 + 0*NCHANNELS)) * CV_8U_SCALE;\n\
            float4 src_pix4 = convert_float4(vload4(0, pSrcRow2 + 1*NCHANNELS)) * CV_8U_SCALE;\n\
\n\
            __constant float* coeffs = c_RGB2YUVCoeffs_420;\n\
\n\
            float Y1 = fma(coeffs[0], src_pix1.z, fma(coeffs[1], src_pix1.y, fma(coeffs[2], src_pix1.x, d1)));\n\
            float Y2 = fma(coeffs[0], src_pix2.z, fma(coeffs[1], src_pix2.y, fma(coeffs[2], src_pix2.x, d1)));\n\
            float Y3 = fma(coeffs[0], src_pix3.z, fma(coeffs[1], src_pix3.y, fma(coeffs[2], src_pix3.x, d1)));\n\
            float Y4 = fma(coeffs[0], src_pix4.z, fma(coeffs[1], src_pix4.y, fma(coeffs[2], src_pix4.x, d1)));\n\
\n\
            float4 UV;\n\
            UV.x = fma(coeffs[3], src_pix1.z, fma(coeffs[4], src_pix1.y, fma(coeffs[5], src_pix1.x, d2)));\n\
            UV.y = fma(coeffs[5], src_pix1.z, fma(coeffs[6], src_pix1.y, fma(coeffs[7], src_pix1.x, d2)));\n\
\n\
            write_imagef(imgY, (int2)(x2+0, y2+0), Y1);\n\
            write_imagef(imgY, (int2)(x2+1, y2+0), Y2);\n\
            write_imagef(imgY, (int2)(x2+0, y2+1), Y3);\n\
            write_imagef(imgY, (int2)(x2+1, y2+1), Y4);\n\
\n\
            write_imagef(imgUV, (int2)(x, y), UV);\n\
        }\n\
    }\n\
}");

ProgramSource fft_oclsrc = ProgramSource("\
#define SQRT_2 0.707106781188f\n\
#define sin_120 0.866025403784f\n\
#define fft5_2  0.559016994374f\n\
#define fft5_3 -0.951056516295f\n\
#define fft5_4 -1.538841768587f\n\
#define fft5_5  0.363271264002f\n\
\n\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
__attribute__((always_inline))\n\
CT mul_complex(CT a, CT b) {\n\
    return (CT)(fma(a.x, b.x, -a.y * b.y), fma(a.x, b.y, a.y * b.x));\n\
}\n\
\n\
__attribute__((always_inline))\n\
CT twiddle(CT a) {\n\
    return (CT)(a.y, -a.x);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void butterfly2(CT a0, CT a1, __local CT* smem, __global const CT* twiddles,\n\
                const int x, const int block_size)\n\
{\n\
    const int k = x & (block_size - 1);\n\
    a1 = mul_complex(twiddles[k], a1);\n\
    const int dst_ind = (x << 1) - k;\n\
\n\
    smem[dst_ind] = a0 + a1;\n\
    smem[dst_ind+block_size] = a0 - a1;\n\
}\n\
\n\
__attribute__((always_inline))\n\
void butterfly4(CT a0, CT a1, CT a2, CT a3, __local CT* smem, __global const CT* twiddles,\n\
                const int x, const int block_size)\n\
{\n\
    const int k = x & (block_size - 1);\n\
    a1 = mul_complex(twiddles[k], a1);\n\
    a2 = mul_complex(twiddles[k + block_size], a2);\n\
    a3 = mul_complex(twiddles[k + 2*block_size], a3);\n\
\n\
    const int dst_ind = ((x - k) << 2) + k;\n\
\n\
    CT b0 = a0 + a2;\n\
    a2 = a0 - a2;\n\
    CT b1 = a1 + a3;\n\
    a3 = twiddle(a1 - a3);\n\
\n\
    smem[dst_ind]                = b0 + b1;\n\
    smem[dst_ind + block_size]   = a2 + a3;\n\
    smem[dst_ind + 2*block_size] = b0 - b1;\n\
    smem[dst_ind + 3*block_size] = a2 - a3;\n\
}\n\
\n\
__attribute__((always_inline))\n\
void butterfly3(CT a0, CT a1, CT a2, __local CT* smem, __global const CT* twiddles,\n\
                const int x, const int block_size)\n\
{\n\
    const int k = x % block_size;\n\
    a1 = mul_complex(twiddles[k], a1);\n\
    a2 = mul_complex(twiddles[k+block_size], a2);\n\
    const int dst_ind = ((x - k) * 3) + k;\n\
\n\
    CT b1 = a1 + a2;\n\
    a2 = twiddle(sin_120*(a1 - a2));\n\
    CT b0 = a0 - (CT)(0.5f)*b1;\n\
\n\
    smem[dst_ind] = a0 + b1;\n\
    smem[dst_ind + block_size] = b0 + a2;\n\
    smem[dst_ind + 2*block_size] = b0 - a2;\n\
}\n\
\n\
__attribute__((always_inline))\n\
void butterfly5(CT a0, CT a1, CT a2, CT a3, CT a4, __local CT* smem, __global const CT* twiddles,\n\
                const int x, const int block_size)\n\
{\n\
    const int k = x % block_size;\n\
    a1 = mul_complex(twiddles[k], a1);\n\
    a2 = mul_complex(twiddles[k + block_size], a2);\n\
    a3 = mul_complex(twiddles[k+2*block_size], a3);\n\
    a4 = mul_complex(twiddles[k+3*block_size], a4);\n\
\n\
    const int dst_ind = ((x - k) * 5) + k;\n\
    __local CT* dst = smem + dst_ind;\n\
\n\
    CT b0, b1, b5;\n\
\n\
    b1 = a1 + a4;\n\
    a1 -= a4;\n\
\n\
    a4 = a3 + a2;\n\
    a3 -= a2;\n\
\n\
    a2 = b1 + a4;\n\
    b0 = a0 - (CT)0.25f * a2;\n\
\n\
    b1 = fft5_2 * (b1 - a4);\n\
    a4 = fft5_3 * (CT)(-a1.y - a3.y, a1.x + a3.x);\n\
    b5 = (CT)(a4.x - fft5_5 * a1.y, a4.y + fft5_5 * a1.x);\n\
\n\
    a4.x += fft5_4 * a3.y;\n\
    a4.y -= fft5_4 * a3.x;\n\
\n\
    a1 = b0 + b1;\n\
    b0 -= b1;\n\
\n\
    dst[0] = a0 + a2;\n\
    dst[block_size] = a1 + a4;\n\
    dst[2 * block_size] = b0 + b5;\n\
    dst[3 * block_size] = b0 - b5;\n\
    dst[4 * block_size] = a1 - a4;\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix2(__local CT* smem, __global const CT* twiddles, const int x, const int block_size, const int t)\n\
{\n\
    CT a0, a1;\n\
\n\
    if (x < t)\n\
    {\n\
        a0 = smem[x];\n\
        a1 = smem[x+t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x < t)\n\
        butterfly2(a0, a1, smem, twiddles, x, block_size);\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix2_B2(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int x2 = x1 + t/2;\n\
    CT a0, a1, a2, a3;\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t];\n\
        a2 = smem[x2]; a3 = smem[x2+t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        butterfly2(a0, a1, smem, twiddles, x1, block_size);\n\
        butterfly2(a2, a3, smem, twiddles, x2, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix2_B3(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int x2 = x1 + t/3;\n\
    const int x3 = x1 + 2*t/3;\n\
    CT a0, a1, a2, a3, a4, a5;\n\
\n\
    if (x1 < t/3)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t];\n\
        a2 = smem[x2]; a3 = smem[x2+t];\n\
        a4 = smem[x3]; a5 = smem[x3+t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/3)\n\
    {\n\
        butterfly2(a0, a1, smem, twiddles, x1, block_size);\n\
        butterfly2(a2, a3, smem, twiddles, x2, block_size);\n\
        butterfly2(a4, a5, smem, twiddles, x3, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix2_B4(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int thread_block = t/4;\n\
    const int x2 = x1 + thread_block;\n\
    const int x3 = x1 + 2*thread_block;\n\
    const int x4 = x1 + 3*thread_block;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7;\n\
\n\
    if (x1 < t/4)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t];\n\
        a2 = smem[x2]; a3 = smem[x2+t];\n\
        a4 = smem[x3]; a5 = smem[x3+t];\n\
        a6 = smem[x4]; a7 = smem[x4+t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/4)\n\
    {\n\
        butterfly2(a0, a1, smem, twiddles, x1, block_size);\n\
        butterfly2(a2, a3, smem, twiddles, x2, block_size);\n\
        butterfly2(a4, a5, smem, twiddles, x3, block_size);\n\
        butterfly2(a6, a7, smem, twiddles, x4, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix2_B5(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int thread_block = t/5;\n\
    const int x2 = x1 + thread_block;\n\
    const int x3 = x1 + 2*thread_block;\n\
    const int x4 = x1 + 3*thread_block;\n\
    const int x5 = x1 + 4*thread_block;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7, a8, a9;\n\
\n\
    if (x1 < t/5)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t];\n\
        a2 = smem[x2]; a3 = smem[x2+t];\n\
        a4 = smem[x3]; a5 = smem[x3+t];\n\
        a6 = smem[x4]; a7 = smem[x4+t];\n\
        a8 = smem[x5]; a9 = smem[x5+t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/5)\n\
    {\n\
        butterfly2(a0, a1, smem, twiddles, x1, block_size);\n\
        butterfly2(a2, a3, smem, twiddles, x2, block_size);\n\
        butterfly2(a4, a5, smem, twiddles, x3, block_size);\n\
        butterfly2(a6, a7, smem, twiddles, x4, block_size);\n\
        butterfly2(a8, a9, smem, twiddles, x5, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix4(__local CT* smem, __global const CT* twiddles, const int x, const int block_size, const int t)\n\
{\n\
    CT a0, a1, a2, a3;\n\
\n\
    if (x < t)\n\
    {\n\
        a0 = smem[x]; a1 = smem[x+t]; a2 = smem[x+2*t]; a3 = smem[x+3*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x < t)\n\
        butterfly4(a0, a1, a2, a3, smem, twiddles, x, block_size);\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix4_B2(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int x2 = x1 + t/2;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7;\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t]; a2 = smem[x1+2*t]; a3 = smem[x1+3*t];\n\
        a4 = smem[x2]; a5 = smem[x2+t]; a6 = smem[x2+2*t]; a7 = smem[x2+3*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        butterfly4(a0, a1, a2, a3, smem, twiddles, x1, block_size);\n\
        butterfly4(a4, a5, a6, a7, smem, twiddles, x2, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix4_B3(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int x2 = x1 + t/3;\n\
    const int x3 = x2 + t/3;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11;\n\
\n\
    if (x1 < t/3)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t]; a2 = smem[x1+2*t]; a3 = smem[x1+3*t];\n\
        a4 = smem[x2]; a5 = smem[x2+t]; a6 = smem[x2+2*t]; a7 = smem[x2+3*t];\n\
        a8 = smem[x3]; a9 = smem[x3+t]; a10 = smem[x3+2*t]; a11 = smem[x3+3*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/3)\n\
    {\n\
        butterfly4(a0, a1, a2, a3, smem, twiddles, x1, block_size);\n\
        butterfly4(a4, a5, a6, a7, smem, twiddles, x2, block_size);\n\
        butterfly4(a8, a9, a10, a11, smem, twiddles, x3, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix8(__local CT* smem, __global const CT* twiddles, const int x, const int block_size, const int t)\n\
{\n\
    const int k = x % block_size;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7;\n\
\n\
    if (x < t)\n\
    {\n\
        int tw_ind = block_size / 8;\n\
\n\
        a0 = smem[x];\n\
        a1 = mul_complex(twiddles[k], smem[x + t]);\n\
        a2 = mul_complex(twiddles[k + block_size],smem[x+2*t]);\n\
        a3 = mul_complex(twiddles[k+2*block_size],smem[x+3*t]);\n\
        a4 = mul_complex(twiddles[k+3*block_size],smem[x+4*t]);\n\
        a5 = mul_complex(twiddles[k+4*block_size],smem[x+5*t]);\n\
        a6 = mul_complex(twiddles[k+5*block_size],smem[x+6*t]);\n\
        a7 = mul_complex(twiddles[k+6*block_size],smem[x+7*t]);\n\
\n\
        CT b0, b1, b6, b7;\n\
\n\
        b0 = a0 + a4;\n\
        a4 = a0 - a4;\n\
        b1 = a1 + a5;\n\
        a5 = a1 - a5;\n\
        a5 = (CT)(SQRT_2) * (CT)(a5.x + a5.y, -a5.x + a5.y);\n\
        b6 = twiddle(a2 - a6);\n\
        a2 = a2 + a6;\n\
        b7 = a3 - a7;\n\
        b7 = (CT)(SQRT_2) * (CT)(-b7.x + b7.y, -b7.x - b7.y);\n\
        a3 = a3 + a7;\n\
\n\
        a0 = b0 + a2;\n\
        a2 = b0 - a2;\n\
        a1 = b1 + a3;\n\
        a3 = twiddle(b1 - a3);\n\
        a6 = a4 - b6;\n\
        a4 = a4 + b6;\n\
        a7 = twiddle(a5 - b7);\n\
        a5 = a5 + b7;\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x < t)\n\
    {\n\
        const int dst_ind = ((x - k) << 3) + k;\n\
        __local CT* dst = smem + dst_ind;\n\
\n\
        dst[0] = a0 + a1;\n\
        dst[block_size] = a4 + a5;\n\
        dst[2 * block_size] = a2 + a3;\n\
        dst[3 * block_size] = a6 + a7;\n\
        dst[4 * block_size] = a0 - a1;\n\
        dst[5 * block_size] = a4 - a5;\n\
        dst[6 * block_size] = a2 - a3;\n\
        dst[7 * block_size] = a6 - a7;\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix3(__local CT* smem, __global const CT* twiddles, const int x, const int block_size, const int t)\n\
{\n\
    CT a0, a1, a2;\n\
\n\
    if (x < t)\n\
    {\n\
        a0 = smem[x]; a1 = smem[x+t]; a2 = smem[x+2*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x < t)\n\
        butterfly3(a0, a1, a2, smem, twiddles, x, block_size);\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix3_B2(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int x2 = x1 + t/2;\n\
    CT a0, a1, a2, a3, a4, a5;\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t]; a2 = smem[x1+2*t];\n\
        a3 = smem[x2]; a4 = smem[x2+t]; a5 = smem[x2+2*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        butterfly3(a0, a1, a2, smem, twiddles, x1, block_size);\n\
        butterfly3(a3, a4, a5, smem, twiddles, x2, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix3_B3(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int x2 = x1 + t/3;\n\
    const int x3 = x2 + t/3;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7, a8;\n\
\n\
    if (x1 < t/3)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t]; a2 = smem[x1+2*t];\n\
        a3 = smem[x2]; a4 = smem[x2+t]; a5 = smem[x2+2*t];\n\
        a6 = smem[x3]; a7 = smem[x3+t]; a8 = smem[x3+2*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/3)\n\
    {\n\
        butterfly3(a0, a1, a2, smem, twiddles, x1, block_size);\n\
        butterfly3(a3, a4, a5, smem, twiddles, x2, block_size);\n\
        butterfly3(a6, a7, a8, smem, twiddles, x3, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix3_B4(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int thread_block = t/4;\n\
    const int x2 = x1 + thread_block;\n\
    const int x3 = x1 + 2*thread_block;\n\
    const int x4 = x1 + 3*thread_block;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11;\n\
\n\
    if (x1 < t/4)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1+t]; a2 = smem[x1+2*t];\n\
        a3 = smem[x2]; a4 = smem[x2+t]; a5 = smem[x2+2*t];\n\
        a6 = smem[x3]; a7 = smem[x3+t]; a8 = smem[x3+2*t];\n\
        a9 = smem[x4]; a10 = smem[x4+t]; a11 = smem[x4+2*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/4)\n\
    {\n\
        butterfly3(a0, a1, a2, smem, twiddles, x1, block_size);\n\
        butterfly3(a3, a4, a5, smem, twiddles, x2, block_size);\n\
        butterfly3(a6, a7, a8, smem, twiddles, x3, block_size);\n\
        butterfly3(a9, a10, a11, smem, twiddles, x4, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix5(__local CT* smem, __global const CT* twiddles, const int x, const int block_size, const int t)\n\
{\n\
    const int k = x % block_size;\n\
    CT a0, a1, a2, a3, a4;\n\
\n\
    if (x < t)\n\
    {\n\
        a0 = smem[x]; a1 = smem[x + t]; a2 = smem[x+2*t]; a3 = smem[x+3*t]; a4 = smem[x+4*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x < t)\n\
        butterfly5(a0, a1, a2, a3, a4, smem, twiddles, x, block_size);\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
__attribute__((always_inline))\n\
void fft_radix5_B2(__local CT* smem, __global const CT* twiddles, const int x1, const int block_size, const int t)\n\
{\n\
    const int x2 = x1+t/2;\n\
    CT a0, a1, a2, a3, a4, a5, a6, a7, a8, a9;\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        a0 = smem[x1]; a1 = smem[x1 + t]; a2 = smem[x1+2*t]; a3 = smem[x1+3*t]; a4 = smem[x1+4*t];\n\
        a5 = smem[x2]; a6 = smem[x2 + t]; a7 = smem[x2+2*t]; a8 = smem[x2+3*t]; a9 = smem[x2+4*t];\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x1 < t/2)\n\
    {\n\
        butterfly5(a0, a1, a2, a3, a4, smem, twiddles, x1, block_size);\n\
        butterfly5(a5, a6, a7, a8, a9, smem, twiddles, x2, block_size);\n\
    }\n\
\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
}\n\
\n\
#ifdef DFT_SCALE\n\
#define SCALE_VAL(x, scale) x*scale\n\
#else\n\
#define SCALE_VAL(x, scale) x\n\
#endif\n\
\n\
__kernel void fft_multi_radix_rows(__global const uchar* src_ptr, int src_step, int src_offset, int src_rows, int src_cols,\n\
                                   __global uchar* dst_ptr, int dst_step, int dst_offset, int dst_rows, int dst_cols,\n\
                                   __global CT* twiddles_ptr, int twiddles_step, int twiddles_offset, const int t, const int nz)\n\
{\n\
    const int x = get_global_id(0);\n\
    const int y = get_group_id(1);\n\
    const int block_size = LOCAL_SIZE/kercn;\n\
    __local CT smem[LOCAL_SIZE];  // used in (y < nz) code branch only, but should be declared in the outermost scope of a kernel function\n\
    if (y < nz)\n\
    {\n\
        __global const CT* twiddles = (__global const CT*)(twiddles_ptr + twiddles_offset);\n\
        const int ind = x;\n\
#ifdef IS_1D\n\
        FT scale = (FT) 1/dst_cols;\n\
#else\n\
        FT scale = (FT) 1/(dst_cols*dst_rows);\n\
#endif\n\
\n\
#ifdef COMPLEX_INPUT\n\
        __global const CT* src = (__global const CT*)(src_ptr + mad24(y, src_step, mad24(x, (int)(sizeof(CT)), src_offset)));\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
            smem[x+i*block_size] = src[i*block_size];\n\
#else\n\
        __global const FT* src = (__global const FT*)(src_ptr + mad24(y, src_step, mad24(x, (int)sizeof(FT), src_offset)));\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
            smem[x+i*block_size] = (CT)(src[i*block_size], 0.f);\n\
#endif\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
        RADIX_PROCESS;\n\
\n\
#ifdef COMPLEX_OUTPUT\n\
#ifdef NO_CONJUGATE\n\
        // copy result without complex conjugate\n\
        const int cols = dst_cols/2 + 1;\n\
#else\n\
        const int cols = dst_cols;\n\
#endif\n\
\n\
        __global CT* dst = (__global CT*)(dst_ptr + mad24(y, dst_step, dst_offset));\n\
        #pragma unroll\n\
        for (int i=x; i<cols; i+=block_size)\n\
            dst[i] = SCALE_VAL(smem[i], scale);\n\
#ifdef REAL_INPUT\n\
#ifdef COMPLEX_OUTPUT\n\
#ifdef IS_1D\n\
        for(int i=x+1; i < (dst_cols+1)/2; i+=block_size)\n\
        {\n\
            dst[dst_cols-i] = (CT)(SCALE_VAL(smem[i].x, scale), SCALE_VAL(-smem[i].y, scale));\n\
        }\n\
#endif\n\
#endif\n\
#endif\n\
#else\n\
        // pack row to CCS\n\
        __local FT* smem_1cn = (__local FT*) smem;\n\
        __global FT* dst = (__global FT*)(dst_ptr + mad24(y, dst_step, dst_offset));\n\
        for (int i=x; i<dst_cols-1; i+=block_size)\n\
            dst[i+1] = SCALE_VAL(smem_1cn[i+2], scale);\n\
        if (x == 0)\n\
            dst[0] = SCALE_VAL(smem_1cn[0], scale);\n\
#endif\n\
    }\n\
    else\n\
    {\n\
        // fill with zero other rows\n\
#ifdef COMPLEX_OUTPUT\n\
        __global CT* dst = (__global CT*)(dst_ptr + mad24(y, dst_step, dst_offset));\n\
#else\n\
        __global FT* dst = (__global FT*)(dst_ptr + mad24(y, dst_step, dst_offset));\n\
#endif\n\
        #pragma unroll\n\
        for (int i=x; i<dst_cols; i+=block_size)\n\
            dst[i] = 0.f;\n\
    }\n\
}\n\
\n\
__kernel void fft_multi_radix_cols(__global const uchar* src_ptr, int src_step, int src_offset, int src_rows, int src_cols,\n\
                                   __global uchar* dst_ptr, int dst_step, int dst_offset, int dst_rows, int dst_cols,\n\
                                   __global CT* twiddles_ptr, int twiddles_step, int twiddles_offset, const int t, const int nz)\n\
{\n\
    const int x = get_group_id(0);\n\
    const int y = get_global_id(1);\n\
\n\
    __local CT smem[LOCAL_SIZE];  // used in (x < nz) code branch only, but should be declared in the outermost scope of a kernel function\n\
    if (x < nz)\n\
    {\n\
        __global const uchar* src = src_ptr + mad24(y, src_step, mad24(x, (int)(sizeof(CT)), src_offset));\n\
        __global const CT* twiddles = (__global const CT*)(twiddles_ptr + twiddles_offset);\n\
        const int ind = y;\n\
        const int block_size = LOCAL_SIZE/kercn;\n\
        FT scale = 1.f/(dst_rows*dst_cols);\n\
\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
            smem[y+i*block_size] = *((__global const CT*)(src + i*block_size*src_step));\n\
\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
        RADIX_PROCESS;\n\
\n\
#ifdef COMPLEX_OUTPUT\n\
        __global uchar* dst = dst_ptr + mad24(y, dst_step, mad24(x, (int)(sizeof(CT)), dst_offset));\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
            *((__global CT*)(dst + i*block_size*dst_step)) = SCALE_VAL(smem[y + i*block_size], scale);\n\
#else\n\
        if (x == 0)\n\
        {\n\
            // pack first column to CCS\n\
            __local FT* smem_1cn = (__local FT*) smem;\n\
            __global uchar* dst = dst_ptr + mad24(y+1, dst_step, dst_offset);\n\
            for (int i=y; i<dst_rows-1; i+=block_size, dst+=dst_step*block_size)\n\
                *((__global FT*) dst) = SCALE_VAL(smem_1cn[i+2], scale);\n\
            if (y == 0)\n\
                *((__global FT*) (dst_ptr + dst_offset)) = SCALE_VAL(smem_1cn[0], scale);\n\
        }\n\
        else if (x == (dst_cols+1)/2)\n\
        {\n\
            // pack last column to CCS (if needed)\n\
            __local FT* smem_1cn = (__local FT*) smem;\n\
            __global uchar* dst = dst_ptr + mad24(dst_cols-1, (int)sizeof(FT), mad24(y+1, dst_step, dst_offset));\n\
            for (int i=y; i<dst_rows-1; i+=block_size, dst+=dst_step*block_size)\n\
                *((__global FT*) dst) = SCALE_VAL(smem_1cn[i+2], scale);\n\
            if (y == 0)\n\
                *((__global FT*) (dst_ptr + mad24(dst_cols-1, (int)sizeof(FT), dst_offset))) = SCALE_VAL(smem_1cn[0], scale);\n\
        }\n\
        else\n\
        {\n\
            __global uchar* dst = dst_ptr + mad24(x, (int)sizeof(FT)*2, mad24(y, dst_step, dst_offset - (int)sizeof(FT)));\n\
            #pragma unroll\n\
            for (int i=y; i<dst_rows; i+=block_size, dst+=block_size*dst_step)\n\
                vstore2(SCALE_VAL(smem[i], scale), 0, (__global FT*) dst);\n\
        }\n\
#endif\n\
    }\n\
}\n\
\n\
__kernel void ifft_multi_radix_rows(__global const uchar* src_ptr, int src_step, int src_offset, int src_rows, int src_cols,\n\
                                    __global uchar* dst_ptr, int dst_step, int dst_offset, int dst_rows, int dst_cols,\n\
                                    __global CT* twiddles_ptr, int twiddles_step, int twiddles_offset, const int t, const int nz)\n\
{\n\
    const int x = get_global_id(0);\n\
    const int y = get_group_id(1);\n\
    const int block_size = LOCAL_SIZE/kercn;\n\
#ifdef IS_1D\n\
    const FT scale = (FT) 1/dst_cols;\n\
#else\n\
    const FT scale = (FT) 1/(dst_cols*dst_rows);\n\
#endif\n\
\n\
    __local CT smem[LOCAL_SIZE];  // used in (y < nz) code branch only, but should be declared in the outermost scope of a kernel function\n\
    if (y < nz)\n\
    {\n\
        __global const CT* twiddles = (__global const CT*)(twiddles_ptr + twiddles_offset);\n\
        const int ind = x;\n\
\n\
#if defined(COMPLEX_INPUT) && !defined(NO_CONJUGATE)\n\
        __global const CT* src = (__global const CT*)(src_ptr + mad24(y, src_step, mad24(x, (int)(sizeof(CT)), src_offset)));\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
        {\n\
            smem[x+i*block_size].x =  src[i*block_size].x;\n\
            smem[x+i*block_size].y = -src[i*block_size].y;\n\
        }\n\
#else\n\
\n\
    #if !defined(REAL_INPUT) && defined(NO_CONJUGATE)\n\
        __global const CT* src = (__global const CT*)(src_ptr + mad24(y, src_step, mad24(2, (int)sizeof(FT), src_offset)));\n\
\n\
        #pragma unroll\n\
        for (int i=x; i<(LOCAL_SIZE-1)/2; i+=block_size)\n\
        {\n\
            smem[i+1].x = src[i].x;\n\
            smem[i+1].y = -src[i].y;\n\
            smem[LOCAL_SIZE-i-1] = src[i];\n\
        }\n\
    #else\n\
\n\
        #pragma unroll\n\
        for (int i=x; i<(LOCAL_SIZE-1)/2; i+=block_size)\n\
        {\n\
            CT src = vload2(0, (__global const FT*)(src_ptr + mad24(y, src_step, mad24(2*i+1, (int)sizeof(FT), src_offset))));\n\
\n\
            smem[i+1].x = src.x;\n\
            smem[i+1].y = -src.y;\n\
            smem[LOCAL_SIZE-i-1] = src;\n\
        }\n\
\n\
    #endif\n\
\n\
        if (x==0)\n\
        {\n\
            smem[0].x = *(__global const FT*)(src_ptr + mad24(y, src_step, src_offset));\n\
            smem[0].y = 0.f;\n\
\n\
            if(LOCAL_SIZE % 2 ==0)\n\
            {\n\
                #if !defined(REAL_INPUT) && defined(NO_CONJUGATE)\n\
                smem[LOCAL_SIZE/2].x = src[LOCAL_SIZE/2-1].x;\n\
                #else\n\
                smem[LOCAL_SIZE/2].x = *(__global const FT*)(src_ptr + mad24(y, src_step, mad24(LOCAL_SIZE-1, (int)sizeof(FT), src_offset)));\n\
                #endif\n\
                smem[LOCAL_SIZE/2].y = 0.f;\n\
            }\n\
        }\n\
#endif\n\
\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
        RADIX_PROCESS;\n\
\n\
        // copy data to dst\n\
#ifdef COMPLEX_OUTPUT\n\
        __global CT* dst = (__global CT*)(dst_ptr + mad24(y, dst_step, mad24(x, (int)(sizeof(CT)), dst_offset)));\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
        {\n\
            dst[i*block_size].x = SCALE_VAL(smem[x + i*block_size].x, scale);\n\
            dst[i*block_size].y = SCALE_VAL(-smem[x + i*block_size].y, scale);\n\
        }\n\
#else\n\
        __global FT* dst = (__global FT*)(dst_ptr + mad24(y, dst_step, mad24(x, (int)(sizeof(FT)), dst_offset)));\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
        {\n\
            dst[i*block_size] = SCALE_VAL(smem[x + i*block_size].x, scale);\n\
        }\n\
#endif\n\
    }\n\
    else\n\
    {\n\
        // fill with zero other rows\n\
#ifdef COMPLEX_OUTPUT\n\
        __global CT* dst = (__global CT*)(dst_ptr + mad24(y, dst_step, dst_offset));\n\
#else\n\
        __global FT* dst = (__global FT*)(dst_ptr + mad24(y, dst_step, dst_offset));\n\
#endif\n\
        #pragma unroll\n\
        for (int i=x; i<dst_cols; i+=block_size)\n\
            dst[i] = 0.f;\n\
    }\n\
}\n\
\n\
__kernel void ifft_multi_radix_cols(__global const uchar* src_ptr, int src_step, int src_offset, int src_rows, int src_cols,\n\
                              __global uchar* dst_ptr, int dst_step, int dst_offset, int dst_rows, int dst_cols,\n\
                              __global CT* twiddles_ptr, int twiddles_step, int twiddles_offset, const int t, const int nz)\n\
{\n\
    const int x = get_group_id(0);\n\
    const int y = get_global_id(1);\n\
\n\
    __local CT smem[LOCAL_SIZE];  // used in (x < nz) code branch only, but should be declared in the outermost scope of a kernel function\n\
    if (x < nz)\n\
    {\n\
#ifdef COMPLEX_INPUT\n\
        __global const uchar* src = src_ptr + mad24(y, src_step, mad24(x, (int)(sizeof(CT)), src_offset));\n\
        __global uchar* dst = dst_ptr + mad24(y, dst_step, mad24(x, (int)(sizeof(CT)), dst_offset));\n\
        __global const CT* twiddles = (__global const CT*)(twiddles_ptr + twiddles_offset);\n\
        const int ind = y;\n\
        const int block_size = LOCAL_SIZE/kercn;\n\
\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
        {\n\
            CT temp = *((__global const CT*)(src + i*block_size*src_step));\n\
            smem[y+i*block_size].x =  temp.x;\n\
            smem[y+i*block_size].y =  -temp.y;\n\
        }\n\
\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
        RADIX_PROCESS;\n\
\n\
        // copy data to dst\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
        {\n\
           __global CT* res = (__global CT*)(dst + i*block_size*dst_step);\n\
            res[0].x = smem[y + i*block_size].x;\n\
            res[0].y = -smem[y + i*block_size].y;\n\
        }\n\
#else\n\
        __global const CT* twiddles = (__global const CT*)(twiddles_ptr + twiddles_offset);\n\
        const int ind = y;\n\
        const int block_size = LOCAL_SIZE/kercn;\n\
\n\
#ifdef EVEN\n\
        if (x!=0 && (x!=(nz-1)))\n\
#else\n\
        if (x!=0)\n\
#endif\n\
        {\n\
            __global const uchar* src = src_ptr + mad24(y, src_step, mad24(2*x-1, (int)sizeof(FT), src_offset));\n\
            #pragma unroll\n\
            for (int i=0; i<kercn; i++)\n\
            {\n\
                CT temp = vload2(0, (__global const FT*)(src + i*block_size*src_step));\n\
                smem[y+i*block_size].x = temp.x;\n\
                smem[y+i*block_size].y = -temp.y;\n\
            }\n\
        }\n\
        else\n\
        {\n\
            int ind = x==0 ? 0: 2*x-1;\n\
            __global const FT* src = (__global const FT*)(src_ptr + mad24(1, src_step, mad24(ind, (int)sizeof(FT), src_offset)));\n\
            int step = src_step/(int)sizeof(FT);\n\
\n\
            #pragma unroll\n\
            for (int i=y; i<(LOCAL_SIZE-1)/2; i+=block_size)\n\
            {\n\
                smem[i+1].x = src[2*i*step];\n\
                smem[i+1].y = -src[(2*i+1)*step];\n\
\n\
                smem[LOCAL_SIZE-i-1].x = src[2*i*step];\n\
                smem[LOCAL_SIZE-i-1].y = src[(2*i+1)*step];\n\
            }\n\
            if (y==0)\n\
            {\n\
                smem[0].x = *(__global const FT*)(src_ptr + mad24(ind, (int)sizeof(FT), src_offset));\n\
                smem[0].y = 0.f;\n\
\n\
                if(LOCAL_SIZE % 2 ==0)\n\
                {\n\
                    smem[LOCAL_SIZE/2].x = src[(LOCAL_SIZE-2)*step];\n\
                    smem[LOCAL_SIZE/2].y = 0.f;\n\
                }\n\
            }\n\
        }\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
        RADIX_PROCESS;\n\
\n\
        // copy data to dst\n\
        __global uchar* dst = dst_ptr + mad24(y, dst_step, mad24(x, (int)(sizeof(CT)), dst_offset));\n\
\n\
        #pragma unroll\n\
        for (int i=0; i<kercn; i++)\n\
        {\n\
            __global CT* res = (__global CT*)(dst + i*block_size*dst_step);\n\
            res[0].x =  smem[y + i*block_size].x;\n\
            res[0].y = -smem[y + i*block_size].y;\n\
        }\n\
#endif\n\
    }\n\
}");

ProgramSource flip_oclsrc = ProgramSource("\
#if kercn != 3\n\
#define loadpix(addr) *(__global const T *)(addr)\n\
#define storepix(val, addr)  *(__global T *)(addr) = val\n\
#define storepix_2(val0, val1, addr0, addr1) \\\n\
    *(__global T *)(addr0) = val0; *(__global T *)(addr1) = val1\n\
#define TSIZE (int)sizeof(T)\n\
#else\n\
#define loadpix(addr) vload3(0, (__global const T1 *)(addr))\n\
#define storepix(val, addr) vstore3(val, 0, (__global T1 *)(addr))\n\
#if DEPTH == 2 || DEPTH == 3\n\
#define storepix_2(val0, val1, addr0, addr1) \\\n\
    ((__global T1 *)(addr0))[0] = val0.x; \\\n\
    ((__global T1 *)(addr1))[0] = val1.x; \\\n\
    ((__global T1 *)(addr0))[1] = val0.y; \\\n\
    ((__global T1 *)(addr1))[1] = val1.y; \\\n\
    ((__global T1 *)(addr0))[2] = val0.z; \\\n\
    ((__global T1 *)(addr1))[2] = val1.z\n\
#else\n\
#define storepix_2(val0, val1, addr0, addr1) \\\n\
    storepix(val0, addr0); \\\n\
    storepix(val1, addr1)\n\
#endif\n\
#define TSIZE ((int)sizeof(T1)*3)\n\
#endif\n\
\n\
__kernel void arithm_flip_rows(__global const uchar * srcptr, int src_step, int src_offset,\n\
                               __global uchar * dstptr, int dst_step, int dst_offset,\n\
                               int rows, int cols, int thread_rows, int thread_cols)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * PIX_PER_WI_Y;\n\
\n\
    if (x < cols)\n\
    {\n\
        int src_index0 = mad24(y0, src_step, mad24(x, TSIZE, src_offset));\n\
        int src_index1 = mad24(rows - y0 - 1, src_step, mad24(x, TSIZE, src_offset));\n\
        int dst_index0 = mad24(y0, dst_step, mad24(x, TSIZE, dst_offset));\n\
        int dst_index1 = mad24(rows - y0 - 1, dst_step, mad24(x, TSIZE, dst_offset));\n\
\n\
        #pragma unroll\n\
        for (int y = y0, y1 = min(thread_rows, y0 + PIX_PER_WI_Y); y < y1; ++y)\n\
        {\n\
            T src0 = loadpix(srcptr + src_index0);\n\
            T src1 = loadpix(srcptr + src_index1);\n\
\n\
            storepix_2(src1, src0, dstptr + dst_index0, dstptr + dst_index1);\n\
\n\
            src_index0 += src_step;\n\
            src_index1 -= src_step;\n\
            dst_index0 += dst_step;\n\
            dst_index1 -= dst_step;\n\
        }\n\
    }\n\
}\n\
\n\
__kernel void arithm_flip_rows_cols(__global const uchar * srcptr, int src_step, int src_offset,\n\
                                    __global uchar * dstptr, int dst_step, int dst_offset,\n\
                                    int rows, int cols, int thread_rows, int thread_cols)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1)*PIX_PER_WI_Y;\n\
\n\
    if (x < cols)\n\
    {\n\
        int src_index0 = mad24(y0, src_step, mad24(x, TSIZE, src_offset));\n\
        int src_index1 = mad24(rows - y0 - 1, src_step, mad24(cols - x - 1, TSIZE, src_offset));\n\
        int dst_index0 = mad24(y0, dst_step, mad24(x, TSIZE, dst_offset));\n\
        int dst_index1 = mad24(rows - y0 - 1, dst_step, mad24(cols - x - 1, TSIZE, dst_offset));\n\
\n\
        #pragma unroll\n\
        for (int y = y0, y1 = min(thread_rows, y0 + PIX_PER_WI_Y); y < y1; ++y)\n\
        {\n\
            T src0 = loadpix(srcptr + src_index0);\n\
            T src1 = loadpix(srcptr + src_index1);\n\
\n\
#if kercn == 2\n\
#if cn == 1\n\
            src0 = src0.s10;\n\
            src1 = src1.s10;\n\
#endif\n\
#elif kercn == 4\n\
#if cn == 1\n\
            src0 = src0.s3210;\n\
            src1 = src1.s3210;\n\
#elif cn == 2\n\
            src0 = src0.s2301;\n\
            src1 = src1.s2301;\n\
#endif\n\
#endif\n\
\n\
            storepix_2(src1, src0, dstptr + dst_index0, dstptr + dst_index1);\n\
\n\
            src_index0 += src_step;\n\
            src_index1 -= src_step;\n\
            dst_index0 += dst_step;\n\
            dst_index1 -= dst_step;\n\
        }\n\
    }\n\
}\n\
\n\
__kernel void arithm_flip_cols(__global const uchar * srcptr, int src_step, int src_offset,\n\
                               __global uchar * dstptr, int dst_step, int dst_offset,\n\
                               int rows, int cols, int thread_rows, int thread_cols)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1)*PIX_PER_WI_Y;\n\
\n\
    if (x < thread_cols)\n\
    {\n\
        int src_index0 = mad24(y0, src_step, mad24(x, TSIZE, src_offset));\n\
        int src_index1 = mad24(y0, src_step, mad24(cols - x - 1, TSIZE, src_offset));\n\
        int dst_index0 = mad24(y0, dst_step, mad24(x, TSIZE, dst_offset));\n\
        int dst_index1 = mad24(y0, dst_step, mad24(cols - x - 1, TSIZE, dst_offset));\n\
\n\
        #pragma unroll\n\
        for (int y = y0, y1 = min(rows, y0 + PIX_PER_WI_Y); y < y1; ++y)\n\
        {\n\
            T src0 = loadpix(srcptr + src_index0);\n\
            T src1 = loadpix(srcptr + src_index1);\n\
\n\
#if kercn == 2\n\
#if cn == 1\n\
            src0 = src0.s10;\n\
            src1 = src1.s10;\n\
#endif\n\
#elif kercn == 4\n\
#if cn == 1\n\
            src0 = src0.s3210;\n\
            src1 = src1.s3210;\n\
#elif cn == 2\n\
            src0 = src0.s2301;\n\
            src1 = src1.s2301;\n\
#endif\n\
#endif\n\
\n\
            storepix_2(src1, src0, dstptr + dst_index0, dstptr + dst_index1);\n\
\n\
            src_index0 += src_step;\n\
            src_index1 += src_step;\n\
            dst_index0 += dst_step;\n\
            dst_index1 += dst_step;\n\
        }\n\
    }\n\
}");

ProgramSource gemm_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#define TSIZE  (int)sizeof(T)\n\
#define WTSIZE (int)sizeof(WT)\n\
\n\
#define IND_A mad24(y, A_step, A_offset)\n\
#define IND_B mad24(x, WTSIZE, B_offset)\n\
#define STEP_B B_step / WTSIZE\n\
\n\
#define LOCAL_SIZE_ODD (LOCAL_SIZE + 1)\n\
\n\
#if cn==2\n\
#if kercn==2\n\
#define MUL(a, b)\\\n\
    {\\\n\
    sum.x += fma(a.x, b.x, - a.y * b.y);\\\n\
    sum.y += fma(a.x, b.y, a.y * b.x);\\\n\
    }\n\
#else\n\
#define MUL(a, b)\\\n\
    {\\\n\
    sum.x += fma(a.x, b.x, - a.y * b.y);\\\n\
    sum.y += fma(a.x, b.y, a.y * b.x);\\\n\
    sum.z += fma(a.x, b.z, - a.y * b.w);\\\n\
    sum.w += fma(a.x, b.w, a.y * b.z);\\\n\
    }\n\
#endif\n\
#else\n\
#define MUL(a, b) sum = fma(a, b, sum);\n\
#endif\n\
\n\
__kernel void gemm(__global const uchar * A_ptr, int A_step, int A_offset,\n\
                   __global const uchar * B_ptr, int B_step, int B_offset,\n\
                   __global uchar * D_ptr, int D_step, int D_offset, int D_rows, int D_cols,\n\
                   int n, T1 alpha, T1 beta)\n\
{\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
\n\
    int lidx = get_local_id(0);\n\
    int lidy = get_local_id(1);\n\
\n\
    __global const T* A = (__global const T*)(A_ptr + IND_A);\n\
    __global const WT* B = (__global const WT*)(B_ptr + IND_B);\n\
\n\
    WT sum = (WT)(0);\n\
\n\
#if LOCAL_SIZE == 1\n\
\n\
    if (x < D_cols && y < D_rows)\n\
    {\n\
        for (int i = 0; i < n; ++i)\n\
            MUL(A[i], B[i*STEP_B]);\n\
#else\n\
\n\
    __local T  a_local[LOCAL_SIZE_ODD*LOCAL_SIZE];\n\
    __local WT b_local[LOCAL_SIZE_ODD*LOCAL_SIZE];\n\
\n\
    int reps;\n\
#if NO_MULT\n\
    reps = (n + LOCAL_SIZE-1)/LOCAL_SIZE;\n\
#else\n\
    reps = n/LOCAL_SIZE;\n\
#endif\n\
\n\
    for (int p = 0; p < reps; ++p)\n\
    {\n\
        if (p * LOCAL_SIZE + lidx < n && y < D_rows)\n\
            a_local[mad24(lidy, LOCAL_SIZE_ODD, lidx)] = A[mad24(p, LOCAL_SIZE, lidx)];\n\
        if (p * LOCAL_SIZE + lidy < n && x < D_cols)\n\
            b_local[mad24(lidy, LOCAL_SIZE_ODD, lidx)] = B[mad24(p, LOCAL_SIZE, lidy)*STEP_B];\n\
\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
        if (x < D_cols && y < D_rows)\n\
        {\n\
#if NO_MULT\n\
            int ie = min(LOCAL_SIZE, n - p * LOCAL_SIZE);\n\
            for (int i = 0; i < ie; ++i)\n\
#else\n\
            for (int i = 0; i < LOCAL_SIZE; ++i)\n\
#endif\n\
                MUL(a_local[mad24(lidy, LOCAL_SIZE_ODD, i)], b_local[mad24(i, LOCAL_SIZE_ODD, lidx)]);\n\
        }\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
    }\n\
\n\
    if (x < D_cols && y < D_rows)\n\
    {\n\
#endif\n\
        __global WT* D = (__global WT*)(D_ptr + mad24(y, D_step, mad24(x, WTSIZE, D_offset)));\n\
#if HAVE_C\n\
        D[0] = mad(alpha, sum, D[0]*beta);\n\
#else\n\
        D[0] = alpha * sum;\n\
#endif\n\
    }\n\
}");

ProgramSource inrange_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
__kernel void inrange(__global const uchar * src1ptr, int src1_step, int src1_offset,\n\
                      __global uchar * dstptr, int dst_step, int dst_offset, int dst_rows, int dst_cols,\n\
#ifdef HAVE_SCALAR\n\
                      __global const SRC_T1 * src2, __global const SRC_T1 * src3,\n\
#else\n\
                      __global const uchar * src2ptr, int src2_step, int src2_offset,\n\
                      __global const uchar * src3ptr, int src3_step, int src3_offset,\n\
#endif\n\
                      int rowsPerWI)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < dst_cols)\n\
    {\n\
        int src1_index = mad24(y0, src1_step, mad24(x, (int)sizeof(SRC_T1) * KERCN, src1_offset));\n\
        int dst_index = mad24(y0, dst_step, mad24(x, COLS_PER_WI, dst_offset));\n\
#ifndef HAVE_SCALAR\n\
        int src2_index = mad24(y0, src2_step, mad24(x, (int)sizeof(SRC_T1) * KERCN, src2_offset));\n\
        int src3_index = mad24(y0, src3_step, mad24(x, (int)sizeof(SRC_T1) * KERCN, src3_offset));\n\
#endif\n\
\n\
        for (int y = y0, y1 = min(dst_rows, y0 + rowsPerWI); y < y1; ++y, src1_index += src1_step, dst_index += dst_step)\n\
        {\n\
#if KERCN >= CN && KERCN == 4 && DEPTH <= 4 && !defined HAVE_SCALAR\n\
            SRC_T src1 = *(__global const SRC_T *)(src1ptr + src1_index);\n\
            SRC_T src2 = *(__global const SRC_T *)(src2ptr + src2_index);\n\
            SRC_T src3 = *(__global const SRC_T *)(src3ptr + src3_index);\n\
            __global DST_T * dst = (__global DST_T *)(dstptr + dst_index);\n\
#if CN == 1\n\
            dst[0] = src2 > src1 || src3 < src1 ? (DST_T)(0) : (DST_T)(255);\n\
#elif CN == 2\n\
            dst[0] = (DST_T)(src2.xy > src1.xy || src3.xy < src1.xy ||\n\
                            src2.zw > src1.zw || src3.zw < src1.zw ? (DST_T)(0) : (DST_T)(255);\n\
#elif CN == 4\n\
            dst[0] = (DST_T)(src2.x > src1.x || src3.x < src1.x ||\n\
                src2.y > src1.y || src3.y < src1.y ||\n\
                src2.z > src1.z || src3.z < src1.z ||\n\
                src2.w > src1.w || src3.w < src1.w ? 0 : 255);\n\
#endif\n\
#else\n\
            __global const SRC_T1 * src1 = (__global const SRC_T1 *)(src1ptr + src1_index);\n\
            __global uchar * dst = dstptr + dst_index;\n\
#ifndef HAVE_SCALAR\n\
            __global const SRC_T1 * src2 = (__global const SRC_T1 *)(src2ptr + src2_index);\n\
            __global const SRC_T1 * src3 = (__global const SRC_T1 *)(src3ptr + src3_index);\n\
#endif\n\
\n\
            #pragma unroll\n\
            for (int px = 0; px < COLS_PER_WI; ++px, src1 += CN\n\
#ifndef HAVE_SCALAR\n\
                , src2 += CN, src3 += CN\n\
#endif\n\
                )\n\
            {\n\
                dst[px] = 255;\n\
\n\
                for (int c = 0; c < CN; ++c)\n\
                    if (src2[c] > src1[c] || src3[c] < src1[c])\n\
                    {\n\
                        dst[px] = 0;\n\
                        break;\n\
                    }\n\
            }\n\
#endif // KERCN >= CN\n\
#ifndef HAVE_SCALAR\n\
            src2_index += src2_step;\n\
            src3_index += src3_step;\n\
#endif\n\
        }\n\
    }\n\
}");

ProgramSource intel_gemm_oclsrc = ProgramSource("\
#if defined(cl_intel_subgroups)\n\
#pragma OPENCL EXTENSION  cl_intel_subgroups : enable\n\
#endif\n\
\n\
#if defined(cl_intel_subgroups)\n\
\n\
#define VEC_SIZE        4\n\
#define LWG_HEIGHT      4\n\
#define TILE_M          8\n\
#define TILE_K          16\n\
#define TILE_N          32\n\
\n\
__attribute__((reqd_work_group_size(8, LWG_HEIGHT, 1)))\n\
__kernel void intelblas_gemm_buffer_NN_sp(\n\
    const __global float *src0, int off0,\n\
    const __global float *src1, int off1,\n\
    __global float *dst, int offd,\n\
    int M,\n\
    int N,\n\
    int K,\n\
    float alpha,\n\
    float beta,\n\
    int ldA,\n\
    int ldB,\n\
    int ldC,\n\
    int start_index,\n\
    int stride)\n\
{\n\
    const int group_x = get_group_id(0);\n\
    const int group_y = get_group_id(1);\n\
    const int local_x = get_local_id(0);\n\
    const int local_y = get_local_id(1);\n\
    const int global_x = get_global_id(0);\n\
    const int global_y = get_global_id(1);\n\
\n\
    float4 brow;\n\
    float2 arow0, arow1, arow2, arow3, arow4, arow5, arow6, arow7;\n\
\n\
    __global float *dst_write0 = dst + local_x * VEC_SIZE + ( group_x * TILE_N ) + ( group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M) * ldC + offd;\n\
\n\
    const __global float *src0_read = src0 + local_x * ( TILE_K / 8 ) + ( group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M ) * ldA + start_index + off0;\n\
\n\
    const __global float *src1_read0 = src1 + local_x * VEC_SIZE + ( group_x * TILE_N ) + start_index * ldB + off1;\n\
\n\
    float4 dot00 = (start_index != 0) ? vload4(0, dst_write0)           : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0          )) : (float4)(0.0));\n\
    float4 dot01 = (start_index != 0) ? vload4(0, dst_write0 + 1 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 1 * ldC)) : (float4)(0.0));\n\
    float4 dot02 = (start_index != 0) ? vload4(0, dst_write0 + 2 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 2 * ldC)) : (float4)(0.0));\n\
    float4 dot03 = (start_index != 0) ? vload4(0, dst_write0 + 3 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 3 * ldC)) : (float4)(0.0));\n\
    float4 dot04 = (start_index != 0) ? vload4(0, dst_write0 + 4 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 4 * ldC)) : (float4)(0.0));\n\
    float4 dot05 = (start_index != 0) ? vload4(0, dst_write0 + 5 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 5 * ldC)) : (float4)(0.0));\n\
    float4 dot06 = (start_index != 0) ? vload4(0, dst_write0 + 6 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 6 * ldC)) : (float4)(0.0));\n\
    float4 dot07 = (start_index != 0) ? vload4(0, dst_write0 + 7 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 7 * ldC)) : (float4)(0.0));\n\
\n\
    int end_index = min(start_index + stride, K);\n\
    int w = start_index;\n\
    while( w + TILE_K <= end_index ) {\n\
        arow0 = (float)alpha * vload2(0, src0_read + 0 * ldA);\n\
        arow1 = (float)alpha * vload2(0, src0_read + 1 * ldA);\n\
        arow2 = (float)alpha * vload2(0, src0_read + 2 * ldA);\n\
        arow3 = (float)alpha * vload2(0, src0_read + 3 * ldA);\n\
        arow4 = (float)alpha * vload2(0, src0_read + 4 * ldA);\n\
        arow5 = (float)alpha * vload2(0, src0_read + 5 * ldA);\n\
        arow6 = (float)alpha * vload2(0, src0_read + 6 * ldA);\n\
        arow7 = (float)alpha * vload2(0, src0_read + 7 * ldA);\n\
\n\
#define MM_DOT_PRODUCT(index, suffix)   \\\n\
        brow = vload4(0, src1_read0);  src1_read0 += ldB; \\\n\
        dot00 = mad((float4)(intel_sub_group_shuffle(arow0.s##suffix,index)),brow,dot00); \\\n\
        dot01 = mad((float4)(intel_sub_group_shuffle(arow1.s##suffix,index)),brow,dot01); \\\n\
        dot02 = mad((float4)(intel_sub_group_shuffle(arow2.s##suffix,index)),brow,dot02); \\\n\
        dot03 = mad((float4)(intel_sub_group_shuffle(arow3.s##suffix,index)),brow,dot03); \\\n\
        dot04 = mad((float4)(intel_sub_group_shuffle(arow4.s##suffix,index)),brow,dot04); \\\n\
        dot05 = mad((float4)(intel_sub_group_shuffle(arow5.s##suffix,index)),brow,dot05); \\\n\
        dot06 = mad((float4)(intel_sub_group_shuffle(arow6.s##suffix,index)),brow,dot06); \\\n\
        dot07 = mad((float4)(intel_sub_group_shuffle(arow7.s##suffix,index)),brow,dot07);\n\
\n\
        MM_DOT_PRODUCT(0,0);\n\
        MM_DOT_PRODUCT(0,1);\n\
        MM_DOT_PRODUCT(1,0);\n\
        MM_DOT_PRODUCT(1,1);\n\
        MM_DOT_PRODUCT(2,0);\n\
        MM_DOT_PRODUCT(2,1);\n\
        MM_DOT_PRODUCT(3,0);\n\
        MM_DOT_PRODUCT(3,1);\n\
        MM_DOT_PRODUCT(4,0);\n\
        MM_DOT_PRODUCT(4,1);\n\
        MM_DOT_PRODUCT(5,0);\n\
        MM_DOT_PRODUCT(5,1);\n\
        MM_DOT_PRODUCT(6,0);\n\
        MM_DOT_PRODUCT(6,1);\n\
        MM_DOT_PRODUCT(7,0);\n\
        MM_DOT_PRODUCT(7,1);\n\
#undef MM_DOT_PRODUCT\n\
\n\
        src0_read += TILE_K;\n\
        w += TILE_K;\n\
    }\n\
\n\
    vstore4(dot00, 0, dst_write0); dst_write0 += ldC;\n\
    vstore4(dot01, 0, dst_write0); dst_write0 += ldC;\n\
    vstore4(dot02, 0, dst_write0); dst_write0 += ldC;\n\
    vstore4(dot03, 0, dst_write0); dst_write0 += ldC;\n\
    vstore4(dot04, 0, dst_write0); dst_write0 += ldC;\n\
    vstore4(dot05, 0, dst_write0); dst_write0 += ldC;\n\
    vstore4(dot06, 0, dst_write0); dst_write0 += ldC;\n\
    vstore4(dot07, 0, dst_write0); dst_write0 += ldC;\n\
}\n\
\n\
#undef VEC_SIZE\n\
#undef LWG_HEIGHT\n\
#undef TILE_M\n\
#undef TILE_K\n\
#undef TILE_N\n\
\n\
#define VEC_SIZE        4\n\
#define LWG_HEIGHT      4\n\
#define TILE_M          8\n\
#define TILE_K          16\n\
#define TILE_N          32\n\
\n\
__attribute__((reqd_work_group_size(8, LWG_HEIGHT, 1)))\n\
__kernel void intelblas_gemm_buffer_NN(\n\
    const __global float *src0, int off0,\n\
    const __global float *src1, int off1,\n\
    __global float *dst, int offd,\n\
    int M,\n\
    int N,\n\
    int K,\n\
    float alpha,\n\
    float beta,\n\
    int ldA,\n\
    int ldB,\n\
    int ldC,\n\
    int start_index,\n\
    int stride)\n\
{\n\
    const int group_x = get_group_id(0);\n\
    const int group_y = get_group_id(1);\n\
    const int local_x = get_local_id(0);\n\
    const int local_y = get_local_id(1);\n\
    const int global_x = get_global_id(0);\n\
    const int global_y = get_global_id(1);\n\
\n\
    float4 brow;\n\
    float2 arow0, arow1, arow2, arow3, arow4, arow5, arow6, arow7;\n\
\n\
    __global float *dst_write0 = dst + local_x * VEC_SIZE + ( group_x * TILE_N ) + ( group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M) * ldC + offd;\n\
\n\
    const __global float *src0_read = src0 + local_x * ( TILE_K / 8 ) + ( group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M ) * ldA + start_index + off0;\n\
\n\
    const __global float *src1_read0 = src1 + local_x * VEC_SIZE + ( group_x * TILE_N ) + start_index * ldB + off1;\n\
\n\
    int border = -(group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M);\n\
\n\
    int row0 = mad24(global_y, TILE_M, 0) < M ? 0 : border;\n\
    int row1 = mad24(global_y, TILE_M, 1) < M ? 1 : border;\n\
    int row2 = mad24(global_y, TILE_M, 2) < M ? 2 : border;\n\
    int row3 = mad24(global_y, TILE_M, 3) < M ? 3 : border;\n\
    int row4 = mad24(global_y, TILE_M, 4) < M ? 4 : border;\n\
    int row5 = mad24(global_y, TILE_M, 5) < M ? 5 : border;\n\
    int row6 = mad24(global_y, TILE_M, 6) < M ? 6 : border;\n\
    int row7 = mad24(global_y, TILE_M, 7) < M ? 7 : border;\n\
\n\
    float4 dot00 = (start_index != 0) ? vload4(0, dst_write0)           : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0          )) : (float4)(0.0));\n\
    float4 dot01 = (start_index != 0) ? vload4(0, dst_write0 + 1 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 1 * ldC)) : (float4)(0.0));\n\
    float4 dot02 = (start_index != 0) ? vload4(0, dst_write0 + 2 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 2 * ldC)) : (float4)(0.0));\n\
    float4 dot03 = (start_index != 0) ? vload4(0, dst_write0 + 3 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 3 * ldC)) : (float4)(0.0));\n\
    float4 dot04 = (start_index != 0) ? vload4(0, dst_write0 + 4 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 4 * ldC)) : (float4)(0.0));\n\
    float4 dot05 = (start_index != 0) ? vload4(0, dst_write0 + 5 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 5 * ldC)) : (float4)(0.0));\n\
    float4 dot06 = (start_index != 0) ? vload4(0, dst_write0 + 6 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 6 * ldC)) : (float4)(0.0));\n\
    float4 dot07 = (start_index != 0) ? vload4(0, dst_write0 + 7 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 7 * ldC)) : (float4)(0.0));\n\
\n\
    int end_index = min(start_index + stride, K);\n\
    int w = start_index;\n\
    while( w + TILE_K <= end_index ) {\n\
        arow0 = (float)alpha * vload2(0, src0_read + row0 * ldA);\n\
        arow1 = (float)alpha * vload2(0, src0_read + row1 * ldA);\n\
        arow2 = (float)alpha * vload2(0, src0_read + row2 * ldA);\n\
        arow3 = (float)alpha * vload2(0, src0_read + row3 * ldA);\n\
        arow4 = (float)alpha * vload2(0, src0_read + row4 * ldA);\n\
        arow5 = (float)alpha * vload2(0, src0_read + row5 * ldA);\n\
        arow6 = (float)alpha * vload2(0, src0_read + row6 * ldA);\n\
        arow7 = (float)alpha * vload2(0, src0_read + row7 * ldA);\n\
\n\
#define MM_DOT_PRODUCT(index,suffix) \\\n\
        brow = vload4(0, src1_read0);  src1_read0 += ldB; \\\n\
        dot00 = mad((float4)(intel_sub_group_shuffle(arow0.s##suffix,index)),brow,dot00); \\\n\
        dot01 = mad((float4)(intel_sub_group_shuffle(arow1.s##suffix,index)),brow,dot01); \\\n\
        dot02 = mad((float4)(intel_sub_group_shuffle(arow2.s##suffix,index)),brow,dot02); \\\n\
        dot03 = mad((float4)(intel_sub_group_shuffle(arow3.s##suffix,index)),brow,dot03); \\\n\
        dot04 = mad((float4)(intel_sub_group_shuffle(arow4.s##suffix,index)),brow,dot04); \\\n\
        dot05 = mad((float4)(intel_sub_group_shuffle(arow5.s##suffix,index)),brow,dot05); \\\n\
        dot06 = mad((float4)(intel_sub_group_shuffle(arow6.s##suffix,index)),brow,dot06); \\\n\
        dot07 = mad((float4)(intel_sub_group_shuffle(arow7.s##suffix,index)),brow,dot07);\n\
\n\
        MM_DOT_PRODUCT(0,0);\n\
        MM_DOT_PRODUCT(0,1);\n\
        MM_DOT_PRODUCT(1,0);\n\
        MM_DOT_PRODUCT(1,1);\n\
        MM_DOT_PRODUCT(2,0);\n\
        MM_DOT_PRODUCT(2,1);\n\
        MM_DOT_PRODUCT(3,0);\n\
        MM_DOT_PRODUCT(3,1);\n\
        MM_DOT_PRODUCT(4,0);\n\
        MM_DOT_PRODUCT(4,1);\n\
        MM_DOT_PRODUCT(5,0);\n\
        MM_DOT_PRODUCT(5,1);\n\
        MM_DOT_PRODUCT(6,0);\n\
        MM_DOT_PRODUCT(6,1);\n\
        MM_DOT_PRODUCT(7,0);\n\
        MM_DOT_PRODUCT(7,1);\n\
#undef MM_DOT_PRODUCT\n\
\n\
        src0_read += TILE_K;\n\
        w += TILE_K;\n\
    }\n\
\n\
    if(w < end_index) {\n\
        arow0.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row0 * ldA)[0] : 0.0f;\n\
        arow0.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row0 * ldA)[1] : 0.0f;\n\
        arow1.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row1 * ldA)[0] : 0.0f;\n\
        arow1.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row1 * ldA)[1] : 0.0f;\n\
        arow2.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row2 * ldA)[0] : 0.0f;\n\
        arow2.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row2 * ldA)[1] : 0.0f;\n\
        arow3.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row3 * ldA)[0] : 0.0f;\n\
        arow3.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row3 * ldA)[1] : 0.0f;\n\
        arow4.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row4 * ldA)[0] : 0.0f;\n\
        arow4.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row4 * ldA)[1] : 0.0f;\n\
        arow5.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row5 * ldA)[0] : 0.0f;\n\
        arow5.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row5 * ldA)[1] : 0.0f;\n\
        arow6.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row6 * ldA)[0] : 0.0f;\n\
        arow6.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row6 * ldA)[1] : 0.0f;\n\
        arow7.x = ((w + local_x * 2) < K) ? (float)alpha * (src0_read + row7 * ldA)[0] : 0.0f;\n\
        arow7.y = ((w + local_x * 2 + 1) < K) ? (float)alpha * (src0_read + row7 * ldA)[1] : 0.0f;\n\
\n\
#define MM_DOT_PRODUCT(index,suffix)   \\\n\
        brow = (w < K) ? vload4(0, src1_read0) : (float)0.0f;  src1_read0 += ldB; w++; \\\n\
        dot00 = mad((float4)(intel_sub_group_shuffle( arow0.s##suffix, index )),brow,dot00 ); \\\n\
        dot01 = mad((float4)(intel_sub_group_shuffle( arow1.s##suffix, index )),brow,dot01 ); \\\n\
        dot02 = mad((float4)(intel_sub_group_shuffle( arow2.s##suffix, index )),brow,dot02 ); \\\n\
        dot03 = mad((float4)(intel_sub_group_shuffle( arow3.s##suffix, index )),brow,dot03 ); \\\n\
        dot04 = mad((float4)(intel_sub_group_shuffle( arow4.s##suffix, index )),brow,dot04 ); \\\n\
        dot05 = mad((float4)(intel_sub_group_shuffle( arow5.s##suffix, index )),brow,dot05 ); \\\n\
        dot06 = mad((float4)(intel_sub_group_shuffle( arow6.s##suffix, index )),brow,dot06 ); \\\n\
        dot07 = mad((float4)(intel_sub_group_shuffle( arow7.s##suffix, index )),brow,dot07 );\n\
\n\
        MM_DOT_PRODUCT(0,0);\n\
        MM_DOT_PRODUCT(0,1);\n\
        MM_DOT_PRODUCT(1,0);\n\
        MM_DOT_PRODUCT(1,1);\n\
        MM_DOT_PRODUCT(2,0);\n\
        MM_DOT_PRODUCT(2,1);\n\
        MM_DOT_PRODUCT(3,0);\n\
        MM_DOT_PRODUCT(3,1);\n\
        MM_DOT_PRODUCT(4,0);\n\
        MM_DOT_PRODUCT(4,1);\n\
        MM_DOT_PRODUCT(5,0);\n\
        MM_DOT_PRODUCT(5,1);\n\
        MM_DOT_PRODUCT(6,0);\n\
        MM_DOT_PRODUCT(6,1);\n\
        MM_DOT_PRODUCT(7,0);\n\
        MM_DOT_PRODUCT(7,1);\n\
#undef MM_DOT_PRODUCT\n\
    }\n\
\n\
    if(global_x * 4 < N && global_y * 8 < M) {\n\
        if(mad24(global_x, 4, 3) < N) {\n\
            vstore4(dot00, 0, dst_write0); dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { vstore4(dot01, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { vstore4(dot02, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { vstore4(dot03, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { vstore4(dot04, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { vstore4(dot05, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { vstore4(dot06, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { vstore4(dot07, 0, dst_write0); }\n\
        } else if(mad24(global_x, 4, 2) < N) {\n\
            vstore2(dot00.xy, 0, dst_write0);\n\
            dst_write0[2] = dot00.z;\n\
            dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) {\n\
                vstore2(dot01.xy, 0, dst_write0);\n\
                dst_write0[2] = dot01.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 2) < M) {\n\
                vstore2(dot02.xy, 0, dst_write0);\n\
                dst_write0[2] = dot02.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 3) < M) {\n\
                vstore2(dot03.xy, 0, dst_write0);\n\
                dst_write0[2] = dot03.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 4) < M) {\n\
                vstore2(dot04.xy, 0, dst_write0);\n\
                dst_write0[2] = dot04.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 5) < M) {\n\
                vstore2(dot05.xy, 0, dst_write0);\n\
                dst_write0[2] = dot05.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 6) < M) {\n\
                vstore2(dot06.xy, 0, dst_write0);\n\
                dst_write0[2] = dot06.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 7) < M) {\n\
                vstore2(dot07.xy, 0, dst_write0);\n\
                dst_write0[2] = dot07.z;\n\
            }\n\
        } else if(mad24(global_x, 4, 1) < N) {\n\
            vstore2(dot00.xy, 0, dst_write0); dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { vstore2(dot01.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { vstore2(dot02.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { vstore2(dot03.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { vstore2(dot04.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { vstore2(dot05.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { vstore2(dot06.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { vstore2(dot07.xy, 0, dst_write0); }\n\
        } else {\n\
            dst_write0[0] = dot00.x; dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { dst_write0[0] = dot01.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { dst_write0[0] = dot02.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { dst_write0[0] = dot03.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { dst_write0[0] = dot04.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { dst_write0[0] = dot05.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { dst_write0[0] = dot06.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { dst_write0[0] = dot07.x; }\n\
        }\n\
    }\n\
}\n\
\n\
#undef VEC_SIZE\n\
#undef LWG_HEIGHT\n\
#undef TILE_M\n\
#undef TILE_K\n\
#undef TILE_N\n\
\n\
#define VEC_SIZE        1\n\
#define LWG_HEIGHT      16\n\
#define TILE_M          8\n\
#define TILE_K          32\n\
#define TILE_N          8\n\
#define SLM_BLOCK       512\n\
\n\
/*\n\
   A     K         B.t()   K            D     N\n\
    -----------       -----------        -----------\n\
    |         |       |         |        |         |\n\
  M |         |  x  N |         |  =>  M |         |\n\
    |         |       |         |        |         |\n\
    -----------       -----------        -----------\n\
*/\n\
\n\
__attribute__((reqd_work_group_size(8, LWG_HEIGHT, 1)))\n\
__kernel void intelblas_gemm_buffer_NT(\n\
    const __global float *src0, int off0,\n\
    const __global float *src1, int off1,\n\
    __global float *dst, int offd,\n\
    int M,\n\
    int N,\n\
    int K,\n\
    float alpha,\n\
    float beta,\n\
    int ldA,\n\
    int ldB,\n\
    int ldC)\n\
{\n\
    const int group_x = get_group_id(0);\n\
    const int group_y = get_group_id(1);\n\
    const int local_x = get_local_id(0);\n\
    const int local_y = get_local_id(1);\n\
    const int global_x = get_global_id(0);\n\
    const int global_y = get_global_id(1);\n\
\n\
    float8 dot00 = 0.f;\n\
    float8 dot01 = 0.f;\n\
    float8 dot02 = 0.f;\n\
    float8 dot03 = 0.f;\n\
    float8 dot04 = 0.f;\n\
    float8 dot05 = 0.f;\n\
    float8 dot06 = 0.f;\n\
    float8 dot07 = 0.f;\n\
\n\
    const int dst_row = (global_y * TILE_M);\n\
    __global float *dst_write0 = dst + global_x + dst_row * ldC + offd;\n\
\n\
    const __global float *src0_read00 = src0 + off0;\n\
    const int a_row_base = global_y * TILE_M;\n\
    const int a_col_base = local_x * (TILE_K / 8);  // <= TILE_K - 4\n\
\n\
    const __global float *src1_read00 = src1 + off1;\n\
    const int b_row_base = (group_x * TILE_N);\n\
    //const int b_col_base = 0;\n\
\n\
    __local float slm_brow[8 * SLM_BLOCK];\n\
\n\
    int local_index = mad24(local_y, 8, local_x) * 4;\n\
    int w = 0;\n\
    for (int b_tile = 0; b_tile < K; b_tile += SLM_BLOCK)\n\
    {\n\
#define UPDATE_BROW(_row) \\\n\
        { \\\n\
            float4 brow; \\\n\
            int b_row = b_row_base + _row; \\\n\
            int b_col = b_tile + local_index; \\\n\
            if (b_row < N && b_col <= K - 4 /*vload4*/) \\\n\
                brow = vload4(0, src1_read00 + mad24(b_row, ldB, b_col)); \\\n\
            else \\\n\
                brow = (float4)0; \\\n\
            vstore4(brow, 0, slm_brow + mad24(_row, SLM_BLOCK, local_index)); \\\n\
        }\n\
\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
        UPDATE_BROW(0);\n\
        UPDATE_BROW(1);\n\
        UPDATE_BROW(2);\n\
        UPDATE_BROW(3);\n\
        UPDATE_BROW(4);\n\
        UPDATE_BROW(5);\n\
        UPDATE_BROW(6);\n\
        UPDATE_BROW(7);\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
#undef UPDATE_BROW\n\
\n\
        for (int k_tile_offset = 0; k_tile_offset < SLM_BLOCK; k_tile_offset += TILE_K)\n\
        {\n\
            int a_col = a_col_base + b_tile + k_tile_offset;\n\
\n\
            if (a_col > K - 4 /*vload4*/)\n\
                break;\n\
\n\
            int slm_brow_col = a_col_base + k_tile_offset;  // <= SLM_BLOCK - 4\n\
#define READ_SLM_BROW(_row) \\\n\
            float4 brow##_row = vload4(0, slm_brow + mad24(_row, SLM_BLOCK, slm_brow_col));\n\
\n\
            READ_SLM_BROW(0);\n\
            READ_SLM_BROW(1);\n\
            READ_SLM_BROW(2);\n\
            READ_SLM_BROW(3);\n\
            READ_SLM_BROW(4);\n\
            READ_SLM_BROW(5);\n\
            READ_SLM_BROW(6);\n\
            READ_SLM_BROW(7);\n\
#undef READ_SLM_BROW\n\
\n\
#define MM_DOT_PRODUCT(_row,_dot) \\\n\
            { \\\n\
                int a_row = a_row_base + _row; \\\n\
                if (a_row < M) { \\\n\
                    float4 arow = vload4(0, src0_read00 + mad24(a_row, ldA, a_col)); \\\n\
                    _dot = mad( (float8)(arow.x), (float8)(brow0.x, brow1.x, brow2.x, brow3.x, brow4.x, brow5.x, brow6.x, brow7.x), _dot ); \\\n\
                    _dot = mad( (float8)(arow.y), (float8)(brow0.y, brow1.y, brow2.y, brow3.y, brow4.y, brow5.y, brow6.y, brow7.y), _dot ); \\\n\
                    _dot = mad( (float8)(arow.z), (float8)(brow0.z, brow1.z, brow2.z, brow3.z, brow4.z, brow5.z, brow6.z, brow7.z), _dot ); \\\n\
                    _dot = mad( (float8)(arow.w), (float8)(brow0.w, brow1.w, brow2.w, brow3.w, brow4.w, brow5.w, brow6.w, brow7.w), _dot ); \\\n\
                } \\\n\
            }\n\
\n\
            MM_DOT_PRODUCT(0,dot00);\n\
            MM_DOT_PRODUCT(1,dot01);\n\
            MM_DOT_PRODUCT(2,dot02);\n\
            MM_DOT_PRODUCT(3,dot03);\n\
            MM_DOT_PRODUCT(4,dot04);\n\
            MM_DOT_PRODUCT(5,dot05);\n\
            MM_DOT_PRODUCT(6,dot06);\n\
            MM_DOT_PRODUCT(7,dot07);\n\
#undef MM_DOT_PRODUCT\n\
        }\n\
    }\n\
\n\
#define REDUCE(_dot) \\\n\
    _dot.s0 = intel_sub_group_shuffle(_dot.s0, 0) + intel_sub_group_shuffle(_dot.s0, 1) + intel_sub_group_shuffle(_dot.s0, 2) + intel_sub_group_shuffle(_dot.s0, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s0, 4) + intel_sub_group_shuffle(_dot.s0, 5) + intel_sub_group_shuffle(_dot.s0, 6) + intel_sub_group_shuffle(_dot.s0, 7); \\\n\
    _dot.s1 = intel_sub_group_shuffle(_dot.s1, 0) + intel_sub_group_shuffle(_dot.s1, 1) + intel_sub_group_shuffle(_dot.s1, 2) + intel_sub_group_shuffle(_dot.s1, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s1, 4) + intel_sub_group_shuffle(_dot.s1, 5) + intel_sub_group_shuffle(_dot.s1, 6) + intel_sub_group_shuffle(_dot.s1, 7); \\\n\
    _dot.s2 = intel_sub_group_shuffle(_dot.s2, 0) + intel_sub_group_shuffle(_dot.s2, 1) + intel_sub_group_shuffle(_dot.s2, 2) + intel_sub_group_shuffle(_dot.s2, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s2, 4) + intel_sub_group_shuffle(_dot.s2, 5) + intel_sub_group_shuffle(_dot.s2, 6) + intel_sub_group_shuffle(_dot.s2, 7); \\\n\
    _dot.s3 = intel_sub_group_shuffle(_dot.s3, 0) + intel_sub_group_shuffle(_dot.s3, 1) + intel_sub_group_shuffle(_dot.s3, 2) + intel_sub_group_shuffle(_dot.s3, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s3, 4) + intel_sub_group_shuffle(_dot.s3, 5) + intel_sub_group_shuffle(_dot.s3, 6) + intel_sub_group_shuffle(_dot.s3, 7); \\\n\
    _dot.s4 = intel_sub_group_shuffle(_dot.s4, 0) + intel_sub_group_shuffle(_dot.s4, 1) + intel_sub_group_shuffle(_dot.s4, 2) + intel_sub_group_shuffle(_dot.s4, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s4, 4) + intel_sub_group_shuffle(_dot.s4, 5) + intel_sub_group_shuffle(_dot.s4, 6) + intel_sub_group_shuffle(_dot.s4, 7); \\\n\
    _dot.s5 = intel_sub_group_shuffle(_dot.s5, 0) + intel_sub_group_shuffle(_dot.s5, 1) + intel_sub_group_shuffle(_dot.s5, 2) + intel_sub_group_shuffle(_dot.s5, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s5, 4) + intel_sub_group_shuffle(_dot.s5, 5) + intel_sub_group_shuffle(_dot.s5, 6) + intel_sub_group_shuffle(_dot.s5, 7); \\\n\
    _dot.s6 = intel_sub_group_shuffle(_dot.s6, 0) + intel_sub_group_shuffle(_dot.s6, 1) + intel_sub_group_shuffle(_dot.s6, 2) + intel_sub_group_shuffle(_dot.s6, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s6, 4) + intel_sub_group_shuffle(_dot.s6, 5) + intel_sub_group_shuffle(_dot.s6, 6) + intel_sub_group_shuffle(_dot.s6, 7); \\\n\
    _dot.s7 = intel_sub_group_shuffle(_dot.s7, 0) + intel_sub_group_shuffle(_dot.s7, 1) + intel_sub_group_shuffle(_dot.s7, 2) + intel_sub_group_shuffle(_dot.s7, 3) +  \\\n\
           intel_sub_group_shuffle(_dot.s7, 4) + intel_sub_group_shuffle(_dot.s7, 5) + intel_sub_group_shuffle(_dot.s7, 6) + intel_sub_group_shuffle(_dot.s7, 7);\n\
\n\
    REDUCE(dot00);\n\
    REDUCE(dot01);\n\
    REDUCE(dot02);\n\
    REDUCE(dot03);\n\
    REDUCE(dot04);\n\
    REDUCE(dot05);\n\
    REDUCE(dot06);\n\
    REDUCE(dot07);\n\
#undef REDUCE\n\
\n\
    float output = 0.0f;\n\
#define OUTPUT(_dot) \\\n\
    output = (local_x == 0) ? _dot.s0 : output; \\\n\
    output = (local_x == 1) ? _dot.s1 : output; \\\n\
    output = (local_x == 2) ? _dot.s2 : output; \\\n\
    output = (local_x == 3) ? _dot.s3 : output; \\\n\
    output = (local_x == 4) ? _dot.s4 : output; \\\n\
    output = (local_x == 5) ? _dot.s5 : output; \\\n\
    output = (local_x == 6) ? _dot.s6 : output; \\\n\
    output = (local_x == 7) ? _dot.s7 : output; \\\n\
    if (beta != 0.0f) \\\n\
        dst_write0[0] = mad(output, (float)alpha, ((float)beta * dst_write0[0])); \\\n\
    else \\\n\
        dst_write0[0] = output * (float)alpha; \\\n\
    dst_write0 += ldC;\n\
\n\
    if (global_x < N && dst_row < M)\n\
    {\n\
        /*if (dst_row + 0 < M)*/ { OUTPUT(dot00); }\n\
        if (dst_row + 1 < M) { OUTPUT(dot01); }\n\
        if (dst_row + 2 < M) { OUTPUT(dot02); }\n\
        if (dst_row + 3 < M) { OUTPUT(dot03); }\n\
        if (dst_row + 4 < M) { OUTPUT(dot04); }\n\
        if (dst_row + 5 < M) { OUTPUT(dot05); }\n\
        if (dst_row + 6 < M) { OUTPUT(dot06); }\n\
        if (dst_row + 7 < M) { OUTPUT(dot07); }\n\
    }\n\
#undef OUTPUT\n\
}\n\
\n\
#undef VEC_SIZE\n\
#undef LWG_HEIGHT\n\
#undef TILE_M\n\
#undef TILE_K\n\
#undef TILE_N\n\
#undef SLM_BLOCK\n\
\n\
#define VEC_SIZE        4\n\
#define LWG_HEIGHT      4\n\
#define TILE_M          8\n\
#define TILE_K          16\n\
#define TILE_N          32\n\
\n\
__attribute__((reqd_work_group_size(8, LWG_HEIGHT, 1)))\n\
__kernel void intelblas_gemm_buffer_TN(\n\
    const __global float *src0, int off0,\n\
    const __global float *src1, int off1,\n\
    __global float *dst, int offd,\n\
    int M,\n\
    int N,\n\
    int K,\n\
    float alpha,\n\
    float beta,\n\
    int ldA,\n\
    int ldB,\n\
    int ldC,\n\
    int start_index,\n\
    int stride)\n\
{\n\
    const int group_x = get_group_id(0);\n\
    const int group_y = get_group_id(1);\n\
    const int local_x = get_local_id(0);\n\
    const int local_y = get_local_id(1);\n\
    const int global_x = get_global_id(0);\n\
    const int global_y = get_global_id(1);\n\
\n\
    float4 brow;\n\
\n\
    __global float *dst_write0 = dst + local_x * VEC_SIZE + ( group_x * TILE_N ) + ( group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M) * ldC + offd;\n\
\n\
    const __global float *src0_read = src0 + (local_x * ( TILE_K / 8 ) + start_index) * ldA + group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M + off0;\n\
\n\
    const __global float *src1_read0 = src1 + local_x * VEC_SIZE + ( group_x * TILE_N ) + start_index * ldB + off1;\n\
\n\
    float4 dot00 = (start_index != 0) ? vload4(0, dst_write0)           : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0          )) : (float4)(0.0));\n\
    float4 dot01 = (start_index != 0) ? vload4(0, dst_write0 + 1 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 1 * ldC)) : (float4)(0.0));\n\
    float4 dot02 = (start_index != 0) ? vload4(0, dst_write0 + 2 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 2 * ldC)) : (float4)(0.0));\n\
    float4 dot03 = (start_index != 0) ? vload4(0, dst_write0 + 3 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 3 * ldC)) : (float4)(0.0));\n\
    float4 dot04 = (start_index != 0) ? vload4(0, dst_write0 + 4 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 4 * ldC)) : (float4)(0.0));\n\
    float4 dot05 = (start_index != 0) ? vload4(0, dst_write0 + 5 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 5 * ldC)) : (float4)(0.0));\n\
    float4 dot06 = (start_index != 0) ? vload4(0, dst_write0 + 6 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 6 * ldC)) : (float4)(0.0));\n\
    float4 dot07 = (start_index != 0) ? vload4(0, dst_write0 + 7 * ldC) : ((beta != 0.0) ? ((float)beta * vload4(0, dst_write0 + 7 * ldC)) : (float4)(0.0));\n\
\n\
    int end_index = min(start_index + stride, K);\n\
    while( start_index + TILE_K <= end_index ) {\n\
        float8 arow0 = (float)alpha * vload8(0, src0_read);\n\
        float8 arow1 = (float)alpha * vload8(0, src0_read + ldA);\n\
\n\
#define MM_DOT_PRODUCT(_arow,index) \\\n\
        brow = vload4(0, src1_read0);  src1_read0 += ldB; \\\n\
        dot00 = mad( (float4)(intel_sub_group_shuffle(_arow.s0, index)), brow, dot00 ); \\\n\
        dot01 = mad( (float4)(intel_sub_group_shuffle(_arow.s1, index)), brow, dot01 ); \\\n\
        dot02 = mad( (float4)(intel_sub_group_shuffle(_arow.s2, index)), brow, dot02 ); \\\n\
        dot03 = mad( (float4)(intel_sub_group_shuffle(_arow.s3, index)), brow, dot03 ); \\\n\
        dot04 = mad( (float4)(intel_sub_group_shuffle(_arow.s4, index)), brow, dot04 ); \\\n\
        dot05 = mad( (float4)(intel_sub_group_shuffle(_arow.s5, index)), brow, dot05 ); \\\n\
        dot06 = mad( (float4)(intel_sub_group_shuffle(_arow.s6, index)), brow, dot06 ); \\\n\
        dot07 = mad( (float4)(intel_sub_group_shuffle(_arow.s7, index)), brow, dot07 );\n\
\n\
        MM_DOT_PRODUCT(arow0,0);\n\
        MM_DOT_PRODUCT(arow1,0);\n\
        MM_DOT_PRODUCT(arow0,1);\n\
        MM_DOT_PRODUCT(arow1,1);\n\
        MM_DOT_PRODUCT(arow0,2);\n\
        MM_DOT_PRODUCT(arow1,2);\n\
        MM_DOT_PRODUCT(arow0,3);\n\
        MM_DOT_PRODUCT(arow1,3);\n\
        MM_DOT_PRODUCT(arow0,4);\n\
        MM_DOT_PRODUCT(arow1,4);\n\
        MM_DOT_PRODUCT(arow0,5);\n\
        MM_DOT_PRODUCT(arow1,5);\n\
        MM_DOT_PRODUCT(arow0,6);\n\
        MM_DOT_PRODUCT(arow1,6);\n\
        MM_DOT_PRODUCT(arow0,7);\n\
        MM_DOT_PRODUCT(arow1,7);\n\
#undef MM_DOT_PRODUCT\n\
\n\
        src0_read += TILE_K * ldA;\n\
        start_index += TILE_K;\n\
    }\n\
\n\
    if(start_index < end_index) {\n\
        float8 arow0 = ((start_index + local_x * 2) < K) ? ((float)alpha * vload8(0, src0_read)) : (float)0.0f;\n\
        float8 arow1 = ((start_index + local_x * 2 + 1) < K) ? ((float)alpha * vload8(0, src0_read + ldA)) : (float)0.0f;\n\
\n\
#define MM_DOT_PRODUCT(_arow,index) \\\n\
        brow = (start_index < K) ? vload4(0, src1_read0) : (float)0.0f;  src1_read0 += ldB; start_index++; \\\n\
        dot00 = mad( (float4)(intel_sub_group_shuffle(_arow.s0, index)), brow, dot00 ); \\\n\
        dot01 = mad( (float4)(intel_sub_group_shuffle(_arow.s1, index)), brow, dot01 ); \\\n\
        dot02 = mad( (float4)(intel_sub_group_shuffle(_arow.s2, index)), brow, dot02 ); \\\n\
        dot03 = mad( (float4)(intel_sub_group_shuffle(_arow.s3, index)), brow, dot03 ); \\\n\
        dot04 = mad( (float4)(intel_sub_group_shuffle(_arow.s4, index)), brow, dot04 ); \\\n\
        dot05 = mad( (float4)(intel_sub_group_shuffle(_arow.s5, index)), brow, dot05 ); \\\n\
        dot06 = mad( (float4)(intel_sub_group_shuffle(_arow.s6, index)), brow, dot06 ); \\\n\
        dot07 = mad( (float4)(intel_sub_group_shuffle(_arow.s7, index)), brow, dot07 );\n\
\n\
        MM_DOT_PRODUCT(arow0,0);\n\
        MM_DOT_PRODUCT(arow1,0);\n\
        MM_DOT_PRODUCT(arow0,1);\n\
        MM_DOT_PRODUCT(arow1,1);\n\
        MM_DOT_PRODUCT(arow0,2);\n\
        MM_DOT_PRODUCT(arow1,2);\n\
        MM_DOT_PRODUCT(arow0,3);\n\
        MM_DOT_PRODUCT(arow1,3);\n\
        MM_DOT_PRODUCT(arow0,4);\n\
        MM_DOT_PRODUCT(arow1,4);\n\
        MM_DOT_PRODUCT(arow0,5);\n\
        MM_DOT_PRODUCT(arow1,5);\n\
        MM_DOT_PRODUCT(arow0,6);\n\
        MM_DOT_PRODUCT(arow1,6);\n\
        MM_DOT_PRODUCT(arow0,7);\n\
        MM_DOT_PRODUCT(arow1,7);\n\
#undef MM_DOT_PRODUCT\n\
    }\n\
\n\
    if(global_x * 4 < N && global_y * 8 < M) {\n\
        if(mad24(global_x, 4, 3) < N) {\n\
            vstore4(dot00, 0, dst_write0); dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { vstore4(dot01, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { vstore4(dot02, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { vstore4(dot03, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { vstore4(dot04, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { vstore4(dot05, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { vstore4(dot06, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { vstore4(dot07, 0, dst_write0); }\n\
        } else if(mad24(global_x, 4, 2) < N) {\n\
            vstore2(dot00.xy, 0, dst_write0);\n\
            dst_write0[2] = dot00.z;\n\
            dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) {\n\
                vstore2(dot01.xy, 0, dst_write0);\n\
                dst_write0[2] = dot01.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 2) < M) {\n\
                vstore2(dot02.xy, 0, dst_write0);\n\
                dst_write0[2] = dot02.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 3) < M) {\n\
                vstore2(dot03.xy, 0, dst_write0);\n\
                dst_write0[2] = dot03.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 4) < M) {\n\
                vstore2(dot04.xy, 0, dst_write0);\n\
                dst_write0[2] = dot04.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 5) < M) {\n\
                vstore2(dot05.xy, 0, dst_write0);\n\
                dst_write0[2] = dot05.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 6) < M) {\n\
                vstore2(dot06.xy, 0, dst_write0);\n\
                dst_write0[2] = dot06.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 7) < M) {\n\
                vstore2(dot07.xy, 0, dst_write0);\n\
                dst_write0[2] = dot07.z;\n\
            }\n\
        } else if(mad24(global_x, 4, 1) < N) {\n\
            vstore2(dot00.xy, 0, dst_write0); dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { vstore2(dot01.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { vstore2(dot02.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { vstore2(dot03.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { vstore2(dot04.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { vstore2(dot05.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { vstore2(dot06.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { vstore2(dot07.xy, 0, dst_write0); }\n\
        } else {\n\
            dst_write0[0] = dot00.x; dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { dst_write0[0] = dot01.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { dst_write0[0] = dot02.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { dst_write0[0] = dot03.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { dst_write0[0] = dot04.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { dst_write0[0] = dot05.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { dst_write0[0] = dot06.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { dst_write0[0] = dot07.x; }\n\
        }\n\
    }\n\
}\n\
\n\
#undef VEC_SIZE\n\
#undef LWG_HEIGHT\n\
#undef TILE_M\n\
#undef TILE_K\n\
#undef TILE_N\n\
\n\
#define VEC_SIZE        4\n\
#define LWG_HEIGHT      4\n\
#define TILE_M          8\n\
#define TILE_K          16\n\
#define TILE_N          32\n\
\n\
__attribute__((reqd_work_group_size(8, LWG_HEIGHT, 1)))\n\
__kernel void intelblas_gemm_buffer_TT(\n\
    const __global float *src0, int off0,\n\
    const __global float *src1, int off1,\n\
    __global float *dst, int offd,\n\
    int M,\n\
    int N,\n\
    int K,\n\
    float alpha,\n\
    float beta,\n\
    int ldA,\n\
    int ldB,\n\
    int ldC,\n\
    int start_index,\n\
    int stride)\n\
{\n\
    const int group_x = get_group_id(0);\n\
    const int group_y = get_group_id(1);\n\
    const int local_x = get_local_id(0);\n\
    const int local_y = get_local_id(1);\n\
    const int global_x = get_global_id(0);\n\
    const int global_y = get_global_id(1);\n\
\n\
    float8 dot0 = 0.f;\n\
    float8 dot1 = 0.f;\n\
    float8 dot2 = 0.f;\n\
    float8 dot3 = 0.f;\n\
\n\
    float16 brow0;\n\
    float16 brow1;\n\
    float16 brow2;\n\
    float16 brow3;\n\
\n\
    __global float *dst_write0 = dst + local_x * VEC_SIZE + ( group_x * TILE_N ) + ( group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M) * ldC + offd;\n\
\n\
    const __global float *src0_read = src0 + (local_x * ( TILE_K / 8 ) + start_index) * ldA + group_y * LWG_HEIGHT * TILE_M + local_y * TILE_M + off0;\n\
\n\
    const __global float *src1_read0 = src1 + (local_x * VEC_SIZE + ( group_x * TILE_N )) * ldB + start_index + off1;\n\
\n\
    float4 dot00 = (start_index != 0) ? vload4(0, dst_write0)           : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0          )) : (float4)(0.0));\n\
    float4 dot01 = (start_index != 0) ? vload4(0, dst_write0 + ldC)     : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0 + ldC    )) : (float4)(0.0));\n\
    float4 dot02 = (start_index != 0) ? vload4(0, dst_write0 + 2 * ldC) : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0 + 2 * ldC)) : (float4)(0.0));\n\
    float4 dot03 = (start_index != 0) ? vload4(0, dst_write0 + 3 * ldC) : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0 + 3 * ldC)) : (float4)(0.0));\n\
    float4 dot04 = (start_index != 0) ? vload4(0, dst_write0 + 4 * ldC) : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0 + 4 * ldC)) : (float4)(0.0));\n\
    float4 dot05 = (start_index != 0) ? vload4(0, dst_write0 + 5 * ldC) : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0 + 5 * ldC)) : (float4)(0.0));\n\
    float4 dot06 = (start_index != 0) ? vload4(0, dst_write0 + 6 * ldC) : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0 + 6 * ldC)) : (float4)(0.0));\n\
    float4 dot07 = (start_index != 0) ? vload4(0, dst_write0 + 7 * ldC) : ((beta != 0.0)? ((float)beta * vload4(0, dst_write0 + 7 * ldC)) : (float4)(0.0));\n\
\n\
    int end_index = min(start_index + stride, K);\n\
    while( start_index + TILE_K <= end_index ) {\n\
        brow0 = vload16(0, src1_read0);\n\
        brow1 = vload16(0, src1_read0 + ldB);\n\
        brow2 = vload16(0, src1_read0 + 2 * ldB);\n\
        brow3 = vload16(0, src1_read0 + 3 * ldB);\n\
\n\
        float8 arow0 = (float)alpha * vload8(0, src0_read);\n\
        float8 arow1 = (float)alpha * vload8(0, src0_read + ldA);\n\
\n\
#define DOT_PRODUCT( _dot, _arow, index, _brow) \\\n\
        _dot.s0 = mad( intel_sub_group_shuffle( _arow.s0, index ), _brow, _dot.s0 ); \\\n\
        _dot.s1 = mad( intel_sub_group_shuffle( _arow.s1, index ), _brow, _dot.s1 ); \\\n\
        _dot.s2 = mad( intel_sub_group_shuffle( _arow.s2, index ), _brow, _dot.s2 ); \\\n\
        _dot.s3 = mad( intel_sub_group_shuffle( _arow.s3, index ), _brow, _dot.s3 ); \\\n\
        _dot.s4 = mad( intel_sub_group_shuffle( _arow.s4, index ), _brow, _dot.s4 ); \\\n\
        _dot.s5 = mad( intel_sub_group_shuffle( _arow.s5, index ), _brow, _dot.s5 ); \\\n\
        _dot.s6 = mad( intel_sub_group_shuffle( _arow.s6, index ), _brow, _dot.s6 ); \\\n\
        _dot.s7 = mad( intel_sub_group_shuffle( _arow.s7, index ), _brow, _dot.s7 );\n\
\n\
#define MM_DOT_PRODUCT( _brow, _dot) \\\n\
        DOT_PRODUCT(_dot, arow0, 0, _brow.s0); \\\n\
        DOT_PRODUCT(_dot, arow1, 0, _brow.s1); \\\n\
        DOT_PRODUCT(_dot, arow0, 1, _brow.s2); \\\n\
        DOT_PRODUCT(_dot, arow1, 1, _brow.s3); \\\n\
        DOT_PRODUCT(_dot, arow0, 2, _brow.s4); \\\n\
        DOT_PRODUCT(_dot, arow1, 2, _brow.s5); \\\n\
        DOT_PRODUCT(_dot, arow0, 3, _brow.s6); \\\n\
        DOT_PRODUCT(_dot, arow1, 3, _brow.s7); \\\n\
        DOT_PRODUCT(_dot, arow0, 4, _brow.s8); \\\n\
        DOT_PRODUCT(_dot, arow1, 4, _brow.s9); \\\n\
        DOT_PRODUCT(_dot, arow0, 5, _brow.sa); \\\n\
        DOT_PRODUCT(_dot, arow1, 5, _brow.sb); \\\n\
        DOT_PRODUCT(_dot, arow0, 6, _brow.sc); \\\n\
        DOT_PRODUCT(_dot, arow1, 6, _brow.sd); \\\n\
        DOT_PRODUCT(_dot, arow0, 7, _brow.se); \\\n\
        DOT_PRODUCT(_dot, arow1, 7, _brow.sf);\n\
\n\
        MM_DOT_PRODUCT( brow0, dot0 );\n\
        MM_DOT_PRODUCT( brow1, dot1 );\n\
        MM_DOT_PRODUCT( brow2, dot2 );\n\
        MM_DOT_PRODUCT( brow3, dot3 );\n\
#undef MM_DOT_PRODUCT\n\
#undef DOT_PRODUCT\n\
\n\
        src1_read0 += TILE_K;\n\
        src0_read += TILE_K * ldA;\n\
        start_index += TILE_K;\n\
    }\n\
\n\
    if(start_index < end_index) {\n\
        brow0 = vload16(0, src1_read0);  src1_read0 += ldB;\n\
        brow1 = vload16(0, src1_read0);  src1_read0 += ldB;\n\
        brow2 = vload16(0, src1_read0);  src1_read0 += ldB;\n\
        brow3 = vload16(0, src1_read0);\n\
\n\
        float8 arow0 = (float)alpha * vload8(0, src0_read);\n\
        float8 arow1 = (float)alpha * vload8(0, src0_read + ldA);\n\
\n\
#define DOT_PRODUCT( _dot, _arow, index, _brow) \\\n\
        _dot.s0 = (w < K) ? mad( intel_sub_group_shuffle( _arow.s0, index ), _brow, _dot.s0 ) : _dot.s0; \\\n\
        _dot.s1 = (w < K) ? mad( intel_sub_group_shuffle( _arow.s1, index ), _brow, _dot.s1 ) : _dot.s1; \\\n\
        _dot.s2 = (w < K) ? mad( intel_sub_group_shuffle( _arow.s2, index ), _brow, _dot.s2 ) : _dot.s2; \\\n\
        _dot.s3 = (w < K) ? mad( intel_sub_group_shuffle( _arow.s3, index ), _brow, _dot.s3 ) : _dot.s3; \\\n\
        _dot.s4 = (w < K) ? mad( intel_sub_group_shuffle( _arow.s4, index ), _brow, _dot.s4 ) : _dot.s4; \\\n\
        _dot.s5 = (w < K) ? mad( intel_sub_group_shuffle( _arow.s5, index ), _brow, _dot.s5 ) : _dot.s5; \\\n\
        _dot.s6 = (w < K) ? mad( intel_sub_group_shuffle( _arow.s6, index ), _brow, _dot.s6 ) : _dot.s6; \\\n\
        _dot.s7 = (w++ < K) ? mad( intel_sub_group_shuffle( _arow.s7, index ), _brow, _dot.s7 ) : _dot.s7;\n\
\n\
#define MM_DOT_PRODUCT( _brow, _dot) \\\n\
        DOT_PRODUCT(_dot, arow0, 0, _brow.s0); \\\n\
        DOT_PRODUCT(_dot, arow1, 0, _brow.s1); \\\n\
        DOT_PRODUCT(_dot, arow0, 1, _brow.s2); \\\n\
        DOT_PRODUCT(_dot, arow1, 1, _brow.s3); \\\n\
        DOT_PRODUCT(_dot, arow0, 2, _brow.s4); \\\n\
        DOT_PRODUCT(_dot, arow1, 2, _brow.s5); \\\n\
        DOT_PRODUCT(_dot, arow0, 3, _brow.s6); \\\n\
        DOT_PRODUCT(_dot, arow1, 3, _brow.s7); \\\n\
        DOT_PRODUCT(_dot, arow0, 4, _brow.s8); \\\n\
        DOT_PRODUCT(_dot, arow1, 4, _brow.s9); \\\n\
        DOT_PRODUCT(_dot, arow0, 5, _brow.sa); \\\n\
        DOT_PRODUCT(_dot, arow1, 5, _brow.sb); \\\n\
        DOT_PRODUCT(_dot, arow0, 6, _brow.sc); \\\n\
        DOT_PRODUCT(_dot, arow1, 6, _brow.sd); \\\n\
        DOT_PRODUCT(_dot, arow0, 7, _brow.se); \\\n\
        DOT_PRODUCT(_dot, arow1, 7, _brow.sf);\n\
\n\
        int w = start_index;\n\
        MM_DOT_PRODUCT( brow0, dot0 );\n\
        w = start_index;\n\
        MM_DOT_PRODUCT( brow1, dot1 );\n\
        w = start_index;\n\
        MM_DOT_PRODUCT( brow2, dot2 );\n\
        w = start_index;\n\
        MM_DOT_PRODUCT( brow3, dot3 );\n\
#undef MM_DOT_PRODUCT\n\
#undef DOT_PRODUCT\n\
    }\n\
\n\
    dot00 += (float4)(dot0.s0, dot1.s0, dot2.s0, dot3.s0);\n\
    dot01 += (float4)(dot0.s1, dot1.s1, dot2.s1, dot3.s1);\n\
    dot02 += (float4)(dot0.s2, dot1.s2, dot2.s2, dot3.s2);\n\
    dot03 += (float4)(dot0.s3, dot1.s3, dot2.s3, dot3.s3);\n\
    dot04 += (float4)(dot0.s4, dot1.s4, dot2.s4, dot3.s4);\n\
    dot05 += (float4)(dot0.s5, dot1.s5, dot2.s5, dot3.s5);\n\
    dot06 += (float4)(dot0.s6, dot1.s6, dot2.s6, dot3.s6);\n\
    dot07 += (float4)(dot0.s7, dot1.s7, dot2.s7, dot3.s7);\n\
\n\
    if(global_x * 4 < N && global_y * 8 < M) {\n\
        if(mad24(global_x, 4, 3) < N) {\n\
            vstore4(dot00, 0, dst_write0); dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { vstore4(dot01, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { vstore4(dot02, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { vstore4(dot03, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { vstore4(dot04, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { vstore4(dot05, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { vstore4(dot06, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { vstore4(dot07, 0, dst_write0); }\n\
        } else if(mad24(global_x, 4, 2) < N) {\n\
            vstore2(dot00.xy, 0, dst_write0);\n\
            dst_write0[2] = dot00.z;\n\
            dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) {\n\
                vstore2(dot01.xy, 0, dst_write0);\n\
                dst_write0[2] = dot01.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 2) < M) {\n\
                vstore2(dot02.xy, 0, dst_write0);\n\
                dst_write0[2] = dot02.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 3) < M) {\n\
                vstore2(dot03.xy, 0, dst_write0);\n\
                dst_write0[2] = dot03.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 4) < M) {\n\
                vstore2(dot04.xy, 0, dst_write0);\n\
                dst_write0[2] = dot04.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 5) < M) {\n\
                vstore2(dot05.xy, 0, dst_write0);\n\
                dst_write0[2] = dot05.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 6) < M) {\n\
                vstore2(dot06.xy, 0, dst_write0);\n\
                dst_write0[2] = dot06.z;\n\
                dst_write0 += ldC;\n\
            } else\n\
                return;\n\
            if(mad24(global_y, 8, 7) < M) {\n\
                vstore2(dot07.xy, 0, dst_write0);\n\
                dst_write0[2] = dot07.z;\n\
            }\n\
        } else if(mad24(global_x, 4, 1) < N) {\n\
            vstore2(dot00.xy, 0, dst_write0); dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { vstore2(dot01.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { vstore2(dot02.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { vstore2(dot03.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { vstore2(dot04.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { vstore2(dot05.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { vstore2(dot06.xy, 0, dst_write0); dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { vstore2(dot07.xy, 0, dst_write0); }\n\
        } else {\n\
            dst_write0[0] = dot00.x; dst_write0 += ldC;\n\
            if(mad24(global_y, 8, 1) < M) { dst_write0[0] = dot01.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 2) < M) { dst_write0[0] = dot02.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 3) < M) { dst_write0[0] = dot03.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 4) < M) { dst_write0[0] = dot04.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 5) < M) { dst_write0[0] = dot05.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 6) < M) { dst_write0[0] = dot06.x; dst_write0 += ldC; }\n\
            else return;\n\
            if(mad24(global_y, 8, 7) < M) { dst_write0[0] = dot07.x; }\n\
        }\n\
    }\n\
}\n\
\n\
#undef VEC_SIZE\n\
#undef LWG_HEIGHT\n\
#undef TILE_M\n\
#undef TILE_K\n\
#undef TILE_N\n\
#endif");

ProgramSource lut_oclsrc = ProgramSource("\
#if lcn == 1\n\
    #if dcn == 4\n\
        #define LUT_OP  \\\n\
            int idx = *(__global const int *)(srcptr + src_index); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx & 0xff]; \\\n\
            dst[1] = lut_l[(idx >> 8) & 0xff]; \\\n\
            dst[2] = lut_l[(idx >> 16) & 0xff]; \\\n\
            dst[3] = lut_l[(idx >> 24) & 0xff];\n\
    #elif dcn == 3\n\
        #define LUT_OP  \\\n\
            uchar3 idx = vload3(0, srcptr + src_index); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx.x]; \\\n\
            dst[1] = lut_l[idx.y]; \\\n\
            dst[2] = lut_l[idx.z];\n\
    #elif dcn == 2\n\
        #define LUT_OP \\\n\
            short idx = *(__global const short *)(srcptr + src_index); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx & 0xff]; \\\n\
            dst[1] = lut_l[(idx >> 8) & 0xff];\n\
    #elif dcn == 1\n\
        #define LUT_OP \\\n\
            uchar idx = (srcptr + src_index)[0]; \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx];\n\
    #else\n\
        #define LUT_OP \\\n\
            __global const srcT * src = (__global const srcT *)(srcptr + src_index); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            for (int cn = 0; cn < dcn; ++cn) \\\n\
                dst[cn] = lut_l[src[cn]];\n\
    #endif\n\
#else\n\
    #if dcn == 4\n\
        #define LUT_OP \\\n\
            __global const uchar4 * src_pixel = (__global const uchar4 *)(srcptr + src_index); \\\n\
            int4 idx = mad24(convert_int4(src_pixel[0]), (int4)(lcn), (int4)(0, 1, 2, 3)); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx.x]; \\\n\
            dst[1] = lut_l[idx.y]; \\\n\
            dst[2] = lut_l[idx.z]; \\\n\
            dst[3] = lut_l[idx.w];\n\
    #elif dcn == 3\n\
        #define LUT_OP \\\n\
            uchar3 src_pixel = vload3(0, srcptr + src_index); \\\n\
            int3 idx = mad24(convert_int3(src_pixel), (int3)(lcn), (int3)(0, 1, 2)); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx.x]; \\\n\
            dst[1] = lut_l[idx.y]; \\\n\
            dst[2] = lut_l[idx.z];\n\
    #elif dcn == 2\n\
        #define LUT_OP \\\n\
            __global const uchar2 * src_pixel = (__global const uchar2 *)(srcptr + src_index); \\\n\
            int2 idx = mad24(convert_int2(src_pixel[0]), lcn, (int2)(0, 1)); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx.x]; \\\n\
            dst[1] = lut_l[idx.y];\n\
    #elif dcn == 1 //error case (1 < lcn) ==> lcn == scn == dcn\n\
        #define LUT_OP \\\n\
            uchar idx = (srcptr + src_index)[0]; \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            dst[0] = lut_l[idx];\n\
    #else\n\
        #define LUT_OP \\\n\
            __global const srcT * src = (__global const srcT *)(srcptr + src_index); \\\n\
            dst = (__global dstT *)(dstptr + dst_index); \\\n\
            for (int cn = 0; cn < dcn; ++cn) \\\n\
                dst[cn] = lut_l[mad24(src[cn], lcn, cn)];\n\
    #endif\n\
#endif\n\
\n\
__kernel void LUT(__global const uchar * srcptr, int src_step, int src_offset,\n\
                  __global const uchar * lutptr, int lut_step, int lut_offset,\n\
                  __global uchar * dstptr, int dst_step, int dst_offset, int rows, int cols)\n\
{\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1) << 2;\n\
\n\
    __local dstT lut_l[256 * lcn];\n\
    __global const dstT * lut = (__global const dstT *)(lutptr + lut_offset);\n\
\n\
    for (int i = mad24((int)get_local_id(1), (int)get_local_size(0), (int)get_local_id(0)),\n\
             step = get_local_size(0) * get_local_size(1); i < 256 * lcn; i += step)\n\
        lut_l[i] = lut[i];\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x < cols && y < rows)\n\
    {\n\
        int src_index = mad24(y, src_step, mad24(x, (int)sizeof(srcT) * dcn, src_offset));\n\
        int dst_index = mad24(y, dst_step, mad24(x, (int)sizeof(dstT) * dcn, dst_offset));\n\
\n\
        __global dstT * dst;\n\
\n\
        LUT_OP;\n\
\n\
        if (y < rows - 1)\n\
        {\n\
            src_index += src_step;\n\
            dst_index += dst_step;\n\
            LUT_OP;\n\
\n\
            if (y < rows - 2)\n\
            {\n\
                src_index += src_step;\n\
                dst_index += dst_step;\n\
                LUT_OP;\n\
\n\
                if (y < rows - 3)\n\
                {\n\
                    src_index += src_step;\n\
                    dst_index += dst_step;\n\
                    LUT_OP;\n\
                }\n\
            }\n\
        }\n\
    }\n\
}");

ProgramSource meanstddev_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#define noconvert\n\
\n\
#if cn != 3\n\
#define loadpix(addr) *(__global const srcT *)(addr)\n\
#define storepix(val, addr)  *(__global dstT *)(addr) = val\n\
#define storesqpix(val, addr)  *(__global sqdstT *)(addr) = val\n\
#define srcTSIZE (int)sizeof(srcT)\n\
#define dstTSIZE (int)sizeof(dstT)\n\
#define sqdstTSIZE (int)sizeof(sqdstT)\n\
#else\n\
#define loadpix(addr) vload3(0, (__global const srcT1 *)(addr))\n\
#define storepix(val, addr) vstore3(val, 0, (__global dstT1 *)(addr))\n\
#define storesqpix(val, addr) vstore3(val, 0, (__global sqdstT1 *)(addr))\n\
#define srcTSIZE ((int)sizeof(srcT1)*3)\n\
#define dstTSIZE ((int)sizeof(dstT1)*3)\n\
#define sqdstTSIZE ((int)sizeof(sqdstT1)*3)\n\
#endif\n\
\n\
__kernel void meanStdDev(__global const uchar * srcptr, int src_step, int src_offset, int cols,\n\
                         int total, int groups, __global uchar * dstptr\n\
#ifdef HAVE_MASK\n\
                         , __global const uchar * mask, int mask_step, int mask_offset\n\
#endif\n\
                        )\n\
{\n\
    int lid = get_local_id(0);\n\
    int gid = get_group_id(0);\n\
    int id = get_global_id(0);\n\
\n\
    __local dstT localMemSum[WGS2_ALIGNED];\n\
    __local sqdstT localMemSqSum[WGS2_ALIGNED];\n\
#ifdef HAVE_MASK\n\
    __local int localMemNonZero[WGS2_ALIGNED];\n\
#endif\n\
\n\
    dstT accSum = (dstT)(0);\n\
    sqdstT accSqSum = (sqdstT)(0);\n\
#ifdef HAVE_MASK\n\
    int accNonZero = 0;\n\
    mask += mask_offset;\n\
#endif\n\
    srcptr += src_offset;\n\
\n\
    for (int grain = groups * WGS; id < total; id += grain)\n\
    {\n\
#ifdef HAVE_MASK\n\
#ifdef HAVE_MASK_CONT\n\
        int mask_index = id;\n\
#else\n\
        int mask_index = mad24(id / cols, mask_step, id % cols);\n\
#endif\n\
        if (mask[mask_index])\n\
#endif\n\
        {\n\
#ifdef HAVE_SRC_CONT\n\
            int src_index = id * srcTSIZE;\n\
#else\n\
            int src_index = mad24(id / cols, src_step, mul24(id % cols, srcTSIZE));\n\
#endif\n\
\n\
            srcT value = loadpix(srcptr + src_index);\n\
            accSum += convertToDT(value);\n\
            sqdstT dvalue = convertToSDT(value);\n\
            accSqSum = fma(dvalue, dvalue, accSqSum);\n\
\n\
#ifdef HAVE_MASK\n\
            ++accNonZero;\n\
#endif\n\
        }\n\
    }\n\
\n\
    if (lid < WGS2_ALIGNED)\n\
    {\n\
        localMemSum[lid] = accSum;\n\
        localMemSqSum[lid] = accSqSum;\n\
#ifdef HAVE_MASK\n\
        localMemNonZero[lid] = accNonZero;\n\
#endif\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (lid >= WGS2_ALIGNED && total >= WGS2_ALIGNED)\n\
    {\n\
        localMemSum[lid - WGS2_ALIGNED] += accSum;\n\
        localMemSqSum[lid - WGS2_ALIGNED] += accSqSum;\n\
#ifdef HAVE_MASK\n\
        localMemNonZero[lid - WGS2_ALIGNED] += accNonZero;\n\
#endif\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    for (int lsize = WGS2_ALIGNED >> 1; lsize > 0; lsize >>= 1)\n\
    {\n\
        if (lid < lsize)\n\
        {\n\
            int lid2 = lsize + lid;\n\
            localMemSum[lid] += localMemSum[lid2];\n\
            localMemSqSum[lid] += localMemSqSum[lid2];\n\
#ifdef HAVE_MASK\n\
            localMemNonZero[lid] += localMemNonZero[lid2];\n\
#endif\n\
        }\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
    }\n\
\n\
    if (lid == 0)\n\
    {\n\
        storepix(localMemSum[0], dstptr + dstTSIZE * gid);\n\
        storesqpix(localMemSqSum[0], dstptr + mad24(dstTSIZE, groups, sqdstTSIZE * gid));\n\
#ifdef HAVE_MASK\n\
        *(__global int *)(dstptr + mad24(dstTSIZE + sqdstTSIZE, groups, (int)sizeof(int) * gid)) = localMemNonZero[0];\n\
#endif\n\
    }\n\
}");

ProgramSource minmaxloc_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
static inline int align(int pos)\n\
{\n\
    return (pos + (MINMAX_STRUCT_ALIGNMENT - 1)) & (~(MINMAX_STRUCT_ALIGNMENT - 1));\n\
}\n\
\n\
#ifdef DEPTH_0\n\
#define MIN_VAL 0\n\
#define MAX_VAL UCHAR_MAX\n\
#elif defined DEPTH_1\n\
#define MIN_VAL SCHAR_MIN\n\
#define MAX_VAL SCHAR_MAX\n\
#elif defined DEPTH_2\n\
#define MIN_VAL 0\n\
#define MAX_VAL USHRT_MAX\n\
#elif defined DEPTH_3\n\
#define MIN_VAL SHRT_MIN\n\
#define MAX_VAL SHRT_MAX\n\
#elif defined DEPTH_4\n\
#define MIN_VAL INT_MIN\n\
#define MAX_VAL INT_MAX\n\
#elif defined DEPTH_5\n\
#define MIN_VAL (-FLT_MAX)\n\
#define MAX_VAL FLT_MAX\n\
#elif defined DEPTH_6\n\
#define MIN_VAL (-DBL_MAX)\n\
#define MAX_VAL DBL_MAX\n\
#endif\n\
\n\
#define noconvert\n\
#define INDEX_MAX UINT_MAX\n\
\n\
#if wdepth <= 4\n\
#define MIN_ABS(a) convertFromU(abs(a))\n\
#define MIN_ABS2(a, b) convertFromU(abs_diff(a, b))\n\
#define MIN(a, b) min(a, b)\n\
#define MAX(a, b) max(a, b)\n\
#else\n\
#define MIN_ABS(a) fabs(a)\n\
#define MIN_ABS2(a, b) fabs(a - b)\n\
#define MIN(a, b) fmin(a, b)\n\
#define MAX(a, b) fmax(a, b)\n\
#endif\n\
\n\
#if kercn != 3\n\
#define loadpix(addr) *(__global const srcT *)(addr)\n\
#define srcTSIZE (int)sizeof(srcT)\n\
#else\n\
#define loadpix(addr) vload3(0, (__global const srcT1 *)(addr))\n\
#define srcTSIZE ((int)sizeof(srcT1) * 3)\n\
#endif\n\
\n\
#ifndef HAVE_MASK\n\
#undef srcTSIZE\n\
#define srcTSIZE (int)sizeof(srcT1)\n\
#endif\n\
\n\
#ifdef NEED_MINVAL\n\
#ifdef NEED_MINLOC\n\
#define CALC_MIN(p, inc) \\\n\
    if (minval > temp.p) \\\n\
    { \\\n\
        minval = temp.p; \\\n\
        minloc = id + inc; \\\n\
    }\n\
#else\n\
#define CALC_MIN(p, inc) \\\n\
    minval = MIN(minval, temp.p);\n\
#endif\n\
#else\n\
#define CALC_MIN(p, inc)\n\
#endif\n\
\n\
#ifdef NEED_MAXVAL\n\
#ifdef NEED_MAXLOC\n\
#define CALC_MAX(p, inc) \\\n\
    if (maxval < temp.p) \\\n\
    { \\\n\
        maxval = temp.p; \\\n\
        maxloc = id + inc; \\\n\
    }\n\
#else\n\
#define CALC_MAX(p, inc) \\\n\
    maxval = MAX(maxval, temp.p);\n\
#endif\n\
#else\n\
#define CALC_MAX(p, inc)\n\
#endif\n\
\n\
#ifdef OP_CALC2\n\
#define CALC_MAX2(p) \\\n\
    maxval2 = MAX(maxval2, temp2.p);\n\
#else\n\
#define CALC_MAX2(p)\n\
#endif\n\
\n\
#define CALC_P(p, inc) \\\n\
    CALC_MIN(p, inc) \\\n\
    CALC_MAX(p, inc) \\\n\
    CALC_MAX2(p)\n\
\n\
__kernel void minmaxloc(__global const uchar * srcptr, int src_step, int src_offset, int cols,\n\
                        int total, int groupnum, __global uchar * dstptr\n\
#ifdef HAVE_MASK\n\
                        , __global const uchar * mask, int mask_step, int mask_offset\n\
#endif\n\
#ifdef HAVE_SRC2\n\
                        , __global const uchar * src2ptr, int src2_step, int src2_offset\n\
#endif\n\
                        )\n\
{\n\
    int lid = get_local_id(0);\n\
    int gid = get_group_id(0);\n\
    int  id = get_global_id(0)\n\
#ifndef HAVE_MASK\n\
    * kercn;\n\
#else\n\
    ;\n\
#endif\n\
\n\
    srcptr += src_offset;\n\
#ifdef HAVE_MASK\n\
    mask += mask_offset;\n\
#endif\n\
#ifdef HAVE_SRC2\n\
    src2ptr += src2_offset;\n\
#endif\n\
\n\
#ifdef NEED_MINVAL\n\
    __local dstT1 localmem_min[WGS2_ALIGNED];\n\
    dstT1 minval = MAX_VAL;\n\
#ifdef NEED_MINLOC\n\
    __local uint localmem_minloc[WGS2_ALIGNED];\n\
    uint minloc = INDEX_MAX;\n\
#endif\n\
#endif\n\
#ifdef NEED_MAXVAL\n\
    dstT1 maxval = MIN_VAL;\n\
    __local dstT1 localmem_max[WGS2_ALIGNED];\n\
#ifdef NEED_MAXLOC\n\
    __local uint localmem_maxloc[WGS2_ALIGNED];\n\
    uint maxloc = INDEX_MAX;\n\
#endif\n\
#endif\n\
#ifdef OP_CALC2\n\
    __local dstT1 localmem_max2[WGS2_ALIGNED];\n\
    dstT1 maxval2 = MIN_VAL;\n\
#endif\n\
\n\
    int src_index;\n\
#ifdef HAVE_MASK\n\
    int mask_index;\n\
#endif\n\
#ifdef HAVE_SRC2\n\
    int src2_index;\n\
#endif\n\
\n\
    dstT temp;\n\
#ifdef HAVE_SRC2\n\
    dstT temp2;\n\
#endif\n\
\n\
    for (int grain = groupnum * WGS\n\
#ifndef HAVE_MASK\n\
        * kercn\n\
#endif\n\
        ; id < total; id += grain)\n\
    {\n\
#ifdef HAVE_MASK\n\
#ifdef HAVE_MASK_CONT\n\
        mask_index = id;\n\
#else\n\
        mask_index = mad24(id / cols, mask_step, id % cols);\n\
#endif\n\
        if (mask[mask_index])\n\
#endif\n\
        {\n\
#ifdef HAVE_SRC_CONT\n\
            src_index = id * srcTSIZE;//mul24(id, srcTSIZE);\n\
#else\n\
            src_index = mad24(id / cols, src_step, mul24(id % cols, srcTSIZE));\n\
#endif\n\
            temp = convertToDT(loadpix(srcptr + src_index));\n\
#ifdef OP_ABS\n\
            temp = MIN_ABS(temp);\n\
#endif\n\
\n\
#ifdef HAVE_SRC2\n\
#ifdef HAVE_SRC2_CONT\n\
            src2_index = id * srcTSIZE; //mul24(id, srcTSIZE);\n\
#else\n\
            src2_index = mad24(id / cols, src2_step, mul24(id % cols, srcTSIZE));\n\
#endif\n\
            temp2 = convertToDT(loadpix(src2ptr + src2_index));\n\
            temp = MIN_ABS2(temp, temp2);\n\
#ifdef OP_CALC2\n\
            temp2 = MIN_ABS(temp2);\n\
#endif\n\
#endif\n\
\n\
#if kercn == 1\n\
#ifdef NEED_MINVAL\n\
#ifdef NEED_MINLOC\n\
            if (minval > temp)\n\
            {\n\
                minval = temp;\n\
                minloc = id;\n\
            }\n\
#else\n\
            minval = MIN(minval, temp);\n\
#endif\n\
#endif\n\
#ifdef NEED_MAXVAL\n\
#ifdef NEED_MAXLOC\n\
            if (maxval < temp)\n\
            {\n\
                maxval = temp;\n\
                maxloc = id;\n\
            }\n\
#else\n\
            maxval = MAX(maxval, temp);\n\
#endif\n\
#ifdef OP_CALC2\n\
            maxval2 = MAX(maxval2, temp2);\n\
#endif\n\
#endif\n\
#elif kercn >= 2\n\
            CALC_P(s0, 0)\n\
            CALC_P(s1, 1)\n\
#if kercn >= 3\n\
            CALC_P(s2, 2)\n\
#if kercn >= 4\n\
            CALC_P(s3, 3)\n\
#if kercn >= 8\n\
            CALC_P(s4, 4)\n\
            CALC_P(s5, 5)\n\
            CALC_P(s6, 6)\n\
            CALC_P(s7, 7)\n\
#if kercn == 16\n\
            CALC_P(s8, 8)\n\
            CALC_P(s9, 9)\n\
            CALC_P(sA, 10)\n\
            CALC_P(sB, 11)\n\
            CALC_P(sC, 12)\n\
            CALC_P(sD, 13)\n\
            CALC_P(sE, 14)\n\
            CALC_P(sF, 15)\n\
#endif\n\
#endif\n\
#endif\n\
#endif\n\
#endif\n\
        }\n\
    }\n\
\n\
    if (lid < WGS2_ALIGNED)\n\
    {\n\
#ifdef NEED_MINVAL\n\
        localmem_min[lid] = minval;\n\
#endif\n\
#ifdef NEED_MAXVAL\n\
        localmem_max[lid] = maxval;\n\
#endif\n\
#ifdef NEED_MINLOC\n\
        localmem_minloc[lid] = minloc;\n\
#endif\n\
#ifdef NEED_MAXLOC\n\
        localmem_maxloc[lid] = maxloc;\n\
#endif\n\
#ifdef OP_CALC2\n\
        localmem_max2[lid] = maxval2;\n\
#endif\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (lid >= WGS2_ALIGNED && total >= WGS2_ALIGNED)\n\
    {\n\
        int lid3 = lid - WGS2_ALIGNED;\n\
#ifdef NEED_MINVAL\n\
#ifdef NEED_MINLOC\n\
        if (localmem_min[lid3] >= minval)\n\
        {\n\
            if (localmem_min[lid3] == minval)\n\
                localmem_minloc[lid3] = min(localmem_minloc[lid3], minloc);\n\
            else\n\
                localmem_minloc[lid3] = minloc,\n\
            localmem_min[lid3] = minval;\n\
        }\n\
#else\n\
        localmem_min[lid3] = MIN(localmem_min[lid3], minval);\n\
#endif\n\
#endif\n\
#ifdef NEED_MAXVAL\n\
#ifdef NEED_MAXLOC\n\
        if (localmem_max[lid3] <= maxval)\n\
        {\n\
            if (localmem_max[lid3] == maxval)\n\
                localmem_maxloc[lid3] = min(localmem_maxloc[lid3], maxloc);\n\
            else\n\
                localmem_maxloc[lid3] = maxloc,\n\
            localmem_max[lid3] = maxval;\n\
        }\n\
#else\n\
        localmem_max[lid3] = MAX(localmem_max[lid3], maxval);\n\
#endif\n\
#endif\n\
#ifdef OP_CALC2\n\
        localmem_max2[lid3] = MAX(localmem_max2[lid3], maxval2);\n\
#endif\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    for (int lsize = WGS2_ALIGNED >> 1; lsize > 0; lsize >>= 1)\n\
    {\n\
        if (lid < lsize)\n\
        {\n\
            int lid2 = lsize + lid;\n\
\n\
#ifdef NEED_MINVAL\n\
#ifdef NEED_MINLOC\n\
            if (localmem_min[lid] >= localmem_min[lid2])\n\
            {\n\
                if (localmem_min[lid] == localmem_min[lid2])\n\
                    localmem_minloc[lid] = min(localmem_minloc[lid2], localmem_minloc[lid]);\n\
                else\n\
                    localmem_minloc[lid] = localmem_minloc[lid2],\n\
                localmem_min[lid] = localmem_min[lid2];\n\
            }\n\
#else\n\
            localmem_min[lid] = MIN(localmem_min[lid], localmem_min[lid2]);\n\
#endif\n\
#endif\n\
#ifdef NEED_MAXVAL\n\
#ifdef NEED_MAXLOC\n\
            if (localmem_max[lid] <= localmem_max[lid2])\n\
            {\n\
                if (localmem_max[lid] == localmem_max[lid2])\n\
                    localmem_maxloc[lid] = min(localmem_maxloc[lid2], localmem_maxloc[lid]);\n\
                else\n\
                    localmem_maxloc[lid] = localmem_maxloc[lid2],\n\
                localmem_max[lid] = localmem_max[lid2];\n\
            }\n\
#else\n\
            localmem_max[lid] = MAX(localmem_max[lid], localmem_max[lid2]);\n\
#endif\n\
#endif\n\
#ifdef OP_CALC2\n\
            localmem_max2[lid] = MAX(localmem_max2[lid], localmem_max2[lid2]);\n\
#endif\n\
        }\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
    }\n\
\n\
    if (lid == 0)\n\
    {\n\
        int pos = 0;\n\
#ifdef NEED_MINVAL\n\
        *(__global dstT1 *)(dstptr + mad24(gid, (int)sizeof(dstT1), pos)) = localmem_min[0];\n\
        pos = mad24(groupnum, (int)sizeof(dstT1), pos);\n\
        pos = align(pos);\n\
#endif\n\
#ifdef NEED_MAXVAL\n\
        *(__global dstT1 *)(dstptr + mad24(gid, (int)sizeof(dstT1), pos)) = localmem_max[0];\n\
        pos = mad24(groupnum, (int)sizeof(dstT1), pos);\n\
        pos = align(pos);\n\
#endif\n\
#ifdef NEED_MINLOC\n\
        *(__global uint *)(dstptr + mad24(gid, (int)sizeof(uint), pos)) = localmem_minloc[0];\n\
        pos = mad24(groupnum, (int)sizeof(uint), pos);\n\
        pos = align(pos);\n\
#endif\n\
#ifdef NEED_MAXLOC\n\
        *(__global uint *)(dstptr + mad24(gid, (int)sizeof(uint), pos)) = localmem_maxloc[0];\n\
#ifdef OP_CALC2\n\
        pos = mad24(groupnum, (int)sizeof(uint), pos);\n\
        pos = align(pos);\n\
#endif\n\
#endif\n\
#ifdef OP_CALC2\n\
        *(__global dstT1 *)(dstptr + mad24(gid, (int)sizeof(dstT1), pos)) = localmem_max2[0];\n\
#endif\n\
    }\n\
}");

ProgramSource mixchannels_oclsrc = ProgramSource("\
#define DECLARE_INPUT_MAT(i) \\\n\
    __global const uchar * src##i##ptr, int src##i##_step, int src##i##_offset,\n\
#define DECLARE_OUTPUT_MAT(i) \\\n\
    __global uchar * dst##i##ptr, int dst##i##_step, int dst##i##_offset,\n\
#define DECLARE_INDEX(i) \\\n\
    int src##i##_index = mad24(src##i##_step, y0, mad24(x, (int)sizeof(T) * scn##i, src##i##_offset)); \\\n\
    int dst##i##_index = mad24(dst##i##_step, y0, mad24(x, (int)sizeof(T) * dcn##i, dst##i##_offset));\n\
#define PROCESS_ELEM(i) \\\n\
    __global const T * src##i = (__global const T *)(src##i##ptr + src##i##_index); \\\n\
    __global T * dst##i = (__global T *)(dst##i##ptr + dst##i##_index); \\\n\
    dst##i[0] = src##i[0]; \\\n\
    src##i##_index += src##i##_step; \\\n\
    dst##i##_index += dst##i##_step;\n\
\n\
__kernel void mixChannels(DECLARE_INPUT_MAT_N DECLARE_OUTPUT_MAT_N int rows, int cols, int rowsPerWI)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < cols)\n\
    {\n\
        DECLARE_INDEX_N\n\
\n\
        for (int y = y0, y1 = min(y0 + rowsPerWI, rows); y < y1; ++y)\n\
        {\n\
            PROCESS_ELEM_N\n\
        }\n\
    }\n\
}");

ProgramSource mulspectrums_oclsrc = ProgramSource("\
inline float2 cmulf(float2 a, float2 b)\n\
{\n\
    return (float2)(mad(a.x, b.x, - a.y * b.y), mad(a.x, b.y, a.y * b.x));\n\
}\n\
\n\
inline float2 conjf(float2 a)\n\
{\n\
    return (float2)(a.x, - a.y);\n\
}\n\
\n\
__kernel void mulAndScaleSpectrums(__global const uchar * src1ptr, int src1_step, int src1_offset,\n\
                                   __global const uchar * src2ptr, int src2_step, int src2_offset,\n\
                                   __global uchar * dstptr, int dst_step, int dst_offset,\n\
                                   int dst_rows, int dst_cols, int rowsPerWI)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < dst_cols)\n\
    {\n\
        int src1_index = mad24(y0, src1_step, mad24(x, (int)sizeof(float2), src1_offset));\n\
        int src2_index = mad24(y0, src2_step, mad24(x, (int)sizeof(float2), src2_offset));\n\
        int dst_index = mad24(y0, dst_step, mad24(x, (int)sizeof(float2), dst_offset));\n\
\n\
        for (int y = y0, y1 = min(dst_rows, y0 + rowsPerWI); y < y1; ++y,\n\
            src1_index += src1_step, src2_index += src2_step, dst_index += dst_step)\n\
        {\n\
            float2 src0 = *(__global const float2 *)(src1ptr + src1_index);\n\
            float2 src1 = *(__global const float2 *)(src2ptr + src2_index);\n\
            __global float2 * dst = (__global float2 *)(dstptr + dst_index);\n\
\n\
#ifdef CONJ\n\
            float2 v = cmulf(src0, conjf(src1));\n\
#else\n\
            float2 v = cmulf(src0, src1);\n\
#endif\n\
            dst[0] = v;\n\
        }\n\
    }\n\
}");

ProgramSource normalize_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#define noconvert\n\
\n\
#if cn != 3\n\
#define loadpix(addr) *(__global const srcT *)(addr)\n\
#define storepix(val, addr)  *(__global dstT *)(addr) = val\n\
#define srcTSIZE (int)sizeof(srcT)\n\
#define dstTSIZE (int)sizeof(dstT)\n\
#else\n\
#define loadpix(addr) vload3(0, (__global const srcT1 *)(addr))\n\
#define storepix(val, addr) vstore3(val, 0, (__global dstT1 *)(addr))\n\
#define srcTSIZE ((int)sizeof(srcT1)*3)\n\
#define dstTSIZE ((int)sizeof(dstT1)*3)\n\
#endif\n\
\n\
__kernel void normalizek(__global const uchar * srcptr, int src_step, int src_offset,\n\
                         __global const uchar * mask, int mask_step, int mask_offset,\n\
                         __global uchar * dstptr, int dst_step, int dst_offset, int dst_rows, int dst_cols\n\
#ifdef HAVE_SCALE\n\
                         , float scale\n\
#endif\n\
#ifdef HAVE_DELTA\n\
                         , float delta\n\
#endif\n\
                         )\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < dst_cols)\n\
    {\n\
        int src_index  = mad24(y0, src_step, mad24(x, srcTSIZE, src_offset));\n\
        int mask_index = mad24(y0, mask_step, x + mask_offset);\n\
        int dst_index  = mad24(y0, dst_step, mad24(x, dstTSIZE, dst_offset));\n\
\n\
        for (int y = y0, y1 = min(y0 + rowsPerWI, dst_rows); y < y1;\n\
            ++y, src_index += src_step, dst_index += dst_step, mask_index += mask_step)\n\
        {\n\
            if (mask[mask_index])\n\
            {\n\
                workT value = convertToWT(loadpix(srcptr + src_index));\n\
#ifdef HAVE_SCALE\n\
#ifdef HAVE_DELTA\n\
                value = fma(value, (workT)(scale), (workT)(delta));\n\
#else\n\
                value *= (workT)(scale);\n\
#endif\n\
#else // not scale\n\
#ifdef HAVE_DELTA\n\
                value += (workT)(delta);\n\
#endif\n\
#endif\n\
\n\
                storepix(convertToDT(value), dstptr + dst_index);\n\
            }\n\
        }\n\
    }\n\
}");

ProgramSource reduce_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#if defined OP_NORM_INF_MASK\n\
\n\
#ifdef DEPTH_0\n\
#define MIN_VAL 0\n\
#define MAX_VAL 255\n\
#elif defined DEPTH_1\n\
#define MIN_VAL -128\n\
#define MAX_VAL 127\n\
#elif defined DEPTH_2\n\
#define MIN_VAL 0\n\
#define MAX_VAL 65535\n\
#elif defined DEPTH_3\n\
#define MIN_VAL -32768\n\
#define MAX_VAL 32767\n\
#elif defined DEPTH_4\n\
#define MIN_VAL INT_MIN\n\
#define MAX_VAL INT_MAX\n\
#elif defined DEPTH_5\n\
#define MIN_VAL (-FLT_MAX)\n\
#define MAX_VAL FLT_MAX\n\
#elif defined DEPTH_6\n\
#define MIN_VAL (-DBL_MAX)\n\
#define MAX_VAL DBL_MAX\n\
#endif\n\
\n\
#define dstT srcT\n\
#define dstT1 srcT1\n\
\n\
#endif // min/max stuff\n\
\n\
#define noconvert\n\
\n\
#ifndef kercn\n\
#define kercn 1\n\
#endif\n\
\n\
#ifdef HAVE_MASK_CONT\n\
#define MASK_INDEX int mask_index = id + mask_offset;\n\
#else\n\
#define MASK_INDEX int mask_index = mad24(id / cols, mask_step, mask_offset + (id % cols))\n\
#endif\n\
\n\
#if cn != 3\n\
#define loadpix(addr) *(__global const srcT *)(addr)\n\
#define storepix(val, addr)  *(__global dstT *)(addr) = val\n\
#if kercn == 1\n\
#define srcTSIZE (int)sizeof(srcT)\n\
#else\n\
#define srcTSIZE (int)sizeof(srcT1)\n\
#endif\n\
#define dstTSIZE (int)sizeof(dstT)\n\
#else\n\
#define loadpix(addr) vload3(0, (__global const srcT1 *)(addr))\n\
#define storepix(val, addr) vstore3(val, 0, (__global dstT1 *)(addr))\n\
#define srcTSIZE ((int)sizeof(srcT1)*3)\n\
#define dstTSIZE ((int)sizeof(dstT1)*3)\n\
#endif\n\
\n\
#if ddepth <= 4\n\
#define SUM_ABS(a) convertFromU(abs(a))\n\
#define SUM_ABS2(a, b) convertFromU(abs_diff(a, b))\n\
#else\n\
#define SUM_ABS(a) fabs(a)\n\
#define SUM_ABS2(a, b) fabs(a - b)\n\
#endif\n\
\n\
#ifdef HAVE_MASK\n\
#ifdef HAVE_SRC2\n\
#define EXTRA_PARAMS , __global const uchar * mask, int mask_step, int mask_offset, __global const uchar * src2ptr, int src2_step, int src2_offset\n\
#else\n\
#define EXTRA_PARAMS , __global const uchar * mask, int mask_step, int mask_offset\n\
#endif\n\
#else\n\
#ifdef HAVE_SRC2\n\
#define EXTRA_PARAMS , __global const uchar * src2ptr, int src2_step, int src2_offset\n\
#else\n\
#define EXTRA_PARAMS\n\
#endif\n\
#endif\n\
\n\
// accumulative reduction stuff\n\
#if defined OP_SUM || defined OP_SUM_ABS || defined OP_SUM_SQR || defined OP_DOT\n\
\n\
#ifdef OP_DOT\n\
#if ddepth <= 4\n\
#define FUNC(a, b, c) a = mad24(b, c, a)\n\
#else\n\
#define FUNC(a, b, c) a = mad(b, c, a)\n\
#endif\n\
\n\
#elif defined OP_SUM\n\
#define FUNC(a, b) a += b\n\
\n\
#elif defined OP_SUM_ABS\n\
#define FUNC(a, b) a += SUM_ABS(b)\n\
\n\
#elif defined OP_SUM_SQR\n\
#if ddepth <= 4\n\
#define FUNC(a, b) a = mad24(b, b, a)\n\
#else\n\
#define FUNC(a, b) a = mad(b, b, a)\n\
#endif\n\
#endif\n\
\n\
#ifdef OP_CALC2\n\
#define DECLARE_LOCAL_MEM \\\n\
    __local dstT localmem[WGS2_ALIGNED], localmem2[WGS2_ALIGNED]\n\
#define DEFINE_ACCUMULATOR \\\n\
    dstT accumulator = (dstT)(0), accumulator2 = (dstT)(0)\n\
#else\n\
#define DECLARE_LOCAL_MEM \\\n\
    __local dstT localmem[WGS2_ALIGNED]\n\
#define DEFINE_ACCUMULATOR \\\n\
    dstT accumulator = (dstT)(0)\n\
#endif\n\
\n\
#ifdef HAVE_SRC2\n\
#ifdef OP_CALC2\n\
#define PROCESS_ELEMS \\\n\
    dstT temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstT temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    temp2 = SUM_ABS(temp2); \\\n\
    FUNC(accumulator2, temp2); \\\n\
    FUNC(accumulator, temp)\n\
#else\n\
#define PROCESS_ELEMS \\\n\
    dstT temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstT temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    FUNC(accumulator, temp)\n\
#endif\n\
#else\n\
#define PROCESS_ELEMS \\\n\
    dstT temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    FUNC(accumulator, temp)\n\
#endif\n\
\n\
#ifdef HAVE_MASK\n\
#define REDUCE_GLOBAL \\\n\
    MASK_INDEX; \\\n\
    if (mask[mask_index]) \\\n\
    { \\\n\
        PROCESS_ELEMS; \\\n\
    }\n\
#elif defined OP_DOT\n\
\n\
#ifdef HAVE_SRC2_CONT\n\
#define SRC2_INDEX int src2_index = mad24(id, srcTSIZE, src2_offset);\n\
#else\n\
#define SRC2_INDEX int src2_index = mad24(id / cols, src2_step, mad24(id % cols, srcTSIZE, src2_offset))\n\
#endif\n\
\n\
#if kercn == 1\n\
#define REDUCE_GLOBAL \\\n\
    SRC2_INDEX; \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)), temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    FUNC(accumulator, temp, temp2)\n\
#elif kercn == 2\n\
#define REDUCE_GLOBAL \\\n\
    SRC2_INDEX; \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)), temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    FUNC(accumulator, temp.s0, temp2.s0); \\\n\
    FUNC(accumulator, temp.s1, temp2.s1)\n\
#elif kercn == 4\n\
#define REDUCE_GLOBAL \\\n\
    SRC2_INDEX; \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)), temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    FUNC(accumulator, temp.s0, temp2.s0); \\\n\
    FUNC(accumulator, temp.s1, temp2.s1); \\\n\
    FUNC(accumulator, temp.s2, temp2.s2); \\\n\
    FUNC(accumulator, temp.s3, temp2.s3)\n\
#elif kercn == 8\n\
#define REDUCE_GLOBAL \\\n\
    SRC2_INDEX; \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)), temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    FUNC(accumulator, temp.s0, temp2.s0); \\\n\
    FUNC(accumulator, temp.s1, temp2.s1); \\\n\
    FUNC(accumulator, temp.s2, temp2.s2); \\\n\
    FUNC(accumulator, temp.s3, temp2.s3); \\\n\
    FUNC(accumulator, temp.s4, temp2.s4); \\\n\
    FUNC(accumulator, temp.s5, temp2.s5); \\\n\
    FUNC(accumulator, temp.s6, temp2.s6); \\\n\
    FUNC(accumulator, temp.s7, temp2.s7)\n\
#elif kercn == 16\n\
#define REDUCE_GLOBAL \\\n\
    SRC2_INDEX; \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)), temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    FUNC(accumulator, temp.s0, temp2.s0); \\\n\
    FUNC(accumulator, temp.s1, temp2.s1); \\\n\
    FUNC(accumulator, temp.s2, temp2.s2); \\\n\
    FUNC(accumulator, temp.s3, temp2.s3); \\\n\
    FUNC(accumulator, temp.s4, temp2.s4); \\\n\
    FUNC(accumulator, temp.s5, temp2.s5); \\\n\
    FUNC(accumulator, temp.s6, temp2.s6); \\\n\
    FUNC(accumulator, temp.s7, temp2.s7); \\\n\
    FUNC(accumulator, temp.s8, temp2.s8); \\\n\
    FUNC(accumulator, temp.s9, temp2.s9); \\\n\
    FUNC(accumulator, temp.sA, temp2.sA); \\\n\
    FUNC(accumulator, temp.sB, temp2.sB); \\\n\
    FUNC(accumulator, temp.sC, temp2.sC); \\\n\
    FUNC(accumulator, temp.sD, temp2.sD); \\\n\
    FUNC(accumulator, temp.sE, temp2.sE); \\\n\
    FUNC(accumulator, temp.sF, temp2.sF)\n\
#endif\n\
\n\
#else // sum or norm with 2 args\n\
#ifdef HAVE_SRC2\n\
#ifdef OP_CALC2 // norm relative\n\
#if kercn == 1\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    temp2 = SUM_ABS(temp2); \\\n\
    FUNC(accumulator, temp); \\\n\
    FUNC(accumulator2, temp2)\n\
#elif kercn == 2\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    temp2 = SUM_ABS(temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator2, temp2.s0); \\\n\
    FUNC(accumulator2, temp2.s1)\n\
#elif kercn == 4\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    temp2 = SUM_ABS(temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3); \\\n\
    FUNC(accumulator2, temp2.s0); \\\n\
    FUNC(accumulator2, temp2.s1); \\\n\
    FUNC(accumulator2, temp2.s2); \\\n\
    FUNC(accumulator2, temp2.s3)\n\
#elif kercn == 8\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    temp2 = SUM_ABS(temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3); \\\n\
    FUNC(accumulator, temp.s4); \\\n\
    FUNC(accumulator, temp.s5); \\\n\
    FUNC(accumulator, temp.s6); \\\n\
    FUNC(accumulator, temp.s7); \\\n\
    FUNC(accumulator2, temp2.s0); \\\n\
    FUNC(accumulator2, temp2.s1); \\\n\
    FUNC(accumulator2, temp2.s2); \\\n\
    FUNC(accumulator2, temp2.s3); \\\n\
    FUNC(accumulator2, temp2.s4); \\\n\
    FUNC(accumulator2, temp2.s5); \\\n\
    FUNC(accumulator2, temp2.s6); \\\n\
    FUNC(accumulator2, temp2.s7)\n\
#elif kercn == 16\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    temp2 = SUM_ABS(temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3); \\\n\
    FUNC(accumulator, temp.s4); \\\n\
    FUNC(accumulator, temp.s5); \\\n\
    FUNC(accumulator, temp.s6); \\\n\
    FUNC(accumulator, temp.s7); \\\n\
    FUNC(accumulator, temp.s8); \\\n\
    FUNC(accumulator, temp.s9); \\\n\
    FUNC(accumulator, temp.sA); \\\n\
    FUNC(accumulator, temp.sB); \\\n\
    FUNC(accumulator, temp.sC); \\\n\
    FUNC(accumulator, temp.sD); \\\n\
    FUNC(accumulator, temp.sE); \\\n\
    FUNC(accumulator, temp.sF); \\\n\
    FUNC(accumulator2, temp2.s0); \\\n\
    FUNC(accumulator2, temp2.s1); \\\n\
    FUNC(accumulator2, temp2.s2); \\\n\
    FUNC(accumulator2, temp2.s3); \\\n\
    FUNC(accumulator2, temp2.s4); \\\n\
    FUNC(accumulator2, temp2.s5); \\\n\
    FUNC(accumulator2, temp2.s6); \\\n\
    FUNC(accumulator2, temp2.s7); \\\n\
    FUNC(accumulator2, temp2.s8); \\\n\
    FUNC(accumulator2, temp2.s9); \\\n\
    FUNC(accumulator2, temp2.sA); \\\n\
    FUNC(accumulator2, temp2.sB); \\\n\
    FUNC(accumulator2, temp2.sC); \\\n\
    FUNC(accumulator2, temp2.sD); \\\n\
    FUNC(accumulator2, temp2.sE); \\\n\
    FUNC(accumulator2, temp2.sF)\n\
#endif\n\
#else // norm with 2 args\n\
#if kercn == 1\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    FUNC(accumulator, temp)\n\
#elif kercn == 2\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1)\n\
#elif kercn == 4\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3)\n\
#elif kercn == 8\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3); \\\n\
    FUNC(accumulator, temp.s4); \\\n\
    FUNC(accumulator, temp.s5); \\\n\
    FUNC(accumulator, temp.s6); \\\n\
    FUNC(accumulator, temp.s7)\n\
#elif kercn == 16\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    dstTK temp2 = convertToDT(loadpix(src2ptr + src2_index)); \\\n\
    temp = SUM_ABS2(temp, temp2); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3); \\\n\
    FUNC(accumulator, temp.s4); \\\n\
    FUNC(accumulator, temp.s5); \\\n\
    FUNC(accumulator, temp.s6); \\\n\
    FUNC(accumulator, temp.s7); \\\n\
    FUNC(accumulator, temp.s8); \\\n\
    FUNC(accumulator, temp.s9); \\\n\
    FUNC(accumulator, temp.sA); \\\n\
    FUNC(accumulator, temp.sB); \\\n\
    FUNC(accumulator, temp.sC); \\\n\
    FUNC(accumulator, temp.sD); \\\n\
    FUNC(accumulator, temp.sE); \\\n\
    FUNC(accumulator, temp.sF)\n\
#endif\n\
#endif\n\
\n\
#else // sum\n\
#if kercn == 1\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    FUNC(accumulator, temp)\n\
#elif kercn == 2\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1)\n\
#elif kercn == 4\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3)\n\
#elif kercn == 8\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3); \\\n\
    FUNC(accumulator, temp.s4); \\\n\
    FUNC(accumulator, temp.s5); \\\n\
    FUNC(accumulator, temp.s6); \\\n\
    FUNC(accumulator, temp.s7)\n\
#elif kercn == 16\n\
#define REDUCE_GLOBAL \\\n\
    dstTK temp = convertToDT(loadpix(srcptr + src_index)); \\\n\
    FUNC(accumulator, temp.s0); \\\n\
    FUNC(accumulator, temp.s1); \\\n\
    FUNC(accumulator, temp.s2); \\\n\
    FUNC(accumulator, temp.s3); \\\n\
    FUNC(accumulator, temp.s4); \\\n\
    FUNC(accumulator, temp.s5); \\\n\
    FUNC(accumulator, temp.s6); \\\n\
    FUNC(accumulator, temp.s7); \\\n\
    FUNC(accumulator, temp.s8); \\\n\
    FUNC(accumulator, temp.s9); \\\n\
    FUNC(accumulator, temp.sA); \\\n\
    FUNC(accumulator, temp.sB); \\\n\
    FUNC(accumulator, temp.sC); \\\n\
    FUNC(accumulator, temp.sD); \\\n\
    FUNC(accumulator, temp.sE); \\\n\
    FUNC(accumulator, temp.sF)\n\
#endif\n\
#endif\n\
#endif\n\
\n\
#ifdef OP_CALC2\n\\\n\
#define SET_LOCAL_1 \\\n\
    localmem[lid] = accumulator; \\\n\
    localmem2[lid] = accumulator2\n\
#define REDUCE_LOCAL_1 \\\n\
    localmem[lid - WGS2_ALIGNED] += accumulator; \\\n\
    localmem2[lid - WGS2_ALIGNED] += accumulator2\n\
#define REDUCE_LOCAL_2 \\\n\
    localmem[lid] += localmem[lid2]; \\\n\
    localmem2[lid] += localmem2[lid2]\n\
#define CALC_RESULT \\\n\
    storepix(localmem[0], dstptr + dstTSIZE * gid); \\\n\
    storepix(localmem2[0], dstptr + mad24(groupnum, dstTSIZE, dstTSIZE * gid))\n\
#else\n\
#define SET_LOCAL_1 \\\n\
    localmem[lid] = accumulator\n\
#define REDUCE_LOCAL_1 \\\n\
    localmem[lid - WGS2_ALIGNED] += accumulator\n\
#define REDUCE_LOCAL_2 \\\n\
    localmem[lid] += localmem[lid2]\n\
#define CALC_RESULT \\\n\
    storepix(localmem[0], dstptr + dstTSIZE * gid)\n\
#endif\n\
\n\
// countNonZero stuff\n\
#elif defined OP_COUNT_NON_ZERO\n\
#define dstT int\n\
#define DECLARE_LOCAL_MEM \\\n\
    __local dstT localmem[WGS2_ALIGNED]\n\
#define DEFINE_ACCUMULATOR \\\n\
    dstT accumulator = (dstT)(0); \\\n\
    srcT1 zero = (srcT1)(0), one = (srcT1)(1)\n\
#if kercn == 1\n\
#define REDUCE_GLOBAL \\\n\
    accumulator += loadpix(srcptr + src_index) == zero ? zero : one\n\
#elif kercn == 2\n\
#define REDUCE_GLOBAL \\\n\
    srcT value = loadpix(srcptr + src_index); \\\n\
    accumulator += value.s0 == zero ? zero : one; \\\n\
    accumulator += value.s1 == zero ? zero : one\n\
#elif kercn == 4\n\
#define REDUCE_GLOBAL \\\n\
    srcT value = loadpix(srcptr + src_index); \\\n\
    accumulator += value.s0 == zero ? zero : one; \\\n\
    accumulator += value.s1 == zero ? zero : one; \\\n\
    accumulator += value.s2 == zero ? zero : one; \\\n\
    accumulator += value.s3 == zero ? zero : one\n\
#elif kercn == 8\n\
#define REDUCE_GLOBAL \\\n\
    srcT value = loadpix(srcptr + src_index); \\\n\
    accumulator += value.s0 == zero ? zero : one; \\\n\
    accumulator += value.s1 == zero ? zero : one; \\\n\
    accumulator += value.s2 == zero ? zero : one; \\\n\
    accumulator += value.s3 == zero ? zero : one; \\\n\
    accumulator += value.s4 == zero ? zero : one; \\\n\
    accumulator += value.s5 == zero ? zero : one; \\\n\
    accumulator += value.s6 == zero ? zero : one; \\\n\
    accumulator += value.s7 == zero ? zero : one\n\
#elif kercn == 16\n\
#define REDUCE_GLOBAL \\\n\
    srcT value = loadpix(srcptr + src_index); \\\n\
    accumulator += value.s0 == zero ? zero : one; \\\n\
    accumulator += value.s1 == zero ? zero : one; \\\n\
    accumulator += value.s2 == zero ? zero : one; \\\n\
    accumulator += value.s3 == zero ? zero : one; \\\n\
    accumulator += value.s4 == zero ? zero : one; \\\n\
    accumulator += value.s5 == zero ? zero : one; \\\n\
    accumulator += value.s6 == zero ? zero : one; \\\n\
    accumulator += value.s7 == zero ? zero : one; \\\n\
    accumulator += value.s8 == zero ? zero : one; \\\n\
    accumulator += value.s9 == zero ? zero : one; \\\n\
    accumulator += value.sA == zero ? zero : one; \\\n\
    accumulator += value.sB == zero ? zero : one; \\\n\
    accumulator += value.sC == zero ? zero : one; \\\n\
    accumulator += value.sD == zero ? zero : one; \\\n\
    accumulator += value.sE == zero ? zero : one; \\\n\
    accumulator += value.sF == zero ? zero : one\n\
#endif\n\
\n\
#define SET_LOCAL_1 \\\n\
    localmem[lid] = accumulator\n\
#define REDUCE_LOCAL_1 \\\n\
    localmem[lid - WGS2_ALIGNED] += accumulator\n\
#define REDUCE_LOCAL_2 \\\n\
    localmem[lid] += localmem[lid2]\n\
#define CALC_RESULT \\\n\
    storepix(localmem[0], dstptr + dstTSIZE * gid)\n\
\n\
#else\n\
#error \"No operation\"\n\
#endif\n\
\n\
#ifdef OP_DOT\n\
#undef EXTRA_PARAMS\n\
#define EXTRA_PARAMS , __global uchar * src2ptr, int src2_step, int src2_offset\n\
#endif\n\
\n\
__kernel void reduce(__global const uchar * srcptr, int src_step, int src_offset, int cols,\n\
                     int total, int groupnum, __global uchar * dstptr EXTRA_PARAMS)\n\
{\n\
    int lid = get_local_id(0);\n\
    int gid = get_group_id(0);\n\
    int  id = get_global_id(0) * kercn;\n\
\n\
    srcptr += src_offset;\n\
#ifdef HAVE_SRC2\n\
    src2ptr += src2_offset;\n\
#endif\n\
\n\
    DECLARE_LOCAL_MEM;\n\
    DEFINE_ACCUMULATOR;\n\
\n\
    for (int grain = groupnum * WGS * kercn; id < total; id += grain)\n\
    {\n\
#ifdef HAVE_SRC_CONT\n\
        int src_index = id * srcTSIZE;\n\
#else\n\
        int src_index = mad24(id / cols, src_step, mul24(id % cols, srcTSIZE));\n\
#endif\n\
#ifdef HAVE_SRC2\n\
#ifdef HAVE_SRC2_CONT\n\
        int src2_index = id * srcTSIZE;\n\
#else\n\
        int src2_index = mad24(id / cols, src2_step, mul24(id % cols, srcTSIZE));\n\
#endif\n\
#endif\n\
        REDUCE_GLOBAL;\n\
    }\n\
\n\
    if (lid < WGS2_ALIGNED)\n\
    {\n\
        SET_LOCAL_1;\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (lid >= WGS2_ALIGNED && total >= WGS2_ALIGNED)\n\
    {\n\
        REDUCE_LOCAL_1;\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    for (int lsize = WGS2_ALIGNED >> 1; lsize > 0; lsize >>= 1)\n\
    {\n\
        if (lid < lsize)\n\
        {\n\
           int lid2 = lsize + lid;\n\
           REDUCE_LOCAL_2;\n\
        }\n\
        barrier(CLK_LOCAL_MEM_FENCE);\n\
    }\n\
\n\
    if (lid == 0)\n\
    {\n\
        CALC_RESULT;\n\
    }\n\
}");

ProgramSource reduce2_oclsrc = ProgramSource("\
#ifdef DOUBLE_SUPPORT\n\
#ifdef cl_amd_fp64\n\
#pragma OPENCL EXTENSION cl_amd_fp64:enable\n\
#elif defined (cl_khr_fp64)\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
#endif\n\
#endif\n\
\n\
#if ddepth == 0\n\
#define MIN_VAL 0\n\
#define MAX_VAL 255\n\
#elif ddepth == 1\n\
#define MIN_VAL -128\n\
#define MAX_VAL 127\n\
#elif ddepth == 2\n\
#define MIN_VAL 0\n\
#define MAX_VAL 65535\n\
#elif ddepth == 3\n\
#define MIN_VAL -32768\n\
#define MAX_VAL 32767\n\
#elif ddepth == 4\n\
#define MIN_VAL INT_MIN\n\
#define MAX_VAL INT_MAX\n\
#elif ddepth == 5\n\
#define MIN_VAL (-FLT_MAX)\n\
#define MAX_VAL FLT_MAX\n\
#elif ddepth == 6\n\
#define MIN_VAL (-DBL_MAX)\n\
#define MAX_VAL DBL_MAX\n\
#else\n\
#error \"Unsupported depth\"\n\
#endif\n\
\n\
#define noconvert\n\
\n\
#if defined OCL_CV_REDUCE_SUM || defined OCL_CV_REDUCE_AVG\n\
#define INIT_VALUE 0\n\
#define PROCESS_ELEM(acc, value) acc += value\n\
#elif defined OCL_CV_REDUCE_MAX\n\
#define INIT_VALUE MIN_VAL\n\
#define PROCESS_ELEM(acc, value) acc = max(value, acc)\n\
#elif defined OCL_CV_REDUCE_MIN\n\
#define INIT_VALUE MAX_VAL\n\
#define PROCESS_ELEM(acc, value) acc = min(value, acc)\n\
#elif defined OCL_CV_REDUCE_SUM2\n\
#define INIT_VALUE 0\n\
#define PROCESS_ELEM(acc, value) acc += value*value\n\
#else\n\
#error \"No operation is specified\"\n\
#endif\n\
\n\
#ifdef OP_REDUCE_PRE\n\
\n\
__kernel void reduce_horz_opt(__global const uchar * srcptr, int src_step, int src_offset, int rows, int cols,\n\
                     __global uchar * dstptr, int dst_step, int dst_offset\n\
#ifdef OCL_CV_REDUCE_AVG\n\
                     , float fscale\n\
#endif\n\
                     )\n\
{\n\
    __local bufT lsmem[TILE_HEIGHT][BUF_COLS][cn];\n\
\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1);\n\
    int liy = get_local_id(1);\n\
    if ((x < BUF_COLS) && (y < rows))\n\
    {\n\
        int src_index = mad24(y, src_step, mad24(x, (int)sizeof(srcT) * cn, src_offset));\n\
\n\
        __global const srcT * src = (__global const srcT *)(srcptr + src_index);\n\
        bufT tmp[cn];\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
            tmp[c] = INIT_VALUE;\n\
\n\
        int src_step_mul = BUF_COLS * cn;\n\
        for (int idx = x; idx < cols; idx += BUF_COLS, src += src_step_mul)\n\
        {\n\
            #pragma unroll\n\
            for (int c = 0; c < cn; ++c)\n\
            {\n\
                bufT value = convertToBufT(src[c]);\n\
                PROCESS_ELEM(tmp[c], value);\n\
            }\n\
        }\n\
\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
            lsmem[liy][x][c] = tmp[c];\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
    if ((x < BUF_COLS / 2) && (y < rows))\n\
    {\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
        {\n\
            PROCESS_ELEM(lsmem[liy][x][c], lsmem[liy][x +  BUF_COLS / 2][c]);\n\
        }\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
    if ((x == 0) && (y < rows))\n\
    {\n\
        int dst_index = mad24(y, dst_step, dst_offset);\n\
\n\
        __global dstT * dst = (__global dstT *)(dstptr + dst_index);\n\
        bufT tmp[cn];\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
            tmp[c] = INIT_VALUE;\n\
\n\
        #pragma unroll\n\
        for (int xin = 0; xin < BUF_COLS / 2; xin ++)\n\
        {\n\
            #pragma unroll\n\
            for (int c = 0; c < cn; ++c)\n\
            {\n\
                PROCESS_ELEM(tmp[c], lsmem[liy][xin][c]);\n\
            }\n\
        }\n\
\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
#ifdef OCL_CV_REDUCE_AVG\n\
            dst[c] = convertToDT(convertToWT(tmp[c]) * fscale);\n\
#else\n\
            dst[c] = convertToDT(tmp[c]);\n\
#endif\n\
    }\n\
}\n\
\n\
#else\n\
__kernel void reduce(__global const uchar * srcptr, int src_step, int src_offset, int rows, int cols,\n\
                     __global uchar * dstptr, int dst_step, int dst_offset\n\
#ifdef OCL_CV_REDUCE_AVG\n\
                     , float fscale\n\
#endif\n\
                     )\n\
{\n\
#if dim == 0 // reduce to a single row\n\
    int x = get_global_id(0);\n\
    if (x < cols)\n\
    {\n\
        int src_index = mad24(x, (int)sizeof(srcT) * cn, src_offset);\n\
        int dst_index = mad24(x, (int)sizeof(dstT0) * cn, dst_offset);\n\
\n\
        __global dstT0 * dst = (__global dstT0 *)(dstptr + dst_index);\n\
        dstT tmp[cn];\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
            tmp[c] = INIT_VALUE;\n\
\n\
        for (int y = 0; y < rows; ++y, src_index += src_step)\n\
        {\n\
            __global const srcT * src = (__global const srcT *)(srcptr + src_index);\n\
            #pragma unroll\n\
            for (int c = 0; c < cn; ++c)\n\
            {\n\
                dstT value = convertToDT(src[c]);\n\
                PROCESS_ELEM(tmp[c], value);\n\
            }\n\
        }\n\
\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
#ifdef OCL_CV_REDUCE_AVG\n\
            dst[c] = convertToDT0(convertToWT(tmp[c]) * fscale);\n\
#else\n\
            dst[c] = convertToDT0(tmp[c]);\n\
#endif\n\
    }\n\
#elif dim == 1 // reduce to a single column\n\
    int y = get_global_id(0);\n\
    if (y < rows)\n\
    {\n\
        int src_index = mad24(y, src_step, src_offset);\n\
        int dst_index = mad24(y, dst_step, dst_offset);\n\
\n\
        __global const srcT * src = (__global const srcT *)(srcptr + src_index);\n\
        __global dstT * dst = (__global dstT *)(dstptr + dst_index);\n\
        dstT tmp[cn];\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
            tmp[c] = INIT_VALUE;\n\
\n\
        for (int x = 0; x < cols; ++x, src += cn)\n\
        {\n\
            #pragma unroll\n\
            for (int c = 0; c < cn; ++c)\n\
            {\n\
                dstT value = convertToDT(src[c]);\n\
                PROCESS_ELEM(tmp[c], value);\n\
            }\n\
        }\n\
\n\
        #pragma unroll\n\
        for (int c = 0; c < cn; ++c)\n\
#ifdef OCL_CV_REDUCE_AVG\n\
            dst[c] = convertToDT0(convertToWT(tmp[c]) * fscale);\n\
#else\n\
            dst[c] = convertToDT0(tmp[c]);\n\
#endif\n\
    }\n\
#else\n\
#error \"Dims must be either 0 or 1\"\n\
#endif\n\
}\n\
#endif");

ProgramSource repeat_oclsrc = ProgramSource("\
#if cn != 3\n\
#define loadpix(addr) *(__global const T *)(addr)\n\
#define storepix(val, addr)  *(__global T *)(addr) = val\n\
#define TSIZE (int)sizeof(T)\n\
#else\n\
#define loadpix(addr) vload3(0, (__global const T1 *)(addr))\n\
#define storepix(val, addr) vstore3(val, 0, (__global T1 *)(addr))\n\
#define TSIZE ((int)sizeof(T1)*3)\n\
#endif\n\
\n\
__kernel void repeat(__global const uchar * srcptr, int src_step, int src_offset, int src_rows, int src_cols,\n\
                     __global uchar * dstptr, int dst_step, int dst_offset)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < src_cols)\n\
    {\n\
        int src_index = mad24(y0, src_step, mad24(x, (int)sizeof(T), src_offset));\n\
        int dst_index0 = mad24(y0, dst_step, mad24(x, (int)sizeof(T), dst_offset));\n\
\n\
        for (int y = y0, y1 = min(src_rows, y0 + rowsPerWI); y < y1; ++y, src_index += src_step, dst_index0 += dst_step)\n\
        {\n\
            T srcelem = loadpix(srcptr + src_index);\n\
\n\
            #pragma unroll\n\
            for (int ey = 0; ey < ny; ++ey)\n\
            {\n\
                int dst_index = mad24(ey * src_rows, dst_step, dst_index0);\n\
\n\
                #pragma unroll\n\
                for (int ex = 0; ex < nx; ++ex)\n\
                {\n\
                    storepix(srcelem, dstptr + dst_index);\n\
                    dst_index = mad24(src_cols, (int)sizeof(T), dst_index);\n\
                }\n\
            }\n\
        }\n\
    }\n\
}");

ProgramSource set_identity_oclsrc = ProgramSource("\
#if kercn != 3\n\
#define storepix(val, addr)  *(__global T *)(addr) = val\n\
#define TSIZE (int)sizeof(T)\n\
#define scalar scalar_\n\
#else\n\
#define storepix(val, addr) vstore3(val, 0, (__global T1 *)(addr))\n\
#define TSIZE ((int)sizeof(T1)*3)\n\
#define scalar (T)(scalar_.x, scalar_.y, scalar_.z)\n\
#endif\n\
\n\
__kernel void setIdentity(__global uchar * srcptr, int src_step, int src_offset, int rows, int cols,\n\
                          ST scalar_)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < cols)\n\
    {\n\
        int src_index = mad24(y0, src_step, mad24(x, TSIZE, src_offset));\n\
\n\
#if kercn == cn\n\
        #pragma unroll\n\
        for (int y = y0, i = 0, y1 = min(rows, y0 + rowsPerWI); i < rowsPerWI; ++y, ++i, src_index += src_step)\n\
            if (y < y1)\n\
                storepix(x == y ? scalar : (T)(0), srcptr + src_index);\n\
#elif kercn == 4 && cn == 1\n\
        if (y0 < rows)\n\
        {\n\
            storepix(x == y0 >> 2 ? (T)(scalar, 0, 0, 0) : (T)(0), srcptr + src_index);\n\
            if (++y0 < rows)\n\
            {\n\
                src_index += src_step;\n\
                storepix(x == y0 >> 2 ? (T)(0, scalar, 0, 0) : (T)(0), srcptr + src_index);\n\
\n\
                if (++y0 < rows)\n\
                {\n\
                    src_index += src_step;\n\
                    storepix(x == y0 >> 2 ? (T)(0, 0, scalar, 0) : (T)(0), srcptr + src_index);\n\
\n\
                    if (++y0 < rows)\n\
                    {\n\
                        src_index += src_step;\n\
                        storepix(x == y0 >> 2 ? (T)(0, 0, 0, scalar) : (T)(0), srcptr + src_index);\n\
                    }\n\
                }\n\
            }\n\
        }\n\
#else\n\
#error \"Incorrect combination of cn && kercn\"\n\
#endif\n\
    }\n\
}");

ProgramSource split_merge_oclsrc = ProgramSource("\
#ifdef OP_MERGE\n\
\n\
#define DECLARE_SRC_PARAM(index) __global const uchar * src##index##ptr, int src##index##_step, int src##index##_offset,\n\
#define DECLARE_INDEX(index) int src##index##_index = mad24(src##index##_step, y0, mad24(x, (int)sizeof(T) * scn##index, src##index##_offset));\n\
#define PROCESS_ELEM(index) \\\n\
    __global const T * src##index = (__global const T *)(src##index##ptr + src##index##_index); \\\n\
    dst[index] = src##index[0]; \\\n\
    src##index##_index += src##index##_step;\n\
\n\
__kernel void merge(DECLARE_SRC_PARAMS_N\n\
                    __global uchar * dstptr, int dst_step, int dst_offset,\n\
                    int rows, int cols, int rowsPerWI)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < cols)\n\
    {\n\
        DECLARE_INDEX_N\n\
        int dst_index = mad24(dst_step, y0, mad24(x, (int)sizeof(T) * cn, dst_offset));\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y, dst_index += dst_step)\n\
        {\n\
            __global T * dst = (__global T *)(dstptr + dst_index);\n\
\n\
            PROCESS_ELEMS_N\n\
        }\n\
    }\n\
}\n\
\n\
#elif defined OP_SPLIT\n\
#define DECLARE_DST_PARAM(index) , __global uchar * dst##index##ptr, int dst##index##_step, int dst##index##_offset\n\
#define DECLARE_INDEX(index) int dst##index##_index = mad24(y0, dst##index##_step, mad24(x, (int)sizeof(T), dst##index##_offset));\n\
#define PROCESS_ELEM(index) \\\n\
    __global T * dst##index = (__global T *)(dst##index##ptr + dst##index##_index); \\\n\
    dst##index[0] = src[index]; \\\n\
    dst##index##_index += dst##index##_step;\n\
\n\
__kernel void split(__global uchar* srcptr, int src_step, int src_offset, int rows, int cols DECLARE_DST_PARAMS, int rowsPerWI)\n\
{\n\
    int x = get_global_id(0);\n\
    int y0 = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < cols)\n\
    {\n\
        DECLARE_INDEX_N\n\
        int src_index = mad24(y0, src_step, mad24(x, cn * (int)sizeof(T), src_offset));\n\
\n\
        for (int y = y0, y1 = min(rows, y0 + rowsPerWI); y < y1; ++y, src_index += src_step)\n\
        {\n\
            __global const T * src = (__global const T *)(srcptr + src_index);\n\
\n\
            PROCESS_ELEMS_N\n\
        }\n\
    }\n\
}\n\
\n\
#else\n\
#error \"No operation\"\n\
#endif");

ProgramSource transpose_oclsrc = ProgramSource("\
#if cn != 3\n\
#define loadpix(addr) *(__global const T *)(addr)\n\
#define storepix(val, addr)  *(__global T *)(addr) = val\n\
#define TSIZE (int)sizeof(T)\n\
#else\n\
#define loadpix(addr) vload3(0, (__global const T1 *)(addr))\n\
#define storepix(val, addr) vstore3(val, 0, (__global T1 *)(addr))\n\
#define TSIZE ((int)sizeof(T1)*3)\n\
#endif\n\
\n\
#ifndef INPLACE\n\
\n\
#define LDS_STEP      (TILE_DIM + 1)\n\
\n\
__kernel void transpose(__global const uchar * srcptr, int src_step, int src_offset, int src_rows, int src_cols,\n\
                        __global uchar * dstptr, int dst_step, int dst_offset)\n\
{\n\
    int gp_x = get_group_id(0),   gp_y = get_group_id(1);\n\
    int gs_x = get_num_groups(0), gs_y = get_num_groups(1);\n\
\n\
    int groupId_x, groupId_y;\n\
\n\
    if (src_rows == src_cols)\n\
    {\n\
        groupId_y = gp_x;\n\
        groupId_x = (gp_x + gp_y) % gs_x;\n\
    }\n\
    else\n\
    {\n\
        int bid = mad24(gs_x, gp_y, gp_x);\n\
        groupId_y =  bid % gs_y;\n\
        groupId_x = ((bid / gs_y) + groupId_y) % gs_x;\n\
    }\n\
\n\
    int lx = get_local_id(0);\n\
    int ly = get_local_id(1);\n\
\n\
    int x = mad24(groupId_x, TILE_DIM, lx);\n\
    int y = mad24(groupId_y, TILE_DIM, ly);\n\
\n\
    int x_index = mad24(groupId_y, TILE_DIM, lx);\n\
    int y_index = mad24(groupId_x, TILE_DIM, ly);\n\
\n\
    __local T tile[TILE_DIM * LDS_STEP];\n\
\n\
    if (x < src_cols && y < src_rows)\n\
    {\n\
        int index_src = mad24(y, src_step, mad24(x, TSIZE, src_offset));\n\
\n\
        #pragma unroll\n\
        for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS)\n\
            if (y + i < src_rows)\n\
            {\n\
                tile[mad24(ly + i, LDS_STEP, lx)] = loadpix(srcptr + index_src);\n\
                index_src = mad24(BLOCK_ROWS, src_step, index_src);\n\
            }\n\
    }\n\
    barrier(CLK_LOCAL_MEM_FENCE);\n\
\n\
    if (x_index < src_rows && y_index < src_cols)\n\
    {\n\
        int index_dst = mad24(y_index, dst_step, mad24(x_index, TSIZE, dst_offset));\n\
\n\
        #pragma unroll\n\
        for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS)\n\
            if ((y_index + i) < src_cols)\n\
            {\n\
                storepix(tile[mad24(lx, LDS_STEP, ly + i)], dstptr + index_dst);\n\
                index_dst = mad24(BLOCK_ROWS, dst_step, index_dst);\n\
            }\n\
    }\n\
}\n\
\n\
#else\n\
__kernel void transpose_inplace(__global uchar * srcptr, int src_step, int src_offset, int src_rows)\n\
{\n\
    int x = get_global_id(0);\n\
    int y = get_global_id(1) * rowsPerWI;\n\
\n\
    if (x < y + rowsPerWI)\n\
    {\n\
        int src_index = mad24(y, src_step, mad24(x, TSIZE, src_offset));\n\
        int dst_index = mad24(x, src_step, mad24(y, TSIZE, src_offset));\n\
        T tmp;\n\
\n\
        #pragma unroll\n\
        for (int i = 0; i < rowsPerWI; ++i, ++y, src_index += src_step, dst_index += TSIZE)\n\
            if (y < src_rows && x < y)\n\
            {\n\
                __global uchar * src = srcptr + src_index;\n\
                __global uchar * dst = srcptr + dst_index;\n\
\n\
                tmp = loadpix(dst);\n\
                storepix(loadpix(src), dst);\n\
                storepix(tmp, src);\n\
            }\n\
    }\n\
}\n\
#endif");

}
