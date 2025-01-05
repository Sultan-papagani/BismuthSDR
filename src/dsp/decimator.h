#pragma once

int decimate(float* input, int size ,int decimation)
{
    int ratio = size/decimation;
    for (int i=0; i<ratio; i++)
    {
        input[i] = input[i * decimation];
    }
    return ratio;
}

int decimate_complex(complex* input, int size ,int decimation)
{
    int ratio = size/decimation;
    for (int i=0; i<ratio; i++)
    {
        input[i].re = input[i * decimation].re;
        input[i].im = input[i * decimation].im;
    }
    return ratio;
}

int decimate_fftw(fftwf_complex* input, int size ,int decimation)
{
    int ratio = size/decimation;
    for (int i=0; i<ratio; i++)
    {
        input[i][0] = input[i * decimation][0];
        input[i][1] = input[i * decimation][1];
    }
    return ratio;
}