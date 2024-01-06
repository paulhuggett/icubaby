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
#ifndef UNITTESTS_ENCODED_CHAR_HPP
#define UNITTESTS_ENCODED_CHAR_HPP

#include <array>

#include "icubaby/icubaby.hpp"

enum class code_point : char32_t {
  cjk_unified_ideograph_2070e = char32_t{0x2070e},
  cjk_unified_ideograph_20731 = char32_t{0x20731},
  cjk_unified_ideograph_20779 = char32_t{0x20779},
  cjk_unified_ideograph_20c53 = char32_t{0x20c53},
  code_point_ffff = char32_t{0xffff},
  cuneiform_sign_uru_times_ki = char32_t{0x12345},
  dollar_sign = char32_t{0x0024},
  hiragana_letter_go = char32_t{0x3054},
  hiragana_letter_ha = char32_t{0x306f},
  hiragana_letter_i = char32_t{0x3044},
  hiragana_letter_ma = char32_t{0x307e},
  hiragana_letter_o = char32_t{0x304a},
  hiragana_letter_su = char32_t{0x3059},
  hiragana_letter_u = char32_t{0x3046},
  hiragana_letter_yo = char32_t{0x3088},
  hiragana_letter_za = char32_t{0x3056},
  linear_b_syllable_b008_a = char32_t{0x10000},
  replacement_char = icubaby::replacement_char,
  start_of_heading = char32_t{0x001},
  start_of_text = char32_t{0x002},

  last_valid_code_point = icubaby::max_code_point
};

template <code_point C, typename To> struct encoded_char;

template <> struct encoded_char<code_point::start_of_heading, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::start_of_heading)};
};
template <> struct encoded_char<code_point::start_of_heading, char16_t> {
  static constexpr std::array<char16_t, 1> value = {static_cast<char16_t> (code_point::start_of_heading)};
};
template <> struct encoded_char<code_point::start_of_heading, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 1> value = {static_cast<icubaby::char8> (code_point::start_of_heading)};
};

template <> struct encoded_char<code_point::start_of_text, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::start_of_text)};
};
template <> struct encoded_char<code_point::start_of_text, char16_t> {
  static constexpr std::array<char16_t, 1> value = {static_cast<char16_t> (code_point::start_of_text)};
};
template <> struct encoded_char<code_point::start_of_text, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 1> value = {static_cast<icubaby::char8> (code_point::start_of_text)};
};

template <> struct encoded_char<code_point::dollar_sign, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::dollar_sign)};
};
template <> struct encoded_char<code_point::dollar_sign, char16_t> {
  static constexpr std::array<char16_t, 1> value = {static_cast<char16_t> (code_point::dollar_sign)};
};
template <> struct encoded_char<code_point::dollar_sign, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 1> value = {static_cast<icubaby::char8> (code_point::dollar_sign)};
};

template <> struct encoded_char<code_point::replacement_char, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::replacement_char)};
};
template <> struct encoded_char<code_point::replacement_char, char16_t> {
  static constexpr std::array<char16_t, 1> value = {static_cast<char16_t> (code_point::replacement_char)};
};
template <> struct encoded_char<code_point::replacement_char, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xef), static_cast<icubaby::char8> (0xbf), static_cast<icubaby::char8> (0xbd)};
};

template <> struct encoded_char<code_point::hiragana_letter_o, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_o)};
};
template <> struct encoded_char<code_point::hiragana_letter_o, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x304a}};
};
template <> struct encoded_char<code_point::hiragana_letter_o, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0x8a),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_ha, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_ha)};
};
template <> struct encoded_char<code_point::hiragana_letter_ha, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x306f}};
};
template <> struct encoded_char<code_point::hiragana_letter_ha, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0xaf),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_yo, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_yo)};
};
template <> struct encoded_char<code_point::hiragana_letter_yo, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x3088}};
};
template <> struct encoded_char<code_point::hiragana_letter_yo, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x82), static_cast<icubaby::char8> (0x88)};
};

template <> struct encoded_char<code_point::hiragana_letter_u, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_u)};
};
template <> struct encoded_char<code_point::hiragana_letter_u, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x3046}};
};
template <> struct encoded_char<code_point::hiragana_letter_u, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81), static_cast<icubaby::char8> (0x86)};
};

template <> struct encoded_char<code_point::hiragana_letter_go, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_go)};
};
template <> struct encoded_char<code_point::hiragana_letter_go, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x3054}};
};
template <> struct encoded_char<code_point::hiragana_letter_go, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0x94),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_za, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_za)};
};
template <> struct encoded_char<code_point::hiragana_letter_za, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x3056}};
};
template <> struct encoded_char<code_point::hiragana_letter_za, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81), static_cast<icubaby::char8> (0x96)};
};

template <> struct encoded_char<code_point::hiragana_letter_i, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_i)};
};
template <> struct encoded_char<code_point::hiragana_letter_i, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x3044}};
};
template <> struct encoded_char<code_point::hiragana_letter_i, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81), static_cast<icubaby::char8> (0x84)};
};

template <> struct encoded_char<code_point::hiragana_letter_ma, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_ma)};
};
template <> struct encoded_char<code_point::hiragana_letter_ma, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x307e}};
};
template <> struct encoded_char<code_point::hiragana_letter_ma, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81), static_cast<icubaby::char8> (0xbe)};
};

template <> struct encoded_char<code_point::hiragana_letter_su, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::hiragana_letter_su)};
};
template <> struct encoded_char<code_point::hiragana_letter_su, char16_t> {
  static constexpr std::array<char16_t, 1> value = {char16_t{0x3059}};
};
template <> struct encoded_char<code_point::hiragana_letter_su, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value = {
      static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81), static_cast<icubaby::char8> (0x99)};
};

template <> struct encoded_char<code_point::code_point_ffff, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::code_point_ffff)};
};
template <> struct encoded_char<code_point::code_point_ffff, char16_t> {
  static constexpr std::array<char16_t, 1> value = {static_cast<char16_t> (code_point::code_point_ffff)};
};
template <> struct encoded_char<code_point::code_point_ffff, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 3> value{
      static_cast<icubaby::char8> (0xef), static_cast<icubaby::char8> (0xbf), static_cast<icubaby::char8> (0xbf)};
};

template <> struct encoded_char<code_point::linear_b_syllable_b008_a, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::linear_b_syllable_b008_a)};
};
template <> struct encoded_char<code_point::linear_b_syllable_b008_a, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd800}, char16_t{0xdc00}};
};
template <> struct encoded_char<code_point::linear_b_syllable_b008_a, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value{
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0x90), static_cast<icubaby::char8> (0x80),
      static_cast<icubaby::char8> (0x80)};
};

template <> struct encoded_char<code_point::cuneiform_sign_uru_times_ki, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::cuneiform_sign_uru_times_ki)};
};
template <> struct encoded_char<code_point::cuneiform_sign_uru_times_ki, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd808}, char16_t{0xdf45}};
};
template <> struct encoded_char<code_point::cuneiform_sign_uru_times_ki, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0x92), static_cast<icubaby::char8> (0x8d),
      static_cast<icubaby::char8> (0x85)};
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_2070e, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::cjk_unified_ideograph_2070e)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_2070e, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd841}, char16_t{0xdf0e}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_2070e, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf0),
      static_cast<icubaby::char8> (0xa0),
      static_cast<icubaby::char8> (0x9c),
      static_cast<icubaby::char8> (0x8e),
  };
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_20731, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::cjk_unified_ideograph_20731)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20731, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd841}, char16_t{0xdf31}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20731, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0), static_cast<icubaby::char8> (0x9c),
      static_cast<icubaby::char8> (0xb1)};
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_20779, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::cjk_unified_ideograph_20779)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20779, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd841}, char16_t{0xdf79}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20779, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0), static_cast<icubaby::char8> (0x9d),
      static_cast<icubaby::char8> (0xb9)};
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_20c53, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::cjk_unified_ideograph_20c53)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20c53, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xd843}, char16_t{0xdc53}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20c53, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0), static_cast<icubaby::char8> (0xb1),
      static_cast<icubaby::char8> (0x93)};
};

template <> struct encoded_char<code_point::last_valid_code_point, char32_t> {
  static constexpr std::array<char32_t, 1> value = {static_cast<char32_t> (code_point::last_valid_code_point)};
};
template <> struct encoded_char<code_point::last_valid_code_point, char16_t> {
  static constexpr std::array<char16_t, 2> value = {char16_t{0xdbff}, char16_t{0xdfff}};
};
template <> struct encoded_char<code_point::last_valid_code_point, icubaby::char8> {
  static constexpr std::array<icubaby::char8, 4> value = {
      static_cast<icubaby::char8> (0xf4), static_cast<icubaby::char8> (0x8f), static_cast<icubaby::char8> (0xbf),
      static_cast<icubaby::char8> (0xbf)};
};

template <code_point C, typename To> inline constexpr auto encoded_char_v = encoded_char<C, To>::value;

template <code_point C, typename To, typename OutputIterator> OutputIterator append (OutputIterator out) {
  constexpr auto& code_units = encoded_char_v<C, To>;
  return std::copy (std::begin (code_units), std::end (code_units), out);
}

#endif  // UNITTESTS_ENCODED_CHAR_HPP
