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

#define WATERFALL_RESOLUTION 1000000

class waterfall : public drawer_interface
{
    public:
        waterfall();
        ~waterfall();

        void set_fps(int _fps);
        void change_data_size(int new_x, int new_y = 0);
        void updatePallette(float colors[][3], int colorCount);
        void updateSource(float* newSource);

        void init() override;
        void draw() override;
        void destroy() override;
        const char* get_name() override;


    private:

        void change_mode();

        bool running = false;

        std::vector<std::string> sdr_list;

        uint32_t waterfallPallet[WATERFALL_RESOLUTION];

        int data_size_X;
        int data_size_Y;

        float zoom;
        float min_db;
        float max_db;

        std::string window_name = "Waterfall##";

        float noise_floor;
        float max_value;

        float size = 1;

        char name[30];
        double_buffer<complex>* selected_source = nullptr;
        complex_to_psd* psd_converter;
        float* psd_output;
        uint32_t* waterfall_buffer;

        GLuint imageID;

        const char* selected_fft_text;
        int selected_fft_index = 3;
        static constexpr const char* fft_sizes_text[] = {"FFT: 1024", "FFT: 2048", "FFT: 4096", "FFT: 8192", "FFT: 16384", "FFT: 32768", "FFT: 65536", "FFT: 131072"};
        static constexpr const int fft_sizes[] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};

        int selected_mode = 0;
        //char selected_mode_text[20];
        const char* selected_mode_text;
        static constexpr const char* modes_array[4] = {"HF Mode","VHF Mode","UHF Mode","SHF Mode"};
        //static constexpr const char* modes = "HF\0 VHF\0 UHF\0 SHF\0";
        static constexpr const int mode_bands_len[4] = {10, 2, 3, 4};
        static constexpr const char* modes_bands[4][10] = {{"160M","80M","40M","30M","20M","17M","15M","12M","10M","CB"},
                                                            {"2M","6M"},
                                                            {"70cm","22cm","13cm"},
                                                            {"10cm","5cm","3cm","1cm"}
                                                            };
};