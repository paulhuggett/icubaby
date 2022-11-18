#include <array>
#include <bit>
#include <iostream>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t8_8 ::input_type, char8_t> &&
               std::is_same_v<icubaby::t8_8 ::output_type, char8_t>);
static_assert (std::is_same_v<icubaby::t8_16::input_type, char8_t> &&
               std::is_same_v<icubaby::t8_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t8_32::input_type, char8_t> &&
               std::is_same_v<icubaby::t8_32::output_type, char32_t>);
static_assert (std::is_same_v<icubaby::t16_8 ::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_8 ::output_type, char8_t>);
static_assert (std::is_same_v<icubaby::t16_16::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t16_32::input_type, char16_t> &&
               std::is_same_v<icubaby::t16_32::output_type, char32_t>);
static_assert (std::is_same_v<icubaby::t32_8 ::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_8 ::output_type, char8_t>);
static_assert (std::is_same_v<icubaby::t32_16::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t32_32::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_32::output_type, char32_t>);

namespace {

void good1 () {
  // | UTF-8 Sequence         | Code point                   |
  // | ---------------------- + ---------------------------- +
  // | 0x24,                  | U+0024  DOLLAR SIGN          |
  // | 0xC2, 0xA2,            | U+00A2  CENT SIGN            |
  // | 0xE0, 0xA4, 0xB9,      | U+0939  DEVANAGARI LETTER HA |
  // | 0xE2, 0x82, 0xAC,      | U+20AC  EURO SIGN            |
  // | 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN  |
  // | 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR  |
  icubaby::t8_32 d;
  assert (d.good ());
  std::array<char32_t, 6> cu{{0}};
  auto out = std::begin (cu);
  out = d (0x24, out);
  assert (out == std::begin (cu) + 1 && cu[0] == char32_t{0x0024});
  {
    // 0xC2, 0xA2 => U+00A2  CENT SIGN
    std::array<char8_t, 2> const cent_sign = {{0xC2, 0xA2}};
    out = d (cent_sign[0], out);
    assert (out == std::begin (cu) + 1);
    out = d (cent_sign[1], out);
    assert (d.good () && out == std::begin (cu) + 2 &&
            cu[1] == char32_t{0x00A2});
  }
  {
    // 0xE0, 0xA4, 0xB9 => U+0939 DEVANAGARI LETTER HA
    std::array<char8_t, 3> const devanagri_letter_ha{{0xE0, 0xA4, 0xB9}};
    out = d (devanagri_letter_ha[0], out);
    assert (out == std::begin (cu) + 2);
    out = d (devanagri_letter_ha[1], out);
    assert (out == std::begin (cu) + 2);
    out = d (devanagri_letter_ha[2], out);
    assert (out == std::begin (cu) + 3 && cu[2] == char32_t{0x0939});
  }
  {
    // 0xE2, 0x82, 0xAC => U+20AC EURO SIGN
    std::array<char8_t, 3> const euro_sign{{0xE2, 0x82, 0xAC}};
    out = d (euro_sign[0], out);
    assert (out == std::begin (cu) + 3);
    out = d (euro_sign[1], out);
    assert (out == std::begin (cu) + 3);
    out = d (euro_sign[2], out);
    assert (out == std::begin (cu) + 4 && cu[3] == char32_t{0x20AC});
  }
  {
    // 0xED, 0x95, 0x9C,      | U+D55C  HANGUL SYLLABLE HAN
    std::array<char8_t, 3> const hangul_syllable_han{{0xED, 0x95, 0x9C}};
    out = d (hangul_syllable_han[0], out);
    assert (out == std::begin (cu) + 4);
    out = d (hangul_syllable_han[1], out);
    assert (out == std::begin (cu) + 4);
    out = d (hangul_syllable_han[2], out);
    assert (out == std::begin (cu) + 5 && cu[4] == char32_t{0xD55C});
  }
  {
    // 0xF0, 0x90, 0x8D, 0x88 | U+10348 GOTHIC LETTER HWAIR
    std::array<char8_t, 4> const gothic_letter_hwair{{0xF0, 0x90, 0x8D, 0x88}};
    out = d (gothic_letter_hwair[0], out);
    assert (out == std::begin (cu) + 5);
    out = d (gothic_letter_hwair[1], out);
    assert (out == std::begin (cu) + 5);
    out = d (gothic_letter_hwair[2], out);
    assert (out == std::begin (cu) + 5);
    out = d (gothic_letter_hwair[3], out);
    assert (out == std::begin (cu) + 6 && cu[5] == char32_t{0x10348});
  }

  assert (d.finalize ());
  assert (d.good ());
}

template <typename T>
class null_output_iterator {
public:
  null_output_iterator& operator= (char32_t) { return *this; }
  null_output_iterator& operator* () { return *this; }
  null_output_iterator& operator++ () { return *this; }
};

// 80 24
// 80 first possible two-CU sequence
void bad1 () {
  icubaby::t8_32 d2;
  assert (d2.good ());
#if 0
  assert (d2 (0x80) == 0xFFFD);
  assert (!d2.good ());
  assert (d2 (0x24) == 0x0024);
  assert (!d2.finalize ());
  assert (!d2.good ());
#endif
}

void bad2 () {
#if 0
  icubaby::utf8_decoder d2;
  assert (d2 (0x80) == 0xFFFD);
  assert (!d2.finalize ());
  assert (!d2.good ());
#endif
}

void good16 () {
  {
    std::array<char32_t, 4> out;
    auto it = std::begin (out);
    icubaby::t16_32 d1;
    assert (d1.good ());

    it = d1 (1, it);
    assert (it == std::begin (out) + 1 && out[0] == 0x0001);
    it = d1 (2, it);
    assert (it == std::begin (out) + 2 && out[1] == 0x0002);
    it = d1 (3, it);
    assert (it == std::begin (out) + 3 && out[2] == 0x0003);
    it = d1 (4, it);
    assert (it == std::begin (out) + 4 && out[3] == 0x0004);
  }
  {
    std::array<char32_t, 5> out;
    auto it = std::begin (out);
    icubaby::t16_32 d2;
    it = d2 (0xFFFF, it);
    assert (it == std::begin (out) + 1 && out[0] == 0xFFFF);
    it = d2 (0xD800, it);
    assert (it == std::begin (out) + 1);
    it = d2 (0xDC00, it);
    assert (it == std::begin (out) + 2 && out[1] == 0x10000);
    //    assert (d2 (0xDC00) == 0x10000);
    it = d2 (0xD800, it);
    assert (it == std::begin (out) + 2);
    it = d2 (0xDC01, it);
    assert (it == std::begin (out) + 3 && out[2] == 0x10001);

    it = d2 (0xD808, it);
    assert (it == std::begin (out) + 3);
    it = d2 (0xDF45, it);
    assert (it == std::begin (out) + 4 && out[3] == 0x12345);

    it = d2 (0xDBFF, it);
    assert (it == std::begin (out) + 4);
    it = d2 (0xDFFF, it);
    assert (it == std::begin (out) + 5 && out[4] == 0x10ffff);
  }
}

// This test encodes every input code point and then decodes it to ensure that
// we get back the character we started with.
template <typename Encoder, typename Decoder>
  requires (icubaby::is_transcoder<Encoder> &&
            icubaby::is_transcoder<Decoder> &&
            std::is_same_v<typename Encoder::output_type,
                           typename Decoder::input_type> &&
            std::is_same_v<typename Encoder::input_type, char32_t> &&
            std::is_same_v<typename Decoder::output_type, char32_t>)
void check_each_code_point (Encoder encode, Decoder decode) {
  std::vector<typename Encoder::output_type> encoded;
  std::vector<typename Decoder::output_type> output;

  for (auto cp = char32_t{0}; cp <= icubaby::max_code_point; ++cp) {
    if (icubaby::is_surrogate (cp)) {
      continue;
    }

    encoded.clear ();
    encode (cp, std::back_inserter (encoded));
    assert (encode.good ());

    output.clear ();
    std::copy (std::begin (encoded), std::end (encoded),
               icubaby::iterator{decode, std::back_inserter (output)});
    assert (decode.finalize ());
    assert (decode.good ());

    assert (output.size () == 1);
    assert (output.front () == cp);
  }
}

std::vector<char32_t> all_code_points () {
  std::vector<char32_t> result;
  result.reserve (icubaby::max_code_point);
  for (auto cp = char32_t{0}; cp <= icubaby::max_code_point; ++cp) {
    if (!icubaby::is_surrogate (cp)) {
      result.push_back (cp);
    }
  }
  return result;
}

template <typename Encoder, typename Decoder>
  requires (icubaby::is_transcoder<Encoder> &&
            icubaby::is_transcoder<Decoder> &&
            std::is_same_v<typename Encoder::output_type,
                           typename Decoder::input_type>)
void check_all_code_points () {
  Encoder encode;
  Decoder decode;

  // 1. Start with the set of all valid UTF-32 code points.
  std::vector<char32_t> const all = all_code_points ();

  // 2. Run the complete set of code points through the encoder.
  std::vector<typename Decoder::input_type> encoded;
  std::copy (std::begin (all), std::end (all),
             icubaby::iterator{encode, std::back_inserter (encoded)});
  assert (encode.finalize ());
  assert (encode.good ());

  // 3. Run the encoded stream from step 2 through the decoder.
  std::vector<char32_t> decoded;
  std::copy (std::begin (encoded), std::end (encoded),
             icubaby::iterator{decode, std::back_inserter (decoded)});
  assert (decode.finalize ());
  assert (decode.good ());

  // 4. Ensure that the result matches the initial UTF-32 collection from
  // step 1.
  assert (std::equal (std::begin (all), std::end (all), std::begin (decoded),
                      std::end (decoded)));
}

void check_utf8_to_16 () {
  using namespace icubaby;
  // 1. Start with the set of all valid UTF-32 code points.
  std::vector<char32_t> const all = all_code_points ();

  // 2. Convert the complete set of code points to UTF-8.
  std::vector<char8_t> all8a;
  t32_8 convert32_8;
  std::copy (std::begin (all), std::end (all),
             iterator{convert32_8, std::back_inserter (all8a)});
  assert (convert32_8.finalize ());
  assert (convert32_8.good ());

  // 3. Convert the UTF-8 stream from step 2 to UTF-16.
  std::vector<char16_t> all16;
  t8_16 convert8_16;
  std::copy (std::begin (all8a), std::end (all8a),
             iterator{convert8_16, std::back_inserter (all16)});
  assert (convert8_16.finalize ());
  assert (convert8_16.good ());

  // 4. Convert the UTF-16 collection from step 3 to UTF-8.
  std::vector<char8_t> all8b;
  t16_8 convert16_8;
  std::copy (std::begin (all16), std::end (all16),
             iterator{convert16_8, std::back_inserter (all8b)});
  assert (convert16_8.finalize ());
  assert (convert16_8.good ());

  // 5. Compare the results of step 2 and step 4.
  assert (std::equal (std::begin (all8a), std::end (all8a), std::begin (all8b),
                      std::end (all8b)));
}

void exhaustive_check () {
  check_each_code_point (icubaby::t32_8{}, icubaby::t8_32{});
  check_each_code_point (icubaby::t32_16{}, icubaby::t16_32{});
  check_each_code_point (icubaby::t32_32{}, icubaby::t32_32{});

  check_all_code_points<icubaby::t32_8, icubaby::t8_32> ();
  check_all_code_points<icubaby::t32_16, icubaby::t16_32> ();
  check_all_code_points<icubaby::t32_32, icubaby::t32_32> ();

  check_utf8_to_16 ();
}

}  // end anonymous namespace

using namespace std::string_literals;

int main () {
  int exit_code = EXIT_SUCCESS;
  try {
    exhaustive_check ();

    good1 ();
    bad1 ();
    bad2 ();
    good16 ();

    auto const in = u8"Hello, world\n"s;
    std::u16string out;
    icubaby::t8_16 utf_8_to_16;
    std::copy (std::begin (in), std::end (in),
               icubaby::iterator{utf_8_to_16, std::back_inserter (out)});
    if (!utf_8_to_16.finalize ()) {
      std::cerr << "Input stream was ill-formed!\n";
    }

    std::cout << "Tests passed\n";
  } catch (std::exception const& ex) {
    std::cerr << "An error occurred: " << ex.what () << '\n';
    exit_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred\n";
    exit_code = EXIT_FAILURE;
  }
  return exit_code;
}
