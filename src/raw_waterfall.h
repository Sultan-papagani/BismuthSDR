#pragma once

#include <stdint.h>
#include <cstdlib>
#include <GL/gl.h> 
#include <math.h>
#include <algorithm>
#include <iostream>
#include <thread>

#include "imgui.h"
#include "dsp/complex_to_psd.h"
#include "drawer_interface.h"

#include "dsp/rotator.h"
#include "dsp/decimating_fir.h"
#include "dsp/fft_window.h"


#define WATERFALL_RESOLUTION 1000000

class raw_waterfall : public drawer_interface
{
    public:
        raw_waterfall();
        ~raw_waterfall();

        void set_fps(int _fps);
        void change_data_size(int new_x, int new_y = 0);
        void updatePallette(float colors[][3], int colorCount);
        void updateSource(float* newSource);

        void init() override;
        void draw() override;
        void destroy() override;
        const char* get_name() override;

        double_buffer<complex>* selected_source = nullptr;
        double_buffer<complex> work_buffer{"work"};
        bool running = false;
        float* psd_output;
    private:

        uint32_t waterfallPallet[WATERFALL_RESOLUTION];

        int data_size_X;
        int data_size_Y;

        float min_db;
        float max_db;

        std::string window_name = "Waterfall##";

        rotator* _rotator;
        double rotation;

        int decim = 1;
        decimating_fir* fir;

        char name[30];
        complex_to_psd* psd_converter;
        uint32_t* waterfall_buffer;

        GLuint imageID;

};