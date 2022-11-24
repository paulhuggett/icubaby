include (FindIconv)
if (Iconv_FOUND)
  add_executable (iconv-test iconv.cpp)
  target_include_directories (iconv-test PUBLIC ${Iconv_INCLUDE_DIRS})
  target_link_libraries (iconv-test PUBLIC icubaby ${Iconv_LIBRARIES})
  setup_target (iconv-test)

  target_compile_options (${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-unreachable-code-return
  )
endif (Iconv_FOUND)
