# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-src"
  "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-build"
  "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-subbuild/nanobench-populate-prefix"
  "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-subbuild/nanobench-populate-prefix/tmp"
  "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-subbuild/nanobench-populate-prefix/src/nanobench-populate-stamp"
  "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-subbuild/nanobench-populate-prefix/src"
  "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-subbuild/nanobench-populate-prefix/src/nanobench-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-subbuild/nanobench-populate-prefix/src/nanobench-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Alone/CLionProjects/my-logger/cmake-build-release-visual-studio/_deps/nanobench-subbuild/nanobench-populate-prefix/src/nanobench-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
