#include <vector>

#include "icubaby/icubaby.hpp"

int main () {
  // The input: we start with a vector of UTF-8 code units. In this case a single U+1F600 GRINNING FACE code point.
  // Note that we use icubaby::char8 here for compatibility with C++ 17. If you know that you will always use C++ 20 or
  // later, you can bypass this type and simply use char8_t.
  auto const in = std::vector{static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
                              static_cast<icubaby::char8> (0x98), static_cast<icubaby::char8> (0x80)};

  // A second vector which will contain the UTF-16 output.
  std::vector<char16_t> out;

  // Instantiate a transcoder which can convert from UTF-8 to UTF-16.
  icubaby::t8_16 t;

  // Now an output iterator based on the t8_16 transcoder which can consume UTF-8 input, convert it to UTF-16 and emit
  // the result to a second output iterator which appends those code units to the 'out' vector.
  auto it = icubaby::iterator{&t, std::back_inserter (out)};

  // Loop through the input assigning each UTF-16 to the output iterator created on the previous line.
  for (auto cu : in) {
    *(it++) = cu;
  }

  // Tell the transcoder that the input has been completely processed.
  (void)t.end_cp (it);
}
