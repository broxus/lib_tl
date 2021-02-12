#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iosfwd>
#include <limits>
#include <utility>

#if defined(_MSC_VER)
#if defined(_M_X64)
#include <intrin.h>
#pragma intrinsic(_umul128)
#endif  // defined(_M_X64)
#endif

#ifdef HAVE_INTRINSIC_INT128
#error HAVE_INTRINSIC_INT128 cannot be directly set
#elif defined(__SIZEOF_INT128__)
#if (defined(__clang__) && !defined(_WIN32)) || (defined(__CUDACC__) && __CUDACC_VER_MAJOR__ >= 9) || \
    (defined(__GNUC__) && !defined(__clang__) && !defined(__CUDACC__))
#define HAVE_INTRINSIC_INT128 1
#elif defined(__CUDACC__)
#if __CUDACC_VER__ >= 70000
#define HAVE_INTRINSIC_INT128 1
#endif  // __CUDACC_VER__ >= 70000
#endif  // defined(__CUDACC__)
#endif  // HAVE_INTRINSIC_INT128

#if defined(IS_BIG_ENDIAN)
#error "IS_BIG_ENDIAN cannot be directly set."
#endif
#if defined(IS_LITTLE_ENDIAN)
#error "IS_LITTLE_ENDIAN cannot be directly set."
#endif

#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define IS_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define IS_BIG_ENDIAN 1
#elif defined(_WIN32)
#define IS_LITTLE_ENDIAN 1
#else
#error "endian detection needs to be set up for your compiler"
#endif

namespace tl {

class int128 {
 public:
  int128() = default;

  // Constructors from arithmetic types
  constexpr int128(int v);
  constexpr int128(unsigned int v);
  constexpr int128(long v);
  constexpr int128(unsigned long v);
  constexpr int128(long long v);
  constexpr int128(unsigned long long v);
#ifdef HAVE_INTRINSIC_INT128
  constexpr int128(__int128 v);
  constexpr explicit int128(unsigned __int128 v);
#endif  // HAVE_INTRINSIC_INT128

  // Assignment operators from arithmetic types
  int128& operator=(int v);
  int128& operator=(unsigned int v);
  int128& operator=(long v);
  int128& operator=(unsigned long v);
  int128& operator=(long long v);
  int128& operator=(unsigned long long v);
#ifdef HAVE_INTRINSIC_INT128
  int128& operator=(__int128 v);
#endif  // HAVE_INTRINSIC_INT128

  // Conversion operators to other arithmetic types
  constexpr explicit operator bool() const;
  constexpr explicit operator char() const;
  constexpr explicit operator signed char() const;
  constexpr explicit operator unsigned char() const;
  constexpr explicit operator char16_t() const;
  constexpr explicit operator char32_t() const;
  constexpr explicit operator short() const;
  constexpr explicit operator unsigned short() const;
  constexpr explicit operator int() const;
  constexpr explicit operator unsigned int() const;
  constexpr explicit operator long() const;
  constexpr explicit operator unsigned long() const;
  constexpr explicit operator long long() const;
  constexpr explicit operator unsigned long long() const;
#ifdef HAVE_INTRINSIC_INT128
  constexpr explicit operator __int128() const;
  constexpr explicit operator unsigned __int128() const;
#endif  // HAVE_INTRINSIC_INT128

  // Arithmetic operators
  int128& operator+=(int128 other);
  int128& operator-=(int128 other);
  int128& operator*=(int128 other);
  int128 operator++(int);  // postfix increment: i++
  int128 operator--(int);  // postfix decrement: i--
  int128& operator++();    // prefix increment:  ++i
  int128& operator--();    // prefix decrement:  --i
  int128& operator&=(int128 other);
  int128& operator|=(int128 other);
  int128& operator^=(int128 other);
  int128& operator<<=(int amount);
  int128& operator>>=(int amount);

  // Int128Low64()
  //
  // Returns the lower 64-bit value of a `int128` value.
  friend constexpr uint64_t Int128Low64(int128 v);

  // Int128High64()
  //
  // Returns the higher 64-bit value of a `int128` value.
  friend constexpr int64_t Int128High64(int128 v);

  // MakeInt128()
  //
  // Constructs a `int128` numeric value from two 64-bit integers. Note that
  // signedness is conveyed in the upper `high` value.
  //
  //   (tl::int128(1) << 64) * high + low
  //
  // Note that this factory function is the only way to construct a `int128`
  // from integer values greater than 2^64 or less than -2^64.
  //
  // Example:
  //
  //   tl::int128 big = tl::MakeInt128(1, 0);
  //   tl::int128 big_n = tl::MakeInt128(-1, 0);
  friend constexpr int128 MakeInt128(int64_t high, uint64_t low);

  // Int128Max()
  //
  // Returns the maximum value for a 128-bit signed integer.
  friend constexpr int128 Int128Max();

  // Int128Min()
  //
  // Returns the minimum value for a 128-bit signed integer.
  friend constexpr int128 Int128Min();

 private:
  constexpr int128(int64_t high, uint64_t low);

#if defined(HAVE_INTRINSIC_INT128)
  __int128 v_;
#else  // HAVE_INTRINSIC_INT128
#if defined(IS_LITTLE_ENDIAN)
  uint64_t lo_;
  int64_t hi_;
#elif defined(IS_BIG_ENDIAN)
  int64_t hi_;
  uint64_t lo_;
#else  // byte order
#error "Unsupported byte order: must be little-endian or big-endian."
#endif  // byte order
#endif  // HAVE_INTRINSIC_INT128
};

std::ostream& operator<<(std::ostream& os, int128 v);

constexpr int128 Int128Max() {
  return int128((std::numeric_limits<int64_t>::max)(), (std::numeric_limits<uint64_t>::max)());
}

constexpr int128 Int128Min() {
  return int128((std::numeric_limits<int64_t>::min)(), 0);
}

}  // namespace tl

// Specialized numeric_limits for int128.
namespace std {
template <>
class numeric_limits<tl::int128> {
 public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr float_denorm_style has_denorm = denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr float_round_style round_style = round_toward_zero;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = false;
  static constexpr int digits = 127;
  static constexpr int digits10 = 38;
  static constexpr int max_digits10 = 0;
  static constexpr int radix = 2;
  static constexpr int min_exponent = 0;
  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent = 0;
  static constexpr int max_exponent10 = 0;
#ifdef HAVE_INTRINSIC_INT128
  static constexpr bool traps = numeric_limits<__int128>::traps;
#else  // HAVE_INTRINSIC_INT128
  static constexpr bool traps = numeric_limits<uint64_t>::traps;
#endif  // HAVE_INTRINSIC_INT128
  static constexpr bool tinyness_before = false;

  static constexpr tl::int128(min)() {
    return tl::Int128Min();
  }
  static constexpr tl::int128 lowest() {
    return tl::Int128Min();
  }
  static constexpr tl::int128(max)() {
    return tl::Int128Max();
  }
  static constexpr tl::int128 epsilon() {
    return 0;
  }
  static constexpr tl::int128 round_error() {
    return 0;
  }
  static constexpr tl::int128 infinity() {
    return 0;
  }
  static constexpr tl::int128 quiet_NaN() {
    return 0;
  }
  static constexpr tl::int128 signaling_NaN() {
    return 0;
  }
  static constexpr tl::int128 denorm_min() {
    return 0;
  }
};
}  // namespace std

namespace tl {

constexpr int128 MakeInt128(int64_t high, uint64_t low) {
  return int128(high, low);
}

// Assignment from integer types.
inline int128& int128::operator=(int v) {
  return *this = int128(v);
}

inline int128& int128::operator=(unsigned int v) {
  return *this = int128(v);
}

inline int128& int128::operator=(long v) {
  return *this = int128(v);
}

inline int128& int128::operator=(unsigned long v) {
  return *this = int128(v);
}

inline int128& int128::operator=(long long v) {
  return *this = int128(v);
}

inline int128& int128::operator=(unsigned long long v) {
  return *this = int128(v);
}

// Arithmetic operators.

int128 operator+(int128 lhs, int128 rhs);
int128 operator-(int128 lhs, int128 rhs);
int128 operator*(int128 lhs, int128 rhs);
int128 operator|(int128 lhs, int128 rhs);
int128 operator&(int128 lhs, int128 rhs);
int128 operator^(int128 lhs, int128 rhs);
int128 operator<<(int128 lhs, int amount);
int128 operator>>(int128 lhs, int amount);

inline int128& int128::operator+=(int128 other) {
  *this = *this + other;
  return *this;
}

inline int128& int128::operator-=(int128 other) {
  *this = *this - other;
  return *this;
}

inline int128& int128::operator*=(int128 other) {
  *this = *this * other;
  return *this;
}

inline int128& int128::operator|=(int128 other) {
  *this = *this | other;
  return *this;
}

inline int128& int128::operator&=(int128 other) {
  *this = *this & other;
  return *this;
}

inline int128& int128::operator^=(int128 other) {
  *this = *this ^ other;
  return *this;
}

inline int128& int128::operator<<=(int amount) {
  *this = *this << amount;
  return *this;
}

inline int128& int128::operator>>=(int amount) {
  *this = *this >> amount;
  return *this;
}

namespace int128_internal {

// Casts from unsigned to signed while preserving the underlying binary
// representation.
constexpr int64_t BitCastToSigned(uint64_t v) {
  // Casting an unsigned integer to a signed integer of the same
  // width is implementation defined behavior if the source value would not fit
  // in the destination type. We step around it with a roundtrip bitwise not
  // operation to make sure this function remains constexpr. Clang, GCC, and
  // MSVC optimize this to a no-op on x86-64.
  return v & (uint64_t{1} << 63) ? ~static_cast<int64_t>(~v) : static_cast<int64_t>(v);
}

}  // namespace int128_internal

#if defined(HAVE_INTRINSIC_INT128)
#include "tl_int128_intrinsic.h"
#else  // HAVE_INTRINSIC_INT128
#include "tl_int128_no_intrinsic.h"
#endif  // HAVE_INTRINSIC_INT128

}  // namespace tl
