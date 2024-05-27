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

#ifndef ICUBABY_ENCODED_CHAR_HPP
#define ICUBABY_ENCODED_CHAR_HPP (1)

#include <algorithm>
#include <array>

#include "icubaby/icubaby.hpp"

enum class code_point : char32_t {
  cent_sign = char32_t{0x00A2},
  digit_zero = char32_t{0x0030},
  digit_one = char32_t{0x0031},
  digit_two = char32_t{0x0032},
  digit_three = char32_t{0x0033},
  cjk_unified_ideograph_2070e = char32_t{0x2070E},
  cjk_unified_ideograph_20731 = char32_t{0x20731},
  cjk_unified_ideograph_20779 = char32_t{0x20779},
  cjk_unified_ideograph_20c53 = char32_t{0x20c53},
  cjk_unified_ideograph_4e16 = char32_t{0x4E16},
  cjk_unified_ideograph_754c = char32_t{0x754C},
  code_point_ffff = char32_t{0xffff},
  cuneiform_sign_uru_times_ki = char32_t{0x12345},
  devanagri_letter_ha = char32_t{0x939},
  dollar_sign = char32_t{0x0024},
  gothic_letter_hwair = char32_t{0x10348},
  hiragana_letter_go = char32_t{0x3054},
  hiragana_letter_ha = char32_t{0x306F},
  hiragana_letter_i = char32_t{0x3044},
  hiragana_letter_ko = char32_t{0x3053},
  hiragana_letter_ma = char32_t{0x307E},
  hiragana_letter_n = char32_t{0x3093},
  hiragana_letter_ni = char32_t{0x306B},
  hiragana_letter_o = char32_t{0x304a},
  hiragana_letter_su = char32_t{0x3059},
  hiragana_letter_ti = char32_t{0x3061},
  hiragana_letter_u = char32_t{0x3046},
  hiragana_letter_yo = char32_t{0x3088},
  hiragana_letter_za = char32_t{0x3056},
  line_feed = char32_t{0x000A},
  linear_b_syllable_b008_a = char32_t{0x10000},
  pilcrow_sign = char32_t{0x00B6},
  pile_of_poop = char32_t{0x1F4A9},
  snowman = char32_t{0x2603},

  replacement_char = icubaby::replacement_char,
  start_of_heading = char32_t{0x001},
  start_of_text = char32_t{0x002},

  u80 = char32_t{0x0080},
  last_valid_code_point = icubaby::max_code_point
};

template <code_point C, typename To> struct encoded_char;

template <> struct encoded_char<code_point::line_feed, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::line_feed)};
};
template <> struct encoded_char<code_point::line_feed, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::line_feed)};
};
template <> struct encoded_char<code_point::line_feed, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::line_feed)};
};

template <> struct encoded_char<code_point::start_of_heading, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::start_of_heading)};
};
template <> struct encoded_char<code_point::start_of_heading, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::start_of_heading)};
};
template <> struct encoded_char<code_point::start_of_heading, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::start_of_heading)};
};

template <> struct encoded_char<code_point::start_of_text, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::start_of_text)};
};
template <> struct encoded_char<code_point::start_of_text, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::start_of_text)};
};
template <> struct encoded_char<code_point::start_of_text, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::start_of_text)};
};

template <> struct encoded_char<code_point::devanagri_letter_ha, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::devanagri_letter_ha)};
};
template <> struct encoded_char<code_point::devanagri_letter_ha, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::devanagri_letter_ha)};
};
template <> struct encoded_char<code_point::devanagri_letter_ha, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xE0), static_cast<icubaby::char8> (0xA4),
                                    static_cast<icubaby::char8> (0xB9)};
};

template <> struct encoded_char<code_point::dollar_sign, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::dollar_sign)};
};
template <> struct encoded_char<code_point::dollar_sign, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::dollar_sign)};
};
template <> struct encoded_char<code_point::dollar_sign, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::dollar_sign)};
};

template <> struct encoded_char<code_point::cent_sign, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cent_sign)};
};
template <> struct encoded_char<code_point::cent_sign, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::cent_sign)};
};
template <> struct encoded_char<code_point::cent_sign, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xC2), static_cast<icubaby::char8> (0xA2)};
};

template <> struct encoded_char<code_point::digit_zero, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::digit_zero)};
};
template <> struct encoded_char<code_point::digit_zero, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::digit_zero)};
};
template <> struct encoded_char<code_point::digit_zero, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::digit_zero)};
};

template <> struct encoded_char<code_point::digit_one, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::digit_one)};
};
template <> struct encoded_char<code_point::digit_one, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::digit_one)};
};
template <> struct encoded_char<code_point::digit_one, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::digit_one)};
};

template <> struct encoded_char<code_point::digit_two, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::digit_two)};
};
template <> struct encoded_char<code_point::digit_two, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::digit_two)};
};
template <> struct encoded_char<code_point::digit_two, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::digit_two)};
};

template <> struct encoded_char<code_point::digit_three, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::digit_three)};
};
template <> struct encoded_char<code_point::digit_three, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::digit_three)};
};
template <> struct encoded_char<code_point::digit_three, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (code_point::digit_three)};
};

template <> struct encoded_char<code_point::pilcrow_sign, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::pilcrow_sign)};
};
template <> struct encoded_char<code_point::pilcrow_sign, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::pilcrow_sign)};
};
template <> struct encoded_char<code_point::pilcrow_sign, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xC2), static_cast<icubaby::char8> (0xB6)};
};

template <> struct encoded_char<code_point::pile_of_poop, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::pile_of_poop)};
};
template <> struct encoded_char<code_point::pile_of_poop, char16_t> {
  static constexpr std::array value{char16_t{0xD83D}, char16_t{0xDCA9}};
};
template <> struct encoded_char<code_point::pile_of_poop, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
                                    static_cast<icubaby::char8> (0x92), static_cast<icubaby::char8> (0xA9)};
};

template <> struct encoded_char<code_point::snowman, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::snowman)};
};
template <> struct encoded_char<code_point::snowman, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::snowman)};
};
template <> struct encoded_char<code_point::snowman, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xE2), static_cast<icubaby::char8> (0x98),
                                    static_cast<icubaby::char8> (0x83)};
};

template <> struct encoded_char<code_point::replacement_char, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::replacement_char)};
};
template <> struct encoded_char<code_point::replacement_char, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::replacement_char)};
};
template <> struct encoded_char<code_point::replacement_char, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xef), static_cast<icubaby::char8> (0xbf),
                                    static_cast<icubaby::char8> (0xbd)};
};

template <> struct encoded_char<code_point::gothic_letter_hwair, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::gothic_letter_hwair)};
};
template <> struct encoded_char<code_point::gothic_letter_hwair, char16_t> {
  static constexpr std::array value{char16_t{0xD800}, char16_t{0xDF48}};
};
template <> struct encoded_char<code_point::gothic_letter_hwair, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xF0),
      static_cast<icubaby::char8> (0x90),
      static_cast<icubaby::char8> (0x8D),
      static_cast<icubaby::char8> (0x88),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_ko, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_ko)};
};
template <> struct encoded_char<code_point::hiragana_letter_ko, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::hiragana_letter_ko)};
};
template <> struct encoded_char<code_point::hiragana_letter_ko, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xE3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0x93),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_n, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_n)};
};
template <> struct encoded_char<code_point::hiragana_letter_n, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::hiragana_letter_n)};
};
template <> struct encoded_char<code_point::hiragana_letter_n, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xE3),
      static_cast<icubaby::char8> (0x82),
      static_cast<icubaby::char8> (0x93),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_ni, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_ni)};
};
template <> struct encoded_char<code_point::hiragana_letter_ni, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::hiragana_letter_ni)};
};
template <> struct encoded_char<code_point::hiragana_letter_ni, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xE3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0xAB),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_o, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_o)};
};
template <> struct encoded_char<code_point::hiragana_letter_o, char16_t> {
  static constexpr std::array value{char16_t{0x304a}};
};
template <> struct encoded_char<code_point::hiragana_letter_o, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xe3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0x8a),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_ha, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_ha)};
};
template <> struct encoded_char<code_point::hiragana_letter_ha, char16_t> {
  static constexpr std::array value{char16_t{0x306f}};
};
template <> struct encoded_char<code_point::hiragana_letter_ha, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xe3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0xaf),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_yo, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_yo)};
};
template <> struct encoded_char<code_point::hiragana_letter_yo, char16_t> {
  static constexpr std::array value{char16_t{0x3088}};
};
template <> struct encoded_char<code_point::hiragana_letter_yo, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x82),
                                    static_cast<icubaby::char8> (0x88)};
};

template <> struct encoded_char<code_point::hiragana_letter_u, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_u)};
};
template <> struct encoded_char<code_point::hiragana_letter_u, char16_t> {
  static constexpr std::array value{char16_t{0x3046}};
};
template <> struct encoded_char<code_point::hiragana_letter_u, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81),
                                    static_cast<icubaby::char8> (0x86)};
};

template <> struct encoded_char<code_point::hiragana_letter_go, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_go)};
};
template <> struct encoded_char<code_point::hiragana_letter_go, char16_t> {
  static constexpr std::array value{char16_t{0x3054}};
};
template <> struct encoded_char<code_point::hiragana_letter_go, icubaby::char8> {
  static constexpr std::array value = {
      static_cast<icubaby::char8> (0xe3),
      static_cast<icubaby::char8> (0x81),
      static_cast<icubaby::char8> (0x94),
  };
};

template <> struct encoded_char<code_point::hiragana_letter_za, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_za)};
};
template <> struct encoded_char<code_point::hiragana_letter_za, char16_t> {
  static constexpr std::array value{char16_t{0x3056}};
};
template <> struct encoded_char<code_point::hiragana_letter_za, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81),
                                    static_cast<icubaby::char8> (0x96)};
};

template <> struct encoded_char<code_point::hiragana_letter_i, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_i)};
};
template <> struct encoded_char<code_point::hiragana_letter_i, char16_t> {
  static constexpr std::array value{char16_t{0x3044}};
};
template <> struct encoded_char<code_point::hiragana_letter_i, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81),
                                    static_cast<icubaby::char8> (0x84)};
};

template <> struct encoded_char<code_point::hiragana_letter_ma, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_ma)};
};
template <> struct encoded_char<code_point::hiragana_letter_ma, char16_t> {
  static constexpr std::array value{char16_t{0x307e}};
};
template <> struct encoded_char<code_point::hiragana_letter_ma, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xe3), static_cast<icubaby::char8> (0x81),
                                    static_cast<icubaby::char8> (0xbe)};
};

template <> struct encoded_char<code_point::hiragana_letter_su, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_su)};
};
template <> struct encoded_char<code_point::hiragana_letter_su, char16_t> {
  static constexpr std::array value{char16_t{0x3059}};
};
template <> struct encoded_char<code_point::hiragana_letter_su, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xE3), static_cast<icubaby::char8> (0x81),
                                    static_cast<icubaby::char8> (0x99)};
};

template <> struct encoded_char<code_point::hiragana_letter_ti, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::hiragana_letter_ti)};
};
template <> struct encoded_char<code_point::hiragana_letter_ti, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::hiragana_letter_ti)};
};
template <> struct encoded_char<code_point::hiragana_letter_ti, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xE3), static_cast<icubaby::char8> (0x81),
                                    static_cast<icubaby::char8> (0xA1)};
};

template <> struct encoded_char<code_point::code_point_ffff, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::code_point_ffff)};
};
template <> struct encoded_char<code_point::code_point_ffff, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::code_point_ffff)};
};
template <> struct encoded_char<code_point::code_point_ffff, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xef), static_cast<icubaby::char8> (0xbf),
                                    static_cast<icubaby::char8> (0xbf)};
};

template <> struct encoded_char<code_point::linear_b_syllable_b008_a, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::linear_b_syllable_b008_a)};
};
template <> struct encoded_char<code_point::linear_b_syllable_b008_a, char16_t> {
  static constexpr std::array value{char16_t{0xd800}, char16_t{0xdc00}};
};
template <> struct encoded_char<code_point::linear_b_syllable_b008_a, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0x90),
                                    static_cast<icubaby::char8> (0x80), static_cast<icubaby::char8> (0x80)};
};

template <> struct encoded_char<code_point::cuneiform_sign_uru_times_ki, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cuneiform_sign_uru_times_ki)};
};
template <> struct encoded_char<code_point::cuneiform_sign_uru_times_ki, char16_t> {
  static constexpr std::array value{char16_t{0xd808}, char16_t{0xdf45}};
};
template <> struct encoded_char<code_point::cuneiform_sign_uru_times_ki, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0x92),
                                    static_cast<icubaby::char8> (0x8d), static_cast<icubaby::char8> (0x85)};
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_2070e, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cjk_unified_ideograph_2070e)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_2070e, char16_t> {
  static constexpr std::array value{char16_t{0xd841}, char16_t{0xdf0e}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_2070e, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xf0),
      static_cast<icubaby::char8> (0xa0),
      static_cast<icubaby::char8> (0x9c),
      static_cast<icubaby::char8> (0x8e),
  };
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_4e16, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cjk_unified_ideograph_4e16)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_4e16, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::cjk_unified_ideograph_4e16)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_4e16, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xE4),
      static_cast<icubaby::char8> (0xB8),
      static_cast<icubaby::char8> (0x96),
  };
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_754c, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cjk_unified_ideograph_754c)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_754c, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::cjk_unified_ideograph_754c)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_754c, icubaby::char8> {
  static constexpr std::array value{
      static_cast<icubaby::char8> (0xE7),
      static_cast<icubaby::char8> (0x95),
      static_cast<icubaby::char8> (0x8C),
  };
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_20731, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cjk_unified_ideograph_20731)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20731, char16_t> {
  static constexpr std::array value{char16_t{0xd841}, char16_t{0xdf31}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20731, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0),
                                    static_cast<icubaby::char8> (0x9c), static_cast<icubaby::char8> (0xb1)};
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_20779, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cjk_unified_ideograph_20779)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20779, char16_t> {
  static constexpr std::array value{char16_t{0xd841}, char16_t{0xdf79}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20779, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0),
                                    static_cast<icubaby::char8> (0x9d), static_cast<icubaby::char8> (0xb9)};
};

template <> struct encoded_char<code_point::cjk_unified_ideograph_20c53, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::cjk_unified_ideograph_20c53)};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20c53, char16_t> {
  static constexpr std::array value{char16_t{0xd843}, char16_t{0xdc53}};
};
template <> struct encoded_char<code_point::cjk_unified_ideograph_20c53, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xf0), static_cast<icubaby::char8> (0xa0),
                                    static_cast<icubaby::char8> (0xb1), static_cast<icubaby::char8> (0x93)};
};

template <> struct encoded_char<code_point::last_valid_code_point, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::last_valid_code_point)};
};
template <> struct encoded_char<code_point::last_valid_code_point, char16_t> {
  static constexpr std::array value{char16_t{0xdbff}, char16_t{0xdfff}};
};
template <> struct encoded_char<code_point::last_valid_code_point, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xf4), static_cast<icubaby::char8> (0x8f),
                                    static_cast<icubaby::char8> (0xbf), static_cast<icubaby::char8> (0xbf)};
};

template <> struct encoded_char<code_point::u80, char32_t> {
  static constexpr std::array value{static_cast<char32_t> (code_point::u80)};
};
template <> struct encoded_char<code_point::u80, char16_t> {
  static constexpr std::array value{static_cast<char16_t> (code_point::u80)};
};
template <> struct encoded_char<code_point::u80, icubaby::char8> {
  static constexpr std::array value{static_cast<icubaby::char8> (0xC2), static_cast<icubaby::char8> (0x80)};
};

template <code_point C, typename To> inline constexpr auto encoded_char_v = encoded_char<C, To>::value;

template <code_point C, typename To, typename OutputIterator> OutputIterator append (OutputIterator out) {
  constexpr auto& code_units = encoded_char_v<C, To>;
#if ICUBABY_HAVE_RANGES
  return std::ranges::copy (code_units, out).out;
#else
  return std::copy (std::begin (code_units), std::end (code_units), out);
#endif  // ICUBABY_HAVE_RANGES
}

#endif  // ICUBABY_ENCODED_CHAR_HPP
