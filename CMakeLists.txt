# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_BUILD_TYPE=Release)

set(CMAKE_CXX_FLAGS_RELEASE "/O2")

project ("AVXIsNotFun")
find_package(benchmark REQUIRED)
# Include sub-projects.
add_subdirectory ("AVXIsNotFun")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-mavx2")
endif()
