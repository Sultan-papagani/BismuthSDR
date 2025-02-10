#pragma once

double cosine(double n, double N, const double* coefs, int coefCount);

inline double freqsamp_to_hz(double freq, double samp) 
{
    return (freq / samp) * 2.0 * 3.14159265358979323846;
}