add_executable (ranges ranges.cpp)
target_compile_features (ranges PUBLIC cxx_std_20)
target_link_libraries (ranges PUBLIC icubaby)
