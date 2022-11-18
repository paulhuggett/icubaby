add_library (icubaby STATIC icubaby.cpp "${CMAKE_CURRENT_SOURCE_DIR}/../../include/icubaby/icubaby.hpp")
target_include_directories (icubaby PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
setup_target (icubaby)
