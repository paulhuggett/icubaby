#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

int main () {
  // The bytes to be converted. An array here, but this could obviously come
  // from any source such as user input, a file, or a network endpoint. Note
  // that the icubaby transcoder deals with a single byte at a time so we don't
  // need to have the entire input available at any time.
  static std::array const input{
      std::byte{0xFE}, std::byte{0xFF}, std::byte{0x00}, std::byte{'H'},
      std::byte{0x00}, std::byte{'e'},  std::byte{0x00}, std::byte{'l'},
      std::byte{0x00}, std::byte{'l'},  std::byte{0x00}, std::byte{'o'},
      std::byte{0x00}, std::byte{' '},  std::byte{0x00}, std::byte{'W'},
      std::byte{0x00}, std::byte{'o'},  std::byte{0x00}, std::byte{'r'},
      std::byte{0x00}, std::byte{'l'},  std::byte{0x00}, std::byte{'d'},
      std::byte{0x00}, std::byte{'\n'}};

  // A vector to contain the UTF-8 output.
  std::vector<icubaby::char8> output;

  // An output iterator that will append each UTF-8 code unit to the `output`
  // vector.
  auto out_it = std::back_inserter (output);

  // The transcoder instance. We consume bytes (indicating that the transcoder
  // must decide on the input encoding) and emit icubaby::char8 (UTF-8).
  icubaby::transcoder<std::byte, icubaby::char8> transcode;

  // Call the transcoder for each source byte. Output goes to the 'out' output
  // iterator.
  for (auto b : input) {
    out_it = transcode (b, out_it);
  }

  // Tell the transcoder that it should have received a complete code point.
  // This always happens at the end of the input.
  (void)transcode.end_cp (out_it);

  // Write the output to the console. This example sticks to the ASCII subset of
  // code point, so this should work on most terminals!
  for (auto c : output) {
    std::cout << static_cast<char> (c);
  }
}
