if (standard GREATER_EQUAL 20)
  add_executable (ranges ranges.cpp)
  target_link_libraries (ranges PUBLIC icubaby)
endif ()
