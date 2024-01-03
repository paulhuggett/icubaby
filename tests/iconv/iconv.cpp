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
#include <iconv.h>

#include <bit>
#include <cassert>
#include <iostream>
#include <system_error>
#include <vector>

#include "icubaby/icubaby.hpp"

namespace {

#if __cpp_lib_endian
using std::endian;
#else
enum class endian {
#ifdef _WIN32
  little = 0,
  big = 1,
  native = little
#else
  little = __ORDER_LITTLE_ENDIAN__,
  big = __ORDER_BIG_ENDIAN__,
  native = __BYTE_ORDER__
#endif
};
#endif  // __cpp_lib_endian

template <typename C> struct char_to_code;
template <> struct char_to_code<icubaby::char8> {
  static constexpr auto code () { return "UTF-8"; }
};
template <> struct char_to_code<char16_t> {
  static constexpr auto code () {
    static_assert (endian::native == endian::little || endian::native == endian::big,
                   "Don't know the iconv encoding name for a mixed endian system");
    if constexpr (endian::native == endian::little) {
      return "UTF16LE";
    } else {
      return "UTF16BE";
    }
  }
};
template <> struct char_to_code<char32_t> {
  static constexpr auto code () {
    static_assert (endian::native == endian::little || endian::native == endian::big,
                   "Don't know the iconv encoding name for a mixed endian system");
    if constexpr (endian::native == endian::little) {
      return "UTF32LE";
    } else {
      return "UTF32BE";
    }
  }
};

class unsupported_conversion : public std::system_error {
public:
  explicit unsupported_conversion (int erc) : std::system_error{std::error_code{erc, std::generic_category ()}} {}
};

template <typename C> void convert_using_iconv (std::vector<char32_t> const &in, std::vector<C> &out) {
  using from_encoding = char32_t;
  iconv_t cd = iconv_open (char_to_code<C>::code (), char_to_code<from_encoding>::code ());
  if (cd == reinterpret_cast<iconv_t> (-1)) {
    int const erc = errno;
    if (erc == EINVAL) {
      throw unsupported_conversion{EINVAL};
    }
    throw std::system_error{std::error_code{erc, std::generic_category ()}, "iconv_open"};
  }

  out.clear ();
  // The initial output buffer. We guess that the output is likely to require
  // at least twice as many code units as the input...
  out.resize (in.size () * icubaby::longest_sequence_v<C>);
  auto total_out_bytes = size_t{0};
  auto const *inbuf = in.data ();
  size_t in_bytes_left = sizeof (from_encoding) * in.size ();
  while (in_bytes_left > 0) {
    auto const out_size = out.size ();
    auto const out_bytes_available = sizeof (C) * out_size - total_out_bytes;
    auto out_bytes_left = out_bytes_available;
    // NOLINTNEXTLINE
    auto *outbuf = reinterpret_cast<char *> (out.data ()) + total_out_bytes;
    // NOLINTNEXTLINE
    if (iconv (cd, reinterpret_cast<char **> (const_cast<from_encoding **> (&inbuf)), &in_bytes_left, &outbuf,
               &out_bytes_left) == static_cast<size_t> (-1)) {
      // E2BIG tells us that the output buffer was too small.
      if (int const erc = errno; erc != E2BIG) {
        throw std::system_error{std::error_code{erc, std::generic_category ()}, "iconv"};
      }
      // The output buffer did not have enough space, so we increase it by 50%.
      out.resize (out_size + out_size / 2);
    }
    total_out_bytes += out_bytes_available - out_bytes_left;
  }
  assert (total_out_bytes % sizeof (C) == 0);
  out.resize (total_out_bytes / sizeof (C));
  iconv_close (cd);
}

std::vector<char32_t> all_code_points () {
  std::vector<char32_t> result;
  for (auto cp = char32_t{0}; cp <= icubaby::max_code_point; ++cp) {
    if (!icubaby::is_surrogate (cp) && cp != icubaby::bom) {
      result.push_back (cp);
    }
  }
  return result;
}

template <typename T, typename = std::enable_if_t<icubaby::is_unicode_char_type_v<T>>>
[[nodiscard]] bool check (std::vector<char32_t> const &all) {
  static constexpr bool trace_failure = true;

  try {
    // Pass the collection of input characters through the icubaby UTF-32 to UTF-8 converter.
    std::vector<T> baby_out;
    baby_out.reserve (all.size () * icubaby::longest_sequence_v<T>);
    icubaby::transcoder<char32_t, T> convert_32_8;
    auto it =
        std::copy (std::begin (all), std::end (all), icubaby::iterator{&convert_32_8, std::back_inserter (baby_out)});
    it = convert_32_8.end_cp (it);
    assert (convert_32_8.well_formed ());

    // Pass the collection of input characters through the libiconv UTF-32 to UTF-8 converter.
    std::vector<T> iconv_out;
    iconv_out.reserve (all.size () * icubaby::longest_sequence_v<T>);
    convert_using_iconv<T> (all, iconv_out);

    // Compare the output of the two converters.
    if (!std::equal (std::begin (iconv_out), std::end (iconv_out), std::begin (baby_out), std::end (baby_out))) {
      if constexpr (trace_failure) {
        std::cerr << "FAILURE\n";
        std::cout << "iconv output size=" << iconv_out.size () << '\n';
        std::cout << "icubaby output size=" << baby_out.size () << '\n';

        for (size_t ctr = 0, end = std::min (iconv_out.size (), baby_out.size ()); ctr < end; ++ctr) {
          if (iconv_out[ctr] != baby_out[ctr]) {
            std::cout << std::hex << ctr << ": "
                      << std::hex << static_cast<uint_least32_t> (all[ctr]) << "-> "
                      << std::hex << static_cast<unsigned> (iconv_out[ctr]) << ' '
                      << std::hex << static_cast<unsigned> (baby_out[ctr]) << '\n';
          }
        }
      }
      return false;
    }

  } catch (unsupported_conversion const &) {
    std::cout << "Skipping " << char_to_code<T>::code () << " iconv test: conversion not supported\n";
  }
  return true;
}

}  // end anonymous namespace

int main () {
  int exit_code = EXIT_SUCCESS;
  try {
    auto const all = all_code_points ();
    // Compare iconv and icubaby conversion of UTF-32 to UTF-8 sequences
    std::cout << "Check UTF-32 to UTF-8 conversion for all code-points\n";
    if (!check<icubaby::char8> (all)) {
      return EXIT_FAILURE;
    }
    // Compare iconv and icubaby conversion of UTF-32 to UTF-16 sequences
    std::cout << "Check UTF-32 to UTF-16 conversion for all code-points\n";
    if (!check<char16_t> (all)) {
      return EXIT_FAILURE;
    }
    std::cout << "iconv tests passed\n";
  } catch (std::exception const &ex) {
    std::cerr << "An error occurred: " << ex.what () << '\n';
    exit_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred\n";
    exit_code = EXIT_FAILURE;
  }
  return exit_code;
}
