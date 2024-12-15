#include "rtlsdr/rtl-sdr.h"
#include <stdio.h>
#include <math.h>
#include <thread>
#include <iostream>
#include <malloc.h>
#include <vector>
#include <mutex>
#include "imgui/imgui.h"
#include <fftw3.h>
#include "complex.h"
#include <cassert>
#include <volk/volk.h>

rtlsdr_dev_t* openDev;
int sample_count;
std::thread workerThread;

std::mutex buffer_mutex;

float* fftWindowBuf;
float* outputDB;
float* smoothingBuf;
complex* data;
fftwf_complex* in;
fftwf_complex* out;
fftwf_plan plan;

int gains[256];
int gain_id;
int selected_bw_index;

float min_db, max_db;

//UI
int fft_rate;
int fftSmoothingSpeed;

//UI olmayan
float fftSmoothingAlpha;
float fftSmoothingBeta;

bool reverse;
bool smooth_fft;

int tiktak = 0;

const char* bw_texts = "1.2MHZ\0 2.4MHz";

static void asyncHandler(unsigned char* buf, uint32_t len, void* ctx) {
    int sampCount = len / 2;
    //std::lock_guard<std::mutex> buffer_lock(buffer_mutex);
    for (int i = 0; i < sampCount; i++) {
        data[i].re = ((float)buf[i * 2] - 127.4) / 128.0f;
        data[i].im = ((float)buf[(i * 2) + 1] - 127.4) / 128.0f;
    }
    std::cout << sampCount << std::endl;
}
void setup_async()
{
    rtlsdr_reset_buffer(openDev);
    rtlsdr_read_async(openDev, asyncHandler, nullptr, 0, sample_count);
}

void calculateSmoothing()
{
    fftSmoothingAlpha = std::min<float>(fftSmoothingSpeed / (fft_rate * 10.0f), 1.0f);
    fftSmoothingBeta = 1 - fftSmoothingAlpha;
}

void configure_rtlsdr(int sample_rate, int gain_index, double frequency)
{

    int oret = rtlsdr_open(&openDev, 0);
    if (oret < 0)
    {
        printf("ATTACK THE D POINT!\nI REFUSE!\nATTACK THE D POINT!\nNO!\nTHANK YOU!\n");
    }

    min_db = -100.0f;
    max_db = 0.0f;

    int n = rtlsdr_get_tuner_gains(openDev, gains);
    rtlsdr_set_tuner_gain_mode(openDev, 1);
    rtlsdr_set_tuner_gain(openDev, gains[7]);

    rtlsdr_set_sample_rate(openDev, sample_rate);
    rtlsdr_set_center_freq(openDev, frequency);

    sample_count = (int)roundf(sample_rate / (200 * 512)) * 512;

    fft_rate = 20;
    fftSmoothingSpeed = 100;

    fftSmoothingAlpha = std::min<float>(fftSmoothingSpeed / (fft_rate * 10.0f), 1.0f);
    fftSmoothingBeta = 1 - fftSmoothingAlpha;

    fftWindowBuf = (float*)malloc(sizeof(float) * sample_count);
    for (int i = 0; i < sample_count; i++) { fftWindowBuf[i] = 1.0f * ((i % 2) ? -1.0f : 1.0f); }
    
    outputDB = (float*)malloc(sizeof(float) * sample_count);
    smoothingBuf = (float*)malloc(sizeof(float) * sample_count);

    for (int i = 0; i < sample_count; i++) {
        smoothingBuf[i] = -1000.0f; 
    }

    data = (complex*)fftwf_malloc(sizeof(complex) * sample_count);
    in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * sample_count);
    out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * sample_count);

    plan = fftwf_plan_dft_1d(sample_count, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    workerThread = std::thread(&setup_async);
}

void draw_UI(int w, int h)
{
    //std::lock_guard<std::mutex> buffer_lock(buffer_mutex);

    tiktak += 1;

    if (reverse){
        // reverse the fft if user wants
        volk_32fc_conjugate_32fc((lv_32fc_t*)data, (lv_32fc_t*)data, sample_count);
    }

    // hamming window
    volk_32fc_32f_multiply_32fc((lv_32fc_t*)in, (lv_32fc_t*)data, fftWindowBuf, sample_count);
    // do DFT
    fftwf_execute(plan);
    // convert to PSD
    volk_32fc_s32f_power_spectrum_32f(outputDB, (lv_32fc_t*)out, sample_count, sample_count);


    if (smooth_fft){
        volk_32f_s32f_multiply_32f(outputDB, outputDB, fftSmoothingAlpha, sample_count);
        volk_32f_s32f_multiply_32f(smoothingBuf, smoothingBuf, fftSmoothingBeta, sample_count);
        volk_32f_x2_add_32f(smoothingBuf, outputDB, smoothingBuf, sample_count);
        memcpy(outputDB, smoothingBuf, sample_count * sizeof(float));
    }

    ImGui::Columns(2, "##bruh", false);
    int w2 = std::round((w * 30) / 100);
    ImGui::SetColumnWidth(0, w2);
    ImGui::SetColumnWidth(1, w-w2);
    ImGui::Combo("Bandwith##rtlsdr_bw", &selected_bw_index, bw_texts);
    if (ImGui::SliderInt("Gain##rtlsdr_gain", &gain_id, 0, 15, "%d'th gain")){
        rtlsdr_set_tuner_gain(openDev, gains[gain_id]);
    }
    ImGui::SliderFloat("Min DB##rtlsdr_mindb", &min_db, -150.0f, 0.0f, "%.3fDB");
    ImGui::SliderFloat("Max DB##rtlsdr_maxdb", &max_db, -100.0f, 0.0f, "%.3fDB");
    ImGui::Checkbox("Reverse FFT##rtlsdr_reverse", &reverse);
    ImGui::Checkbox("FFT Smoothing##rtlsdr_smooth", &smooth_fft);
    if (ImGui::InputInt("FFT Smoothing speed##fft_smoothing_speed", &fftSmoothingSpeed)) {
            fftSmoothingSpeed = std::max<int>(fftSmoothingSpeed, 1);
            calculateSmoothing();
    }
    if (ImGui::InputInt("FFT Rate##fft_rate", &fft_rate, 1, 10)) {
            fft_rate = std::max<int>(1, fft_rate);
            calculateSmoothing();
    }
    ImGui::NextColumn();
    // display dB in ui
    ImVec2 eski_pos = ImGui::GetCursorPos();
    ImVec2 size = ImGui::CalcTextSize("XXDB");
    int step_size = h / (15 - 1);
    int count_of_text = round(h / size.y);
    int r = min_db;
    for (int i=0; i<15; i++){
        ImGui::Text(std::to_string(r).append("dB").c_str());
        r -= 10;
        ImVec2 a = ImGui::GetCursorPos();
        a.y += step_size;
        ImGui::SetCursorPos(a);
    }
    ImGui::SetCursorPos(eski_pos);
    ImGui::PlotLines("##plot_sdr", outputDB, sample_count, 0, NULL, min_db, max_db, ImGui::GetContentRegionAvail());
    ImGui::Columns(2);
}


void stop()
{
    rtlsdr_cancel_async(openDev);
    if (workerThread.joinable()) {workerThread.join(); }
}

