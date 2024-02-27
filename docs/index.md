---
title: icubaby
---
# icubaby

A C++ Baby Library to Immediately Convert Unicode. A portable, header-only, dependency-free, library for C++ 17 or later. Fast, minimal, and easy to use for converting sequences of text between any of the Unicode UTF encodings. It does not allocate dynamic memory and neither throws or catches exceptions.

> icubaby is in no way related to the [International Components for Unicode](https://icu.unicode.org) library!

## Introduction

C++ 17 [deprecated the standard library's `<codecvt>` header file](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0618r0.html) leading to the search for alternative libraries for Unicode conversion. icubaby is a portable, header-only, dependency-free C++ library designed for fast and immediate Unicode conversion between UTF encodings. Notably, it does not allocate dynamic memory and avoids exceptions. 

## Installation

To integrate icubaby into your C++ project, follow these steps:

1. Clone the repository

   ~~~bash
   git clone https://github.com/paulhuggett/icubaby.git
   ~~~

2. Include the `icubaby.hpp` header in your C++ code.

## Usage

1. C++ 20 Range Adaptor

   Leverage the C++20 Range Adaptor for expressive and concise code when working with ranges of elements.

   ~~~cpp
   auto const src = std::array{char32_t{0x1F600}};
   auto const r = src | icubaby::ranges::transcode<char32_t, char16_t>;
   std::vector<char16_t> out;
   std::ranges::copy(r, std::back_inserter(out));
   ~~~

   [Learn more](cxx20-range-adaptor.md)

2. Iterator Interface

   Utilize the iterator interface for flexibility in processing sequences of text with iterator-based algorithms.

   ~~~cpp
   auto const src = std::array{char32_t{0x1F600}};
   std::vector<char16_t> out;
   icubaby::t32_16 t;
   auto it = icubaby::iterator{&t, std::back_inserter (out)};
   it = std::copy (std::begin (src), std::end (src), it);
   t.end_cp (it);
   ~~~

   [Learn more](iterator-interface.md)

3. Explicit Conversion

   Drive the conversion one code-unit at a time, providing fine-grained control over the conversion process.

   ~~~cpp
   auto const src = std::array{char32_t{0x1F600}};
   std::vector<char16_t> out;
   icubaby::t8_16 t;
   auto it = std::back_inserter (out);
   for (auto cu: src) {
     it = t (cu, it);
   }
   t.end_cp (it);
   ~~~

   [Learn more](explicit-conversion.md)

## API

### Macro constants

Macro name            | Description
--------------------- | -----------
ICUBABY_CXX20         | Has value 1 when compiled with C++ 20 or later and 0 otherwise.
ICUBABY_CXX20REQUIRES | Used to enable the `require` keyword to state template constraints when compiled with C++ 20. An empty macro when compiled with versions of C++ prior to 20.

### Helper types

Type name | Description
--------- | -------------------------------------------------
t8_8      | A transcoder which converts from UTF-8 to UTF-8.<br>Equivalent to `using t8_8 = transcoder<char8_t, char8_t>`.
t8_16     | A transcoder which converts from UTF-8 to UTF-16.<br>Equivalent to `using t8_16 = transcoder<char8_t, char16_t>`.
t8_32     | A transcoder which converts from UTF-8 to UTF-32.<br>Equivalent to `using t8_32 = transcoder<char8_t, char32_t>`.
t16_8     | A transcoder which converts from UTF-16 to UTF-8.<br>Equivalent to `using t16_8 = transcoder<char16_t, char8_t>`.
t16_16    | A transcoder which converts from UTF-16 to UTF-16.<br>Equivalent to `using t16_16 = transcoder<char16_t, char16_t>`.
t16_32    | A transcoder which converts from UTF-16 to UTF-32.<br>Equivalent to `using t16_32 = transcoder<char16_t, char32_t>`.
t32_8     | A transcoder which converts from UTF-32 to UTF-8.<br>Equivalent to `using 32_8 = transcoder<char32_t, char8_t>`.
t32_16    | A transcoder which converts from UTF-32 to UTF-16.<br>Equivalent to `using t32_16 = transcoder<char32_t, char16_t>`.
t32_32    | A transcoder which converts from UTF-32 to UTF-32.<br>Equivalent to `using t32_32 = transcoder<char32_t, char32_t>`.

### Constants

Name | Description
---- | -----------
`replacement_char` | A name for the code point U+FFFD REPLACEMENT CHARACTER.
`code_point_bits` | The number of bits required to represent a code point. Starting with Unicode 2.0, characters are encoded in the range U+0000..U+10FFFF, which amounts to a 21-bit code space.
`first_high_surrogate ` | `inline constexpr auto first_high_surrogate = char32_t{0xD800};`
`last_high_surrogate` | `inline constexpr auto last_high_surrogate = char32_t{0xDBFF};`
`first_low_surrogate` | `inline constexpr auto first_low_surrogate = char32_t{0xDC00};`
`last_low_surrogate` | `inline constexpr auto last_low_surrogate = char32_t{0xDFFF};`
`max_code_point` | `inline constexpr auto max_code_point = char32_t{0x10FFFF};`

### Utilities

~~~cpp
constexpr bool is_high_surrogate (char32_t c) noexcept;
constexpr bool is_low_surrogate (char32_t c) noexcept;
constexpr bool is_surrogate (char32_t c) noexcept;
~~~

~~~cpp
constexpr bool is_code_point_start (char8_t c) noexcept;
constexpr bool is_code_point_start (char16_t c) noexcept;
constexpr bool is_code_point_start (char32_t c) noexcept;
~~~

#### length

Returns the number of code points in a sequence.

> Note: the input sequence must be well formed for the result to be accurate.

~~~cpp
#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

template <std::ranges::input_range R, typename Proj = std::identity>
  requires unicode_char_type<std::ranges::range_value_t<R>>
constexpr std::ranges::range_difference_t<R>
length (R&& r, Proj proj = {});

template <std::input_iterator I, std::sentinel_for<I> S, typename Proj = std::identity>
  requires unicode_char_type<typename std::iterator_traits<I>::value_type>
constexpr std::iter_difference_t<I>
length (I first, S last, Proj proj = {});

#else

template <typename InputIterator,
          typename = std::enable_if_t<is_unicode_char_type_v<typename std::iterator_traits<InputIterator>::value_type>>>
constexpr typename std::iterator_traits<InputIterator>::difference_type
length (InputIterator first, InputIterator last);

#endif
~~~

Parameter | Description
--------- | -----------
first     | The start of the range of code units to examine.
last      | The end of the range of code units to examine.
proj      | Projection to apply to the elements.
r         | The range of the elements to examine.

#### index

~~~cpp
#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

template <std::ranges::input_range R, typename Proj = std::identity>
constexpr std::ranges::borrowed_iterator_t<R>
index (R&& r, std::size_t pos, Proj proj = {});

template <std::input_iterator I, std::sentinel_for<I> S, typename Proj = std::identity>
constexpr I
index (I first, S last, std::size_t pos, Proj proj = {});

#else

template <typename InputIterator,
          typename = std::enable_if_t<is_unicode_char_type_v<typename std::iterator_traits<InputIterator>::value_type>>>
constexpr InputIterator
index (InputIterator first, InputIterator last, std::size_t pos);

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
~~~

Returns an iterator to the beginning of the pos'th code point in a range of code-units.

> Note: the input sequence must be well formed for the result to be accurate.

Parameter | Description
--------- | -----------
first     | The start of the range of elements to examine.
last      | The end of the range of elements to examine.
pos       | The number of code points to move.
r         | The range of code units to examine.

##### Returns

An iterator that is `pos` code-points after the start of the range or `last` if the end of the range was encountered.

### char8

~~~cpp
namespace icubaby {
#ifdef __cpp_char8_t
using char8 = char8_t;
#else
using char8 = char;
#endif
} // end namespace icubaby
~~~

C++ 20 introduced `char8_t` as the type for UTF-8 character representation. Since this library needs to work with earlier versions of C++, we have `icubaby::char8` which is defined as either `char8_t` (for C++ 20 or later) or `char` to match the type of a u8 string literal.

### transcoder

~~~cpp
namespace icubaby {
template <typename From, typename To>
class transcoder {
public:
  using input_type = From;
  using output_type = To;

  template <typename OutputIterator>
  OutputIterator operator() (input_type c, OutputIterator dest);

  template <typename OutputIterator>
  OutputIterator end_cp (OutputIterator dest);

  bool well_formed () const;
};
} // end namespace icubaby
~~~

Where `From` and `To` are each any of `icubaby::char8`, `char16_t`, or `char32_t`.

It’s possible for `From` and `To` to be the same character type. This can be used to both validate and/or correct unchecked input such as data arriving at a network port.

#### Member types

Member type | Definition
----------- | -----------
input_type  | The character type from which conversions will be performed. May be any of [`icubaby::char8`](#char8), `char16_t` or `char32_t`.
output_type | The character type to which the transcoder will convert. May be any of [`icubaby::char8`](#char8), `char16_t` or `char32_t`.

#### Member functions

Member function | Description
--------------- | -----------
(constructor)   | Constructs a new transcoder.
(destructor)    | Destructs a transcoder.
operator()      | Accepts a single code unit in the input encoding and, once an entire code point has been consumed, produces the equivalent code point expressed in the output encoding.
end_cp          | Call once the entire input has been fed to `operator()` to ensure the sequence did not end with a partial character.
well_formed     | Returns true if the input was well formed, false otherwise.
partial         | Returns true if part of a multi code unit code point has been consumed, false otherwise.

##### constructor

~~~cpp
transcoder ();
explicit transcoder (bool well_formed);
~~~

##### operator()

~~~cpp
template <typename OutputIterator>
  requires std::output_iterator<OutputIterator, output_type>
OutputIterator operator() (input_type c, OutputIterator dest) noexcept;
~~~

This member function is the heart of the transcoder. It accepts a single code unit in the input encoding and, once an entire code point has been consumed, produces the equivalent code point expressed in the output encoding. Malformed input is detected and be replaced with the Unicode [replacement character](https://unicode.org/glossary/#replacement_character) (U+FFFD REPLACEMENT CHARACTER).

###### Parameters

Name | Description
---- | -----------
c    | A code unit in the input encoding.
dest | An output iterator to which code units in the output encoding are written.

###### Return value

The `dest` iterator one past the last element assigned.

##### end_cp

~~~cpp
template <typename OutputIterator>
  requires std::output_iterator<OutputIterator, output_type>
OutputIterator end_cp (OutputIterator dest);
~~~

Call once the entire input has been fed to operator() to ensures the sequence did not end with a partial character.

###### Parameters

Name | Description
---- | -----------
dest | An output iterator to which code units in the output encoding are written.

###### Return value

The `dest` iterator one past the last element assigned.

##### well_formed

~~~cpp
[[nodiscard]] constexpr bool well_formed () const;
~~~

Returns true if the input was well formed, false otherwise.

### iterator

~~~cpp
template <typename Transcoder, typename OutputIterator>
class icubaby::iterator;
~~~

`icubaby::iterator<>` is an output iterator which can simplify use of transcoders with algorithms that take an iterator argument. When a value is written to the iterator, it calls the `transcoder<>::operator()` function. For example:

~~~cpp
icubaby::t8_16 t;
std::vector<char16_t> out;
auto it = icubaby::iterator{&t, std::back_inserter (out)};
*(it++) = char8_t{'A'};
~~~

After writing to the `icubaby::iterator` instance `it`, the vector `out` contains 0x0041.

`Transcoder` should be a type which implements the `transcoder<>` interface [described above](#transcoder); `OutputIterator` should be an [output iterator](https://en.cppreference.com/w/cpp/iterator/output_iterator) which produces values of type `Transcoder::output_type`.

#### Member types

Member type       | Definition
----------------- | --------------------------
iterator_category | `std::output_iterator_tag`
value_type        | `void`
difference_type   | `std::ptrdiff_t`
pointer           | `void`
reference         | `void`

#### Member functions

Member function | Description
--------------- | -----------
(constructor)   | Constructs a new iterator<br><small>(public member function)</small>.
operator=       | Passes an individual character to the associated transcoder<br><small>(public member function)</small>.
operator*       | no-op<br><small>(public member function)</small>
operator++      | no-op<br><small>(public member function)</small>
operator++(int) | no-op<br><small>(public member function)</small>
base            | Accesses the underlying iterator<br><small>(public member function)</small>.
transcoder      | Accesses the underlying transcoder<br><small>(public member function)</small>.

## Examples

### Convert using std::copy()

The example code below converts from UTF-8 to UTF-16 using `icubaby::t8_16` (this name is just a shortened form of `icubaby::transcoder<icubaby::char8, char16_t>`). To convert from UTF-<i>x</i> to UTF-<i>y</i> just use t<i>x</i>\_<i>y</i> (UTF-16 to UTF-8 is `t16_8`, UTF-32 to UTF-8 is `t32_8`, and so on).

In this code we use `std::copy()` to loop over the input code units and pass them to `icubaby::iterator<>`. `iterator<>` conveniently passes each code unit to the transcoder along with the supplied output iterator (`std::back_inserter(out)` here) and returns the updated iterator.

This code continues to process and generate characters even if we see badly formed input. Once all of the characters are processed, the function will return an empty optional if any input was bad. If the input was well formed, the UTF-16 equivalent is returned.

~~~cpp
#include "icubaby/icubaby.hpp"
#include <string_view>

// In C++17, std::u8stringview would become std::basic_string_view<icubaby::char8>

std::optional<std::u16string> convert (std::u8string_view const& src) {
  std::u16string out;

  // t8_16 is the class which converts from UTF-8 to UTF-16.
  icubaby::t8_16 utf_8_to_16;
  // We could combine the next three lines, if desired.
  auto it = icubaby::iterator{&utf_8_to_16, std::back_inserter (out)};
  it = std::copy (std::begin (src), std::end (src), it);
  utf_8_to_16.end_cp (it);
  if (!utf_8_to_16.well_formed ()) {
    // The input was malformed or ended with a partial character.
    return std::nullopt;
  }
  return out;
}

~~~

### Convert using an explicit loop

Whilst std::copy() provides a compact method of performing conversions, sometimes more control is needed. This example converts the code units using an explicit loop and exits as soon as malformed input is encountered.

~~~cpp
std::optional<std::u16string>
convert2 (std::u8string const & src) {
  // The UTF-16 code units are written to 'out' via the 'it' iterator.
  std::u16string out;
  auto it = std::back_inserter (out);
  icubaby::t8_16 utf_8_to_16;
  for (auto const c: src) {
    // Pass this UTF-8 code-unit to the transcoder.
    it = utf_8_to_16(c, it);
    if (!utf_8_to_16.well_formed()) {
      // The input was malformed. Bail immediately.
      return std::nullopt;
    }
  }
  // Tell the converter that this it the end of the sequence.
  utf_8_to_16.end_cp (it)
  // Check that we didn't end with a partial character.
  if (!utf_8_to_16.well_formed ()) {
    return std::nullopt;
  }
  return out; // Conversion was successful.
}
~~~

