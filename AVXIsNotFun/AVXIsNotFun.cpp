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
const long int length = 1024*1024*8;
int * a = nullptr;
int * b = nullptr;
int * c = nullptr;
int * d = nullptr;
int width = 0;
int height = 0;
unsigned char* img_buffer = nullptr;
int div_val[8] = { 3,3,3,3,3,3,3,3 };
__m256i result, B, C, D, E;
static void BM_simd(benchmark::State& state) {

	for (auto _ : state) {
		
		for (int i = 0; i < length; i += sizeof(__m256i) / sizeof(unsigned int)) {
			B = _mm256_lddqu_si256((__m256i*)(&b[i]));
			C = _mm256_lddqu_si256((__m256i*)(&b[i]));
			D = _mm256_lddqu_si256((__m256i*)(&b[i]));
			result = _mm256_add_epi32(B, C);
			result = _mm256_add_epi32(result, D);
			result = _mm256_div_epi32(result, E);
			_mm256_storeu_si256((__m256i*)&a[i], result);
		}
	}
}
static void BM_nosimd(benchmark::State& state){
	for (auto _ : state) {
		for (int i = 0; i < length; i += 1) {
			a[i] = (b[i] + c[i] + d[i])/3;
		}
	}
}
void _toGreyScale_simd(unsigned char* rgb_img, int width, int height, int components, unsigned char* output) {

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
	for (int i = 0; i < length; i++) {
		b[i] = 1;
		c[i] = 2;
		d[i] = 3;
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
//BENCHMARK(BM_toGreyScale)->Unit(benchmark::TimeUnit::kMicrosecond);
BENCHMARK(BM_simd)->Unit(benchmark::TimeUnit::kMicrosecond);
BENCHMARK(BM_nosimd)->Unit(benchmark::TimeUnit::kMicrosecond);
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
	
		//std::cout << argv[0] << "\n";
		////std::string image_loc = argv[1];
		////std::string image_out = argv[2];
		//width = 0;
		//height = 0;
		//int components = 0;
		//img_buffer = stbi_load("./HelloKitty.jpg", &width, &height, &components, 4);
		//
	a = new int[length];
	b = new int[length];
	c = new int[length];
	d = new int[length];

	E = _mm256_loadu_epi32(&div_val[0]);
	fill_arrays();
	google_benchmark(n, argv);
	/*	unsigned char * grey_img = toGreyScale(img_buffer, width, height,4);
		stbi_write_jpg("./Trash.jpg", width, height, 4, grey_img, 100);
	*/
	//STBI_FREE(img_buffer);
	//delete grey_img;
	std::cout << "Run successful.\n";

	
	
	return 0;
}
