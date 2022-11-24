#include <array>
#include <bit>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

namespace {

// This test encodes every input code point and then decodes it to ensure that
// we get back the character we started with.
template <typename Encoder, typename Decoder>
ICUBABY_CXX20REQUIRES (
    (icubaby::is_transcoder<Encoder> && icubaby::is_transcoder<Decoder> &&
     std::is_same_v<typename Encoder::output_type,
                    typename Decoder::input_type> &&
     std::is_same_v<typename Encoder::input_type, char32_t> &&
     std::is_same_v<typename Decoder::output_type, char32_t>))
void check_each_code_point () {
  Encoder encode;
  Decoder decode;

  std::vector<typename Encoder::output_type> encoded;
  std::vector<typename Decoder::output_type> output;

  for (auto cp = char32_t{0}; cp <= icubaby::max_code_point; ++cp) {
    if (icubaby::is_surrogate (cp)) {
      continue;
    }

    auto encoded_it = std::back_inserter (encoded);
    encoded.clear ();
    encode.finalize (encode (cp, encoded_it));
    assert (encode.good ());

    output.clear ();
    auto it = icubaby::iterator{&decode, std::back_inserter (output)};
    std::copy (std::begin (encoded), std::end (encoded), it);
    it = decode.finalize (it);
    assert (decode.good ());

    assert (output.size () == 1);
    assert (output.front () == cp);
  }
}

std::vector<char32_t> all_code_points () {
  std::vector<char32_t> result;
  result.reserve (icubaby::max_code_point);
  for (auto cp = char32_t{0}; cp <= icubaby::max_code_point; ++cp) {
    // The high-surrogate and low-surrogate code points are designated for
    // surrogate code units in the UTF-16 character encoding form. They are
    // unassigned to any abstract character.
    if (!icubaby::is_surrogate (cp)) {
      result.push_back (cp);
    }
  }
  return result;
}

template <typename Encoder, typename Decoder>
ICUBABY_CXX20REQUIRES ((icubaby::is_transcoder<Encoder> &&
                        icubaby::is_transcoder<Decoder> &&
                        std::is_same_v<typename Encoder::output_type,
                                       typename Decoder::input_type>))
void check_all_code_points () {
  Encoder encode;
  icubaby::transcoder<typename Encoder::output_type,
                      typename Decoder::input_type>
      midcode;
  Decoder decode;

  // 1. Start with the set of all valid UTF-32 code points.
  std::vector<char32_t> const all = all_code_points ();

  // 2. Run the complete set of code points through the encoder.
  std::vector<typename Decoder::input_type> encoded;
  auto encoded_it =
      std::copy (std::begin (all), std::end (all),
                 icubaby::iterator{&encode, std::back_inserter (encoded)});
  encode.finalize (encoded_it);
  assert (encode.good ());

  // 2a. Pass the output from step 2 through the mid-coder.
  std::vector<typename Decoder::input_type> midcoded;
  auto midcoded_it =
      std::copy (std::begin (encoded), std::end (encoded),
                 icubaby::iterator{&midcode, std::back_inserter (midcoded)});
  midcode.finalize (midcoded_it);
  assert (midcode.good ());

  // 3. Run the encoded stream from step 2 through the decoder.
  std::vector<typename Decoder::output_type> decoded;
  auto decoded_it =
      std::copy (std::begin (midcoded), std::end (midcoded),
                 icubaby::iterator{&decode, std::back_inserter (decoded)});
  decode.finalize (decoded_it);
  assert (decode.good ());

  // 4. Ensure that the result matches the initial UTF-32 collection from
  // step 1.
  if (!std::equal (std::begin (all), std::end (all), std::begin (decoded),
                   std::end (decoded))) {
    std::cout << all.size () << '\n';
    std::cout << decoded.size () << '\n';

    for (size_t ctr = 0, end = std::min (all.size (), decoded.size ());
         ctr < end; ++ctr) {
      if (all[ctr] != decoded[ctr]) {
        std::cout << std::hex << ctr << ": "
                  << "U+" << std::hex << static_cast<unsigned> (all[ctr]) << ' '
                  << "U+" << std::hex << static_cast<unsigned> (decoded[ctr])
                  << '\n';
      }
    }
  }
}

void check_utf8_to_16 () {
  using namespace icubaby;
  // 1. Start with the set of all valid UTF-32 code points.
  std::vector<char32_t> const all = all_code_points ();

  // 2. Convert the complete set of code points to UTF-8.
  std::vector<char8> all8a;
  t32_8 convert32_8;
  convert32_8.finalize (
      std::copy (std::begin (all), std::end (all),
                 iterator{&convert32_8, std::back_inserter (all8a)}));
  assert (convert32_8.good ());

  // 3. Convert the UTF-8 stream from step 2 to UTF-16.
  std::vector<char16_t> all16;
  t8_16 convert8_16;
  convert8_16.finalize (
      std::copy (std::begin (all8a), std::end (all8a),
                 iterator{&convert8_16, std::back_inserter (all16)}));
  assert (convert8_16.good ());

  // 4. Convert the UTF-16 collection from step 3 to UTF-8.
  std::vector<char8> all8b;
  t16_8 convert16_8;
  convert16_8.finalize (
      std::copy (std::begin (all16), std::end (all16),
                 iterator{&convert16_8, std::back_inserter (all8b)}));
  assert (convert16_8.good ());

  // 5. Compare the results of step 2 and step 4.
  assert (std::equal (std::begin (all8a), std::end (all8a), std::begin (all8b),
                      std::end (all8b)));
}

}  // end anonymous namespace

using namespace std::string_literals;

int main () {
  int exit_code = EXIT_SUCCESS;
  try {
    check_each_code_point<icubaby::t32_8, icubaby::t8_32> ();
    check_each_code_point<icubaby::t32_16, icubaby::t16_32> ();
    check_each_code_point<icubaby::t32_32, icubaby::t32_32> ();

    check_all_code_points<icubaby::t32_8, icubaby::t8_32> ();
    check_all_code_points<icubaby::t32_16, icubaby::t16_32> ();
    check_all_code_points<icubaby::t32_32, icubaby::t32_32> ();

    check_utf8_to_16 ();

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
