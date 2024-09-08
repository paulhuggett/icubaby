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

cmake_minimum_required (VERSION 3.22)

project (icubaby
  VERSION "1.2.3"
  DESCRIPTION "Little Library to Immediately Convert Unicode"
  LANGUAGES C CXX
)
option (ICUBABY_COVERAGE "Generate LLVM Source-based Coverage")
option (ICUBABY_CXX17 "Use C++17 (rather than the default C++20)")
option (ICUBABY_EXAMPLES "Include example code in the generated build")
option (ICUBABY_FUZZTEST "Enable FuzzTest")
option (ICUBABY_LIBCXX "Use libc++ rather than libstdc++ (clang only)")
option (ICUBABY_SANITIZE "Enable undefined behavior- and address-sanitizers where supported")
option (ICUBABY_STANDALONE "A standalone build includes test executables")
option (ICUBABY_WERROR "Compiler warnings are errors")

set (icubaby_project_root "${CMAKE_CURRENT_SOURCE_DIR}")
list (APPEND CMAKE_MODULE_PATH "${icubaby_project_root}/cmake")

include (setup_target)
include (setup_gtest)

if (ICUBABY_FUZZTEST)
  set (FUZZTEST_FUZZING_MODE On)
  include (FetchContent)
  set (FUZZTEST_REPO_BRANCH "1635d42" CACHE STRING "FuzzTest repository branch.")
  message ("Building fuzztest at tag " ${FUZZTEST_REPO_BRANCH})
  FetchContent_Declare (
    fuzztest
    GIT_REPOSITORY https://github.com/google/fuzztest.git
    GIT_TAG ${FUZZTEST_REPO_BRANCH}
  )
  FetchContent_MakeAvailable (fuzztest)
  include (GoogleTest)
  fuzztest_setup_fuzzing_flags ()
else ()
  setup_gtest ()
endif (ICUBABY_FUZZTEST)
enable_testing ()

# A custom target from which the install will hang
add_custom_target (
  install-icubaby
  COMMAND "${CMAKE_COMMAND}" -D CMAKE_INSTALL_COMPONENT=icubaby -P
          "${CMAKE_BINARY_DIR}/cmake_install.cmake"
  USES_TERMINAL
)

# icubaby target

set (icubaby_include_dir "${icubaby_project_root}/include")
set (icubaby_headers "${icubaby_include_dir}/icubaby/icubaby.hpp")
add_library (icubaby INTERFACE "${icubaby_headers}")
target_include_directories (icubaby SYSTEM INTERFACE
  "$<BUILD_INTERFACE:${icubaby_include_dir}>"
  "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties (icubaby PROPERTIES PUBLIC_HEADER "${icubaby_headers}")
include (GNUInstallDirs)
install (
  TARGETS icubaby
  EXPORT icubaby
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/icubaby"
    COMPONENT icubaby
  LIBRARY COMPONENT icubaby
  ARCHIVE COMPONENT icubaby
  INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
add_dependencies (install-icubaby icubaby)

# examples

if (ICUBABY_EXAMPLES)
  add_subdirectory (examples)
endif (ICUBABY_EXAMPLES)

# tests

if (ICUBABY_STANDALONE)
    add_subdirectory (tests)
endif (ICUBABY_STANDALONE)

if (TARGET gtest)
  add_subdirectory (unittests)
endif ()

install (
  EXPORT icubaby
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/icubaby"
  NAMESPACE icubaby::
)
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VENDOR "Paul Bowen-Huggett")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include (CPack)
