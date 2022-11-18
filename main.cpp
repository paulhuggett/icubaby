#include <array>
#include <bit>
#include <iostream>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

static_assert (std::is_same_v<icubaby::t32_8 ::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_8 ::output_type, char8_t>);
static_assert (std::is_same_v<icubaby::t32_16::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_16::output_type, char16_t>);
static_assert (std::is_same_v<icubaby::t32_32::input_type, char32_t> &&
               std::is_same_v<icubaby::t32_32::output_type, char32_t>);

namespace {

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
