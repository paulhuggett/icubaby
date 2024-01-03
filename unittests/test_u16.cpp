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

#include "encoded_char.hpp"
#include "icubaby/icubaby.hpp"
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
  auto it = transcoder (static_cast<char16_t> (code_point::dollar_sign), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ()) << "input should be well formed";
  EXPECT_FALSE (transcoder.partial ()) << "there were no surrogate code units";
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::dollar_sign, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, StartOfHeadingAndText) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char16_t> (code_point::start_of_heading), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  it = transcoder (static_cast<char16_t> (code_point::start_of_text), it);
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
TYPED_TEST (Utf16, CharFFFF) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char16_t> (code_point::code_point_ffff), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
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

  auto it = transcoder (std::get<0> (code_units), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ()) << "input is well formed after just a high surrogate";
  EXPECT_TRUE (transcoder.partial ()) << "partial() should be true after a high surrogate";
  EXPECT_TRUE (output.empty ()) << "there should be no output after a high surrogate";
  it = transcoder (std::get<1> (code_units), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ()) << "partial() should be false after a high/low surrogate pair";
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::linear_b_syllable_b008_a, TypeParam>));
  transcoder.end_cp (it);
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
  auto it = transcoder (std::get<0> (char1), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ()) << "a high surrogate means we have a partial code point";
  EXPECT_TRUE (output.empty ()) << "there should be no output after a high surrogate";

  it = transcoder (std::get<1> (char1), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  append<code_point::cuneiform_sign_uru_times_ki, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));

  // Repeat the pattern for the second CU pair.
  it = transcoder (std::get<0> (char2), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  it = transcoder (std::get<1> (char2), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  append<code_point::last_valid_code_point, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));

  // End of input sequence.
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighSurrogateWithoutLow) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  transcoder (static_cast<char16_t> (code_point::dollar_sign), it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  append<code_point::dollar_sign, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighSurrogateFollowedbyAnotherHigh) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), it);
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
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  static_assert (
      icubaby::is_high_surrogate (std::get<0> (encoded_char_v<code_point::linear_b_syllable_b008_a, char16_t>)));
  it = transcoder (std::get<0> (encoded_char_v<code_point::linear_b_syllable_b008_a, char16_t>), it);
  EXPECT_FALSE (transcoder.well_formed ()) << "high followed by high is not well formed input";
  EXPECT_TRUE (transcoder.partial ()) << "partial() should be true after a high surrogate";
  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));

  static_assert (
      icubaby::is_low_surrogate (std::get<1> (encoded_char_v<code_point::linear_b_syllable_b008_a, char16_t>)));
  it = transcoder (std::get<1> (encoded_char_v<code_point::linear_b_syllable_b008_a, char16_t>), it);
  EXPECT_FALSE (transcoder.well_formed ()) << "high followed by high is not well formed input";
  EXPECT_FALSE (transcoder.partial ()) << "we saw high followed by low: a complete code point";

  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<code_point::replacement_char, TypeParam> (std::back_inserter (expected));
  append<code_point::linear_b_syllable_b008_a, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, LonelyLowSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto it = transcoder (static_cast<char16_t> (icubaby::first_low_surrogate), std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ()) << "a low surrogate must be preceeded by a high";
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
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
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  EXPECT_THAT (output, ElementsAreArray (encoded_char_v<code_point::replacement_char, TypeParam>));
}
