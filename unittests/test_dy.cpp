#include "icubaby/dy.hpp"

#include <vector>

#include <gmock/gmock.h>

using testing::ElementsAre;

TEST (Dy, Utf8BOM) {
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> out;
  auto out_it = std::back_inserter (out);
  out_it = transcoder (std::byte{0xEF}, out_it);
  out_it = transcoder (std::byte{0xBB}, out_it);
  out_it = transcoder (std::byte{0xBF}, out_it);
  out_it = transcoder (std::byte{'A'}, out_it);
  out_it = transcoder (std::byte{'b'}, out_it);
  out_it = transcoder (std::byte{'c'}, out_it);
  (void)transcoder.end_cp (out_it);
  EXPECT_THAT (out, ElementsAre ('A', 'b', 'c'));
}
TEST (Dy, Utf16BigEndianBOM) {
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> out;
  auto out_it = std::back_inserter (out);
  out_it = transcoder (std::byte{0xFE}, out_it);
  out_it = transcoder (std::byte{0xFF}, out_it);
  out_it = transcoder (std::byte{0x00}, out_it);
  out_it = transcoder (std::byte{'A'}, out_it);
  out_it = transcoder (std::byte{0x00}, out_it);
  out_it = transcoder (std::byte{'b'}, out_it);
  out_it = transcoder (std::byte{0x00}, out_it);
  out_it = transcoder (std::byte{'c'}, out_it);
  (void)transcoder.end_cp (out_it);
  EXPECT_THAT (out, ElementsAre ('A', 'b', 'c'));
}
TEST (Dy, Utf16LittleEndianBOM) {
  icubaby::runtime_transcoder<icubaby::char8> transcoder;
  std::vector<decltype (transcoder)::output_type> out;
  auto out_it = std::back_inserter (out);
  out_it = transcoder (std::byte{0xFF}, out_it);
  out_it = transcoder (std::byte{0xFE}, out_it);
  out_it = transcoder (std::byte{'A'}, out_it);
  out_it = transcoder (std::byte{0x00}, out_it);
  out_it = transcoder (std::byte{'b'}, out_it);
  out_it = transcoder (std::byte{0x00}, out_it);
  out_it = transcoder (std::byte{'c'}, out_it);
  out_it = transcoder (std::byte{0x00}, out_it);
  (void)transcoder.end_cp (out_it);
  EXPECT_THAT (out, ElementsAre ('A', 'b', 'c'));
}
