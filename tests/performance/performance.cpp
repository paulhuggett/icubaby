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
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string_view>
#include <vector>

#include "icubaby/icubaby.hpp"

using icubaby::char8;

namespace {

template <typename CharType> class single_code_point {
public:
  constexpr std::size_t size () const noexcept { return size_; }
  constexpr std::size_t max_size () const noexcept { return max_size_; }

  constexpr auto begin () noexcept { return cus_.begin (); }
  constexpr auto begin () const noexcept { return cus_.begin (); }
  constexpr auto end () noexcept { return cus_.begin () + size_; }
  constexpr auto end () const noexcept { return cus_.begin () + size_; }

  constexpr void push_back (CharType const &value) {
    assert (this->size () < this->max_size ());
    cus_[size_++] = value;
  }
  constexpr void push_back (CharType &&value) {
    assert (this->size () < this->max_size ());
    cus_[size_++] = std::move (value);
  }

  class output_iterator {
  public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = void;

    constexpr explicit output_iterator (single_code_point *const scp) noexcept : container_{scp} {}
    constexpr output_iterator &operator= (CharType const &value) {
      container_->push_back (value);
      return *this;
    }
    constexpr output_iterator &operator= (CharType &&value) {
      container_->push_back (std::move (value));
      return *this;
    }
    constexpr output_iterator &operator* () noexcept { return *this; }
    constexpr output_iterator &operator++ () noexcept { return *this; }
    constexpr output_iterator operator++ (int) noexcept { return *this; }

  private:
    single_code_point *container_;
  };

private:
  static constexpr auto max_size_ = icubaby::longest_sequence<CharType> ();
  std::size_t size_ = 0;
  std::array<CharType, max_size_> cus_;
};

template <typename CharType>
constexpr single_code_point<CharType>::output_iterator back_inserter (single_code_point<CharType> &container) {
  using iterator = single_code_point<CharType>::output_iterator;
  return iterator{&container};
}

constexpr auto num_high_surrogates = static_cast<std::uint_least32_t> (icubaby::last_high_surrogate) -
                                     static_cast<std::uint_least32_t> (icubaby::first_high_surrogate) + 1;
constexpr auto num_low_surrogates = static_cast<std::uint_least32_t> (icubaby::last_low_surrogate) -
                                    static_cast<std::uint_least32_t> (icubaby::first_low_surrogate) + 1;
constexpr auto total_code_points =
    static_cast<std::uint_least32_t> (icubaby::max_code_point) + 1U - 1U - num_high_surrogates - num_low_surrogates;

template <typename Encoding> std::vector<single_code_point<Encoding>> all_code_points () {
  std::vector<single_code_point<Encoding>> result;
  result.reserve (total_code_points);

  for (auto code_point = char32_t{0}; code_point <= icubaby::max_code_point; ++code_point) {
    if (!icubaby::is_surrogate (code_point) && code_point != icubaby::byte_order_mark) {
      icubaby::transcoder<char32_t, Encoding> transcoder;
      single_code_point<Encoding> scp;
      (void)transcoder.end_cp (transcoder (code_point, back_inserter (scp)));
      result.push_back (std::move (scp));
    }
  }
  assert (result.size () == total_code_points);
  result.shrink_to_fit ();
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

template <typename FromEncoding, typename ToEncoding> void go (unsigned long const iterations) {
  std::cout << name<FromEncoding>::value << " -> " << name<ToEncoding>::value << ": " << std::flush;

  std::chrono::steady_clock timer;
  std::vector<ToEncoding> output;
  output.resize (icubaby::longest_sequence<ToEncoding> () * total_code_points);
  icubaby::transcoder<FromEncoding, ToEncoding> transcoder;
  auto const all = all_code_points<FromEncoding> ();
  auto const start_time = timer.now ();
  for (auto ctr = 0UL; ctr < iterations; ++ctr) {
    auto output_iterator = output.begin ();
    for (auto const &cus : all) {
      for (auto cu : cus) {
        output_iterator = transcoder (cu, output_iterator);
      }
    }
    (void)transcoder.end_cp (output_iterator);
  }

  auto const elapsed = timer.now () - start_time;
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds> (elapsed).count () /
                   static_cast<double> (iterations)
            << " ms" << std::endl;
}

}  // end anonymous namespace

int main (int argc, const char *argv[]) {
  int exit_code = EXIT_SUCCESS;
  try {
    auto iterations = 1024UL;
    if (argc > 2) {
      std::cout << argv[0] << ": [iterations]\n";
      return EXIT_FAILURE;
    }
    if (argc > 1) {
      auto pos = std::size_t{0};
      auto const arg = std::string{argv[1]};
      iterations = std::stoul (arg, &pos);
      if (pos != arg.length ()) {
        throw std::invalid_argument ("invalid iteration count");
      }
    }

    std::cout << "Time to transcode all code points:" << std::endl;
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
