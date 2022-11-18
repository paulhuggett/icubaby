add_executable (icubaby-unittests harness.cpp test_u8.cpp test_u16.cpp)
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
