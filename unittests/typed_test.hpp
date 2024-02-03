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

#ifndef ICUBABY_UNITTESTS_TYPED_TEST_HPP
#define ICUBABY_UNITTESTS_TYPED_TEST_HPP

#include <gtest/gtest.h>

#include <string>

#include "icubaby/icubaby.hpp"

// TODO(paul): Remove this code!
// A specialization of the gtest GetTypeName<char8_t>() function. This is required for compiling with (at least)
// Xcode 14.1/15.2 where we have a link error due to missing typeinfo for char8_t.
#if defined(__cpp_char8_t) && defined(__cpp_lib_char8_t)
namespace testing::internal {

template <> inline std::string GetTypeName<char8_t> () {
  return "char8_t";
}

}  // end namespace testing::internal
#endif

[[noreturn, maybe_unused]] inline void unreachable () {
  // Uses compiler specific extensions if possible. Even if no extension is used, undefined behavior is still raised by
  // an empty function body and the noreturn attribute.
#ifdef __GNUC__  // GCC, Clang, ICC
  __builtin_unreachable ();
#elif defined(_MSC_VER)  // MSVC
  __assume (false);
#endif
}

/// A type that is always false. Used to improve the failure mesages from
/// static_assert().
template <typename... T> [[maybe_unused]] constexpr bool always_false = false;

class OutputTypeNames {
public:
  template <typename T> static std::string GetName (int index) {
    (void)index;
    if constexpr (std::is_same<T, icubaby::char8> ()) {
      return "icubaby::char8";
    } else if constexpr (std::is_same<T, char16_t> ()) {
      return "char16_t";
    } else if constexpr (std::is_same<T, char32_t> ()) {
      return "char32_t";
    } else {
      static_assert (always_false<T>, "non-exhaustive visitor");
      unreachable ();
    }
  }
};

using OutputTypes = testing::Types<icubaby::char8, char16_t, char32_t>;

#endif  // ICUBABY_UNITTESTS_TYPED_TEST_HPP
