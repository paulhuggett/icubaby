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

#include <gmock/gmock.h>

#include <vector>

#include "icubaby/icubaby.hpp"
#if ICUBABY_FUZZTEST
#include "fuzztest/fuzztest.h"
#endif

using testing::ElementsAre;

namespace icubaby {

// Teach Google Test how to display values of type icubaby::encoding.
void PrintTo (encoding enc, std::ostream* os);
void PrintTo (encoding enc, std::ostream* os) {
  char const* str = "**error**";
  switch (enc) {
  case encoding::unknown: str = "unknown"; break;
  case encoding::utf8: str = "utf8"; break;
  case encoding::utf16be: str = "utf16be"; break;
  case encoding::utf16le: str = "utf16le"; break;
  case encoding::utf32be: str = "utf32be"; break;
  case encoding::utf32le: str = "utf32le"; break;
  }
  *os << str;
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
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
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
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
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
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
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
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
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
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
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
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
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
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
  dest = transcoder (std::byte{0xFF}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), icubaby::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (icubaby::replacement_char, 'A', 'b', 'c'));
}

#if ICUBABY_FUZZTEST
static void ByteTranscoderNeverCrashes (std::vector<std::byte> const& input) {
  icubaby::transcoder<std::byte, char32_t> transcoder;
  std::vector<char32_t> output;
  (void)transcoder.end_cp (
      std::copy (std::begin (input), std::end (input), icubaby::iterator{&transcoder, std::back_inserter (output)}));
}
FUZZ_TEST (ByteTranscoder, ByteTranscoderNeverCrashes);
#endif  // ICUBABY_FUZZTEST
