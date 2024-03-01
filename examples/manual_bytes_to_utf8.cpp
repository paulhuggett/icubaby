#include <iostream>

#include "icubaby/icubaby.hpp"

int main () {
  // The bytes to be converted. An array here, but this could obviously come from any source such as user input, a
  // file, or a network endpoint. Note that the icubaby transcoder deals with a single byte at a time so we don't
  // need to have the entire input available at any time.
  static std::array const input{std::byte{0xFE}, std::byte{0xFF}, std::byte{0x00}, std::byte{'H'},  std::byte{0x00},
                                std::byte{'e'},  std::byte{0x00}, std::byte{'l'},  std::byte{0x00}, std::byte{'l'},
                                std::byte{0x00}, std::byte{'o'},  std::byte{0x00}, std::byte{' '},  std::byte{0x00},
                                std::byte{'W'},  std::byte{0x00}, std::byte{'o'},  std::byte{0x00}, std::byte{'r'},
                                std::byte{0x00}, std::byte{'l'},  std::byte{0x00}, std::byte{'d'},  std::byte{0x00},
                                std::byte{'\n'}};

  // An output iterator: where the UTF-8 output will be written.
  std::ostream_iterator<std::uint_least8_t> out{std::cout};

  // The transcoder instance. We consume bytes (indicating that the transcoder must decide on the input encoding)
  // and emit icubaby::char8 (UTF-8).
  icubaby::transcoder<std::byte, icubaby::char8> transcode;

  // Call the transcoder for each source byte. Output goes to the 'out' output iterator.
  for (auto b : input) {
    out = transcode (b, out);
  }

  // Tell the transoder that it should have received a complete code point. This always happens at the end of the input.
  (void)transcode.end_cp (out);
}
