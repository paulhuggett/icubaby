//===- tests/performance/performance.cpp ----------------------------------===//
//*                   __                                            *
//*  _ __   ___ _ __ / _| ___  _ __ _ __ ___   __ _ _ __   ___ ___  *
//* | '_ \ / _ \ '__| |_ / _ \| '__| '_ ` _ \ / _` | '_ \ / __/ _ \ *
//* | |_) |  __/ |  |  _| (_) | |  | | | | | | (_| | | | | (_|  __/ *
//* | .__/ \___|_|  |_|  \___/|_|  |_| |_| |_|\__,_|_| |_|\___\___| *
//* |_|                                                             *
//===----------------------------------------------------------------------===//
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
//===----------------------------------------------------------------------===//
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "icubaby/icubaby.hpp"

using icubaby::char8;

namespace {

constexpr auto num_code_points = static_cast<std::uint_least32_t> (icubaby::max_code_point) + 1U;
constexpr auto num_high_surrogates = static_cast<std::uint_least32_t> (icubaby::last_high_surrogate) -
                                     static_cast<std::uint_least32_t> (icubaby::first_high_surrogate) + 1U;
constexpr auto num_low_surrogates = static_cast<std::uint_least32_t> (icubaby::last_low_surrogate) -
                                    static_cast<std::uint_least32_t> (icubaby::first_low_surrogate) + 1U;
constexpr auto total_code_points = num_code_points - 1U - num_high_surrogates - num_low_surrogates;

template <typename Encoding, typename OutputIterator>
constexpr OutputIterator convert_code_point (char32_t const code_point, OutputIterator const inserter) {
  icubaby::transcoder<char32_t, Encoding> transcoder;
  auto const result = transcoder.end_cp (transcoder (code_point, inserter));
  assert (transcoder.well_formed ());
  return result;
}

template <typename Encoding> struct all_code_points {
  std::vector<Encoding> code_units;
  std::vector<std::uint_least8_t> sizes;
};

template <typename Encoding> all_code_points<Encoding> make_all_code_points () {
  all_code_points<Encoding> result;
  result.sizes.reserve (total_code_points);

  auto old_size = std::size_t{0};
  auto inserter = std::back_inserter (result.code_units);

  for (auto code_point = char32_t{0}; code_point <= icubaby::max_code_point; ++code_point) {
    if (!icubaby::is_surrogate (code_point) && code_point != icubaby::byte_order_mark) {
      inserter = convert_code_point<Encoding> (code_point, inserter);

      auto const new_size = result.code_units.size ();
      assert (new_size - old_size <= icubaby::longest_sequence_v<Encoding>);
      assert (new_size - old_size > 0);
      result.sizes.push_back (static_cast<std::uint_least8_t> (new_size - old_size));
      old_size = new_size;
    }
  }
  assert (result.sizes.size () == total_code_points);
  return result;
}

template <typename Encoding> struct name {};
template <> struct name<char8> {
  static constexpr auto value = "UTF-8";
};
template <> struct name<char16_t> {
  static constexpr auto value = "UTF-16";
};
template <> struct name<char32_t> {
  static constexpr auto value = "UTF-32";
};

#if defined(_MSC_VER)
#define ICUBABY_NOINLINE [[msvc::noinline]]
#elif defined(__clang__)
#if defined(__clang_major__) && __clang_major__ >= 15
#define ICUBABY_NOINLINE [[clang::noinline]]
#else
#define ICUBABY_NOINLINE __attribute__((noinline))
#endif  // __clang_major__
#elif defined(__GNUC__)
#define ICUBABY_NOINLINE [[gnu::noinline]]
#else
#define ICUBABY_NOINLINE
#endif

template <typename FromEncoding, typename ToEncoding> ICUBABY_NOINLINE void go (std::uint_least16_t const iterations) {
  std::cout << name<FromEncoding>::value << " -> " << name<ToEncoding>::value << ": " << std::flush;

  std::vector<ToEncoding> output;
  output.resize (icubaby::longest_sequence<ToEncoding> () * total_code_points);
  icubaby::transcoder<FromEncoding, ToEncoding> transcoder;
  auto const all = make_all_code_points<FromEncoding> ();
  auto const start_time = std::chrono::steady_clock::now ();

  for (auto iteration = std::uint_least16_t{0}; iteration < iterations; ++iteration) {
    auto output_iterator = output.begin ();
    auto cu_pos = all.code_units.begin ();
    for (auto const cu_count : all.sizes) {
      for (auto cu_ctr = 0U; cu_ctr < cu_count; ++cu_ctr) {
        output_iterator = transcoder (*cu_pos, output_iterator);
        ++cu_pos;
      }
      (void)transcoder.end_cp (output_iterator);
      assert (transcoder.well_formed ());
    }
  }

  auto const elapsed = std::chrono::steady_clock::now () - start_time;
  std::cout << static_cast<double> (std::chrono::duration_cast<std::chrono::milliseconds> (elapsed).count ()) /
                   static_cast<double> (iterations)
            << " ms\n"
            << std::flush;
}

std::uint_least16_t iteration_count (std::string_view const str) {
  auto pos = std::size_t{0};
  auto const iterations = std::stoul (std::string{str}, &pos);
  if (pos != str.length ()) {
    throw std::invalid_argument ("invalid iteration count");
  }
  if constexpr (std::numeric_limits<decltype (iterations)>::max () > std::numeric_limits<std::uint_least16_t>::max ()) {
    if (iterations > std::numeric_limits<std::uint_least16_t>::max ()) {
      throw std::invalid_argument ("iteration count too large");
    }
  }
  return static_cast<std::uint_least16_t> (iterations);
}

}  // end anonymous namespace

int main (int const argc, char const *argv[]) {
  auto exit_code = EXIT_SUCCESS;
  try {
    if (argc > 2) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      std::cout << argv[0] << ": [iterations]\n";
      return EXIT_FAILURE;
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    auto const iterations = iteration_count (argc > 1 ? argv[1] : "16");
    std::cout << "Time to transcode all code points (" << iterations << " iterations):\n" << std::flush;
    go<char8, char8> (iterations);
    go<char8, char16_t> (iterations);
    go<char8, char32_t> (iterations);
    go<char16_t, char8> (iterations);
    go<char16_t, char16_t> (iterations);
    go<char16_t, char32_t> (iterations);
    go<char32_t, char8> (iterations);
    go<char32_t, char16_t> (iterations);
    go<char32_t, char32_t> (iterations);
  } catch (std::exception const &ex) {
    std::cerr << "Error: " << ex.what () << '\n';
    exit_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown Error\n";
    exit_code = EXIT_FAILURE;
  }
  return exit_code;
}
