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
#include <gtest/gtest.h>

#include "encoded_char.hpp"
#include "icubaby/icubaby.hpp"
#include "typed_test.hpp"

// NOLINTNEXTLINE
TEST (IsSurrogate, High) {
  EXPECT_FALSE (icubaby::is_high_surrogate (icubaby::first_high_surrogate - 1));
  EXPECT_TRUE (icubaby::is_high_surrogate (icubaby::first_high_surrogate));
  EXPECT_TRUE (icubaby::is_high_surrogate (icubaby::last_high_surrogate));
  EXPECT_FALSE (icubaby::is_high_surrogate (icubaby::last_high_surrogate + 1));
}
// NOLINTNEXTLINE
TEST (IsSurrogate, Low) {
  EXPECT_FALSE (icubaby::is_low_surrogate (icubaby::first_low_surrogate - 1));
  EXPECT_TRUE (icubaby::is_low_surrogate (icubaby::first_low_surrogate));
  EXPECT_TRUE (icubaby::is_low_surrogate (icubaby::last_low_surrogate));
  EXPECT_FALSE (icubaby::is_low_surrogate (icubaby::last_low_surrogate + 1));
}
// NOLINTNEXTLINE
TEST (IsSurrogate, Any) {
  EXPECT_FALSE (icubaby::is_surrogate (icubaby::first_high_surrogate - 1));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::first_high_surrogate));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::last_high_surrogate));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::last_high_surrogate + 1));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::first_low_surrogate - 1));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::first_low_surrogate));
  EXPECT_TRUE (icubaby::is_surrogate (icubaby::last_low_surrogate));
  EXPECT_FALSE (icubaby::is_surrogate (icubaby::last_low_surrogate + 1));
}
// NOLINTNEXTLINE
TEST (IsCodePointStart, Utf8) {
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b0000'0000)))
      << "Single byte code point";
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b0111'1111)))
      << "Single byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1100'0000)))
      << "First byte of a two byte code point";
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1101'1111)))
      << "First byte of a two byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1110'0000)))
      << "First byte of a three byte code point";
  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1110'1111)))
      << "First byte of a three byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1111'0000)))
      << "First byte of a four byte code point";

  EXPECT_TRUE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1111'0111)))
      << "First byte of a four byte code point";

  EXPECT_FALSE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1000'0000)));
  EXPECT_FALSE (
      icubaby::is_code_point_start (static_cast<icubaby::char8> (0b1011'1111)));
}
// NOLINTNEXTLINE
TEST (IsCodePointStart, Utf16) {
  EXPECT_TRUE (icubaby::is_code_point_start (char16_t{0x0000}));
  EXPECT_TRUE (icubaby::is_code_point_start (char16_t{0x0000}));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::first_high_surrogate - 1)));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::first_high_surrogate)));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::last_high_surrogate)));
  EXPECT_FALSE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::first_low_surrogate)));
  EXPECT_FALSE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::last_low_surrogate)));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char16_t> (icubaby::last_low_surrogate + 1)));
  EXPECT_TRUE (icubaby::is_code_point_start (char16_t{0xFFFF}));
}
// NOLINTNEXTLINE
TEST (IsCodePointStart, Utf32) {
  EXPECT_TRUE (icubaby::is_code_point_start (char32_t{0}));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char32_t> (icubaby::first_high_surrogate - 1)));
  EXPECT_FALSE (icubaby::is_code_point_start (icubaby::first_high_surrogate));
  EXPECT_FALSE (icubaby::is_code_point_start (icubaby::last_low_surrogate));
  EXPECT_TRUE (icubaby::is_code_point_start (
      static_cast<char32_t> (icubaby::last_low_surrogate + 1)));
  EXPECT_TRUE (icubaby::is_code_point_start (icubaby::max_code_point));
  EXPECT_FALSE (icubaby::is_code_point_start (
      static_cast<char32_t> (icubaby::max_code_point + 1)));
  EXPECT_FALSE (icubaby::is_code_point_start (char32_t{0xFFFF'FFFF}));
}

namespace {

struct AsciiUtf8 : testing::Test {
  AsciiUtf8 () : CUs{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'} {}
  std::vector<icubaby::char8> const CUs;
};

}  // end anonymous namespace

// NOLINTNEXTLINE
TEST_F (AsciiUtf8, Length) {
  EXPECT_EQ (11U, icubaby::length (std::begin (CUs), std::end (CUs)));
}
// NOLINTNEXTLINE
TEST_F (AsciiUtf8, Index) {
  auto begin = std::begin (CUs);
  auto end = std::end (CUs);
  auto it = begin;
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{0}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{1}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{2}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{3}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{4}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{5}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{6}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{7}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{8}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{9}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{10}));
  std::advance (it, 1);
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{11}));

  EXPECT_EQ (end, icubaby::index (begin, end, size_t{12}));
  EXPECT_EQ (end,
             icubaby::index (begin, end, std::numeric_limits<size_t>::max ()));
}

namespace {

template <typename T>
struct Hiragana : testing::Test {
  Hiragana () {
    auto it =
        append<code_point::hiragana_letter_o, T> (std::back_inserter (CUs));
    it = append<code_point::hiragana_letter_ha, T> (it);
    it = append<code_point::hiragana_letter_yo, T> (it);
    it = append<code_point::hiragana_letter_u, T> (it);
    it = append<code_point::hiragana_letter_go, T> (it);
    it = append<code_point::hiragana_letter_za, T> (it);
    it = append<code_point::hiragana_letter_i, T> (it);
    it = append<code_point::hiragana_letter_ma, T> (it);
    append<code_point::hiragana_letter_su, T> (it);
  }
  std::vector<T> CUs;
};

}  // end anonymous namespace

TYPED_TEST_SUITE (Hiragana, OutputTypes, OutputTypeNames);
// NOLINTNEXTLINE
TYPED_TEST (Hiragana, Length) {
  EXPECT_EQ (9U,
             icubaby::length (std::begin (this->CUs), std::end (this->CUs)));
}
// NOLINTNEXTLINE
TYPED_TEST (Hiragana, Index) {
  auto begin = std::begin (this->CUs);
  auto end = std::end (this->CUs);
  auto it = begin;
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{0}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_o, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{1}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_ha, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{2}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_yo, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{3}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_u, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{4}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_go, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{5}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_za, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{6}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_i, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{7}));
  std::advance (
      it,
      encoded_char<code_point::hiragana_letter_ma, TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{8}));

  EXPECT_EQ (end, icubaby::index (begin, end, size_t{9}));
}

namespace {

template <typename T>
struct CjkUnifiedIdeographCodePoints : testing::Test {
  CjkUnifiedIdeographCodePoints () {
    auto it = append<code_point::cjk_unified_ideograph_2070e, T> (
        std::back_inserter (CUs));
    it = append<code_point::cjk_unified_ideograph_20731, T> (it);
    it = append<code_point::cjk_unified_ideograph_20779, T> (it);
    append<code_point::cjk_unified_ideograph_20c53, T> (it);
  }

  std::vector<T> CUs;
};

}  // end anonymous namespace

TYPED_TEST_SUITE (CjkUnifiedIdeographCodePoints, OutputTypes, OutputTypeNames);
// NOLINTNEXTLINE
TYPED_TEST (CjkUnifiedIdeographCodePoints, Length) {
  EXPECT_EQ (icubaby::length (std::begin (this->CUs), std::end (this->CUs)),
             4U);
}
// NOLINTNEXTLINE
TYPED_TEST (CjkUnifiedIdeographCodePoints, Index) {
  auto begin = std::begin (this->CUs);
  auto end = std::end (this->CUs);
  auto it = begin;
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{0}));
  std::advance (it, encoded_char<code_point::cjk_unified_ideograph_2070e,
                                 TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{1}));
  std::advance (it, encoded_char<code_point::cjk_unified_ideograph_20731,
                                 TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{2}));
  std::advance (it, encoded_char<code_point::cjk_unified_ideograph_20779,
                                 TypeParam>::value.size ());
  EXPECT_EQ (it, icubaby::index (begin, end, size_t{3}));

  EXPECT_EQ (end, icubaby::index (begin, end, size_t{4}));
}
