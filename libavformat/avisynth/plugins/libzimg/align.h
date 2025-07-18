#pragma once

#ifndef ZIMG_ALIGN_H_
#define ZIMG_ALIGN_H_

namespace zimg {

/**
 * 64-byte alignment allows the use of instructions up to AVX-512.
 */
#if defined(ZIMG_X86)
#if defined(__AVX512F__)
constexpr int ALIGNMENT = 64;
constexpr int ALIGNMENT_RELAXED = 64;
#elif defined(__AVX__)
constexpr int ALIGNMENT = 32;
constexpr int ALIGNMENT_RELAXED = 32;
#endif
#elif defined(ZIMG_ARM)
constexpr int ALIGNMENT = 16;
constexpr int ALIGNMENT_RELAXED = 16;
#else
constexpr int ALIGNMENT = alignof(long double);
constexpr int ALIGNMENT_RELAXED = alignof(long double);
#endif

/**
 * Round up the argument x to the nearest multiple of n.
 * x must be non-negative and n must be positive and power-of-2.
 */
template <class T>
constexpr T ceil_n(T x, unsigned n) { return (x + (n - 1)) & ~static_cast<T>(n - 1); }

/**
 * Round down the argument x to the nearest multiple of n.
 * x must be non-negative and n must be positive and power-of-2.
 */
template <class T>
constexpr T floor_n(T x, unsigned n) { return x & ~static_cast<T>(n - 1); }

/**
 * Alignment in units of object count.
 *
 * @tparam T type of object
 */
template <class T>
constexpr unsigned AlignmentOf = ALIGNMENT >= sizeof(T) ? ALIGNMENT / sizeof(T) : 1;

} // namespace zimg

#endif // ZIMG_ALIGN_H_
