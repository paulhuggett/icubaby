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

add_executable (icubaby-exhaust-test exhaust.cpp)
target_link_libraries (icubaby-exhaust-test PUBLIC icubaby)
setup_target (icubaby-exhaust-test)

if (TARGET gtest)
  set (ICUBABY_TEST_TARGET icubaby-unittests)
  set (ICUBABY_OUT_XML "${CMAKE_BINARY_DIR}/${ICUBABY_TEST_TARGET}.xml")
  add_custom_command (
    TARGET icubaby-exhaust-test
    PRE_LINK
    COMMAND "$<TARGET_FILE:${ICUBABY_TEST_TARGET}>"
            "--gtest_output=xml:${ICUBABY_OUT_XML}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running unit tests" DEPENDS ${ICUBABY_TEST_TARGET}
    BYPRODUCTS "${CMAKE_BINARY_DIR}/${ICUBABY_OUT_XML}"
    VERBATIM
  )
endif (TARGET gtest)
