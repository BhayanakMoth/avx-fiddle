#pragma once
#include <iostream>
// The coefficients to produce the gray values
constexpr float mulRedFloat = 0.29891f;
constexpr float mulGreenFloat = 0.58661f;
constexpr float mulBlueFloat = 0.11448f;
// Same coefficients in 16-bit fixed point form.
constexpr uint16_t mulRed = (uint16_t)(mulRedFloat * 0x10000);
constexpr uint16_t mulGreen = (uint16_t)(mulGreenFloat * 0x10000);
constexpr uint16_t mulBlue = (uint16_t)(mulBlueFloat * 0x10000);

void toGreyScale(uint32_t* source_pixels, uint8_t* destinationBytes, int width,
                 int height, int components);
uint8_t* toGrayScaleAVX(uint32_t* rgb_img, uint8_t* out_img, uint32_t width,
                        uint32_t height);
void sample();