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

static_assert (std::is_same_v<icubaby::t16_8 ::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_8 ::output_type, icubaby::char8>);
// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t16_16::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t16_32::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_32::output_type, char32_t>);

// TODO: Remove this code!
// A specialization of the gtest GetTypeName<char8_t>() function. This is
// required for compiling with Xcode 14.1 where we have a link error due to
// missing typeinfo for char8_t.
#if defined(__cpp_char8_t) && defined(__cpp_lib_char8_t)
namespace testing {
namespace internal {

template <>
std::string GetTypeName<char8_t> () {
  return "char8_t";
}

}  // end namespace internal
}  // end namespace testing
#endif

using namespace std::string_literals;
using testing::ElementsAre;
using testing::ElementsAreArray;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

namespace {

constexpr auto start_of_heading = char32_t{0x001};
constexpr auto start_of_text = char32_t{0x002};
constexpr auto replacement_char = icubaby::replacement_char;
constexpr auto dollar_sign = char32_t{0x0024};
constexpr auto code_point_ffff = char32_t{0xffff};
constexpr auto linear_b_syllable_b008_a = char32_t{0x10000};
constexpr auto cuneiform_sign_uru_times_ki = char32_t{0x12345};
constexpr auto last_valid_code_point = char32_t{0x10ffff};

template <char32_t C, typename To>
struct encoded_char;

template <>
struct encoded_char<start_of_heading, char32_t> {
  static constexpr std::array<char32_t, 1> value = {start_of_heading};
};
template <>
struct encoded_char<start_of_heading, char16_t> {
  static constexpr std::array<char16_t, 1> value = {
      static_cast<char16_t> (start_of_heading)};
};
template <>
struct encoded_char<start_of_heading, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 1> value = {
      static_cast<icubaby::char8> (start_of_heading)};
};

template <>
struct encoded_char<start_of_text, char32_t> {
  static constexpr std::array<char32_t, 1> value = {start_of_text};
};
template <>
struct encoded_char<start_of_text, char16_t> {
  static constexpr std::array<char16_t, 1> value = {
      static_cast<char16_t> (start_of_text)};
};
template <>
struct encoded_char<start_of_text, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 1> value = {
      static_cast<icubaby::char8> (start_of_text)};
};

template <>
struct encoded_char<dollar_sign, char32_t> {
  static constexpr std::array<char32_t, 1> value = {dollar_sign};
};
template <>
struct encoded_char<dollar_sign, char16_t> {
  static constexpr std::array<char16_t, 1> value = {
      static_cast<char16_t> (dollar_sign)};
};
template <>
struct encoded_char<dollar_sign, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 1> value = {
      static_cast<icubaby::char8> (dollar_sign)};
};

template <>
struct encoded_char<replacement_char, char32_t> {
  static constexpr std::array<char32_t, 1> value = {replacement_char};
};
template <>
struct encoded_char<replacement_char, char16_t> {
  static constexpr std::array<char16_t, 1> value = {
      static_cast<char16_t> (replacement_char)};
};
template <>
struct encoded_char<replacement_char, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xEF), static_cast<icubaby::char8> (0xBF),
      static_cast<icubaby::char8> (0xBD)};
};

template <>
struct encoded_char<code_point_ffff, char32_t> {
  static constexpr std::array<char32_t, 1> value = {code_point_ffff};
};
template <>
struct encoded_char<code_point_ffff, char16_t> {
  static constexpr std::array<char16_t, 1> value = {
      static_cast<char16_t> (code_point_ffff)};
};
template <>
struct encoded_char<code_point_ffff, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value{
      static_cast<icubaby::char8> (0xef), static_cast<icubaby::char8> (0xbf),
      static_cast<icubaby::char8> (0xbf)};
};

template <>
struct encoded_char<linear_b_syllable_b008_a, char32_t> {
  static constexpr std::array<char32_t, 1> value = {linear_b_syllable_b008_a};
};
template <>
struct encoded_char<linear_b_syllable_b008_a, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd800},
                                                    char16_t{0xdc00}};
};
template <>
struct encoded_char<linear_b_syllable_b008_a, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value{
      static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x90),
      static_cast<icubaby::char8> (0x80), static_cast<icubaby::char8> (0x80)};
};

template <>
struct encoded_char<cuneiform_sign_uru_times_ki, char32_t> {
  static constexpr std::array<char32_t, 1> value = {
      cuneiform_sign_uru_times_ki};
};
template <>
struct encoded_char<cuneiform_sign_uru_times_ki, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd808},
                                                    char16_t{0xdf45}};
};
template <>
struct encoded_char<cuneiform_sign_uru_times_ki, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0x92),
      static_cast<icubaby::char8> (0x8d), static_cast<icubaby::char8> (0x85)};
};

template <>
struct encoded_char<last_valid_code_point, char32_t> {
  static constexpr std::array<char32_t, 1> value = {last_valid_code_point};
};
template <>
struct encoded_char<last_valid_code_point, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xdbff},
                                                    char16_t{0xdfff}};
};
template <>
struct encoded_char<last_valid_code_point, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf4), static_cast<icubaby::char8> (0x8f),
      static_cast<icubaby::char8> (0xbf), static_cast<icubaby::char8> (0xbf)};
};

template <char32_t C, typename To>
inline constexpr auto encoded_char_v = encoded_char<C, To>::value;

template <char32_t C, typename To, typename OutputIterator>
OutputIterator append (OutputIterator out) {
  constexpr auto& code_units = encoded_char_v<C, To>;
  return std::copy (std::begin (code_units), std::end (code_units), out);
}

template <typename T>
class Utf16 : public testing::Test {
protected:
  std::vector<T> output_;
  icubaby::transcoder<char16_t, T> transcoder_;
};

[[noreturn, maybe_unused]] inline void unreachable () {
  // Uses compiler specific extensions if possible.
  // Even if no extension is used, undefined behavior is still raised by
  // an empty function body and the noreturn attribute.
#ifdef __GNUC__  // GCC, Clang, ICC
  __builtin_unreachable ();
#elif defined (_MSC_VER)  // MSVC
  __assume (false);
#endif
}

/// A type that is always false. Used to improve the failure mesages from
/// static_assert().
template <typename... T>
[[maybe_unused]] constexpr bool always_false = false;

class OutputTypeNames {
public:
  template <typename T>
  static std::string GetName (int index) {
    (void)index;
    if constexpr (std::is_same<T, icubaby::char8> ()) {
      return "icubaby::char8"s;
    } else if constexpr (std::is_same<T, char16_t> ()) {
      return "char16_t"s;
    } else if constexpr (std::is_same<T, char32_t> ()) {
      return "char32_t"s;
    } else {
      static_assert (always_false<T>, "non-exhaustive visitor");
      unreachable ();
    }
  }
};

}  // end anonymous namespace

using OutputTypes = testing::Types<icubaby::char8, char16_t, char32_t>;
TYPED_TEST_SUITE (Utf16, OutputTypes, OutputTypeNames);
// NOLINTNEXTLINE
TYPED_TEST (Utf16, GoodDollarSign) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto it = transcoder (static_cast<char16_t> (dollar_sign),
                        std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ()) << "input should be well formed";
  EXPECT_FALSE (transcoder.partial ()) << "there were no surrogate code units";
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output,
               ElementsAreArray (encoded_char_v<dollar_sign, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, StartOfHeadingAndText) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char16_t> (start_of_heading),
                        std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  it = transcoder (static_cast<char16_t> (start_of_text), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<start_of_heading, TypeParam> (std::back_inserter (expected));
  append<start_of_text, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, CharFFFF) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char16_t> (code_point_ffff),
                        std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output,
               ElementsAreArray (encoded_char_v<code_point_ffff, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, FirstHighLowSurrogatePair) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  static constexpr auto code_units =
      std::make_pair (static_cast<char16_t> (icubaby::first_high_surrogate),
                      static_cast<char16_t> (icubaby::first_low_surrogate));

  static_assert (icubaby::is_high_surrogate (std::get<0> (code_units)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (code_units)));

  auto it = transcoder (std::get<0> (code_units), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ())
      << "input is well formed after just a high surrogate";
  EXPECT_TRUE (transcoder.partial ())
      << "partial() should be true after a high surrogate";
  EXPECT_TRUE (output.empty ())
      << "there should be no output after a high surrogate";
  it = transcoder (std::get<1> (code_units), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ())
      << "partial() should be false after a high/low surrogate pair";
  EXPECT_THAT (
      output,
      ElementsAreArray (encoded_char_v<linear_b_syllable_b008_a, TypeParam>));
  transcoder.end_cp (it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (
      output,
      ElementsAreArray (encoded_char_v<linear_b_syllable_b008_a, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighLowSurrogatePairExample) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;

  constexpr auto const& char1 =
      encoded_char_v<cuneiform_sign_uru_times_ki, char16_t>;
  static_assert (char1.size () == 2U);
  static_assert (icubaby::is_high_surrogate (std::get<0> (char1)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (char1)));

  constexpr auto const& char2 = encoded_char_v<last_valid_code_point, char16_t>;
  static_assert (char2.size () == 2U);
  static_assert (icubaby::is_high_surrogate (std::get<0> (char2)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (char2)));

  std::vector<TypeParam> expected;
  auto it = transcoder (std::get<0> (char1), std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ())
      << "a high surrogate means we have a partial code point";
  EXPECT_TRUE (output.empty ())
      << "there should be no output after a high surrogate";

  it = transcoder (std::get<1> (char1), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  append<cuneiform_sign_uru_times_ki, TypeParam> (
      std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));

  // Repeat the pattern for the second CU pair.
  it = transcoder (std::get<0> (char2), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  it = transcoder (std::get<1> (char2), it);
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  append<last_valid_code_point, TypeParam> (std::back_inserter (expected));
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
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate),
                        std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  transcoder (static_cast<char16_t> (dollar_sign), it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<replacement_char, TypeParam> (std::back_inserter (expected));
  append<dollar_sign, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighSurrogateFollowedbyAnotherHigh) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate),
                        std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate), it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_THAT (output,
               ElementsAreArray (encoded_char_v<replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, HighSurrogateFollowedByHighLowPair) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate),
                        std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());

  static_assert (icubaby::is_high_surrogate (
      std::get<0> (encoded_char_v<linear_b_syllable_b008_a, char16_t>)));
  it = transcoder (
      std::get<0> (encoded_char_v<linear_b_syllable_b008_a, char16_t>), it);
  EXPECT_FALSE (transcoder.well_formed ())
      << "high followed by high is not well formed input";
  EXPECT_TRUE (transcoder.partial ())
      << "partial() should be true after a high surrogate";
  EXPECT_THAT (output,
               ElementsAreArray (encoded_char_v<replacement_char, TypeParam>));

  static_assert (icubaby::is_low_surrogate (
      std::get<1> (encoded_char_v<linear_b_syllable_b008_a, char16_t>)));
  it = transcoder (
      std::get<1> (encoded_char_v<linear_b_syllable_b008_a, char16_t>), it);
  EXPECT_FALSE (transcoder.well_formed ())
      << "high followed by high is not well formed input";
  EXPECT_FALSE (transcoder.partial ())
      << "we saw high followed by low: a complete code point";

  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  std::vector<TypeParam> expected;
  append<replacement_char, TypeParam> (std::back_inserter (expected));
  append<linear_b_syllable_b008_a, TypeParam> (std::back_inserter (expected));
  EXPECT_THAT (output, ElementsAreArray (expected));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, LonelyLowSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto it = transcoder (static_cast<char16_t> (icubaby::first_low_surrogate),
                        std::back_inserter (output));
  EXPECT_FALSE (transcoder.well_formed ())
      << "a low surrogate must be preceeded by a high";
  EXPECT_FALSE (transcoder.partial ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_THAT (output,
               ElementsAreArray (encoded_char_v<replacement_char, TypeParam>));
}
// NOLINTNEXTLINE
TYPED_TEST (Utf16, LonelyHighSurrogate) {
  auto& transcoder = this->transcoder_;
  auto& output = this->output_;
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());
  auto it = transcoder (static_cast<char16_t> (icubaby::first_high_surrogate),
                        std::back_inserter (output));
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (output.empty ());
  transcoder.end_cp (it);
  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_FALSE (transcoder.partial ());

  EXPECT_THAT (output,
               ElementsAreArray (encoded_char_v<replacement_char, TypeParam>));
}
