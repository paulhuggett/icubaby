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

set (icubaby_project_root "${CMAKE_CURRENT_SOURCE_DIR}")
list (APPEND CMAKE_MODULE_PATH "${icubaby_project_root}/cmake")

if (CXX17)
    set (standard 17)
else ()
    set (standard 20)
endif ()

include (setup_target)
include (setup_gtest)

setup_gtest ()

add_library (icubaby INTERFACE "${icubaby_project_root}/include/icubaby/icubaby.hpp")
target_include_directories (icubaby INTERFACE "${icubaby_project_root}/include")

add_subdirectory (tests)
add_subdirectory (unittests)

