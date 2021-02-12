#pragma once

constexpr uint64_t Int128Low64(int128 v) {
  return v.lo_;
}

constexpr int64_t Int128High64(int128 v) {
  return v.hi_;
}

#if defined(IS_LITTLE_ENDIAN)

constexpr int128::int128(int64_t high, uint64_t low) : lo_(low), hi_(high) {
}

constexpr int128::int128(int v) : lo_{static_cast<uint64_t>(v)}, hi_{v < 0 ? ~int64_t{0} : 0} {
}
constexpr int128::int128(long v)  // NOLINT(runtime/int)
    : lo_{static_cast<uint64_t>(v)}, hi_{v < 0 ? ~int64_t{0} : 0} {
}
constexpr int128::int128(long long v)  // NOLINT(runtime/int)
    : lo_{static_cast<uint64_t>(v)}, hi_{v < 0 ? ~int64_t{0} : 0} {
}

constexpr int128::int128(unsigned int v) : lo_{v}, hi_{0} {
}
constexpr int128::int128(unsigned long v) : lo_{v}, hi_{0} {
}
constexpr int128::int128(unsigned long long v) : lo_{v}, hi_{0} {
}

#elif defined(IS_BIG_ENDIAN)

constexpr int128::int128(int64_t high, uint64_t low) : hi_{high}, lo_{low} {
}

constexpr int128::int128(int v) : hi_{v < 0 ? ~int64_t{0} : 0}, lo_{static_cast<uint64_t>(v)} {
}
constexpr int128::int128(long v)  // NOLINT(runtime/int)
    : hi_{v < 0 ? ~int64_t{0} : 0}, lo_{static_cast<uint64_t>(v)} {
}
constexpr int128::int128(long long v)  // NOLINT(runtime/int)
    : hi_{v < 0 ? ~int64_t{0} : 0}, lo_{static_cast<uint64_t>(v)} {
}

constexpr int128::int128(unsigned int v) : hi_{0}, lo_{v} {
}
constexpr int128::int128(unsigned long v) : hi_{0}, lo_{v} {
}
constexpr int128::int128(unsigned long long v) : hi_{0}, lo_{v} {
}

#else  // byte order
#error "Unsupported byte order: must be little-endian or big-endian."
#endif  // byte order

constexpr int128::operator bool() const {
  return lo_ || hi_;
}

constexpr int128::operator char() const {
  return static_cast<char>(static_cast<long long>(*this));
}

constexpr int128::operator signed char() const {
  return static_cast<signed char>(static_cast<long long>(*this));
}

constexpr int128::operator unsigned char() const {
  return static_cast<unsigned char>(lo_);
}

constexpr int128::operator char16_t() const {
  return static_cast<char16_t>(lo_);
}

constexpr int128::operator char32_t() const {
  return static_cast<char32_t>(lo_);
}

constexpr int128::operator short() const {
  return static_cast<short>(static_cast<long long>(*this));
}

constexpr int128::operator unsigned short() const {
  return static_cast<unsigned short>(lo_);
}

constexpr int128::operator int() const {
  return static_cast<int>(static_cast<long long>(*this));
}

constexpr int128::operator unsigned int() const {
  return static_cast<unsigned int>(lo_);
}

constexpr int128::operator long() const {
  return static_cast<long>(static_cast<long long>(*this));
}

constexpr int128::operator unsigned long() const {
  return static_cast<unsigned long>(lo_);
}

constexpr int128::operator long long() const {
  return int128_internal::BitCastToSigned(lo_);
}

constexpr int128::operator unsigned long long() const {
  return static_cast<unsigned long long>(lo_);
}

int128 operator-(int128 v);
bool operator!=(int128 lhs, int128 rhs);

// Comparison operators.

inline bool operator==(int128 lhs, int128 rhs) {
  return (Int128Low64(lhs) == Int128Low64(rhs) && Int128High64(lhs) == Int128High64(rhs));
}

inline bool operator!=(int128 lhs, int128 rhs) {
  return !(lhs == rhs);
}

inline bool operator<(int128 lhs, int128 rhs) {
  return (Int128High64(lhs) == Int128High64(rhs))  //
             ? (Int128Low64(lhs) < Int128Low64(rhs))
             : (Int128High64(lhs) < Int128High64(rhs));
}

inline bool operator>(int128 lhs, int128 rhs) {
  return (Int128High64(lhs) == Int128High64(rhs))  //
             ? (Int128Low64(lhs) > Int128Low64(rhs))
             : (Int128High64(lhs) > Int128High64(rhs));
}

inline bool operator<=(int128 lhs, int128 rhs) {
  return !(lhs > rhs);
}

inline bool operator>=(int128 lhs, int128 rhs) {
  return !(lhs < rhs);
}

// Unary operators.

inline int128 operator-(int128 v) {
  int64_t hi = ~Int128High64(v);
  uint64_t lo = ~Int128Low64(v) + 1;
  if (lo == 0) {
    ++hi;  // carry
  }
  return MakeInt128(hi, lo);
}

inline bool operator!(int128 v) {
  return !Int128Low64(v) && !Int128High64(v);
}

inline int128 operator~(int128 val) {
  return MakeInt128(~Int128High64(val), ~Int128Low64(val));
}

// Arithmetic operators.

inline int128 operator+(int128 lhs, int128 rhs) {
  int128 result = MakeInt128(Int128High64(lhs) + Int128High64(rhs), Int128Low64(lhs) + Int128Low64(rhs));
  if (Int128Low64(result) < Int128Low64(lhs)) {  // check for carry
    return MakeInt128(Int128High64(result) + 1, Int128Low64(result));
  }
  return result;
}

inline int128 operator-(int128 lhs, int128 rhs) {
  int128 result = MakeInt128(Int128High64(lhs) - Int128High64(rhs), Int128Low64(lhs) - Int128Low64(rhs));
  if (Int128Low64(lhs) < Int128Low64(rhs)) {  // check for carry
    return MakeInt128(Int128High64(result) - 1, Int128Low64(result));
  }
  return result;
}

inline int128 operator*(int128 lhs, int128 rhs) {
#if defined(HAVE_INTRINSIC_INT128)
  return static_cast<unsigned __int128>(lhs) * static_cast<unsigned __int128>(rhs);
#elif defined(_MSC_VER) && defined(_M_X64)
  uint64_t lhs_low = Int128Low64(lhs);
  uint64_t lhs_high = static_cast<uint64_t>(Int128High64(lhs));

  uint64_t rhs_low = Int128Low64(rhs);
  uint64_t rhs_high = static_cast<uint64_t>(Int128High64(rhs));

  uint64_t carry;
  uint64_t low = _umul128(lhs_low, rhs_low, &carry);
  return MakeInt128(int128_internal::BitCastToSigned(lhs_low * rhs_high + lhs_high * rhs_low + carry), low);
#else  // HAVE_INTRINSIC_INT128
  uint64_t lhsLow = Int128Low64(lhs);
  uint64_t lhsHigh = static_cast<uint64_t>(Int128High64(lhs));

  uint64_t rhsLow = Int128Low64(rhs);
  uint64_t rhsHigh = static_cast<uint64_t>(Int128High64(rhs));

  uint64_t a32 = lhsLow >> 32;
  uint64_t a00 = lhsLow & 0xffffffff;
  uint64_t b32 = rhsLow >> 32;
  uint64_t b00 = rhsLow & 0xffffffff;

  uint64_t high = lhsHigh * rhsLow + lhsLow * rhsHigh + a32 * b32;
  uint64_t low = a00 * b00;

  uint64_t semiMul_a32_b00 = a32 * b00;
  low += semiMul_a32_b00 << 32;
  high += semiMul_a32_b00 >> 32;

  uint64_t semiMul_a00_b32 = a00 * b32;
  low += semiMul_a00_b32 << 32;
  high += semiMul_a00_b32 >> 32;

  return MakeInt128(int128_internal::BitCastToSigned(high), low);
#endif  // HAVE_INTRINSIC_INT128
}

inline int128 int128::operator++(int) {
  int128 tmp(*this);
  *this += 1;
  return tmp;
}

inline int128 int128::operator--(int) {
  int128 tmp(*this);
  *this -= 1;
  return tmp;
}

inline int128& int128::operator++() {
  *this += 1;
  return *this;
}

inline int128& int128::operator--() {
  *this -= 1;
  return *this;
}

inline int128 operator|(int128 lhs, int128 rhs) {
  return MakeInt128(Int128High64(lhs) | Int128High64(rhs), Int128Low64(lhs) | Int128Low64(rhs));
}

inline int128 operator&(int128 lhs, int128 rhs) {
  return MakeInt128(Int128High64(lhs) & Int128High64(rhs), Int128Low64(lhs) & Int128Low64(rhs));
}

inline int128 operator^(int128 lhs, int128 rhs) {
  return MakeInt128(Int128High64(lhs) ^ Int128High64(rhs), Int128Low64(lhs) ^ Int128Low64(rhs));
}

inline int128 operator<<(int128 lhs, int amount) {
  // uint64_t shifts of >= 64 are undefined, so we need some special-casing.
  if (amount < 64) {
    if (amount != 0) {
      return MakeInt128((Int128High64(lhs) << amount) | static_cast<int64_t>(Int128Low64(lhs) >> (64 - amount)),
                        Int128Low64(lhs) << amount);
    }
    return lhs;
  }
  return MakeInt128(static_cast<int64_t>(Int128Low64(lhs) << (amount - 64)), 0);
}

inline int128 operator>>(int128 lhs, int amount) {
  // uint64_t shifts of >= 64 are undefined, so we need some special-casing.
  if (amount < 64) {
    if (amount != 0) {
      return MakeInt128(Int128High64(lhs) >> amount,
                        (Int128Low64(lhs) >> amount) | (static_cast<uint64_t>(Int128High64(lhs)) << (64 - amount)));
    }
    return lhs;
  }
  return MakeInt128(0, static_cast<uint64_t>(Int128High64(lhs) >> (amount - 64)));
}
