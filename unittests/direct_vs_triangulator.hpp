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

#ifndef ICUBABY_DIRECT_VS_TRIANGULATOR_HPP
#define ICUBABY_DIRECT_VS_TRIANGULATOR_HPP (1)

#include <gtest/gtest.h>

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

/// Checks that a transcoder which converts directly between two encodings behaves identically to
/// icubaby::details::triangulator<> which converts between the same encodings via an intermediate UTF-32 encoding.
/// The output and the well_formed() and partial() states are compared after every input code unit and after the
/// call to end_cp().
///
/// The object retains its output buffers between calls to avoid repeated memory allocation when performing a large
/// number of checks.
template <typename FromEncoding, typename ToEncoding> class direct_vs_triangulator {
public:
  using direct_type = icubaby::transcoder<FromEncoding, ToEncoding>;
  using indirect_type = icubaby::details::triangulator<FromEncoding, ToEncoding>;

  testing::AssertionResult operator() (std::initializer_list<FromEncoding> input) {
    return this->check (std::begin (input), std::end (input));
  }
  template <typename Container> testing::AssertionResult operator() (Container const& input) {
    return this->check (std::begin (input), std::end (input));
  }

private:
  std::vector<ToEncoding> direct_out_;
  std::vector<ToEncoding> indirect_out_;

  template <typename Iterator> testing::AssertionResult check (Iterator first, Iterator last) {
    direct_type direct;
    indirect_type indirect;
    direct_out_.clear ();
    indirect_out_.clear ();
    auto index = std::size_t{0};
    for (; first != last; ++first, ++index) {
      (void)direct (*first, std::back_inserter (direct_out_));
      (void)indirect (*first, std::back_inserter (indirect_out_));
      if (auto result = compare (direct, indirect); !result) {
        return result << " after input code unit #" << index;
      }
    }
    (void)direct.end_cp (std::back_inserter (direct_out_));
    (void)indirect.end_cp (std::back_inserter (indirect_out_));
    if (auto result = compare (direct, indirect); !result) {
      return result << " after end_cp()";
    }
    return testing::AssertionSuccess ();
  }

  testing::AssertionResult compare (direct_type const& direct, indirect_type const& indirect) const {
    if (direct_out_ != indirect_out_) {
      return testing::AssertionFailure () << "output mismatch: " << testing::PrintToString (direct_out_) << " vs. "
                                          << testing::PrintToString (indirect_out_);
    }
    if (direct.well_formed () != indirect.well_formed ()) {
      return testing::AssertionFailure ()
             << "well_formed() mismatch: " << direct.well_formed () << " vs. " << indirect.well_formed ();
    }
    if (direct.partial () != indirect.partial ()) {
      return testing::AssertionFailure ()
             << "partial() mismatch: " << direct.partial () << " vs. " << indirect.partial ();
    }
    return testing::AssertionSuccess ();
  }
};

#endif  // ICUBABY_DIRECT_VS_TRIANGULATOR_HPP
