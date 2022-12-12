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
#ifndef ENCODED_CHAR_HPP
#define ENCODED_CHAR_HPP

#include <array>

#include "icubaby/icubaby.hpp"

constexpr auto start_of_heading = char32_t{0x001};
constexpr auto start_of_text = char32_t{0x002};
constexpr auto replacement_char = icubaby::replacement_char;
constexpr auto dollar_sign = char32_t{0x0024};
constexpr auto code_point_ffff = char32_t{0xffff};
constexpr auto linear_b_syllable_b008_a = char32_t{0x10000};
constexpr auto cuneiform_sign_uru_times_ki = char32_t{0x12345};
constexpr auto cjk_unified_ideograph_2070e = char32_t{0x2070e};
constexpr auto cjk_unified_ideograph_20731 = char32_t{0x20731};
constexpr auto cjk_unified_ideograph_20779 = char32_t{0x20779};
constexpr auto cjk_unified_ideograph_20c53 = char32_t{0x20c53};
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
struct encoded_char<cjk_unified_ideograph_2070e, char32_t> {
  static constexpr std::array<char32_t, 1> value = {
      cjk_unified_ideograph_2070e};
};
template <>
struct encoded_char<cjk_unified_ideograph_2070e, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd841},
                                                    char16_t{0xdf0e}};
};
template <>
struct encoded_char<cjk_unified_ideograph_2070e, icubaby::char8> {
  static constexpr std::array<char8_t, 4> value = {
      static_cast<icubaby::char8> (0xf0),
      static_cast<icubaby::char8> (0xa0),
      static_cast<icubaby::char8> (0x9c),
      static_cast<icubaby::char8> (0x8e),
  };
};

template <>
struct encoded_char<cjk_unified_ideograph_20731, char32_t> {
  static constexpr std::array<char32_t, 1> value = {
      cjk_unified_ideograph_20731};
};
template <>
struct encoded_char<cjk_unified_ideograph_20731, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd841},
                                                    char16_t{0xdf31}};
};
template <>
struct encoded_char<cjk_unified_ideograph_20731, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0xA0),
      static_cast<icubaby::char8> (0x9C), static_cast<icubaby::char8> (0xB1)};
};

template <>
struct encoded_char<cjk_unified_ideograph_20779, char32_t> {
  static constexpr std::array<char32_t, 1> value = {
      cjk_unified_ideograph_20779};
};
template <>
struct encoded_char<cjk_unified_ideograph_20779, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd841},
                                                    char16_t{0xdf79}};
};
template <>
struct encoded_char<cjk_unified_ideograph_20779, icubaby::char8> {
  static constexpr std::array<char8_t, 4> value = {
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0),
      static_cast<icubaby::char8> (0x9d), static_cast<icubaby::char8> (0xb9)};
};

template <>
struct encoded_char<cjk_unified_ideograph_20c53, char32_t> {
  static constexpr std::array<char32_t, 1> value = {
      cjk_unified_ideograph_20c53};
};
template <>
struct encoded_char<cjk_unified_ideograph_20c53, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd843},
                                                    char16_t{0xdc53}};
};
template <>
struct encoded_char<cjk_unified_ideograph_20c53, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0),
      static_cast<icubaby::char8> (0xb1), static_cast<icubaby::char8> (0x93)};
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

#endif  // ENCODED_CHAR_HPP
