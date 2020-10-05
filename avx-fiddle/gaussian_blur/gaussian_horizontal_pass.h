#pragma once
void gaussianHorizontal_avx(float *oTemp, unsigned int *id, float *od,
                            int width, int height, int Nwidth, float *a0,
                            float *a1, float *a2, float *a3, float *b1,
                            float *b2, float *cprev, float *cnext);