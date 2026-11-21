// MIT License
//
// Copyright (c) 2022-2026 Paul Bowen-Huggett
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

// Checks that the library can be used in constant expressions. Each check is a constexpr function which is evaluated
// both by a static_assert (at compile time) and by a test (at run time, so that it contributes to coverage).

#include <array>
#include <cstddef>
#include <iterator>

#include "icubaby/icubaby.hpp"

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
#include <algorithm>
#include <ranges>
#include <utility>
#endif

#include "gtest/gtest.h"

namespace {

using icubaby::char8;

constexpr char8 c8 (unsigned const value) noexcept {
  return static_cast<char8> (value);
}
constexpr std::byte by (unsigned const value) noexcept {
  return static_cast<std::byte> (value);
}

/// Returns true if the first \p count elements of \p actual are equal to \p expected.
template <typename T, std::size_t N, std::size_t M>
constexpr bool starts_with (std::array<T, N> const& actual, std::size_t const count, std::array<T, M> const& expected) {
  if (count != M) {
    return false;
  }
  for (std::size_t index = 0; index < M; ++index) {
    if (actual.at (index) != expected.at (index)) {
      return false;
    }
  }
  return true;
}

/// Converts \p input to the \p To encoding by calling the transcoder's function-call operator and end_cp(). Returns
/// true if the output matches \p expected and the transcoder's well_formed() matches \p well_formed.
template <typename To, typename From, std::size_t N, std::size_t M>
constexpr bool convert (std::array<From, N> const& input, std::array<To, M> const& expected, bool const well_formed) {
  std::array<To, 32> out{};
  icubaby::transcoder<From, To> transcoder;
  auto* out_it = out.data ();
  for (auto const code_unit : input) {
    out_it = transcoder (code_unit, out_it);
  }
  out_it = transcoder.end_cp (out_it);
  return starts_with (out, static_cast<std::size_t> (std::distance (out.data (), out_it)), expected) &&
         transcoder.well_formed () == well_formed && !transcoder.partial ();
}

/// Converts \p input to the \p To encoding using icubaby::iterator. Returns true if the output matches \p expected.
template <typename To, typename From, std::size_t N, std::size_t M>
constexpr bool convert_with_iterator (std::array<From, N> const& input, std::array<To, M> const& expected) {
  std::array<To, 32> out{};
  icubaby::transcoder<From, To> transcoder;
  icubaby::iterator out_it{&transcoder, out.data ()};
  for (auto const code_unit : input) {
    *out_it = code_unit;
    ++out_it;
  }
  auto* const last = transcoder.end_cp (out_it).base ();
  return starts_with (out, static_cast<std::size_t> (std::distance (out.data (), last)), expected);
}

// U+0041 LATIN CAPITAL LETTER A, U+20AC EURO SIGN, U+1F600 GRINNING FACE in each encoding.
constexpr std::array utf8{c8 (0x41), c8 (0xE2), c8 (0x82), c8 (0xAC), c8 (0xF0), c8 (0x9F), c8 (0x98), c8 (0x80)};
constexpr std::array utf16{char16_t{0x0041}, char16_t{0x20AC}, char16_t{0xD83D}, char16_t{0xDE00}};
constexpr std::array utf32{char32_t{0x0041}, char32_t{0x20AC}, char32_t{0x1F600}};

// Malformed input in each encoding: U+0041 followed by an incomplete or invalid code point.
constexpr std::array bad_utf8{c8 (0x41), c8 (0xC3)};
constexpr std::array bad_utf16{char16_t{0x0041}, char16_t{0xD800}};
constexpr std::array bad_utf32{char32_t{0x0041}, char32_t{0x110000}};

// U+0041 followed by U+FFFD REPLACEMENT CHARACTER in each encoding.
constexpr std::array replaced_utf8{c8 (0x41), c8 (0xEF), c8 (0xBF), c8 (0xBD)};
constexpr std::array replaced_utf16{char16_t{0x0041}, char16_t{0xFFFD}};
constexpr std::array replaced_utf32{char32_t{0x0041}, char32_t{0xFFFD}};

// The same three code points as bytes: UTF-8 without a byte order mark, then UTF-16 and UTF-32 in both byte orders
// with a byte order mark.
constexpr std::array bytes_utf8{by (0x41), by (0xE2), by (0x82), by (0xAC), by (0xF0), by (0x9F), by (0x98), by (0x80)};
constexpr std::array bytes_utf16be{by (0xFE), by (0xFF), by (0x00), by (0x41), by (0x20),
                                   by (0xAC), by (0xD8), by (0x3D), by (0xDE), by (0x00)};
constexpr std::array bytes_utf16le{by (0xFF), by (0xFE), by (0x41), by (0x00), by (0xAC),
                                   by (0x20), by (0x3D), by (0xD8), by (0x00), by (0xDE)};
constexpr std::array bytes_utf32be{by (0x00), by (0x00), by (0xFE), by (0xFF), by (0x00), by (0x00),
                                   by (0x00), by (0x41), by (0x00), by (0x00), by (0x20), by (0xAC),
                                   by (0x00), by (0x01), by (0xF6), by (0x00)};
constexpr std::array bytes_utf32le{by (0xFF), by (0xFE), by (0x00), by (0x00), by (0x41), by (0x00),
                                   by (0x00), by (0x00), by (0xAC), by (0x20), by (0x00), by (0x00),
                                   by (0x00), by (0xF6), by (0x01), by (0x00)};
constexpr std::array bad_bytes{by (0x41), by (0xC3)};

constexpr bool from_utf8 () {
  return convert<char8> (utf8, utf8, true) && convert<char16_t> (utf8, utf16, true) &&
         convert<char32_t> (utf8, utf32, true) && convert<char8> (bad_utf8, replaced_utf8, false) &&
         convert<char16_t> (bad_utf8, replaced_utf16, false) && convert<char32_t> (bad_utf8, replaced_utf32, false);
}
constexpr bool from_utf16 () {
  return convert<char8> (utf16, utf8, true) && convert<char16_t> (utf16, utf16, true) &&
         convert<char32_t> (utf16, utf32, true) && convert<char8> (bad_utf16, replaced_utf8, false) &&
         convert<char16_t> (bad_utf16, replaced_utf16, false) && convert<char32_t> (bad_utf16, replaced_utf32, false);
}
constexpr bool from_utf32 () {
  return convert<char8> (utf32, utf8, true) && convert<char16_t> (utf32, utf16, true) &&
         convert<char32_t> (utf32, utf32, true) && convert<char8> (bad_utf32, replaced_utf8, false) &&
         convert<char16_t> (bad_utf32, replaced_utf16, false) && convert<char32_t> (bad_utf32, replaced_utf32, false);
}
constexpr bool from_bytes () {
  return convert<char8> (bytes_utf8, utf8, true) && convert<char16_t> (bytes_utf16be, utf16, true) &&
         convert<char32_t> (bytes_utf16le, utf32, true) && convert<char8> (bytes_utf32be, utf8, true) &&
         convert<char16_t> (bytes_utf32le, utf16, true) && convert<char32_t> (bad_bytes, replaced_utf32, false);
}
constexpr icubaby::encoding selected_encoding () {
  icubaby::transcoder<std::byte, char8> transcoder;
  std::array<char8, 16> out{};
  auto* out_it = out.data ();
  for (auto const value : bytes_utf32le) {
    out_it = transcoder (value, out_it);
  }
  return transcoder.selected_encoding ();
}
constexpr bool with_iterator () {
  return convert_with_iterator<char16_t> (utf8, utf16) && convert_with_iterator<char8> (utf32, utf8) &&
         convert_with_iterator<char32_t> (bytes_utf16be, utf32);
}
constexpr bool length_and_index () {
  return icubaby::length (utf8.begin (), utf8.end ()) == 3 && icubaby::length (utf16.begin (), utf16.end ()) == 3 &&
         icubaby::index (utf8.begin (), utf8.end (), 2) == std::next (utf8.begin (), 4) &&
         icubaby::index (utf16.begin (), utf16.end (), 3) == utf16.end ();
}

static_assert (from_utf8 ());
static_assert (from_utf16 ());
static_assert (from_utf32 ());
static_assert (from_bytes ());
static_assert (selected_encoding () == icubaby::encoding::utf32le);
static_assert (with_iterator ());
static_assert (length_and_index ());

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
/// Converts \p input to the \p To encoding using icubaby::views::transcode. Returns true if the output matches
/// \p expected.
template <typename To, typename From, std::size_t N, std::size_t M>
constexpr bool convert_with_view (std::array<From, N> const& input, std::array<To, M> const& expected) {
  std::array<To, 32> out{};
  auto const result = std::ranges::copy (input | icubaby::views::transcode<From, To>, out.begin ());
  return starts_with (out, static_cast<std::size_t> (std::distance (out.begin (), result.out)), expected);
}
constexpr bool with_view () {
  // The view's well_formed() cannot be used in a constant expression but malformed input is still replaced.
  return convert_with_view<char16_t> (utf8, utf16) && convert_with_view<char8> (utf32, utf8) &&
         convert_with_view<char32_t> (utf16, utf32) && convert_with_view<char8> (bytes_utf16le, utf8) &&
         convert_with_view<char32_t> (bad_utf8, replaced_utf32) && convert_with_view<char8> (bad_utf16, replaced_utf8);
}
/// Copies, assigns and moves a view, then passes it to a standard range adaptor (which moves it again). Returns true
/// if the result holds the first three UTF-16 code units of the input.
constexpr bool copy_view () {
  auto const view = utf8 | icubaby::views::transcode<char8, char16_t>;
  auto copy = view;
  copy = view;
  auto moved = std::move (copy);
  moved = utf8 | icubaby::views::transcode<char8, char16_t>;
  std::array<char16_t, 32> out{};
  auto const result = std::ranges::copy (std::move (moved) | std::views::take (3), out.begin ());
  return starts_with (out, static_cast<std::size_t> (std::distance (out.begin (), result.out)),
                      std::array{char16_t{0x0041}, char16_t{0x20AC}, char16_t{0xD83D}});
}
constexpr bool ranges_length_and_index () {
  return icubaby::length (utf8) == 3 && icubaby::index (utf16, 2) == std::next (utf16.begin (), 2);
}
static_assert (with_view ());
static_assert (copy_view ());
static_assert (ranges_length_and_index ());
#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

}  // end anonymous namespace

// NOLINTNEXTLINE
TEST (Constexpr, FromUtf8) {
  EXPECT_TRUE (from_utf8 ());
}
// NOLINTNEXTLINE
TEST (Constexpr, FromUtf16) {
  EXPECT_TRUE (from_utf16 ());
}
// NOLINTNEXTLINE
TEST (Constexpr, FromUtf32) {
  EXPECT_TRUE (from_utf32 ());
}
// NOLINTNEXTLINE
TEST (Constexpr, FromBytes) {
  EXPECT_TRUE (from_bytes ());
  EXPECT_EQ (selected_encoding (), icubaby::encoding::utf32le);
}
// NOLINTNEXTLINE
TEST (Constexpr, Iterator) {
  EXPECT_TRUE (with_iterator ());
}
// NOLINTNEXTLINE
TEST (Constexpr, LengthAndIndex) {
  EXPECT_TRUE (length_and_index ());
}
#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
// NOLINTNEXTLINE
TEST (Constexpr, View) {
  EXPECT_TRUE (with_view ());
}
// NOLINTNEXTLINE
TEST (Constexpr, CopyView) {
  EXPECT_TRUE (copy_view ());
}
// NOLINTNEXTLINE
TEST (Constexpr, RangesLengthAndIndex) {
  EXPECT_TRUE (ranges_length_and_index ());
}
#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
