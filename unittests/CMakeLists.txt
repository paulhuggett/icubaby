# MIT License
#
# Copyright (c) 2022-2024 Paul Bowen-Huggett
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

add_executable (icubaby-unittests
  encoded_char.hpp
  test_rtrans.cpp
  test_u16.cpp
  test_u32.cpp
  test_u8.cpp
  test_utility.cpp
  typed_test.hpp
)
setup_target (icubaby-unittests PEDANTIC $<NOT:$<BOOL:${ICUBABY_FUZZTEST}>>)
target_link_libraries (icubaby-unittests PUBLIC icubaby)

target_compile_options (
  icubaby-unittests
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
      -Wno-global-constructors
      -Wno-used-but-marked-unused>
    $<$<CXX_COMPILER_ID:GNU>:>
    # -wd4702 : unreachable code.
    $<$<CXX_COMPILER_ID:MSVC>:
      -wd4702>
)
add_test(NAME icubaby-unittests COMMAND icubaby-unittests)

if (ICUBABY_FUZZTEST)
  target_link_libraries (icubaby-unittests PUBLIC gtest gmock)
  link_fuzztest (icubaby-unittests)
  gtest_discover_tests (icubaby-unittests)
else ()
  target_sources (icubaby-unittests PUBLIC harness.cpp)
  target_link_libraries (icubaby-unittests PUBLIC gmock_main)
endif (ICUBABY_FUZZTEST)
