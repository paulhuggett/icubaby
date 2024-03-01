if (NOT ICUBABY_CXX17)
  add_executable (icubaby-example-view-utf32-to-16 view_utf32_to_16.cpp)
  target_link_libraries (icubaby-example-view-utf32-to-16 PRIVATE icubaby)
  setup_target (icubaby-example-view-utf32-to-16)

  add_executable (icubaby-example-bytes-to-utf8 bytes_to_utf8.cpp)
  target_link_libraries (icubaby-example-bytes-to-utf8 PRIVATE icubaby)
  setup_target (icubaby-example-bytes-to-utf8)
endif ()

add_executable (icubaby-example-utf8-to-16-iterator iterator.cpp)
target_link_libraries (icubaby-example-utf8-to-16-iterator PRIVATE icubaby)
setup_target (icubaby-example-utf8-to-16-iterator)

add_executable (icubaby-example-manual-bytes-to-utf8 manual_bytes_to_utf8.cpp)
target_link_libraries (icubaby-example-manual-bytes-to-utf8 PRIVATE icubaby)
setup_target (icubaby-example-manual-bytes-to-utf8)
