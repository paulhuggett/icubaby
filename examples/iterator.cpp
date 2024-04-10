#include <iostream>
#include <iterator>
#include <string_view>
#include <vector>

#include "icubaby/icubaby.hpp"

namespace {

// Dump the number of code units and code points within the supplied container.
template <typename Container>
void describe (Container const& container, std::string_view const encoding) {
  auto const code_units = container.size ();
  auto const code_points =
      icubaby::length (std::begin (container), std::end (container));

  std::cout << encoding << " is " << code_units << ' '
            << (code_units == 1U ? "code unit" : "code units") << " and "
            << code_points << ' '
            << (code_points == 1U ? "code point" : "code points") << '\n';
}

}  // end anonymous namespace

int main () {
  // The input: we start with a vector of UTF-8 code units. In this case a
  // single U+1F600 GRINNING FACE code point. Note that we use icubaby::char8
  // here for compatibility with C++ 17. If you always use C++ 20 or later, you
  // can bypass this type and simply use char8_t.
  auto const input = std::vector{
      static_cast<icubaby::char8> (0xF0), static_cast<icubaby::char8> (0x9F),
      static_cast<icubaby::char8> (0x98), static_cast<icubaby::char8> (0x80)};
  describe (input, "UTF-8");

  // A second vector which will contain the UTF-16 output.
  std::vector<char16_t> output;

  // Instantiate a transcoder which can convert from UTF-8 to UTF-16.
  icubaby::t8_16 transcoder;

  // Now an output iterator based on the t8_16 transcoder which can consume
  // UTF-8 input and convert it to UTF-16. It will then emit the results to a
  // second output iterator (`inserter`) which appends those code units to the
  // `output` vector.
  auto inserter = std::back_inserter (output);
  auto output_it = icubaby::iterator{&transcoder, inserter};

  // Loop through the input assigning each UTF-16 to the `it` output iterator
  // created on the previous line.
  for (auto code_unit : input) {
    *(output_it++) = code_unit;
  }

  // Tell the transcoder that the input has been completely processed.
  (void)transcoder.end_cp (output_it);

  describe (output, "UTF-16");
  std::cout << "Input " << (transcoder.well_formed () ? "was" : "was not")
            << " well formed\n";
}
