#include "Grayscale.h"
void toGreyScale(uint32_t* source_pixels, uint8_t* destinationBytes , int width, int height, int components)
{

	const uint32_t* const sourceEnd = source_pixels + (width*height);

	for (; source_pixels < sourceEnd; source_pixels++, destinationBytes++)
	{
		const uint32_t pixel = *source_pixels;
		const uint16_t red = (uint16_t)(pixel & 0xFF) * mulRed;
		const uint16_t green = (uint16_t)((pixel >> 8) & 0xFF) * mulGreen;
		const uint16_t blue = (uint16_t)((pixel >> 16) & 0xFF) * mulBlue;
		uint16_t out = (red + green + blue);
		uint16_t out2 = out >> 8;
		*destinationBytes = (uint8_t)((red + green + blue) >> 8);
	}
}