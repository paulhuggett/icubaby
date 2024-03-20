#include "icubaby/icubaby.hpp"

#if (__linux__ || __APPLE__) && ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include <array>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <ranges>
#include <type_traits>
#include <utility>

namespace {

template <std::unsigned_integral Unsigned>
constexpr std::size_t base10digits (Unsigned value = std::numeric_limits<Unsigned>::max ()) noexcept {
  return value < 10U ? std::size_t{1} : std::size_t{1} + base10digits<Unsigned> (value / Unsigned{10});
}

template <std::unsigned_integral Unsigned> class unsigned_to_characters {
public:
  using base10storage = std::array<char, base10digits<Unsigned> ()>;

  /// Converts an unsigned numeric value to an array of characters.
  ///
  /// \param v  The unsigned number value to be converted.
  /// \result  A range denoting the range of valid characters in the internal buffer.
  std::ranges::subrange<typename base10storage::iterator> operator() (Unsigned v) noexcept {
    auto const end = buffer_.end ();
    auto pos = std::prev (end);
    if (v == 0U) {
      *pos = '0';
      return {pos, end};
    }

    for (; v > 0; v /= 10U) {
      *(pos--) = (v % 10U) + '0';
    }
    return {std::next (pos), end};
  }

private:
  base10storage buffer_;
};

template <std::size_t Size> constexpr std::size_t strlength (char const (&)[Size]) noexcept {
  return Size - 1U;
}

ssize_t write_char (int fd, char c) {
  return ::write (fd, &c, sizeof (c));
}

void say_signal_number (int fd, int sig) {
  static char const message[] = "Signal: ";
  write (fd, message, strlength (message));

  if (sig < 0) {
    write_char (fd, '-');
    sig = -sig;
  }
  static unsigned_to_characters<unsigned> str;
  auto const range = str (static_cast<unsigned> (sig));
  write (fd, std::to_address (std::begin (range)), range.size ());
  write_char (fd, '\n');
}

}  // end anonymous namespace

extern "C" {

[[noreturn]] static void handler (int sig) {
  say_signal_number (STDERR_FILENO, sig);

  static std::array<void *, 20> arr;
  // get void*'s for all entries on the stack
  auto const size = backtrace (arr.data (), arr.size ());
  backtrace_symbols_fd (arr.data (), size, STDERR_FILENO);
  std::_Exit (EXIT_FAILURE);
}

}  // extern "C"

class sigsegv_backtrace {
public:
  sigsegv_backtrace () {
    static char const message[] = "Installing SIGSEGV handler\n";
    write (STDERR_FILENO, message, strlength (message));
    signal (SIGSEGV, handler);
  }
};

extern sigsegv_backtrace gc;
sigsegv_backtrace gc;

#endif  // (__linux__ || __APPLE__) && ICUBABY_HAVE_RANGES && ICUBABY_HAVE_CONCEPTS
