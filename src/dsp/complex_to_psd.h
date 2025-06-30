#pragma once
#include <fftw3.h>
#include <volk/volk.h>
#include "resize.h"
#include "double_buffer.h"
#include "types.h"
#include "dsp_math.h"

class complex_to_psd
{
    public:
        complex_to_psd(double_buffer<complex>* _input, float* _output, int _psd_size)
        {
            input = _input;
            output = _output;
            psd_size = _psd_size;

            // FFT window
            fftWindow = (float*)malloc(sizeof(float) * psd_size);
            const double coefs[] = { 0.54, 0.46 };
            for (int i = 0; i < psd_size; i++) { fftWindow[i] = cosine(i, psd_size, coefs, sizeof(coefs) / sizeof(double))*(1.0f * ((i % 2) ? -1.0f : 1.0f)); }

            // FFTW malloc
            in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * psd_size);
            out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * psd_size);
            plan = fftwf_plan_dft_1d(psd_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

            // init resizer (inop right now)
            resizer = new resize(input, psd_size);

            smoothingBuf = (float*)malloc(sizeof(float) * psd_size);
            for (int i = 0; i < psd_size; i++) {
                smoothingBuf[i] = -100.0f; 
            }
            fftSmoothingAlpha = fftSmoothingSpeed;
            fftSmoothingBeta = 1 - fftSmoothingAlpha;
        }

        ~complex_to_psd()
        {
            fftw_free(in);
            fftw_free(out);
            free(fftWindow);
            free(smoothingBuf);
        }

        void resize_converter(int _psd_size)
        {
            psd_size = _psd_size;

            // free memory
            free(fftWindow);
            free(smoothingBuf);
            fftwf_free(in);
            fftwf_free(out);
            fftwf_destroy_plan(plan);
            //delete resizer;

            // re-create fft window
            fftWindow = (float*)malloc(sizeof(float) * psd_size);
            const double coefs[] = { 0.54, 0.46 };
            for (int i = 0; i < psd_size; i++) { fftWindow[i] = cosine(i, psd_size, coefs, sizeof(coefs) / sizeof(double))*(1.0f * ((i % 2) ? -1.0f : 1.0f)); }

            // re-allocate memory
            in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * psd_size);
            out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * psd_size);
            plan = fftwf_plan_dft_1d(psd_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

            smoothingBuf = (float*)malloc(sizeof(float) * psd_size);
            for (int i = 0; i < psd_size; i++) {
                smoothingBuf[i] = -100.0f; 
            }

            // resize the resizer :0
            resizer->change_size(psd_size);
        }

        void change_input(double_buffer<complex>* _input)
        {
            input = _input;
            resizer->change_input(input);
        }

        void change_output(float* _output)
        {
            output = _output;
        }

        void change_smoothing()
        {
            fftSmoothingAlpha = fftSmoothingSpeed;
            fftSmoothingBeta = 1 - fftSmoothingAlpha;
        }

        void run(bool apply_window = true) 
        {
            // read the input
            int size = input->read();

            if (reverse){
                volk_32fc_conjugate_32fc((lv_32fc_t*)input->output, (lv_32fc_t*)input->output, psd_size);
            }

            // resize it (INOP)
            resizer->run(size);

            // apply FFT window
            if (apply_window){
                volk_32fc_32f_multiply_32fc((lv_32fc_t*)in, (lv_32fc_t*)input->output, fftWindow, psd_size);
            }else{
                // or not
                memcpy(in, input->output, sizeof(complex) * size);
            }

            // FFT
            fftwf_execute(plan);

            // Calculates the log10 power value for each input point.
            volk_32fc_s32f_power_spectrum_32f(output, (lv_32fc_t*)out, psd_size, psd_size); 

            if (smooth_fft){
                volk_32f_s32f_multiply_32f(output, output, fftSmoothingAlpha, psd_size);
                volk_32f_s32f_multiply_32f(smoothingBuf, smoothingBuf, fftSmoothingBeta, psd_size);
                volk_32f_x2_add_32f(smoothingBuf, output, smoothingBuf, psd_size);
                memcpy(output, smoothingBuf, psd_size * sizeof(float));
            }

            // release buffer
            input->flush();
        }

        bool reverse = false;
        bool smooth_fft = false;
        float fftSmoothingSpeed = 1.0f;

    private:

        int psd_size;
        resize* resizer;
        double_buffer<complex>* input;
        float* output;

        float* smoothingBuf;
        float fftSmoothingAlpha;
        float fftSmoothingBeta;

        float* fftWindow;
        fftwf_complex* in;
        fftwf_complex* out;
        fftwf_plan plan;
};