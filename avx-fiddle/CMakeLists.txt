# CMakeList.txt : CMake project for AVXIsNotFun, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (avx-fiddle "avx-fiddle.cpp" "avx-fiddle.h"  "Grayscale.h" "ScalarGrayscale.cpp" "AVX2Grayscale.cpp")
target_link_libraries(avx-fiddle benchmark::benchmark)
# TODO: Add tests and install targets if needed.
