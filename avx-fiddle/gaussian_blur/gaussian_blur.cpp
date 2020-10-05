/*
Reference:
https://software.intel.com/content/dam/develop/external/us/en/documents/gaussian-filter-181134.pdf
https://software.intel.com/content/dam/develop/public/us/en/downloads/gaussian_blur_0311.cpp
*/

#include "gaussian_blur.h"

#include <immintrin.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int num_thread;

// coefficients for gaussian filter
float a0 = 0, a1 = 0, a2 = 0, a3 = 0, b1 = 0, b2 = 0, cprev = 0, cnext = 0;

// Main IIR Gaussian Blur Filter
int main(int argc, char *argv[]) {
  FILE *in;
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned int numPixels = 0;
  unsigned char header[54];
  float sigma;
  int order = 0;
  unsigned int *img0m;
  float *img1AVXm;
  unsigned int *img2AVXm;

  // aligned pointers
  unsigned int *img0;
  float *img1AVX;
  unsigned int *img2AVX;

  int test = 1;
  int status = 0;

  if (argc != 2) {
    perror("Usage: gaussian_blur <image_name>");
    exit(1);
  }

  in = fopen(argv[1], "rb");
  if (in == NULL) {
    perror("Could not open image");
    return 1;
  }

  if (fread(header, 1, 54, in) != 54) {
    perror("Could not read image header");
    return 2;
  }

  width = abs(*(short *)&header[18]);
  height = abs(*(short *)&header[22]);

  if ((width & 7) || (height & 7)) {
    printf("Only images with width and height a multiple of 8 are supported\n");
    return 3;
  }
  // Increase image/intermediate image width to avoid cache thrash by atlease
  // one cacheline
  // New image width -Nwidth
  int Nwidth = width + 32;
  int buffWidth = Nwidth * height;
  numPixels = width * height;

  img0m = (unsigned int *)malloc((buffWidth * 4) + (height * 64));
  img1AVXm = (float *)malloc((buffWidth * 4 * sizeof(float)) + (height * 64));
  img2AVXm = (unsigned int *)malloc((buffWidth * 4) + (height * 64));

  img0 = (unsigned int *)(((long long)img0m + 64) + ~(63));
  img1AVX = (float *)(((long long)img1AVXm + 64) + ~(63));
  img2AVX = (unsigned int *)(((long long)img2AVXm + 64) + ~(63));

  // Temp buffer initializtion, used inbetween passes.
  int buffer_size_each_thread =
      width * 4 *
      4;  // as image is symmetric width=height, this calculation works,
          // otherwise for each pass there should be separate temp buffer.
  float *oTempm = (float *)malloc(
      buffer_size_each_thread * num_thread * sizeof(float) +
      64);  // temp buffer to store data between passes - 16- 4 threads
  float *oTemp = (float *)(((long long)oTempm + 64) + ~(63));

  unsigned char *tmp = (unsigned char *)malloc(numPixels * 3);
  if (fread(tmp, 1, numPixels * 3, in) != numPixels * 3) {
    perror("can't read input image");
    return 4;
  }
  fclose(in);

  // copy the RGB values from the image, pad the 4th byte with zero
  // so we get RGBA
  unsigned char *img0byte = (unsigned char *)img0;
  // copy image to the buffer
  unsigned int i = 0;
  unsigned int k = 0;
  for (unsigned int j = 0; j < height; j++) {
    k = j * (width);
    for (i; i < (j * (Nwidth) + width); i++, k++) {
      img0byte[i * 4] = tmp[k * 3];
      img0byte[i * 4 + 1] = tmp[k * 3 + 1];
      img0byte[i * 4 + 2] = tmp[k * 3 + 2];
      img0byte[i * 4 + 3] = 0;
    }
    for (; i < (j * (Nwidth) + Nwidth); i++) {
      img0byte[i * 4] = 0;
      img0byte[i * 4 + 1] = 0;
      img0byte[i * 4 + 2] = 0;
      img0byte[i * 4 + 3] = 0;
    }

  }  // for loop copy image to the buffer

  header[18] = (short)width;
  writeBMP((unsigned char *)img0, header, height, width, Nwidth,
           "original Image.bmp");
  free(tmp);

  // Calculate IIR Gaussian Filter coefficeints
  sigma = 20.0f;
  calGaussianCoeff(sigma, &a0, &a1, &a2, &a3, &b1, &b2, &cprev, &cnext);
  printf("\n IIR Gaussian Filter Coefficients are: \n");
  printf(
      "a0 = %f, a1 = %f, a2 = %f, a3 = %f, b1 = %f, b2 = %f, cprev = %f, cnext "
      "= %f\n",
      a0, a1, a2, a3, b1, b2, cprev, cnext);

  printf("\nimage width = %d, height = %d\n", width, height);

  // AVX implementation
  _mm256_zeroupper();
#pragma omp parallel for
  for (int i = 0; i < height; i += 4) {
    int tidx = omp_get_thread_num();
    gaussianHorizontal_avx(oTemp + tidx * buffer_size_each_thread,
                           &img0[i * Nwidth], &img1AVX[4 * i], width, height,
                           Nwidth, &a0, &a1, &a2, &a3, &b1, &b2, &cprev,
                           &cnext);
  }

#pragma omp parallel for
  for (int i = 0; i < width; i += 4) {
    int tidx = omp_get_thread_num();
    gaussianVertical_avx(oTemp + tidx * buffer_size_each_thread,
                         &img1AVX[4 * i * height], &img2AVX[i], height, Nwidth,
                         &a0, &a1, &a2, &a3, &b1, &b2, &cprev, &cnext);
  }

  writeBMP((unsigned char *)img2AVX, header, height, width, Nwidth,
           "blur_AVX.bmp");

  free(oTempm);
  free(img0m);
  free(img1AVXm);
  free(img2AVXm);
  fflush(stdout);
}
