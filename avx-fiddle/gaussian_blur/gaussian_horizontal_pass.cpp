/*
Reference:
https://software.intel.com/content/dam/develop/external/us/en/documents/gaussian-filter-181134.pdf
https://software.intel.com/content/dam/develop/public/us/en/downloads/gaussian_blur_0311.cpp
*/

#include "gaussian_horizontal_pass.h"

#include <immintrin.h>

/* AVX Implementation: gaussianHorizontal_avx
 * oTemp  - Temporary small buffer used between left to right pass
 * id     - input image
 * od     - output image from this filter
 * height - image height
 * width  - image original width
 * Nwidth - Padded width
 * a0, a1, a2, a3, b1, b2, cprev, cnext: Gaussian coefficients
 */
void gaussianHorizontal_avx(float *oTemp, unsigned int *id, float *od,
                            int width, int height, int Nwidth, float *a0,
                            float *a1, float *a2, float *a3, float *b1,
                            float *b2, float *cprev, float *cnext) {
  __m256 prevIn, currIn, prevOut, prev2Out, prevInN, currInN, prevOutN,
      prev2OutN;

  // edge calculations
  prevIn = _mm256_castsi256_ps(
      _mm256_castsi128_si256(_mm_cvtepu8_epi32(*(__m128i *)(id))));
  prevIn = _mm256_insertf128_ps(
      prevIn, _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + Nwidth)))),
      0x1);
  prevIn = _mm256_cvtepi32_ps(_mm256_castps_si256(prevIn));
  prev2Out = _mm256_mul_ps(prevIn, _mm256_broadcast_ss((float *)cprev));
  prevOut = prev2Out;

  prevInN = _mm256_castsi256_ps(
      _mm256_castsi128_si256(_mm_cvtepu8_epi32(*(__m128i *)(id + 2 * Nwidth))));
  prevInN = _mm256_insertf128_ps(
      prevInN,
      _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + 3 * Nwidth)))),
      0x01);
  prevInN = _mm256_cvtepi32_ps(_mm256_castps_si256(prevInN));
  prev2OutN = _mm256_mul_ps(prevInN, _mm256_broadcast_ss((float *)cprev));
  prevOutN = prev2OutN;

  for (int y = 0; y < width; y++) {
    // Load 2 input pixels from adjacent rows
    __m256 currIn = _mm256_castsi256_ps(
        _mm256_castsi128_si256(_mm_cvtepu8_epi32(*(__m128i *)(id))));
    currIn = _mm256_insertf128_ps(
        currIn,
        _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + Nwidth)))), 0x1);
    currIn = _mm256_cvtepi32_ps(_mm256_castps_si256(currIn));
    // Load 2 more input pixels from next adjacent rows
    __m256 currInN = _mm256_castsi256_ps(_mm256_castsi128_si256(
        _mm_cvtepu8_epi32(*(__m128i *)(id + 2 * Nwidth))));
    currInN = _mm256_insertf128_ps(
        currInN,
        _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + 3 * Nwidth)))),
        0x1);
    currInN = _mm256_cvtepi32_ps(_mm256_castps_si256(currInN));

    __m256 currComp = _mm256_mul_ps(currIn, _mm256_broadcast_ss((float *)a0));
    __m256 currCompN = _mm256_mul_ps(currInN, _mm256_broadcast_ss((float *)a0));
    __m256 temp1 = _mm256_mul_ps(prevIn, _mm256_broadcast_ss((float *)a1));
    __m256 temp1N = _mm256_mul_ps(prevInN, _mm256_broadcast_ss((float *)a1));
    __m256 temp2 = _mm256_mul_ps(prevOut, _mm256_broadcast_ss((float *)b1));
    __m256 temp2N = _mm256_mul_ps(prevOutN, _mm256_broadcast_ss((float *)b1));
    __m256 temp3 = _mm256_mul_ps(prev2Out, _mm256_broadcast_ss((float *)b2));
    __m256 temp3N = _mm256_mul_ps(prev2OutN, _mm256_broadcast_ss((float *)b2));
    currComp = _mm256_add_ps(currComp, temp1);
    currCompN = _mm256_add_ps(currCompN, temp1N);
    temp2 = _mm256_add_ps(temp2, temp3);
    temp2N = _mm256_add_ps(temp2N, temp3N);
    prev2Out = prevOut;
    prevOut = _mm256_sub_ps(currComp, temp2);
    prev2OutN = prevOutN;
    prevOutN = _mm256_sub_ps(currCompN, temp2N);
    prevIn = currIn;
    prevInN = currInN;
    // store output data
    _mm256_store_ps((float *)(oTemp), prevOut);
    _mm256_store_ps((float *)(oTemp + 8), prevOutN);
    id += 1;
    oTemp += 16;  // move to next element
  }

  // right-to-left horizontal pass
  // reset pointers
  id -= 1;
  od += 4 * height * (width - 1);  // set to last row.
  oTemp -= 16;

  // edge calculations
  prevIn = _mm256_castsi256_ps(
      _mm256_castsi128_si256(_mm_cvtepu8_epi32(*(__m128i *)(id))));
  prevIn = _mm256_insertf128_ps(
      prevIn, _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + Nwidth)))),
      0x1);
  prevIn = _mm256_cvtepi32_ps(_mm256_castps_si256(prevIn));
  prev2Out = _mm256_mul_ps(prevIn, _mm256_broadcast_ss((float *)cnext));
  prevOut = prev2Out;
  currIn = prevIn;

  prevInN = _mm256_castsi256_ps(
      _mm256_castsi128_si256(_mm_cvtepu8_epi32(*(__m128i *)(id + 2 * Nwidth))));
  prevInN = _mm256_insertf128_ps(
      prevInN,
      _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + 3 * Nwidth)))),
      0x01);
  prevInN = _mm256_cvtepi32_ps(_mm256_castps_si256(prevInN));
  prev2OutN = _mm256_mul_ps(prevInN, _mm256_broadcast_ss((float *)cnext));
  prevOutN = prev2OutN;
  currInN = prevInN;

  for (int y = width - 1; y >= 0; y--) {
    // load inputs
    __m256 inNext = _mm256_castsi256_ps(
        _mm256_castsi128_si256(_mm_cvtepu8_epi32(*(__m128i *)(id))));
    inNext = _mm256_insertf128_ps(
        inNext,
        _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + Nwidth)))), 0x1);
    inNext = _mm256_cvtepi32_ps(_mm256_castps_si256(inNext));

    __m256 inNextN = _mm256_castsi256_ps(_mm256_castsi128_si256(
        _mm_cvtepu8_epi32(*(__m128i *)(id + 2 * Nwidth))));
    inNextN = _mm256_insertf128_ps(
        inNextN,
        _mm_castsi128_ps(_mm_cvtepu8_epi32((*(__m128i *)(id + 3 * Nwidth)))),
        0x01);
    inNextN = _mm256_cvtepi32_ps(_mm256_castps_si256(inNextN));
    // get the current output from temp buffer
    __m256 output = _mm256_load_ps((float *)(oTemp));       // Current output
    __m256 outputN = _mm256_load_ps((float *)(oTemp + 8));  // Current output

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
    // add previous pass output
    output = _mm256_add_ps(output, prevOut);

    // next set
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
    // add previous pass output
    outputN = _mm256_add_ps(outputN, prevOutN);

    _mm256_stream_ps((float *)(od), output);
    _mm256_stream_ps((float *)(od + 8), outputN);

    id -= 1;
    od -= 4 * height;
    oTemp -= 16;
  }
  _mm256_zeroupper();
}