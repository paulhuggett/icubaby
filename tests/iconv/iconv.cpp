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

#include <iconv.h>

#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <vector>

#include "icubaby/icubaby.hpp"

namespace {

#if defined(__cpp_lib_endian) && __cpp_lib_endian >= 201907L
using std::endian;
#elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
enum class endian { little = __ORDER_LITTLE_ENDIAN__, big = __ORDER_BIG_ENDIAN__, native = __BYTE_ORDER__ };
#elif defined(_WIN32) || defined(__i386__) || defined(__x86_64__)
enum class endian { little = 0, big = 1, native = little };
#else
#error "Can't determine endianness of the target system"
#endif
static_assert (endian::native == endian::big || endian::native == endian::little, "Endianness must be big or little.");

// char to code
// ~~~~~~~~~~~~
template <typename C> struct char_to_code;
template <> struct char_to_code<icubaby::char8> {
  [[nodiscard]] static constexpr auto code () noexcept { return "UTF-8"; }
};
template <> struct char_to_code<char16_t> {
  [[nodiscard]] static constexpr auto code () noexcept {
    if constexpr (endian::native == endian::little) {
      return "UTF16LE";
    } else {
      return "UTF16BE";
    }
  }
};
template <> struct char_to_code<char32_t> {
  [[nodiscard]] static constexpr auto code () noexcept {
    if constexpr (endian::native == endian::little) {
      return "UTF32LE";
    } else {
      return "UTF32BE";
    }
  }
};

template <typename ResultType, typename ArgType, typename = std::enable_if_t<std::is_pointer_v<ResultType>>>
constexpr ResultType pointer_cast (ArgType *const ptr) noexcept {
#if defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806L
  return std::bit_cast<ResultType> (ptr);
#else
  ResultType result = nullptr;
  static_assert (sizeof (ptr) == sizeof (result));
  (void)std::memcpy (&result, &ptr, sizeof (result));
  return result;
#endif
}

class iconv_unsupported_conversion : public std::invalid_argument {
public:
  iconv_unsupported_conversion () : std::invalid_argument{"conversion is not supported by iconv"} {}
};

template <typename FromEncoding, typename ToEncoding> class iconv_converter {
public:
  iconv_converter ();
  iconv_converter (iconv_converter const &) = delete;
  iconv_converter (iconv_converter &&) noexcept = delete;

  ~iconv_converter () noexcept;

  iconv_converter &operator= (iconv_converter const &) = delete;
  iconv_converter &operator= (iconv_converter &&) noexcept = delete;

  /// Convert a container full of code-points in FromEncoding to a new container full of ToEncoding code-points.
  std::vector<ToEncoding> convert (std::vector<FromEncoding> const &input);
  /// Close the iconv conversion descriptor. It is safe to call this function more than once.
  void close ();

private:
  /// \returns The value (iconv_t)-1 which the iconv library uses to indicate failure.
  static constexpr iconv_t bad () noexcept;

  iconv_t descriptor_ = iconv_open (char_to_code<ToEncoding>::code (), char_to_code<FromEncoding>::code ());
};

// (ctor)
// ~~~~~~
template <typename FromEncoding, typename ToEncoding> iconv_converter<FromEncoding, ToEncoding>::iconv_converter () {
  if (descriptor_ == bad ()) {
    auto const erc = errno;
    static_assert (std::is_integral_v<decltype (erc)>, "Expected errno to yield an integer type");
    if (erc == EINVAL) {
      throw iconv_unsupported_conversion{};
    }
    throw std::system_error{std::error_code{erc, std::generic_category ()}, "iconv_open"};
  }
}

// (dtor)
// ~~~~~~
template <typename FromEncoding, typename ToEncoding>
iconv_converter<FromEncoding, ToEncoding>::~iconv_converter () noexcept {
  if (descriptor_ != bad ()) {
    // Note that we don't throw in the event of an error here.
    (void)iconv_close (descriptor_);
  }
}

// convert
// ~~~~~~~
template <typename FromEncoding, typename ToEncoding>
std::vector<ToEncoding> iconv_converter<FromEncoding, ToEncoding>::convert (std::vector<FromEncoding> const &input) {
  std::vector<ToEncoding> out;
  out.resize (input.size ());
  auto total_out_bytes = std::size_t{0};
  auto const *inbuf = input.data ();
  std::size_t in_bytes_left = sizeof (FromEncoding) * input.size ();
  while (in_bytes_left > 0) {
    auto const out_size = out.size ();
    auto const out_bytes_available = sizeof (ToEncoding) * out_size - total_out_bytes;
    std::size_t out_bytes_left = out_bytes_available;
    // NOLINTBEGIN(cppcoreguidelines-pro-type-const-cast)
    if (auto *outbuf = pointer_cast<char *> (out.data ()) + total_out_bytes;
        iconv (descriptor_, pointer_cast<char **> (const_cast<FromEncoding **> (&inbuf)), &in_bytes_left, &outbuf,
               &out_bytes_left) == static_cast<std::size_t> (-1)) {
      // NOLINTEND(cppcoreguidelines-pro-type-const-cast)
      // E2BIG tells us that the output buffer was too small.
      auto const erc = errno;
      static_assert (std::is_integral_v<decltype (erc)>, "Expected errno to yield an integer type");
      if (erc != E2BIG) {
        throw std::system_error{std::error_code{erc, std::generic_category ()}, "iconv failed"};
      }
      // The output buffer did not have enough space, so we increase it by 50%.
      out.resize (out_size + out_size / 2);
    }
    total_out_bytes += out_bytes_available - out_bytes_left;
  }
  assert (total_out_bytes % sizeof (ToEncoding) == 0);
  out.resize (total_out_bytes / sizeof (ToEncoding));
  return out;
}

// close
// ~~~~~
template <typename FromEncoding, typename ToEncoding> void iconv_converter<FromEncoding, ToEncoding>::close () {
  if (descriptor_ == bad ()) {
    return;
  }
  auto const good = iconv_close (descriptor_) == 0;
  descriptor_ = bad ();
  if (!good) {
    throw std::system_error{std::error_code{errno, std::generic_category ()}, "iconv_close failed"};
  }
}

template <typename FromEncoding, typename ToEncoding>
constexpr iconv_t iconv_converter<FromEncoding, ToEncoding>::bad () noexcept {
#if defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806L
  return std::bit_cast<iconv_t> (std::intptr_t{-1});
#else
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr)
  return reinterpret_cast<iconv_t> (std::intptr_t{-1});
#endif
}

// convert using iconv
// ~~~~~~~~~~~~~~~~~~~
template <typename C, typename = std::enable_if_t<icubaby::is_unicode_char_type_v<C>>>
std::vector<C> convert_using_iconv (std::vector<char32_t> const &input) {
  iconv_converter<char32_t, C> converter;
  auto result = converter.convert (input);
  converter.close ();
  return result;
}

// convert using icubaby
// ~~~~~~~~~~~~~~~~~~~~~
template <typename ToEncoding, typename = std::enable_if_t<icubaby::is_unicode_char_type_v<ToEncoding>>>
std::vector<ToEncoding> convert_using_icubaby (std::vector<char32_t> const &input) {
  std::vector<ToEncoding> out;
  out.reserve (input.size () * icubaby::longest_sequence_v<ToEncoding>);
  icubaby::transcoder<char32_t, ToEncoding> convert_32_x;
  auto pos =
      std::copy (std::begin (input), std::end (input), icubaby::iterator{&convert_32_x, std::back_inserter (out)});
  (void)convert_32_x.end_cp (pos);
  assert (convert_32_x.well_formed ());
  return out;
}

// all code points
// ~~~~~~~~~~~~~~~
std::vector<char32_t> all_code_points () {
  std::vector<char32_t> result;
  for (auto code_point = char32_t{0}; code_point <= icubaby::max_code_point; ++code_point) {
    if (!icubaby::is_surrogate (code_point) && code_point != icubaby::byte_order_mark) {
      result.push_back (code_point);
    }
  }
  return result;
}

// show diff
// ~~~~~~~~~
template <typename C, typename = std::enable_if_t<icubaby::is_unicode_char_type_v<C>>>
void show_diff (std::ostream &stream, std::vector<C> const &iconv_out, std::vector<C> const &baby_out) {
  stream << "iconv output size=" << iconv_out.size () << '\n';
  stream << "icubaby output size=" << baby_out.size () << '\n';

  auto iconv_pos = std::begin (iconv_out);
  auto baby_pos = std::begin (baby_out);
  auto ctr = std::size_t{0};

  using difference_type = typename std::iterator_traits<decltype (iconv_pos)>::difference_type;
  auto const end = iconv_pos + static_cast<difference_type> (std::min (iconv_out.size (), baby_out.size ()));
  while (iconv_pos != end) {
    if (*iconv_pos != *baby_pos) {
      stream << std::hex << ctr << ": " << std::hex << static_cast<std::uint_least32_t> (*iconv_pos) << ' ' << std::hex
             << static_cast<std::uint_least32_t> (baby_out[ctr]) << '\n';
    }
    ++iconv_pos;
    ++baby_pos;
    ++ctr;
  }
}

// check
// ~~~~~
template <typename T, typename = std::enable_if_t<icubaby::is_unicode_char_type_v<T>>>
[[nodiscard]] bool check (std::vector<char32_t> const &all) {
  static constexpr bool trace_failure = true;

  try {
    // Pass the collection of input characters through the icubaby UTF-32 to UTF-8/16 converter.
    std::vector<T> const baby_out = convert_using_icubaby<T> (all);

    // ... now through the libiconv UTF-32 to UTF-8/16 converter.
    std::vector<T> const iconv_out = convert_using_iconv<T> (all);

    // Compare the output of the two converters.
    if (!std::equal (std::begin (iconv_out), std::end (iconv_out), std::begin (baby_out), std::end (baby_out))) {
      if constexpr (trace_failure) {
        std::cerr << "FAILURE\n";
        show_diff (std::cout, iconv_out, baby_out);
      }
      return false;
    }
  } catch (iconv_unsupported_conversion const &ex) {
    std::cout << "Skipping " << char_to_code<T>::code () << " iconv test: " << ex.what () << '\n';
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
