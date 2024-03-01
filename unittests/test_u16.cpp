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

static_assert (std::is_same_v<icubaby::t16_8 ::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_8 ::output_type, icubaby::char8>);
// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t16_16::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t16_32::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_32::output_type, char32_t>);

using namespace std::string_literals;
using testing::ElementsAreArray;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

namespace {

template <typename T> class Utf16 : public testing::Test {
protected:
  std::vector<T> output_;
  icubaby::transcoder<char16_t, T> transcoder_;
};

}  // end anonymous namespace

TYPED_TEST_SUITE (Utf16, OutputTypes, OutputTypeNames);
// NOLINTNEXTLINE
TYPED_TEST (Utf16, GoodDollarSign) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto pos = transcoder (static_cast<char16_t> (code_point::dollar_sign), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ()) << "input should be well formed";
  EXPECT_FALSE (transcoder.partial ()) << "there were no surrogate code units";
  (void)transcoder.end_cp (pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::dollar_sign, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, StartOfHeadingAndText) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto pos = transcoder (static_cast<char16_t> (code_point::start_of_heading), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  pos = transcoder (static_cast<char16_t> (code_point::start_of_text), pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  (void)transcoder.end_cp (pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  auto expected_out = std::back_inserter (expected);
  expected_out = append<code_point::start_of_heading, TypeParam> (expected_out);
  (void)append<code_point::start_of_text, TypeParam> (expected_out);
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, CharFFFF) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto pos = transcoder (static_cast<char16_t> (code_point::code_point_ffff), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  (void)transcoder.end_cp (pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::code_point_ffff, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, FirstHighLowSurrogatePair) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  static constexpr auto code_units = std::make_pair (static_cast<char16_t> (icubaby::first_high_surrogate),
                                                     static_cast<char16_t> (icubaby::first_low_surrogate));

  static_assert (icubaby::is_high_surrogate (std::get<0> (code_units)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (code_units)));

  auto pos = transcoder (std::get<0> (code_units), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ()) << "input is well formed after just a high surrogate";
  EXPECT_TRUE (transcoder.partial ()) << "partial() should be true after a high surrogate";
  EXPECT_TRUE (output.empty ()) << "there should be no output after a high surrogate";
  pos = transcoder (std::get<1> (code_units), pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ()) << "partial() should be false after a high/low surrogate pair";
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::linear_b_syllable_b008_a, TypeParam>));
  (void)transcoder.end_cp (pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::linear_b_syllable_b008_a, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighLowSurrogatePairExample) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  constexpr auto const& char1 = encoded_char_v<code_point::cuneiform_sign_uru_times_ki, char16_t>;
  static_assert (char1.size () == 2U);
  static_assert (icubaby::is_high_surrogate (std::get<0> (char1)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (char1)));

  constexpr auto const& char2 = encoded_char_v<code_point::last_valid_code_point, char16_t>;
  static_assert (char2.size () == 2U);
  static_assert (icubaby::is_high_surrogate (std::get<0> (char2)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (char2)));

  std::vector<TypeParam> expected;
  auto pos = transcoder (std::get<0> (char1), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ()) << "a high surrogate means we have a partial code point";
  EXPECT_TRUE (output.empty ()) << "there should be no output after a high surrogate";

  pos = transcoder (std::get<1> (char1), pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  (void)append<code_point::cuneiform_sign_uru_times_ki, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));

  // Repeat the pattern for the second CU pair.
  pos = transcoder (std::get<0> (char2), pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  pos = transcoder (std::get<1> (char2), pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  (void)append<code_point::last_valid_code_point, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));

  // End of input sequence.
  (void)transcoder.end_cp (pos);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighSurrogateWithoutLow) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto pos = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  (void)transcoder (static_cast<char16_t> (code_point::dollar_sign), pos);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  auto expected_out = std::back_inserter (expected);
  expected_out = append<code_point::replacement_char, TypeParam> (expected_out);
  (void)append<code_point::dollar_sign, TypeParam> (expected_out);
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighSurrogateFollowedbyAnotherHigh) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto pos = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  pos = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), pos);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighSurrogateFollowedByHighLowPair) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  static constexpr auto initial_code_unit =
      static_cast<char16_t> (static_cast<std::uint_least16_t> (icubaby::first_high_surrogate) + 1U);
  auto pos = transcoder (initial_code_unit, std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  auto const& linear_b_syllable_b008_a = encoded_char_v<code_point::linear_b_syllable_b008_a, char16_t>;
  ASSERT_TRUE (icubaby::is_high_surrogate (std::get<0> (linear_b_syllable_b008_a)))
      << "The first code unit of linear_b_syllable_b008_a was expected to be a high surrogate";
  ASSERT_TRUE (std::get<0> (linear_b_syllable_b008_a) != initial_code_unit)
      << "Expected our second code unit to be different from the first";
  pos = transcoder (std::get<0> (linear_b_syllable_b008_a), pos);
  EXPECT_FALSE (transcoder.well_formed ()) << "high followed by high is not well formed input";
  EXPECT_TRUE (transcoder.partial ()) << "partial() should be true after a high surrogate";
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));

  ASSERT_TRUE (icubaby::is_low_surrogate (std::get<1> (linear_b_syllable_b008_a)))
      << "The second code unit of linear_b_syllable_b008_a was expected to be a low surrogate";
  pos = transcoder (std::get<1> (linear_b_syllable_b008_a), pos);
  EXPECT_FALSE (transcoder.well_formed ()) << "high followed by high is not well formed input";
  EXPECT_FALSE (transcoder.partial ()) << "we saw high followed by low: a complete code point";

  ASSERT_TRUE (linear_b_syllable_b008_a.size () == 2U)
      << "Expected linear_b_syllable_b008_a to consist of 2 code units";
  (void)transcoder.end_cp (pos);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  auto expected_out = std::back_inserter (expected);
  expected_out = append<code_point::replacement_char, TypeParam> (expected_out);
  (void)append<code_point::linear_b_syllable_b008_a, TypeParam> (expected_out);
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, LonelyLowSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto pos = transcoder (static_cast<char16_t> (icubaby::first_low_surrogate), std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ()) << "a low surrogate must be preceeded by a high";
  EXPECT_FALSE (transcoder.partial ());
  (void)transcoder.end_cp (pos);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, LonelyHighSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto pos = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  (void)transcoder.end_cp (pos);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}

#if ICUBABY_FUZZTEST

template <typename OutputEncoding>
static std::tuple<std::vector<OutputEncoding>, bool> Manual (std::vector<char16_t> const& input) {
  std::vector<OutputEncoding> manout;
  icubaby::transcoder<char16_t, OutputEncoding> transcoder;
  auto out = std::back_inserter (manout);
  for (auto const code_unit : input) {
    out = transcoder (code_unit, out);
  }
  (void)transcoder.end_cp (out);
  return std::make_tuple (std::move (manout), transcoder.well_formed ());
}

template <typename OutputEncoding> static void ManualAndIteratorAlwaysMatch (std::vector<char16_t> const& input) {
  // Do the conversion manually...
  auto const [man_out, man_well_formed] = Manual<OutputEncoding> (input);
  // Use the iterator interface to perform the conversion...
  std::vector<OutputEncoding> it_out;
  icubaby::transcoder<char16_t, OutputEncoding> it_t16;
  (void)it_t16.end_cp (
      std::copy (std::begin (input), std::end (input), icubaby::iterator{&it_t16, std::back_inserter (it_out)}));
  EXPECT_EQ (man_well_formed, it_t16.well_formed ());
  EXPECT_THAT (man_out, testing::ContainerEq (it_out));
}

static void ManualAndIteratorAlwaysMatch8 (std::vector<char16_t> const& input) {
  ManualAndIteratorAlwaysMatch<icubaby::char8> (input);
}
FUZZ_TEST (T16, ManualAndIteratorAlwaysMatch8);

static void ManualAndIteratorAlwaysMatch16 (std::vector<char16_t> const& input) {
  ManualAndIteratorAlwaysMatch<char16_t> (input);
}
FUZZ_TEST (T16, ManualAndIteratorAlwaysMatch16);

static void ManualAndIteratorAlwaysMatch32 (std::vector<char16_t> const& input) {
  ManualAndIteratorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T16, ManualAndIteratorAlwaysMatch32);

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

template <typename OutputEncoding> static void ManualAndRangeAdaptorAlwaysMatch (std::vector<char16_t> const& input) {
  // Do the conversion manually...
  auto const [man_out, man_well_formed] = Manual<OutputEncoding> (input);
  // Use the range adaptor interface to perform the conversion...
  std::vector<OutputEncoding> rng_out;
  auto r = input | icubaby::views::transcode<char16_t, OutputEncoding>;
  std::ranges::copy (r, std::back_inserter (rng_out));
  EXPECT_EQ (man_well_formed, r.well_formed ());
  EXPECT_THAT (rng_out, testing::ContainerEq (man_out));
}

static void ManualAndRangeAdaptorAlwaysMatch8 (std::vector<char16_t> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<icubaby::char8> (input);
}
FUZZ_TEST (T16, ManualAndRangeAdaptorAlwaysMatch8);

static void ManualAndRangeAdaptorAlwaysMatch16 (std::vector<char16_t> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<char16_t> (input);
}
FUZZ_TEST (T16, ManualAndRangeAdaptorAlwaysMatch16);

static void ManualAndRangeAdaptorAlwaysMatch32 (std::vector<char16_t> const& input) {
  ManualAndRangeAdaptorAlwaysMatch<char32_t> (input);
}
FUZZ_TEST (T16, ManualAndRangeAdaptorAlwaysMatch32);

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#endif  // ICUBABY_FUZZTEST

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
