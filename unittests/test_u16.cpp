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

using testing::ElementsAre;
using testing::ElementsAreArray;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

namespace {

class Utf16To32 : public testing::Test {
protected:
  std::vector<char32_t> output_;
  icubaby::t16_32 transcoder_;
};

}  // end anonymous namespace

// NOLINTNEXTLINE
TEST_F (Utf16To32, Char1) {
  EXPECT_TRUE (transcoder_.well_formed ());

  // Explicitly state the expected type of the operator() return value just this
  // first time.
  std::back_insert_iterator<decltype (output_)> it =
      transcoder_ (char16_t{1}, std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ()) << "input was well formed";
  EXPECT_FALSE (transcoder_.partial ()) << "there were no surrogate code units";
  EXPECT_THAT (output_, ElementsAre (char32_t{1}))
      << "transcoder should have written a single char32_t{1} to the output "
         "container";
  transcoder_.end_cp (it);
  EXPECT_TRUE (transcoder_.well_formed ()) << "input was well formed";
  EXPECT_FALSE (transcoder_.partial ()) << "there were no surrogate code units";

  *(it++) = char32_t{2};
  EXPECT_THAT (output_, ElementsAre (char32_t{1}, char32_t{2}))
      << "Check for operator() return iterator failed";
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, Char2) {
  auto it = transcoder_ (char16_t{2}, std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  transcoder_.end_cp (it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (char32_t{2}));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, Char1AndChar2) {
  auto it = transcoder_ (char16_t{1}, std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  it = transcoder_ (char16_t{2}, it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  transcoder_.end_cp (it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (char32_t{1}, char32_t{2}));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, CharFFFF) {
  auto it = transcoder_ (char16_t{0xffff}, std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  transcoder_.end_cp (it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (char32_t{0xffff}));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, FirstHighLowSurrogatePair) {
  static constexpr auto code_units =
      std::make_pair (static_cast<char16_t> (icubaby::first_high_surrogate),
                      static_cast<char16_t> (icubaby::first_low_surrogate));
  static constexpr auto expected_cp = char32_t{0x10000};

  static_assert (icubaby::is_high_surrogate (std::get<0> (code_units)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (code_units)));

  auto it =
      transcoder_ (std::get<0> (code_units), std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ())
      << "input is well formed after just a high surrogate";
  EXPECT_TRUE (transcoder_.partial ())
      << "partial() should be true after a high surrogate";
  EXPECT_TRUE (output_.empty ())
      << "there should be no output after a high surrogate";
  it = transcoder_ (std::get<1> (code_units), it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ())
      << "partial() should be false after a high/low surrogate pair";
  EXPECT_THAT (output_, ElementsAre (expected_cp))
      << "high/low surrogate pair should yield a single code point";
  transcoder_.end_cp (it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (expected_cp));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, HighLowSurrogatePairExample) {
  static constexpr auto first =
      std::make_pair (char16_t{0xd808}, char16_t{0xdf45});
  static constexpr auto second =
      std::make_pair (char16_t{0xdbff}, char16_t{0xdfff});
  static constexpr std::array<char32_t, 2> expected{char32_t{0x12345},
                                                    char32_t{0x10ffff}};

  static_assert (icubaby::is_high_surrogate (std::get<0> (first)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (first)));
  static_assert (icubaby::is_high_surrogate (std::get<0> (second)));
  static_assert (icubaby::is_low_surrogate (std::get<1> (second)));

  auto it = transcoder_ (std::get<0> (first), std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_TRUE (transcoder_.partial ())
      << "a high surrogate means we have a partial code point";
  EXPECT_TRUE (output_.empty ())
      << "there should be no output after a high surrogate";

  it = transcoder_ (std::get<1> (first), it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAreArray (expected.data (), 1));

  // Repeat the pattern for the second CU pair.
  it = transcoder_ (std::get<0> (second), it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_TRUE (transcoder_.partial ());
  it = transcoder_ (std::get<1> (second), it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAreArray (expected));

  // End of input sequence.
  transcoder_.end_cp (it);
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAreArray (expected));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, HighSurrogateWithoutLow) {
  auto it = transcoder_ (char16_t{0xd800}, std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_TRUE (transcoder_.partial ());
  EXPECT_TRUE (output_.empty ());
  transcoder_ (char16_t{0x0000}, it);
  EXPECT_FALSE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_,
               ElementsAre (icubaby::replacement_char, char32_t{0x0000}));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, HighSurrogateFollowedbyAnotherHigh) {
  auto it =
      transcoder_ (icubaby::first_high_surrogate, std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_TRUE (transcoder_.partial ());
  EXPECT_TRUE (output_.empty ());
  it = transcoder_ (icubaby::first_high_surrogate, it);
  EXPECT_FALSE (transcoder_.well_formed ());
  EXPECT_TRUE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (icubaby::replacement_char));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, HighSurrogateFollowedByHighLowPair) {
  static constexpr std::array<char16_t, 3> code_units{
      char16_t{0xd800}, char16_t{0xd800}, char16_t{0xdc00}};
  static constexpr auto expected_cp = char32_t{0x10000};

  static_assert (icubaby::is_high_surrogate (std::get<0> (code_units)));
  auto it =
      transcoder_ (std::get<0> (code_units), std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_TRUE (transcoder_.partial ());
  EXPECT_TRUE (output_.empty ());

  static_assert (icubaby::is_high_surrogate (std::get<1> (code_units)));
  it = transcoder_ (std::get<1> (code_units), it);
  EXPECT_FALSE (transcoder_.well_formed ())
      << "high followed by high is not well formed input";
  EXPECT_TRUE (transcoder_.partial ())
      << "partial() should be true after a high surrogate";
  EXPECT_THAT (output_, ElementsAre (icubaby::replacement_char));

  static_assert (icubaby::is_low_surrogate (std::get<2> (code_units)));
  it = transcoder_ (std::get<2> (code_units), it);
  EXPECT_FALSE (transcoder_.well_formed ())
      << "high followed by high is not well formed input";
  EXPECT_FALSE (transcoder_.partial ())
      << "we saw high followed by low: a complete code point";

  transcoder_.end_cp (it);
  EXPECT_FALSE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (icubaby::replacement_char, expected_cp));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, LonelyLowSurrogate) {
  auto it = transcoder_ (static_cast<char16_t> (icubaby::first_low_surrogate),
                         std::back_inserter (output_));
  EXPECT_FALSE (transcoder_.well_formed ())
      << "a low surrogate must be preceeded by a high";
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (icubaby::replacement_char));

  transcoder_.end_cp (it);
  EXPECT_FALSE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (icubaby::replacement_char));
}

// NOLINTNEXTLINE
TEST_F (Utf16To32, LonelyHighSurrogate) {
  auto it =
      transcoder_ (icubaby::first_high_surrogate, std::back_inserter (output_));
  EXPECT_TRUE (transcoder_.well_formed ());
  EXPECT_TRUE (transcoder_.partial ());
  EXPECT_TRUE (output_.empty ());
  it = transcoder_.end_cp (it);
  EXPECT_FALSE (transcoder_.well_formed ());
  EXPECT_FALSE (transcoder_.partial ());
  EXPECT_THAT (output_, ElementsAre (icubaby::replacement_char));
}
