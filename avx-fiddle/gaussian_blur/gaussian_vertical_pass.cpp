/*
Reference:
https://software.intel.com/content/dam/develop/external/us/en/documents/gaussian-filter-181134.pdf
https://software.intel.com/content/dam/develop/public/us/en/downloads/gaussian_blur_0311.cpp
*/

#include "gaussian_vertical_pass.h"

#include <immintrin.h>

/* AVX Implementation: gaussianHorizontal_avx
 * oTemp  - Temporary small buffer used between left to right pass
 * id     - input image
 * od     - output image from this filter
 * height - image height
 * width  - image width
 * a0, a1, a2, a3, b1, b2, cprev, cnext: Gaussian coefficients
 */
void gaussianVertical_avx(float *oTemp, float *id, unsigned int *od, int height,
                          int width, float *a0, float *a1, float *a2, float *a3,
                          float *b1, float *b2, float *cprev, float *cnext) {
  __m256 prevIn, currIn, prevOut, prev2Out, prevInN, currInN, prevOutN,
      prev2OutN;
  // Edge calculations
  prevIn = _mm256_castps128_ps256(_mm_loadu_ps((float *)id));
  prevIn = _mm256_insertf128_ps(prevIn,
                                _mm_loadu_ps((float *)(id + 4 * height)), 0x1);
  prev2Out = _mm256_mul_ps(prevIn, _mm256_broadcast_ss((float *)cprev));
  prevOut = prev2Out;
  prevInN = _mm256_castps128_ps256(_mm_loadu_ps((float *)(id + 8 * height)));
  prevInN = _mm256_insertf128_ps(
      prevInN, _mm_loadu_ps((float *)(id + 12 * height)), 0x1);

  prev2OutN = _mm256_mul_ps(prevInN, _mm256_broadcast_ss((float *)cprev));
  prevOutN = prev2OutN;

  // it is widthwise as input is transposed.
  for (int y = 0; y < height; y++) {
    // Unpack 2 input pixels
    currIn = _mm256_castps128_ps256(_mm_loadu_ps((float *)id));
    currIn = _mm256_insertf128_ps(
        currIn, _mm_loadu_ps((float *)(id + 4 * height)), 0x1);

    // Unpack 2 more input pixels
    currInN = _mm256_castps128_ps256(_mm_loadu_ps((float *)(id + 8 * height)));
    currInN = _mm256_insertf128_ps(
        currInN, _mm_loadu_ps((float *)(id + 12 * height)), 0x1);
    // Process first 2 pixels
    __m256 currComp = _mm256_mul_ps(currIn, _mm256_broadcast_ss((float *)a0));
    __m256 temp1 = _mm256_mul_ps(prevIn, _mm256_broadcast_ss((float *)a1));
    __m256 temp2 = _mm256_mul_ps(prevOut, _mm256_broadcast_ss((float *)b1));
    __m256 temp3 = _mm256_mul_ps(prev2Out, _mm256_broadcast_ss((float *)b2));
    currComp = _mm256_add_ps(currComp, temp1);
    temp2 = _mm256_add_ps(temp2, temp3);
    prev2Out = prevOut;
    prevOut = _mm256_sub_ps(currComp, temp2);
    prevIn = currIn;
    // Store output in temp buffer
    _mm256_store_ps((float *)(oTemp), prevOut);

    // Process next 2 pixels
    currComp = _mm256_mul_ps(currInN, _mm256_broadcast_ss((float *)a0));
    temp1 = _mm256_mul_ps(prevInN, _mm256_broadcast_ss((float *)a1));
    temp2 = _mm256_mul_ps(prevOutN, _mm256_broadcast_ss((float *)b1));
    temp3 = _mm256_mul_ps(prev2OutN, _mm256_broadcast_ss((float *)b2));
    currComp = _mm256_add_ps(currComp, temp1);
    temp2 = _mm256_add_ps(temp2, temp3);
    prev2OutN = prevOutN;
    prevOutN = _mm256_sub_ps(currComp, temp2);
    prevInN = currInN;
    _mm256_store_ps((float *)(oTemp + 8), prevOutN);

    id += 4;
    oTemp += 16;  // move to next row
  }               // height = j loop

  // start right-to-left pass
  id -= 4;
  oTemp -= 16;
  od += width * (height - 1);  // set to last row.

  // edge caculations
  prevIn = _mm256_castps128_ps256(_mm_loadu_ps((float *)id));
  prevIn = _mm256_insertf128_ps(prevIn,
                                _mm_loadu_ps((float *)(id + 4 * height)), 0x1);
  currIn = prevIn;
  prev2Out = _mm256_mul_ps(prevIn, _mm256_broadcast_ss((float *)cnext));
  prevOut = prev2Out;
  prevInN = _mm256_castps128_ps256(_mm_loadu_ps((float *)(id + 8 * height)));
  prevInN = _mm256_insertf128_ps(
      prevInN, _mm_loadu_ps((float *)(id + 12 * height)), 0x1);
  currInN = prevInN;
  prev2OutN = _mm256_mul_ps(prevInN, _mm256_broadcast_ss((float *)cnext));
  prevOutN = prev2OutN;

  for (int y = height - 1; y >= 0; y--) {
    // load 2 input pixel
    __m256 inNext = _mm256_castps128_ps256(_mm_loadu_ps((float *)id));
    inNext = _mm256_insertf128_ps(
        inNext, _mm_loadu_ps((float *)(id + 4 * height)), 0x1);

    // load 2 more input pixel
    __m256 inNextN =
        _mm256_castps128_ps256(_mm_loadu_ps((float *)(id + 8 * height)));
    inNextN = _mm256_insertf128_ps(
        inNextN, _mm_loadu_ps((float *)(id + 12 * height)), 0x1);

    // get output from left-to-right pass
    __m256 output = _mm256_load_ps((float *)(oTemp));

    __m256 currComp = _mm256_mul_ps(currIn, _mm256_broadcast_ss((float *)a2));
    __m256 temp1 = _mm256_mul_ps(prevIn, _mm256_broadcast_ss((float *)a3));
    __m256 temp2 = _mm256_mul_ps(prevOut, _mm256_broadcast_ss((float *)b1));
    __m256 temp3 = _mm256_mul_ps(prev2Out, _mm256_broadcast_ss((float *)b2));
    currComp = _mm256_add_ps(currComp, temp1);
    temp2 = _mm256_add_ps(temp2, temp3);
    prev2Out = prevOut;
    prevOut = _mm256_sub_ps(currComp, temp2);
    prevIn = currIn;
    currIn = inNext;

    // add currently computed output left-to-right pass output
    output = _mm256_add_ps(output, prevOut);

    output = _mm256_castsi256_ps(_mm256_cvttps_epi32(output));

    __m128i output2 = _mm_castps_si128(_mm256_extractf128_ps(output, 1));
    __m128i output1 = _mm_castps_si128(_mm256_castps256_ps128(output));

    // get the left-to-right pass output
    __m256 outputN = _mm256_load_ps((float *)(oTemp + 8));

    currComp = _mm256_mul_ps(currInN, _mm256_broadcast_ss((float *)a2));
    temp1 = _mm256_mul_ps(prevInN, _mm256_broadcast_ss((float *)a3));
    temp2 = _mm256_mul_ps(prevOutN, _mm256_broadcast_ss((float *)b1));
    temp3 = _mm256_mul_ps(prev2OutN, _mm256_broadcast_ss((float *)b2));
    currComp = _mm256_add_ps(currComp, temp1);
    temp2 = _mm256_add_ps(temp2, temp3);
    prev2OutN = prevOutN;
    prevOutN = _mm256_sub_ps(currComp, temp2);
    prevInN = currInN;
    currInN = inNextN;
    outputN = _mm256_add_ps(outputN, prevOutN);
    outputN = _mm256_castsi256_ps(_mm256_cvttps_epi32(outputN));

    // convert to RGBA
    __m128i output4 = _mm_castps_si128(_mm256_extractf128_ps(outputN, 1));
    __m128i output3 = _mm_castps_si128(_mm256_castps256_ps128(outputN));

    output1 = _mm_packus_epi32(output1, output2);
    output3 = _mm_packus_epi32(output3, output4);
    output1 = _mm_packus_epi16(output1, output3);

    _mm_store_si128((__m128i *)(od), output1);

    id -= 4;
    od -= width;
    oTemp -= 16;  // move to previous row
  }               // height = j loop
  _mm256_zeroupper();
}  // gaussianvertical AVX