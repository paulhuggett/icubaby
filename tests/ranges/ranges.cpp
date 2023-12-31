#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>
#include <vector>
#include <version>

#include "icubaby/icubaby.hpp"

#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L

template <typename CharType>
struct char_to_output_type {};
template <>
struct char_to_output_type<char8_t> {
  using type = unsigned;
};
template <>
struct char_to_output_type<char16_t> {
  using type = std::uint_least16_t;
};
template <>
struct char_to_output_type<char32_t> {
  using type = std::uint_least32_t;
};

template <typename CharType>
void dump_vector (std::vector<CharType> const& v) {
  constexpr auto width = std::is_same_v<CharType, char8_t> ? 2 : 4;
  auto const* separator = "";
  for (auto c : v) {
    std::cout << separator << "0x" << std::setw (width) << std::setfill ('0') << std::uppercase
              << std::hex << static_cast<typename char_to_output_type<CharType>::type> (c);
    separator = " ";
  }
  std::cout << '\n';
}

int main () {
  // clang-format off
  std::vector const in{
      char8_t{0xE3}, char8_t{0x81}, char8_t{0x93},  // U+3053 HIRAGANA LETTER KO
      char8_t{0xE3}, char8_t{0x82}, char8_t{0x93},  // U+3093 HIRAGANA LETTER N
      char8_t{0xE3}, char8_t{0x81}, char8_t{0xAB},  // U+306B HIRAGANA LETTER NI
      char8_t{0xE3}, char8_t{0x81}, char8_t{0xA1},  // U+3061 HIRAGANA LETTER TI
      char8_t{0xE3}, char8_t{0x81}, char8_t{0xAF},  // U+306F HIRAGANA LETTER HA
      char8_t{0xE4}, char8_t{0xB8}, char8_t{0x96},  // U+4E16 CJK UNIFIED IDEOGRAPH-4E16
      char8_t{0xE7}, char8_t{0x95}, char8_t{0x8C},  // U+754C CJK UNIFIED IDEOGRAPH-754C
      char8_t{0x0A}   // U+000A LINE FEED
  };
  // clang-format on
  {
    std::cout << "Convert the UTF-8 stream to UTF-16:\n";
    std::vector<char16_t> out16;
    std::ranges::copy (in | icubaby::ranges::transcode<char8_t, char16_t>,
                       std::back_inserter (out16));
    dump_vector (out16);
    assert ((out16 ==
             std::vector{{char16_t{0x3053}, char16_t{0x3093}, char16_t{0x306B},
                          char16_t{0x3061}, char16_t{0x306F}, char16_t{0x4E16},
                          char16_t{0x754C}, char16_t{0x000A}}}));
  }

  std::cout << "Convert the UTF-8 stream to UTF-32:\n";
  std::vector<char32_t> out32;
  std::ranges::copy (in | icubaby::ranges::transcode<char8_t, char32_t>,
                     std::back_inserter (out32));
  dump_vector (out32);
  assert ((out32 ==
           std::vector{{char32_t{0x3053}, char32_t{0x3093}, char32_t{0x306B},
                        char32_t{0x3061}, char32_t{0x306F}, char32_t{0x4E16},
                        char32_t{0x754C}, char32_t{0x000A}}}));

  std::vector<char16_t> out16;
  {
    std::cout << "Convert the UTF-32 stream to UTF-16:\n";
    std::ranges::copy (out32 | icubaby::ranges::transcode<char32_t, char16_t>,
                       std::back_inserter (out16));
    dump_vector (out16);
    assert ((out16 ==
             std::vector{{char16_t{0x3053}, char16_t{0x3093}, char16_t{0x306B},
                          char16_t{0x3061}, char16_t{0x306F}, char16_t{0x4E16},
                          char16_t{0x754C}, char16_t{0x000A}}}));
  }

  {
    std::cout << "Convert the UTF-16 stream to UTF-32:\n";
    std::vector<char32_t> out32;
    std::ranges::copy (out16 | icubaby::ranges::transcode<char16_t, char32_t>,
                       std::back_inserter (out32));
    dump_vector (out32);
    assert ((out32 ==
             std::vector{{char32_t{0x3053}, char32_t{0x3093}, char32_t{0x306B},
                          char32_t{0x3061}, char32_t{0x306F}, char32_t{0x4E16},
                          char32_t{0x754C}, char32_t{0x000A}}}));
  }

  std::cout << "Convert the UTF-16 stream to UTF-8:\n";
  std::vector<char8_t> out8;
  std::ranges::copy (out16 | icubaby::ranges::transcode<char16_t, char8_t>,
                     std::back_inserter (out8));
  dump_vector (out8);
  assert (std::ranges::equal (in, out8));
}

#else

int main () {
  std::cout << "Sorry, C++ icubaby ranges aren't supported yet.\n";
}

#endif
