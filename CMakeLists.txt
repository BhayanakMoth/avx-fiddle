# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("AVXIsNotFun")
find_package(benchmark REQUIRED)
# Include sub-projects.
add_subdirectory ("AVXIsNotFun")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-mavx2")
endif()
