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

cmake_minimum_required(VERSION 3.22)

project (icubaby CXX)

option (WERROR "Compiler warnings are errors")
option (CXX17 "Use C++17 (rather than the default C++20")
option (LIBCXX "Use libc++ rather than libstdc++ (clang only)")
option (COVERAGE "Generate LLVM Source-based Coverage")

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (CXX17)
    set (standard 17)
else ()
    set (standard 20)
endif ()

include (setup_target)

# Tell gtest to link against the "Multi-threaded Debug DLL runtime
# library" on Windows.
set (gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll")
add_subdirectory (googletest)
foreach (target gtest gmock gmock_main gtest_main)
  set_target_properties (${target} PROPERTIES
    CXX_STANDARD ${standard}
    CXX_STANDARD_REQUIRED Yes
    CXX_EXTENSIONS No
  )
  set (gclang_options -Wno-implicit-int-float-conversion)
  if (LIBCXX)
    list (APPEND gclang_options -stdlib=libc++)
  endif ()
  set (gintelllvm_options ${gclang_options} -Wno-tautological-constant-compare)

  target_compile_definitions (${target} PUBLIC GTEST_REMOVE_LEGACY_TEST_CASEAPI_=1)
  target_compile_options (${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${gclang_options}>
    $<$<CXX_COMPILER_ID:IntelLLVM>:${gintelllvm_options}>
  )
  target_link_options (${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${gclang_options}>
    $<$<CXX_COMPILER_ID:IntelLLVM>:${gintelllvm_options}>
  )
endforeach()

add_subdirectory (lib)
add_subdirectory (tests)
add_subdirectory (unittests)

