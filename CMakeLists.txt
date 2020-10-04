cmake_minimum_required (VERSION 3.8)
project ("avx-fiddle")

find_package(benchmark REQUIRED)

set(CMAKE_BUILD_TYPE=Release)
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -O2 -mavx2 -march=native")

add_subdirectory ("avx-fiddle")
