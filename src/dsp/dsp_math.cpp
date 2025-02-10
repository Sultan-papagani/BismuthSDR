#include "math.h"
#include "dsp_math.h"

double cosine(double n, double N, const double* coefs, int coefCount) {
    double win = 0.0;
    double sign = 1.0;
    for (int i = 0; i < coefCount; i++) {
        win += sign * coefs[i] * cos((double)i * 2.0 * 3.14f * n / N);
        sign = -sign;
    }
    return win;
};
