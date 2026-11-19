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

// icubaby itself
#include "icubaby/icubaby.hpp"

// standard library
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <vector>

#if ICUBABY_HAVE_RANGES
#include <ranges>
#endif  // ICUBABY_HAVE_RANGES

// google mock/test/fuzz
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#if defined(ICUBABY_FUZZTEST) && ICUBABY_FUZZTEST
#include <fuzztest/fuzztest.h>

#ifdef __cpp_lib_char8_t
namespace testing::internal {

inline void PrintTo(char8_t c, ::std::ostream* os) {
  // TODO(b/418738869): Incorrect for values not representing valid codepoints.
  // Also see https://github.com/google/googletest/issues/4762.
  PrintTo(static_cast<char32_t>(c), os);
}

} // end namespace testing::internal
#endif // __cpp_lib_char8_t
#endif // ICUBABY_FUZZTEST

// Local includes
#include "typed_test.hpp"

using testing::ElementsAre;
using testing::ElementsAreArray;

namespace icubaby {

// Teach Google Test how to display values of type icubaby::encoding.
void PrintTo (encoding enc, std::ostream* stream);
void PrintTo (encoding const enc, std::ostream* const stream) {
  char const* str = nullptr;
  switch (enc) {
  case encoding::unknown: str = "unknown"; break;
  case encoding::utf8: str = "utf8"; break;
  case encoding::utf16be: str = "utf16be"; break;
  case encoding::utf16le: str = "utf16le"; break;
  case encoding::utf32be: str = "utf32be"; break;
  case encoding::utf32le: str = "utf32le"; break;
  default: str = "**error**"; break;
  }
  *stream << str;
}

}  // end namespace icubaby

// NOLINTNEXTLINE
TEST (ByteTranscoder, Empty) {
  std::vector<char32_t> output;
  auto dest = std::back_inserter (output);

  icubaby::transcoder<std::byte, char32_t> transcoder;
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ());
  EXPECT_TRUE (output.empty ());
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf8BOM) {
  std::vector<char32_t> output;
  auto dest = std::back_inserter (output);

  icubaby::transcoder<std::byte, char32_t> transcoder;
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  dest = transcoder (std::byte{0xEF}, dest);
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  dest = transcoder (std::byte{0xBB}, dest);
  EXPECT_TRUE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  dest = transcoder (std::byte{0xBF}, dest);
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());

  dest = transcoder (std::byte{'A'}, dest);
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  dest = transcoder (std::byte{'b'}, dest);
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  dest = transcoder (std::byte{'c'}, dest);
  EXPECT_FALSE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.partial ());
  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf8MissingBOM) {
  icubaby::transcoder<std::byte, icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf8FirstByteOfBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // EF A4 80 is U+F900 CJK COMBATIBILITY IDEOGRAPH-F900
  dest = transcoder (std::byte{0xEF}, dest);
  dest = transcoder (std::byte{0xA4}, dest);
  dest = transcoder (std::byte{0x80}, dest);

  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (char32_t{0xF900}, char32_t{'A'}, char32_t{'b'}, char32_t{'c'}));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf8FirstByteOfBOMOnly) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  (void)transcoder.end_cp (transcoder (std::byte{0xEF}, std::back_inserter (output)));

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (icubaby::replacement_char));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf8FirstTwoBytesOfBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // EF BB BC is U+FEFC ARABIC LIGATURE LAM WITH ALEF FINAL FORM
  dest = transcoder (std::byte{0xEF}, dest);
  dest = transcoder (std::byte{0xBB}, dest);
  dest = transcoder (std::byte{0xBC}, dest);

  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (char32_t{0xFEFC}, char32_t{'A'}, char32_t{'b'}, char32_t{'c'}));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf16BigEndianBOM) {
  icubaby::transcoder<std::byte, icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  dest = transcoder (std::byte{0xFE}, dest);
  dest = transcoder (std::byte{0xFF}, dest);

  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'A'}, dest);

  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'b'}, dest);

  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf16be);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf16FirstByteOfBigEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFE.
  dest = transcoder (std::byte{0xFE}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (icubaby::replacement_char, 'A'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf16LittleEndianBOM) {
  icubaby::transcoder<std::byte, icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  dest = transcoder (std::byte{0xFF}, dest);
  dest = transcoder (std::byte{0xFE}, dest);

  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{0x00}, dest);

  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{0x00}, dest);

  dest = transcoder (std::byte{'c'}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf16le);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf16FirstByteOfLittleEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with something ill-formed.
  dest = transcoder (std::byte{0xFF}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (icubaby::replacement_char, 'A'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf32BigEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with something ill-formed.
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0xFE}, dest);
  dest = transcoder (std::byte{0xFF}, dest);

  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'A'}, dest);

  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'b'}, dest);

  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf32be);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf32FirstByteOfBigEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with something ill-formed.
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ('\0', 'A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf32FirstTwoBytesOfBigEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with something ill-formed.
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ('\0', '\0', 'A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf32FirstThreeBytesOfBigEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with something ill-formed.
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0xFE}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ('\0', '\0', icubaby::replacement_char, 'A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf32LittleEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with something ill-formed.
  dest = transcoder (std::byte{0xFF}, dest);
  dest = transcoder (std::byte{0xFE}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);

  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);

  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);

  dest = transcoder (std::byte{'c'}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  dest = transcoder (std::byte{0x00}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf32le);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf32FirstByteOfLittleEndianBOM) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with something ill-formed.
  dest = transcoder (std::byte{0xFF}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (icubaby::replacement_char, 'A', 'b', 'c'));
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
namespace {

std::vector<std::byte> make_bytes (std::initializer_list<unsigned> const values) {
  std::vector<std::byte> result;
  result.reserve (values.size ());
  (void)std::transform (std::begin (values), std::end (values), std::back_inserter (result),
                        [] (unsigned const value) { return static_cast<std::byte> (value); });
  return result;
}

template <typename T> class ByteTranscoderTruncated : public testing::Test {
protected:
  using output_type = T;

  /// Converts a sequence of code points to the output encoding.
  static std::vector<output_type> encode (std::initializer_list<char32_t> const code_points) {
    std::vector<output_type> result;
    icubaby::transcoder<char32_t, output_type> transcoder;
    auto dest = std::back_inserter (result);
    for (auto const code_point : code_points) {
      dest = transcoder (code_point, dest);
    }
    (void)transcoder.end_cp (dest);
    return result;
  }

  /// Passes \p head to a byte transcoder followed by the first \p count bytes of \p tail, then calls end_cp(). Checks
  /// partial() as each byte of \p tail is consumed, then the output and the partial() and well_formed() states.
  ///
  /// \param head  Input bytes that end on a code unit boundary.
  /// \param head_partial  The expected value of partial() once \p head has been consumed.
  /// \param tail  The bytes of a single code unit.
  /// \param count  The number of bytes of \p tail to be consumed.
  /// \param expected  The expected output code points.
  /// \param well_formed  The expected value of well_formed() after end_cp().
  static void check (std::vector<std::byte> const& head, bool const head_partial, std::vector<std::byte> const& tail,
                     std::size_t const count, std::initializer_list<char32_t> const expected, bool const well_formed) {
    icubaby::transcoder<std::byte, output_type> transcoder;
    std::vector<output_type> output;
    auto dest = std::back_inserter (output);
    for (auto const value : head) {
      dest = transcoder (value, dest);
    }
    EXPECT_EQ (transcoder.partial (), head_partial);
    for (auto index = std::size_t{0}; index < count; ++index) {
      dest = transcoder (tail.at (index), dest);
      EXPECT_TRUE (transcoder.partial ()) << "after byte #" << index << " of the final code unit";
    }
    (void)transcoder.end_cp (dest);
    EXPECT_FALSE (transcoder.partial ());
    EXPECT_EQ (transcoder.well_formed (), well_formed);
    EXPECT_THAT (output, ElementsAreArray (encode (expected)));
  }

  /// Checks the input made of \p bom, then optionally \p unit (the encoding of 'A'), then each possible truncation
  /// of the code unit \p tail.
  ///
  /// \param bom_partial  The expected value of partial() once \p bom has been consumed. This is true if the bytes
  ///   could also be the start of a different byte order mark.
  static void truncations (std::vector<std::byte> const& bom, std::vector<std::byte> const& unit,
                           std::vector<std::byte> const& tail, bool const bom_partial = false) {
    std::vector<std::byte> head = bom;
    head.insert (std::end (head), std::begin (unit), std::end (unit));
    for (auto count = std::size_t{0}; count < tail.size (); ++count) {
      SCOPED_TRACE (testing::Message () << count << " byte(s) of the final code unit");
      if (count == 0) {
        check (head, false, tail, count, {'A'}, true);
      } else {
        // The bytes of an incomplete code unit straight after the BOM.
        check (bom, bom_partial, tail, count, {icubaby::replacement_char}, false);
        // The bytes of an incomplete code unit after a complete one.
        check (head, false, tail, count, {'A', icubaby::replacement_char}, false);
      }
    }
  }
};

}  // end anonymous namespace

TYPED_TEST_SUITE (ByteTranscoderTruncated, OutputTypes, OutputTypeNames);

// NOLINTNEXTLINE
TYPED_TEST (ByteTranscoderTruncated, Utf16BE) {
  TestFixture::truncations (make_bytes ({0xFE, 0xFF}), make_bytes ({0x00, 'A'}), make_bytes ({0x00, 'B'}));
}
// NOLINTNEXTLINE
TYPED_TEST (ByteTranscoderTruncated, Utf16LE) {
  // FF FE could be the start of a UTF-32 LE byte order mark so the transcoder is in the partial state.
  TestFixture::truncations (make_bytes ({0xFF, 0xFE}), make_bytes ({'A', 0x00}), make_bytes ({'B', 0x00}), true);
}
// NOLINTNEXTLINE
TYPED_TEST (ByteTranscoderTruncated, Utf32BE) {
  TestFixture::truncations (make_bytes ({0x00, 0x00, 0xFE, 0xFF}), make_bytes ({0x00, 0x00, 0x00, 'A'}),
                            make_bytes ({0x00, 0x00, 0x00, 'B'}));
}
// NOLINTNEXTLINE
TYPED_TEST (ByteTranscoderTruncated, Utf32LE) {
  TestFixture::truncations (make_bytes ({0xFF, 0xFE, 0x00, 0x00}), make_bytes ({'A', 0x00, 0x00, 0x00}),
                            make_bytes ({'B', 0x00, 0x00, 0x00}));
}
// A high surrogate followed by an incomplete code unit. The high surrogate is reported with a U+FFFD REPLACEMENT
// CHARACTER as it would be if it were followed by any complete code unit other than a low surrogate; the incomplete
// code unit produces a second.
// NOLINTNEXTLINE
TYPED_TEST (ByteTranscoderTruncated, Utf16BEHighSurrogate) {
  TestFixture::check (make_bytes ({0xFE, 0xFF, 0xD8, 0x3D}), true, make_bytes ({0xDE, 0x00}), 1,
                      {icubaby::replacement_char, icubaby::replacement_char}, false);
}
// NOLINTNEXTLINE
TYPED_TEST (ByteTranscoderTruncated, Utf16LEHighSurrogate) {
  TestFixture::check (make_bytes ({0xFF, 0xFE, 0x3D, 0xD8}), true, make_bytes ({0x00, 0xDE}), 1,
                      {icubaby::replacement_char, icubaby::replacement_char}, false);
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

#if ICUBABY_HAVE_RANGES
// NOLINTNEXTLINE
TEST (ByteTranscoder, RangesNoBOM) {
  std::array const input{std::byte{'H'}, std::byte{'e'}, std::byte{'l'}, std::byte{'l'}, std::byte{'o'}};
  std::vector<char32_t> output;

  auto range = input | icubaby::views::transcode<std::byte, char32_t>;
  (void)std::ranges::copy (range, std::back_inserter (output));

  EXPECT_THAT (output, ElementsAre (char32_t{'H'}, char32_t{'e'}, char32_t{'l'}, char32_t{'l'}, char32_t{'o'}));
  EXPECT_TRUE (range.well_formed ());
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, RangesUtf8BOM) {
  std::array const input{std::byte{0xEF}, std::byte{0xBB}, std::byte{0xBF}, std::byte{'H'},
                         std::byte{'e'},  std::byte{'l'},  std::byte{'l'},  std::byte{'o'}};
  std::vector<char32_t> output;

  auto range = input | icubaby::views::transcode<std::byte, char32_t>;
  (void)std::ranges::copy (range, std::back_inserter (output));

  EXPECT_THAT (output, ElementsAre (char32_t{'H'}, char32_t{'e'}, char32_t{'l'}, char32_t{'l'}, char32_t{'o'}));
  EXPECT_TRUE (range.well_formed ());
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, RangesUtf16BE) {
  std::array const input{std::byte{0xFE}, std::byte{0xFF}, std::byte{0x00},
                         std::byte{'A'},  std::byte{0x00}, std::byte{'b'}};
  std::vector<char32_t> output;

  auto range = input | icubaby::views::transcode<std::byte, char32_t>;
  (void)std::ranges::copy (range, std::back_inserter (output));
  EXPECT_THAT (output, ElementsAre (char32_t{'A'}, char32_t{'b'}));
  EXPECT_TRUE (range.well_formed ());
}
// NOLINTNEXTLINE
TEST (ByteTranscoder, Utf16BEAndIterMove) {
  std::array const input{std::byte{0xFE}, std::byte{0xFF}, std::byte{0x00},
                         std::byte{'A'},  std::byte{0x00}, std::byte{'b'}};
  std::vector<char32_t> output;
  output.reserve (2);
  auto range = input | icubaby::views::transcode<std::byte, char32_t>;
  for (std::move_iterator first{range.begin ()}, last{range.end ()}; first != last; ++first) {
    (void)output.emplace_back (iter_move (first));
  }
  EXPECT_THAT (output, ElementsAre (char32_t{'A'}, char32_t{'b'}));
  EXPECT_TRUE (range.well_formed ());
}
#endif  // ICUBABY_HAVE_RANGES

namespace {

void ByteTranscoderNeverCrashes (std::vector<std::byte> const& input) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<char32_t> output;
  auto output_iterator = icubaby::iterator{&transcoder, std::back_inserter (output)};
#if ICUBABY_HAVE_RANGES
  output_iterator = std::ranges::copy (input, output_iterator).out;
#else
  output_iterator = std::copy (std::begin (input), std::end (input), output_iterator);
#endif  // ICUBABY_HAVE_RANGES
  (void)transcoder.end_cp (output_iterator);
}

}  // end anonymous namespace

#if ICUBABY_FUZZTEST
// NOLINTNEXTLINE
FUZZ_TEST (ByteTranscoder, ByteTranscoderNeverCrashes);
#endif  // ICUBABY_FUZZTEST

// NOLINTNEXTLINE
TEST (ByteTranscoder, ByteTranscoderNeverCrashesWithEmptyInput) {
  ByteTranscoderNeverCrashes (std::vector<std::byte>{});
}
