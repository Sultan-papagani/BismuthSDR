#pragma once
#include <volk/volk.h>
#include "double_buffer.h"

class decimating_fir
{
    public:

        decimating_fir(double_buffer<complex>* _input, int _tap_count, const float* _taps, int _decim)
        {   
            selected_input = _input;
            tap_count = _tap_count;
            taps = _taps;
            decim = _decim;
        }

        ~decimating_fir()
        {

        }

        void reconfigure(double_buffer<complex>* _input, int _tap_count, const float* _taps, int _decim)
        {
            selected_input = _input;
            tap_count = _tap_count;
            taps = _taps;
            decim = _decim;
        }

        void change_input(double_buffer<complex>* _input)
        {
            selected_input = _input;
        }

        void change_taps(int _tap_count, const float* _taps)
        {
            tap_count = _tap_count;
            taps = _taps;
        }

        void change_decim(int _decim)
        {
            decim = _decim;
        }

        void run()
        {
            int len = selected_input->read();
            int first_index = 0;
            for (int _offset = 0; _offset < len; _offset += decim) 
            {
                volk_32fc_x2_dot_prod_32fc((lv_32fc_t*)&selected_input->output[first_index++], (lv_32fc_t*)&selected_input->output[_offset], (lv_32fc_t*)taps, tap_count / 2);
            }
            selected_input->flush(first_index);
        }

        int run(float* input, float* output, int len)
        {
            int first_index = 0;
            for (int i=0; i<len; i+= decim)
            {
                volk_32f_x2_dot_prod_32f(&output[first_index++], &input[i], taps, tap_count);
            }
            return len / decim;
        }

        double_buffer<complex>* selected_input;
        int tap_count;
        const float* taps;
        int decim;
};