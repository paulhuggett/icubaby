/// \file icubaby.hpp

// UTF-8 conversion is based on the "Flexible and Economical UTF-8 Decoder" by
// Bjoern Hoehrmann <bjoern@hoehrmann.de> See
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
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

namespace icubaby {

inline constexpr auto replacement_char = char32_t{0xFFFD};
inline constexpr auto first_high_surrogate = char32_t{0xD800};
inline constexpr auto first_low_surrogate = char32_t{0xDC00};
inline constexpr auto max_code_point = char32_t{0x10FFFF};

constexpr bool is_high_surrogate (char32_t c) {
  return c >= first_high_surrogate && c <= 0xDBFF;
}
constexpr bool is_low_surrogate (char32_t c) {
  return c >= first_low_surrogate && c <= 0xDFFF;
}
constexpr bool is_surrogate (char32_t c) {
  return is_high_surrogate (c) || is_low_surrogate (c);
}

constexpr bool is_utf8_char_start (char8_t c) noexcept {
  return (c & 0xC0U) != 0x80U;
}

/// Returns the number of code-points in a UTF-8 sequence.
template <typename Iterator>
constexpr auto utf8_length (Iterator first, Iterator last) {
  return std::count_if (first, last,
                        [] (auto c) { return is_utf8_char_start (c); });
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
  requires std::input_iterator<InputIterator>
InputIterator index (InputIterator first, InputIterator last, std::size_t pos) {
  auto start_count = std::size_t{0};
  return std::find_if (first, last, [&start_count, pos] (char8_t c) {
    return is_utf8_char_start (c) ? (start_count++ == pos) : false;
  });
}

template <typename T>
concept is_transcoder = requires (T t) {
                          typename T::input_type;
                          typename T::output_type;
                          { t.finalize () } -> std::convertible_to<bool>;
                          { t.good () } -> std::convertible_to<bool>;
                        };

/// An encoder takes a sequence of one of more code-units and converts it to an
/// individual char32_t code-point.
template <typename From, typename To>
class transcoder;

template <>
class transcoder<char32_t, char8_t> {
public:
  using input_type = char32_t;
  using output_type = char8_t;

  template <typename OutputIt>
    requires std::output_iterator<OutputIt, output_type>
  OutputIt operator() (input_type c, OutputIt dest) {
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

  constexpr bool finalize () const { return good (); }
  constexpr bool good () const { return good_; }

private:
  bool good_ = true;
};

template <>
class transcoder<char8_t, char32_t> {
public:
  using input_type = char8_t;
  using output_type = char32_t;

  template <typename OutputIt>
    requires std::output_iterator<OutputIt, output_type>
  OutputIt operator() (input_type code_unit, OutputIt dest) {
    auto const type = utf8d_[code_unit];
    code_point_ = (state_ != accept) ? (code_unit & 0x3FU) | (code_point_ << 6)
                                     : (0xFF >> type) & code_unit;
    auto const idx = 256U + state_ * 16U + type;
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
  /// \returns True if the input represented valid UTF-8.
  bool finalize () {
    if (state_ != accept) {
      state_ = reject;
      good_ = false;
    }
    return good_;
  }

  /// \returns True if the input represented valid UTF-8.
  constexpr bool good () const { return good_; }

private:
  static std::array<uint8_t const, 400> const utf8d_;
  unsigned code_point_ : 21 = 0;  // U+10FFFF is the maximum code point.
  unsigned good_ : 1 = true;
  enum : std::uint8_t { accept, reject };
  unsigned state_ : 8 = accept;
};

template <>
class transcoder<char32_t, char16_t> {
public:
  using input_type = char32_t;
  using output_type = char16_t;

  template <typename OutputIt>
    requires std::output_iterator<OutputIt, output_type>
  OutputIt operator() (char32_t code_point, OutputIt dest) const {
    if (is_surrogate (code_point)) {
      dest = (*this) (replacement_char, dest);
    } else if (code_point <= 0xFFFF) {
      *(dest++) = static_cast<output_type> (code_point);
    } else if (code_point > max_code_point) {
      dest = (*this) (replacement_char, dest);
    } else {
      *(dest++) = static_cast<output_type> (0xD7C0 + (code_point >> 10));
      *(dest++) =
          static_cast<output_type> (first_low_surrogate + (code_point & 0x3FF));
    }
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial character.
  /// \returns True if the input represented valid UTF-8.
  bool finalize () const { return true; }

  /// \returns True if the input represented valid UTF-8.
  bool good () const { return true; }
};

template <>
class transcoder<char16_t, char32_t> {
public:
  using input_type = char16_t;
  using output_type = char32_t;

  template <typename OutputIt>
    requires std::output_iterator<OutputIt, output_type>
  OutputIt operator() (input_type c, OutputIt dest) {
    if (!has_high_) {
      if (!is_high_surrogate (c)) {
        *(dest++) = c;
      } else {
        assert (c >= first_high_surrogate);
        auto const h = c - first_high_surrogate;
        assert (h < (1U << high_bits));
        high_ = h;
        has_high_ = true;
      }
      return dest;
    }
    if (is_low_surrogate (c)) {
      has_high_ = false;
      *(dest++) = (static_cast<char32_t> (high_) << 10) +
                  (c - first_low_surrogate) + 0x10000;
      return dest;
    }
    good_ = false;
    return dest;
  }

  bool finalize () {
    if (has_high_) {
      good_ = false;
    }
    return good_;
  }

  bool good () const { return good_; }

private:
  static constexpr int high_bits = 10;
  unsigned high_ : high_bits = 0;
  unsigned has_high_ : 1 = false;
  unsigned good_ : 1 = 1;
};

namespace details {

template <typename From, typename To>
class double_transcoder {
public:
  using input_type = From;
  using output_type = To;

  template <typename OutputIt>
    requires std::output_iterator<OutputIt, output_type>
  OutputIt operator() (input_type c, OutputIt dest) {
    if (to_inter_ (c, &inter_) != &inter_) {
      dest = to_out_ (inter_, dest);
    }
    return dest;
  }

  bool finalize () {
    to_inter_.finalize ();
    to_out_.finalize ();
    return good ();
  }
  bool good () const { return to_inter_.good () && to_out_.good (); }

private:
  transcoder<input_type, char32_t> to_inter_;
  transcoder<char32_t, output_type> to_out_;
  char32_t inter_ = 0;
};

}  // end namespace details

template <>
class transcoder<char8_t, char16_t>
    : public details::double_transcoder<char8_t, char16_t> {};
template <>
class transcoder<char16_t, char8_t>
    : public details::double_transcoder<char16_t, char8_t> {};

template <>
class transcoder<char8_t, char8_t>
    : public details::double_transcoder<char8_t, char8_t> {};
template <>
class transcoder<char16_t, char16_t>
    : public details::double_transcoder<char16_t, char16_t> {};
template <>
class transcoder<char32_t, char32_t> {
public:
  using input_type = char32_t;
  using output_type = char32_t;

  template <typename OutputIt>
    requires std::output_iterator<OutputIt, output_type>
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
  constexpr bool finalize () const { return good (); }
  constexpr bool good () const { return good_; }

private:
  bool good_ = true;
};

/// A shorter name for the UTF-8 to UTF-8 transcoder. This, of course,
/// represents no change and is included for completeness.
using t8_8 = transcoder<char8_t, char8_t>;
/// A shorter name for the UTF-8 to UTF-16 transcoder.
using t8_16 = transcoder<char8_t, char16_t>;
/// A shorter name for the UTF-8 to UTF-32 transcoder.
using t8_32 = transcoder<char8_t, char32_t>;

/// A shorter name for the UTF-16 to UTF-8 transcoder.
using t16_8 = transcoder<char16_t, char8_t>;
/// A shorter name for the UTF-16 to UTF-16 transcoder. This, of course,
/// represents no change and is included for completeness.
using t16_16 = transcoder<char16_t, char16_t>;
/// A shorter name for the UTF-16 to UTF-32 transcoder.
using t16_32 = transcoder<char16_t, char32_t>;

/// A shorter name for the UTF-32 to UTF-8 transcoder.
using t32_8 = transcoder<char32_t, char8_t>;
/// A shorter name for the UTF-32 to UTF-16 transcoder.
using t32_16 = transcoder<char32_t, char16_t>;
/// A shorter name for the UTF-32 to UTF-32 transcoder. This, of course,
/// represents no change and is included for completeness.
using t32_32 = transcoder<char32_t, char32_t>;

template <typename Transcoder, typename OutputIterator>
  requires (
      is_transcoder<Transcoder> &&
      std::output_iterator<OutputIterator, typename Transcoder::output_type>)
class iterator {
public:
  using iterator_category = std::output_iterator_tag;
  using value_type = void;
  using difference_type = std::ptrdiff_t;
  using pointer = void;
  using reference = void;

  constexpr iterator (Transcoder& t, OutputIterator it)
      : transcoder_{t}, it_{it} {}

  iterator& operator= (typename Transcoder::input_type const& value) {
    it_ = transcoder_ (value, it_);
    return *this;
  }

  constexpr iterator& operator* () noexcept { return *this; }
  constexpr iterator& operator++ () noexcept { return *this; }
  constexpr iterator operator++ (int) noexcept { return *this; }

private:
  Transcoder transcoder_;
  OutputIterator it_;
};

template <typename Transcoder, typename OutputIterator>
iterator (Transcoder& t, OutputIterator it)
    -> iterator<Transcoder, OutputIterator>;

}  // end namespace icubaby

#endif  // ICUBABY_ICUBABY_HPP
