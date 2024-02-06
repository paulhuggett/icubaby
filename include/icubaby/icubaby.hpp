//*  _         _          _          *
//* (_)__ _  _| |__  __ _| |__ _  _  *
//* | / _| || | '_ \/ _` | '_ \ || | *
//* |_\__|\_,_|_.__/\__,_|_.__/\_, | *
//*                            |__/  *
// Home page:
// https://paulhuggett.github.io/icubaby/
//
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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file   icubaby.hpp
/// \brief  A C++ Baby Library to Immediately Convert Unicode. A header only,
///   dependency free, library for C++ 17 or later. Fast, minimal, and easy to
///   use for converting a sequence in any of UTF-8, UTF-16, or UTF-32.

/// \mainpage
/// A C++ Library to Immediately Convert Unicode. It is a portable, header-only, dependency-free library for C++ 17 or
/// later. Fast, minimal, and easy to use for converting sequences of text between any of the Unicode UTF encodings. It
/// does not allocate dynamic memory and neither throws or catches exceptions.

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
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

/// \brief ICUBABY_CXX20 has value 1 when compiling with C++ 20 or later and 0
///   otherwise.
/// \hideinitializer
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

#ifndef ICUBABY_CXX20
#include <version>
#endif

/// \brief Defined as 1 if the standard library's __cpp_lib_ranges macro is available and 0 otherwise.
#ifdef __cpp_lib_ranges
#define ICUBABY_CPP_LIB_RANGES_DEFINED (1)
#else
#define ICUBABY_CPP_LIB_RANGES_DEFINED (0)
#endif

/// \brief Tests for the availability of library support for C++ 20 ranges.
#define ICUBABY_HAVE_RANGES (ICUBABY_CPP_LIB_RANGES_DEFINED && __cpp_lib_ranges >= 201811L)
#if ICUBABY_HAVE_RANGES
#include <ranges>
#endif

/// \brief Defined as true if compiler and library support for concepts are available.
#ifdef __cpp_concepts
#define ICUBABY_CPP_CONCEPTS_DEFINED (1)
#else
#define ICUBABY_CPP_CONCEPTS_DEFINED (0)
#endif

/// \brief Defined as 1 if the standard library's __cpp_lib_concepts macro is available and 0 otherwise.
#ifdef __cpp_lib_concepts
#define ICUBABY_CPP_LIB_CONCEPTS_DEFINED (1)
#else
#define ICUBABY_CPP_LIB_CONCEPTS_DEFINED (0)
#endif

/// \brief A macro that evaluates true if the compiler and library have support for C++ 20 concepts.
#define ICUBABY_HAVE_CONCEPTS                                                                       \
  (ICUBABY_CPP_CONCEPTS_DEFINED && __cpp_concepts >= 201907L && ICUBABY_CPP_LIB_CONCEPTS_DEFINED && \
   __cpp_lib_concepts >= 202002L)

#if ICUBABY_HAVE_CONCEPTS
#include <concepts>
/// \brief Defined as `requires x` if concepts are supported and as nothing
///   otherwise.
/// \hideinitializer
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ICUBABY_REQUIRES(x) requires x
#else
#define ICUBABY_REQUIRES(x)
#endif  // ICUBABY_HAVE_CONCEPTS

/// \brief Defined as `[[no_unique_address]]` if the attribute is supported and
///   as nothing otherwise.
/// \hideinitializer
#if ICUBABY_CXX20
#define ICUBABY_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
#define ICUBABY_NO_UNIQUE_ADDRESS
#endif

#ifdef ICUBABY_INSIDE_NS
namespace ICUBABY_INSIDE_NS {
#endif

namespace icubaby {

/// \brief Implementation details of the icubaby interface.
///
/// Functions and types defined in this namespace are not part of the icubaby public interface
/// and should not be used in client code. They may change at any time!
namespace details {

/// \brief A compile-time list of types.
///
/// An instance of type_list represents an element in a list. It is somewhat
/// like a cons cell in Lisp: it has two slots, and each slot holds a type.
/// A list is formed when a series of type_list instances are chained together,
/// so that each cell refers to the next one. There is one type_list instance
/// for each list member. The 'first' member holds a member type and the 'rest'
/// field is used to chain together type_list instances. The end of the list is
/// represented by a type_list specialization which takes no arguments and
/// contains no members.
template <typename... Types> struct type_list;

/// \brief A compile-time list of types. This specialization defines the end of the list.
/// \see type_list, type_list<First, Rest>.
template <> struct type_list<> {};

/// \brief A compile-time list of types. This specialization holds a member of the list.
/// \see type_list, type_list<>.
template <typename First, typename Rest> struct type_list<First, Rest> {
  using first = First;  ///< The first member of a list of types.
  using rest = Rest;    ///< The remaining members of the type list.
};

#if ICUBABY_HAVE_CONCEPTS
/// \brief Defines the requirements of a type that is a member of a type list.
///
/// An element in a type list must contain member types names 'first' and
/// 'rest'. The end of the list is given by the type_list<> specialization.
template <typename T>
concept is_type_list = requires {
  typename T::first;
  typename T::rest;
} || std::is_same_v<T, type_list<>>;
#endif

/// \brief Constructs a type_list from a template parameter pack.
/// \see make<>, make<T, Ts...>, make_t
template <typename... Types> struct make;
/// \brief Constructs an empty type_list.
/// \see make, make<T, Ts...>, make_t
template <> struct make<> {
  using type = type_list<>;  ///< An empty type list.
};
/// \brief Constructs a type_list from a template parameter pack.
/// \see make, make<>, make_t
template <typename T, typename... Ts> struct make<T, Ts...> {
  /// A list of types.
  ///
  /// The first element of the list is type \p T: the remaining members
  /// are \p Ts.
  using type = type_list<T, typename make<Ts...>::type>;
};
/// \brief A helper template for make<>.
template <typename... Types> using make_t = typename make<Types...>::type;

/// \brief Yields true if the type list contains a type matching \p Element
///   and false otherwise.
/// \see contains_v
/// \tparam TypeList A list of types (formed by type_list).
/// \tparam Element  The type to be checked.
template <typename TypeList, typename Element>
ICUBABY_REQUIRES (is_type_list<TypeList>)
struct contains : std::bool_constant<std::is_same_v<Element, typename TypeList::first> ||
                                     contains<typename TypeList::rest, Element>::value> {};
/// \brief Yields false: the empty type list does not contain a type matching \p Element.
/// \see contains_v
/// \tparam Element  The type to be checked.
template <typename Element> struct contains<type_list<>, Element> : std::bool_constant<false> {};

/// A helper variable template for contains<>.
template <typename TypeList, typename Element> inline constexpr bool contains_v = contains<TypeList, Element>::value;

}  // end namespace details

/// The type of a UTF-8 code unit.
#if defined(__cpp_char8_t) && defined(__cpp_lib_char8_t)
using char8 = char8_t;
#else
using char8 = char;
#endif

/// A UTF-8 string.
using u8string = std::basic_string<char8>;
/// A UTF-8 string_view.
using u8string_view = std::basic_string_view<char8>;

/// A constant for U+FFFD REPLACEMENT CHARACTER
inline constexpr auto replacement_char = char32_t{0xFFFD};
/// A constant for U+FEFF ZERO WIDTH NO-BREAK SPACE (BYTE ORDER MARK)
inline constexpr auto zero_width_no_break_space = char32_t{0xFEFF};
/// A constant for U+FEFF ZERO WIDTH NO-BREAK SPACE (BYTE ORDER MARK)
inline constexpr auto byte_order_mark = zero_width_no_break_space;

/// \brief The number of bits required to represent a code point.
///
/// Starting with Unicode 2.0, characters are encoded in the range
/// U+0000..U+10FFFF, which amounts to a 21-bit code space.
inline constexpr auto code_point_bits = 21U;

/// The code point of the first UTF-16 high surrogate.
inline constexpr auto first_high_surrogate = char32_t{0xD800};
/// The code point of the last UTF-16 high surrogate.
inline constexpr auto last_high_surrogate = char32_t{0xDBFF};
/// The code point of the first UTF-16 low surrogate.
inline constexpr auto first_low_surrogate = char32_t{0xDC00};
/// The code point of the last UTF-16 low surrogate.
inline constexpr auto last_low_surrogate = char32_t{0xDFFF};

/// The number of the last code point.
inline constexpr auto max_code_point = char32_t{0x10FFFF};
static_assert (uint_least32_t{1} << code_point_bits > max_code_point);

/// \brief The number of code-units in the longest legal representation of a code-point.
template <typename Encoding> struct longest_sequence {};
/// \brief The number of code-units in the longest legal UTF-8 representation of a code-point.
template <> struct longest_sequence<char8> : std::integral_constant<std::size_t, 4> {};
/// \brief The number of code-units in the longest legal UTF-16 representation of a code-point.
template <> struct longest_sequence<char16_t> : std::integral_constant<std::size_t, 2> {};
/// \brief The number of code-units in the longest legal UTF-32 representation of a code-point.
template <> struct longest_sequence<char32_t> : std::integral_constant<std::size_t, 1> {};

/// A helper variable template to simplify use of longest_sequence<>.
template <typename Encoding> inline constexpr std::size_t longest_sequence_v = longest_sequence<Encoding>::value;

/// A list of the character types used for UTF-8 UTF-16, and UTF-32 encoded
/// text.
using character_types = details::make_t<char8, char16_t, char32_t>;

/// \brief Checks whether the argument is one of the unicode character types
///
/// Provides the constant `value` which is equal to true, if T is one of the unicode character types as defined by
/// icubaby::character_types. Otherwise, value is equal to false.
/// \tparam T  The type to be checked.
template <typename T> struct is_unicode_char_type : std::bool_constant<details::contains_v<character_types, T>> {};
/// \brief A helper variable template to simplify use of icubaby::is_unicode_char_type.
template <typename T> inline constexpr bool is_unicode_char_type_v = is_unicode_char_type<T>::value;

#if ICUBABY_HAVE_CONCEPTS
/// \brief Checks whether the argument is one of the unicode character types
///
/// The unicode_char_type concept defines the requires of a type that matches one of the types that denote a
/// Unicode encoding.
template <typename T>
concept unicode_char_type = is_unicode_char_type_v<T>;
#endif

/// \brief Returns true if the code point \p code_point represents a UTF-16 high surrogate.
///
/// \param code_point  The code point to be tested.
/// \returns true if the code point \p code_point represents a UTF-16 high surrogate.
constexpr bool is_high_surrogate (char32_t code_point) noexcept {
  return code_point >= first_high_surrogate && code_point <= last_high_surrogate;
}
/// \brief Returns true if the code point \p code_point represents a UTF-16 low surrogate.
///
/// \param code_point  The code point to be tested.
/// \returns true if the code point \p code_point represents a UTF-16 low surrogate.
constexpr bool is_low_surrogate (char32_t code_point) noexcept {
  return code_point >= first_low_surrogate && code_point <= last_low_surrogate;
}
/// \brief Returns true if the code point \p code_point represents a UTF-16 low or high surrogate.
///
/// \param code_point  The code point to be tested.
/// \returns true if the code point \p c represents a UTF-16 high or low surrogate.
constexpr bool is_surrogate (char32_t code_point) noexcept {
  return is_high_surrogate (code_point) || is_low_surrogate (code_point);
}

/// \brief Returns true if \p code_unit represents the start of a multi-byte UTF-8 sequence.
///
/// \param code_unit  The UTF-8 code unit to be tested.
/// \returns true if \p code_unit represents the start of a multi-byte UTF-8 sequence.
constexpr bool is_code_point_start (char8 code_unit) noexcept {
  static_assert (sizeof (code_unit) == sizeof (std::byte));
  return (static_cast<std::byte> (code_unit) & std::byte{0xC0}) != std::byte{0x80};
}
/// \brief Returns true if \p code_unit represents the start of a UTF-16 high/low surrogate pair.
///
/// \param code_unit  The UTF-16 code unit to be tested.
/// \returns  true if \p code_unit represents the start of a UTF-16 high/low surrogate pair.
constexpr bool is_code_point_start (char16_t code_unit) noexcept {
  return !is_low_surrogate (code_unit);
}
/// \brief Returns true if \p code_unit represents a valid UTF-32 code point.
///
/// \param code_unit  The UTF-32 code unit to be tested.
/// \returns  true if \p code_unit represents a valid UTF-32 code point.
constexpr bool is_code_point_start (char32_t code_unit) noexcept {
  return !is_surrogate (code_unit) && code_unit <= max_code_point;
}

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

/// \brief Returns the number of code points in a sequence.
///
/// \note The input sequence must be well formed for the result to be accurate.
/// \tparam Range  An input range.
/// \tparam Proj  Type of the projection applied to elements.
/// \param range The range of the elements to examine.
/// \param proj  Projection to apply to the elements.
/// \returns  The number of code points.
template <std::ranges::input_range Range, typename Proj = std::identity>
  requires unicode_char_type<std::ranges::range_value_t<Range>>
constexpr std::ranges::range_difference_t<Range> length (Range&& range, Proj proj = {}) {
  return std::ranges::count_if (
      std::forward<Range> (range),
      [] (unicode_char_type auto const code_unit) { return is_code_point_start (code_unit); }, proj);
}

/// \brief Returns the number of code points in a sequence.
///
/// \note The input sequence must be well formed for the result to be accurate.
/// \param first  The start of the range of code units to examine.
/// \param last  The end of the range of code units to examine.
/// \param proj  Projection to apply to the elements.
/// \returns  The number of code points.
template <std::input_iterator I, std::sentinel_for<I> S, typename Proj = std::identity>
  requires unicode_char_type<typename std::iterator_traits<I>::value_type>
constexpr std::iter_difference_t<I> length (I first, S last, Proj proj = {}) {
  return length (std::ranges::subrange{first, last}, proj);
}

#else

/// \brief Returns the number of code points in a sequence.
///
/// \note The input sequence must be well formed for the result to be accurate.
/// \param first  The start of the range of code units to examine.
/// \param last  The end of the range of code units to examine.
/// \returns  The number of code points.
template <typename InputIterator,
          typename = std::enable_if_t<is_unicode_char_type_v<typename std::iterator_traits<InputIterator>::value_type>>>
constexpr typename std::iterator_traits<InputIterator>::difference_type length (InputIterator first,
                                                                                InputIterator last) {
  return std::count_if (first, last, [] (auto c) { return is_code_point_start (c); });
}

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

/// Returns an iterator to the beginning of the pos'th code point in the range of code-units given by \p range.
///
/// \tparam Range  An input range.
/// \tparam Proj   The type of the projection applied to elements.
/// \param range  The range of code units to examine.
/// \param pos  The number of code points to move.
/// \param proj  Projection to apply to the elements.
/// \returns  Iterator to the start of the selected code point or iterator equal to last if no such element is found.
template <std::ranges::input_range Range, typename Proj = std::identity>
constexpr std::ranges::borrowed_iterator_t<Range> index (Range&& range, std::size_t pos, Proj proj = {}) {
  auto count = std::size_t{0};
  return std::ranges::find_if (
      std::forward<Range> (range),
      [&count, pos] (unicode_char_type auto const code_unit) {
        return is_code_point_start (code_unit) ? (count++ == pos) : false;
      },
      proj);
}

/// Returns an iterator to the beginning of the pos'th code point in the code
/// unit sequence [first, last).
///
/// \param first  The start of the range of code units to examine.
/// \param last  The end of the range of code units to examine.
/// \param pos  The number of code points to move.
/// \param proj  Projection to apply to the elements.
/// \returns  An iterator that is 'pos' code points after the start of the range or
///           'last' if the end of the range was encountered.
template <std::input_iterator I, std::sentinel_for<I> S, typename Proj = std::identity>
constexpr I index (I first, S last, std::size_t pos, Proj proj = {}) {
  return index (std::ranges::subrange{first, last}, pos, proj);
}

#else

/// Returns an iterator to the beginning of the pos'th code point in the code
/// unit sequence [first, last).
///
/// \param first  The start of the range of code units to examine.
/// \param last  The end of the range of code units to examine.
/// \param pos  The number of code points to move.
/// \returns  An iterator that is 'pos' code points after the start of the range or
///           'last' if the end of the range was encountered.
template <typename InputIterator,
          typename = std::enable_if_t<is_unicode_char_type_v<typename std::iterator_traits<InputIterator>::value_type>>>
constexpr InputIterator index (InputIterator first, InputIterator last, std::size_t pos) {
  auto count = std::size_t{0};
  return std::find_if (first, last, [&count, pos] (auto c) {
    static_assert (is_unicode_char_type_v<std::decay_t<decltype (c)>>);
    return is_code_point_start (c) ? (count++ == pos) : false;
  });
}

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#if ICUBABY_HAVE_CONCEPTS
/// \brief Defines the requirements of a type that provides the transcoder interface.
template <typename T>
concept is_transcoder = requires (T tcdr) {
  typename T::input_type;
  typename T::output_type;
  // we must also have operator() and end_cp() which
  // both take template arguments.
  { tcdr.well_formed () } -> std::convertible_to<bool>;
  { tcdr.partial () } -> std::convertible_to<bool>;
};

#endif  // ICUBABY_HAVE_CONCEPTS

/// A transcoder takes a sequence of one of more code-units in one Unicode
/// encoding (one of UTF-8, UTF-16, or UTF-32) and and converts it to another.
///
/// Each of the specializations of this template (there is one for each input/output combination) supplies
/// the same interface. We have:
///
/// - operator(). This member function accepts accepts a code unit in the source encoding and
///   writes code units in the output encoding to an output iterator as they are produced.
/// - end_cp(). Call once the entire input sequence has been fed to operator(). This
///   function ensures that the sequence did not end with a partial code point and flushes any remaining output.
/// - well_formed(). Indicates whether the input was well formed.
/// - partial(). Indicates whether a "partial" code point has been passed to operator(). If true, one or more code
///   units are required to build the complete code point.
#if ICUBABY_HAVE_CONCEPTS
template <unicode_char_type From, unicode_char_type To> class transcoder;
#else
template <typename From, typename To> class transcoder;
#endif  // ICUBABY_HAVE_CONCEPTS

/// \brief An output iterator which passes code units being output through a
///   transcoder.
///
/// This iterator simplifies the job of converting unicode representation and
/// storing the results of that conversion. Each time that a code point is
/// recovered from the sequence written to this class, the equivalent
/// sequence is written to the iterator with which the object was constructed.
///
/// For example, a function to convert a UTF-16 string to UTF-8 becomes very
/// simple:
/// ~~~
/// std::u8string utf16_to_8_demo (std::u16string u16) {
///   std::u8string u8;
///   icubaby::t16_8 t;
///   icubaby::iterator out{&t, std::back_inserter(u8)};
///   t.end_cp (std::copy (u16.begin(), u16.end(), out));
///   return u8;
/// }
/// ~~~
///
/// \tparam Transcoder  A transcoder type.
/// \tparam OutputIterator  An output iterator type.
template <typename Transcoder, typename OutputIterator>
ICUBABY_REQUIRES ((is_transcoder<Transcoder> && std::output_iterator<OutputIterator, typename Transcoder::output_type>))
class iterator {
public:
  /// Defines this class as fulfilling the requirements of an output iterator.
  using iterator_category = std::output_iterator_tag;
  /// The class is an output iterator and as such does not yield values.
  using value_type = void;
  /// A type that can be used to identify distance between iterators.
  using difference_type = std::ptrdiff_t;
  /// Defines a pointer to the type iterated over (none in the case of this iterator).
  using pointer = void;
  /// Defines a reference to the type iterated over (none in the case of this iterator).
  using reference = void;

  /// Initializes the underlying transcoder and the output iterator to which elements will be written.
  ///
  /// \param transcoder  The underlying transcoder. This class does not take ownership of the pointer.
  /// \param out  An output iterator to which code units produced by the \p transcoder will be written.
  iterator (Transcoder* transcoder, OutputIterator out) : transcoder_{transcoder}, out_{out} {}
  iterator (iterator const& rhs) = default;
  iterator (iterator&& rhs) noexcept = default;

  ~iterator () noexcept = default;

  /// Passes a code unit to the associated transcoder.
  /// \param value The code unit to be passed to the transcoder.
  /// \returns \*this
  iterator& operator= (typename Transcoder::input_type const& value) {
    out_ = (*transcoder_) (value, out_);
    return *this;
  }

  iterator& operator= (iterator const& rhs) = default;
  iterator& operator= (iterator&& rhs) noexcept = default;

  /// \brief no-op
  /// \returns \*this
  constexpr iterator& operator* () noexcept { return *this; }
  /// \brief no-op
  /// \returns \*this
  constexpr iterator& operator++ () noexcept { return *this; }
  /// \brief no-op
  /// \returns \*this
  constexpr iterator operator++ (int) noexcept { return *this; }

  /// Accesses the underlying iterator.
  [[nodiscard]] constexpr OutputIterator base () const noexcept { return out_; }
  /// Accesses the underlying transcoder.
  [[nodiscard]] constexpr Transcoder* transcoder () noexcept { return transcoder_; }
  /// Accesses the underlying transcoder.
  [[nodiscard]] constexpr Transcoder const* transcoder () const noexcept { return transcoder_; }

private:
  Transcoder* transcoder_;
  /// An output iterator to which code units produced by the transcoder will be written.
  ICUBABY_NO_UNIQUE_ADDRESS OutputIterator out_;
};

/// A class template argument deduction guide for icubaby::iterator.
template <typename Transcoder, typename OutputIterator>
iterator (Transcoder& transcoder, OutputIterator out) -> iterator<Transcoder, OutputIterator>;

/// Takes a sequence of UTF-32 code units and converts them to UTF-8.
template <> class transcoder<char32_t, char8> {
public:
  /// The type of the code units consumed by this transcoder.
  using input_type = char32_t;
  /// The type of the code units produced by this transcoder.
  using output_type = char8;

  constexpr transcoder () noexcept = default;
  /// Initializes a transcoder instance with an initial value for its "well formed" state. This can be useful if
  /// converting a stream of data which may be using different encodings.
  ///
  /// \param well_formed The initial value for the transcoder's "well formed" state.
  explicit constexpr transcoder (bool well_formed) noexcept : well_formed_{well_formed} {}

  /// Accepts a code unit in the UTF-32 source encoding. As UTF-8 output code units are generated, they are written to
  /// the output iterator \p dest.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param code_unit  A code unit in the source encoding.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_unit, OutputIterator dest) noexcept {
    if (code_unit < 0x80) {
      *(dest++) = static_cast<output_type> (code_unit);
      return dest;
    }
    if (code_unit < 0x800) {
      return transcoder::write2 (code_unit, dest);
    }
    if (is_surrogate (code_unit)) {
      return transcoder::not_well_formed (dest);
    }
    if (code_unit < 0x10000) {
      return transcoder::write3 (code_unit, dest);
    }
    if (code_unit <= max_code_point) {
      return transcoder::write4 (code_unit, dest);
    }
    return transcoder::not_well_formed (dest);
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator end_cp (OutputIterator dest) const {
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> end_cp (iterator<transcoder, OutputIterator> dest) {
    auto tcdr = dest.transcoder ();
    assert (tcdr == this);
    return {tcdr, tcdr->end_cp (dest.base ())};
  }

  /// \returns True if the input represented well formed UTF-32.
  [[nodiscard]] constexpr bool well_formed () const noexcept { return well_formed_; }
  /// \returns True if a partial code-point has been passed to operator() and
  /// false otherwise.
  [[nodiscard]] static constexpr bool partial () noexcept { return false; }

private:
  bool well_formed_ = true;

  template <typename OutputIterator> static OutputIterator write2 (input_type code_unit, OutputIterator dest) {
    *(dest++) = static_cast<output_type> ((code_unit >> 6U) | 0xc0U);
    *(dest++) = static_cast<output_type> ((code_unit & 0x3fU) | 0x80U);
    return dest;
  }
  template <typename OutputIterator> static OutputIterator write3 (input_type code_unit, OutputIterator dest) {
    *(dest++) = static_cast<output_type> ((code_unit >> 12U) | 0xe0U);
    *(dest++) = static_cast<output_type> (((code_unit >> 6U) & 0x3fU) | 0x80U);
    *(dest++) = static_cast<output_type> ((code_unit & 0x3fU) | 0x80U);
    return dest;
  }
  template <typename OutputIterator> static OutputIterator write4 (input_type code_unit, OutputIterator dest) {
    *(dest++) = static_cast<output_type> ((code_unit >> 18U) | 0xf0U);
    *(dest++) = static_cast<output_type> (((code_unit >> 12U) & 0x3fU) | 0x80U);
    *(dest++) = static_cast<output_type> (((code_unit >> 6U) & 0x3fU) | 0x80U);
    *(dest++) = static_cast<output_type> ((code_unit & 0x3fU) | 0x80U);
    return dest;
  }

  template <typename OutputIterator> OutputIterator not_well_formed (OutputIterator dest) {
    well_formed_ = false;
    static_assert (!is_surrogate (replacement_char));
    return (*this) (replacement_char, dest);
  }
};

/// Takes a sequence of UTF-8 code units and converts them to UTF-32.
template <> class transcoder<char8, char32_t> {
public:
  /// The type of the code units consumed by this transcoder.
  using input_type = char8;
  /// The type of the code units produced by this transcoder.
  using output_type = char32_t;

  constexpr transcoder () noexcept : transcoder (true) {}
  /// Initializes a transcoder instance with an initial value for its "well formed" state. This can be useful if
  /// converting a stream of data which may be using different encodings.
  ///
  /// \param well_formed The initial value for the transcoder's "well formed" state.
  explicit constexpr transcoder (bool well_formed) noexcept
      : code_point_{0}, well_formed_{static_cast<uint_least32_t> (well_formed)}, pad_{0}, state_{accept} {
    // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
    pad_ = 0;  // Suppress warning about pad_ being unused.
  }

  /// Accepts a code unit in the UTF-8 source encoding. As UTF-32 output code units are generated, they are written to
  /// the output iterator \p dest.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of
  ///   output_type can be written.
  /// \param code_unit  A UTF-8 code unit,
  /// \param dest  Iterator to which the output should be written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_unit, OutputIterator dest) {
    // Prior to C++20, char8 might be signed.
    static_assert (sizeof (input_type) == sizeof (std::uint8_t));
    auto const ucu = static_cast<std::uint8_t> (code_unit);
    static_assert (std::is_unsigned_v<decltype (ucu)> && std::numeric_limits<decltype (ucu)>::max () <= utf8d_.size ());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    auto const type = utf8d_[ucu];
    code_point_ = (state_ != accept)
                      ? static_cast<std::uint_least32_t> (static_cast<std::byte> (code_unit) & std::byte{0x3FU}) |
                            static_cast<uint_least32_t> (code_point_ << 6U)
                      : (0xFFU >> type) & ucu;
    auto const idx = 256U + state_ + type;
    assert (idx < utf8d_.size ());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    state_ = utf8d_[idx];
    switch (state_) {
    case accept: *(dest++) = code_point_; break;
    case reject:
      well_formed_ = false;
      state_ = accept;
      *(dest++) = replacement_char;
      break;
    default: break;
    }
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator end_cp (OutputIterator dest) {
    if (state_ != accept) {
      state_ = reject;
      *(dest++) = replacement_char;
      well_formed_ = false;
    }
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> end_cp (iterator<transcoder, OutputIterator> dest) {
    auto tcdr = dest.transcoder ();
    assert (tcdr == this);
    return {tcdr, tcdr->end_cp (dest.base ())};
  }

  /// \returns True if the input represented well formed UTF-8.
  [[nodiscard]] constexpr bool well_formed () const noexcept { return well_formed_; }
  /// \returns True if a partial code-point has been passed to operator() and
  /// false otherwise.
  [[nodiscard]] constexpr bool partial () const noexcept { return state_ != accept; }

private:
  static inline std::array<uint8_t, 364> const utf8d_ = {{
    // clang-format off
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
     0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12,
    // clang-format on
  }};
  uint_least32_t code_point_ : code_point_bits;
  uint_least32_t well_formed_ : 1;
  uint_least32_t pad_ : 2;
  enum : std::uint8_t { accept, reject = 12 };
  uint_least32_t state_ : 8;
};

/// Takes a sequence of UTF-32 code units and converts them to UTF-16.
template <> class transcoder<char32_t, char16_t> {
public:
  /// The type of the code units consumed by this transcoder.
  using input_type = char32_t;
  /// The type of the code units produced by this transcoder.
  using output_type = char16_t;

  constexpr transcoder () noexcept = default;
  /// Initializes a transcoder instance with an initial value for its "well formed" state. This can be useful if
  /// converting a stream of data which may be using different encodings.
  ///
  /// \param well_formed The initial value for the transcoder's "well formed" state.
  explicit constexpr transcoder (bool well_formed) noexcept : well_formed_{well_formed} {}

  /// Accepts a code unit in the UTF-32 source encoding. As UTF-16 output code units are generated, they are written to
  /// the output iterator \p dest.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of
  ///   output_type can be written.
  /// \param code_unit  A UTF-32 code unit,
  /// \param dest  Iterator to which the output should be written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_unit, OutputIterator dest) {
    if (is_surrogate (code_unit) || code_unit > max_code_point) {
      dest = (*this) (replacement_char, dest);
      well_formed_ = false;
    } else if (code_unit <= 0xFFFF) {
      *(dest++) = static_cast<output_type> (code_unit);
    } else {
      *(dest++) = static_cast<output_type> (0xD7C0U + (code_unit >> 10U));
      *(dest++) = static_cast<output_type> (first_low_surrogate + (code_unit & 0x3FFU));
    }
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  The output iterator.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator end_cp (OutputIterator dest) {
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> end_cp (iterator<transcoder, OutputIterator> dest) {
    auto tcdr = dest.transcoder ();
    assert (tcdr == this);
    return {tcdr, tcdr->end_cp (dest.base ())};
  }

  /// \returns True if the input represented valid UTF-32.
  [[nodiscard]] constexpr bool well_formed () const noexcept { return well_formed_; }
  /// \returns True if a partial code-point has been passed to operator() and
  /// false otherwise.
  [[nodiscard]] static constexpr bool partial () noexcept { return false; }

private:
  bool well_formed_ = true;
};

/// Takes a sequence of UTF-16 code units and converts them to UTF-32.
template <> class transcoder<char16_t, char32_t> {
public:
  /// The type of the code units consumed by this transcoder.
  using input_type = char16_t;
  /// The type of the code units produced by this transcoder.
  using output_type = char32_t;

  constexpr transcoder () noexcept : transcoder (true) {}
  /// Initializes a transcoder instance with an initial value for its "well formed" state. This can be useful if
  /// converting a stream of data which may be using different encodings.
  ///
  /// \param well_formed The initial value for the transcoder's "well formed" state.
  explicit constexpr transcoder (bool well_formed) noexcept
      : high_{0},
        has_high_{static_cast<uint_least16_t> (false)},
        well_formed_{static_cast<uint_least16_t> (well_formed)} {}

  /// Accepts a code unit in the UTF-16 source encoding. As UTF-32 output code units are generated, they are written to
  /// the output iterator \p dest.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param code_unit  A code unit in the source encoding.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_unit, OutputIterator dest) {
    if (!has_high_) {
      if (is_high_surrogate (code_unit)) {
        // A high surrogate code unit indicates that this is the first of a
        // high/low surrogate pair.
        assert (code_unit >= first_high_surrogate);
        auto const high_cu = code_unit - first_high_surrogate;
        assert (high_cu < std::numeric_limits<decltype (high_)>::max ());
        assert (high_cu < (1U << high_bits));
        high_ = static_cast<uint_least16_t> (high_cu);
        has_high_ = true;
        return dest;
      }

      // A low surrogate without a preceeding high surrogate.
      if (is_low_surrogate (code_unit)) {
        well_formed_ = false;
        code_unit = replacement_char;
      }
      *(dest++) = code_unit;
      return dest;
    }

    // A high surrogate followed by a low surrogate.
    if (is_low_surrogate (code_unit)) {
      *(dest++) = (static_cast<char32_t> (high_) << high_bits) + (code_unit - first_low_surrogate) + 0x10000;
      high_ = 0;
      has_high_ = false;
      return dest;
    }
    // There was a high surrogate followed by something other than a low
    // surrogate. A high surrogate followed by a second high surrogate yields
    // a single REPLACEMENT CHARACTER. A high followed by something other than
    // a low surrogate gives REPLACEMENT CHARACTER followed by the second input
    // code point.
    *(dest++) = replacement_char;
    if (!is_high_surrogate (code_unit)) {
      *(dest++) = code_unit;
      high_ = 0;
      has_high_ = false;
    }
    well_formed_ = false;
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  The output iterator.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator end_cp (OutputIterator dest) {
    if (has_high_) {
      *(dest++) = replacement_char;
      high_ = 0;
      has_high_ = false;
      well_formed_ = false;
    }
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> end_cp (iterator<transcoder, OutputIterator> dest) {
    auto tcdr = dest.transcoder ();
    assert (tcdr == this);
    return {tcdr, tcdr->end_cp (dest.base ())};
  }

  /// \returns True if the input represented well formed UTF-16.
  [[nodiscard]] constexpr bool well_formed () const noexcept { return well_formed_; }
  /// \returns True if a partial code-point has been passed to operator() and
  /// false otherwise.
  [[nodiscard]] constexpr bool partial () const noexcept { return has_high_; }

private:
  static constexpr auto high_bits = 10U;
  /// The previous high surrogate that was passed to operator(). Valid if
  /// has_high_ is true.
  uint_least16_t high_ : high_bits;
  /// true if the previous code unit passed to operator() was a high surrogate,
  /// false otherwise.
  uint_least16_t has_high_ : 1;
  /// true if the code units passed to operator() represent well formed UTF-16
  /// input, false otherwise.
  uint_least16_t well_formed_ : 1;
};

namespace details {

template <typename From, typename To> class double_transcoder {
public:
  /// The type of the code units consumed by this transcoder.
  using input_type = From;
  /// The type of the code units produced by this transcoder.
  using output_type = To;

  /// Accepts a code unit in the source encoding (as given by double_transcoder::input_type). These are first converted
  /// to UTF-32 and then to the output encoding (double_transcover::output_type). As output code units are generated,
  /// they are written to the output iterator \p dest.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param code_unit  A code unit in the source encoding.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_unit, OutputIterator dest) {
    // The (intermediate) output from the conversion to UTF-32. It's possible
    // for the transcoder to produce more than a single output code unit if the
    // input is malformed.
    std::array<char32_t, 2> intermediate{};
    // NOLINTNEXTLINE(llvm-qualified-auto,readability-qualified-auto)
    auto const begin = std::begin (intermediate);
    return copy (begin, to_inter_ (code_unit, begin), dest);
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator end_cp (OutputIterator dest) {
    std::array<char32_t, 2> intermediate{};
    // NOLINTNEXTLINE(llvm-qualified-auto,readability-qualified-auto)
    auto const begin = std::begin (intermediate);
    return copy (begin, to_inter_.end_cp (begin), dest);
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder<From, To>, OutputIterator> end_cp (
      iterator<transcoder<From, To>, OutputIterator> dest) {
    auto const tcdr = dest.transcoder ();
    assert (tcdr == this);
    return {tcdr, tcdr->end_cp (dest.base ())};
  }

  /// \returns True if the input passed to operator() was valid.
  [[nodiscard]] constexpr bool well_formed () const noexcept {
    return to_inter_.well_formed () && to_out_.well_formed ();
  }

  /// \returns True if a partial code-point has been passed to operator() and
  /// false otherwise.
  [[nodiscard]] constexpr bool partial () const noexcept { return to_inter_.partial (); }

private:
  transcoder<input_type, char32_t> to_inter_;
  transcoder<char32_t, output_type> to_out_;

  template <typename InputIterator, typename OutputIterator>
  OutputIterator copy (InputIterator first, InputIterator last, OutputIterator dest) {
    (void)std::for_each (first, last, [this, &dest] (char32_t const code_unit) { dest = to_out_ (code_unit, dest); });
    return dest;
  }
};

}  // end namespace details

/// Takes a sequence of UTF-8 code units and converts them to UTF-16.
template <> class transcoder<char8, char16_t> : public details::double_transcoder<char8, char16_t> {};
/// Takes a sequence of UTF-16 code units and converts them to UTF-8.
template <> class transcoder<char16_t, char8> : public details::double_transcoder<char16_t, char8> {};
/// Takes a sequence of UTF-8 code units and converts them to UTF-8.
template <> class transcoder<char8, char8> : public details::double_transcoder<char8, char8> {};
/// Takes a sequence of UTF-16 code units and converts them to UTF-16.
template <> class transcoder<char16_t, char16_t> : public details::double_transcoder<char16_t, char16_t> {};
/// Takes a sequence of UTF-32 code units and converts them to UTF-32.
template <> class transcoder<char32_t, char32_t> {
public:
  /// The type of the code units consumed by this transcoder.
  using input_type = char32_t;
  /// The type of the code units produced by this transcoder.
  using output_type = char32_t;

  /// Accepts a code unit in the UTF-32 source encoding. As UTF-32 output code units are generated, they are written to
  /// the output iterator \p dest.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of
  ///   output_type can be written.
  /// \param code_unit  A UTF-32 code unit,
  /// \param dest  Iterator to which the output should be written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type code_unit, OutputIterator dest) {
    // From D90 in Chapter 3 of Unicode 15.0.0
    // <https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf>:
    //
    // "Because surrogate code points are not included in the set of Unicode
    // scalar values, UTF-32 code units in the range 0000D80016..0000DFFF16 are
    // ill-formed. Any UTF-32 code unit greater than 0x0010FFFF is ill-formed."
    if (code_unit > max_code_point || is_surrogate (code_unit)) {
      well_formed_ = false;
      code_unit = replacement_char;
    }
    *(dest++) = code_unit;
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  The output iterator.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr OutputIterator end_cp (OutputIterator dest) const {
    return dest;
  }

  /// Call once the entire input sequence has been fed to operator(). This
  /// function ensures that the sequence did not end with a partial code point.
  ///
  /// \tparam OutputIterator  An output iterator type to which values of type output_type can be written.
  /// \param dest  An output iterator to which the output sequence is written.
  /// \returns  Iterator one past the last element assigned.
  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  constexpr iterator<transcoder, OutputIterator> end_cp (iterator<transcoder, OutputIterator> dest) {
    auto tcdr = dest.transcoder ();
    assert (tcdr == this);
    return {tcdr, tcdr->end_cp (dest.base ())};
  }

  /// \returns True if the input represented well formed UTF-32.
  [[nodiscard]] constexpr bool well_formed () const noexcept { return well_formed_; }
  /// \returns True if a partial code-point has been passed to operator() and
  /// false otherwise.
  [[nodiscard]] static constexpr bool partial () noexcept { return false; }

private:
  bool well_formed_ = true;
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

#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

/// \brief icubaby C++ 20 ranges support types.
namespace ranges {

/// \brief A range adaptor for lazily converting between Unicode encodings.
///
/// A range adaptor that represents view of an underlying sequence consisting of Unicode code points in the encoding
/// given by FromEncoding and produces the equivalent code points in the encoding given by ToEncoding.
///
/// \tparam FromEncoding  The encoding using by the underlying sequence.
/// \tparam ToEncoding  The encoding that will be produced by this range adaptor.
/// \tparam View  The type of the underlying view.
template <unicode_char_type FromEncoding, unicode_char_type ToEncoding, std::ranges::input_range View>
  requires std::ranges::view<View>
class transcode_view : public std::ranges::view_interface<transcode_view<FromEncoding, ToEncoding, View>> {
public:
  class iterator;
  class sentinel;

  /// \brief Default initializes the base view of a new transcode_view instance.
  transcode_view () requires std::default_initializable<View> = default;
  /// \brief Initializes the base view of a new transcode_view instance.
  constexpr explicit transcode_view (View base) : base_ (std::move (base)) {}

  /// \returns The base view.
  constexpr View base () const& requires std::copy_constructible<View> { return base_; }
  /// \returns Moves the base view out of this object.
  constexpr View base () && { return std::move (base_); }

  /// \brief Obtains the beginning iterator of a transcode_view.
  constexpr auto begin () const { return iterator{*this, std::ranges::begin (base_)}; }
  /// \brief Obtains the sentinel denoting the end of transcode_view.
  constexpr auto end () const {
    if constexpr (std::ranges::common_range<View>) {
      return iterator{*this, std::ranges::end (base_)};
    } else {
      return sentinel{*this};
    }
  }

  /// \returns True if the input processed was well formed.
  [[nodiscard]] constexpr bool well_formed () const noexcept { return well_formed_; }

private:
  [[no_unique_address]] View base_ = View ();
  mutable bool well_formed_ = true;
};

/// \brief The iterator type of transcode_view.
template <unicode_char_type FromEncoding, unicode_char_type ToEncoding, std::ranges::input_range View>
  requires std::ranges::view<View>
class transcode_view<FromEncoding, ToEncoding, View>::iterator {
public:
  /// Defines this class as fulfilling the requirements of a forward iterator.
  using iterator_category = std::forward_iterator_tag;
  /// Define this class as following the forward iterator concept.
  using iterator_concept = std::forward_iterator_tag;

  /// The type produced by this iterator.
  using value_type = ToEncoding;
  /// A type that can be used to identify distance between iterators.
  using difference_type = std::ranges::range_difference_t<View>;

  iterator () requires std::default_initializable<std::ranges::iterator_t<View>> = default;
  constexpr iterator (transcode_view const& parent, std::ranges::iterator_t<View> const& current)
      : current_{current}, parent_{&parent}, state_{current} {
    assert (state_.empty ());
    // Prime the input state so that a dereference of the iterator will yield the first of the
    // output code-units.
    current_ = state_.fill (parent_);
  }

  /// \brief Returns the underlying view
  constexpr std::ranges::iterator_t<View> const& base () const& noexcept { return current_; }
  /// \brief Returns the underlying view
  constexpr std::ranges::iterator_t<View> base () && { return std::move (current_); }

  constexpr value_type const& operator* () const { return state_.front (); }
  constexpr std::ranges::iterator_t<View> operator->() const { return state_.front (); }

  constexpr iterator& operator++ () {
    state_.advance ();
    if (state_.empty ()) {
      // We've exhausted the stashed output code units. Refill the buffer and reset.
      current_ = state_.fill (parent_);
    }
    return *this;
  }
  constexpr void operator++ (int) { ++*this; }
  constexpr iterator operator++ (int) requires std::ranges::forward_range<View> {
    auto result = *this;
    ++*this;
    return result;
  }

  friend constexpr bool operator== (iterator const& lhs, iterator const& rhs)
    requires std::equality_comparable<std::ranges::iterator_t<View>>
  {
    return lhs.current_ == rhs.current_;
  }

  friend constexpr std::ranges::range_rvalue_reference_t<View> iter_move (iterator const& iter) noexcept (
      noexcept (std::ranges::iter_move (iter.current_))) {
    return std::ranges::iter_move (iter.current_);
  }

  friend constexpr void iter_swap (iterator const& lhs,
                                   iterator const& rhs) noexcept (noexcept (std::ranges::iter_swap (lhs.current_,
                                                                                                    rhs.current_)))
    requires std::indirectly_swappable<std::ranges::iterator_t<View>>
  {
    return std::ranges::iter_swap (lhs.current_, rhs.current_);
  }

private:
  std::ranges::iterator_t<View> current_{};
  transcode_view const* parent_ = nullptr;

  class state {
  public:
    constexpr explicit state (std::ranges::iterator_t<View> const& iter)
        : next_{iter}, valid_{out_.end (), out_.end ()} {}
    constexpr state () : state{std::ranges::iterator_t<View>{}} {}

    [[nodiscard]] constexpr bool empty () const noexcept { return valid_.empty (); }
    /// Returns the first element from the range of code units forming the current code point.
    [[nodiscard]] constexpr auto& front () const noexcept {
      assert (!valid_.empty ());
      return valid_.front ();
    }
    /// Removes the first element from the range of code units forming the current code point.
    constexpr void advance () noexcept {
      assert (!valid_.empty ());
      (void)valid_.advance (1);
    }

    /// Consumes enough code-units from the base iterator to form a single code-point. The resulting
    /// code-units in the output encoding can be sequentially accessed using the front() and
    /// advance() methods.
    ///
    /// \returns The updated base iterator.
    constexpr std::ranges::iterator_t<View> fill (transcode_view const* parent);

  private:
    using out_type = std::array<ToEncoding, longest_sequence_v<ToEncoding> * 2>;
    using iterator = typename out_type::iterator;

    std::ranges::iterator_t<View> next_;
    transcoder<FromEncoding, ToEncoding> transcoder_;
    /// The container into which the transcoder's output will be written.
    out_type out_{};
    /// The valid range of code units in the out_ container. Determines the code-units to be
    /// produced when the view is dereferenced.
    std::ranges::subrange<iterator> valid_;
  };
  mutable state state_{};
};

template <unicode_char_type FromEncoding, unicode_char_type ToEncoding, std::ranges::input_range View>
  requires std::ranges::view<View>
constexpr std::ranges::iterator_t<View> transcode_view<FromEncoding, ToEncoding, View>::iterator::state::fill (
    transcode_view const* parent) {
  auto result = next_;
  assert (this->empty () && "out_ was not empty when fill called");

  auto out_it = out_.begin ();
  auto const input_end = std::ranges::end (parent->base_);
  // Loop until we've produced a code-point's worth of code-units in the out
  // container or we've run out of input.
  while (out_it == out_.begin () && next_ != input_end) {
    out_it = transcoder_ (*next_, out_it);
    ++next_;
  }
  if (next_ == input_end) {
    // We've consumed the entire input so tell the transcoder and get any final output.
    out_it = transcoder_.end_cp (out_it);
  }
  assert (out_it >= out_.begin () && out_it <= out_.end ());
  if (!transcoder_.well_formed ()) {
    parent->well_formed_ = false;
  }
  valid_ = std::ranges::subrange<iterator>{out_.begin (), out_it};
  return result;
}

/// \brief The sentinel type of transcode_view when the underlying view is not a common_range.
template <unicode_char_type FromEncoding, unicode_char_type ToEncoding, std::ranges::input_range View>
  requires std::ranges::view<View>
class transcode_view<FromEncoding, ToEncoding, View>::sentinel {
public:
  sentinel () = default;
  constexpr explicit sentinel (transcode_view& parent) : end_{std::ranges::end (parent.base_)} {}
  constexpr std::ranges::sentinel_t<View> base () const { return end_; }
  /// \brief Compares and iterator and sential for equality.
  friend constexpr bool operator== (iterator const& lhs, sentinel const& rhs) { return lhs.current_ == rhs.end_; }

private:
  std::ranges::sentinel_t<View> end_{};
};

namespace views::transcode {

template <unicode_char_type FromEncoding, unicode_char_type ToEncoding> class transcode_range_adaptor {
public:
  template <std::ranges::viewable_range Range> constexpr auto operator() (Range&& range) const {
    return transcode_view<FromEncoding, ToEncoding, std::ranges::views::all_t<Range>>{std::forward<Range> (range)};
  }
};

template <unicode_char_type FromEncoding, unicode_char_type ToEncoding, std::ranges::viewable_range Range>
constexpr auto operator| (Range&& range, transcode_range_adaptor<FromEncoding, ToEncoding> const& adaptor) {
  return adaptor (std::forward<Range> (range));
}

}  // end namespace views::transcode

template <unicode_char_type FromEncoding, unicode_char_type ToEncoding>
inline constexpr auto transcode = views::transcode::transcode_range_adaptor<FromEncoding, ToEncoding>{};

}  // end namespace ranges

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

}  // end namespace icubaby

#ifdef ICUBABY_INSIDE_NS
}  // end namespace ICUBABY_INSIDE_NS
#endif

#endif  // ICUBABY_ICUBABY_HPP
