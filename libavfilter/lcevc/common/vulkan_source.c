#include <stddef.h>
#include "vulkan_source.h"

const unsigned char *upscale_vertical_spv = "\
// Vulkan Vertical Upscale shader. Specialised for a 4 tap kernel.\n\
\n\
#version 450\n\
\n\
layout(local_size_x_id = 0, local_size_y_id = 1, local_size_z_id = 2) in;\n\
\n\
layout(std430, set = 0, binding = 0) buffer restrict readonly SrcImageBuffer {\n\
    uint samples[];\n\
} src;\n\
\n\
layout(std430, set = 0, binding = 1) buffer restrict writeonly DstImageBuffer {\n\
    uint samples[];\n\
} dst;\n\
\n\
#include \"common.glsl\"\n\
#include \"upscale_constants.glsl\"\n\
\n\
void main()\n\
{\n\
    const uint x = gl_GlobalInvocationID.x;\n\
    const uint y = gl_GlobalInvocationID.y;\n\
    const uint dstWidth = srcWidth;\n\
    const uint dstHeight = 2 * srcHeight;\n\
    const uint packDensity = 2; // two internal format pixels per uint (for now)\n\
    const uint srcWidthPacked = pixelStrideIn / packDensity;\n\
    const uint srcWidthPackedReal = srcWidth / packDensity;\n\
    const uint dstWidthPacked = dstWidth / packDensity;\n\
    const uint srcOffset = pixelOffsetIn / packDensity;\n\
    const uint dstOffset = pixelOffsetIn; // 2x for doubling image size\n\
\n\
    if (x >= srcWidthPackedReal || y >= srcHeight) {\n\
        return;\n\
    }\n\
\n\
    if (y == 0) { // first and last pixel in pairs\n\
        const ivec2 first = unpack16bitInternals(src.samples[srcOffset + x]);\n\
        const ivec2 second = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked]);\n\
\n\
        ivec2 dstFirst;\n\
        dstFirst.x = clamp_s15(ds + kernel[3] * first.x + kernel[2] * first.x + kernel[1] * first.x + kernel[0] * second.x);\n\
        dstFirst.y = clamp_s15(ds + kernel[3] * first.y + kernel[2] * first.y + kernel[1] * first.y + kernel[0] * second.y);\n\
\n\
        dst.samples[dstOffset + x] = pack16bitInternals(dstFirst);\n\
\n\
        const ivec2 last = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (srcHeight - 1)]);\n\
        const ivec2 secondToLast = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (srcHeight - 2)]);\n\
\n\
        ivec2 dstLast;\n\
        dstLast.x = clamp_s15(ds + kernel[0] * secondToLast.x + kernel[1] * last.x + kernel[2] * last.x + kernel[3] * last.x);\n\
        dstLast.y = clamp_s15(ds + kernel[0] * secondToLast.y + kernel[1] * last.y + kernel[2] * last.y + kernel[3] * last.y);\n\
\n\
        dst.samples[dstOffset + x + dstWidthPacked * (dstHeight - 1)] = pack16bitInternals(dstLast);\n\
    } else if (y == 1) { // first pair of pixels in pairs\n\
        const ivec2 first = unpack16bitInternals(src.samples[srcOffset + x]);\n\
        const ivec2 second = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked]);\n\
        const ivec2 third = unpack16bitInternals(src.samples[srcOffset + x + 2 * srcWidthPacked]);\n\
\n\
        ivec2 dstForwards;\n\
        dstForwards.x = clamp_s15(ds + kernel[0] * first.x + kernel[1] * first.x + kernel[2] * second.x + kernel[3] * third.x);\n\
        dstForwards.y = clamp_s15(ds + kernel[0] * first.y + kernel[1] * first.y + kernel[2] * second.y + kernel[3] * third.y);\n\
\n\
        ivec2 dstBackwards;\n\
        dstBackwards.x = clamp_s15(ds + kernel[3] * first.x + kernel[2] * first.x + kernel[1] * second.x + kernel[0] * third.x);\n\
        dstBackwards.y = clamp_s15(ds + kernel[3] * first.y + kernel[2] * first.y + kernel[1] * second.y + kernel[0] * third.y);\n\
\n\
        dst.samples[dstOffset + x + dstWidthPacked] = pack16bitInternals(dstForwards);\n\
        dst.samples[dstOffset + x + 2 * dstWidthPacked] = pack16bitInternals(dstBackwards);\n\
    }\n\
\n\
    else if (y == 2) { // last pair of pixels in pairs\n\
        const ivec2 last = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (srcHeight - 1)]);\n\
        const ivec2 secondToLast = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (srcHeight - 2)]);\n\
        const ivec2 thirdToLast = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (srcHeight - 3)]);\n\
\n\
        ivec2 dstForwards;\n\
        dstForwards.x = clamp_s15(ds + kernel[0] * thirdToLast.x + kernel[1] * secondToLast.x + kernel[2] * last.x + kernel[3] * last.x);\n\
        dstForwards.y = clamp_s15(ds + kernel[0] * thirdToLast.y + kernel[1] * secondToLast.y + kernel[2] * last.y + kernel[3] * last.y);\n\
\n\
        ivec2 dstBackwards;\n\
        dstBackwards.x = clamp_s15(ds + kernel[3] * thirdToLast.x + kernel[2] * secondToLast.x + kernel[1] * last.x + kernel[0] * last.x);\n\
        dstBackwards.y = clamp_s15(ds + kernel[3] * thirdToLast.y + kernel[2] * secondToLast.y + kernel[1] * last.y + kernel[0] * last.y);\n\
\n\
        dst.samples[dstOffset + x + dstWidthPacked * (dstHeight - 3)] = pack16bitInternals(dstForwards);\n\
        dst.samples[dstOffset + x + dstWidthPacked * (dstHeight - 2)] = pack16bitInternals(dstBackwards);\n\
    } else if (y < srcHeight) { // middle pairs of pixels in pairs\n\
        const ivec2 first = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (y - 3)]);\n\
        const ivec2 second = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (y - 2)]);\n\
        const ivec2 third = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * (y - 1)]);\n\
        const ivec2 fourth = unpack16bitInternals(src.samples[srcOffset + x + srcWidthPacked * y]);\n\
\n\
        ivec2 dstForwards;\n\
        dstForwards.x = clamp_s15(ds + kernel[0] * first.x + kernel[1] * second.x + kernel[2] * third.x + kernel[3] * fourth.x);\n\
        dstForwards.y = clamp_s15(ds + kernel[0] * first.y + kernel[1] * second.y + kernel[2] * third.y + kernel[3] * fourth.y);\n\
\n\
        ivec2 dstBackwards;\n\
        dstBackwards.x = clamp_s15(ds + kernel[3] * first.x + kernel[2] * second.x + kernel[1] * third.x + kernel[0] * fourth.x);\n\
        dstBackwards.y = clamp_s15(ds + kernel[3] * first.y + kernel[2] * second.y + kernel[1] * third.y + kernel[0] * fourth.y);\n\
\n\
        dst.samples[dstOffset + x + dstWidthPacked * (2 * y - 3)] = pack16bitInternals(dstForwards);\n\
        dst.samples[dstOffset + x + dstWidthPacked * (2 * y - 2)] = pack16bitInternals(dstBackwards);\n\
    }\n\
}";

const unsigned char *upscale_horizontal_spv = "\
// Vulkan Horizontal Upscale shader. Specialised for a 4 tap kernel.\n\
\n\
#version 450\n\
\n\
layout(local_size_x_id = 0, local_size_y_id = 1, local_size_z_id = 2) in;\n\
\n\
layout(std430, set = 0, binding = 0) buffer restrict readonly SrcImageBuffer {\n\
    uint samples[];\n\
} src;\n\
\n\
layout(std430, set = 0, binding = 1) buffer restrict writeonly DstImageBuffer {\n\
    uint samples[];\n\
} dst;\n\
\n\
layout(std430, set = 0, binding = 2) buffer restrict writeonly BaseImageBuffer {\n\
    uint samples[];\n\
} base;\n\
\n\
#include \"common.glsl\"\n\
#include \"upscale_constants.glsl\"\n\
\n\
void main()\n\
{\n\
    const uint x = gl_GlobalInvocationID.x;\n\
    const uint y = gl_GlobalInvocationID.y;\n\
    const uint dstWidth = 2 * srcWidth;\n\
    const uint dstHeight = srcHeight;\n\
    const uint packDensity = 2;\n\
    const uint srcWidthPacked = srcWidth / 2; // horizontal src is always 16 bit and stride == width\n\
    const uint dstWidthPacked = pixelStrideOut / packDensity;\n\
    const uint dstWidthPackedReal = dstWidth / packDensity;\n\
    const uint srcOffset = pixelOffsetIn / 2;\n\
    const uint dstOffset = pixelOffsetIn;\n\
    const uint baseOffset = pixelOffsetIn / 4;\n\
\n\
    if (x >= srcWidthPacked || y >= srcHeight / 2) {\n\
        return;\n\
    }\n\
\n\
    if (x == 0) { // first four pixels performed for two rows\n\
        // 1st row\n\
        const ivec2 leftRow1 = unpack16bitInternals(src.samples[srcOffset + 2 * y * srcWidthPacked]);\n\
        const ivec2 rightRow1 = unpack16bitInternals(src.samples[srcOffset + 1 + 2 * y * srcWidthPacked]);\n\
        const ivec4 row1 = ivec4(leftRow1, rightRow1);\n\
\n\
        ivec4 dstRow1;\n\
        dstRow1.x = clamp_s15(ds + kernel[3] * row1.x + kernel[2] * row1.x + kernel[1] * row1.x + kernel[0] * row1.y);\n\
        dstRow1.y = clamp_s15(ds + kernel[0] * row1.x + kernel[1] * row1.x + kernel[2] * row1.y + kernel[3] * row1.z);\n\
        dstRow1.z = clamp_s15(ds + kernel[3] * row1.x + kernel[2] * row1.x + kernel[1] * row1.y + kernel[0] * row1.z);\n\
        dstRow1.w = clamp_s15(ds + kernel[0] * row1.x + kernel[1] * row1.y + kernel[2] * row1.z + kernel[3] * row1.w);\n\
\n\
        // 2nd row\n\
        const ivec2 leftRow2 = unpack16bitInternals(src.samples[srcOffset + (2 * y + 1) * srcWidthPacked]);\n\
        const ivec2 rightRow2 = unpack16bitInternals(src.samples[srcOffset + 1 + (2 * y + 1) * srcWidthPacked]);\n\
        const ivec4 row2 = ivec4(leftRow2, rightRow2);\n\
\n\
        ivec4 dstRow2;\n\
        dstRow2.x = clamp_s15(ds + kernel[3] * row2.x + kernel[2] * row2.x + kernel[1] * row2.x + kernel[0] * row2.y);\n\
        dstRow2.y = clamp_s15(ds + kernel[0] * row2.x + kernel[1] * row2.x + kernel[2] * row2.y + kernel[3] * row2.z);\n\
        dstRow2.z = clamp_s15(ds + kernel[3] * row2.x + kernel[2] * row2.x + kernel[1] * row2.y + kernel[0] * row2.z);\n\
        dstRow2.w = clamp_s15(ds + kernel[0] * row2.x + kernel[1] * row2.y + kernel[2] * row2.z + kernel[3] * row2.w);\n\
\n\
        // PA\n\
        int adjustA = 0;\n\
        int adjustB = 0;\n\
        if (pa == 1) {\n\
            const ivec2 basePels = unpack16bitInternals(base.samples[baseOffset + y * srcWidthPacked]);\n\
            adjustA = basePels.x - ((dstRow1.x + dstRow1.y + dstRow2.x + dstRow2.y + 2) >> 2);\n\
            adjustB = basePels.y - ((dstRow1.z + dstRow1.w + dstRow2.z + dstRow2.w + 2) >> 2);\n\
        }\n\
\n\
        dst.samples[dstOffset + 2 * y * dstWidthPacked] = pack16bitInternals(dstRow1.xy + adjustA);\n\
        dst.samples[dstOffset + 1 + 2 * y * dstWidthPacked] = pack16bitInternals(dstRow1.zw + adjustB);\n\
        dst.samples[dstOffset + (2 * y + 1) * dstWidthPacked] = pack16bitInternals(dstRow2.xy + adjustA);\n\
        dst.samples[dstOffset + 1 + (2 * y + 1) * dstWidthPacked] = pack16bitInternals(dstRow2.zw + adjustB);\n\
    } else if (x == srcWidthPacked - 1) { // last four pixels performed for two rows\n\
        // 1st row\n\
        const ivec2 leftRow1 = unpack16bitInternals(src.samples[srcOffset + srcWidthPacked - 2 + 2 * y * srcWidthPacked]);\n\
        const ivec2 rightRow1 = unpack16bitInternals(src.samples[srcOffset + srcWidthPacked - 1 + 2 * y * srcWidthPacked]);\n\
        const ivec4 row1 = ivec4(leftRow1, rightRow1);\n\
\n\
        ivec4 dstRow1;\n\
        dstRow1.x = clamp_s15(ds + kernel[3] * row1.x + kernel[2] * row1.y + kernel[1] * row1.z + kernel[0] * row1.w);\n\
        dstRow1.y = clamp_s15(ds + kernel[0] * row1.y + kernel[1] * row1.z + kernel[2] * row1.w + kernel[3] * row1.w);\n\
        dstRow1.z = clamp_s15(ds + kernel[3] * row1.y + kernel[2] * row1.z + kernel[1] * row1.w + kernel[0] * row1.w);\n\
        dstRow1.w = clamp_s15(ds + kernel[0] * row1.z + kernel[1] * row1.w + kernel[2] * row1.w + kernel[3] * row1.w);\n\
\n\
        // 2nd row\n\
        const ivec2 leftRow2 = unpack16bitInternals(src.samples[srcOffset + srcWidthPacked - 2 + (2 * y + 1) * srcWidthPacked]);\n\
        const ivec2 rightRow2 = unpack16bitInternals(src.samples[srcOffset + srcWidthPacked - 1 + (2 * y + 1) * srcWidthPacked]);\n\
        const ivec4 row2 = ivec4(leftRow2, rightRow2);\n\
\n\
        ivec4 dstRow2;\n\
        dstRow2.x = clamp_s15(ds + kernel[3] * row2.x + kernel[2] * row2.y + kernel[1] * row2.z + kernel[0] * row2.w);\n\
        dstRow2.y = clamp_s15(ds + kernel[0] * row2.y + kernel[1] * row2.z + kernel[2] * row2.w + kernel[3] * row2.w);\n\
        dstRow2.z = clamp_s15(ds + kernel[3] * row2.y + kernel[2] * row2.z + kernel[1] * row2.w + kernel[0] * row2.w);\n\
        dstRow2.w = clamp_s15(ds + kernel[0] * row2.z + kernel[1] * row2.w + kernel[2] * row2.w + kernel[3] * row2.w);\n\
\n\
        // PA\n\
        int adjustA = 0;\n\
        int adjustB = 0;\n\
        if (pa == 1) {\n\
            const ivec2 basePels = unpack16bitInternals(base.samples[baseOffset + x + y * srcWidthPacked]);\n\
            adjustA = basePels.x - ((dstRow1.x + dstRow1.y + dstRow2.x + dstRow2.y + 2) >> 2);\n\
            adjustB = basePels.y - ((dstRow1.z + dstRow1.w + dstRow2.z + dstRow2.w + 2) >> 2);\n\
        }\n\
        dst.samples[dstOffset + dstWidthPackedReal - 2 + 2 * y * dstWidthPacked] = pack16bitInternals(dstRow1.xy + adjustA);\n\
        dst.samples[dstOffset + dstWidthPackedReal - 1 + 2 * y * dstWidthPacked] = pack16bitInternals(dstRow1.zw + adjustB);\n\
        dst.samples[dstOffset + dstWidthPackedReal - 2 + (2 * y + 1) * dstWidthPacked] = pack16bitInternals(dstRow2.xy + adjustA);\n\
        dst.samples[dstOffset + dstWidthPackedReal - 1 + (2 * y + 1) * dstWidthPacked] = pack16bitInternals(dstRow2.zw + adjustB);\n\
    } else if (x < srcWidthPacked - 1) { // middle pixels in fours performed for two rows.\n\
        // 1st row\n\
        const ivec2 firstRow1 = unpack16bitInternals(src.samples[srcOffset + x - 1 + 2 * y * srcWidthPacked]);\n\
        const ivec2 secondRow1 = unpack16bitInternals(src.samples[srcOffset + x + 2 * y * srcWidthPacked]);\n\
        const ivec2 thirdRow1 = unpack16bitInternals(src.samples[srcOffset + x + 1 + 2 * y * srcWidthPacked]);\n\
\n\
        ivec4 dstRow1;\n\
        dstRow1.x = clamp_s15(ds + kernel[3] * firstRow1.x + kernel[2] * firstRow1.y + kernel[1] * secondRow1.x + kernel[0] * secondRow1.y);\n\
        dstRow1.y = clamp_s15(ds + kernel[0] * firstRow1.y + kernel[1] * secondRow1.x + kernel[2] * secondRow1.y + kernel[3] * thirdRow1.x);\n\
        dstRow1.z = clamp_s15(ds + kernel[3] * firstRow1.y + kernel[2] * secondRow1.x + kernel[1] * secondRow1.y + kernel[0] * thirdRow1.x);\n\
        dstRow1.w = clamp_s15(ds + kernel[0] * secondRow1.x + kernel[1] * secondRow1.y + kernel[2] * thirdRow1.x + kernel[3] * thirdRow1.y);\n\
\n\
        // 2nd row\n\
        const ivec2 firstRow2 = unpack16bitInternals(src.samples[srcOffset + x - 1 + (2 * y + 1) * srcWidthPacked]);\n\
        const ivec2 secondRow2 = unpack16bitInternals(src.samples[srcOffset + x + (2 * y + 1) * srcWidthPacked]);\n\
        const ivec2 thirdRow2 = unpack16bitInternals(src.samples[srcOffset + x + 1 + (2 * y + 1) * srcWidthPacked]);\n\
\n\
        ivec4 dstRow2;\n\
        dstRow2.x = clamp_s15(ds + kernel[3] * firstRow2.x + kernel[2] * firstRow2.y + kernel[1] * secondRow2.x + kernel[0] * secondRow2.y);\n\
        dstRow2.y = clamp_s15(ds + kernel[0] * firstRow2.y + kernel[1] * secondRow2.x + kernel[2] * secondRow2.y + kernel[3] * thirdRow2.x);\n\
        dstRow2.z = clamp_s15(ds + kernel[3] * firstRow2.y + kernel[2] * secondRow2.x + kernel[1] * secondRow2.y + kernel[0] * thirdRow2.x);\n\
        dstRow2.w = clamp_s15(ds + kernel[0] * secondRow2.x + kernel[1] * secondRow2.y + kernel[2] * thirdRow2.x + kernel[3] * thirdRow2.y);\n\
\n\
        // PA\n\
        int adjustA = 0;\n\
        int adjustB = 0;\n\
        if (pa == 1) {\n\
            const ivec2 basePels = unpack16bitInternals(base.samples[baseOffset + x + y * srcWidthPacked]);\n\
            adjustA = basePels.x - ((dstRow1.x + dstRow1.y + dstRow2.x + dstRow2.y + 2) >> 2);\n\
            adjustB = basePels.y - ((dstRow1.z + dstRow1.w + dstRow2.z + dstRow2.w + 2) >> 2);\n\
        }\n\
        dst.samples[dstOffset + 2 * x + dstWidthPacked * 2 * y] = pack16bitInternals(dstRow1.xy + adjustA);\n\
        dst.samples[dstOffset + 2 * x + 1 + dstWidthPacked * 2 * y] = pack16bitInternals(dstRow1.zw + adjustB);\n\
        dst.samples[dstOffset + 2 * x + dstWidthPacked * (2 * y + 1)] = pack16bitInternals(dstRow2.xy + adjustA);\n\
        dst.samples[dstOffset + 2 * x + 1 + dstWidthPacked * (2 * y + 1)] = pack16bitInternals(dstRow2.zw + adjustB);\n\
    }\n\
}";

const unsigned char *apply_spv = "\
// Vulkan Apply shader\n\
\n\
#version 450\n\
\n\
layout(local_size_x_id = 0, local_size_y_id = 1, local_size_z_id = 2) in;\n\
\n\
layout(std430, set = 0, binding = 0) buffer restrict readonly CommandBuffer {\n\
    uint samples[];\n\
} cmd;\n\
\n\
layout(std430, set = 0, binding = 1) buffer restrict writeonly PlaneBuffer {\n\
    uint samples[];\n\
} dst;\n\
\n\
layout(push_constant) uniform consts {\n\
    int srcWidth;\n\
    int srcHeight;\n\
    int residualOffset;\n\
    int stride;\n\
    int saturate;\n\
    int testVal;\n\
    int layerCount;\n\
};\n\
\n\
const uint ADD = 0;\n\
const uint SET = 1;\n\
const uint SETZERO = 2;\n\
const uint CLEARSET = 3;\n\
const int SATURATION = 32767;\n\
const uint bit8 = 1;\n\
\n\
uint applyCounter = 0;\n\
\n\
#include \"common.glsl\"\n\
\n\
void applyTu(uint firstPixelinTu, uint firstResidual, uint operation, uint tuWidth)\n\
{\n\
    for (uint j = 0; j < tuWidth; ++j) {\n\
        for (uint i = 0; i < tuWidth / 2; ++i) {\n\
            const uint pixelPairIndex = firstPixelinTu + i + j * stride / 2;\n\
            const uint residualPairIndex = residualOffset / 4 + firstResidual / 2 +\n\
                                           (applyCounter * tuWidth * tuWidth / 2) + i + j * tuWidth / 2;\n\
\n\
            if (pixelPairIndex >= srcWidth * srcHeight / 2) {\n\
                return;\n\
            }\n\
\n\
            ivec2 pixelPair;\n\
            if (operation == ADD) {\n\
                if (saturate == 1) {\n\
                    pixelPair.x = SATURATION;\n\
                    pixelPair.y = SATURATION;\n\
                } else {\n\
                    pixelPair = unpack16bitInternals(dst.samples[pixelPairIndex]);\n\
                    const ivec2 residualPair = unpack16bitInternals(cmd.samples[residualPairIndex]);\n\
                    pixelPair.x += residualPair.x;\n\
                    pixelPair.y += residualPair.y;\n\
                }\n\
            } else if (operation == SET) {\n\
                if (saturate == 1) {\n\
                    pixelPair.x = SATURATION;\n\
                    pixelPair.y = SATURATION;\n\
                } else {\n\
                    const ivec2 residualPair = unpack16bitInternals(cmd.samples[residualPairIndex]);\n\
                    pixelPair.x = residualPair.x;\n\
                    pixelPair.y = residualPair.y;\n\
                }\n\
            } else if (operation == SETZERO) {\n\
                pixelPair.x = 0;\n\
                pixelPair.y = 0;\n\
            }\n\
            dst.samples[pixelPairIndex] = pack16bitInternals(pixelPair);\n\
        }\n\
    }\n\
    if (operation != SETZERO) {\n\
        applyCounter++;\n\
    }\n\
}\n\
\n\
void process64Bitmask(uint bitmaskMost, uint bitmaskLeast, uint operation,\n\
                      uint startingPixelInBLock, uint tu, uint tuWidth, uint dds)\n\
{\n\
    for (uint i = 0; i < 64; ++i) {\n\
        uint bitSelector;\n\
        uint willApply;\n\
        if (i < 32) {\n\
            bitSelector = 1 << (31 - i);\n\
            willApply = bitmaskMost & bitSelector;\n\
        } else {\n\
            bitSelector = 1 << (63 - i);\n\
            willApply = bitmaskLeast & bitSelector;\n\
        }\n\
\n\
        const uint tusInRow = (dds == 1) ? 8 : 16;\n\
        const uint tuX = i % tusInRow;\n\
        const uint tuY = i / tusInRow;\n\
\n\
        const uint index = startingPixelInBLock + tuX * 2 + tuY * tuWidth * stride / 2;\n\
\n\
        if (willApply > 0) {\n\
            if (operation == ADD) {\n\
                applyTu(index, tu, ADD, tuWidth);\n\
            } else if (operation == SET) {\n\
                applyTu(index, tu, SET, tuWidth);\n\
            } else if (operation == SETZERO) {\n\
                applyTu(index, tu, SETZERO, tuWidth);\n\
            } else { // CLEARSET\n\
                applyTu(index, tu, SET, tuWidth);\n\
            }\n\
        } else {\n\
            if (operation == CLEARSET) {\n\
                applyTu(index, 0, SETZERO, tuWidth);\n\
            }\n\
        }\n\
    }\n\
}\n\
\n\
void main()\n\
{\n\
    const uint CMDSIZE =\n\
        10; // 4 without DD. The 3 extra (wasted) uint64_t are still part of the command stream in DDS.\n\
    const uint tuWidth = (layerCount == 16) ? 4 : 2;\n\
    const uint x = gl_GlobalInvocationID.x;\n\
    const uint commandIndex = x * CMDSIZE;\n\
    ivec4 command = ivec4(cmd.samples[commandIndex], cmd.samples[commandIndex + 1],\n\
                          cmd.samples[commandIndex + 2], cmd.samples[commandIndex + 3]);\n\
\n\
    uint operation = command.x & 0x3;        // One of GpuApplyOperation.\n\
    uint block = (command.x >> 2) & 0x3FFFF; // Number of block within temporal buffer to operate on.\n\
    uint tu = ((command.x >> 20) & 0xFFF) |\n\
              ((command.y & 0x7FFF) << 12); // Offset into CommandBuffer of first residual for operation.\n\
    uint bitStart = (command.y >> 15) & 0xFF;  // First set bit in bitmask.\n\
    uint bitCount = (command.y >> 23) & 0x1FF; // Number of set bits in bitmask.\n\
    uint bitmaskLeast = command.z;\n\
    uint bitmaskMost = command.w;\n\
\n\
    // DD\n\
    uint bitmask2Least = 0;\n\
    uint bitmask2Most = 0;\n\
    uint bitmask3Least = 0;\n\
    uint bitmask3Most = 0;\n\
    uint bitmask4Least = 0;\n\
    uint bitmask4Most = 0;\n\
    if (layerCount == 4) {\n\
        bitmask2Least = cmd.samples[commandIndex + 4];\n\
        bitmask2Most = cmd.samples[commandIndex + 5];\n\
        bitmask3Least = cmd.samples[commandIndex + 6];\n\
        bitmask3Most = cmd.samples[commandIndex + 7];\n\
        bitmask4Least = cmd.samples[commandIndex + 8];\n\
        bitmask4Most = cmd.samples[commandIndex + 9];\n\
    }\n\
\n\
    const uint blocksInRow = (srcWidth % 32 == 0) ? (srcWidth / 32) : (srcWidth / 32) + 1;\n\
    const uint blocksInColumn = (srcHeight % 32 == 0) ? (srcHeight / (2*32)) : (srcHeight / (2*32)) + 1;\n\
    const uint blockX = block % blocksInRow;\n\
    const uint blockY = block / blocksInRow;\n\
    const uint firstPixelInBLock = blockX * 16 + blockY * (stride / 2) * 32;\n\
\n\
    if (layerCount == 16) { // DDS\n\
        process64Bitmask(bitmaskMost, bitmaskLeast, operation, firstPixelInBLock, tu, tuWidth, 1);\n\
    } else { // DD\n\
        process64Bitmask(bitmaskMost, bitmaskLeast, operation, firstPixelInBLock, tu, tuWidth, 0);\n\
        process64Bitmask(bitmask2Most, bitmask2Least, operation, firstPixelInBLock + 2 * stride, tu, tuWidth, 0);\n\
        process64Bitmask(bitmask3Most, bitmask3Least, operation, firstPixelInBLock + 2 * 2 * stride, tu, tuWidth, 0);\n\
        process64Bitmask(bitmask4Most, bitmask4Least, operation, firstPixelInBLock + 3 * 2 * stride, tu, tuWidth, 0);\n\
    }\n\
}";

const unsigned char *conversion_spv = "\
// Vulkan Vertical format conversion shader\n\
\n\
#version 450\n\
\n\
layout(local_size_x_id = 0, local_size_y_id = 1, local_size_z_id = 2) in;\n\
\n\
layout(std430, set = 0, binding = 0) buffer restrict readonly SrcImageBuffer {\n\
    uint samples[];\n\
} src;\n\
\n\
layout(std430, set = 0, binding = 1) buffer restrict writeonly DstImageBuffer {\n\
    uint samples[];\n\
} dst;\n\
\n\
layout(push_constant) uniform consts {\n\
    int numPixels; // The amount of pixels in the src buffer\n\
    int bit8;\n\
    int toInternal;\n\
    int nv12;      // pixel pointer to the start of the UV plane\n\
    int uvFlipped; // bool to signal NV21\n\
    int batchSize; // number of elements to process in each shader invocation\n\
};\n\
\n\
#include \"common.glsl\"\n\
\n\
void main()\n\
{\n\
    const uint x = gl_GlobalInvocationID.x;\n\
    const uint y = gl_GlobalInvocationID.y;\n\
    const uint index = x + gl_NumWorkGroups.x * gl_WorkGroupSize.x * y;\n\
    const uint packDensity = (bit8 == 1) ? 4 : 2;\n\
    const uint numPixelPacks = numPixels / packDensity;\n\
    const uint numBatches = numPixelPacks / batchSize;\n\
\n\
    if (toInternal == 1) {\n\
        if (bit8 == 1) {\n\
            if (nv12 != 0) {\n\
                const uint jumpU = nv12 / (2 * 2);\n\
                const uint jumpV = jumpU / 2;\n\
                for (int i = 0; i < batchSize; ++i) {\n\
                    const uint pixelsDone = (i + x * batchSize) * packDensity;\n\
                    if (pixelsDone >= numPixels) {\n\
                        return;\n\
                    }\n\
                    const ivec4 pixels =\n\
                        toInternalFrom8bit(unpack8bit(src.samples[batchSize * index + i]));\n\
                    if (pixelsDone < nv12) {\n\
                        dst.samples[batchSize * 2 * index + 2 * i] = pack16bitInternals(pixels.xy);\n\
                        dst.samples[batchSize * 2 * index + 2 * i + 1] = pack16bitInternals(pixels.zw);\n\
                    } else {\n\
                        dst.samples[jumpU + batchSize * index + i] = pack16bitInternals(ivec2(pixels.x, pixels.z));\n\
                        dst.samples[jumpU + jumpV + batchSize * index + i] = pack16bitInternals(ivec2(pixels.y, pixels.w));\n\
                    }\n\
                }\n\
            } else {\n\
                for (int i = 0; i < batchSize; ++i) {\n\
                    const uint pixelsDone = (i + x * batchSize) * packDensity;\n\
                    if (pixelsDone >= numPixels) {\n\
                        return;\n\
                    }\n\
                    const ivec4 pixels = toInternalFrom8bit(unpack8bit(src.samples[batchSize * index + i]));\n\
                    dst.samples[batchSize * 2 * index + 2 * i] = pack16bitInternals(pixels.xy);\n\
                    dst.samples[batchSize * 2 * index + 2 * i + 1] = pack16bitInternals(pixels.zw);\n\
                }\n\
            }\n\
        } else {\n\
            for (int i = 0; i < batchSize; ++i) {\n\
                const uint pixelsDone = (i + x * batchSize) * packDensity;\n\
                if (pixelsDone >= numPixels) {\n\
                    return;\n\
                }\n\
                const ivec2 pixels = toInternalFrom16bit(unpack16bit(src.samples[batchSize * index + i]));\n\
                dst.samples[batchSize * index + i] = pack16bitInternals(pixels);\n\
            }\n\
        }\n\
    } else {\n\
        if (bit8 == 1) {\n\
            if (nv12 != 0) {\n\
                const uint jumpU = nv12 / (2 * 2);\n\
                const uint jumpV = jumpU / 2;\n\
                for (int i = 0; i < batchSize; ++i) {\n\
                    const uint pixelsDone = (i + x * batchSize) * packDensity;\n\
                    if (pixelsDone >= numPixels) {\n\
                        return;\n\
                    }\n\
                    if (pixelsDone < nv12) {\n\
                        const ivec2 pixels1 = unpack16bitInternals(src.samples[batchSize * 2 * index + 2 * i]);\n\
                        const ivec2 pixels2 = unpack16bitInternals(src.samples[batchSize * 2 * index + 2 * i + 1]);\n\
                        dst.samples[batchSize * index + i] = pack8bit(\n\
                            fromInternalTo8bit(ivec4(pixels1.x, pixels1.y, pixels2.x, pixels2.y)));\n\
                    } else {\n\
                        const ivec2 pixels1 = unpack16bitInternals(src.samples[jumpU + batchSize * index + i]);\n\
                        const ivec2 pixels2 = unpack16bitInternals(src.samples[jumpU + jumpV + batchSize * index + i]);\n\
                        dst.samples[batchSize * index + i] = pack8bit(\n\
                            fromInternalTo8bit(ivec4(pixels1.x, pixels2.x, pixels1.y, pixels2.y)));\n\
                    }\n\
                }\n\
            } else {\n\
                for (int i = 0; i < batchSize; ++i) {\n\
                    const uint pixelsDone = (i + x * batchSize) * packDensity;\n\
                    if (pixelsDone >= numPixels) {\n\
                        return;\n\
                    }\n\
                    const ivec2 pixels1 = unpack16bitInternals(src.samples[batchSize * 2 * index + 2 * i]);\n\
                    const ivec2 pixels2 = unpack16bitInternals(src.samples[batchSize * 2 * index + 2 * i + 1]);\n\
                    dst.samples[batchSize * index + i] = pack8bit(fromInternalTo8bit(ivec4(pixels1.x, pixels1.y, pixels2.x, pixels2.y)));\n\
                }\n\
            }\n\
        } else {\n\
            for (int i = 0; i < batchSize; ++i) {\n\
                const uint pixelsDone = (i + x * batchSize) * packDensity;\n\
                if (pixelsDone >= numPixels) {\n\
                    return;\n\
                }\n\
\n\
                const ivec2 pixels = unpack16bitInternals(src.samples[batchSize * index + i]);\n\
                dst.samples[batchSize * index + i] = pack16bit(fromInternalTo16bit(pixels));\n\
            }\n\
        }\n\
    }\n\
}";

const unsigned char *blit_spv = "\
// Vulkan blit shader\n\
\n\
#version 450\n\
\n\
layout(local_size_x_id = 0, local_size_y_id = 1, local_size_z_id = 2) in;\n\
\n\
layout(std430, set = 0, binding = 0) buffer restrict readonly SrcImageBuffer {\n\
    uint samples[];\n\
} src;\n\
\n\
layout(std430, set = 0, binding = 1) buffer restrict writeonly DstImageBuffer {\n\
    uint samples[];\n\
} dst;\n\
\n\
layout(push_constant) uniform consts {\n\
    int width;\n\
    int height;\n\
    int batchSize; // number of elements to process in each shader invocation\n\
};\n\
\n\
#include \"common.glsl\"\n\
\n\
void main()\n\
{\n\
    const uint x = gl_GlobalInvocationID.x;\n\
    const uint y = gl_GlobalInvocationID.y;\n\
    const uint index = x + (width / 2) * y;\n\
    if (index < ((width / 2) * height)) {\n\
        const ivec2 srcPixels = unpack16bitInternals(src.samples[index]);\n\
        ivec2 dstPixels = unpack16bitInternals(dst.samples[index]);\n\
        ivec2 sum;\n\
        sum.x = saturateS16(dstPixels.x + srcPixels.x);\n\
        sum.y = saturateS16(dstPixels.y + srcPixels.y);\n\
        dst.samples[index] = pack16bitInternals(sum);\n\
    }\n\
}";
