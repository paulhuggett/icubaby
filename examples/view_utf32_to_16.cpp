#include <array>
#include <vector>

#include "icubaby/icubaby.hpp"

int main () {
  // The code points to be converted. Here just a single U+1F600 GRINNING FACE emoji but obviously the array
  // could contain many more code points.
  auto const in = std::array{char32_t{0x1F600}};

  // Take the 'in' container and send it lazily through a UTF-32 to UTF-16 transcoder.
  auto r = in | icubaby::views::transcode<char32_t, char16_t>;

  // A vector to contain the output UTF=16 code units.
  std::vector<char16_t> out;

  // Copy from the input range to the output vector.
  std::ranges::copy (r, std::back_inserter (out));
}
