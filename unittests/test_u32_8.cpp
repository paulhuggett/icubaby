// MIT License
//
// Copyright (c) 2022 Paul Bowen-Huggett
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
#include <gmock/gmock.h>

#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t32_8 ::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_8 ::output_type, icubaby::char8>);
static_assert (std::is_same_v<icubaby::t32_16::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_16::output_type, char16_t>);
// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t32_32::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_32::output_type, char32_t>);

// NOLINTNEXTLINE
TEST (Utf32To8, MaxPlus1) {
  std::vector<icubaby::char8> out;
  // This is the Unicode Replacement Character U+FFFD as UTF-8.
  std::vector<icubaby::char8> const replacement{static_cast<icubaby::char8> (0xef), static_cast<icubaby::char8> (0xbf),
                                                static_cast<icubaby::char8> (0xbd)};
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;

  it = d1 (icubaby::max_code_point + 1, it);
  EXPECT_FALSE (d1.well_formed ());
  EXPECT_THAT (out, testing::ContainerEq (replacement));
}

// NOLINTNEXTLINE
TEST (Utf32To8, FirstLowSurrogate) {
  std::vector<icubaby::char8> out;
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;
  it = d1 (icubaby::first_low_surrogate, it);
  EXPECT_FALSE (d1.well_formed ());
}

// NOLINTNEXTLINE
TEST (Utf32To8, LowestTwoByteSequence) {
  std::vector<icubaby::char8> out;
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;
  it = d1 (0x80, it);
  EXPECT_TRUE (d1.well_formed ());
  d1.end_cp (it);
  EXPECT_TRUE (d1.well_formed ());
  EXPECT_THAT (out, testing::ElementsAre (static_cast<icubaby::char8> (0xc2), static_cast<icubaby::char8> (0x80)));
}

#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L
// NOLINTNEXTLINE
TEST (Utf32To8, RangesCopy) {
  std::vector const in{char32_t{0x3053}, char32_t{0x3093}, char32_t{0x306B}, char32_t{0x3061},
                       char32_t{0x306F}, char32_t{0x4E16}, char32_t{0x754C}, char32_t{0x000A}};

  std::vector<char8_t> out8;
  std::ranges::copy (in | icubaby::ranges::transcode<char32_t, char8_t>, std::back_inserter (out8));
  // clang-format off
  EXPECT_THAT (out8, testing::ElementsAre (
    char8_t{0xE3}, char8_t{0x81}, char8_t{0x93}, // U+3053 HIRAGANA LETTER KO
    char8_t{0xE3}, char8_t{0x82}, char8_t{0x93}, // U+3093 HIRAGANA LETTER N
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xAB}, // U+306B HIRAGANA LETTER NI
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xA1}, // U+3061 HIRAGANA LETTER TI
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xAF}, // U+306F HIRAGANA LETTER HA
    char8_t{0xE4}, char8_t{0xB8}, char8_t{0x96}, // U+4E16 CJK UNIFIED IDEOGRAPH-4E16
    char8_t{0xE7}, char8_t{0x95}, char8_t{0x8C}, // U+754C CJK UNIFIED IDEOGRAPH-754C
    char8_t{0x0A}                                // U+000A LINE FEED
  ));
  // clang-format on
}
#endif  // __cpp_lib_ranges
