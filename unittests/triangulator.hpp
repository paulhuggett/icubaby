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

#ifndef ICUBABY_TRIANGULATOR_HPP
#define ICUBABY_TRIANGULATOR_HPP (1)

#include <type_traits>

#include "icubaby/icubaby.hpp"

/// \brief A "triangulator" converts from the \p FromEncoding encoding to the \p ToEncoding encoding via an
///        intermediate UTF-32 encoding.
///
/// Each of the library's transcoders converts directly between its input and output encodings. The triangulator is a
/// simple reference implementation against which the unit tests check the direct transcoders.
///
/// \tparam FromEncoding  The source encoding.
/// \tparam ToEncoding  The destination encoding.
template <typename FromEncoding, typename ToEncoding> class triangulator {
public:
  /// The type of the code units consumed by this transcoder.
  using input_type = FromEncoding;
  /// The type of the code units produced by this transcoder.
  using output_type = ToEncoding;

  /// Accepts a code unit in the source encoding (as given by triangulator::input_type). These are first converted
  /// to UTF-32 and then to the output encoding (triangulator::output_type). As output code units are generated,
  /// they are written to the output iterator \p dest.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param code_unit  A code unit in the source encoding.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
  OutputIterator operator() (input_type code_unit, OutputIterator dest) noexcept (
      icubaby::details::is_nothrow_output_v<OutputIterator, output_type>) {
    // The UTF-32 output from intermediate_ is passed directly to output_.
    return intermediate_ (code_unit, icubaby::iterator{&output_, dest}).base ();
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
  OutputIterator end_cp (OutputIterator dest) noexcept (
      icubaby::details::is_nothrow_output_v<OutputIterator, output_type>) {
    return output_.end_cp (intermediate_.end_cp (icubaby::iterator{&output_, dest}).base ());
  }

  /// \returns True if the input passed to operator() was valid.
  [[nodiscard]] constexpr bool well_formed () const noexcept {
    return intermediate_.well_formed () && output_.well_formed ();
  }

  /// \returns True if a partial code-point has been passed to operator() and
  /// false otherwise.
  [[nodiscard]] constexpr bool partial () const noexcept { return intermediate_.partial (); }

private:
  /// We use the intermediate_ transcoder to convert from the input encoding to UTF-32.
  icubaby::transcoder<input_type, char32_t> intermediate_{};
  /// The output_ transcoder converts from the intermediate (UTF-32) encoding to the selected output encoding.
  icubaby::transcoder<char32_t, output_type> output_{};
  // The UTF-32 output of intermediate_ is written to output_ using an icubaby::iterator. If intermediate_ and output_
  // had the same type, intermediate_.end_cp() would choose its overload for an icubaby::iterator of its own type.
  static_assert (!std::is_same_v<decltype (intermediate_), decltype (output_)>,
                 "A UTF-32 to UTF-32 triangulator is not supported");
};

#endif  // ICUBABY_TRIANGULATOR_HPP
