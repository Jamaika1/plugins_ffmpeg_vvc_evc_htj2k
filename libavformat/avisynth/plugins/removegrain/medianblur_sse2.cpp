//#ifdef INTEL_INTRINSICS
#include "../../avisynth.h"
#include "medianblur_sse2.h"

#define MEDIANPROCESSOR_SSE2
#include "medianblur.hpp"
#undef MEDIANPROCESSOR_SSE2

// instantiate
template void MedianProcessor_sse2<uint8_t, 8, InstructionSet::SSE2>::calculate_median<uint8_t, 8, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint8_t, 10, InstructionSet::SSE2>::calculate_median<uint16_t, 10, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint8_t, 12, InstructionSet::SSE2>::calculate_median<uint16_t, 12, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint8_t, 14, InstructionSet::SSE2>::calculate_median<uint16_t, 14, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint8_t, 16, InstructionSet::SSE2>::calculate_median<uint16_t, 16, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint8_t, 16, InstructionSet::SSE2>::calculate_median<float, 16, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint8_t, 16, InstructionSet::SSE2>::calculate_median<float, 16, true>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);

template void MedianProcessor_sse2<uint16_t, 8, InstructionSet::SSE2>::calculate_median<uint8_t, 8, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint16_t, 10, InstructionSet::SSE2>::calculate_median<uint16_t, 10, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint16_t, 12, InstructionSet::SSE2>::calculate_median<uint16_t, 12, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint16_t, 14, InstructionSet::SSE2>::calculate_median<uint16_t, 14, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint16_t, 16, InstructionSet::SSE2>::calculate_median<uint16_t, 16, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint16_t, 16, InstructionSet::SSE2>::calculate_median<float, 16, false>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<uint16_t, 16, InstructionSet::SSE2>::calculate_median<float, 16, true>(uint8_t* dstp, const uint8_t* srcp, int dst_pitch, int src_pitch, int width, int height, int radius, void* buffer);

template void MedianProcessor_sse2<int32_t, 8, InstructionSet::SSE2>::calculate_temporal_median<uint8_t, 8, false>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<int32_t, 10, InstructionSet::SSE2>::calculate_temporal_median<uint16_t, 10, false>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<int32_t, 12, InstructionSet::SSE2>::calculate_temporal_median<uint16_t, 12, false>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<int32_t, 14, InstructionSet::SSE2>::calculate_temporal_median<uint16_t, 14, false>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<int32_t, 16, InstructionSet::SSE2>::calculate_temporal_median<uint16_t, 16, false>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<int32_t, 16, InstructionSet::SSE2>::calculate_temporal_median<float, 16, false>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);
template void MedianProcessor_sse2<int32_t, 16, InstructionSet::SSE2>::calculate_temporal_median<float, 16, true>(uint8_t * dstp, int dst_pitch, const uint8_t * *src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);

template
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void calculate_temporal_median_sr0_tr1_sse4<uint8_t>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);

template
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void calculate_temporal_median_sr0_tr1_sse4<uint16_t>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);

template
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void calculate_temporal_median_sr0_tr1_sse4<float>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);

template
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void calculate_temporal_median_sr0_tr2_sse4<uint8_t>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);

template
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void calculate_temporal_median_sr0_tr2_sse4<uint16_t>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);

template
#if defined(GCC) || defined(CLANG)
__attribute__((__target__("sse4.1")))
#endif
void calculate_temporal_median_sr0_tr2_sse4<float>(uint8_t* dstp, int dst_pitch, const uint8_t** src_ptrs, const int* src_pitches, int frames_count, int width, int height, int radius, void* buffer);

//#endif // INTEL_INTRINSICS
