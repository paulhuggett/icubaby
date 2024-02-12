#include "icubaby/dy.hpp"

#include <vector>

#include <gmock/gmock.h>

using testing::ElementsAre;

// NOLINTNEXTLINE
TEST (Dy, Utf8BOM) {
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  dest = transcoder (std::byte{0xEF}, dest);
  dest = transcoder (std::byte{0xBB}, dest);
  dest = transcoder (std::byte{0xBF}, dest);

  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (Dy, Utf8MissingBOM) {
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  dest = transcoder (std::byte{'A'}, dest);
  dest = transcoder (std::byte{'b'}, dest);
  dest = transcoder (std::byte{'c'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_TRUE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf8);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (Dy, Utf8FirstByteOfBOM) {
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
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (char32_t{0xF900}, char32_t{'A'}, char32_t{'b'}, char32_t{'c'}));
}
// NOLINTNEXTLINE
TEST (Dy, Utf8FirstTwoBytesOfBOM) {
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
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (char32_t{0xFEFC}, char32_t{'A'}, char32_t{'b'}, char32_t{'c'}));
}
// NOLINTNEXTLINE
TEST (Dy, Utf16BigEndianBOM) {
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
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf16be);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (Dy, Utf16FirstByteOfBigEndianBOM) {
  icubaby::runtime_transcoder<char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFE.
  dest = transcoder (std::byte{0xFE}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (icubaby::replacement_char, 'A'));
}
// NOLINTNEXTLINE
TEST (Dy, Utf16LittleEndianBOM) {
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
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf16le);
  EXPECT_THAT (output, ElementsAre ('A', 'b', 'c'));
}
// NOLINTNEXTLINE
TEST (Dy, Utf16FirstByteOfLittleEndianBOM) {
  icubaby::runtime_transcoder<char32_t> transcoder;
  std::vector<decltype (transcoder)::output_type> output;
  auto dest = std::back_inserter (output);
  // No legal UTF-8 sequence starts with 0xFF so we'll end up with someting ill-formed.
  dest = transcoder (std::byte{0xFF}, dest);
  dest = transcoder (std::byte{'A'}, dest);
  (void)transcoder.end_cp (dest);

  EXPECT_FALSE (transcoder.well_formed ());
  EXPECT_EQ (transcoder.selected_encoding (), decltype(transcoder)::encoding::utf8);
  EXPECT_THAT (output, ElementsAre (icubaby::replacement_char, 'A'));
}
