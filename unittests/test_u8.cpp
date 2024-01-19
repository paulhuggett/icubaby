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

#include <array>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

// icubaby itself.
#include "icubaby/icubaby.hpp"

// Google Test/Mock
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#if ICUBABY_FUZZTEST
#include "fuzztest/fuzztest.h"
#endif

// Local includes
#include "encoded_char.hpp"
#include "typed_test.hpp"

using testing::ContainerEq;
using testing::ElementsAre;

// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t8_8 ::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_8 ::output_type, icubaby::char8>);
// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t8_16::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_16::output_type, char16_t>);
// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t8_32::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_32::output_type, char32_t>);

namespace {

template <typename T> class Utf8 : public testing::Test {
protected:
  std::vector<T> output_;
  icubaby::transcoder<icubaby::char8, T> transcoder_;
};

}  // end anonymous namespace

TYPED_TEST_SUITE (Utf8, OutputTypes, OutputTypeNames);
// NOLINTNEXTLINE
TYPED_TEST (Utf8, DollarSign) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto out = std::back_inserter (output);
  EXPECT_TRUE (transcoder.well_formed ());
  out = transcoder (static_cast<icubaby::char8> (0x24), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<code_point::dollar_sign, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));

  transcoder.end_cp (out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ContainerEq (expected));
}

// NOLINTNEXTLINE
TYPED_TEST (Utf8, FirstLowSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  auto it = std::back_inserter (output);
  // Illegal UTF-8 for U+D800: the first low surrogate code point.
  it = transcoder (static_cast<icubaby::char8> (0xED), it);
  it = transcoder (static_cast<icubaby::char8> (0xA0), it);
  it = transcoder (static_cast<icubaby::char8> (0x80), it);
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());

  std::vector<TypeParam> expected;
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}

// NOLINTNEXTLINE
TYPED_TEST (Utf8, LowestTwoByteSequence) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = std::back_inserter (output);
  it = transcoder (static_cast<icubaby::char8> (0xC2), it);
  it = transcoder (static_cast<icubaby::char8> (0x80), it);
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());

  std::vector<TypeParam> expected;
  append<code_point::u80, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}

// NOLINTNEXTLINE
TYPED_TEST (Utf8, CentSign) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto out = std::back_inserter (output);

  constexpr auto const& cent_sign = encoded_char<code_point::cent_sign, icubaby::char8>::value;
  static_assert (cent_sign.size () == 2U, "cent_sign should be two UTF-8 code units");

  std::vector<TypeParam> expected;
  append<code_point::cent_sign, TypeParam> (std::back_inserter (expected));

  out = transcoder (cent_sign.at (0), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (cent_sign.at (1), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ContainerEq (expected));

  out = transcoder.end_cp (out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ContainerEq (expected));
}

// NOLINTNEXTLINE
TYPED_TEST (Utf8, DevanagariLetterHa) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto out = std::back_inserter (output);

  constexpr auto const& devanagri_letter_ha = encoded_char<code_point::devanagri_letter_ha, icubaby::char8>::value;
  static_assert (devanagri_letter_ha.size () == 3U, "devanagri_letter_ha should be three UTF-8 code units");

  std::vector<TypeParam> expected;
  append<code_point::devanagri_letter_ha, TypeParam> (std::back_inserter (expected));

  out = transcoder (devanagri_letter_ha.at (0), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (devanagri_letter_ha.at (1), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (devanagri_letter_ha.at (2), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ContainerEq (expected));

  out = transcoder.end_cp (out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ContainerEq (expected));
}

// NOLINTNEXTLINE
TYPED_TEST (Utf8, GoodGothicLetterHwair) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto out = std::back_inserter (output);

  constexpr auto const& gothic_letter_hwair = encoded_char<code_point::gothic_letter_hwair, icubaby::char8>::value;
  static_assert (gothic_letter_hwair.size () == 4U, "gothic_letter_hwair should be four UTF-8 code units");

  std::vector<TypeParam> expected;
  append<code_point::gothic_letter_hwair, TypeParam> (std::back_inserter (expected));

  out = transcoder (gothic_letter_hwair.at (0), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (gothic_letter_hwair.at (1), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (gothic_letter_hwair.at (2), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (gothic_letter_hwair.at (3), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ContainerEq (expected));

  out = transcoder.end_cp (out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ContainerEq (expected));
}

// NOLINTNEXTLINE
TYPED_TEST (Utf8, Bad1) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto out = std::back_inserter (output);

  std::vector<TypeParam> expected;
  out = transcoder (static_cast<icubaby::char8> (0x80), out);
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  out = transcoder (static_cast<icubaby::char8> (0x24), out);
  append<code_point::dollar_sign, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  transcoder.end_cp (out);
  EXPECT_THAT (output, ContainerEq (expected));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
}

// NOLINTNEXTLINE
TYPED_TEST (Utf8, PartialEndCp) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto out = std::back_inserter (output);

  constexpr auto& gothic_letter_hwair = encoded_char<code_point::gothic_letter_hwair, icubaby::char8>::value;
  static_assert (gothic_letter_hwair.size () == 4U, "gothic_letter_hwair should be four UTF-8 code units");

  out = transcoder (gothic_letter_hwair.at (0), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (gothic_letter_hwair.at (1), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder (gothic_letter_hwair.at (2), out);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  out = transcoder.end_cp (out);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}

namespace {

// The tests built with this fixture are derived from the "broken UTF-8" test page
// found here <https://hsivonen.fi/broken-utf-8/test.html>
template <typename OutputChar> class Utf8BadInput : public testing::Test {
protected:
  template <typename... Args> static std::vector<OutputChar> convert (Args&&... args) {
    icubaby::transcoder<icubaby::char8, OutputChar> transcoder;
    std::vector<OutputChar> output;
    add8 (transcoder, std::back_inserter (output), std::forward<Args> (args)...);
    return output;
  }

  static auto is_expected (std::size_t num) {
    std::vector<OutputChar> expected;
    replacement_chars (expected, num);
    return ContainerEq (expected);
  }

private:
  template <typename OutputIterator>
  static OutputIterator add8 (icubaby::transcoder<icubaby::char8, OutputChar>& transcoder, OutputIterator out, int c) {
    if (c < 0 || c > 255) {
      throw std::range_error ("character value out of range");
    }
    return transcoder.end_cp (transcoder (static_cast<icubaby::char8> (c), out));
  }
  template <typename OutputIterator, typename... Args>
  static OutputIterator add8 (icubaby::transcoder<icubaby::char8, OutputChar>& transcoder, OutputIterator out, int c,
                              Args... args) {
    if (c < 0 || c > 255) {
      throw std::range_error ("character value out of range");
    }
    return add8 (transcoder, transcoder (static_cast<icubaby::char8> (c), out), args...);
  }

  static void replacement_chars (std::vector<OutputChar>& v, std::size_t num) {
    if (num == 0U) {
      return;
    }
    append<code_point::replacement_char, OutputChar> (std::back_inserter (v));
    replacement_chars (v, num - 1U);
  }
};

}  // end anonymous namespace

TYPED_TEST_SUITE (Utf8BadInput, OutputTypes, OutputTypeNames);

// Non-shortest forms for lowest single-byte (U+0000)
TYPED_TEST (Utf8BadInput, NonShortestFormsForLowestSingleByte) {
  // Two-byte sequence (C0 80)
  EXPECT_THAT (this->convert (0xC0, 0x80), this->is_expected (2));
  // Three-byte sequence (E0 80 80)
  EXPECT_THAT (this->convert (0xE0, 0x80, 0x80), this->is_expected (2));
  // Four-byte sequence (F0 80 80 80)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0x80), this->is_expected (3));
  // Five-byte sequence (F8 80 80 80 80)
  EXPECT_THAT (this->convert (0xF8, 0x80, 0x80, 0x80, 0x80), this->is_expected (5));
  // Six-byte sequence (FC 80 80 80 80 80)
  EXPECT_THAT (this->convert (0xFC, 0x80, 0x80, 0x80, 0x80, 0x80), this->is_expected (6));
}

// Non-shortest forms for highest single-byte (U+007F)
TYPED_TEST (Utf8BadInput, NonShortestFormsForHighestSingleByte) {
  // Two-byte sequence (C1 BF)
  EXPECT_THAT (this->convert (0xC1, 0xBF), this->is_expected (2));
  // Three-byte sequence (E0 81 BF)
  EXPECT_THAT (this->convert (0xE0, 0x81, 0xBF), this->is_expected (2));
  // Four-byte sequence (F0 80 81 BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x81, 0xBF), this->is_expected (3));
  // Five-byte sequence (F8 80 80 81 BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0x81, 0xBF), this->is_expected (4));
  // Six-byte sequence (FC 80 80 80 81 BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0x80, 0x81, 0xBF), this->is_expected (5));
}

// Non-shortest forms for lowest two-byte (U+0080)
TYPED_TEST (Utf8BadInput, NonShortestFormsForLowestTwoByte) {
  // Three-byte sequence (E0 82 80)
  EXPECT_THAT (this->convert (0xE0, 0x82, 0x80), this->is_expected (2));
  // Four-byte sequence (F0 80 82 80)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x82, 0x80), this->is_expected (3));
  // Five-byte sequence (F8 80 80 82 80)
  EXPECT_THAT (this->convert (0xF8, 0x80, 0x80, 0x82, 0x80), this->is_expected (5));
  // Six-byte sequence (FC 80 80 80 82 80)
  EXPECT_THAT (this->convert (0xFC, 0x80, 0x80, 0x80, 0x82, 0x80), this->is_expected (6));
}

// Non-shortest forms for highest two-byte (U+07FF)
TYPED_TEST (Utf8BadInput, NonShortestFormsForHighestTwoByte) {
  // Three-byte sequence (E0 9F BF)
  EXPECT_THAT (this->convert (0xE0, 0x9F, 0xBF), this->is_expected (2));
  // Four-byte sequence (F0 80 9F BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x9F, 0xBF), this->is_expected (3));
  // Five-byte sequence (F8 80 80 9F BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0x9F, 0xBF), this->is_expected (4));
  // Six-byte sequence (FC 80 80 80 9F BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0x80, 0x9F, 0xBF), this->is_expected (5));
}

// Non-shortest forms for lowest three-byte (U+0800)
TYPED_TEST (Utf8BadInput, NonShortestFormsForLowestThreeByte) {
  // Four-byte sequence (F0 80 A0 80)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0xA0, 0x80), this->is_expected (3));
  // Five-byte sequence (F8 80 80 A0 80)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0xA0, 0x80), this->is_expected (4));
  // Six-byte sequence (FC 80 80 80 A0 80)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0x80, 0xA0, 0x80), this->is_expected (5));
}

// Non-shortest forms for highest three-byte (U+FFFF)
TYPED_TEST (Utf8BadInput, NonShortestFormsForHighestThreeByte) {
  // Four-byte sequence (F0 8F BF BF)
  EXPECT_THAT (this->convert (0xF0, 0x8F, 0xBF, 0xBF), this->is_expected (3));
  // Five-byte sequence (F8 80 8F BF BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x8F, 0xBF, 0xBF), this->is_expected (4));
  // Six-byte sequence (FC 80 80 8F BF BF)
  EXPECT_THAT (this->convert (0xF0, 0x80, 0x80, 0x8F, 0xBF, 0xBF), this->is_expected (5));
}

// Non-shortest forms for lowest four-byte (U+10000)
TYPED_TEST (Utf8BadInput, NonShortestFormsForLowestFourByte) {
  // Five-byte sequence (F8 80 90 80 80)
  EXPECT_THAT (this->convert (0xF8, 0x80, 0x90, 0x80, 0x80), this->is_expected (5));
  // Six-byte sequence (FC 80 80 90 80 80)
  EXPECT_THAT (this->convert (0xFC, 0x80, 0x80, 0x90, 0x80, 0x80), this->is_expected (6));
}

// Non-shortest forms for last Unicode (U+10FFFF)
TYPED_TEST (Utf8BadInput, NonShortestFormsForLastUnicode) {
  // Five-byte sequence (F8 84 8F BF BF)
  EXPECT_THAT (this->convert (0xF8, 0x84, 0x8F, 0xBF, 0xBF), this->is_expected (5));
  // Six-byte sequence (FC 80 84 8F BF BF)
  EXPECT_THAT (this->convert (0xF8, 0x84, 0x8F, 0xBF, 0xBF, 0xBF), this->is_expected (6));
}

// Out of range
TYPED_TEST (Utf8BadInput, OutOfRange) {
  // One past Unicode (F4 90 80 80)
  EXPECT_THAT (this->convert (0xF7, 0x90, 0x80, 0x80), this->is_expected (4));
  // Longest five-byte sequence (FB BF BF BF BF)
  EXPECT_THAT (this->convert (0xFC, 0xBF, 0xBF, 0xBF, 0xBF), this->is_expected (5));
  // Longest six-byte sequence (FD BF BF BF BF BF)
  EXPECT_THAT (this->convert (0xFC, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF), this->is_expected (6));
  // First surrogate (ED A0 80)
  EXPECT_THAT (this->convert (0xED, 0xA0, 0x80), this->is_expected (2));
  // Last surrogate (ED BF BF)
  EXPECT_THAT (this->convert (0xED, 0xBF, 0xBF), this->is_expected (2));
  // CESU-8 surrogate pair (ED A0 BD ED B2 A9)
  EXPECT_THAT (this->convert (0xED, 0xA0, 0xBD, 0xED, 0xB2, 0xA9), this->is_expected (4));
}

// Out of range and non-shortest
TYPED_TEST (Utf8BadInput, OutOfRangeAndNonShortest) {
  // One past Unicode as five-byte sequence (F8 84 90 80 80)
  EXPECT_THAT (this->convert (0xF8, 0x84, 0x90, 0x80, 0x80), this->is_expected (5));
  // One past Unicode as six-byte sequence (FC 80 84 90 80 80)
  EXPECT_THAT (this->convert (0xFC, 0x80, 0x84, 0x90, 0x80, 0x80), this->is_expected (6));
  // First surrogate as four-byte sequence (F0 8D A0 80)
  EXPECT_THAT (this->convert (0xF0, 0x8D, 0xA0, 0x80), this->is_expected (3));
  // Last surrogate as four-byte sequence (F0 8D BF BF)
  EXPECT_THAT (this->convert (0xF0, 0x8D, 0xBF, 0xBF), this->is_expected (3));
  // CESU-8 surrogate pair as two four-byte overlongs (F0 8D A0 BD F0 8D B2 A9)
  EXPECT_THAT (this->convert (0xF0, 0x8D, 0xA0, 0xBD, 0xF0, 0x8D, 0xB2, 0xA9), this->is_expected (6));
}

// Lone trails
TYPED_TEST (Utf8BadInput, LoneTrails) {
  // One (80)
  EXPECT_THAT (this->convert (0x80), this->is_expected (1));
  // Two (80 80)
  EXPECT_THAT (this->convert (0x80, 0x80), this->is_expected (2));
  // Three (80 80 80)
  EXPECT_THAT (this->convert (0x80, 0x80, 0x80), this->is_expected (3));
  // Four (80 80 80 80)
  EXPECT_THAT (this->convert (0x80, 0x80, 0x80, 0x80), this->is_expected (4));
  // Five (80 80 80 80 80)
  EXPECT_THAT (this->convert (0x80, 0x80, 0x80, 0x80, 0x80), this->is_expected (5));
  // Six (80 80 80 80 80 80)
  EXPECT_THAT (this->convert (0x80, 0x80, 0x80, 0x80, 0x80, 0x80), this->is_expected (6));
  // Seven (80 80 80 80 80 80 80)
  EXPECT_THAT (this->convert (0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80), this->is_expected (7));
  // After five-byte (FB BF BF BF BF 80)
  EXPECT_THAT (this->convert (0xFB, 0xBF, 0xBF, 0xBF, 0xBF, 0x80), this->is_expected (6));
  // After six-byte (FD BF BF BF BF BF 80)
  EXPECT_THAT (this->convert (0xFD, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0x80), this->is_expected (7));
}

TYPED_TEST (Utf8BadInput, LoneTrailsAfterValidTwoByte) {
  // After valid two-byte (C2 B6 80)
  auto const output = this->convert (0xC2, 0xB6, 0x80);
  std::vector<TypeParam> expected;
  append<code_point::pilcrow_sign, TypeParam> (std::back_inserter (expected));
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}

TYPED_TEST (Utf8BadInput, LoneTrailsAfterValidThreeByte) {
  // After valid three-byte (E2 98 83 80)
  auto const output = this->convert (0xE2, 0x98, 0x83, 0x80);
  std::vector<TypeParam> expected;
  append<code_point::snowman, TypeParam> (std::back_inserter (expected));
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}

TYPED_TEST (Utf8BadInput, LoneTrailsAfterValidFourByte) {
  // After valid four-byte (F0 9F 92 A9 80)
  auto const output = this->convert (0xF0, 0x9F, 0x92, 0xA9, 0x80);
  std::vector<TypeParam> expected;
  append<code_point::pile_of_poop, TypeParam> (std::back_inserter (expected));
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}

// Truncated sequences
TYPED_TEST (Utf8BadInput, TruncatedSequences) {
  // Two-byte lead (C2)
  EXPECT_THAT (this->convert (0xC2), this->is_expected (1));
  // Three-byte lead (E2)
  EXPECT_THAT (this->convert (0xE2), this->is_expected (1));
  // Three-byte lead and one trail (E2 98)
  EXPECT_THAT (this->convert (0xE2, 0x98), this->is_expected (1));
  // Four-byte lead (F0)
  EXPECT_THAT (this->convert (0xF0), this->is_expected (1));
  // Four-byte lead and one trail (F0 9F)
  EXPECT_THAT (this->convert (0xF0, 0x9F), this->is_expected (1));
  // Four-byte lead and two trails (F0 9F 92)
  EXPECT_THAT (this->convert (0xF0, 0x9F, 0x92), this->is_expected (1));
}

// Leftovers
TYPED_TEST (Utf8BadInput, Leftovers) {
  // FE (FE)
  EXPECT_THAT (this->convert (0xFE), this->is_expected (1));
  // FE and trail (FE 80)
  EXPECT_THAT (this->convert (0xFE, 0x80), this->is_expected (2));
  // FF (FF)
  EXPECT_THAT (this->convert (0xFF), this->is_expected (1));
  // FF and trail (FF 80)
  EXPECT_THAT (this->convert (0xFF, 0x80), this->is_expected (2));
}

#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L

// NOLINTNEXTLINE
TYPED_TEST (Utf8, RangesCopy) {
  auto& output = this->output_;

  std::vector<icubaby::char8> src;
  append<code_point::hiragana_letter_ko, icubaby::char8> (std::back_inserter (src));
  append<code_point::hiragana_letter_n, icubaby::char8> (std::back_inserter (src));
  append<code_point::hiragana_letter_ni, icubaby::char8> (std::back_inserter (src));
  append<code_point::hiragana_letter_ti, icubaby::char8> (std::back_inserter (src));
  append<code_point::hiragana_letter_ha, icubaby::char8> (std::back_inserter (src));
  append<code_point::cjk_unified_ideograph_4e16, icubaby::char8> (std::back_inserter (src));
  append<code_point::cjk_unified_ideograph_754c, icubaby::char8> (std::back_inserter (src));
  append<code_point::line_feed, icubaby::char8> (std::back_inserter (src));

  std::ranges::copy (src | icubaby::ranges::transcode<char8_t, TypeParam>, std::back_inserter (output));

  std::vector<TypeParam> expected;
  append<code_point::hiragana_letter_ko, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_n, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_ni, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_ti, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_ha, TypeParam> (std::back_inserter (expected));
  append<code_point::cjk_unified_ideograph_4e16, TypeParam> (std::back_inserter (expected));
  append<code_point::cjk_unified_ideograph_754c, TypeParam> (std::back_inserter (expected));
  append<code_point::line_feed, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf8, RangesBadInput) {
  auto& output = this->output_;

  std::array const bad_input{static_cast<icubaby::char8> (0xC3), static_cast<icubaby::char8> (0x28)};
  auto r = bad_input | icubaby::ranges::transcode<char8_t, TypeParam>;
  std::ranges::copy (r, std::back_inserter (output));
  EXPECT_FALSE (r.well_formed ());

  std::vector<TypeParam> expected;
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ContainerEq (expected));
}

#endif  // __cpp_lib_ranges

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
TEST (Utf8To32, RangesBadInput) {
  // clang-format off
  std::vector const in{
    char8_t{0xF3}, char8_t{0x81},
  };
  // clang-format on
  std::vector<char32_t> out32;
  auto const r = in | icubaby::ranges::transcode<char8_t, char32_t>;
  std::ranges::copy (r, std::back_inserter (out32));
  EXPECT_THAT (out32, ElementsAre (char32_t{icubaby::replacement_char}));
  EXPECT_FALSE (r.well_formed ());
}
#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#if ICUBABY_FUZZTEST

template <typename OutputEncoding>
static std::tuple<std::vector<OutputEncoding>, bool> Manual (std::vector<icubaby::char8> const& input) {
  std::vector<OutputEncoding> manout;
  icubaby::transcoder<icubaby::char8, OutputEncoding> t;
  auto out = std::back_inserter (manout);
  for (auto const c : input) {
    out = t (c, out);
  }
  t.end_cp (out);
  return std::make_tuple (std::move (manout), t.well_formed ());
}

template <typename OutputEncoding> static void ManualAndIteratorAlwaysMatch (std::vector<icubaby::char8> const& input) {
  // Do the conversion manually...
  auto const [man_out, man_well_formed] = Manual<OutputEncoding> (input);
  // Use the iterator interface to perform the conversion...
  std::vector<OutputEncoding> it_out;
  icubaby::transcoder<icubaby::char8, OutputEncoding> it_t16;
  it_t16.end_cp (
      std::copy (std::begin (input), std::end (input), icubaby::iterator{&it_t16, std::back_inserter (it_out)}));
  EXPECT_EQ (man_well_formed, it_t16.well_formed ());
  EXPECT_THAT (man_out, ContainerEq (it_out));
}

static void ManualAndIteratorAlwaysMatch8 (std::vector<icubaby::char8> const& input) {
  ManualAndIteratorAlwaysMatch<icubaby::char8> (input);
}
FUZZ_TEST (T8, ManualAndIteratorAlwaysMatch8);

static void ManualAndIteratorAlwaysMatch16 (std::vector<icubaby::char8> const& input) {
  ManualAndIteratorAlwaysMatch<char16_t> (input);
}
FUZZ_TEST (T8, ManualAndIteratorAlwaysMatch16);

static void ManualAndIteratorAlwaysMatch32 (std::vector<icubaby::char8> const& input) {
  ManualAndIteratorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T8, ManualAndIteratorAlwaysMatch32);

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

template <typename OutputEncoding>
static void ManualAndRangeAdaptorAlwaysMatch (std::vector<icubaby::char8> const& input) {
  // Do the conversion manually...
  auto const [man_out, man_well_formed] = Manual<OutputEncoding> (input);
  // Use the range adaptor interface to perform the conversion...
  std::vector<OutputEncoding> rng_out;
  auto r = input | icubaby::ranges::transcode<icubaby::char8, OutputEncoding>;
  std::ranges::copy (r, std::back_inserter (rng_out));
  EXPECT_EQ (man_well_formed, r.well_formed ());
  EXPECT_THAT (rng_out, ContainerEq (man_out));
}

static void ManualAndRangeAdaptorAlwaysMatch8 (std::vector<icubaby::char8> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<icubaby::char8> (input);
}
FUZZ_TEST (T8, ManualAndRangeAdaptorAlwaysMatch8);

static void ManualAndRangeAdaptorAlwaysMatch16 (std::vector<icubaby::char8> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<char16_t> (input);
}
FUZZ_TEST (T8, ManualAndRangeAdaptorAlwaysMatch16);

static void ManualAndRangeAdaptorAlwaysMatch32 (std::vector<icubaby::char8> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T8, ManualAndRangeAdaptorAlwaysMatch32);

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#endif  // ICUBABY_FUZZTEST

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
