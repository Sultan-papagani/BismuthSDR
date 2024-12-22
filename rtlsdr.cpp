#include "rtlsdr/rtl-sdr.h"
#include <stdio.h>
#include <math.h>
#include <cmath>
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
#include "imgui/implot.h"
#include <GL/gl.h> //platform spesific. see sdrpp
#include <algorithm>


rtlsdr_dev_t* openDev;
int sample_count;
std::thread workerThread;

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
bool flip_fft;
bool smooth_fft = true;
bool running = true;

GLuint imageID;

#define RTLSDR_SAMPLE 16 * 32 * 512

double _frequency;
int _samplerate;
int _halfsamplerate;

uint32_t* waterfallData;
int old_x, old_y;

#define WATERFALL_RESOLUTION 1000000
uint32_t waterfallPallet[WATERFALL_RESOLUTION];

float DEFAULT_COLOR_MAP[][3] = {
    { 0x00, 0x00, 0x20 },
    { 0x00, 0x00, 0x30 },
    { 0x00, 0x00, 0x50 },
    { 0x00, 0x00, 0x91 },
    { 0x1E, 0x90, 0xFF },
    { 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0x00 },
    { 0xFE, 0x6D, 0x16 },
    { 0xFF, 0x00, 0x00 },
    { 0xC6, 0x00, 0x00 },
    { 0x9F, 0x00, 0x00 },
    { 0x75, 0x00, 0x00 },
    { 0x4A, 0x00, 0x00 }
};

float mapValue(float inputValue, float inputStart, float inputEnd, float outputStart, float outputEnd) {
    float inputRange = inputEnd - inputStart;
    float outputRange = outputEnd - outputStart;
    return outputStart + (inputValue - inputStart) * (outputRange / inputRange);
}

void updatePallette(float colors[][3], int colorCount) {
    
    for (int i = 0; i < WATERFALL_RESOLUTION; i++) {
        int lowerId = floorf(((float)i / (float)WATERFALL_RESOLUTION) * colorCount);
        int upperId = ceilf(((float)i / (float)WATERFALL_RESOLUTION) * colorCount);
        lowerId = std::clamp<int>(lowerId, 0, colorCount - 1);
        upperId = std::clamp<int>(upperId, 0, colorCount - 1);
        float ratio = (((float)i / (float)WATERFALL_RESOLUTION) * colorCount) - lowerId;
        float r = (colors[lowerId][0] * (1.0 - ratio)) + (colors[upperId][0] * (ratio));
        float g = (colors[lowerId][1] * (1.0 - ratio)) + (colors[upperId][1] * (ratio));
        float b = (colors[lowerId][2] * (1.0 - ratio)) + (colors[upperId][2] * (ratio));
        waterfallPallet[i] = ((uint32_t)255 << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
    }
}

static void asyncHandler(unsigned char* buf, uint32_t len, void* ctx) {
    int sampCount = len / 2;
    //std::lock_guard<std::mutex> buffer_lock(buffer_mutex);
    for (int i = 0; i < sampCount; i++) {
        data[i].re = ((float)buf[i * 2] - 127.4) / 128.0f;
        data[i].im = ((float)buf[(i * 2) + 1] - 127.4) / 128.0f;
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

void configure_rtlsdr(int sample_rate, int gain_index, double frequency)
{

    int oret = rtlsdr_open(&openDev, 0);
    if (oret < 0)
    {
        running = false;
        printf("ATTACK THE D POINT!\nI REFUSE!\nATTACK THE D POINT!\nNO!\nTHANK YOU!\n");
    }

    _frequency = frequency;
    _samplerate = sample_rate;
    _halfsamplerate /= 2;

    glGenTextures(1, &imageID);

    min_db = -100.0f;
    max_db = 0.0f;

    rtlsdr_get_tuner_gains(openDev, gains);
    rtlsdr_set_tuner_gain_mode(openDev, 1);
    rtlsdr_set_tuner_gain(openDev, gains[7]);
    rtlsdr_set_sample_rate(openDev, sample_rate);
    rtlsdr_set_center_freq(openDev, frequency);

    sample_count = (int)roundf(sample_rate / (200 * 512)) * 512;

    fft_rate = 50;
    fftSmoothingSpeed = 50;

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

    updatePallette(DEFAULT_COLOR_MAP, 13);
    printf("oldu");

    workerThread = std::thread(&setup_async);
}


void draw_UI(int w, int h)
{
    //std::lock_guard<std::mutex> buffer_lock(buffer_mutex);

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

    if (flip_fft)
    {
        volk_32f_s32f_multiply_32f(outputDB, outputDB, -1.0f, sample_count);
    }


    //adşkfjasdkşljfşalsd

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),  ImGuiDockNodeFlags_PassthruCentralNode);
    
    ImGui::Begin("Plugins", NULL);
    
    if (ImGui::SliderInt("Gain##rtlsdr_gain", &gain_id, 0, 15, "%d'th gain"))
    {
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
    for (int i=0; i<200; i++)
    {
        ImGui::Text("ATTACK THE D POINT!");
    }

    ImGui::End();

    bool dsf = true;
    ImGui::Begin("PSD", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::MenuItem("Add 10000 items");
            ImGui::MenuItem("Clear items");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    //ImGui::PlotLines("##plot_sdr", outputDB, sample_count, 0, NULL, min_db, max_db, ImGui::GetContentRegionAvail());
    if (running)
    {
        if (ImPlot::BeginPlot("##waterfall", ImVec2(-1,-1), ImPlotFlags_NoInputs | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect))
        {
            //ImPlot::SetupAxes("##","dB",ImPlotAxisFlags_NoTickLabels, 0);
            //ImPlot::SetupAxisFormat(ImAxis_X1, fmtr);
            ImPlot::SetupAxesLimits(0,sample_count,min_db,max_db, ImPlotCond_Always);
            //ImPlot::PlotLine("aaa", bruhData, outputDB, sample_count);
            ImPlot::PlotLine("AAA", outputDB, sample_count);
            ImPlot::EndPlot();
        }
    }
    else
    {
        ImGui::Text("RTL-SDR NOT CONNECTED");
    }
    if(ImGui::IsWindowHovered())
    {
        ImGuiIO& io = ImGui::GetIO();
        _frequency += round(io.MouseWheel * 50000);
        while(rtlsdr_get_center_freq(openDev) != _frequency && running){
            rtlsdr_set_center_freq(openDev, _frequency);
        }
    }
    
    ImGui::End();
  
    ImGui::Begin("Waterfall", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 size = ImGui::GetWindowSize();
    int width = round(size.x);
    int height = round(size.y);

    // boy değiştiyse
    if (width != old_x || height != old_y)
    {
        free(waterfallData);
        waterfallData = (uint32_t*)malloc(sizeof(uint32_t) * width * height);
        memset(waterfallData, 0, sizeof(uint32_t) * width * height);
    }

    //waterfalla veri ekle
    float dataRange = max_db - min_db;
    int oran = round(sample_count / width);
    for (int i=0; i<width; i++)
    {
        float pixel = (std::clamp<float>(outputDB[i*oran], min_db, max_db) - min_db) / dataRange;
        int id = (int)(pixel * (WATERFALL_RESOLUTION - 1));
        waterfallData[i] = waterfallPallet[id];
        
        //waterfallData[i] = ((uint32_t)255 << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
    }

    // ekranı kaydır
    memmove(&waterfallData[width], waterfallData, (width * (height - 1)) * sizeof(uint32_t));

    glGenTextures(1, &imageID);
    glBindTexture(GL_TEXTURE_2D, imageID);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x , size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)waterfallData);

    ImGui::Image((ImTextureID)(intptr_t)imageID, ImVec2(width,height));

    old_x = width;
    old_y = height;
    ImGui::End();

    ImGui::Begin("Controls", NULL);
    //ImGui::VSliderFloat("Zoom##rtlsdr_zoom",ImVec2(25,130), &min_db, -150.0f, 0.0f, "%.3fDB");
    ImGui::VSliderFloat("Min DB##rtlsdr_mindb",ImVec2(25,130), &min_db, -150.0f, 0.0f, "%.3fDB");
    ImGui::VSliderFloat("Max DB##rtlsdr_maxdb",ImVec2(25,130), &max_db, -100.0f, 0.0f, "%.3fDB");
    ImGui::End();


}


void stop()
{
    rtlsdr_cancel_async(openDev);
    rtlsdr_close(openDev);
    if (workerThread.joinable()) {workerThread.join(); }
}

