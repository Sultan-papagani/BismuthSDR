#include "radio.h"
#include "imgui.h"

radio::radio()
{

}

radio::~radio()
{
    
}

static int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData )
{
    radio* _self = (radio*)userData;
    int i = _self->actualCallback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status, userData);
    return i;
}

float phase_f(float im, float re)
{
    return atan2f(im, re);
}

float phase_f(complex val)
{
    return atan2f(val.im, val.re);
}

complex conjugate(complex val)
{
    return complex{val.re, -val.im};
}


float normalizePhase(float diff) {
    if (diff > FL_M_PI) { diff -= 2.0f * FL_M_PI; }
    else if (diff <= -FL_M_PI) { diff += 2.0f * FL_M_PI; }
    return diff;
}

double hzToRads(double freq, double samplerate) {
        return 2.0 * DB_M_PI * (freq / samplerate);
}


int radio::actualCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData )
{
    unsigned int i;
    float y[4166];

   int len = 4166;
   int overflow = 0;
   int new_reader = reader + 4166;

    // we are behind the writer
    if (reader + 4166 < writer){
        // check for MAX size
        if (reader + 4166 > size){
            len = size - reader; // read to end of it
            overflow = 4166 - len; // and read the rest
            new_reader = overflow; // snap back to zero + overflow 
        }
    }else{
        // we are on the lead (or writer went back to zero)
        if (writer - (4166) < 0){
            // not enough to be behind of it.
            // bu we CAN just send the last samples so you wouldnt hear clicks
            for (int i = 0; i < len; i++) {
                float cphase = phase_f(ring_buffer[reader-4166 + i].im, ring_buffer[reader-4166 + i].re);
                y[i] = normalizePhase(cphase - phase) * invDeviation;
                phase = cphase;
            }
            float *buffer = (float*) outputBuffer;
            for (int i=0; i<nBufferFrames; i++){
                *buffer++ = y[i];
                *buffer++ = y[i];
            }
            // end it
            return 0;
        }else{
            // we can just be behind
            reader = writer - (4166);
            return 0;
        }
    }


    for (int i = 0; i < len; i++) {
        float cphase = phase_f(ring_buffer[reader + i].im, ring_buffer[reader + i].re);
        y[i] = normalizePhase(cphase - phase) * invDeviation;
        phase = cphase;
    }

    if (overflow){
        for (int i = 0; i < overflow; i++) {
            float cphase = phase_f(ring_buffer[i].im, ring_buffer[i].re);
            y[len+i] = normalizePhase(cphase - phase) * invDeviation;
            phase = cphase;
        }
    }

    reader = new_reader;
    stereo_converter.run(y, (float*)outputBuffer, 4166);
    return 0;
}

void radio::init()
{
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    unsigned int sampleRate = 250000;
    unsigned int bufferFrames = 4166; 
    if(dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &audioCallback, (void *)this ))
    {
        std::cout << '\n' << dac.getErrorText() << '\n' << std::endl;
    }

    invDeviation = 1.0;//1.0 / hzToRads(150000 / 2, 150000);

    fftWindow = (float*)malloc(sizeof(float) * 5888);
    const double coefs[] = { 0.54, 0.46 };
    for (int i = 0; i < 5888; i++) { fftWindow[i] = cosine(i, 5888, coefs, sizeof(coefs) / sizeof(double))*(1.0f * ((i % 2) ? -1.0f : 1.0f)); }

    size = 5000000;
    ring_buffer = (complex*)calloc(size, sizeof(complex));
}

void radio::draw(){
    if (ImGui::CollapsingHeader("Radio")){
        if (ImGui::Button("Run"))
        {
            running = true;
            selected_source = sdr_manager::sdr_list[0]->streams[0];
            _rotator = new rotator(&work_buffer, offset, 2400000);
            fir = new decimating_fir(&work_buffer, fir_16_8_len, fir_16_8_taps, 1);

            if ( dac.startStream() ) {
                std::cout << dac.getErrorText() << std::endl;
            }
        }
        if(ImGui::InputDouble("Freq", &offset, 50000, 10000)){
            sdr_manager::sdr_list[0]->set_center_frequency(offset);
        }
    }

    // process data
    if (running)
    {
        int len = selected_source->read();
        if (selected_source->data_ready){
            int overflow = 0;
            int write_now = len;
            if (writer + len > size){
                std::cout<<"overflow"<<std::endl;
                write_now = size - writer;
                overflow = std::abs(write_now - len); 
            }
            memcpy(&ring_buffer[writer], selected_source->output, sizeof(complex) * write_now);
            writer += write_now;
            if (overflow){
                writer = overflow;
                memcpy(ring_buffer, &selected_source->output[write_now], sizeof(complex) * overflow);
            }
            selected_source->data_ready = false;
        }

        selected_source->flush();
    }
}

void radio::destroy(){

}

const char* radio::get_name(){
    return "";
}