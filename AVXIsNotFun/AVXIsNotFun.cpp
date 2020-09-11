// AVXIsNotFun.cpp
//

#include "AVXIsNotFun.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../ThirdParty/stb_image_write.h"
#include <immintrin.h>
#include <benchmark/benchmark.h>
#include <string>

using namespace std;

float a[256] = { 0 };
float b[256] = { 0 };
float c[256] = { 0 };
int width = 0;
int height = 0;
unsigned char* img_buffer = nullptr;
static void simd(benchmark::State& state) {
	for (auto _ : state) {
		__m256 result, B, C;
		for (int i = 0; i < 256; i += sizeof(__m256) / sizeof(float)) {
			B = _mm256_load_ps(&b[i]);
			C = _mm256_load_ps(&c[i]);
			result = _mm256_add_ps(B, C);
			_mm256_store_ps(&a[i], result);
		}
	}
}
static void nonsimd(benchmark::State& state){
	for (auto _ : state) {
		for (int i = 0; i < 256; i += 1) {
			a[i] = b[i] + c[i];
		}
	}
}
void _toGreyScale(unsigned char* rgb_img, int width, int height, int components, unsigned char * output) {
	
	unsigned int out_idx = 0;
	for (unsigned int i = 0; i < width * height * components; i += 4) {
		unsigned char r = rgb_img[i];
		unsigned char g = rgb_img[i + 1];
		unsigned char b = rgb_img[i + 2];
		int out = (r + g + b) / 3;
		output[out_idx] = out;
		output[out_idx + 1] = out;
		output[out_idx + 2] = out;
		output[out_idx + 3] = 255;
		out_idx += 4;
	}
}
static void BM_toGreyScale(benchmark::State& state) {
	int components = 4;
	unsigned char* output = new unsigned char[width * height * components];
	
	for (auto _ : state) {
		_toGreyScale(img_buffer, width, height, components, output);
	}
	delete output;
}
void fill_arrays() {
	for (int i = 0; i < 256; i++) {
		b[i] = 1.0;
		c[i] = 2.0;

	}
}

int google_benchmark(int argc, char** argv) {
	benchmark::Initialize(&argc, argv);
	fill_arrays();
	if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
	benchmark::RunSpecifiedBenchmarks();
}
int check_arrays() {
	int ret = 0;
	for (int i = 0; i < 256; i++) {
		if (a[i] == 3)
			continue;
		else
			printf("FAIL, corruption in arithmetic");
		ret = -1;
		break;
	}
	return ret;
}
BENCHMARK(BM_toGreyScale)->Unit(benchmark::TimeUnit::kMicrosecond);
//unsigned char* toGreyScale(unsigned char* rgb_img, int width, int height,int components) {
//	unsigned char * output = new unsigned char[width * height * components];
//	unsigned int out_idx = 0;
//	for (unsigned int i = 0; i < width*height*components; i+=4) {
//		unsigned char r = rgb_img[i];
//		unsigned char g = rgb_img[i+1];
//		unsigned char b = rgb_img[i+2];
//		int out = (r + g + b) / 3;
//		output[out_idx] = out;
//		output[out_idx+1] = out;
//		output[out_idx+2] = out;
//		output[out_idx + 3] = 255;
//		out_idx += 4;
//	}
//	return output;
//}
int main(int  n, char * argv[])
{
	
		std::cout << argv[0] << "\n";
		//std::string image_loc = argv[1];
		//std::string image_out = argv[2];
		width = 0;
		height = 0;
		int components = 0;
		img_buffer = stbi_load("./HelloKitty.jpg", &width, &height, &components, 4);
		google_benchmark(n, argv);
	/*	unsigned char * grey_img = toGreyScale(img_buffer, width, height,4);
		stbi_write_jpg("./Trash.jpg", width, height, 4, grey_img, 100);
	*/	STBI_FREE(img_buffer);
		//delete grey_img;
		std::cout << "Run successful.\n";

	
	
	return 0;
}
