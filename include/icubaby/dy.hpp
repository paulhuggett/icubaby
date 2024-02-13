#ifndef ICUBABY_DY_HPP
#define ICUBABY_DY_HPP

#include <algorithm>
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

enum class encoding {
  unknown,
  utf8,
  utf16be,
  utf16le,
  utf32be,
  utf32le,
};

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

  template <std::output_iterator<output_type> OutputIterator>
  OutputIterator operator() (input_type value, OutputIterator dest) noexcept {
    switch (state_) {
    case states::start:
      buffer_pos_ = std::begin (buffer_);
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
        state_ = states::run8_start;
      }
      break;
    case states::utf8_bom_byte2:
      *(buffer_pos_++) = value;
      if (value == std::byte{0xBB}) {
        state_ = states::utf8_bom_byte3;
      } else {
        // Default input encoding. Emit buffer.
        state_ = states::run8_start;
      }
      break;
    case states::utf8_bom_byte3:
      if (value == std::byte{0xBF}) {
        // A complete UTF-8 BOM. Drop the buffer contents and start decoding as UTF-8.
        (void)transcoder_.template emplace<t8_type> ();
        encoding_ = encoding::utf8;
        state_ = states::run8;
        buffer_pos_ = std::begin (buffer_);
      } else {
        // Default input encoding. Emit buffer.
        *(buffer_pos_++) = value;
        state_ = states::run8_start;
      }
      break;
    case states::utf16_be_bom_byte2:
      if (value == std::byte{0xFF}) {
        (void)transcoder_.template emplace<t16_type> ();
        encoding_ = encoding::utf16be;
        state_ = states::run_16_bit_big_endian_first_byte;
        buffer_pos_ = std::begin (buffer_);
      } else {
        // Default input encoding. Emit buffer.
        *(buffer_pos_++) = value;
        state_ = states::run8_start;
      }
      break;

    case states::utf32_or_16_le_bom_byte2:
      *(buffer_pos_++) = value;
      if (value == std::byte{0xFE}) {
        state_ = states::utf32_or_16_le_bom_byte3;
      } else {
        state_ = states::run8_start;
      }
      break;

    case states::utf32_or_16_be_bom_byte2:
      *(buffer_pos_++) = value;
      if (value == std::byte{0x00}) {
        state_ = states::utf32_be_bom_byte3;
      } else {
        // Default input encoding. Emit buffer.
        state_ = states::run8_start;
      }
      break;

    case states::utf32_be_bom_byte3:
      *(buffer_pos_++) = value;
      if (value == std::byte{0xFE}) {
        state_ = states::utf32_be_bom_byte4;
      } else {
        // Default input encoding. Emit buffer.
        state_ = states::run8_start;
      }
      break;

    case states::utf32_or_16_le_bom_byte3:
      if (value == std::byte{0x00}) {
        *(buffer_pos_++) = value;
        state_ = states::utf32_le_bom_byte4;
      } else {
        (void)transcoder_.template emplace<t16_type> ();
        encoding_ = encoding::utf16le;
        buffer_pos_ = std::begin (buffer_);
        *(buffer_pos_++) = value;
        state_ = states::run_16_bit_little_endian_second_byte;
      }
      break;

    case states::utf32_le_bom_byte4:
      *(buffer_pos_++) = value;
      if (value == std::byte{0x00}) {
        (void)transcoder_.template emplace<t32_type> ();
        encoding_ = encoding::utf32le;
        state_ = states::run_32le_byte1;
        buffer_pos_ = std::begin (buffer_);
      } else {
        // Default input encoding. Emit buffer.
        state_ = states::run8_start;
      }
      break;

    case states::utf32_be_bom_byte4:
      *(buffer_pos_++) = value;
      if (value == std::byte{0xFF}) {
        (void)transcoder_.template emplace<t32_type> ();
        encoding_ = encoding::utf32be;
        state_ = states::run_32be_byte1;
        buffer_pos_ = std::begin (buffer_);
      } else {
        // Default input encoding. Emit buffer.
        state_ = states::run8_start;
      }
      break;

    case states::run8_start:
      dest = this->run8_start (dest);
      [[fallthrough]];
    case states::run8:
      assert (std::holds_alternative<t8_type> (transcoder_));
      dest = std::get<t8_type> (transcoder_) (static_cast<char8> (value), dest);
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

    case states::run_32be_byte1:
      buffer_[0] = value;
      state_ = states::run_32be_byte2;
      break;
    case states::run_32be_byte2:
      buffer_[1] = value;
      state_ = states::run_32be_byte3;
      break;
    case states::run_32be_byte3:
      buffer_[2] = value;
      state_ = states::run_32be_byte4;
      break;
    case states::run_32be_byte4:
      assert (std::holds_alternative<t32_type> (transcoder_));
      state_ = states::run_32be_byte1;
      dest = std::get<t32_type> (transcoder_) (
          static_cast<char32_t> ((static_cast<std::uint_least32_t> (buffer_[0]) << 24) |
                                 (static_cast<std::uint_least32_t> (buffer_[1]) << 16) |
                                 (static_cast<std::uint_least32_t> (buffer_[2]) << 8) |
                                 static_cast<std::uint_least32_t> (value)),
          dest);
      break;

    case states::run_32le_byte1:
      buffer_[0] = value;
      state_ = states::run_32le_byte2;
      break;
    case states::run_32le_byte2:
      buffer_[1] = value;
      state_ = states::run_32le_byte3;
      break;
    case states::run_32le_byte3:
      buffer_[2] = value;
      state_ = states::run_32le_byte4;
      break;
    case states::run_32le_byte4:
      assert (std::holds_alternative<t32_type> (transcoder_));
      state_ = states::run_32le_byte1;
      dest = std::get<t32_type> (transcoder_) (
          static_cast<char32_t> ((static_cast<std::uint_least32_t> (value << 24)) |
                                 (static_cast<std::uint_least32_t> (buffer_[2]) << 16) |
                                 (static_cast<std::uint_least32_t> (buffer_[1]) << 8) |
                                 (static_cast<std::uint_least32_t> (buffer_[0]) << 0)),
          dest);
      break;
    }
    return dest;
  }

  template <std::output_iterator<output_type> OutputIterator> OutputIterator end_cp (OutputIterator dest) noexcept {
    return std::visit (
        [this, &dest] (auto& arg) {
          if constexpr (std::is_same_v<std::decay_t<decltype (arg)>, std::monostate>) {
            return run8_start (dest);
          } else {
            return arg.end_cp (dest);
          }
        },
        transcoder_);
  }

  template <std::output_iterator<output_type> OutputIterator>
  constexpr iterator<runtime_transcoder, OutputIterator> end_cp (iterator<runtime_transcoder, OutputIterator> dest) {
    auto tcdr = dest.transcoder ();
    assert (tcdr == this);
    return {tcdr, tcdr->end_cp (dest.base ())};
  }

  [[nodiscard]] bool well_formed () const {
    return std::visit (
        [] (auto const& arg) {
          if constexpr (std::is_same_v<std::decay_t<decltype (arg)>, std::monostate>) {
            return true;
          } else {
            return arg.well_formed ();
          }
        },
        transcoder_);
  }

  [[nodiscard]] bool partial () const {
    return std::visit (
        [this] (auto const& arg) {
          if constexpr (std::is_same_v<std::decay_t<decltype (arg)>, std::monostate>) {
            return this->state_ != states::start;
          } else {
            return arg.partial ();
          }
        },
        transcoder_);
  }

  [[nodiscard]] encoding selected_encoding () const noexcept {
    return encoding_;
  }

private:
  enum class states {
    start,
    utf8_bom_byte2,
    utf8_bom_byte3,
    utf16_be_bom_byte2,
    utf32_or_16_le_bom_byte2,
    utf32_or_16_be_bom_byte2,
    utf32_or_16_le_bom_byte3,

    utf32_be_bom_byte3,
    utf32_be_bom_byte4,

    utf32_le_bom_byte4,

    run8_start,
    run8,

    run_16_bit_big_endian_first_byte,
    run_16_bit_big_endian_second_byte,

    run_16_bit_little_endian_first_byte,
    run_16_bit_little_endian_second_byte,

    run_32be_byte1,
    run_32be_byte2,
    run_32be_byte3,
    run_32be_byte4,

    run_32le_byte1,
    run_32le_byte2,
    run_32le_byte3,
    run_32le_byte4,

  };
  states state_ = states::start;
  using buffer_type = std::array<std::byte, 4>;
  buffer_type buffer_{};
  buffer_type::iterator buffer_pos_ = std::begin (buffer_);

  using t8_type = transcoder<icubaby::char8, ToEncoding>;
  using t16_type = transcoder<char16_t, ToEncoding>;
  using t32_type = transcoder<char32_t, ToEncoding>;
  std::variant<std::monostate, t8_type, t16_type, t32_type> transcoder_;
  encoding encoding_ = encoding::unknown;

  template <typename OutputIterator>
  //  ICUBABY_REQUIRES ((std::output_iterator<OutputIterator, output_type>))
  OutputIterator run8_start (OutputIterator dest) noexcept {
    assert (std::holds_alternative<std::monostate> (transcoder_));
    auto & trans = transcoder_.template emplace<t8_type> ();
    encoding_ = encoding::utf8;
    auto const first = std::begin (buffer_);
    std::for_each (first, buffer_pos_, [&trans, &dest] (std::byte value) { dest = trans (static_cast<char8> (value), dest); });
    buffer_pos_ = first;
    state_ = states::run8;
    return dest;
  }
};

}  // end namespace icubaby

#endif  // ICUBABY_DY_HPP
