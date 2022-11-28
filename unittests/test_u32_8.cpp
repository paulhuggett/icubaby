#include <gmock/gmock.h>

#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t32_8 ::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_8 ::output_type, icubaby::char8>);
static_assert (std::is_same_v<icubaby::t32_16::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t32_32::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_32::output_type, char32_t>);

// NOLINTNEXTLINE
TEST (Utf32To8, MaxPlus1) {
  std::vector<icubaby::char8> out;
  // This is the Unicode Replacement Character U+FFFD as UTF-8.
  std::vector<icubaby::char8> const replacement{
      static_cast<icubaby::char8> (0xef), static_cast<icubaby::char8> (0xbf),
      static_cast<icubaby::char8> (0xbd)};
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;

  it = d1 (icubaby::max_code_point + 1, it);
  EXPECT_FALSE (d1.well_formed ());
  EXPECT_THAT (out, testing::ContainerEq (replacement));
}

// NOLINTNEXTLINE
TEST (Utf32To8, FirstLowSurrogate) {
  std::vector<icubaby::char8> out;
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;
  it = d1 (icubaby::first_low_surrogate, it);
  EXPECT_FALSE (d1.well_formed ());
}

// NOLINTNEXTLINE
TEST (Utf32To8, LowestTwoByteSequence) {
  std::vector<icubaby::char8> out;
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;
  it = d1 (0x80, it);
  EXPECT_TRUE (d1.well_formed ());
  d1.end_cp (it);
  EXPECT_TRUE (d1.well_formed ());
  EXPECT_THAT (out, testing::ElementsAre (static_cast<icubaby::char8> (0xc2),
                                          static_cast<icubaby::char8> (0x80)));
}
