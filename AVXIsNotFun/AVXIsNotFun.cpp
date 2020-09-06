// AVXIsNotFun.cpp
//

#include "AVXIsNotFun.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../ThirdParty/stb_image_write.h"

using namespace std;

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

		stbi_write_jpg(image_out.c_str(), width, height, components, img_buffer, 1);
		STBI_FREE(img_buffer);
		std::cout << "Run successful.\n";
		std::system("pause");
	}
	else
	{
		std::cout << "Usage: AVXIsNotFun.exe /path/to/input /path/to/output \n";
		std::system("pause");
	}
	return 0;
}
