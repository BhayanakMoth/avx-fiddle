// avx-fiddle.cpp
//

#include "avx-fiddle.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <benchmark/benchmark.h>
#include <immintrin.h>

#include <memory>
#include <string>

#include "../ThirdParty/stb_image_write.h"
#include "Grayscale.h"
using namespace std;
int google_benchmark(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
  benchmark::RunSpecifiedBenchmarks();
}

static void BM_AVXGrayScale(benchmark::State& state) {
  int width = 0;
  int height = 0;
  int comp = 0;
  uint8_t* img_buffer =
      stbi_load("./HelloKitty.jpg", &width, &height, &comp, 4);
  uint8_t* out_img = new uint8_t[width * height];
  for (auto _ : state)
    uint8_t* out =
        toGrayScaleAVX((uint32_t*)img_buffer, out_img, width, height);

  // stbi_write_jpg("./Trash.jpg", width, height, 1, out_img, 1);
  STBI_FREE(img_buffer);
  delete out_img;
}
static void BM_GrayScale(benchmark::State& state) {
  int width = 0;
  int height = 0;
  int comp = 0;
  uint8_t* img_buffer =
      stbi_load("./HelloKitty.jpg", &width, &height, &comp, 4);
  uint8_t* out_img = new uint8_t[width * height];
  for (auto _ : state)
    toGreyScale((uint32_t*)img_buffer, out_img, width, height, 4);

  // stbi_write_jpg("./Trash.jpg", width, height, 1, out_img, 1);
  STBI_FREE(img_buffer);
  delete out_img;
}

BENCHMARK(BM_AVXGrayScale)->Unit(benchmark::TimeUnit::kMillisecond);
BENCHMARK(BM_GrayScale)->Unit(benchmark::TimeUnit::kMillisecond);

int main(int n, char* argv[]) {
  google_benchmark(n, argv);

  std::cout << "Run successful.\n";
  return 0;
}
