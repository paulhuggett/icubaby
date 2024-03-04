#include <iostream>

#include "icubaby/icubaby.hpp"

// The ICUBABY_HAVE_RANGES and ICUBABY_HAVE_CONCEPTS macros are true if the corresponding features are available
// in both the compiler and standard library.
#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

int main () {
  // The bytes to be converted. An array here, but this could obviously come from any source such as user input, a
  // file, or a network endpoint. Note that the icubaby transcoder deals with a single byte at a time so we don't
  // need to have the entire input available at any time.
  static std::array const input{
      std::byte{0xFE}, std::byte{0xFF}, std::byte{0x00}, std::byte{'H'}, std::byte{0x00}, std::byte{'e'},
      std::byte{0x00}, std::byte{'l'},  std::byte{0x00}, std::byte{'l'}, std::byte{0x00}, std::byte{'o'},
      std::byte{0x00}, std::byte{' '},  std::byte{0x00}, std::byte{'W'}, std::byte{0x00}, std::byte{'o'},
      std::byte{0x00}, std::byte{'r'},  std::byte{0x00}, std::byte{'l'}, std::byte{0x00}, std::byte{'d'},
  };

  // A pipeline where the input array is converted from a series of bytes to a stream of UTF-8 code units
  // and then finally to std::uint_least8_t for display to the user.
  auto range = input | icubaby::views::transcode<std::byte, char8_t> |
               std::views::transform ([] (char8_t code_unit) { return static_cast<std::uint_least8_t> (code_unit); });

  // Copy the elements of range directly to `std::cout`.
  (void)std::ranges::copy (range, std::ostream_iterator<std::uint_least8_t> (std::cout));
}

#else

int main () {
  std::cout << "Sorry, icubaby C++ 20 ranges aren't supported by your build.\n";
}

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
