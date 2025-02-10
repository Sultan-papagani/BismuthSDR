#pragma once
#include "dsp/double_buffer.h"
#include <volk/volk.h>

class rotator{
    public:
        rotator(double_buffer<complex>* input, double offset, double samplerate);
        rotator(double_buffer<complex>* input, double offset);
        ~rotator();

        void run();
        void run(complex* in, complex* out, int len);
        void set_offset(double offset);
        void set_offset(double offset, double samplerate);
        void set_input(double_buffer<complex>* input);

        double_buffer<complex>* input;

        lv_32fc_t phase;
        lv_32fc_t phaseDelta;
};