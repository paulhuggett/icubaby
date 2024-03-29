
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

include (CheckCXXCompilerFlag)
include (CheckLinkerFlag)

function (setup_target target)
  cmake_parse_arguments (
    arg # prefix
    "" # options
    "PEDANTIC" # one-value-keywords
    "" # multi-value-keywords
    ${ARGN}
  )
  # PEDANTIC defaults to "Yes".
  if ("${arg_PEDANTIC}" STREQUAL "")
    set (arg_PEDANTIC "Yes")
  endif ()

  set (clang_warning_options
    -Weverything
    -Wno-c++14-extensions
    -Wno-c++20-compat
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-c99-extensions
    -Wno-ctad-maybe-unsupported
    -Wno-documentation-unknown-command
    -Wno-exit-time-destructors
    -Wno-padded
    -Wno-undef
  )
  # Some clang warning switches are not available in all versions of the compiler.
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
    check_cxx_compiler_flag (-Wno-unsafe-buffer-usage CLANG_W_UNSAFE_BUFFER_USAGE)
    list (APPEND clang_warning_options $<$<BOOL:${CLANG_W_UNSAFE_BUFFER_USAGE}>:-Wno-unsafe-buffer-usage>)
  endif ()

  set (gcc_warning_options
    -Wall
    -Wextra
    -pedantic
  )
  set (msvc_warning_options
    -W4
    -wd4324 # 4324: structure was padded due to alignment specifier
    -wd4068 # 4068: unknown pragma
  )

  if (ICUBABY_WERROR)
    list (APPEND clang_options -Werror)
    list (APPEND gcc_options -Werror)
    list (APPEND msvc_options /WX)
  endif ()
  if (ICUBABY_LIBCXX)
    list (APPEND clang_options -stdlib=libc++)
  endif ()
  if (ICUBABY_COVERAGE)
    list (APPEND clang_options -fprofile-instr-generate -fcoverage-mapping)
    list (APPEND gcc_options -fprofile-arcs -ftest-coverage)
  endif ()
  if (ICUBABY_SANITIZE)
    list (APPEND clang_options -fsanitize=undefined -fsanitize=address)
    list (APPEND gcc_options -fsanitize=undefined -fsanitize=address -fno-sanitize-recover)
  endif ()

  list (APPEND clang_options $<$<BOOL:${arg_PEDANTIC}>:${clang_warning_options}>)
  list (APPEND gcc_options   $<$<BOOL:${arg_PEDANTIC}>:${gcc_warning_options}>)
  list (APPEND msvc_options  $<$<BOOL:${arg_PEDANTIC}>:${msvc_warning_options}>)

  target_compile_options (${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:IntelLLVM>>:${clang_options}>
    $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
    $<$<CXX_COMPILER_ID:MSVC>:${msvc_options}>
  )
  target_compile_definitions (${target} PUBLIC ICUBABY_FUZZTEST=$<BOOL:${ICUBABY_FUZZTEST}>)
  target_compile_features (${target} PUBLIC $<IF:$<BOOL:${ICUBABY_CXX17}>,cxx_std_17,cxx_std_20>)
  target_link_options (${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:IntelLLVM>>:${clang_options}>
    $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
    $<$<CXX_COMPILER_ID:MSVC>:>
  )

  # If the link step supports -Wl,-Map generate a map file from the link.
  check_linker_flag (CXX "-Wl,-Map=map.txt" map_supported)
  if (map_supported)
    target_link_options (${target} PRIVATE "-Wl,-Map=$<TARGET_FILE_DIR:${target}>/map.txt")
  endif ()

endfunction (setup_target)

