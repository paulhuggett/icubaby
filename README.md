# icubaby

A C++ Baby Library to Immediately Convert Unicode. icubaby is a portable,
header-only, dependency-free library for C++ 17 or later. It is fast, minimal,
and easy to use for converting sequences of text between any of the Unicode UTF
encodings. It does not allocate dynamic memory and neither throws nor catches
exceptions (although exceptions thrown by an output iterator that you supply
are passed through to you).

> icubaby is in no way related to the
> [International Components for Unicode](https://icu.unicode.org) library and has
> no connection to any Intensive Care Unit!

## Status

| Category | Badges |
| --- | --- |
| License | [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) |
| Continuous Integration | [![CI Build & Test](https://github.com/paulhuggett/icubaby/actions/workflows/ci.yaml/badge.svg)](https://github.com/paulhuggett/icubaby/actions/workflows/ci.yaml) [![Documentation Status](https://readthedocs.org/projects/paulhuggett-icubaby/badge/?version=latest)](https://paulhuggett-icubaby.readthedocs.io/en/latest/?badge=latest) |
| Static Analysis | [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=paulhuggett_icubaby&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=paulhuggett_icubaby) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/d7aafd88d8ef4be7b03b568e957f0103)](https://app.codacy.com/gh/paulhuggett/icubaby/dashboard) [![CodeQL](https://github.com/paulhuggett/icubaby/actions/workflows/codeql.yaml/badge.svg)](https://github.com/paulhuggett/icubaby/actions/workflows/codeql.yaml) [![Microsoft C++ Code Analysis](https://github.com/paulhuggett/icubaby/actions/workflows/msvc.yaml/badge.svg)](https://github.com/paulhuggett/icubaby/actions/workflows/msvc.yaml) [![Coverity](https://img.shields.io/coverity/scan/29639.svg)](https://scan.coverity.com/projects/paulhuggett-icubaby) |
| Runtime Analysis | [![Fuzz Test](https://github.com/paulhuggett/icubaby/actions/workflows/fuzztest.yaml/badge.svg)](https://github.com/paulhuggett/icubaby/actions/workflows/fuzztest.yaml) [![codecov](https://codecov.io/gh/paulhuggett/icubaby/graph/badge.svg?token=YFO0SOXQE9)](https://codecov.io/gh/paulhuggett/icubaby) |
| [OpenSSF](https://openssf.org) |  [![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/paulhuggett/icubaby/badge)](https://securityscorecards.dev/viewer/?uri=github.com/paulhuggett/icubaby) [![OpenSSF Best Practices](https://www.bestpractices.dev/projects/8282/badge)](https://www.bestpractices.dev/projects/8282) |

## Introduction

C++ 17 [deprecated](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0618r0.html)
the standard library's `<codecvt>` header file which contained its Unicode
conversion facets. Those features weren’t easy to use correctly, but without
them code must look to other libraries. icubaby is such a library: it converts
text between the Unicode encodings. It is simple to use and exceptionally
simple to integrate into a project.

The library offers an API which converts to and from UTF-8, UTF-16, or UTF-32
encodings. It can also consume a byte stream where an optional
[byte order mark](https://en.wikipedia.org/wiki/Byte_order_mark) at the start of
the stream identifies both the source encoding and byte order. If there is no
byte order mark, the stream is assumed to be UTF-8.

Malformed input is never an error: each ill-formed sequence is replaced by
U+FFFD REPLACEMENT CHARACTER, and a transcoder’s `well_formed()` member function
tells you whether any was found.

## Installation

icubaby is entirely contained within a single header file. Installation can be
as simple as copying that file ([include/icubaby/icubaby.hpp](include/icubaby/icubaby.hpp))
into your project. It has no dependencies and self-configures to your environment.

## Usage

Check out the project documentation: https://paulhuggett-icubaby.readthedocs.io/en/latest/

icubaby uses four different types to express the different Unicode encodings that it supports:

| Type | Meaning |
| ---- | ------- |
| `std::byte` | Encoding and byte order are determined by a byte order mark at the start of the stream (UTF-8 if there is none). Input only. |
| `icubaby::char8` | UTF-8. `icubaby::char8` is defined as `char8_t` when the native type is available and `char` otherwise |
| `char16_t` | UTF-16 host-native endian |
| `char32_t` | UTF-32 host-native endian |

There are three ways to use the icubaby library depending on your needs:

1. [C++ 20 range adaptor](#c-20-range-adaptor)
2. [Output Iterator interface](#the-output-iterator-interface)
3. [Converting one code-unit at a time](#converting-one-code-unit-at-a-time)

### 1. C++ 20 Range Adaptor

C++ 20 introduced the ranges library for more composable and less
error-prone interaction with iterators and containers. In icubaby, we can
transform a range of input values from one Unicode encoding to another using
a single range adaptor:

```cpp
auto const in = std::array{char32_t{0x1F600}};
auto r = in | icubaby::views::transcode<char32_t, char16_t>;
std::vector<char16_t> out;
std::ranges::copy(r, std::back_inserter(out));
```

This code converts a single Unicode code-point 😀 (U+1F600 GRINNING FACE) from
UTF-32 to UTF-16 and will copy two UTF-16 code-units (0xD83D and 0xDE00) into
the `out` vector.

```cpp
auto const in = std::array{std::byte{0xFE}, std::byte{0xFF}, std::byte{0x00},
                           std::byte{'A'},  std::byte{0x00}, std::byte{'b'}};
auto r = in | icubaby::views::transcode<std::byte, icubaby::char8>;
std::vector<icubaby::char8> out;
std::ranges::copy(r, std::back_inserter(out));
```

This snippet converts “Ab” (U+0041 LATIN CAPITAL LETTER A, U+0062 LATIN SMALL
LETTER B) from big-endian UTF-16 to UTF-8. The leading byte order mark (FE FF)
selects the input encoding and is not copied to the output.

See the [C++20 Range Adaptor documentation](https://paulhuggett-icubaby.readthedocs.io/en/latest/ranges.html)
for more details.

### 2. The Output Iterator Interface

```cpp
auto const in = std::vector{static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
                            static_cast<icubaby::char8> (0x98), static_cast<icubaby::char8> (0x80)};
std::vector<char16_t> out;
icubaby::t8_16 t;
auto it = icubaby::iterator{&t, std::back_inserter (out)};
for (auto cu: in) {
  *(it++) = cu;
}
it = t.end_cp (it);
```

The `icubaby::iterator<>` class offers a familiar output iterator for using a
transcoder. Each code unit in the input encoding is written to the iterator,
which writes the equivalent code units in the output encoding to a second
iterator. This lets you use standard algorithms such as
[`std::copy`](https://en.cppreference.com/w/cpp/algorithm/copy) with the
library. Calling the transcoder’s `end_cp()` member function once all of the
input has been written checks that the input didn’t end part way through a code
point.

### 3. Converting One Code-Unit at a Time

Let’s try converting a single Unicode emoji character 😀 (U+1F600 GRINNING
FACE) expressed as four UTF-8 code units (0xF0, 0x9F, 0x98, 0x80) to UTF-16
(where it is the surrogate pair 0xD83D, 0xDE00).

```cpp
std::vector<char16_t> out;
auto it = std::back_inserter (out);
icubaby::t8_16 t;
for (auto cu: {0xF0, 0x9F, 0x98, 0x80}) {
  it = t (cu, it);
}
it = t.end_cp (it);
```

The `out` vector will contain the two UTF-16 code units 0xD83D and 0xDE00. See
the [explicit conversion documentation](https://paulhuggett-icubaby.readthedocs.io/en/latest/explicit-conversion.html)
for more details.
