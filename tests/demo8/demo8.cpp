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

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <version>

// Do we have library support for C++ 20 std::format()?
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#define HAVE_CPP_LIB_FORMAT (1)
#include <format>
#else
// We're lacking std::format(). Fall back to using iostreams manipulators.
#define HAVE_CPP_LIB_FORMAT (0)
#include <iomanip>
#endif

#include "icubaby/icubaby.hpp"

namespace {

#if HAVE_CPP_LIB_FORMAT

template <typename StringType> void show (std::ostream& stream, StringType const& str) {
  auto const* separator = "";
  for (auto const code_unit : str) {
    stream << separator;
    if constexpr (sizeof (code_unit) == 1) {
      stream << std::format ("{:02X}", static_cast<std::uint_least8_t> (code_unit));
    } else {
      stream << std::format ("{:04X}", static_cast<std::uint_least16_t> (code_unit));
    }
    separator = " ";
  }
  stream << '\n';
}

#else

/// \brief A class used to save an iostream's formatting flags on construction
/// and restore them on destruction.
///
/// Used to manage the restoration of the flags on exit from a scope.
class ios_flags_saver {
public:
  explicit ios_flags_saver (std::ios_base& stream) : stream_{&stream}, flags_{stream.flags ()} {}
  ios_flags_saver (ios_flags_saver const&) = delete;
  ios_flags_saver (ios_flags_saver&&) noexcept = delete;

  ~ios_flags_saver () { (void)stream_->flags (flags_); }

  ios_flags_saver& operator= (ios_flags_saver const&) = delete;
  ios_flags_saver& operator= (ios_flags_saver&&) noexcept = delete;

private:
  std::ios_base* stream_;
  std::ios_base::fmtflags flags_;
};

template <typename StringType> void show (std::ostream& stream, StringType const& str) {
  ios_flags_saver const saved_flags{stream};
  stream << std::setfill ('0') << std::hex << std::uppercase;
  auto const* separator = "";
  for (auto const code_unit : str) {
    stream << separator << std::setw (sizeof (code_unit) * 2) << static_cast<std::uint_least16_t> (code_unit);
    separator = " ";
  }
  stream << '\n';
}

#endif  // HAVE_CPP_LIB_FORMAT

template <typename StringType> void show (std::ostream& stream, std::optional<StringType> const& str) {
  if (!str) {
    return;  // do something.
  }
  show (stream, *str);
}

std::optional<std::u16string> convert (std::basic_string_view<icubaby::char8> const& src) {
  std::u16string out;

  // t8_16 is the class which converts from UTF-8 to UTF-16.
  // This name is a shortened form of transcoder<char8_t, char16_T>.
  icubaby::t8_16 utf_8_to_16;
  auto out_it = icubaby::iterator{&utf_8_to_16, std::back_inserter (out)};
#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L
  out_it = std::ranges::copy (src, out_it).out;
#else
  out_it = std::copy (std::begin (src), std::end (src), out_it);
#endif
  (void)utf_8_to_16.end_cp (out_it);
  if (!utf_8_to_16.well_formed ()) {
    // The input was malformed or ended with a partial character.
    return std::nullopt;
  }
  return out;
}
std::optional<std::u16string> convert2 (std::basic_string_view<icubaby::char8> const& src) {
  // The UTF-16 code units are written to the 'out' string via the 'out_it' output iterator.
  std::u16string out;
  auto out_it = std::back_inserter (out);
  icubaby::t8_16 utf_8_to_16;
  for (icubaby::char8 const code_unit : src) {
    // Pass this UTF-8 code-unit to the transcoder.
    out_it = utf_8_to_16 (code_unit, out_it);
    if (!utf_8_to_16.well_formed ()) {
      // The input was malformed. Bail out immediately.
      return std::nullopt;
    }
  }
  // Check that the input finished with a complete character.
  (void)utf_8_to_16.end_cp (out_it);
  if (!utf_8_to_16.well_formed ()) {
    return std::nullopt;
  }
  return out;  // Conversion was successful.
}

void c3 () {
  icubaby::t8_16 transcoder;
  std::vector<char16_t> out;
  auto out_it = icubaby::iterator{&transcoder, std::back_inserter (out)};
  *(out_it++) = icubaby::char8{'A'};
  (void)transcoder.end_cp (std::back_inserter (out));
  show (std::cout, out);
}

void c4 () {
  std::vector<char16_t> out;
  icubaby::t8_16 transcoder;
  std::array const input{static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
                         static_cast<icubaby::char8> (0x98), static_cast<icubaby::char8> (0x80)};
  auto out_it = icubaby::iterator{&transcoder, std::back_inserter (out)};
  for (auto const code_unit : input) {
    *(out_it++) = code_unit;
  }
  (void)transcoder.end_cp (out_it);
  show (std::cout, out);
}

void c5 () {
  std::array const input{static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
                         static_cast<icubaby::char8> (0x98), static_cast<icubaby::char8> (0x80)};
  std::vector<char16_t> out;
  icubaby::t8_16 transcoder;
  auto output_iterator = icubaby::iterator{&transcoder, std::back_inserter (out)};
#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L
  output_iterator = std::ranges::copy (input, output_iterator).out;
#else
  output_iterator = std::copy (std::begin (input), std::end (input), output_iterator);
#endif
  (void)transcoder.end_cp (output_iterator);
}

}  // namespace

int main () {
  icubaby::char8 const* input = u8"こんにちは世界\n";
  show (std::cout, std::basic_string_view (input));
  show (std::cout, convert (input));
  show (std::cout, convert2 (input));
  c3 ();
  c4 ();
  c5 ();

  enum class code_point : char32_t {
    cjk_unified_ideograph_2070e = char32_t{0x2070E},
    cjk_unified_ideograph_20731 = char32_t{0x20731},
    cjk_unified_ideograph_20779 = char32_t{0x20779},
    cjk_unified_ideograph_20c53 = char32_t{0x20C53},
  };

  icubaby::t32_16 transcode;
  std::vector<char16_t> out;
  auto out_it = std::back_inserter (out);
  out_it = transcode (static_cast<char32_t> (code_point::cjk_unified_ideograph_2070e), out_it);
  out_it = transcode (static_cast<char32_t> (code_point::cjk_unified_ideograph_20731), out_it);
  out_it = transcode (static_cast<char32_t> (code_point::cjk_unified_ideograph_20779), out_it);
  out_it = transcode (static_cast<char32_t> (code_point::cjk_unified_ideograph_20c53), out_it);
  show (std::cout, out);
}
