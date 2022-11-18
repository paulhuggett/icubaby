#include <gmock/gmock.h>

#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t16_8 ::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_8 ::output_type, char8_t>);
static_assert (std::is_same_v<icubaby::t16_16::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t16_32::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_32::output_type, char32_t>);

using testing::ElementsAre;

TEST (Utf16, Good1) {
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  icubaby::t16_32 d1;
  EXPECT_TRUE (d1.good ());

  it = d1 (char16_t{1}, it);
  EXPECT_TRUE (d1.good ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x0001}));
  it = d1 (char16_t{2}, it);
  EXPECT_TRUE (d1.good ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x0001}, char32_t{0x0002}));
  it = d1 (char16_t{3}, it);
  EXPECT_TRUE (d1.good ());
  EXPECT_THAT (
      out, ElementsAre (char32_t{0x0001}, char32_t{0x0002}, char32_t{0x0003}));
  it = d1 (char16_t{4}, it);
  EXPECT_TRUE (d1.good ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x0001}, char32_t{0x0002},
                                 char32_t{0x0003}, char32_t{0x0004}));
}

TEST (Utf16, Good2) {
  auto const match1 = ElementsAre (char32_t{0xFFFF});
  auto const match2 = ElementsAre (char32_t{0xFFFF}, char32_t{0x10000});
  auto const match3 =
      ElementsAre (char32_t{0xFFFF}, char32_t{0x10000}, char32_t{0x10001});
  auto const match4 = ElementsAre (char32_t{0xFFFF}, char32_t{0x10000},
                                   char32_t{0x10001}, char32_t{0x12345});
  auto const match5 =
      ElementsAre (char32_t{0xFFFF}, char32_t{0x10000}, char32_t{0x10001},
                   char32_t{0x12345}, char32_t{0x10ffff});

  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  icubaby::t16_32 d2;
  it = d2 (0xFFFF, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match1);
  it = d2 (0xD800, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match1);
  it = d2 (0xDC00, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match2);
  it = d2 (0xD800, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match2);
  it = d2 (0xDC01, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match3);

  it = d2 (0xD808, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match3);
  it = d2 (0xDF45, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match4);

  it = d2 (0xDBFF, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match4);
  it = d2 (0xDFFF, it);
  EXPECT_TRUE (d2.good ());
  EXPECT_THAT (out, match5);
}
