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

template <typename C>
struct char_to_code;
template <>
struct char_to_code<icubaby::char8> {
  static constexpr auto code () { return "UTF-8"; }
};
template <>
struct char_to_code<char16_t> {
  static constexpr auto code () {
    static_assert (
        endian::native == endian::little || endian::native == endian::big,
        "Don't know the iconv encoding name for a mixed endian system");
    if constexpr (endian::native == endian::little) {
      return "UTF16LE";
    } else {
      return "UTF16BE";
    }
  }
};
template <>
struct char_to_code<char32_t> {
  static constexpr auto code () {
    static_assert (
        endian::native == endian::little || endian::native == endian::big,
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
  explicit unsupported_conversion (int erc)
      : std::system_error{std::error_code{erc, std::generic_category ()}} {}
};

template <typename C>
std::vector<C> convert_using_iconv (std::vector<char32_t> const &in) {
  iconv_t cd =
      iconv_open (char_to_code<C>::code (), char_to_code<char32_t>::code ());
  if (cd == reinterpret_cast<iconv_t> (-1)) {
    int const erc = errno;
    if (erc == EINVAL) {
      throw unsupported_conversion{EINVAL};
    }
    throw std::system_error{std::error_code{erc, std::generic_category ()},
                            "iconv_open"};
  }

  std::vector<C> out;
  // The initial output buffer. We guess that the output is likely to require
  // at least twice as many code units as the input...
  out.resize (in.size () * 2);
  auto total_out_bytes = size_t{0};
  auto const *inbuf = in.data ();
  size_t in_bytes_left = sizeof (char32_t) * in.size ();
  while (in_bytes_left > 0) {
    auto const out_size = out.size ();
    auto const out_bytes_available = sizeof (C) * out_size - total_out_bytes;
    auto out_bytes_left = out_bytes_available;
    auto outbuf = reinterpret_cast<char *> (out.data ()) + total_out_bytes;
    if (iconv (cd, reinterpret_cast<char **> (const_cast<char32_t **> (&inbuf)),
               &in_bytes_left, &outbuf,
               &out_bytes_left) == static_cast<size_t> (-1)) {
      int const erc = errno;
      // E2BIG tells us that the output buffer was too small.
      if (erc != E2BIG) {
        throw std::system_error{std::error_code{erc, std::generic_category ()},
                                "iconv"};
      }
      // The output buffer did not have enough space, so we increase it by 50%.
      out.resize (out_size + out_size / 2);
    }
    total_out_bytes += out_bytes_available - out_bytes_left;
  }
  assert (total_out_bytes % sizeof (C) == 0);
  out.resize (total_out_bytes / sizeof (C));
  iconv_close (cd);
  return out;
}

std::vector<char32_t> all_code_points () {
  std::vector<char32_t> result;
  for (auto cp = char32_t{0}; cp <= icubaby::max_code_point; ++cp) {
    if (!icubaby::is_surrogate (cp)) {
      result.push_back (cp);
    }
  }
  return result;
}

template <typename T>
void check (std::vector<char32_t> const &all) {
  static constexpr bool trace_failure = false;

  try {
    std::vector<T> baby_out;
    icubaby::transcoder<char32_t, T> convert_32_8;
    auto it = std::copy (
        std::begin (all), std::end (all),
        icubaby::iterator{&convert_32_8, std::back_inserter (baby_out)});
    it = convert_32_8.end_cp (it);
    assert (convert_32_8.good ());

    std::vector<T> iconv_out = convert_using_iconv<T> (all);

    if constexpr (trace_failure) {
      std::cout << iconv_out.size () << '\n';
      std::cout << baby_out.size () << '\n';

      for (size_t ctr = 0, end = std::min (iconv_out.size (), baby_out.size ());
           ctr < end; ++ctr) {
        std::cout << std::hex << static_cast<unsigned> (iconv_out[ctr]) << ' '
                  << std::hex << static_cast<unsigned> (baby_out[ctr]) << '\n';
      }
    }
    assert (iconv_out.size () == baby_out.size ());
    assert (std::equal (std::begin (iconv_out), std::end (iconv_out),
                        std::begin (baby_out)));
  } catch (unsupported_conversion const &) {
    std::cout << "Skipping " << char_to_code<T>::code ()
              << " iconv test: conversion not supported\n";
  }
}

}  // end anonymous namespace

int main () {
  int exit_code = EXIT_SUCCESS;
  try {
    auto const all = all_code_points ();
    // Compare iconv and icubaby conversion of UTF-32 to UTF-8 sequences
    check<icubaby::char8> (all);
    // Compare iconv and icubaby conversion of UTF-32 to UTF-16 sequences
    check<char16_t> (all);
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
