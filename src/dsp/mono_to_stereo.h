#pragma once 
#include <volk/volk.h>

class mono_to_stereo
{
    public:

        mono_to_stereo(){};

        ~mono_to_stereo(){};

        int run(float* in, float* out, int count)
        {
            volk_32f_x2_interleave_32fc((lv_32fc_t*)out, in, in, count);
            return count * 2;
        }
};
