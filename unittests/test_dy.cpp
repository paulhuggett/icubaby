#include "icubaby/dy.hpp"

#include <vector>

#include <gmock/gmock.h>
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
TEST (ByteTranscoder, Utf8BOM) {
  std::vector<char32_t> output;
  auto dest = std::back_inserter (output);

  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
TEST (ByteTranscoder, Utf8FirstTwoBytesOfBOM) {
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
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
  icubaby::runtime_transcoder<char32_t> transcoder;
  std::vector<char32_t> output;
  (void)transcoder.end_cp (
      std::copy (std::begin (input), std::end (input), icubaby::iterator{&transcoder, std::back_inserter (output)}));
}
FUZZ_TEST (ByteTranscoder, ByteTranscoderNeverCrashes);
#endif  // ICUBABY_FUZZTEST
