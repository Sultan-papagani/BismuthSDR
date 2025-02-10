#pragma once
#include <volk/volk.h>
#include "double_buffer.h"
#include "dsp_math.h"

class fft_window
{
    public: 

        fft_window(double_buffer<complex>* _input,int _fft_size)
        {
            fft_size = _fft_size;
            input = _input;

            // FFT window
            fftWindow = (float*)malloc(sizeof(float) * fft_size);
            const double coefs[] = { 0.54, 0.46 };
            for (int i = 0; i < fft_size; i++) { fftWindow[i] = cosine(i, fft_size, coefs, sizeof(coefs) / sizeof(double))*(1.0f * ((i % 2) ? -1.0f : 1.0f)); }
        }

        ~fft_window()
        {
            free(fftWindow);
        }

        void reconfigure(double_buffer<complex>* _input,int _fft_size)
        {
            fft_size = _fft_size;
            input = _input;

            // FFT window
            fftWindow = (float*)malloc(sizeof(float) * fft_size);
            const double coefs[] = { 0.54, 0.46 };
            for (int i = 0; i < fft_size; i++) { fftWindow[i] = cosine(i, fft_size, coefs, sizeof(coefs) / sizeof(double))*(1.0f * ((i % 2) ? -1.0f : 1.0f)); }
        }

        void resize(int _fft_size)
        {
            free(fftWindow);
            fft_size = _fft_size;

            fftWindow = (float*)malloc(sizeof(float) * fft_size);
            const double coefs[] = { 0.54, 0.46 };
            for (int i = 0; i < fft_size; i++) { fftWindow[i] = cosine(i, fft_size, coefs, sizeof(coefs) / sizeof(double))*(1.0f * ((i % 2) ? -1.0f : 1.0f)); }
        }

        void change_input(double_buffer<complex>* _input)
        {
            input = _input;
        }

        void run()
        {
            int len = input->read();
            volk_32fc_32f_multiply_32fc((lv_32fc_t*)input->output, (lv_32fc_t*)input->output, fftWindow, fft_size);
            input->flush();
        }

        int fft_size;
        float* fftWindow;
        double_buffer<complex>* input;

};