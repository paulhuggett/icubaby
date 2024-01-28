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

#ifndef ICUBABY_FUZZTEST
#define ICUBABY_FUZZTEST (0)
#endif
#if ICUBABY_FUZZTEST
#include "fuzztest/fuzztest.h"
#endif

// Local includes
#include "encoded_char.hpp"
#include "typed_test.hpp"

static_assert (std::is_same_v<icubaby::t32_8 ::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_8 ::output_type, icubaby::char8>);
static_assert (std::is_same_v<icubaby::t32_16::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_16::output_type, char16_t>);
// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t32_32::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_32::output_type, char32_t>);

using namespace std::string_literals;
using testing::ElementsAreArray;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

namespace {

template <typename T> class Utf32 : public testing::Test {
protected:
  using output_type = T;
  std::vector<output_type> output_;
  icubaby::transcoder<char32_t, output_type> transcoder_;
};

}  // end anonymous namespace

TYPED_TEST_SUITE (Utf32, OutputTypes, OutputTypeNames);
// NOLINTNEXTLINE
TYPED_TEST (Utf32, GoodDollarSign) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto it = transcoder (static_cast<char32_t> (code_point::dollar_sign), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ()) << "input should be well formed";
  EXPECT_FALSE (transcoder.partial ()) << "there were no surrogate code units";
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::dollar_sign, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, StartOfHeadingAndText) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char32_t> (code_point::start_of_heading), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  it = transcoder (static_cast<char32_t> (code_point::start_of_text), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<code_point::start_of_heading, TypeParam> (std::back_inserter (expected));
  append<code_point::start_of_text, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, CharFFFF) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char32_t> (code_point::code_point_ffff), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::code_point_ffff, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, FirstHighSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  auto it = transcoder (icubaby::first_high_surrogate, std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, LastHighSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  auto it = transcoder (icubaby::last_high_surrogate, std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, FirstLowSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  auto it = transcoder (icubaby::first_low_surrogate, std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, LastLowSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  auto it = transcoder (icubaby::last_low_surrogate, std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, MaxCodePoint) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  auto it = transcoder (icubaby::max_code_point, std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::last_valid_code_point, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, BeyondMaxCodePoint) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  auto it = transcoder (static_cast<char32_t> (static_cast<std::uint_least32_t> (icubaby::max_code_point) + 1U),
                        std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

// NOLINTNEXTLINE
TYPED_TEST (Utf32, RangesCopy) {
  auto& output = this->output_;

  std::vector const in{
      static_cast<char32_t> (code_point::cjk_unified_ideograph_2070e),
      static_cast<char32_t> (code_point::code_point_ffff),
      static_cast<char32_t> (code_point::cuneiform_sign_uru_times_ki),
      static_cast<char32_t> (code_point::dollar_sign),
      static_cast<char32_t> (code_point::hiragana_letter_go),
      static_cast<char32_t> (code_point::hiragana_letter_ha),
      static_cast<char32_t> (code_point::hiragana_letter_i),
      static_cast<char32_t> (code_point::hiragana_letter_ma),
      static_cast<char32_t> (code_point::hiragana_letter_o),
      static_cast<char32_t> (code_point::hiragana_letter_su),
      static_cast<char32_t> (code_point::hiragana_letter_u),
      static_cast<char32_t> (code_point::hiragana_letter_yo),
      static_cast<char32_t> (code_point::hiragana_letter_za),
      static_cast<char32_t> (code_point::linear_b_syllable_b008_a),
      static_cast<char32_t> (code_point::start_of_heading),
      static_cast<char32_t> (code_point::start_of_text),
  };

  auto r = in | icubaby::ranges::transcode<char32_t, TypeParam>;
  std::ranges::copy (r, std::back_inserter (output));

  std::vector<TypeParam> expected;
  append<code_point::cjk_unified_ideograph_2070e, TypeParam> (std::back_inserter (expected));
  append<code_point::code_point_ffff, TypeParam> (std::back_inserter (expected));
  append<code_point::cuneiform_sign_uru_times_ki, TypeParam> (std::back_inserter (expected));
  append<code_point::dollar_sign, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_go, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_ha, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_i, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_ma, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_o, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_su, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_u, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_yo, TypeParam> (std::back_inserter (expected));
  append<code_point::hiragana_letter_za, TypeParam> (std::back_inserter (expected));
  append<code_point::linear_b_syllable_b008_a, TypeParam> (std::back_inserter (expected));
  append<code_point::start_of_heading, TypeParam> (std::back_inserter (expected));
  append<code_point::start_of_text, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));
  EXPECT_TRUE (r.well_formed ());
}
// NOLINTNEXTLINE
TYPED_TEST (Utf32, RangesBadInput) {
  auto& output = this->output_;
  std::vector const in{char32_t{0xFFFFFFFF}};
  auto const r = in | icubaby::ranges::transcode<char32_t, TypeParam>;
  std::ranges::copy (r, std::back_inserter (output));
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
  EXPECT_FALSE (r.well_formed ());
}

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#if ICUBABY_FUZZTEST

template <typename OutputEncoding>
static std::tuple<std::vector<OutputEncoding>, bool> Manual (std::vector<char32_t> const& input) {
  std::vector<OutputEncoding> man_out;
  icubaby::transcoder<char32_t, OutputEncoding> t;
  auto out = std::back_inserter (man_out);
  for (auto const c : input) {
    out = t (c, out);
  }
  t.end_cp (out);
  return std::make_tuple (std::move (man_out), t.well_formed ());
}

template <typename OutputEncoding> static void ManualAndIteratorAlwaysMatch (std::vector<char32_t> const& input) {
  // Do the conversion manually...
  auto const [man_out, man_well_formed] = Manual<OutputEncoding> (input);
  // Use the iterator interface to perform the conversion...
  std::vector<OutputEncoding> it_out;
  icubaby::transcoder<char32_t, OutputEncoding> t;
  t.end_cp (std::copy (std::begin (input), std::end (input), icubaby::iterator{&t, std::back_inserter (it_out)}));
  EXPECT_EQ (man_well_formed, t.well_formed ());
  EXPECT_THAT (man_out, testing::ContainerEq (it_out));
}

static void ManualAndIteratorAlwaysMatch8 (std::vector<char32_t> const& input) {
  ManualAndIteratorAlwaysMatch<icubaby::char8> (input);
}
FUZZ_TEST (T32, ManualAndIteratorAlwaysMatch8);

static void ManualAndIteratorAlwaysMatch16 (std::vector<char32_t> const& input) {
  ManualAndIteratorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T32, ManualAndIteratorAlwaysMatch16);

static void ManualAndIteratorAlwaysMatch32 (std::vector<char32_t> const& input) {
  ManualAndIteratorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T32, ManualAndIteratorAlwaysMatch32);

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

template <typename OutputEncoding> static void ManualAndRangeAdaptorAlwaysMatch (std::vector<char32_t> const& input) {
  // Do the conversion manually...
  auto const [man_out, man_well_formed] = Manual<OutputEncoding> (input);
  // Use the range adaptor interface to perform the conversion...
  std::vector<OutputEncoding> rng_out;
  auto r = input | icubaby::ranges::transcode<char32_t, OutputEncoding>;
  std::ranges::copy (r, std::back_inserter (rng_out));
  EXPECT_EQ (man_well_formed, r.well_formed ());
  EXPECT_THAT (man_out, testing::ContainerEq (rng_out));
}

static void ManualAndRangeAdaptorAlwaysMatch8 (std::vector<char32_t> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<icubaby::char8> (input);
}
FUZZ_TEST (T32, ManualAndRangeAdaptorAlwaysMatch8);

static void ManualAndRangeAdaptorAlwaysMatch16 (std::vector<char32_t> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T32, ManualAndRangeAdaptorAlwaysMatch16);

static void ManualAndRangeAdaptorAlwaysMatch32 (std::vector<char32_t> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T32, ManualAndRangeAdaptorAlwaysMatch32);

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#endif  // ICUBABY_FUZZTEST

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
