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

// An alias template for a two-dimensional std::array
template <typename T, std::size_t Row, std::size_t Col> using array2d = std::array<std::array<T, Col>, Row>;

inline array2d<std::byte, 5, 4> const boms{{
    {std::byte{0xFE}, std::byte{0xFF}},                                    // UTF-16 BE
    {std::byte{0xFF}, std::byte{0xFE}},                                    // UTF-16 LE
    {std::byte{0x00}, std::byte{0x00}, std::byte{0xFE}, std::byte{0xFF}},  // UTF-32 BE
    {std::byte{0xFF}, std::byte{0xFE}, std::byte{0x00}, std::byte{0x00}},  // UTF-32 LE
    {std::byte{0xEF}, std::byte{0xBB}, std::byte{0xBF}},                   // UTF-8
}};

#if ICUBABY_HAVE_CONCEPTS
#define ICUBABY_CONCEPT_OUTPUT_ITERATOR(x) std::output_iterator<x>
#else
#define ICUBABY_CONCEPT_OUTPUT_ITERATOR(x) typename
#endif

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

  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
  OutputIterator operator() (input_type value, OutputIterator dest) noexcept {
    switch (state_) {
    case states::start: dest = this->start_state (value, dest); break;
    case states::utf8_bom_byte2:
      buffer_[byte_no (state_)] = value;
      // Start decoding as UTF-8. If we have a complete UTF-8 BOM drop it, otherwise copy the buffer to output.
      dest = this->run8_start (value != boms[boms_index_from_state (state_)][byte_no (state_)], dest);
      break;

    case states::utf16_be_bom_byte1:
      buffer_[byte_no (state_)] = value;
      // We either have a complete UTF-16 BE BOM in which case we start transcoding or we default to UTF-8 emitting the
      // bytes consumed so far.
      dest = (value == boms[boms_index_from_state (state_)][byte_no (state_)]) ? this->run16_start (dest)
                                                                               : this->run8_start (true, dest);
      break;

    case states::utf32_or_16_le_bom_byte2:
      if (value != std::byte{0x00}) {
        dest = this->run16_start (dest);
        state_ = states::run_16le_byte1;
        buffer_[0] = value;
        break;
      }
      [[fallthrough]];

    case states::utf8_bom_byte1:
    case states::utf32_or_16_le_bom_byte1:
    case states::utf32_or_16_be_bom_byte1:
    case states::utf32_be_bom_byte2:
      buffer_[byte_no (state_)] = value;
      if (value == boms[boms_index_from_state (state_)][byte_no (state_)]) {
        state_ = next_byte (state_);
      } else {
        // Default input encoding. Emit buffer.
        dest = this->run8_start (true, dest);
      }
      break;

    case states::utf32_le_bom_byte3:
    case states::utf32_be_bom_byte3:
      buffer_[3] = value;
      if (value == (is_little_endian (state_) ? std::byte{0x00} : std::byte{0xFF})) {
        (void)transcoder_.template emplace<t32_type> ();
        encoding_ = is_little_endian (state_) ? encoding::utf32le : encoding::utf32be;
        state_ = set_run_mode (set_byte (state_, 0));
      } else {
        // Default input encoding. Emit buffer.
        dest = this->run8_start (true, dest);
      }
      break;

    case states::run_8:
      assert (std::holds_alternative<t8_type> (transcoder_));
      dest = std::get<t8_type> (transcoder_) (static_cast<char8> (value), dest);
      break;

    case states::run_16be_byte1:
    case states::run_16le_byte1:
      assert (std::holds_alternative<t16_type> (transcoder_));
      dest =
          std::get<t16_type> (transcoder_) (state_ == states::run_16be_byte1 ? char16_from_big_endian_buffer (value)
                                                                             : char16_from_little_endian_buffer (value),
                                            dest);
      state_ = set_byte (state_, 0);
      break;

    case states::run_16be_byte0:
    case states::run_16le_byte0:
    case states::run_32be_byte0:
    case states::run_32be_byte1:
    case states::run_32be_byte2:
    case states::run_32le_byte0:
    case states::run_32le_byte1:
    case states::run_32le_byte2:
      buffer_[byte_no (state_)] = value;
      state_ = next_byte (state_);
      break;
    case states::run_32be_byte3:
    case states::run_32le_byte3:
      assert (std::holds_alternative<t32_type> (transcoder_));
      dest =
          std::get<t32_type> (transcoder_) (state_ == states::run_32be_byte3 ? char32_from_big_endian_buffer (value)
                                                                             : char32_from_little_endian_buffer (value),
                                            dest);
      state_ = set_byte (state_, 0);
      break;
    }
    return dest;
  }

  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
  OutputIterator end_cp (OutputIterator dest) noexcept {
    return std::visit (
        [this, &dest] (auto& arg) {
          if constexpr (std::is_same_v<std::decay_t<decltype (arg)>, std::monostate>) {
            return run8_start (true, dest);
          } else {
            return arg.end_cp (dest);
          }
        },
        transcoder_);
  }

  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
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
  // +---+---+---+---+---+---+
  // |   c   | e | r |   b   |
  // +---+---+---+---+---+---+
  // c : Encoding unknown(0), UTF-8(1), UTF-16(2), UTF-32(3).
  // e : Big(0)/Little(1) endian
  // r : Run/BOM.
  // b : Byte Number (0-3)
  static constexpr std::byte byte_no (std::uint_least8_t index) {
    assert (index < 4U);
    return static_cast<std::byte> (index);
  }

  static constexpr auto encoding_shift = 4U;
  static constexpr auto endian_shift = 3U;
  static constexpr auto run_shift = 2U;

  static constexpr auto encoding_mask = std::byte{0b11 << encoding_shift};  // one of unknown or UTF-8/16/32.
  static constexpr auto endian_mask = std::byte{1U << endian_shift};        // one of big_endian or little_endian.
  static constexpr auto run_mask = std::byte{1U << run_shift};              // run or bom mode.
  static constexpr auto byte_no_mask = std::byte{0b11};                     // values from 0-3.

  static constexpr auto encoding_utf16 = std::byte{0b00 << encoding_shift};
  static constexpr auto encoding_utf32 = std::byte{0b01 << encoding_shift};
  static constexpr auto encoding_utf8 = std::byte{0b10 << encoding_shift};
  static constexpr auto encoding_unknown = std::byte{0b11 << encoding_shift};

  static constexpr auto bom_mode = std::byte{0};
  static constexpr auto run_mode = run_mask;

  static constexpr auto big_endian = std::byte{0};
  static constexpr auto little_endian = endian_mask;

  enum class states : std::uint_least8_t {
    start = static_cast<std::uint_least8_t> (encoding_unknown | bom_mode | byte_no (0)),

    utf8_bom_byte1 = static_cast<std::uint_least8_t> (encoding_utf8 | big_endian | bom_mode | byte_no (1U)),
    utf8_bom_byte2 = static_cast<std::uint_least8_t> (encoding_utf8 | big_endian | bom_mode | byte_no (2U)),

    utf16_be_bom_byte1 = static_cast<std::uint_least8_t> (encoding_utf16 | big_endian | bom_mode | byte_no (1U)),
    utf32_be_bom_byte2 = static_cast<std::uint_least8_t> (encoding_utf32 | big_endian | bom_mode | byte_no (2U)),
    utf32_be_bom_byte3 = static_cast<std::uint_least8_t> (encoding_utf32 | big_endian | bom_mode | byte_no (3U)),

    utf32_or_16_be_bom_byte1 = static_cast<std::uint_least8_t> (encoding_utf32 | big_endian | bom_mode | byte_no (1U)),

    utf32_or_16_le_bom_byte1 =
        static_cast<std::uint_least8_t> (encoding_utf32 | little_endian | bom_mode | byte_no (1U)),
    utf32_or_16_le_bom_byte2 =
        static_cast<std::uint_least8_t> (encoding_utf32 | little_endian | bom_mode | byte_no (2U)),
    utf32_le_bom_byte3 = static_cast<std::uint_least8_t> (encoding_utf32 | little_endian | bom_mode | byte_no (3U)),

    run_8 = static_cast<std::uint_least8_t> (encoding_utf8 | big_endian | run_mode | byte_no (0U)),

    run_16be_byte0 = static_cast<std::uint_least8_t> (encoding_utf16 | big_endian | run_mode | byte_no (0U)),
    run_16be_byte1 = static_cast<std::uint_least8_t> (encoding_utf16 | big_endian | run_mode | byte_no (1U)),

    run_16le_byte0 = static_cast<std::uint_least8_t> (encoding_utf16 | little_endian | run_mode | byte_no (0U)),
    run_16le_byte1 = static_cast<std::uint_least8_t> (encoding_utf16 | little_endian | run_mode | byte_no (1U)),

    run_32be_byte0 = static_cast<std::uint_least8_t> (encoding_utf32 | big_endian | run_mode | byte_no (0U)),
    run_32be_byte1 = static_cast<std::uint_least8_t> (encoding_utf32 | big_endian | run_mode | byte_no (1U)),
    run_32be_byte2 = static_cast<std::uint_least8_t> (encoding_utf32 | big_endian | run_mode | byte_no (2U)),
    run_32be_byte3 = static_cast<std::uint_least8_t> (encoding_utf32 | big_endian | run_mode | byte_no (3U)),

    run_32le_byte0 = static_cast<std::uint_least8_t> (encoding_utf32 | little_endian | run_mode | byte_no (0U)),
    run_32le_byte1 = static_cast<std::uint_least8_t> (encoding_utf32 | little_endian | run_mode | byte_no (1U)),
    run_32le_byte2 = static_cast<std::uint_least8_t> (encoding_utf32 | little_endian | run_mode | byte_no (2U)),
    run_32le_byte3 = static_cast<std::uint_least8_t> (encoding_utf32 | little_endian | run_mode | byte_no (3U)),
  };

  static constexpr bool is_run_mode (states state) {
    return (static_cast<std::underlying_type_t<states>> (state) & run_mask) != run_mode;
  }
  static constexpr bool is_little_endian (states state) {
    return (static_cast<std::byte> (state) & endian_mask) == little_endian;
  }
  static constexpr std::uint_least8_t byte_no (states state) {
    return static_cast<std::uint_least8_t> (static_cast<std::byte> (state) & byte_no_mask);
  }
  static constexpr states set_byte (states state, std::uint_least8_t byte_number) {
    assert (byte_number < 4);
    return static_cast<states> ((static_cast<std::byte> (state) & ~byte_no_mask) |
                                static_cast<std::byte> (byte_number));
  }
  static constexpr states next_byte (states state) { return set_byte (state, byte_no (state) + 1); }

  static constexpr states set_run_mode (states const state) noexcept {
    assert ((static_cast<std::byte> (state) & run_mask) == bom_mode);
    return static_cast<states> ((static_cast<std::byte> (state) & ~run_mask) | run_mode);
  }

  static constexpr std::size_t boms_index_from_state (states const state) noexcept {
    auto const state_byte = static_cast<std::byte> (state);
    assert (((state_byte & (encoding_mask | endian_mask)) >> endian_shift) == (state_byte >> endian_shift));
    return static_cast<std::size_t> (state_byte >> endian_shift);
  }

  using t8_type = transcoder<icubaby::char8, ToEncoding>;
  using t16_type = transcoder<char16_t, ToEncoding>;
  using t32_type = transcoder<char32_t, ToEncoding>;

  states state_ = states::start;
  std::array<std::byte, 4> buffer_{};
  encoding encoding_ = encoding::unknown;
  std::variant<std::monostate, t8_type, t16_type, t32_type> transcoder_;

  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
  OutputIterator start_state (input_type value, OutputIterator dest) noexcept {
    buffer_[0] = value;
    if (value == std::byte{0xEF}) {
      state_ = states::utf8_bom_byte1;
    } else if (value == std::byte{0xFE}) {
      state_ = states::utf16_be_bom_byte1;
    } else if (value == std::byte{0xFF}) {
      state_ = states::utf32_or_16_le_bom_byte1;
    } else if (value == std::byte{0x00}) {
      state_ = states::utf32_or_16_be_bom_byte1;
    } else {
      dest = this->run8_start (true, dest);
    }
    return dest;
  }

  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
  OutputIterator run8_start (bool copy_buffer, OutputIterator dest) noexcept {
    assert (std::holds_alternative<std::monostate> (transcoder_));
    auto & trans = transcoder_.template emplace<t8_type> ();
    encoding_ = encoding::utf8;
    if (copy_buffer) {
      auto const first = std::begin (buffer_);
      (void)std::for_each (first, first + byte_no (state_) + 1,
                           [&trans, &dest] (std::byte value) { dest = trans (static_cast<char8> (value), dest); });
    }
    state_ = states::run_8;
    return dest;
  }

  template <ICUBABY_CONCEPT_OUTPUT_ITERATOR (output_type) OutputIterator>
  OutputIterator run16_start (OutputIterator dest) {
    (void)transcoder_.template emplace<t16_type> ();
    encoding_ = is_little_endian (state_) ? encoding::utf16le : encoding::utf16be;
    state_ = is_little_endian (state_) ? states::run_16le_byte0 : states::run_16be_byte0;
    return dest;
  }

  constexpr char16_t char16_from_big_endian_buffer (input_type value) const noexcept {
    return static_cast<char16_t> ((static_cast<std::uint_least16_t> (buffer_[0]) << 8) |
                                  static_cast<std::uint_least16_t> (value));
  }
  constexpr char16_t char16_from_little_endian_buffer (input_type value) const noexcept {
    return static_cast<char16_t> ((static_cast<std::uint_least16_t> (value) << 8) |
                                  static_cast<std::uint_least16_t> (buffer_[0]));
  }
  constexpr char32_t char32_from_big_endian_buffer (input_type value) const noexcept {
    return static_cast<char32_t> (
        (static_cast<std::uint_least32_t> (buffer_[0]) << 24) | (static_cast<std::uint_least32_t> (buffer_[1]) << 16) |
        (static_cast<std::uint_least32_t> (buffer_[2]) << 8) | static_cast<std::uint_least32_t> (value));
  }
  constexpr char32_t char32_from_little_endian_buffer (input_type value) const noexcept {
    return static_cast<char32_t> (
        (static_cast<std::uint_least32_t> (value << 24)) | (static_cast<std::uint_least32_t> (buffer_[2]) << 16) |
        (static_cast<std::uint_least32_t> (buffer_[1]) << 8) | (static_cast<std::uint_least32_t> (buffer_[0])));
  }
};

}  // end namespace icubaby

#endif  // ICUBABY_DY_HPP
