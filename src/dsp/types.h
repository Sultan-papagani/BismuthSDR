#pragma once
#include <math.h>

struct complex
{
    float re;
    float im;

    inline complex conj() {
        return complex{ re, -im };
    }

    inline float phase() {
        return atan2f(im, re);
    }

    inline float amplitude() {
        return sqrt((re * re) + (im * im));
    }

    complex operator*(const complex& b) {
        return complex{ (re * b.re) - (im * b.im), (im * b.re) + (re * b.im) };
    }

};
