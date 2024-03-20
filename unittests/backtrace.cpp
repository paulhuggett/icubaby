#if __linux__

#include <execinfo.h>
#include <signal.h>

#include <array>
#include <cstdio>
#include <cstdlib>

class sigsegv_backtrace {
public:
  sigsegv_backtrace () {
    std::fprintf (stderr, "Installing signal handler\n");
    signal (SIGSEGV, handler);
  }

private:
  [[noreturn]] static void handler (int sig) {
    static std::array<void *, 20> arr;

    // get void*'s for all entries on the stack
    auto const size = backtrace (arr.data (), arr.size ());

    // print out all the frames to stderr
    static char const message [] = "Error: SIGSEGV\n";
    write (STDERR_FILENO, message, sizeof (message) - 1);
    backtrace_symbols_fd (arr.data (), size, STDERR_FILENO);
    std::_Exit (EXIT_FAILURE);
  }
};

extern sigsegv_backtrace gc;
sigsegv_backtrace gc;

#endif  // __linux__
