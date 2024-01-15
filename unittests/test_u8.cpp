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
  out = transcoder (0x24, out);
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
  it = transcoder (0xED, it);
  it = transcoder (0xA0, it);
  it = transcoder (0x80, it);
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
  it = transcoder (0xC2, it);
  it = transcoder (0x80, it);
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

  constexpr auto& cent_sign = encoded_char<code_point::cent_sign, icubaby::char8>::value;
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

  constexpr auto& devanagri_letter_ha = encoded_char<code_point::devanagri_letter_ha, icubaby::char8>::value;
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

  constexpr auto& gothic_letter_hwair = encoded_char<code_point::gothic_letter_hwair, icubaby::char8>::value;
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
