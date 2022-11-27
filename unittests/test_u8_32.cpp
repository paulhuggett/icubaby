#include <gmock/gmock.h>

#include <array>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

// NOLINTNEXTLINE(misc-redundant-expression)
static_assert (std::is_same_v<icubaby::t8_8 ::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_8 ::output_type, icubaby::char8>);
static_assert (std::is_same_v<icubaby::t8_16::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t8_32::input_type, icubaby::char8> &&
               std::is_same_v<icubaby::t8_32::output_type, char32_t>);

using testing::ElementsAre;

// | UTF-8 Sequence         | Code point                   |
// | ---------------------- + ---------------------------- +
// | 0x24,                  | U+0024  DOLLAR SIGN          |
// | 0xC2, 0xA2,            | U+00A2  CENT SIGN            |
// | 0xE0, 0xA4, 0xB9,      | U+0939  DEVANAGARI LETTER HA |
// | 0xE2, 0x82, 0xAC,      | U+20AC  EURO SIGN            |
// | 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN  |
// | 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR  |

// NOLINTNEXTLINE
TEST (Utf8To32, GoodDollarSign) {
  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  EXPECT_TRUE (d.well_formed ());
  out = d (0x24, out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0024}));
  d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0024}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodCentSign) {
  // 0xC2, 0xA2 => U+00A2  CENT SIGN
  std::array<icubaby::char8, 2> const cent_sign = {
      {static_cast<icubaby::char8> (0xC2), static_cast<icubaby::char8> (0xA2)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (cent_sign[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (cent_sign[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x00A2}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x00A2}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodDevanagariLetterHa) {
  // 0xE0, 0xA4, 0xB9 => U+0939 DEVANAGARI LETTER HA
  std::array<icubaby::char8, 3> const devanagri_letter_ha{
      {static_cast<icubaby::char8> (0xE0), static_cast<icubaby::char8> (0xA4),
       static_cast<icubaby::char8> (0xB9)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (devanagri_letter_ha[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (devanagri_letter_ha[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (devanagri_letter_ha[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0939}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x0939}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodEuroSign) {
  // 0xE2, 0x82, 0xAC => U+20AC EURO SIGN
  std::array<icubaby::char8, 3> const euro_sign{
      {static_cast<icubaby::char8> (0xE2), static_cast<icubaby::char8> (0x82),
       static_cast<icubaby::char8> (0xAC)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (euro_sign[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (euro_sign[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (euro_sign[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x20AC}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x20AC}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodHangulSyllableHan) {
  // 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN
  std::array<icubaby::char8, 3> const hangul_syllable_han{
      {static_cast<icubaby::char8> (0xED), static_cast<icubaby::char8> (0x95),
       static_cast<icubaby::char8> (0x9C)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (hangul_syllable_han[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (hangul_syllable_han[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (hangul_syllable_han[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0xD55C}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0xD55C}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, GoodGothicLetterHwair) {
  // 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR
  std::array<icubaby::char8, 4> const gothic_letter_hwair{
      {static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x90),
       static_cast<icubaby::char8> (0x8D), static_cast<icubaby::char8> (0x88)}};

  std::vector<char32_t> cu;
  auto out = std::back_inserter (cu);

  icubaby::t8_32 d;
  out = d (gothic_letter_hwair[0], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (gothic_letter_hwair[1], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (gothic_letter_hwair[2], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_TRUE (cu.empty ());
  out = d (gothic_letter_hwair[3], out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x10348}));
  out = d.end_cp (out);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (cu, ElementsAre (char32_t{0x10348}));
}

// NOLINTNEXTLINE
TEST (Utf8To32, Bad1) {
  icubaby::t8_32 d2;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d2 (static_cast<icubaby::char8> (0x80), it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  EXPECT_FALSE (d2.well_formed ());
  it = d2 (icubaby::char8{0x24}, it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char, char32_t{0x24}));
  EXPECT_FALSE (d2.well_formed ());
  it = d2.end_cp (it);
  EXPECT_FALSE (d2.well_formed ());
}

// NOLINTNEXTLINE
TEST (Utf8To32, Bad2) {
  icubaby::t8_32 d2;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d2 (static_cast<icubaby::char8> (0x80), it);
  EXPECT_FALSE (d2.well_formed ());
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  it = d2.end_cp (it);
  EXPECT_THAT (out, ElementsAre (icubaby::replacement_char));
  EXPECT_FALSE (d2.well_formed ());
}

// NOLINTNEXTLINE
TEST (Utf8To32, AssignBad) {
  icubaby::t32_8 t1;
  std::vector<icubaby::char8> out;
  // A code unit t1 will signal as an error (!good()).
  t1.end_cp (t1 (icubaby::first_low_surrogate, std::back_inserter (out)));
  EXPECT_FALSE (t1.well_formed ());

  icubaby::t8_32 t2{t1.well_formed ()};
  EXPECT_FALSE (t2.well_formed ()) << "The 'good' state should be transfered";
}

// NOLINTNEXTLINE
TEST (Utf8To32, PartialEndCp) {
  // U+1F0A6 PLAYING CARD SIX OF SPADES
  // UTF-8: F0 9F 82 A6

  icubaby::t8_32 d;
  std::vector<char32_t> out;
  auto it = std::back_inserter (out);
  it = d (static_cast<icubaby::char8> (0xF0), it);
  it = d (static_cast<icubaby::char8> (0x9F), it);
  it = d (static_cast<icubaby::char8> (0x82), it);
  it = d (static_cast<icubaby::char8> (0xA6), it);
  EXPECT_TRUE (d.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x1F0A6}));
  // Now just the first two bytes of that sequence.
  it = d (static_cast<icubaby::char8> (0xF0), it);
  it = d (static_cast<icubaby::char8> (0x9F), it);
  EXPECT_TRUE (d.well_formed ());
  it = d.end_cp (it);
  EXPECT_FALSE (d.well_formed ());
  EXPECT_THAT (out, ElementsAre (char32_t{0x1F0A6}, icubaby::replacement_char));
}
