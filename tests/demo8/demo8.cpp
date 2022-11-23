#include <iomanip>
#include <iostream>
#include <optional>
#include <string_view>

#include "icubaby/icubaby.hpp"

namespace {

void show (std::ostream& os, auto const& str) {
  os << std::setfill('0') << std::hex;
  auto separator = "";
  for (auto const c: str) {
    os << separator << std::setw (sizeof (c) * 2) << static_cast<std::uint16_t> (c);
    separator = " ";
  }
  os << '\n';
}

std::optional<std::u16string> convert (std::u8string_view const& src) {
  std::u16string out;

  // t8_16 is the class which converts from UTF-8 to UTF-16.
  // This name is a shortned form of transcoder<char8_t, char16_T>.
  icubaby::t8_16 utf_8_to_16;
  auto it = icubaby::iterator{&utf_8_to_16, std::back_inserter (out)};
  it = std::copy (std::begin (src), std::end (src), it);
  utf_8_to_16.finalize (it);
  if (!utf_8_to_16.good ()) {
    // The input was malformed or ended with a partial character.
    return std::nullopt;
  }
  return out;
}
std::optional<std::u16string> convert2 (std::u8string_view const& src) {
  // The UTF-16 code units are written to the 'out' string via the 'it' output iterator.
  std::u16string out;
  auto it = std::back_inserter (out);
  icubaby::t8_16 utf_8_to_16;
  for (char8_t const c: src) {
    // Pass this UTF-8 code-unit to the transcoder.
    it = utf_8_to_16(c, it);
    if (!utf_8_to_16.good()) {
      // The input was malformed. Bail out immediately.
      return std::nullopt;
    }
  }
  // Check that the input finished with a complete character.
  it = utf_8_to_16.finalize (it);
  if (!utf_8_to_16.good ()) {
    return std::nullopt;
  }
  return out; // Conversion was successful.
}

}  // namespace

int main () {
  using namespace std::string_view_literals;
  auto const in = u8"こんにちは世界\n"sv;
  show (std::cout, in);
  show (std::cout, *convert (in));
  show (std::cout, *convert2 (in));
}

