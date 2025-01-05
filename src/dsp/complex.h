#pragma once

struct complex
{
    complex operator*(const float b) {
        return complex{ re * b, im * b };
    }

    complex operator*(const double b) {
        return complex{ re * (float)b, im * (float)b };
    }

    complex operator/(const float b) {
        return complex{ re / b, im / b };
    }

    complex operator/(const double b) {
        return complex{ re / (float)b, im / (float)b };
    }

    complex operator*(const complex& b) {
        return complex{ (re * b.re) - (im * b.im), (im * b.re) + (re * b.im) };
    }

    complex operator+(const complex& b) {
        return complex{ re + b.re, im + b.im };
    }

    complex operator-(const complex& b) {
        return complex{ re - b.re, im - b.im };
        }

    float re;
    float im;
};