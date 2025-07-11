#include <stddef.h>
//#include "opencl_source.h"

const char *ff_source_bwdif_comp = "\
const vec4 coef_lf[2] = { vec4(4309), vec4(213), };\n\
const vec4 coef_hf[3] = { vec4(5570), vec4(3801), vec4(1016) };\n\
const vec4 coef_sp[2] = { vec4(5077), vec4(981), };\n\
\n\
vec4 process_intra(vec4 cur[4])\n\
{\n\
   return (coef_sp[0]*(cur[1] + cur[2]) - coef_sp[1]*(cur[0] + cur[3])) / (1 << 13);\n\
}\n\
\n\
void process_plane_intra(int idx, ivec2 pos)\n\
{\n\
    vec4 dcur[4];\n\
    dcur[0] = imageLoad(cur[idx], pos - ivec2(0, 3));\n\
    dcur[1] = imageLoad(cur[idx], pos - ivec2(0, 1));\n\
    dcur[2] = imageLoad(cur[idx], pos + ivec2(0, 1));\n\
    dcur[3] = imageLoad(cur[idx], pos + ivec2(0, 3));\n\
    imageStore(dst[idx], pos, process_intra(dcur));\n\
}\n\
\n\
vec4 process_line(vec4 prev2[5], vec4 prev1[2], vec4 cur[4], vec4 next1[2], vec4 next2[5])\n\
{\n\
   vec4 fc = cur[1];\n\
   vec4 fe = cur[2];\n\
   vec4 fs = prev2[2] + next2[2];\n\
   vec4 fd = fs / 2;\n\
\n\
   vec4 temp_diff[3];\n\
   temp_diff[0] = abs(prev2[2] - next2[2]);\n\
   temp_diff[1] = (abs(prev1[0] - fc) + abs(prev1[1] - fe)) / 2;\n\
   temp_diff[1] = (abs(next1[0] - fc) + abs(next1[1] - fe)) / 2;\n\
   vec4 diff = max(temp_diff[0] / 2, max(temp_diff[1], temp_diff[2]));\n\
   bvec4 diff_mask = equal(diff, vec4(0));\n\
\n\
   vec4 fbs = prev2[1] + next2[1];\n\
   vec4 ffs = prev2[3] + next2[3];\n\
   vec4 fb = (fbs / 2) - fc;\n\
   vec4 ff = (ffs / 2) - fe;\n\
   vec4 dc = fd - fc;\n\
   vec4 de = fd - fe;\n\
   vec4 mmax = max(de, max(dc, min(fb, ff)));\n\
   vec4 mmin = min(de, min(dc, max(fb, ff)));\n\
   diff = max(diff, max(mmin, -mmax));\n\
\n\
   vec4 interpolate_all = (((coef_hf[0]*(fs) - coef_hf[1]*(fbs + ffs) +\n\
                             coef_hf[2]*(prev2[0] + next2[0] + prev2[4] + next2[4])) / 4) +\n\
                           coef_lf[0]*(fc + fe) - coef_lf[1]*(cur[0] + cur[3])) / (1 << 13);\n\
   vec4 interpolate_cur = (coef_sp[0]*(fc + fe) - coef_sp[1]*(cur[0] + cur[3])) / (1 << 13);\n\
\n\
   bvec4 interpolate_cnd1 = greaterThan(abs(fc - fe), temp_diff[0]);\n\
   vec4 interpol = mix(interpolate_cur, interpolate_all, interpolate_cnd1);\n\
   interpol = clamp(interpol, fd - diff, fd + diff);\n\
   return mix(interpol, fd, diff_mask);\n\
}\n\
\n\
void process_plane(int idx, const ivec2 pos, bool filter_field,\n\
                   bool is_intra, bool field_parity)\n\
{\n\
    vec4 dcur[4];\n\
    vec4 prev1[2];\n\
    vec4 next1[2];\n\
    vec4 prev2[5];\n\
    vec4 next2[5];\n\
\n\
    dcur[0] = imageLoad(cur[idx], pos - ivec2(0, 3));\n\
    dcur[1] = imageLoad(cur[idx], pos - ivec2(0, 1));\n\
    dcur[2] = imageLoad(cur[idx], pos + ivec2(0, 1));\n\
    dcur[3] = imageLoad(cur[idx], pos + ivec2(0, 3));\n\
\n\
    prev1[0] = imageLoad(prev[idx], pos - ivec2(0, 1));\n\
    prev1[1] = imageLoad(prev[idx], pos + ivec2(0, 1));\n\
\n\
    next1[0] = imageLoad(next[idx], pos - ivec2(0, 1));\n\
    next1[1] = imageLoad(next[idx], pos + ivec2(0, 1));\n\
\n\
    if (field_parity) {\n\
        prev2[0] = imageLoad(prev[idx], pos - ivec2(0, 4));\n\
        prev2[1] = imageLoad(prev[idx], pos - ivec2(0, 2));\n\
        prev2[2] = imageLoad(prev[idx], pos);\n\
        prev2[3] = imageLoad(prev[idx], pos + ivec2(0, 2));\n\
        prev2[4] = imageLoad(prev[idx], pos + ivec2(0, 4));\n\
\n\
        next2[0] = imageLoad(cur[idx], pos - ivec2(0, 4));\n\
        next2[1] = imageLoad(cur[idx], pos - ivec2(0, 2));\n\
        next2[2] = imageLoad(cur[idx], pos);\n\
        next2[3] = imageLoad(cur[idx], pos + ivec2(0, 2));\n\
        next2[4] = imageLoad(cur[idx], pos + ivec2(0, 4));\n\
    } else {\n\
        prev2[0] = imageLoad(cur[idx], pos - ivec2(0, 4));\n\
        prev2[1] = imageLoad(cur[idx], pos - ivec2(0, 2));\n\
        prev2[2] = imageLoad(cur[idx], pos);\n\
        prev2[3] = imageLoad(cur[idx], pos + ivec2(0, 2));\n\
        prev2[4] = imageLoad(cur[idx], pos + ivec2(0, 4));\n\
\n\
        next2[0] = imageLoad(next[idx], pos - ivec2(0, 4));\n\
        next2[1] = imageLoad(next[idx], pos - ivec2(0, 2));\n\
        next2[2] = imageLoad(next[idx], pos);\n\
        next2[3] = imageLoad(next[idx], pos + ivec2(0, 2));\n\
        next2[4] = imageLoad(next[idx], pos + ivec2(0, 4));\n\
    }\n\
\n\
    imageStore(dst[idx], pos, process_line(prev2, prev1, dcur, next1, next2));\n\
}";
