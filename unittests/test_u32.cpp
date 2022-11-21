#include <gmock/gmock.h>

#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t32_8 ::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_8 ::output_type, char8_t>);
static_assert (std::is_same_v<icubaby::t32_16::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t32_32::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_32::output_type, char32_t>);

TEST (Utf32To8, MaxPlus1) {
  std::vector<char8_t> out;
  // This is the Unicode Replacement Character U+FFFD as UTF-8.
  std::vector<char8_t> const replacement{char8_t{0xEF}, char8_t{0xBF},
                                         char8_t{0xBD}};
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;

  it = d1 (icubaby::max_code_point + 1, it);
  EXPECT_FALSE (d1.good ());
  EXPECT_THAT (out, testing::ContainerEq (replacement));
}

TEST (Utf32To8, FirstLowSurrogate) {
  std::vector<char8_t> out;
  auto it = std::back_inserter (out);
  icubaby::t32_8 d1;
  it = d1 (icubaby::first_low_surrogate, it);
  EXPECT_FALSE (d1.good ());
}
