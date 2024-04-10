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

include (setup_target)

function (add_example target)
  cmake_parse_arguments (
    arg # prefix
    "" # options
    "" # one-value-keywords
    "SOURCES" # multi-value-keywords
    ${ARGN}
  )
  set (actual_target "icubaby-example-${target}")
  add_executable ("${actual_target}" ${arg_SOURCES})
  target_link_libraries ("${actual_target}" PUBLIC icubaby)
  setup_target ("${actual_target}")
endfunction (add_example)

add_example (view-utf32-to-16 SOURCES view_utf32_to_16.cpp)
add_example (bytes-to-utf8 SOURCES bytes_to_utf8.cpp)
add_example (utf8-to-16-iterator SOURCES iterator.cpp)
add_example (manual-bytes-to-utf8 SOURCES manual_bytes_to_utf8.cpp)
