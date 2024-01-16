// MIT License
//
// Copyright (c) 2022-2024 Paul Bowen-Huggett
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>
#include <version>

// Do we have library support for C++ 20 std::format()?
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#define HAVE_CPP_LIB_FORMAT (1)
#include <format>
#else
// We're lacking std::format(). Fall back to using iostreams manipulators.
#include <iomanip>
#endif

// Do we have library support for C++ 20 ranges?
#if defined(__cpp_lib_ranges) && __cpp_lib_ranges > 201811L
#include <ranges>
#endif

#include "icubaby/icubaby.hpp"

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

namespace {

std::array const expected8{
    char8_t{0xE3}, char8_t{0x81}, char8_t{0x93},                 // U+3053 HIRAGANA LETTER KO
    char8_t{0xE3}, char8_t{0x82}, char8_t{0x93},                 // U+3093 HIRAGANA LETTER N
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xAB},                 // U+306B HIRAGANA LETTER NI
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xA1},                 // U+3061 HIRAGANA LETTER TI
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xAF},                 // U+306F HIRAGANA LETTER HA
    char8_t{0xE4}, char8_t{0xB8}, char8_t{0x96},                 // U+4E16 CJK UNIFIED IDEOGRAPH-4E16
    char8_t{0xE7}, char8_t{0x95}, char8_t{0x8C},                 // U+754C CJK UNIFIED IDEOGRAPH-754C
    char8_t{0xF0}, char8_t{0x9F}, char8_t{0x98}, char8_t{0x80},  // U+1F600 GRINNING FACE
    char8_t{0x0A}                                                // U+000A LINE FEED
};

std::array const expected16{
    char16_t{0x3053},                    // U+3053 HIRAGANA LETTER KO
    char16_t{0x3093},                    // U+3093 HIRAGANA LETTER N
    char16_t{0x306B},                    // U+306B HIRAGANA LETTER NI
    char16_t{0x3061},                    // U+3061 HIRAGANA LETTER TI
    char16_t{0x306F},                    // U+306F HIRAGANA LETTER HA
    char16_t{0x4E16},                    // U+4E16 CJK UNIFIED IDEOGRAPH-4E16
    char16_t{0x754C},                    // U+754C CJK UNIFIED IDEOGRAPH-754C
    char16_t{0xD83D}, char16_t{0xDE00},  // U+1F600 GRINNING FACE
    char16_t{0x000A}                     // U+000A LINE FEED
};

std::array const expected32{
    char32_t{0x3053},   // U+3053 HIRAGANA LETTER KO
    char32_t{0x3093},   // U+3093 HIRAGANA LETTER N
    char32_t{0x306B},   // U+306B HIRAGANA LETTER NI
    char32_t{0x3061},   // U+3061 HIRAGANA LETTER TI
    char32_t{0x306F},   // U+306F HIRAGANA LETTER HA
    char32_t{0x4E16},   // U+4E16 CJK UNIFIED IDEOGRAPH-4E16
    char32_t{0x754C},   // U+754C CJK UNIFIED IDEOGRAPH-754C
    char32_t{0x1F600},  // U+1F600 GRINNING FACE
    char32_t{0x000A}    // U+000A LINE FEED
};

template <icubaby::unicode_char_type CharType> struct char_to_output_type {};
template <> struct char_to_output_type<char8_t> {
  using type = unsigned;
};
template <> struct char_to_output_type<char16_t> {
  using type = std::uint_least16_t;
};
template <> struct char_to_output_type<char32_t> {
  using type = std::uint_least32_t;
};

#if HAVE_CPP_LIB_FORMAT

template <icubaby::unicode_char_type CharType>
std::ostream& dump_vector (std::ostream& os, std::vector<CharType> const& v) {
  auto const* separator = "";
  for (auto c : v) {
    auto const oc = static_cast<typename char_to_output_type<CharType>::type> (c);
    if constexpr (std::is_same_v<CharType, char8_t>) {
      os << std::format ("{}0x{:02X}", separator, oc);
    } else {
      os << std::format ("{}0x{:04X}", separator, oc);
    }
    separator = " ";
  }
  os << '\n';
  return os;
}

#else

/// \brief A class used to save an iostream's formatting flags on construction
/// and restore them on destruction.
///
/// Used to manage the restoration of the flags on exit from a scope.
class ios_flags_saver {
public:
  explicit ios_flags_saver (std::ios_base& stream) : stream_{stream}, flags_{stream.flags ()} {}
  ios_flags_saver (ios_flags_saver const&) = delete;
  ios_flags_saver (ios_flags_saver&&) noexcept = delete;

  ~ios_flags_saver () { stream_.flags (flags_); }

  ios_flags_saver& operator= (ios_flags_saver const&) = delete;
  ios_flags_saver& operator= (ios_flags_saver&&) noexcept = delete;

private:
  std::ios_base& stream_;
  std::ios_base::fmtflags flags_;
};

template <icubaby::unicode_char_type CharType>
std::ostream& dump_vector (std::ostream& os, std::vector<CharType> const& v) {
  ios_flags_saver _{os};
  constexpr auto width = std::is_same_v<CharType, char8_t> ? 2 : 4;
  auto const* separator = "";
  for (auto c : v) {
    os << separator << "0x" << std::setw (width) << std::setfill ('0') << std::uppercase << std::hex
       << static_cast<typename char_to_output_type<CharType>::type> (c);
    separator = " ";
  }
  os << '\n';
  return os;
}

#endif  // HAVE_CPP_LIB_FORMAT

template <std::ranges::input_range ActualRange, std::ranges::input_range ExpectedRange>
void check (ActualRange const& actual, ExpectedRange const& expected) {
  if (!std::ranges::equal (actual, expected)) {
    std::cerr << "Actual range did not equal the expected!\n";
    std::exit (EXIT_FAILURE);
  }
}

template <std::ranges::input_range Range> std::vector<char16_t> convert_8_to_16 (Range const& in) {
  auto r = in | icubaby::ranges::transcode<char8_t, char16_t>;
  std::vector<char16_t> out16;
  std::ranges::copy (r, std::back_inserter (out16));

  std::cout << "Convert the UTF-8 stream to UTF-16:\n ";
  dump_vector (std::cout, out16);
  std::cout << " well formed? " << r.well_formed () << '\n';

  check (out16, expected16);
  return out16;
}

template <std::ranges::input_range Range> std::vector<char32_t> convert_8_to_32 (Range const& in) {
  std::vector<char32_t> out32;
  std::ranges::copy (in | icubaby::ranges::transcode<char8_t, char32_t>, std::back_inserter (out32));

  std::cout << "Convert the UTF-8 stream to UTF-32:\n ";
  dump_vector (std::cout, out32);

  check (out32, expected32);
  return out32;
}

template <std::ranges::input_range Range> std::vector<char16_t> convert_32_to_16 (Range const& in) {
  std::vector<char16_t> out16;
  std::ranges::copy (in | icubaby::ranges::transcode<char32_t, char16_t>, std::back_inserter (out16));

  std::cout << "Convert the UTF-32 stream to UTF-16:\n ";
  dump_vector (std::cout, out16);

  check (out16, expected16);
  return out16;
}

template <std::ranges::input_range Range> std::vector<char32_t> convert_16_to_32 (Range const& in) {
  std::vector<char32_t> out32;
  auto const r = in | icubaby::ranges::transcode<char16_t, char32_t>;
  std::ranges::copy (r, std::back_inserter (out32));

  std::cout << "Convert the UTF-16 stream to UTF-32:\n ";
  dump_vector (std::cout, out32);
#if HAVE_CPP_LIB_FORMAT
  std::cout << std::format (" well formed? {}\n", r.well_formed ());
#else
  ios_flags_saver _{std::cout};
  std::cout << " well formed? " << std::boolalpha << r.well_formed () << '\n';
#endif

  check (out32, expected32);
  return out32;
}

template <std::ranges::input_range Range> std::vector<char8_t> convert_16_to_8 (Range const& in) {
  std::vector<char8_t> out8;
  auto const r = in | icubaby::ranges::transcode<char16_t, char8_t>;

  std::cout << "Convert the UTF-16 stream to UTF-8:\n ";
  std::ranges::copy (r, std::back_inserter (out8));
  dump_vector (std::cout, out8);
#if HAVE_CPP_LIB_FORMAT
  std::cout << std::format (" well formed? {}\n", r.well_formed ());
#else
  ios_flags_saver _{std::cout};
  std::cout << " well formed? " << std::boolalpha << r.well_formed () << '\n';
#endif

  return out8;
}

}  // end anonymous namespace

int main () {
  auto const& in = expected8;
#if HAVE_CPP_LIB_FORMAT
  std::cout << std::format ("input length is {} code points\n", icubaby::length (in));
#else
  std::cout << "input length is " << icubaby::length (in) << " code-points\n";
#endif

  auto const out16 = convert_8_to_16 (in);
  auto const out32 = convert_8_to_32 (in);
  convert_32_to_16 (out32);
  convert_16_to_32 (out16);
  auto const out8 = convert_16_to_8 (out16);
  assert (std::ranges::equal (in, out8));
}

#else

int main () {
  std::cout << "Sorry, icubaby C++ 20 ranges aren't supported by your build.\n";
}

#endif
