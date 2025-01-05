#include "rtl-sdr.h"
#include <stdio.h>
#include <math.h>
#include <cmath>
#include <thread>
#include <iostream>
#include <malloc.h>
#include <vector>
#include <mutex>
#include <imgui.h>
#include <fftw3.h>
#include <cassert>
#include <volk/volk.h>
#include <implot.h>
#include <GL/gl.h> //platform spesific. see sdrpp
#include <algorithm>
#include "dsp/complex.h"
#include <format>

#include "waterfall2.h"
#include "decimator.h"
#include "dsp/buffer.h"

rtlsdr_dev_t* openDev;
int sample_count;
std::thread workerThread;

float* fftWindowBuf;
float* outputDB;
float* outputDB_temp;
float* smoothingBuf;
fftwf_complex* in;
fftwf_complex* out;
fftwf_plan plan;

int gains[256];
int gain_id;

float min_db, max_db;

//UI
int fft_rate;
int fftSmoothingSpeed;

//UI olmayan
float fftSmoothingAlpha;
float fftSmoothingBeta;

bool reverse;
bool flip_fft;
bool smooth_fft = false;
bool running = true;


double _frequency;
int _samplerate;
int decim_no = 1;


waterfallYeni* rtlsdr_waterfall;

buffer rtlsdr_data;

int lastSampCount;

static void asyncHandler(unsigned char* buf, uint32_t len, void* ctx) {
    int sampCount = len / 2;
    {
        for (int i = 0; i < sampCount; i++) {
            rtlsdr_data._input[i].re = ((float)buf[i * 2] - 127.4) / 128.0f;
            rtlsdr_data._input[i].im = ((float)buf[(i * 2) + 1] - 127.4) / 128.0f;
        }
        //debug
        if (sampCount != lastSampCount)
        {
            lastSampCount = sampCount;
            std::cout<<sampCount<<" ama gereken: "<<sample_count<<std::endl;
        }
        rtlsdr_data.swap(len);
    
    }
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

double cosine(double n, double N, const double* coefs, int coefCount) {
        double win = 0.0;
        double sign = 1.0;
        for (int i = 0; i < coefCount; i++) {
            win += sign * coefs[i] * cos((double)i * 2.0 * 3.14f * n / N);
            sign = -sign;
        }
        return win;
}

void rtlsdr_configure(int sample_rate, int gain_index, double frequency)
{

    int oret = rtlsdr_open(&openDev, 0);
    if (oret < 0)
    {
        running = false;
        printf("ATTACK THE D POINT!\nI REFUSE!\nATTACK THE D POINT!\nNO!\nTHANK YOU!\n");
    }

    _frequency = frequency;
    _samplerate = sample_rate;

    min_db = -120.0f;
    max_db = 0.0f;

    rtlsdr_get_tuner_gains(openDev, gains);
    rtlsdr_set_tuner_gain_mode(openDev, 1);
    rtlsdr_set_tuner_gain(openDev, gains[7]);
    rtlsdr_set_sample_rate(openDev, sample_rate);
    rtlsdr_set_center_freq(openDev, frequency);

    sample_count = (int)roundf(sample_rate / (200 * 512)) * 512;
    lastSampCount = sample_count;

    fft_rate = 50;
    fftSmoothingSpeed = 50;

    fftSmoothingAlpha = std::min<float>(fftSmoothingSpeed / (fft_rate * 10.0f), 1.0f);
    fftSmoothingBeta = 1 - fftSmoothingAlpha;

    fftWindowBuf = (float*)malloc(sizeof(float) * sample_count);
    const double coefs[] = { 0.54, 0.46 };
    for (int i = 0; i < sample_count; i++) { fftWindowBuf[i] = cosine(i, sample_count, coefs, sizeof(coefs) / sizeof(double))*(1.0f * ((i % 2) ? -1.0f : 1.0f)); }
    
    outputDB = (float*)malloc(sizeof(float) * sample_count);
    smoothingBuf = (float*)malloc(sizeof(float) * sample_count);


    for (int i = 0; i < sample_count; i++) {
        smoothingBuf[i] = -100.0f; 
    }

    in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * sample_count);
    out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * sample_count);
    plan = fftwf_plan_dft_1d(sample_count, in, out, FFTW_FORWARD, FFTW_ESTIMATE);


    ImPlotStyle& style = ImPlot::GetStyle();
    ImVec4* colors = style.Colors;
    style.LineWeight       = 1;
    style.Marker           = ImPlotMarker_None;
    style.MarkerSize       = 4;
    style.MarkerWeight     = 1;
    style.FillAlpha        = 1.0f;
    style.ErrorBarSize     = 5;
    style.ErrorBarWeight   = 1.5f;
    style.DigitalBitHeight = 8;
    style.DigitalBitGap    = 4;
    style.PlotBorderSize   = 0;
    style.MinorAlpha       = 1.0f;
    style.MajorTickLen     = ImVec2(0,0);
    style.MinorTickLen     = ImVec2(0,0);
    style.MajorTickSize    = ImVec2(0,0);
    style.MinorTickSize    = ImVec2(0,0);
    style.MajorGridSize    = ImVec2(1.2f,1.2f);
    style.MinorGridSize    = ImVec2(1.2f,1.2f);
    style.PlotPadding      = ImVec2(0,0);
    style.LabelPadding     = ImVec2(5,5);
    style.LegendPadding    = ImVec2(5,5);
    style.MousePosPadding  = ImVec2(5,5);
    style.PlotMinSize      = ImVec2(300,225);

    rtlsdr_waterfall = new waterfallYeni("rtlsdr waterfall 1", 1, outputDB, sample_count, 700, min_db, max_db);
    workerThread = std::thread(&setup_async);
}


void rtlsdr_render_ui(int w, int h)
{
    // dsp kodu
    int bufSize = rtlsdr_data.read();
    {

        if (reverse){
            volk_32fc_conjugate_32fc((lv_32fc_t*)rtlsdr_data._output, (lv_32fc_t*)rtlsdr_data._output, bufSize);
        }

        volk_32fc_32f_multiply_32fc((lv_32fc_t*)in, (lv_32fc_t*)rtlsdr_data._output, fftWindowBuf, bufSize);
        fftwf_execute(plan);
        volk_32fc_s32f_power_spectrum_32f(outputDB, (lv_32fc_t*)out, bufSize, bufSize);

        
        bufSize = decimate(outputDB, bufSize, decim_no);


        if (smooth_fft){
            volk_32f_s32f_multiply_32f(outputDB, outputDB, fftSmoothingAlpha, bufSize);
            volk_32f_s32f_multiply_32f(smoothingBuf, smoothingBuf, fftSmoothingBeta, bufSize);
            volk_32f_x2_add_32f(smoothingBuf, outputDB, smoothingBuf, bufSize);
            memcpy(outputDB, smoothingBuf, bufSize * sizeof(float));
        }

        if (flip_fft){
            volk_32f_s32f_multiply_32f(outputDB, outputDB, -1.0f, bufSize);
        }
  
    }

    // modul başlangıc kutusu
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),  ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::Begin("Modules", NULL);

    // rtlsdr'ın plugin kutusu
    if (ImGui::CollapsingHeader("RTL-SDR", ImGuiTreeNodeFlags_DefaultOpen)){
    
        if (ImGui::SliderInt("Gain##rtlsdr_gain", &gain_id, 0, 15, "%d'th gain")){ 
            rtlsdr_set_tuner_gain(openDev, gains[gain_id]);
        }

        ImGui::Checkbox("Reverse FFT##rtlsdr_reverse", &reverse);
        ImGui::Checkbox("Flip FFT##rtlsdr_reverse", &flip_fft);
        ImGui::Checkbox("FFT Smoothing##rtlsdr_smooth", &smooth_fft);
        if (ImGui::InputInt("FFT Smoothing speed##fft_smoothing_speed", &fftSmoothingSpeed)) {
                fftSmoothingSpeed = std::max<int>(fftSmoothingSpeed, 1);
                calculateSmoothing();
        }
        if (ImGui::InputInt("FFT Rate##fft_rate", &fft_rate, 1, 10)) {
                fft_rate = std::max<int>(1, fft_rate);
                calculateSmoothing();
        }
        ImGui::SliderInt("Decimate##rtlsdr_decim", &decim_no, 1, 128, "decim by:%d");

        float max = -99999999;
        for (int i=0; i<bufSize; i++)
        {
                if(outputDB[i] > max) {max = outputDB[i];}
        }
        ImGui::Text(std::format("baseband: {:.2f}dbFS", max).c_str());
        ImGui::End();
    }

    // PSD display
    ImGui::Begin("PSD", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    if (ImPlot::BeginPlot("##waterfall", ImGui::GetContentRegionAvail(), ImPlotFlags_CanvasOnly | ImPlotFlags_NoFrame))
    {
        ImPlot::SetupAxes("##","##",ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_NoSideSwitch, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_NoSideSwitch);
        ImPlot::SetupAxesLimits(0,bufSize,min_db,max_db, ImPlotCond_Always);
        ImPlot::PlotLine("AAA", outputDB, bufSize);
        ImPlot::EndPlot();
    }
    
    // tune
    if(ImGui::IsWindowHovered() && running)
    {
        ImGuiIO& io = ImGui::GetIO();
        _frequency += round(io.MouseWheel * 50000);
        while(rtlsdr_get_center_freq(openDev) != _frequency && running){
            rtlsdr_set_center_freq(openDev, _frequency);
        }
    }
    
    // modül kutusu bitişi
    ImGui::End();

    rtlsdr_waterfall->render(bufSize, min_db, max_db);
    rtlsdr_data.imDone();

    //kontrol barı ama waterfalla taşı
    ImGui::Begin("Controls", NULL);
    ImGui::Text("Zoom");
    ImGui::VSliderFloat("##rtlsdr_zoom",ImVec2(25,130), &min_db, -120.0f, 0.0f);
    ImGui::Text("Min DB");
    ImGui::VSliderFloat("##rtlsdr_mindb",ImVec2(25,130), &min_db, -120.0f, 0.0f);
    ImGui::Text("Max DB");
    ImGui::VSliderFloat("##rtlsdr_maxdb",ImVec2(25,130), &max_db, -120.0f, 0.0f);
    ImGui::End();
}


void rtlsdr_stop()
{
    rtlsdr_cancel_async(openDev);
    rtlsdr_close(openDev);
    if (workerThread.joinable()) {workerThread.join(); }
    free(outputDB);
    free(fftWindowBuf);
    free(smoothingBuf);
    fftwf_free(in);
    fftwf_free(out);

}

