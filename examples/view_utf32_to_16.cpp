#include <algorithm>
#include <array>
#include <iterator>
#include <vector>

#include "icubaby/icubaby.hpp"

// The ICUBABY_HAVE_RANGES and ICUBABY_HAVE_CONCEPTS macros are true if the
// corresponding features are available in both the compiler and standard
// library.
#if ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

int main () {
  // The code points to be converted. Here just a single U+1F600 GRINNING FACE
  // emoji but obviously the array could contain many more code points.
  auto const input = std::array{char32_t{0x1F600}};

  // Take the 'input' container and send it lazily through a UTF-32 to UTF-16
  // transcoder.
  auto r = input | icubaby::views::transcode<char32_t, char16_t>;

  // A vector to contain the output UTF=16 code units.
  std::vector<char16_t> out;

  // Copy from the input range to the output vector.
  std::ranges::copy (r, std::back_inserter (out));
}

#else

int main () {
  std::cout << "Sorry, icubaby C++ 20 ranges aren't supported by your build.\n";
}

#endif  // ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
