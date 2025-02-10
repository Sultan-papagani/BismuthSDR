#pragma once
#include "double_buffer.h"

class resize
{
    public:
        resize(double_buffer<complex>* _input, int _output_size)
        {
            input = _input;
            output_size = _output_size;
        }

        ~resize()
        {
            
        }

        void reconfigure(double_buffer<complex>* _input, int _output_size)
        {
            input = _input;
            output_size = _output_size;
        }

        void change_input(double_buffer<complex>* _input)
        {
            input = _input;
        }

        void change_size(int _output_size)
        {
            output_size = _output_size;
        }

        // INOP
        void run(int size)
        {
            return;
        }

    double_buffer<complex>* input;
    int output_size;
};