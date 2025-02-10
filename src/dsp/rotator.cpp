#include "rotator.h"
#include "dsp/dsp_math.h"

rotator::rotator(double_buffer<complex>* _input, double offset, double samplerate){
    input = _input;
    offset = freqsamp_to_hz(offset, samplerate);
    phase = lv_cmake(1.0f, 0.0f);
    phaseDelta = lv_cmake(cos(offset), sin(offset));
}

rotator::rotator(double_buffer<complex>* _input, double offset){
    input = _input;
    phase = lv_cmake(1.0f, 0.0f);
    phaseDelta = lv_cmake(cos(offset), sin(offset));
}

rotator::~rotator(){

}

void rotator::run()
{
    int len = input->read();
    volk_32fc_s32fc_x2_rotator2_32fc((lv_32fc_t*)input->output, (lv_32fc_t*)input->output, &phaseDelta, &phase, input->buffer_size);
    input->flush();
}

void rotator::run(complex* in, complex* out, int len)
{
    volk_32fc_s32fc_x2_rotator2_32fc((lv_32fc_t*)out, (lv_32fc_t*)in, &phaseDelta, &phase, len);
}

void rotator::set_offset(double offset)
{
    phase = lv_cmake(1.0f, 0.0f);
    phaseDelta = lv_cmake(cos(offset), sin(offset));
}

void rotator::set_offset(double offset, double samplerate)
{
    offset = freqsamp_to_hz(offset, samplerate);
    phase = lv_cmake(1.0f, 0.0f);
    phaseDelta = lv_cmake(cos(offset), sin(offset));
}

void rotator::set_input(double_buffer<complex>* _input){
    input = _input;
}