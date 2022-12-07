# MIT License
#
# Copyright (c) 2022 Paul Bowen-Huggett
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
  harness.cpp
  test_u8_32.cpp
  test_u16.cpp
  test_u32_8.cpp
)
setup_target (icubaby-unittests)

target_link_libraries (icubaby-unittests PRIVATE icubaby gtest gmock)

set (clang_options
  -Wno-global-constructors
  -Wno-used-but-marked-unused
)
set (gcc_options
)
set (msvc_options
)
target_compile_options (icubaby-unittests PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${clang_options}>
  $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
  $<$<CXX_COMPILER_ID:MSVC>:${msvc_options}>
)
