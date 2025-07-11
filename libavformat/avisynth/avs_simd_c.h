// AviSynth+.  Copyright 2025 AviSynth+ Project
// https://avs-plus.net
// http://avisynth.nl
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
// http://www.gnu.org/copyleft/gpl.html .
//
// Linking Avisynth statically or dynamically with other modules is making a
// combined work based on Avisynth.  Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Avisynth give you
// permission to link Avisynth with independent modules that communicate with
// Avisynth solely through the interfaces defined in avisynth.h, regardless of the license
// terms of these independent modules, and to copy and distribute the
// resulting combined work under terms of your choice, provided that
// every copy of the combined work is accompanied by a complete copy of
// the source code of Avisynth (the version of Avisynth used to produce the
// combined work), being distributed under the terms of the GNU General
// Public License plus this exception.  An independent module is a module
// which is not derived from or based on Avisynth, such as 3rd-party filters,
// import and export plugins, or graphical user interfaces.

// SIMD-like C++ classes (C)2025 Ferenc Pintér

#ifndef __AVS_SIMD_C_H__
#define __AVS_SIMD_C_H__
// Auto-vectorization friendly types for smart compilers
// Some helper static functions, marked with c++ 17 [[maybe_unused]] attribute

#include "avs/config.h" // force inline variants
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <array>
#include <stdexcept>

// Determine if we can use vector attribute

// As of 2025 clang (llvm) supports vector attributes and also produces fast code
// gcc can use it but may produce slower code with than without vector attributes.
// (gcc 12.2 on aarch64 Raspberry Pi 5 is slow)
// We still keep it here, maybe non-arm platforms can benefit with gcc as well.
// Note: llvm defines __clang__ as well
#if defined(__GNUC__) || defined(__clang__)
#define HAS_VECTOR_ATTRIBUTE 1
#else
#define HAS_VECTOR_ATTRIBUTE 0
#endif

// Vector type definitions
#if HAS_VECTOR_ATTRIBUTE
    // GCC/Clang supports vector attribute
using uint8_vec32_t = uint8_t __attribute__((vector_size(32)));  // 256-bit (32 bytes)
using uint8_vec16_t = uint8_t __attribute__((vector_size(16)));  // 128-bit (16 bytes)
using uint8_vec8_t  = uint8_t __attribute__((vector_size(8)));   // 64-bit  (8 bytes)
using uint8_vec4_t  = uint8_t __attribute__((vector_size(4)));   // 32-bit  (4 bytes)

using int16_vec16_t = int16_t __attribute__((vector_size(32)));  // 256-bit (32 bytes)
using int16_vec8_t  = int16_t __attribute__((vector_size(16)));  // 128-bit (16 bytes)
using int16_vec4_t  = int16_t __attribute__((vector_size(8)));   // 64-bit  (8 bytes)

using uint16_vec16_t = uint16_t __attribute__((vector_size(32))); // 256-bit (32 bytes)
using uint16_vec8_t  = uint16_t __attribute__((vector_size(16))); // 128-bit (16 bytes)
using uint16_vec4_t  = uint16_t __attribute__((vector_size(8)));  // 64-bit  (8 bytes)

using int32_vec8_t  = int32_t __attribute__((vector_size(32)));  // 256-bit (32 bytes)
using int32_vec4_t  = int32_t __attribute__((vector_size(16)));  // 128-bit (16 bytes)
using int32_vec2_t  = int32_t __attribute__((vector_size(8)));   // 64-bit  (8 bytes)

using float_vec8_t  = float __attribute__((vector_size(32)));    // 256-bit (32 bytes)
using float_vec4_t  = float __attribute__((vector_size(16)));    // 128-bit (16 bytes)
using float_vec2_t  = float __attribute__((vector_size(8)));     // 64-bit  (8 bytes)

#else
    // std::array fallback for e.g. MSVC
    // std::array is a lightweight wrapper around C-style arrays, and has no overhead.
using uint8_vec32_t = std::array<uint8_t, 32>;  // 256-bit (32 bytes)
using uint8_vec16_t = std::array<uint8_t, 16>;  // 128-bit (16 bytes)
using uint8_vec8_t  = std::array<uint8_t, 8>;   // 64-bit  (8 bytes)
using uint8_vec4_t  = std::array<uint8_t, 4>;   // 32-bit  (4 bytes)

using int16_vec16_t = std::array<int16_t, 16>;  // 256-bit (32 bytes)
using int16_vec8_t  = std::array<int16_t, 8>;   // 128-bit (16 bytes)
using int16_vec4_t  = std::array<int16_t, 4>;   // 64-bit  (8 bytes)

using uint16_vec16_t = std::array<uint16_t, 16>; // 256-bit (32 bytes)
using uint16_vec8_t  = std::array<uint16_t, 8>;  // 128-bit (16 bytes)
using uint16_vec4_t  = std::array<uint16_t, 4>;  // 64-bit  (8 bytes)

using int32_vec8_t  = std::array<int32_t, 8>;   // 256-bit (32 bytes)
using int32_vec4_t  = std::array<int32_t, 4>;   // 128-bit (16 bytes)
using int32_vec2_t  = std::array<int32_t, 2>;   // 64-bit  (8 bytes)

using float_vec8_t  = std::array<float, 8>;     // 256-bit (32 bytes)
using float_vec4_t  = std::array<float, 4>;     // 128-bit (16 bytes)
using float_vec2_t  = std::array<float, 2>;     // 64-bit  (8 bytes)

#endif

// Type traits for vector elements
template <typename T>
struct vector_traits {};


#define DEFINE_VECTOR_TRAITS(VecType, ElemType, Size) \
    template <> struct vector_traits<VecType> { \
        using element_type = ElemType; \
        static constexpr size_t size = Size; \
    };

// Covering 8 to 32 bytes long vectors.
// No int64_t or double, they are not relevant in AviSynth
DEFINE_VECTOR_TRAITS(uint8_vec32_t, uint8_t, 32)
DEFINE_VECTOR_TRAITS(uint8_vec16_t, uint8_t, 16)
DEFINE_VECTOR_TRAITS(uint8_vec8_t, uint8_t, 8)
DEFINE_VECTOR_TRAITS(uint8_vec4_t, uint8_t, 4)
DEFINE_VECTOR_TRAITS(int16_vec16_t, int16_t, 16)
DEFINE_VECTOR_TRAITS(int16_vec8_t, int16_t, 8)
DEFINE_VECTOR_TRAITS(int16_vec4_t, int16_t, 4)
DEFINE_VECTOR_TRAITS(uint16_vec16_t, uint16_t, 16)
DEFINE_VECTOR_TRAITS(uint16_vec8_t, uint16_t, 8)
DEFINE_VECTOR_TRAITS(uint16_vec4_t, uint16_t, 4)
DEFINE_VECTOR_TRAITS(int32_vec8_t, int32_t, 8)
DEFINE_VECTOR_TRAITS(int32_vec4_t, int32_t, 4)
DEFINE_VECTOR_TRAITS(int32_vec2_t, int32_t, 2)
DEFINE_VECTOR_TRAITS(float_vec8_t, float, 8)
DEFINE_VECTOR_TRAITS(float_vec4_t, float, 4)
DEFINE_VECTOR_TRAITS(float_vec2_t, float, 2)

// Specialization for array-based vector operations
#if !HAS_VECTOR_ATTRIBUTE
template <typename VecType, typename ScalarType = typename vector_traits<VecType>::element_type>
class VectorOps {
public:
  static AVS_FORCEINLINE VecType add(const VecType& a, const VecType& b) {
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = a[i] + b[i];
    }
    return result;
  }

  static AVS_FORCEINLINE VecType subtract(const VecType& a, const VecType& b) {
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = a[i] - b[i];
    }
    return result;
  }

  static AVS_FORCEINLINE VecType multiply(const VecType& a, const VecType& b) {
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = a[i] * b[i];
    }
    return result;
  }

  static AVS_FORCEINLINE VecType scalar_multiply(const VecType& a, ScalarType scalar) {
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = a[i] * scalar;
    }
    return result;
  }

  static AVS_FORCEINLINE VecType left_shift(const VecType& a, int shift) {
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = a[i] << shift;
    }
    return result;
  }

  static AVS_FORCEINLINE VecType right_shift(const VecType& a, int shift) {
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = a[i] >> shift;
    }
    return result;
  }
};
#endif

// Template for vector class wrapper
// Passing "Derived" allows syntax for assigning a wrapper class to a derived class.
template <typename Derived, typename VecType>
class VectorWrapper {
private:
  using ElementType = typename vector_traits<VecType>::element_type;
  static constexpr size_t N = vector_traits<VecType>::size;
  VecType data;

public:

  // Default constructor
  AVS_FORCEINLINE VectorWrapper() {
#if HAS_VECTOR_ATTRIBUTE
    data = VecType{};  // empty initializer
#else
    data.fill(ElementType{});
#endif
  }

  // Constructor from scalar
  AVS_FORCEINLINE explicit VectorWrapper(ElementType val) {
#if HAS_VECTOR_ATTRIBUTE
    data = VecType{};
    for (size_t i = 0; i < N; ++i) {
      data[i] = val;
    }
#else
    data.fill(val);
#endif
  }

  // Constructor from raw vector type
  AVS_FORCEINLINE explicit VectorWrapper(const VecType& vec) : data(vec) {}

  // Conversion operator, allowing implicit conversion to the derived type.
  // Supporting syntax like: Int32x4 = src * coeff; where the right side is a VectorWrapper<Int32x4, int32_vec4_t>
  AVS_FORCEINLINE operator Derived() const {
    return static_cast<const Derived>(data);
  }

  // Copy constructor
  AVS_FORCEINLINE VectorWrapper(const VectorWrapper& other) : data(other.data) {}

  // Note: There is no need to define assignment operators for move or copy, as the underlying 'data' type
  // (either std::array or vector attribute) already provides support for these operations.

  // Constructors from initializer list, e.g. x = {32768, 0, 0, 0};

  // Constexpr variadic template constructor for compile-time initialization.Thx AI :)
  template <typename... Args,
    typename = std::enable_if_t<sizeof...(Args) + 1 == N>>
    AVS_FORCEINLINE constexpr VectorWrapper(ElementType arg, Args... args) {
    static_assert(sizeof...(args) + 1 == N, "Initializer list must have exactly N elements.");
    ElementType values[] = { arg, static_cast<ElementType>(args)... };
    for (size_t i = 0; i < N; ++i) {
      data[i] = values[i];
    }
  }

  // std::initializer_list constructor for runtime initialization
  AVS_FORCEINLINE VectorWrapper(std::initializer_list<ElementType> values) {
    if (values.size() != N) {
      throw std::logic_error("Initializer list must have exactly N elements");
    }
    size_t i = 0;
    for (auto val : values) {
      data[i++] = val;
    }
  }

  // Load only the lower half of the vector (e.g., 8 bytes for a 16-byte vector)
  AVS_FORCEINLINE void load_lo(const ElementType* ptr) {
#if HAS_VECTOR_ATTRIBUTE
    for (size_t i = 0; i < N / 2; ++i) {
      data[i] = ptr[i];
    }
    for (size_t i = N / 2; i < N; ++i) {
      data[i] = ElementType{}; // Zero out the upper half
    }
#else
    std::copy(ptr, ptr + (N / 2), data.begin());
    std::fill(data.begin() + (N / 2), data.end(), ElementType{});
#endif
  }

  // Conversions from other types as long as their size matches
  // Full and half size

  // Full: Widening or narrowing without range checks, e.g., no saturation when converting down.
  template <typename OtherDerived, typename OtherVecType>
  AVS_FORCEINLINE static VectorWrapper convert_from(const VectorWrapper<OtherDerived, OtherVecType>& other) {
    static_assert(vector_traits<VecType>::size == vector_traits<OtherVecType>::size,
      "Vector sizes must match for convert_from.");
#if HAS_VECTOR_ATTRIBUTE
    VecType result_vec;
    result_vec = __builtin_convertvector(other.raw(), VecType);
    return VectorWrapper(result_vec);
#else
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = static_cast<typename vector_traits<VecType>::element_type>(other[i]);
    }
    return VectorWrapper(result);
#endif

  }

  // Helf: Widening or narrowing the lower half of the vector (e.g., 8 bytes for a 16-byte vector)
  template <typename OtherDerived, typename OtherVecType>
  AVS_FORCEINLINE void convert_from_lo(const VectorWrapper<OtherDerived, OtherVecType>& other) {
    static_assert(vector_traits<VecType>::size == vector_traits<OtherVecType>::size / 2,
      "Source vector sizes must twice as of source for convert_from_lo.");
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      data[i] = static_cast<typename vector_traits<VecType>::element_type>(other[i]);
    }
  }

  // Helf: Widening or narrowing the upper half of the vector (e.g., 8 bytes for a 16-byte vector)
  template <typename OtherDerived, typename OtherVecType>
  AVS_FORCEINLINE void convert_from_hi(const VectorWrapper<OtherDerived, OtherVecType>& other) {
    static_assert(vector_traits<VecType>::size == vector_traits<OtherVecType>::size / 2,
      "Source vector sizes must twice as of source for convert_from_hi.");
    constexpr auto half_size = vector_traits<OtherVecType>::size / 2;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      data[i] = static_cast<typename vector_traits<VecType>::element_type>(other[half_size + i]);
    }
  }

  // Ffill an integer vector/array from integer pointer source.
  // Versions for uint8_t, int16_t (short), uint16_t and int32_t

  AVS_FORCEINLINE void load_from_any_intptr(const uint8_t* ptr) {
    static_assert(
      std::is_same_v<typename vector_traits<VecType>::element_type, int32_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, int16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint8_t>,
      "load_from_any_intptr is only valid for int32_t, int16_t, uint16_t or uint8_t target element types."
      );

    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      data[i] = static_cast<typename vector_traits<VecType>::element_type>(ptr[i]);
    }
  }

  AVS_FORCEINLINE void load_from_any_intptr(const short* ptr) {
    static_assert(
      std::is_same_v<typename vector_traits<VecType>::element_type, int32_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, int16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint8_t>,
      "load_from_any_intptr is only valid for int32_t, int16_t, uint16_t or uint8_t target element types."
      );

    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      data[i] = static_cast<typename vector_traits<VecType>::element_type>(ptr[i]);
    }
  }

  AVS_FORCEINLINE void load_from_any_intptr(const uint16_t* ptr) {
    static_assert(
      std::is_same_v<typename vector_traits<VecType>::element_type, int32_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, int16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint8_t>,
      "load_from_any_intptr is only valid for int32_t, int16_t, uint16_t or uint8_t target element types."
      );

    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      data[i] = static_cast<typename vector_traits<VecType>::element_type>(ptr[i]);
    }
  }

  AVS_FORCEINLINE void load_from_any_intptr(const int32_t* ptr) {
    static_assert(
      std::is_same_v<typename vector_traits<VecType>::element_type, int32_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, int16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint16_t> ||
      std::is_same_v<typename vector_traits<VecType>::element_type, uint8_t>,
      "load_from_any_intptr is only valid for int32_t, int16_t, uint16_t or uint8_t target element types."
      );

    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      data[i] = static_cast<typename vector_traits<VecType>::element_type>(ptr[i]);
    }
  }

  // Syntax: variable.load(myPointer);
  // Load from pointer (potentially unaligned)
  AVS_FORCEINLINE void load(const ElementType* ptr) {
#if HAS_VECTOR_ATTRIBUTE
    for (size_t i = 0; i < N; ++i) {
      data[i] = ptr[i];
    }
#else
    std::copy(ptr, ptr + N, data.begin());
#endif
  }

  // Syntax: variable = VecType::from_ptr(myPointer);
  // Static factory method
  AVS_FORCEINLINE static VectorWrapper from_ptr(const ElementType* ptr) {
    VectorWrapper result;
    result.load(ptr);  // Reuse the instance method
    return result;
  }

  // Store to pointer (potentially unaligned)
  AVS_FORCEINLINE void store(ElementType* ptr) const {
#if HAS_VECTOR_ATTRIBUTE
    for (size_t i = 0; i < N; ++i) {
      ptr[i] = data[i];
    }
#else
    std::copy(data.begin(), data.end(), ptr);
#endif
  }

  // Addition operator (+)
  AVS_FORCEINLINE VectorWrapper operator+(const VectorWrapper& other) const {
#if HAS_VECTOR_ATTRIBUTE
    return VectorWrapper(data + other.data);
#else
    return VectorWrapper(VectorOps<VecType>::add(data, other.data));
#endif
  }

  // Subtraction operator (-)
  AVS_FORCEINLINE VectorWrapper operator-(const VectorWrapper& other) const {
#if HAS_VECTOR_ATTRIBUTE
    return VectorWrapper(data - other.data);
#else
    return VectorWrapper(VectorOps<VecType>::subtract(data, other.data));
#endif
  }

  // Multiplication operator (*)
  AVS_FORCEINLINE VectorWrapper operator*(const VectorWrapper& other) const {
#if HAS_VECTOR_ATTRIBUTE
    return VectorWrapper(data * other.data);
#else
    return VectorWrapper(VectorOps<VecType>::multiply(data, other.data));
#endif
  }

  // Scalar multiplication (*val)
  AVS_FORCEINLINE VectorWrapper operator*(ElementType scalar) const {
#if HAS_VECTOR_ATTRIBUTE
    return VectorWrapper(data * scalar);
#else
    return VectorWrapper(VectorOps<VecType>::scalar_multiply(data, scalar));
#endif
  }

  // Left shift operator (<<)
  VectorWrapper operator<<(int shift) const {
#if HAS_VECTOR_ATTRIBUTE
    return VectorWrapper(data << shift);
#else
    return VectorWrapper(VectorOps<VecType>::left_shift(data, shift));
#endif
  }

  // Right shift operator (arithmetic) (>>)
  AVS_FORCEINLINE VectorWrapper operator>>(int shift) const {
#if HAS_VECTOR_ATTRIBUTE
    return VectorWrapper(data >> shift);
#else
    return VectorWrapper(VectorOps<VecType>::right_shift(data, shift));
#endif
  }

  // We do not define special operators for logical shift, but provide a function.
  AVS_FORCEINLINE VectorWrapper shift_right_logical(int shift) const {
    // Cast to unsigned type for logical shift
    using UnsignedVecType = std::make_unsigned_t<typename vector_traits<VecType>::element_type>;
    VecType result;
    for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
      result[i] = static_cast<UnsignedVecType>(data[i]) >> shift;
    }
    return VectorWrapper(result);
  }

  // Addition assignment operator (+=)
  AVS_FORCEINLINE VectorWrapper& operator+=(const VectorWrapper& other) {
#if HAS_VECTOR_ATTRIBUTE
    data += other.data;
#else
    data = VectorOps<VecType>::add(data, other.data);
#endif
    return *this;
  }

  // Subtract assignment operator (-=)
  AVS_FORCEINLINE VectorWrapper& operator-=(const VectorWrapper& other) {
#if HAS_VECTOR_ATTRIBUTE
    data -= other.data;
#else
    data = VectorOps<VecType>::sub(data, other.data);
#endif
    return *this;
  }

  // Multiplication assignment operator (*=)
  AVS_FORCEINLINE VectorWrapper& operator*=(const VectorWrapper& other) {
#if HAS_VECTOR_ATTRIBUTE
    data *= other.data;
#else
    data = VectorOps<VecType>::mul(data, other.data);
#endif
    return *this;
  }

  // right shift assignment operator (>>=)
  AVS_FORCEINLINE VectorWrapper& operator>>=(int shift) {
#if HAS_VECTOR_ATTRIBUTE
    data >>= shift;
#else
    data = VectorOps<VecType>::right_shift(data, shift);
#endif
    return *this;
  }

  // left shift assignment operator (<<=)
  AVS_FORCEINLINE VectorWrapper& operator<<=(int shift) {
#if HAS_VECTOR_ATTRIBUTE
    data <<= shift;
#else
    data = VectorOps<VecType>::left_shift(data, shift);
#endif
    return *this;
  }

  // Minimum operation: z = x.min(scalar)
  AVS_FORCEINLINE VectorWrapper min(ElementType scalar) const {
#if HAS_VECTOR_ATTRIBUTE
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = data[i] < scalar ? data[i] : scalar;
    }
    return VectorWrapper(result);
#else
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = std::min(data[i], scalar);
    }
    return VectorWrapper(result);
#endif
  }

  // Maximum operation: z = x.max(scalar)
  AVS_FORCEINLINE VectorWrapper max(ElementType scalar) const {
#if HAS_VECTOR_ATTRIBUTE
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = data[i] > scalar ? data[i] : scalar;
    }
    return VectorWrapper(result);
#else
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = std::max(data[i], scalar);
    }
    return VectorWrapper(result);
#endif
  }

  // Minimum operation (element-wise): z = x.min(y)
  AVS_FORCEINLINE VectorWrapper min(const VectorWrapper& other) const {
#if HAS_VECTOR_ATTRIBUTE
    // Use a ternary operator with a comparison to simulate min operation
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = data[i] < other.data[i] ? data[i] : other.data[i];
    }
    return VectorWrapper(result);
#else
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = std::min(data[i], other.data[i]);
    }
    return VectorWrapper(result);
#endif
  }

  // Maximum operation (element-wise) : z = x.max(y)
  AVS_FORCEINLINE VectorWrapper max(const VectorWrapper& other) const {
#if HAS_VECTOR_ATTRIBUTE
    // Use a ternary operator with a comparison to simulate max operation
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = data[i] > other.data[i] ? data[i] : other.data[i];
    }
    return VectorWrapper(result);
#else
    VecType result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = std::max(data[i], other.data[i]);
    }
    return VectorWrapper(result);
#endif
  }

  // Static min functions similar to SIMD intrinsics: z = min(x, y)
  AVS_FORCEINLINE static VectorWrapper min(const VectorWrapper& a, const VectorWrapper& b) {
    return a.min(b);
  }

  // Static max function similar to SIMD intrinsics: z = max(x, y)
  AVS_FORCEINLINE static VectorWrapper max(const VectorWrapper& a, const VectorWrapper& b) {
    return a.max(b);
  }

  // Accessor for raw data
  AVS_FORCEINLINE const VecType& raw() const { return data; }
  AVS_FORCEINLINE VecType& raw() { return data; }

  // Element access (const and non-const versions)
  AVS_FORCEINLINE ElementType operator[](size_t idx) const {
    return data[idx];
  }

  // a set method instead of non-const operator[]
  // use: x.set(index, value) instead of x[index] = value
  AVS_FORCEINLINE void set(size_t idx, ElementType value) {
    data[idx] = value;
  }

  AVS_FORCEINLINE int32_t horiz_add_int32() {
    int32_t sum = 0;
    for (size_t i = 0; i < N; ++i) {
      sum += data[i];
    }
    return sum;
  }

  AVS_FORCEINLINE float horiz_add_float() {
    float sum = 0;
    for (size_t i = 0; i < N; ++i) {
      sum += static_cast<const float>(data[i]);
    }
    return sum;
  }

  static constexpr size_t size() { return N; }
};

// Scalar multiplication (scalar on left)
template <typename Derived, typename VecType>
AVS_FORCEINLINE VectorWrapper<Derived, VecType> operator*(
  typename vector_traits<VecType>::element_type scalar,
  const VectorWrapper<Derived, VecType>& vec) {
  return vec * scalar;
}

// Stream output operator
template <typename Derived, typename VecType>
std::ostream& operator<<(std::ostream& os, const VectorWrapper<Derived, VecType>& vec) {
  os << "[";
  for (size_t i = 0; i < vector_traits<VecType>::size; ++i) {
    os << vec[i];
    if (i < vector_traits<VecType>::size - 1) os << ", ";
  }
  os << "]";
  return os;
}

// Concrete class definitions for 8, 16 and 32 bytes vectors
class Uint8x32 : public VectorWrapper<Uint8x32, uint8_vec32_t> {
public:
  using Base = VectorWrapper<Uint8x32, uint8_vec32_t>;
  using Base::Base;
  using Base::operator=;
};

class Uint8x16 : public VectorWrapper<Uint8x16, uint8_vec16_t> {
public:
  using Base = VectorWrapper<Uint8x16, uint8_vec16_t>;
  using Base::Base;
  using Base::operator=;
};

class Uint8x8 : public VectorWrapper<Uint8x8, uint8_vec8_t> {
public:
  using Base = VectorWrapper<Uint8x8, uint8_vec8_t>;
  using Base::Base;
  using Base::operator=;
};

class Uint8x4 : public VectorWrapper<Uint8x4, uint8_vec4_t> {
public:
  using Base = VectorWrapper<Uint8x4, uint8_vec4_t>;
  using Base::Base;
  using Base::operator=;
};

class Int16x16 : public VectorWrapper<Int16x16, int16_vec16_t> {
public:
  using Base = VectorWrapper<Int16x16, int16_vec16_t>;
  using Base::Base;
  using Base::operator=;
};

class Int16x8 : public VectorWrapper<Int16x8, int16_vec8_t> {
public:
  using Base = VectorWrapper<Int16x8, int16_vec8_t>;
  using Base::Base;
  using Base::operator=;
};

class Int16x4 : public VectorWrapper<Int16x4, int16_vec4_t> {
public:
  using Base = VectorWrapper<Int16x4, int16_vec4_t>;
  using Base::Base;
  using Base::operator=;
};

class Uint16x16 : public VectorWrapper<Uint16x16, uint16_vec16_t> {
public:
  using Base = VectorWrapper<Uint16x16, uint16_vec16_t>;
  using Base::Base;
  using Base::operator=;
};

class Uint16x8 : public VectorWrapper<Uint16x8, uint16_vec8_t> {
public:
  using Base = VectorWrapper<Uint16x8, uint16_vec8_t>;
  using Base::Base;
  using Base::operator=;
};

class Uint16x4 : public VectorWrapper<Uint16x4, uint16_vec4_t> {
public:
  using Base = VectorWrapper<Uint16x4, uint16_vec4_t>;
  using Base::Base;
  using Base::operator=;
};

class Int32x8 : public VectorWrapper<Int32x8, int32_vec8_t> {
public:
  using Base = VectorWrapper<Int32x8, int32_vec8_t>;
  using Base::Base;
  using Base::operator=;
};

class Int32x4 : public VectorWrapper<Int32x4, int32_vec4_t> {
public:
  using Base = VectorWrapper<Int32x4, int32_vec4_t>;
  using Base::Base;
  using Base::operator=;
};

class Int32x2 : public VectorWrapper<Int32x2, int32_vec2_t> {
public:
  using Base = VectorWrapper<Int32x2, int32_vec2_t>;
  using Base::Base;
  using Base::operator=;
};

class Float8 : public VectorWrapper<Float8, float_vec8_t> {
public:
  using Base = VectorWrapper<Float8, float_vec8_t>;
  using Base::Base;
  using Base::operator=;
};

class Float4 : public VectorWrapper<Float4, float_vec4_t> {
public:
  using Base = VectorWrapper<Float4, float_vec4_t>;
  using Base::Base;
  using Base::operator=;
};

class Float2 : public VectorWrapper<Float2, float_vec2_t> {
public:
  using Base = VectorWrapper<Float2, float_vec2_t>;
  using Base::Base;
  using Base::operator=;
};

// Saturated narrowing integer conversion functions.

// 8x int32_t -> 8x uint8_t
[[maybe_unused]]
static AVS_FORCEINLINE void convert_and_saturate_int32x8_to_uint8x8(const Int32x8& src1, Uint8x8& result) {
  for (size_t i = 0; i < 8; ++i) {
    result.set(i, static_cast<uint8_t>(std::min(std::max(src1[i], 0), 255)));
  }
}

// 4x int32_t -> 4x uint8_t
[[maybe_unused]]
static AVS_FORCEINLINE void convert_and_saturate_int32x4_to_uint8x4(const Int32x4& src1, Uint8x4& result) {
  for (size_t i = 0; i < 4; ++i) {
    result.set(i, static_cast<uint8_t>(std::min(std::max(src1[i], 0), 255)));
  }
}

// 8x int32_t -> 8x uint16_t
[[maybe_unused]] static AVS_FORCEINLINE void convert_and_saturate_int32x8_to_uint16x8(const Int32x8& src1, Uint16x8& result) {
  for (size_t i = 0; i < 8; ++i) {
    result.set(i, static_cast<uint16_t>(std::min(std::max(src1[i], 0), 65535)));
  }
}

// 8x int32_t -> 8x uint16_t with 0 <= x <= limit
[[maybe_unused]] static AVS_FORCEINLINE void convert_and_saturate_int32x8_to_uint16x8_limit(const Int32x8& src1, Uint16x8& result, const uint16_t limit) {
  for (size_t i = 0; i < 8; ++i) {
    result.set(i, static_cast<uint16_t>(std::min(std::max(src1[i], 0), (int)limit)));
  }
}

// 4x int32_t -> 4x uint16_t
[[maybe_unused]] static AVS_FORCEINLINE void convert_and_saturate_int32x4_to_uint16x4(const Int32x4& src1, Uint16x4& result) {
  for (size_t i = 0; i < 4; ++i) {
    result.set(i, static_cast<uint16_t>(std::min(std::max(src1[i], 0), 65535)));
  }
}

// 4x int32_t -> 4x uint16_t with 0 <= x <= limit
[[maybe_unused]]
static AVS_FORCEINLINE void convert_and_saturate_int32x4_to_uint16x4_limit(const Int32x4& src1, Uint16x4& result, const uint16_t limit) {
  for (size_t i = 0; i < 4; ++i) {
    result.set(i, static_cast<uint16_t>(std::min(std::max(src1[i], 0), (int)limit)));
  }
}

// two 4x int32_t -> 8x uint16_t
// like an _mm_packus_epi32
[[maybe_unused]]
static AVS_FORCEINLINE void convert_and_saturate_int32x4x2_to_uint16x8(const Int32x4& src1, const Int32x4& src2, Uint16x8& result) {
  for (size_t i = 0; i < 4; ++i) {
    result.set(i, static_cast<uint16_t>(std::min(std::max(src1[i], 0), 65535)));
    result.set(i + 4, static_cast<uint16_t>(std::min(std::max(src2[i], 0), 65535)));
  }
}

// two 4x int32_t -> 8x uint16_t
// like a _mm_packus_epi32 followed by a max(x,limit)
[[maybe_unused]]
static AVS_FORCEINLINE void convert_and_saturate_int32x4x2_to_uint16x8_limit(const Int32x4& src1, const Int32x4& src2, Uint16x8& result, const uint16_t limit) {
  for (size_t i = 0; i < 4; ++i) {
    result.set(i, static_cast<uint16_t>(std::min(std::max(src1[i], 0), (int)limit)));
    result.set(i + 4, static_cast<uint16_t>(std::min(std::max(src2[i], 0), (int)limit)));
  }
}

// two 4x int32_t -> 8x uint8_t
// like an _mm_packs_epi32 followed by _mm_packus_epi16
[[maybe_unused]]
static AVS_FORCEINLINE void convert_and_saturate_int32x4x2_to_uint8x8(const Int32x4& src1, const Int32x4& src2, Uint8x8& result) {
  for (size_t i = 0; i < 4; ++i) {
    result.set(i, static_cast<uint8_t>(std::min(std::max(src1[i], 0), 255)));
    result.set(i + 4, static_cast<uint8_t>(std::min(std::max(src2[i], 0), 255)));
  }
  // This one is optimized with LLVM, but not with gcc or MSVC
  /* bravo LLVM x86-64 icx 2025.1.0, using return Int32x4

        movdqu  xmm0, xmmword ptr [rdi]
        packssdw        xmm0, xmm0
        packuswb        xmm0, xmm0
        movdqu  xmm1, xmmword ptr [rsi + 16]
        packssdw        xmm1, xmm1
        packuswb        xmm1, xmm1
        movd    eax, xmm0
        movd    ecx, xmm1
        shl     rcx, 32
        or      rax, rcx
        ret
    */
}

// int16 * int16->int32 with partial result addition.
// Simulates the _mm_madd_epi16 x86 SIMD intrinsic function.
// returns [a0*b0 + a1*b1, a2*b2 + a3*b3, a4*b4 + a5*b5, a6*b6 + a7*b7]
// When the pre-addition order is not important use the reduce_add version,
// which may be easier to optimize on non-x86 architectures.
// (only LLVM was able to optimize the x86 version and compile into real madd)
[[maybe_unused]]
static AVS_FORCEINLINE Int32x4 simul_madd_epi16(const Int16x8& a, const Int16x8& b) {
  Int32x4 result;

  // pairwise multiplication and horizontal addition
  // Yes, I know c++ integer promotion rules, but maybe the compiler
  // will see our forced static_casting the already short type.
  // Otherwise, yes, it makes no sense.
  for (size_t i = 0; i < 4; ++i) {
    size_t idx = i * 2;
    int32_t sum =
      static_cast<short>(a[idx]) * static_cast<short>(b[idx]) +
      static_cast<short>(a[idx + 1]) * static_cast<short>(b[idx + 1]);

    result.set(i, sum);
  }
  return result;
}


// int16 * int16 -> int32 with partial result addition.
// Unlike the classic x86 SIMD madd_epi16, the sum of multiplications
// is not horizontally adjacent, instead, the lanes are added together.
// returns [a0*b0 + a4*b4, a1*b1 + a5*b5, a2*b2 + a6*b6, a3*b3 + a7*b7]
// However, since the order is not always important, this approach is
// beneficial for non-x64 processor architectures when the compiler
// recognizes the widening multiplication pattern.
[[maybe_unused]]
static AVS_FORCEINLINE Int32x4 mul16x16_reduce_to_Int32x4(const Int16x8 &a, const Int16x8 &b) {
  Int32x4 result_lo, result_hi;

  for (size_t i = 0; i < 4; ++i) {
    result_lo.set(i, static_cast<short>(a[i]) * static_cast<short>(b[i]));
  }
  for (size_t i = 0; i < 4; ++i) {
    result_hi.set(i, static_cast<short>(a[i + 4]) * static_cast<short>(b[i + 4]));
  }
  Int32x4 result = result_lo + result_hi;
  return result;
}

[[maybe_unused]]
static AVS_FORCEINLINE Int32x4 mul16x16_to_Int32x4(const Int16x4& a, const Int16x4& b) {
  Int32x4 result;

  for (size_t i = 0; i < 4; ++i) {
    result.set(i, static_cast<short>(a[i]) * static_cast<short>(b[i]));
  }
  return result;
}

#endif  // __AVS_SIMD_C_H__
