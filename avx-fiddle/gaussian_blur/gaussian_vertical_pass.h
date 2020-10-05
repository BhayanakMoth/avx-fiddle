#pragma once

void gaussianVertical_avx(float *oTemp, float *id, unsigned int *od, int height,
                          int width, float *a0, float *a1, float *a2, float *a3,
                          float *b1, float *b2, float *cprev, float *cnext);