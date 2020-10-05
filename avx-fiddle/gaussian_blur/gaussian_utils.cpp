/*
Reference:
https://software.intel.com/content/dam/develop/external/us/en/documents/gaussian-filter-181134.pdf
https://software.intel.com/content/dam/develop/public/us/en/downloads/gaussian_blur_0311.cpp
*/

#include "gaussian_utils.h"

#include <stdio.h>
#include <stdlib.h>

void writeBMP(unsigned char *image, unsigned char *header, int height,
              int width, int Nwidth, const char *name) {
  // RGB
  int numPixels = height * Nwidth;
  unsigned char *tmp = (unsigned char *)malloc(numPixels * 3);
  int i = 0;
  int k = 0;

  for (int i = 0; i < height; i++) {
    int j = i * Nwidth;
    for (j; j < (i * Nwidth + width); j++, k++) {
      tmp[k * 3] = image[j * 4];
      tmp[k * 3 + 1] = image[j * 4 + 1];
      tmp[k * 3 + 2] = image[j * 4 + 2];
    }
  }
  FILE *out = fopen(name, "wb");
  if (out == NULL) {
    fprintf(stderr, "Can't open output image %s", name);
  }
  if (fwrite(header, 1, 54, out) != 54) {
    fprintf(stderr, "Can't write header %s", name);
  }

  if (fwrite(tmp, 1, numPixels * 3, out) != numPixels * 3) {
    fprintf(stderr, "Write to the output image %s failed", name);
  }
  fclose(out);
  free(tmp);
}