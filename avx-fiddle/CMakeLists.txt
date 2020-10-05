cmake_minimum_required(VERSION 3.8)

add_executable(avx-fiddle "avx-fiddle.cpp" "avx-fiddle.h" "Grayscale.h" "ScalarGrayscale.cpp" "AVX2Grayscale.cpp")

set(GAUSSIAN_BLUR_ROOT gaussian_blur)

set(GAUSSIAN_BLUR_INCLUDES ${GAUSSIAN_BLUR_ROOT}/gaussian_blur.h
													 ${GAUSSIAN_BLUR_ROOT}/gaussian_coefficient.h
													 ${GAUSSIAN_BLUR_ROOT}/gaussian_horizontal_pass.h
													 ${GAUSSIAN_BLUR_ROOT}/gaussian_utils.h
													 ${GAUSSIAN_BLUR_ROOT}/gaussian_vertical_pass.h)

set(GAUSSIAN_BLUR_SOURCES ${GAUSSIAN_BLUR_ROOT}/gaussian_blur.cpp
													${GAUSSIAN_BLUR_ROOT}/gaussian_coefficient.cpp
													${GAUSSIAN_BLUR_ROOT}/gaussian_horizontal_pass.cpp
													${GAUSSIAN_BLUR_ROOT}/gaussian_utils.cpp
													${GAUSSIAN_BLUR_ROOT}/gaussian_vertical_pass.cpp)

add_executable(gaussian_blur ${GAUSSIAN_BLUR_INCLUDES} ${GAUSSIAN_BLUR_SOURCES})

target_link_libraries(avx-fiddle benchmark::benchmark)
#TODO : Add tests and install targets if needed.
