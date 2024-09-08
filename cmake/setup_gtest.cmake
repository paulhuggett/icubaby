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

include (CheckCXXCompilerFlag)

function (setup_gtest)
  if (EXISTS "${icubaby_project_root}/googletest/CMakeLists.txt")
    # Tell gtest to link against the "Multi-threaded Debug DLL runtime library"
    # on Windows.
    set (gtest_force_shared_crt On CACHE BOOL "Always use msvcrt.dll")
    # We don't want to install either gtest or gmock.
    set (INSTALL_GTEST Off CACHE BOOL "Disable gtest install")
    set (INSTALL_GMOCK Off CACHE BOOL "Disable gmock install")
    add_subdirectory ("${icubaby_project_root}/googletest")

    foreach (target gtest gmock gmock_main gtest_main)
      set (gclang_options -Wno-implicit-int-float-conversion  -Wno-covered-switch-default)
      if (ICUBABY_LIBCXX)
        list (APPEND gclang_options -stdlib=libc++)
      endif ()
      set (gintelllvm_options ${gclang_options} -Wno-tautological-constant-compare)

      target_compile_features (${target} PUBLIC $<IF:$<BOOL:${ICUBABY_CXX17}>,cxx_std_17,cxx_std_20>)
      target_compile_definitions (${target} PUBLIC GTEST_REMOVE_LEGACY_TEST_CASEAPI_=1)
      target_compile_options (${target} PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${gclang_options}>
        $<$<CXX_COMPILER_ID:IntelLLVM>:${gintelllvm_options}>
      )
      target_link_options (${target} PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${gclang_options}>
        $<$<CXX_COMPILER_ID:IntelLLVM>:${gintelllvm_options}>
      )
    endforeach()
  endif()
endfunction (setup_gtest)
