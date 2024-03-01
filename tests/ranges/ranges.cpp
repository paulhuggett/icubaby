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

#include <iostream>

#include "icubaby/icubaby.hpp"

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <ranges>
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

namespace {

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
void dump_vector (std::ostream& stream, std::vector<CharType> const& input) {
  auto const* separator = "";
  for (auto code_unit : input) {
    auto const output_code_unit = static_cast<typename char_to_output_type<CharType>::type> (code_unit);
    if constexpr (std::is_same_v<CharType, char8_t>) {
      stream << std::format ("{}0x{:02X}", separator, output_code_unit);
    } else {
      stream << std::format ("{}0x{:04X}", separator, output_code_unit);
    }
    separator = " ";
  }
  stream << '\n';
}

void dump_well_formed (std::ostream& stream, bool well_formed) {
  stream << std::format (" well formed? {}\n", well_formed);
}

#else

/// \brief A class used to save an iostream's formatting flags on construction
/// and restore them on destruction.
///
/// Used to manage the restoration of the flags on exit from a scope.
class ios_flags_saver {
public:
  explicit ios_flags_saver (std::ios_base& stream) : stream_{&stream}, flags_{stream.flags ()} {}
  ios_flags_saver (ios_flags_saver const&) = delete;
  ios_flags_saver (ios_flags_saver&&) noexcept = delete;

  ~ios_flags_saver () { stream_->flags (flags_); }

  ios_flags_saver& operator= (ios_flags_saver const&) = delete;
  ios_flags_saver& operator= (ios_flags_saver&&) noexcept = delete;

private:
  std::ios_base* stream_;
  std::ios_base::fmtflags flags_;
};

template <icubaby::unicode_char_type CharType>
void dump_vector (std::ostream& stream, std::vector<CharType> const& input) {
  ios_flags_saver const saved_flags{stream};
  constexpr auto width = std::is_same_v<CharType, char8_t> ? 2 : 4;
  auto const* separator = "";
  for (auto const code_unit : input) {
    stream << separator << "0x" << std::setw (width) << std::setfill ('0') << std::uppercase << std::hex
           << static_cast<typename char_to_output_type<CharType>::type> (code_unit);
    separator = " ";
  }
  stream << '\n';
}

void dump_well_formed (std::ostream& stream, bool well_formed) {
  ios_flags_saver const saved_flags{stream};
  stream << " well formed? " << std::boolalpha << well_formed << '\n';
}

#endif  // HAVE_CPP_LIB_FORMAT

template <std::ranges::input_range ActualRange, std::ranges::input_range ExpectedRange>
  requires std::is_same_v<std::ranges::range_value_t<ActualRange>, std::ranges::range_value_t<ExpectedRange>>
[[nodiscard]] bool check (ActualRange const& actual, ExpectedRange const& expected) {
  if (!std::ranges::equal (actual, expected)) {
    std::cerr << "Actual range did not equal the expected!\n";
    return false;
  }
  return true;
}

template <std::ranges::input_range Range>
  requires std::is_same_v<std::ranges::range_value_t<Range>, char8_t>
std::vector<char16_t> convert_8_to_16 (Range const& input) {
  std::cout << "Convert the UTF-8 stream to UTF-16:\n ";

  auto range = input | icubaby::views::transcode<char8_t, char16_t>;
  std::vector<char16_t> out16;
  (void)std::ranges::copy (range, std::back_inserter (out16));

  dump_vector (std::cout, out16);
  dump_well_formed (std::cout, range.well_formed ());
  return out16;
}

template <std::ranges::input_range Range>
  requires std::is_same_v<std::ranges::range_value_t<Range>, char8_t>
std::vector<char32_t> convert_8_to_32 (Range const& input) {
  std::cout << "Convert the UTF-8 stream to UTF-32:\n ";

  std::vector<char32_t> out32;
  auto range = input | icubaby::views::transcode<char8_t, char32_t>;
  (void)std::ranges::copy (range, std::back_inserter (out32));

  dump_vector (std::cout, out32);
  dump_well_formed (std::cout, range.well_formed ());
  return out32;
}

template <std::ranges::input_range Range>
  requires std::is_same_v<std::ranges::range_value_t<Range>, char32_t>
std::vector<char16_t> convert_32_to_16 (Range const& input) {
  std::cout << "Convert the UTF-32 stream to UTF-16:\n ";

  std::vector<char16_t> out16;
  auto range = input | icubaby::views::transcode<char32_t, char16_t>;
  (void)std::ranges::copy (range, std::back_inserter (out16));

  dump_vector (std::cout, out16);
  dump_well_formed (std::cout, range.well_formed ());
  return out16;
}

template <std::ranges::input_range Range>
  requires std::is_same_v<std::ranges::range_value_t<Range>, char16_t>
std::vector<char32_t> convert_16_to_32 (Range const& input) {
  std::cout << "Convert the UTF-16 stream to UTF-32:\n ";

  std::vector<char32_t> out32;
  auto range = input | icubaby::views::transcode<char16_t, char32_t>;
  (void)std::ranges::copy (range, std::back_inserter (out32));

  dump_vector (std::cout, out32);
  dump_well_formed (std::cout, range.well_formed ());
  return out32;
}

template <std::ranges::input_range Range>
  requires std::is_same_v<std::ranges::range_value_t<Range>, char16_t>
std::vector<char8_t> convert_16_to_8 (Range const& input) {
  std::cout << "Convert the UTF-16 stream to UTF-8:\n ";

  std::vector<char8_t> out8;
  auto range = input | icubaby::views::transcode<char16_t, char8_t>;
  (void)std::ranges::copy (range, std::back_inserter (out8));

  dump_vector (std::cout, out8);
  dump_well_formed (std::cout, range.well_formed ());
  return out8;
}

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

}  // end anonymous namespace

int main () {
  int exit_code = EXIT_SUCCESS;
  try {
    auto const& input = expected8;
#if HAVE_CPP_LIB_FORMAT
    std::cout << std::format ("input length is {} code points\n", icubaby::length (input));
#else
    std::cout << "input length is " << icubaby::length (input) << " code-points\n";
#endif

    auto const out16 = convert_8_to_16 (input);
    if (!check (out16, expected16)) {
      exit_code = EXIT_FAILURE;
    }

    auto const out32 = convert_8_to_32 (input);
    if (!check (out32, expected32)) {
      exit_code = EXIT_FAILURE;
    }

    if (!check (convert_32_to_16 (out32), expected16)) {
      exit_code = EXIT_FAILURE;
    }
    if (!check (convert_16_to_32 (out16), expected32)) {
      exit_code = EXIT_FAILURE;
    }
    if (!check (convert_16_to_8 (out16), input)) {
      exit_code = EXIT_FAILURE;
    }
  } catch (std::exception const& ex) {
    std::cerr << "Error: " << ex.what () << '\n';
    exit_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error was raised\n";
    exit_code = EXIT_FAILURE;
  }
  return exit_code;
}

#else

int main () {
  std::cout << "Sorry, icubaby C++ 20 ranges aren't supported by your build.\n";
}

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
