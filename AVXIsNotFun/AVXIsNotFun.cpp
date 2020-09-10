// AVXIsNotFun.cpp
//

#include "AVXIsNotFun.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../ThirdParty/stb_image_write.h"
#include <immintrin.h>
using namespace std;

float a[256] = { 0 };
float b[256] = { 0 };
float c[256] = { 0 };
void foo() {
	__m256 result, B, C;
	for (int i = 0; i < 256; i += sizeof(__m256) / sizeof(float)) {
		B = _mm256_load_ps(&b[i]);
		C = _mm256_load_ps(&c[i]);
		result = _mm256_add_ps(B, C);
		_mm256_store_ps(&a[i], result);
	}
}
void fill_arrays() {
	for (int i = 0; i < 256; i++) {
		b[i] = 1.0;
		c[i] = 2.0;

	}
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
int main(int  n, char * argv[])
{

	if (n == 3) {
		std::cout << argv[0] << "\n";
		std::string image_loc = argv[1];
		std::string image_out = argv[2];
		int width = 0;
		int height = 0;
		int components = 0;
		unsigned char* img_buffer = stbi_load(image_loc.c_str(), &width, &height, &components, 0);
		//Filtering stuff
		fill_arrays();
		foo();
		stbi_write_jpg(image_out.c_str(), width, height, components, img_buffer, 1);
		STBI_FREE(img_buffer);
		std::cout << "Run successful.\n";

	}
	else
	{
		std::cout << "Usage: AVXIsNotFun.exe /path/to/input.jpeg /path/to/output.jpeg \n";
	}
	return 0;
}
