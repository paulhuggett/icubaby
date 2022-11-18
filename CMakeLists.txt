cmake_minimum_required(VERSION 3.22)

project (icubaby CXX)

function (setup_target target)
  set (clang_options
    -Weverything
    -Wno-c++14-extensions
    -Wno-c++20-compat
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-c99-extensions
    -Wno-exit-time-destructors
    -Wno-padded
    -Wno-undef
    -stdlib=libc++
  )
  set (gcc_options
    -Wall
    -Wextra
    -pedantic
  )
  set (msvc_options
    -W4
    -wd4324 # 4324: structure was padded due to alignment specifier
    -wd4068 # 4068: unknown pragma
  )

  if (WERROR)
    list (APPEND clang_options -Werror)
    list (APPEND gcc_options -Werror)
    list (APPEND msvc_options /WX)
  endif ()

  if (COVERAGE)
    list (APPEND clang_options -fprofile-instr-generate -fcoverage-mapping)
  endif ()

  set_target_properties (${target} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED Yes
    CXX_EXTENSIONS No
  )

  target_compile_options (${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${clang_options}>
    $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
    $<$<CXX_COMPILER_ID:MSVC>:${msvc_options}>
  )
  target_link_options (${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${clang_options}>
    $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
    $<$<CXX_COMPILER_ID:MSVC>:>
  )
endfunction (setup_target)

add_subdirectory (lib)

add_executable (main main.cpp)
target_link_libraries (main PUBLIC icubaby)
setup_target (main)

add_subdirectory (tests)

