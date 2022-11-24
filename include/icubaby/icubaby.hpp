/// \file icubaby.hpp

// UTF-8 to UTF-32 conversion is based on the "Flexible and Economical UTF-8
// Decoder" by Bjoern Hoehrmann <bjoern@hoehrmann.de> See
// http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
//
// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
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

#ifndef ICUBABY_ICUBABY_HPP
#define ICUBABY_ICUBABY_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

#if __cplusplus >= 202002L
#define ICUBABY_CXX20 (1)
#elif defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
// MSVC does not set the value of __cplusplus correctly unless the
// /Zc:__cplusplus is supplied. We have to detect C++20 using its
// compiler-specific macros instead.
#define ICUBABY_CXX20 (1)
#else
#define ICUBABY_CXX20 (0)
#endif

#if ICUBABY_CXX20
#define ICUBABY_CXX20REQUIRES(x) requires x
#else
#define ICUBABY_CXX20REQUIRES(x)
#endif  // ICUBABY_CXX20

namespace icubaby {

#if ICUBABY_CXX20
using char8 = char8_t;
#else
using char8 = char;
#endif

inline constexpr auto replacement_char = char32_t{0xFFFD};
inline constexpr auto first_high_surrogate = char32_t{0xD800};
inline constexpr auto last_high_surrogate = char32_t{0xDBFF};
inline constexpr auto first_low_surrogate = char32_t{0xDC00};
inline constexpr auto last_low_surrogate = char32_t{0xDFFF};
inline constexpr auto max_code_point = char32_t{0x10FFFF};

constexpr bool is_high_surrogate (char32_t c) noexcept {
  return c >= first_high_surrogate && c <= last_high_surrogate;
}
constexpr bool is_low_surrogate (char32_t c) noexcept {
  return c >= first_low_surrogate && c <= last_low_surrogate;
}
constexpr bool is_surrogate (char32_t c) noexcept {
  return is_high_surrogate (c) || is_low_surrogate (c);
}

constexpr bool is_code_point_start (char8 c) noexcept {
  return (static_cast<std::make_unsigned_t<decltype (c)>> (c) & 0xC0U) != 0x80U;
}
constexpr bool is_code_point_start (char16_t c) noexcept {
  return !is_low_surrogate (c);
}
constexpr bool is_code_point_start (char32_t) noexcept {
  return true;
}

/// Returns the number of code points in a sequence.
template <typename InputIterator>
ICUBABY_CXX20REQUIRES (std::input_iterator<InputIterator>)
constexpr auto length (InputIterator first, InputIterator last) {
  return std::count_if (first, last,
                        [] (auto c) { return is_code_point_start (c); });
}

/// Returns an iterator to the beginning of the pos'th code-point in the UTF-8
/// sequence [first, last].
///
/// \param first  The start of the range of elements to examine.
/// \param last  The end of the range of elements to examine.
/// \param pos  The number of code points to move.
/// \returns  An iterator that is 'pos' codepoints after the start of the range or
///           'last' if the end of the range was encountered.
template <typename InputIterator>
ICUBABY_CXX20REQUIRES (std::input_iterator<InputIterator>)
InputIterator index (InputIterator first, InputIterator last, std::size_t pos) {
  auto start_count = std::size_t{0};
  return std::find_if (first, last, [&start_count, pos] (auto c) {
    return is_code_point_start (c) ? (start_count++ == pos) : false;
  });
}

#if ICUBABY_CXX20
template <typename T>
concept is_transcoder = requires (T t) {
                          typename T::input_type;
                          typename T::output_type;
                          // we must also have operator() and finalize() which
                          // both take template arguments.
                          { t.good () } -> std::convertible_to<bool>;
                        };
#endif  // ICUBABY_CXX20

/// An encoder takes a sequence of one of more code-units and converts it to an
/// individual char32_t code-point.
template <typename From, typename To>
class transcoder;

template <typename Transcoder, typename OutputIterator>
ICUBABY_CXX20REQUIRES (
    (is_transcoder<Transcoder> &&
     std::output_iterator<OutputIterator, typename Transcoder::output_type>))
class iterator {
public:
  using iterator_category = std::output_iterator_tag;
  using value_type = void;
  using difference_type = std::ptrdiff_t;
  using pointer = void;
  using reference = void;

  constexpr iterator (Transcoder* const transcoder, OutputIterator it)
      : transcoder_{transcoder}, it_{it} {}
  iterator (iterator const& rhs) = default;

  iterator& operator= (typename Transcoder::input_type const& value) {
    it_ = (*transcoder_) (value, it_);
    return *this;
  }

  iterator& operator= (iterator const& rhs) = default;

  constexpr iterator& operator* () noexcept { return *this; }
  constexpr iterator& operator++ () noexcept { return *this; }
  constexpr iterator operator++ (int) noexcept { return *this; }

  /// Accesses the underlying iterator.
  OutputIterator base () const noexcept { return it_; }
  /// Accesses the underlying transcoder.
  Transcoder* transcoder () noexcept { return transcoder_; }
  Transcoder const* transcoder () const noexcept { return transcoder_; }

private:
  Transcoder* transcoder_;
  [[no_unique_address]] OutputIterator it_;
};

template <typename Transcoder, typename OutputIterator>
iterator (Transcoder& t, OutputIterator it)
    -> iterator<Transcoder, OutputIterator>;

template <>
class transcoder<char32_t, char8> {
public:
  using input_type = char32_t;
  using output_type = char8;

  transcoder () = default;
  explicit transcoder (bool well_formed) : good_{well_formed} {}

  /// \tparam OutputIterator  An output iterator type to which value of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type c, OutputIterator dest) noexcept {
    if (c < 0x80) {
      *(dest++) = static_cast<output_type> (c);
      return dest;
    }
    if (c < 0x800) {
      *(dest++) = static_cast<output_type> ((c >> 6) | 0xC0);
      *(dest++) = static_cast<output_type> ((c & 0x3F) | 0x80);
      return dest;
    }
    if (is_surrogate (c)) {
      good_ = false;
      static_assert (!is_surrogate (replacement_char));
      return (*this) (replacement_char, dest);
    }
    if (c < 0x10000) {
      *(dest++) = static_cast<output_type> ((c >> 12) | 0xE0);
      *(dest++) = static_cast<output_type> (((c >> 6) & 0x3F) | 0x80);
      *(dest++) = static_cast<output_type> ((c & 0x3F) | 0x80);
      return dest;
    }
    if (c <= max_code_point) {
      *(dest++) = static_cast<output_type> ((c >> 18) | 0xF0);
      *(dest++) = static_cast<output_type> (((c >> 12) & 0x3F) | 0x80);
      *(dest++) = static_cast<output_type> (((c >> 6) & 0x3F) | 0x80);
      *(dest++) = static_cast<output_type> ((c & 0x3F) | 0x80);
      return dest;
    }
    good_ = false;
    return (*this) (replacement_char, dest);
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial character.
  ///
  /// \tparam OutputIterator  An output iterator type to which value of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator finalize (OutputIterator dest) const {
    return dest;
  }

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> finalize (
      iterator<transcoder, OutputIterator> dest) {
    auto t = dest.transcoder ();
    assert (t == this);
    return {t, t->finalize (dest.base ())};
  }

  /// \returns True if the input represented well formed UTF-32.
  constexpr bool good () const { return good_; }

private:
  bool good_ = true;
};

template <>
class transcoder<char8, char32_t> {
public:
  using input_type = char8;
  using output_type = char32_t;

  constexpr transcoder () noexcept : transcoder(true) {}
  explicit constexpr transcoder (bool well_formed) noexcept
      : code_point_{0}, good_{well_formed}, pad_{0}, state_{accept} {
    pad_ = 0;
  }

  /// \tparam OutputIterator  An output iterator type to which value of type output_type can be written.
  /// \param code_unit  A UTF-8 code unit,
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_unit, OutputIterator dest) {
    // Prior to C++20, char8 might be signed.
    auto const ucu = static_cast<std::make_unsigned_t<input_type>> (code_unit);
    assert (ucu < utf8d_.size ());
    auto const type = utf8d_[ucu];
    code_point_ =
        (state_ != accept)
            ? (ucu & 0x3FU) | static_cast<uint_least32_t> (code_point_ << 6U)
            : (0xFF >> type) & ucu;
    auto const idx = 256U + state_ + type;
    assert (idx < utf8d_.size ());
    state_ = utf8d_[idx];
    switch (state_) {
    case accept: *(dest++) = code_point_; break;
    case reject:
      good_ = false;
      state_ = accept;
      *(dest++) = replacement_char;
      break;
    default: break;
    }
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial character.
  ///
  /// \tparam OutputIterator  An output iterator type to which value of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator finalize (OutputIterator dest) {
    if (state_ != accept) {
      state_ = reject;
      *(dest++) = replacement_char;
      good_ = false;
    }
    return dest;
  }

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> finalize (
      iterator<transcoder, OutputIterator> dest) {
    auto t = dest.transcoder ();
    assert (t == this);
    return {t, t->finalize (dest.base ())};
  }

  /// \returns True if the input represented well formed UTF-8.
  constexpr bool good () const { return good_; }

private:
  static inline std::array<uint8_t, 364> const utf8d_ = {{
      // The first part of the table maps bytes to character classes that
      // to reduce the size of the transition table and create bitmasks.
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      1,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      9,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      7,
      8,
      8,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      2,
      10,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      4,
      3,
      3,
      11,
      6,
      6,
      6,
      5,
      8,
      8,
      8,
      8,
      8,
      8,
      8,
      8,
      8,
      8,
      8,

      // The second part is a transition table that maps a combination
      // of a state of the automaton and a character class to a state.
      0,
      12,
      24,
      36,
      60,
      96,
      84,
      12,
      12,
      12,
      48,
      72,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      0,
      12,
      12,
      12,
      12,
      12,
      0,
      12,
      0,
      12,
      12,
      12,
      24,
      12,
      12,
      12,
      12,
      12,
      24,
      12,
      24,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      24,
      12,
      12,
      12,
      12,
      12,
      24,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      24,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      36,
      12,
      36,
      12,
      12,
      12,
      36,
      12,
      12,
      12,
      12,
      12,
      36,
      12,
      36,
      12,
      12,
      12,
      36,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
      12,
  }};
  static constexpr auto code_point_bits = 21;
  static_assert (uint_least32_t{1} << code_point_bits > max_code_point);
  uint_least32_t code_point_ : code_point_bits;
  uint_least32_t good_ : 1;
  uint_least32_t pad_ : 2;
  enum : std::uint8_t { accept, reject = 12 };
  uint_least32_t state_ : 8;
};

template <>
class transcoder<char32_t, char16_t> {
public:
  using input_type = char32_t;
  using output_type = char16_t;

  transcoder () = default;
  explicit transcoder (bool well_formed) : good_{well_formed} {}

  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  The output iterator.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_point, OutputIterator dest) {
    if (code_point <= 0xFFFF) {
      *(dest++) = static_cast<output_type> (code_point);
    } else if (is_surrogate (code_point) || code_point > max_code_point) {
      dest = (*this) (replacement_char, dest);
      good_ = false;
    } else {
      *(dest++) = static_cast<output_type> (0xD7C0 + (code_point >> 10));
      *(dest++) =
          static_cast<output_type> (first_low_surrogate + (code_point & 0x3FF));
    }
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial character.
  ///
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  The output iterator.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator finalize (OutputIterator dest) {
    return dest;
  }

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> finalize (
      iterator<transcoder, OutputIterator> dest) {
    auto t = dest.transcoder ();
    assert (t == this);
    return {t, t->finalize (dest.base ())};
  }

  /// \returns True if the input represented valid UTF-32.
  constexpr bool good () const { return good_; }

private:
  bool good_ = true;
};

template <>
class transcoder<char16_t, char32_t> {
public:
  using input_type = char16_t;
  using output_type = char32_t;

  transcoder () = default;
  explicit transcoder (bool well_formed) : good_{well_formed} {}

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type c, OutputIterator dest) {
    if (!has_high_) {
      if (is_high_surrogate (c)) {
        // A high surrogate character indicates that this is the first of a
        // high/low surrogate pair.
        assert (c >= first_high_surrogate);
        auto const h = c - first_high_surrogate;
        assert (h < std::numeric_limits<decltype (high_)>::max ());
        high_ = static_cast<uint_least16_t> (h);
        has_high_ = true;
        return dest;
      }

      *(dest++) = c;
      return dest;
    }

    if (is_low_surrogate (c)) {
      // We saw a high/low surrogate pair.
      has_high_ = false;
      *(dest++) = (static_cast<char32_t> (high_) << 10) +
                  (c - first_low_surrogate) + 0x10000;
      return dest;
    }
    // There was a high surrogate followed by something other than a low
    // surrogate.
    if (is_high_surrogate (c)) {
      c = replacement_char;
    }
    *(dest++) = replacement_char;
    *(dest++) = c;
    good_ = false;
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial character.
  ///
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  The output iterator.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator finalize (OutputIterator dest) {
    if (has_high_) {
      *(dest++) = replacement_char;
      good_ = false;
    }
    return dest;
  }

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> finalize (
      iterator<transcoder, OutputIterator> dest) {
    auto t = dest.transcoder ();
    assert (t == this);
    return {t, t->finalize (dest.base ())};
  }

  bool good () const { return good_; }

private:
  static constexpr int high_bits = 10;
  bool has_high_ = false;
  bool good_ = true;
  uint_least16_t high_ = 0;
};

namespace details {

template <typename From, typename To>
class double_transcoder {
public:
  using input_type = From;
  using output_type = To;

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type c, OutputIterator dest) {
    if (to_inter_ (c, &inter_) != &inter_) {
      dest = to_out_ (inter_, dest);
    }
    return dest;
  }

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator finalize (OutputIterator dest) {
    if (to_inter_.finalize (&inter_) != &inter_) {
      dest = to_out_ (inter_, dest);
    }
    return to_out_.finalize (dest);
  }

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder<From, To>, OutputIterator> finalize (
      iterator<transcoder<From, To>, OutputIterator> dest) {
    auto t = dest.transcoder ();
    assert (t == this);
    return {t, t->finalize (dest.base ())};
  }

  bool good () const { return to_inter_.good () && to_out_.good (); }

private:
  char32_t inter_ = 0;
  transcoder<input_type, char32_t> to_inter_;
  transcoder<char32_t, output_type> to_out_;
};

}  // end namespace details

template <>
class transcoder<char8, char16_t>
    : public details::double_transcoder<char8, char16_t> {};
template <>
class transcoder<char16_t, char8>
    : public details::double_transcoder<char16_t, char8> {};
template <>
class transcoder<char8, char8>
    : public details::double_transcoder<char8, char8> {};
template <>
class transcoder<char16_t, char16_t>
    : public details::double_transcoder<char16_t, char16_t> {};
template <>
class transcoder<char32_t, char32_t> {
public:
  using input_type = char32_t;
  using output_type = char32_t;

  template <typename OutputIt>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIt, output_type>))
  OutputIt operator() (input_type c, OutputIt dest) {
    // From D90 in Chapter 3 of Unicode 15.0.0
    // <https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf>:
    //
    // "Because surrogate code points are not included in the set of Unicode
    // scalar values, UTF-32 code units in the range 0000D80016..0000DFFF16 are
    // ill-formed. Any UTF-32 code unit greater than 0x0010FFFF is ill-formed."
    if (c > max_code_point || is_surrogate (c)) {
      good_ = false;
      c = replacement_char;
    }
    *(dest++) = c;
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial character.
  ///
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  The output iterator.
  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator finalize (OutputIterator dest) const {
    return dest;
  }

  template <typename OutputIterator>
  ICUBABY_CXX20REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> finalize (
      iterator<transcoder, OutputIterator> dest) {
    auto t = dest.transcoder ();
    assert (t == this);
    return {t, t->finalize (dest.base ())};
  }

  constexpr bool good () const { return good_; }

private:
  bool good_ = true;
};

/// A shorter name for the UTF-8 to UTF-8 transcoder. This, of course,
/// represents no change and is included for completeness.
using t8_8 = transcoder<char8, char8>;
/// A shorter name for the UTF-8 to UTF-16 transcoder.
using t8_16 = transcoder<char8, char16_t>;
/// A shorter name for the UTF-8 to UTF-32 transcoder.
using t8_32 = transcoder<char8, char32_t>;

/// A shorter name for the UTF-16 to UTF-8 transcoder.
using t16_8 = transcoder<char16_t, char8>;
/// A shorter name for the UTF-16 to UTF-16 transcoder. This, of course,
/// represents no change and is included for completeness.
using t16_16 = transcoder<char16_t, char16_t>;
/// A shorter name for the UTF-16 to UTF-32 transcoder.
using t16_32 = transcoder<char16_t, char32_t>;

/// A shorter name for the UTF-32 to UTF-8 transcoder.
using t32_8 = transcoder<char32_t, char8>;
/// A shorter name for the UTF-32 to UTF-16 transcoder.
using t32_16 = transcoder<char32_t, char16_t>;
/// A shorter name for the UTF-32 to UTF-32 transcoder. This, of course,
/// represents no change and is included for completeness.
using t32_32 = transcoder<char32_t, char32_t>;

}  // end namespace icubaby

#endif  // ICUBABY_ICUBABY_HPP
