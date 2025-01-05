#pragma once

#include <stdint.h>
#include <cstdlib>
#include <GL/gl.h> //platform spesific. see sdrpp
#include <math.h>
#include <algorithm>
#include "imgui.h"
#include <iostream>

#define WATERFALL_RESOLUTION 1000000

class waterfallYeni
{
    public:
        waterfallYeni(const char* _name, int _ownerID, float* _waterfall_data, int _data_size_X, int _data_size_Y, int _min_db, int _max_db);
        ~waterfallYeni();

        void change_data_size(int new_x, int new_y);
        void render(int size_x, int _min_db, int _max_db);
        void updatePallette(float colors[][3], int colorCount);

    private:
        uint32_t* waterfall_buffer;
        uint32_t waterfallPallet[WATERFALL_RESOLUTION];

        bool window_fill;
        int data_size_Y_UI;
        const char* name;
        int ownerID;
        float* waterfall_data;
        int data_size_X;
        int data_size_Y;
        int window_size_x;
        int window_size_y;
        int min_db;
        int max_db;
        GLuint imageID;
};