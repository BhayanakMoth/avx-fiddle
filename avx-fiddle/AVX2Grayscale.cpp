#include <immintrin.h>

#include "Grayscale.h"
// Get 16 RGB pixel values packed
inline __m256i packRed(__m256i a, __m256i b) {
  __m256i mask = _mm256_set1_epi32(0xFF);
  a = _mm256_and_si256(a, mask);
  b = _mm256_and_si256(b, mask);
  __m256i result = _mm256_packus_epi32(a, b);
  return _mm256_slli_si256(result, 1);
}
inline __m256i packGreen(__m256i a, __m256i b) {
  __m256i mask = _mm256_set1_epi32(0xFF00);
  a = _mm256_and_si256(a, mask);
  b = _mm256_and_si256(b, mask);
  __m256i result = _mm256_packus_epi32(a, b);
  return result;
}
inline __m256i packBlue(__m256i a, __m256i b) {
  __m256i mask = _mm256_set1_epi32(0xFF00);
  a = _mm256_srli_si256(a, 1);
  b = _mm256_srli_si256(b, 1);
  a = _mm256_and_si256(a, mask);
  b = _mm256_and_si256(b, mask);
  __m256i result = _mm256_packus_epi32(a, b);
  return result;
}
inline __m256i togray(__m256i red, __m256i green, __m256i blue) {
  red = _mm256_mulhi_epu16(red, _mm256_set1_epi16((short)mulRed));
  green = _mm256_mulhi_epu16(green, _mm256_set1_epi16((short)mulGreen));
  blue = _mm256_mulhi_epu16(blue, _mm256_set1_epi16((short)mulBlue));
  __m256i result = _mm256_add_epi16(_mm256_add_epi16(red, green), blue);
  return _mm256_srli_epi16(result, 8);
}
__m256i convert(__m256i* src_img) {
  __m256i* src = src_img;

  __m256i a = _mm256_loadu_si256(src);
  __m256i b = _mm256_loadu_si256((src + 1));
  __m256i red = packRed(a, b);
  __m256i green = packGreen(a, b);
  __m256i blue = packBlue(a, b);
  __m256i result = togray(red, green, blue);
  return result;
}
uint8_t* toGrayScaleAVX(uint32_t* rgb_img, uint8_t* out_img, uint32_t width,
                        uint32_t height) {
  __m256i* src = (__m256i*)rgb_img;
  __m256i* out = (__m256i*)out_img;
  uint32_t components = 4;
  __m256i* end = src + (width * height) / sizeof(__m256i);
  for (uint32_t i = 0; i < width * height * components;
       i += sizeof(__m256i) * 4) {
    __m256i lo = convert(src);
    __m256i hi = convert(src + 2);
    src += 4;
    constexpr int permuteControl1 = _MM_SHUFFLE(3, 1, 2, 0);
    lo = _mm256_permute4x64_epi64(lo, permuteControl1);
    hi = _mm256_permute4x64_epi64(hi, permuteControl1);

    auto packed = _mm256_packus_epi16(lo, hi);
    constexpr int permuteControl2 = _MM_SHUFFLE(3, 1, 2, 0);
    packed = _mm256_permute4x64_epi64(packed, permuteControl2);
    _mm256_storeu_si256(out, packed);
    out += 1;
  }
  // return nullptr;
  return out_img;
}

void sample() {
  const uint32_t width = 32;
  const uint32_t height = 32;
  const uint32_t length = width * height;
  uint8_t* buff = new uint8_t[length * 4];
  for (int i = 0; i < length; i += 4) {
    buff[i] = 255 - (i % 255);
    buff[i + 1] = 160;
    buff[i + 2] = 140;
    buff[i + 3] = 230;
  }
  // toGrayScaleAVX((uint32_t*)buff, width, height);
}