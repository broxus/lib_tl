#pragma once

namespace int128_internal {

constexpr __int128 BitCastToSigned(unsigned __int128 v) {
  return v & (static_cast<unsigned __int128>(1) << 127) ? ~static_cast<__int128>(~v) : static_cast<__int128>(v);
}

}  // namespace int128_internal

inline int128& int128::operator=(__int128 v) {
  v_ = v;
  return *this;
}

constexpr uint64_t Int128Low64(int128 v) {
  return static_cast<uint64_t>(v.v_ & ~uint64_t{0});
}

constexpr int64_t Int128High64(int128 v) {
  return int128_internal::BitCastToSigned(static_cast<uint64_t>(static_cast<unsigned __int128>(v.v_) >> 64));
}

constexpr int128::int128(int64_t high, uint64_t low)
    : v_(int128_internal::BitCastToSigned(static_cast<unsigned __int128>(high) << 64) | low) {
}

constexpr int128::int128(int v) : v_{v} {
}

constexpr int128::int128(long v) : v_{v} {
}

constexpr int128::int128(long long v) : v_{v} {
}

constexpr int128::int128(__int128 v) : v_{v} {
}

constexpr int128::int128(unsigned int v) : v_{v} {
}

constexpr int128::int128(unsigned long v) : v_{v} {
}

constexpr int128::int128(unsigned long long v) : v_{v} {
}

constexpr int128::int128(unsigned __int128 v) : v_{static_cast<__int128>(v)} {
}

inline int128::int128(float v) {
  v_ = static_cast<__int128>(v);
}

inline int128::int128(double v) {
  v_ = static_cast<__int128>(v);
}

inline int128::int128(long double v) {
  v_ = static_cast<__int128>(v);
}

constexpr int128::int128(uint128 v) : v_{static_cast<__int128>(v)} {
}

constexpr int128::operator bool() const {
  return static_cast<bool>(v_);
}

constexpr int128::operator char() const {
  return static_cast<char>(v_);
}

constexpr int128::operator signed char() const {
  return static_cast<signed char>(v_);
}

constexpr int128::operator unsigned char() const {
  return static_cast<unsigned char>(v_);
}

constexpr int128::operator char16_t() const {
  return static_cast<char16_t>(v_);
}

constexpr int128::operator char32_t() const {
  return static_cast<char32_t>(v_);
}

constexpr int128::operator short() const {
  return static_cast<short>(v_);
}

constexpr int128::operator unsigned short() const {
  return static_cast<unsigned short>(v_);
}

constexpr int128::operator int() const {
  return static_cast<int>(v_);
}

constexpr int128::operator unsigned int() const {
  return static_cast<unsigned int>(v_);
}

constexpr int128::operator long() const {
  return static_cast<long>(v_);
}

constexpr int128::operator unsigned long() const {
  return static_cast<unsigned long>(v_);
}

constexpr int128::operator long long() const {
  return static_cast<long long>(v_);
}

constexpr int128::operator unsigned long long() const {
  return static_cast<unsigned long long>(v_);
}

constexpr int128::operator __int128() const {
  return v_;
}

constexpr int128::operator unsigned __int128() const {
  return static_cast<unsigned __int128>(v_);
}

inline int128::operator float() const {
  return static_cast<float>(v_);
}

inline int128::operator double() const {
  return static_cast<double>(v_);
}

inline int128::operator long double() const {
  return static_cast<long double>(v_);
}

// Comparison operators.

inline bool operator==(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) == static_cast<__int128>(rhs);
}

inline bool operator!=(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) != static_cast<__int128>(rhs);
}

inline bool operator<(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) < static_cast<__int128>(rhs);
}

inline bool operator>(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) > static_cast<__int128>(rhs);
}

inline bool operator<=(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) <= static_cast<__int128>(rhs);
}

inline bool operator>=(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) >= static_cast<__int128>(rhs);
}

// Unary operators.

inline int128 operator-(int128 v) {
  return -static_cast<__int128>(v);
}

inline bool operator!(int128 v) {
  return !static_cast<__int128>(v);
}

inline int128 operator~(int128 val) {
  return ~static_cast<__int128>(val);
}

// Arithmetic operators.

inline int128 operator+(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) + static_cast<__int128>(rhs);
}

inline int128 operator-(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) - static_cast<__int128>(rhs);
}

inline int128 operator*(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) * static_cast<__int128>(rhs);
}

inline int128 int128::operator++(int) {
  int128 tmp(*this);
  ++v_;
  return tmp;
}

inline int128 int128::operator--(int) {
  int128 tmp(*this);
  --v_;
  return tmp;
}

inline int128& int128::operator++() {
  ++v_;
  return *this;
}

inline int128& int128::operator--() {
  --v_;
  return *this;
}

inline int128 operator|(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) | static_cast<__int128>(rhs);
}

inline int128 operator&(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) & static_cast<__int128>(rhs);
}

inline int128 operator^(int128 lhs, int128 rhs) {
  return static_cast<__int128>(lhs) ^ static_cast<__int128>(rhs);
}

inline int128 operator<<(int128 lhs, int amount) {
  return static_cast<__int128>(lhs) << amount;
}

inline int128 operator>>(int128 lhs, int amount) {
  return static_cast<__int128>(lhs) >> amount;
}
