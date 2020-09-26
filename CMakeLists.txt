# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_BUILD_TYPE=Release)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mavx2 -march=native")

project ("avx-fiddle")
find_package(benchmark REQUIRED)
# Include sub-projects.
add_subdirectory ("avx-fiddle")

