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

#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t16_8 ::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_8 ::output_type, icubaby::char8>);
// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t16_16::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t16_32::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_32::output_type, char32_t>);

using testing::ElementsAre;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

// NOLINTNEXTLINE
TEST (Utf16, Good1) {
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  icubaby::t16_32 d1;
  EXPECT_TRUE (d1.well_formed ());

  it = d1 (char16_t{1}, it);
  EXPECT_TRUE (d1.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x0001}));
  it = d1 (char16_t{2}, it);
  EXPECT_TRUE (d1.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x0001}, char32_t{0x0002}));
  it = d1 (char16_t{3}, it);
  EXPECT_TRUE (d1.well_formed ());
  EXPECT_THAT (
      out, ElementsAre (char32_t{0x0001}, char32_t{0x0002}, char32_t{0x0003}));
  it = d1 (char16_t{4}, it);
  EXPECT_TRUE (d1.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x0001}, char32_t{0x0002},
                                 char32_t{0x0003}, char32_t{0x0004}));
  d1.end_cp (it);
  EXPECT_TRUE (d1.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x0001}, char32_t{0x0002},
                                 char32_t{0x0003}, char32_t{0x0004}));
}

// NOLINTNEXTLINE
TEST (Utf16, Good2) {
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  icubaby::t16_32 d2;
  auto const match1 = ElementsAre (char32_t{0xFFFF});
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xFFFF, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match1);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xD800, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match1);
  auto const match2 = ElementsAre (char32_t{0xFFFF}, char32_t{0x10000});
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xDC00, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match2);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xD800, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match2);
  auto const match3 =
      ElementsAre (char32_t{0xFFFF}, char32_t{0x10000}, char32_t{0x10001});
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xDC01, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match3);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xD808, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match3);
  auto const match4 = ElementsAre (char32_t{0xFFFF}, char32_t{0x10000},
                                   char32_t{0x10001}, char32_t{0x12345});
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xDF45, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match4);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xDBFF, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_THAT (out, match4);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xDFFF, it);
  EXPECT_TRUE (d2.well_formed ());
  auto const match5 =
      ElementsAre (char32_t{0xFFFF}, char32_t{0x10000}, char32_t{0x10001},
                   char32_t{0x12345}, char32_t{0x10ffff});
  EXPECT_THAT (out, match5);
  it = d2.end_cp (it);
  EXPECT_THAT (out, match5);
  EXPECT_TRUE (d2.well_formed ());
}

// NOLINTNEXTLINE
TEST (Utf16, HighSurrogateWithoutLow) {
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  icubaby::t16_32 d2;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  it = d2 (0xD800, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_TRUE (out.empty ());
  it = d2 (0x0000, it);
  EXPECT_FALSE (d2.well_formed ());
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char, char32_t{0x0000}));
}

// NOLINTNEXTLINE
TEST (Utf16, HighSurrogateFollowedbyAnotherHigh) {
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  icubaby::t16_32 d2;
  it = d2 (icubaby::first_high_surrogate, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_TRUE (out.empty ());
  it = d2 (icubaby::first_high_surrogate, it);
  EXPECT_FALSE (d2.well_formed ());
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
}

// NOLINTNEXTLINE
TEST (Utf16, HighSurrogateAtEnd) {
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  icubaby::t16_32 d2;
  it = d2 (icubaby::first_high_surrogate, it);
  EXPECT_TRUE (d2.well_formed ());
  EXPECT_TRUE (out.empty ());
  it = d2.end_cp (it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  EXPECT_FALSE (d2.well_formed ());
}
