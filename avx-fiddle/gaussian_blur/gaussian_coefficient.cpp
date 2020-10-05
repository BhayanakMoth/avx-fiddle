#include "gaussian_coefficient.h"

#include <math.h>

/* Calcualte Gaussian Blur Filter Coefficiens
 *  alpha -> smooting gradient depends on sigma
 *  k = ((1-e^-alpha)^2)/(1+2*alpha*e^-alpha - e^-2alpha)
 *  a0 = k; a1 = k*(alpha-1)*e^-alpha; a2 = k*(alpha+1)*e^-alpha; a3 =
 * -k*e^(-2*alpha) b1 = -2*e^-alpha; b2 = e^(-2*alpha)
 */
void calGaussianCoeff(float sigma, float *a0, float *a1, float *a2, float *a3,
                      float *b1, float *b2, float *cprev, float *cnext) {
  float alpha, lamma, k;
  // defensive check
  if (sigma < 0.5f) sigma = 0.5f;

  alpha = (float)exp((0.726) * (0.726)) / sigma;
  lamma = (float)exp(-alpha);
  *b2 = (float)exp(-2 * alpha);
  k = (1 - lamma) * (1 - lamma) / (1 + 2 * alpha * lamma - (*b2));
  *a0 = k;
  *a1 = k * (alpha - 1) * lamma;
  *a2 = k * (alpha + 1) * lamma;
  *a3 = -k * (*b2);
  *b1 = -2 * lamma;
  *cprev = (*a0 + *a1) / (1 + *b1 + *b2);
  *cnext = (*a2 + *a3) / (1 + *b1 + *b2);
}