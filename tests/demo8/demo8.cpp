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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

#include "icubaby/icubaby.hpp"

namespace {

template <typename StringType>
void show (std::ostream& os, StringType const& str) {
  os << std::setfill('0') << std::hex;
  auto const* separator = "";
  for (auto const c: str) {
    os << separator << std::setw (sizeof (c) * 2) << static_cast<std::uint16_t> (c);
    separator = " ";
  }
  os << '\n';
}

std::optional<std::u16string> convert (
    std::basic_string_view<icubaby::char8> const& src) {
  std::u16string out;

  // t8_16 is the class which converts from UTF-8 to UTF-16.
  // This name is a shortned form of transcoder<char8_t, char16_T>.
  icubaby::t8_16 utf_8_to_16;
  auto it = icubaby::iterator{&utf_8_to_16, std::back_inserter (out)};
  it = std::copy (std::begin (src), std::end (src), it);
  utf_8_to_16.end_cp (it);
  if (!utf_8_to_16.well_formed ()) {
    // The input was malformed or ended with a partial character.
    return std::nullopt;
  }
  return out;
}
std::optional<std::u16string> convert2 (
    std::basic_string_view<icubaby::char8> const& src) {
  // The UTF-16 code units are written to the 'out' string via the 'it' output iterator.
  std::u16string out;
  auto it = std::back_inserter (out);
  icubaby::t8_16 utf_8_to_16;
  for (icubaby::char8 const c : src) {
    // Pass this UTF-8 code-unit to the transcoder.
    it = utf_8_to_16(c, it);
    if (!utf_8_to_16.well_formed ()) {
      // The input was malformed. Bail out immediately.
      return std::nullopt;
    }
  }
  // Check that the input finished with a complete character.
  it = utf_8_to_16.end_cp (it);
  if (!utf_8_to_16.well_formed ()) {
    return std::nullopt;
  }
  return out; // Conversion was successful.
}

void c3 () {
  icubaby::t8_16 t;
  std::vector<char16_t> out;
  auto it = icubaby::iterator{&t, std::back_inserter (out)};
  *(it++) = icubaby::char8{'A'};
  t.end_cp (std::back_inserter (out));
  show (std::cout, out);
}

void c4 () {
  std::vector<char16_t> out;
  icubaby::t8_16 t;
  std::array<icubaby::char8, 4> const in{
      static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
      static_cast<icubaby::char8> (0x98), static_cast<icubaby::char8> (0x80)};
  auto it = icubaby::iterator{&t, std::back_inserter (out)};
  for (auto cu : in) {
    *(it++) = cu;
  }
  it = t.end_cp (it);
  show (std::cout, out);
}

void c5 () {
  std::array<icubaby::char8, 4> const in{
      static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
      static_cast<icubaby::char8> (0x98), static_cast<icubaby::char8> (0x80)};
  std::vector<char16_t> out;
  icubaby::t8_16 t;
#if __cpp_lib_ranges
  auto it =
      std::ranges::copy (in, icubaby::iterator{&t, std::back_inserter (out)})
          .out;
#else
  auto it = std::copy (std::begin (in), std::end (in),
                       icubaby::iterator{&t, std::back_inserter (out)});
#endif
  t.end_cp (it);
}

}  // namespace

int main () {
  using namespace std::string_view_literals;
  icubaby::char8 const* in = u8"こんにちは世界\n";
  show (std::cout, std::basic_string_view<icubaby::char8> (in));
  show (std::cout, *convert (in));
  show (std::cout, *convert2 (in));
  c3 ();
  c4 ();
  c5 ();

  icubaby::t32_16 t;
  std::vector<char16_t> out;
  auto it = std::back_inserter (out);
  it = t (char32_t{0x2070E}, it);
  it = t (char32_t{0x20731}, it);
  it = t (char32_t{0x20779}, it);
  it = t (char32_t{0x20C53}, it);
  show (std::cout, out);
}

