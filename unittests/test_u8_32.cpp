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

#include <array>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t8_8 ::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_8 ::output_type, icubaby::char8>);
static_assert (std::is_same_v<icubaby::t8_16::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t8_32::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_32::output_type, char32_t>);

using testing::ElementsAre;

// | UTF-8 Sequence         | Code point                   |
// | ---------------------- + ---------------------------- +
// | 0x24,                  | U+0024  DOLLAR SIGN          |
// | 0xC2, 0xA2,            | U+00A2  CENT SIGN            |
// | 0xE0, 0xA4, 0xB9,      | U+0939  DEVANAGARI LETTER HA |
// | 0xE2, 0x82, 0xAC,      | U+20AC  EURO SIGN            |
// | 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN  |
// | 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR  |

// NOLINTNEXTLINE
TEST (Utf8To32, GoodDollarSign) {
  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  EXPECT_TRUE (d.well_formed ());
  out = d (0x24, out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_FALSE (d.partial ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0024}));
  d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_FALSE (d.partial ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0024}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodCentSign) {
  // 0xC2, 0xA2 => U+00A2  CENT SIGN
  std::array<icubaby::char8, 2> const cent_sign = {
      {static_cast<icubaby::char8> (0xC2), static_cast<icubaby::char8> (0xA2)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (cent_sign[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (d.partial ());
  EXPECT_TRUE (cu.empty ());
  out = d (cent_sign[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_FALSE (d.partial ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x00A2}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_FALSE (d.partial ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x00A2}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodDevanagariLetterHa) {
  // 0xE0, 0xA4, 0xB9 => U+0939 DEVANAGARI LETTER HA
  std::array<icubaby::char8, 3> const devanagri_letter_ha{
      {static_cast<icubaby::char8> (0xE0), static_cast<icubaby::char8> (0xA4), static_cast<icubaby::char8> (0xB9)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (devanagri_letter_ha[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (d.partial ());
  EXPECT_TRUE (cu.empty ());
  out = d (devanagri_letter_ha[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (d.partial ());
  EXPECT_TRUE (cu.empty ());
  out = d (devanagri_letter_ha[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_FALSE (d.partial ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0939}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_FALSE (d.partial ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0939}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodEuroSign) {
  // 0xE2, 0x82, 0xAC => U+20AC EURO SIGN
  std::array<icubaby::char8, 3> const euro_sign{
      {static_cast<icubaby::char8> (0xE2), static_cast<icubaby::char8> (0x82), static_cast<icubaby::char8> (0xAC)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (euro_sign[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (euro_sign[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (euro_sign[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x20AC}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x20AC}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodHangulSyllableHan) {
  // 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN
  std::array<icubaby::char8, 3> const hangul_syllable_han{
      {static_cast<icubaby::char8> (0xED), static_cast<icubaby::char8> (0x95), static_cast<icubaby::char8> (0x9C)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (hangul_syllable_han[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (hangul_syllable_han[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (hangul_syllable_han[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0xD55C}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0xD55C}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodGothicLetterHwair) {
  // 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR
  std::array<icubaby::char8, 4> const gothic_letter_hwair{
      {static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x90), static_cast<icubaby::char8> (0x8D),
       static_cast<icubaby::char8> (0x88)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (gothic_letter_hwair[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (gothic_letter_hwair[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (gothic_letter_hwair[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (gothic_letter_hwair[3], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x10348}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x10348}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, Bad1) {
  icubaby::t8_32 d2;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d2 (static_cast<icubaby::char8> (0x80), it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  EXPECT_FALSE (d2.well_formed ());
  it = d2 (icubaby::char8{0x24}, it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char, char32_t{0x24}));
  EXPECT_FALSE (d2.well_formed ());
  it = d2.end_cp (it);
  EXPECT_FALSE (d2.well_formed ());
}

// NOLINTNEXTLINE
TEST (Utf8To32, Bad2) {
  icubaby::t8_32 d2;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d2 (static_cast<icubaby::char8> (0x80), it);
  EXPECT_FALSE (d2.well_formed ());
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  it = d2.end_cp (it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  EXPECT_FALSE (d2.well_formed ());
}

// NOLINTNEXTLINE
TEST (Utf8To32, AssignBad) {
  icubaby::t32_8 t1;
  std::vector<icubaby::char8> out;
  // A code unit t1 will signal as an error (!good()).
  t1.end_cp (t1 (icubaby::first_low_surrogate, std::back_inserter (out)));
  EXPECT_FALSE (t1.well_formed ());

  icubaby::t8_32 t2{t1.well_formed ()};
  EXPECT_FALSE (t2.well_formed ()) << "The 'good' state should be transfered";
}

// NOLINTNEXTLINE
TEST (Utf8To32, PartialEndCp) {
  // U+1F0A6 PLAYING CARD SIX OF SPADES
  // UTF-8: F0 9F 82 A6

  icubaby::t8_32 d;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d (static_cast<icubaby::char8> (0xF0), it);
  it = d (static_cast<icubaby::char8> (0x9F), it);
  it = d (static_cast<icubaby::char8> (0x82), it);
  it = d (static_cast<icubaby::char8> (0xA6), it);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x1F0A6}));
  // Now just the first two bytes of that sequence.
  it = d (static_cast<icubaby::char8> (0xF0), it);
  it = d (static_cast<icubaby::char8> (0x9F), it);
  EXPECT_TRUE (d.well_formed ());
  it = d.end_cp (it);
  EXPECT_FALSE (d.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x1F0A6}, icubaby::replacement_char));
}

#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L
// NOLINTNEXTLINE
TEST (Utf8To32, RangesCopy) {
  // clang-format off
  std::vector const in{
    char8_t{0xE3}, char8_t{0x81}, char8_t{0x93},  // U+3053 HIRAGANA LETTER KO
    char8_t{0xE3}, char8_t{0x82}, char8_t{0x93},  // U+3093 HIRAGANA LETTER N
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xAB},  // U+306B HIRAGANA LETTER NI
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xA1},  // U+3061 HIRAGANA LETTER TI
    char8_t{0xE3}, char8_t{0x81}, char8_t{0xAF},  // U+306F HIRAGANA LETTER HA
    char8_t{0xE4}, char8_t{0xB8}, char8_t{0x96},  // U+4E16 CJK UNIFIED IDEOGRAPH-4E16
    char8_t{0xE7}, char8_t{0x95}, char8_t{0x8C},  // U+754C CJK UNIFIED IDEOGRAPH-754C
    char8_t{0x0A}   // U+000A LINE FEED
  };
  // clang-format on
  std::vector<char32_t> out32;
  std::ranges::copy (in | icubaby::ranges::transcode<char8_t, char32_t>, std::back_inserter (out32));
  EXPECT_THAT (out32, testing::ElementsAre (char32_t{0x3053}, char32_t{0x3093}, char32_t{0x306B}, char32_t{0x3061},
                                            char32_t{0x306F}, char32_t{0x4E16}, char32_t{0x754C}, char32_t{0x000A}));
}
#endif  // __cpp_lib_ranges
