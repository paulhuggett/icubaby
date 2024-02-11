#ifndef ICUBABY_DY_HPP
#define ICUBABY_DY_HPP

#include <bit>
#include <variant>

#include "icubaby/icubaby.hpp"

namespace icubaby {

#if defined(__cpp_lib_endian) && __cpp_lib_endian >= 201907L
using std::endian;
#elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
enum class endian { little = __ORDER_LITTLE_ENDIAN__, big = __ORDER_BIG_ENDIAN__, native = __BYTE_ORDER__ };
#elif defined(_WIN32) || defined(__i386__) || defined(__x86_64__)
enum class endian { little = 0, big = 1, native = little };
#else
#error "Can't determine endianness of the target system"
#endif
static_assert (endian::native == endian::big || endian::native == endian::little, "Endianness must be big or little.");

constexpr std::uint_least32_t byte_swap (std::uint_least32_t value) {
  return static_cast<std::uint_least32_t> (((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) |
                                           ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24));
}
constexpr std::uint_least16_t byte_swap (std::uint_least16_t value) {
  return static_cast<std::uint_least16_t> (((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8));
}
constexpr std::uint_least32_t to_big_endian (std::uint_least32_t value) {
  if constexpr (endian::native == endian::big) {
    return value;
  } else {
    return byte_swap (value);
  }
}
constexpr std::uint_least16_t to_big_endian (std::uint_least16_t value) {
  if constexpr (endian::native == endian::big) {
    return value;
  } else {
    return byte_swap (value);
  }
}
constexpr std::uint_least32_t to_little_endian (std::uint_least32_t value) {
  if constexpr (endian::native == endian::little) {
    return value;
  } else {
    return byte_swap (value);
  }
}
constexpr std::uint_least16_t to_little_endian (std::uint_least16_t value) {
  if constexpr (endian::native == endian::little) {
    return value;
  } else {
    return byte_swap (value);
  }
}

/*
Encoding        Representation (hexadecimal)
UTF-8[a]        EF BB BF
UTF-16 (BE)     FE FF
UTF-16 (LE)     FF FE
UTF-32 (BE)     00 00 FE FF
UTF-32 (LE)     FF FE 00 00
*/
#if ICUBABY_HAVE_CONCEPTS
template <unicode_char_type ToEncoding>
#else
template <typename ToEncoding>
#endif  // ICUBABY_HAVE_CONCEPTS
class runtime_transcoder {
public:
  using input_type = std::byte;
  using output_type = ToEncoding;

  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator operator() (input_type value, OutputIterator dest) noexcept {
    switch (state_) {
    case states::start:
      *(buffer_pos_++) = value;
      if (value == std::byte{0xEF}) {
        state_ = states::utf8_bom_byte2;
      } else if (value == std::byte{0xFE}) {
        state_ = states::utf16_be_bom_byte2;
      } else if (value == std::byte{0xFF}) {
        state_ = states::utf32_or_16_le_bom_byte2;
      } else if (value == std::byte{0x00}) {
        state_ = states::utf32_or_16_be_bom_byte2;
      } else {
      }
      break;
    case states::utf8_bom_byte2:
      *(buffer_pos_++) = value;
      if (value == std::byte{0xBB}) {
        state_ = states::utf8_bom_byte3;
      } else {
        // Default input encoding. Emit buffer.
      }
      break;
    case states::utf8_bom_byte3:
      if (value == std::byte{0xBF}) {
        transcoder_.template emplace<t8_type> ();
        state_ = states::run8;
        buffer_pos_ = std::begin (buffer_);
      } else {
        // Default input encoding. Emit buffer.
        *(buffer_pos_++) = value;
      }
      break;
    case states::utf16_be_bom_byte2:
      if (value == std::byte{0xFF}) {
        transcoder_.template emplace<t16_type> ();
        state_ = states::run_16_bit_big_endian_first_byte;
        buffer_pos_ = std::begin (buffer_);
      } else {
        // Default input encoding. Emit buffer.
        *(buffer_pos_++) = value;
      }
      break;

    case states::utf32_or_16_le_bom_byte2:
      if (value == std::byte{0xFE}) {
        transcoder_.template emplace<t16_type> ();
        state_ = states::run_16_bit_little_endian_first_byte;
        buffer_pos_ = std::begin (buffer_);
      } else if (value == std::byte{0xFF}) {
        // FIXME: could be UTF32LE.
      }
      break;
    case states::utf32_or_16_be_bom_byte2:

    case states::byte3:
    case states::byte4: break;

    case states::run8:
      assert (std::holds_alternative<t8_type> (transcoder_));
      dest = std::get<t8_type> (transcoder_) (static_cast<icubaby::char8> (value), dest);
      break;

    case states::run_16_bit_big_endian_first_byte:
      buffer_[0] = value;
      state_ = states::run_16_bit_big_endian_second_byte;
      break;
    case states::run_16_bit_big_endian_second_byte:
      assert (std::holds_alternative<t16_type> (transcoder_));
      state_ = states::run_16_bit_big_endian_first_byte;
      dest = std::get<t16_type> (transcoder_) (static_cast<char16_t> ((buffer_[0] << 8) | value), dest);
      break;

    case states::run_16_bit_little_endian_first_byte:
      buffer_[0] = value;
      state_ = states::run_16_bit_little_endian_second_byte;
      break;
    case states::run_16_bit_little_endian_second_byte:
      assert (std::holds_alternative<t16_type> (transcoder_));
      state_ = states::run_16_bit_little_endian_first_byte;
      dest = std::get<t16_type> (transcoder_) (static_cast<char16_t> ((value << 8) | buffer_[0]), dest);
      break;
    }
    return dest;
  }

  template <typename OutputIterator>
  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator end_cp (OutputIterator dest) noexcept {
    return std::visit (
        [&dest] (auto& arg) {
          if constexpr (std::is_same_v<std::decay_t<decltype (arg)>, std::monostate>) {
            return dest;
          } else {
            return arg.end_cp (dest);
          }
        },
        transcoder_);
  }

private:
  enum class states {
    start,
    utf8_bom_byte2,
    utf8_bom_byte3,
    utf16_be_bom_byte2,
    utf32_or_16_le_bom_byte2,
    utf32_or_16_be_bom_byte2,
    byte3,
    byte4,
    run8,

    run_16_bit_big_endian_first_byte,
    run_16_bit_big_endian_second_byte,

    run_16_bit_little_endian_first_byte,
    run_16_bit_little_endian_second_byte,
  };
  states state_ = states::start;
  using buffer_type = std::array<std::byte, 4>;
  buffer_type buffer_;
  buffer_type::iterator buffer_pos_ = std::begin (buffer_);

  using t8_type = transcoder<icubaby::char8, ToEncoding>;
  using t16_type = transcoder<char16_t, ToEncoding>;
  using t32_type = transcoder<char32_t, ToEncoding>;
  std::variant<std::monostate, t8_type, t16_type, t32_type> transcoder_;
};

}  // end namespace icubaby

#endif  // ICUBABY_DY_HPP
