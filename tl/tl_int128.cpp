#include "tl_int128.h"

#include <cassert>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>

#ifdef __has_attribute
#define HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define HAS_ATTRIBUTE(x) 0
#endif

#ifdef __has_builtin
#define HAVE_BUILTIN(x) __has_builtin(x)
#else
#define HAVE_BUILTIN(x) 0
#endif

#if HAS_ATTRIBUTE(always_inline) || (defined(__GNUC__) && !defined(__clang__))
#define ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#define HAVE_ATTRIBUTE_ALWAYS_INLINE 1
#else
#define ATTRIBUTE_ALWAYS_INLINE
#endif

#if defined(__GNUC__) && !defined(__clang__)
// GCC
#define NUMERIC_INTERNAL_HAVE_BUILTIN_OR_GCC(x) 1
#else
#define NUMERIC_INTERNAL_HAVE_BUILTIN_OR_GCC(x) HAVE_BUILTIN(x)
#endif

#if NUMERIC_INTERNAL_HAVE_BUILTIN_OR_GCC(__builtin_clz) && NUMERIC_INTERNAL_HAVE_BUILTIN_OR_GCC(__builtin_clzll)
#define INTERNAL_CONSTEXPR_CLZ constexpr
#define INTERNAL_HAS_CONSTEXPR_CLZ 1
#else
#define INTERNAL_CONSTEXPR_CLZ
#define INTERNAL_HAS_CONSTEXPR_CLZ 0
#endif

namespace tl {

namespace {

ATTRIBUTE_ALWAYS_INLINE INTERNAL_CONSTEXPR_CLZ inline int CountLeadingZeroes32(uint32_t x) {
#if HAVE_BUILTIN(__builtin_clz)
  static_assert(sizeof(unsigned int) == sizeof(x), "__builtin_clz does not take 32-bit arg");
  // Handle 0 as a special case because __builtin_clz(0) is undefined.
  return x == 0 ? 32 : __builtin_clz(x);
#elif defined(_MSC_VER) && !defined(__clang__)
  unsigned long result = 0;
  if (_BitScanReverse(&result, x)) {
    return 31 - result;
  }
  return 32;
#else
  int zeroes = 28;
  if (x >> 16) {
    zeroes -= 16;
    x >>= 16;
  }
  if (x >> 8) {
    zeroes -= 8;
    x >>= 8;
  }
  if (x >> 4) {
    zeroes -= 4;
    x >>= 4;
  }
  return "\4\3\2\2\1\1\1\1\0\0\0\0\0\0\0"[x] + zeroes;
#endif
}

// Returns the 0-based position of the last set bit (i.e., most significant bit)
// in the given uint128. The argument is not 0.
//
// For example:
//   Given: 5 (decimal) == 101 (binary)
//   Returns: 2
inline ATTRIBUTE_ALWAYS_INLINE int Fls128(int128 n) {
  if (auto hi = static_cast<uint64_t>(Int128High64(n))) {
    return 127 - CountLeadingZeroes32(hi);
  }
  const uint64_t low = Int128Low64(n);
  return 63 - CountLeadingZeroes32(low);
}

// Long division/modulo for int128 implemented using the shift-subtract
// division algorithm adapted from:
// https://stackoverflow.com/questions/5386377/division-without-using
inline void DivModImpl(int128 dividend, int128 divisor, int128& quotient_ret, int128& remainder_ret) {
  assert(divisor != 0);

  if (divisor > dividend) {
    quotient_ret = 0;
    remainder_ret = dividend;
    return;
  }

  if (divisor == dividend) {
    quotient_ret = 1;
    remainder_ret = 0;
    return;
  }

  int128 denominator = divisor;
  int128 quotient = 0;

  // Left aligns the MSB of the denominator and the dividend.
  const int shift = Fls128(dividend) - Fls128(denominator);
  denominator <<= shift;

  // Uses shift-subtract algorithm to divide dividend by denominator. The
  // remainder will be left in dividend.
  for (int i = 0; i <= shift; ++i) {
    quotient <<= 1;
    if (dividend >= denominator) {
      dividend -= denominator;
      quotient |= 1;
    }
    denominator >>= 1;
  }

  quotient_ret = quotient;
  remainder_ret = dividend;
}

int128 UnsignedAbsoluteValue(int128 v) {
  return Int128High64(v) < 0 ? -v : v;
}

std::string Int128ToFormattedString(int128 v, std::ios_base::fmtflags flags) {
  // Select a divisor which is the largest power of the base < 2^64.
  int128 div{};
  int div_base_log;
  switch (flags & std::ios::basefield) {
    case std::ios::hex:
      div = 0x1000000000000000;  // 16^15
      div_base_log = 15;
      break;
    case std::ios::oct:
      div = 01000000000000000000000;  // 8^21
      div_base_log = 21;
      break;
    default:                        // std::ios::dec
      div = 10000000000000000000u;  // 10^19
      div_base_log = 19;
      break;
  }

  // Now piece together the uint128 representation from three chunks of the
  // original value, each less than "div" and therefore representable as a
  // uint64_t.
  std::ostringstream os;
  std::ios_base::fmtflags copy_mask = std::ios::basefield | std::ios::showbase | std::ios::uppercase;
  os.setf(flags & copy_mask, copy_mask);
  int128 high = v;
  int128 low{};
  DivModImpl(high, div, high, low);
  int128 mid{};
  DivModImpl(high, div, high, mid);
  if (Int128Low64(high) != 0) {
    os << Int128Low64(high);
    os << std::noshowbase << std::setfill('0') << std::setw(div_base_log);
    os << Int128Low64(mid);
    os << std::setw(div_base_log);
  } else if (Int128Low64(mid) != 0) {
    os << Int128Low64(mid);
    os << std::noshowbase << std::setfill('0') << std::setw(div_base_log);
  }
  os << Int128Low64(low);
  return os.str();
}

}  // namespace

std::ostream& operator<<(std::ostream& os, int128 v) {
  std::ios_base::fmtflags flags = os.flags();
  std::string rep;

  // Add the sign if needed.
  bool print_as_decimal =
      (flags & std::ios::basefield) == std::ios::dec || (flags & std::ios::basefield) == std::ios_base::fmtflags();
  if (print_as_decimal) {
    if (Int128High64(v) < 0) {
      rep = "-";
    } else if (flags & std::ios::showpos) {
      rep = "+";
    }
  }

  rep.append(Int128ToFormattedString(
      print_as_decimal ? UnsignedAbsoluteValue(v) : MakeInt128(static_cast<uint64_t>(Int128High64(v)), Int128Low64(v)),
      os.flags()));

  // Add the requisite padding.
  std::streamsize width = os.width(0);
  if (static_cast<size_t>(width) > rep.size()) {
    switch (flags & std::ios::adjustfield) {
      case std::ios::left:
        rep.append(width - rep.size(), os.fill());
        break;
      case std::ios::internal:
        if (print_as_decimal && (rep[0] == '+' || rep[0] == '-')) {
          rep.insert(1, width - rep.size(), os.fill());
        } else if ((flags & std::ios::basefield) == std::ios::hex && (flags & std::ios::showbase) && v != 0) {
          rep.insert(2, width - rep.size(), os.fill());
        } else {
          rep.insert(0, width - rep.size(), os.fill());
        }
        break;
      default:  // std::ios::right
        rep.insert(0, width - rep.size(), os.fill());
        break;
    }
  }

  return os << rep;
}

}  // namespace tl

namespace std {
constexpr bool numeric_limits<tl::int128>::is_specialized;
constexpr bool numeric_limits<tl::int128>::is_signed;
constexpr bool numeric_limits<tl::int128>::is_integer;
constexpr bool numeric_limits<tl::int128>::is_exact;
constexpr bool numeric_limits<tl::int128>::has_infinity;
constexpr bool numeric_limits<tl::int128>::has_quiet_NaN;
constexpr bool numeric_limits<tl::int128>::has_signaling_NaN;
constexpr float_denorm_style numeric_limits<tl::int128>::has_denorm;
constexpr bool numeric_limits<tl::int128>::has_denorm_loss;
constexpr float_round_style numeric_limits<tl::int128>::round_style;
constexpr bool numeric_limits<tl::int128>::is_iec559;
constexpr bool numeric_limits<tl::int128>::is_bounded;
constexpr bool numeric_limits<tl::int128>::is_modulo;
constexpr int numeric_limits<tl::int128>::digits;
constexpr int numeric_limits<tl::int128>::digits10;
constexpr int numeric_limits<tl::int128>::max_digits10;
constexpr int numeric_limits<tl::int128>::radix;
constexpr int numeric_limits<tl::int128>::min_exponent;
constexpr int numeric_limits<tl::int128>::min_exponent10;
constexpr int numeric_limits<tl::int128>::max_exponent;
constexpr int numeric_limits<tl::int128>::max_exponent10;
constexpr bool numeric_limits<tl::int128>::traps;
constexpr bool numeric_limits<tl::int128>::tinyness_before;
}  // namespace std
