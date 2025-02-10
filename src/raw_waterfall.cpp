#include <chrono>
#include <cstring>
#include "raw_waterfall.h"
#include "implot.h"
#include "global_helper.h"
#include "main_page.h"

#include <fftw3.h>
#include <volk/volk.h>

#include "logger.h"

const unsigned int fir_16_8_len = 72;
const float fir_16_8_taps[72] = {
0.000003688211243,
0.000013660745184,
0.000027864244875,
0.000056024595736,
0.000099948148808,
0.000168552197041,
0.000269382284025,
0.000413325712010,
0.000612084815649,
0.000879405538662,
0.001230104825804,
0.001680092905716,
0.002245658820654,
0.002942957882659,
0.003787212583021,
0.004791922493325,
0.005967969572298,
0.007322738536994,
0.008859256322921,
0.010575423925694,
0.012463379308760,
0.014509046778473,
0.016691912242846,
0.018985060458242,
0.021355495646189,
0.023764754477139,
0.026169804021396,
0.028524201614471,
0.030779477612194,
0.032886687871454,
0.034798070863046,
0.036468735790737,
0.037858303447209,
0.038932421304764,
0.039664078609141,
0.040034655893826,
0.040034655893826,
0.039664078609141,
0.038932421304764,
0.037858303447209,
0.036468735790737,
0.034798070863046,
0.032886687871454,
0.030779477612194,
0.028524201614471,
0.026169804021396,
0.023764754477139,
0.021355495646189,
0.018985060458242,
0.016691912242846,
0.014509046778473,
0.012463379308760,
0.010575423925694,
0.008859256322921,
0.007322738536994,
0.005967969572298,
0.004791922493325,
0.003787212583021,
0.002942957882659,
0.002245658820654,
0.001680092905716,
0.001230104825804,
0.000879405538662,
0.000612084815649,
0.000413325712010,
0.000269382284025,
0.000168552197041,
0.000099948148808,
0.000056024595736,
0.000027864244875,
0.000013660745184,
0.000003688211243,
};

raw_waterfall::raw_waterfall()
{
    data_size_X = 8000;
    data_size_Y = 500;
    min_db = -160;
    max_db = 0;

    strcpy(name, generate_random_string(10));

    window_name.reserve(window_name.size() + 11);
    window_name.append(name);

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

    updatePallette(DEFAULT_COLOR_MAP, 13);
    glGenTextures(1, &imageID);

    psd_output = (float*)calloc(data_size_X, sizeof(float));
    waterfall_buffer = (uint32_t*)calloc(data_size_X * data_size_Y, sizeof(uint32_t));

    _rotator = new rotator(&work_buffer, rotation, 2400000);
    psd_converter = new complex_to_psd(&work_buffer, psd_output, data_size_X);
    fir = new decimating_fir(&work_buffer, fir_16_8_len, fir_16_8_taps, decim);

}

void raw_waterfall::init() {

}

void raw_waterfall::destroy() {
    
}

const char* raw_waterfall::get_name() {
    return name;
}

raw_waterfall::~raw_waterfall()
{
    free(psd_output);
    free(waterfall_buffer);
};

void raw_waterfall::change_data_size(int new_x, int new_y)
{
    if (new_y == 0){
        new_y = data_size_Y;
    }else{
        data_size_Y = new_y;
    }
    data_size_X = new_x;

    free(waterfall_buffer);
    free(psd_output);

    waterfall_buffer = (uint32_t*)calloc(new_x * new_y, sizeof(uint32_t));
    psd_output = (float*)calloc(new_x, sizeof(float));

    psd_converter->resize_converter(new_x);
    psd_converter->change_output(psd_output);
};

void raw_waterfall::draw()
{

    if (ImGui::CollapsingHeader(window_name.c_str())){
        ImGui::PushID(name);
        if (ImGui::Button("Destory Waterfall")){
            remove_drawer_later(name);
        }
        if (ImGui::InputDouble("Rotator Offset", &rotation, 10.0, 50.0)){
            _rotator->set_offset(rotation, 2400000);
        }
        if (ImGui::SliderInt("FIR Decim", &decim, 1, 100)){
            if (decim % 2 == 0 || decim == 1){
                std::cout<<decim<<std::endl;
                fir->decim = decim;
            }
        }
        ImGui::PopID();
    }

    ImGui::Begin(window_name.c_str(), NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) 
    {
        if (ImGui::SmallButton(running ? "|| Pause": "> Start"))
        {
            if ( selected_source != nullptr){
                running = !running;
            }
        }
        ImGui::EndMenuBar();
    }

    if (running){

        int len = selected_source->read();
        memcpy(work_buffer.input, selected_source->output, sizeof(complex) * len);
        selected_source->flush();
        work_buffer.swap(len);

        _rotator->run();
        fir->run();
        if (data_size_X != work_buffer.buffer_size){
            Logger::log(LogL::WARNING, "[RW] new FFT size:");
            change_data_size(work_buffer.buffer_size);
        }
        psd_converter->run();
    }


    ImGui::BeginChild("##psd_display", ImVec2(0,0), ImGuiChildFlags_ResizeY);
    ImGui::SetCursorPosY(10);
    ImVec2 fft_space = ImGui::GetContentRegionAvail();
    fft_space.x -= 10;
    if (ImPlot::BeginPlot("##waterfall", fft_space, ImPlotFlags_CanvasOnly | ImPlotFlags_NoFrame))
    {
        ImPlot::SetupAxesLimits(0,data_size_X,min_db,max_db, ImPlotCond_Always);
        ImPlot::PlotLine("##", psd_output, data_size_X);
        ImPlot::EndPlot();
    }
    ImGui::EndChild();

    ImGui::BeginChild("##waterfall_display", ImVec2(0,0));
    ImVec2 total_space = ImGui::GetContentRegionAvail();

    //push_data
    if (running){
        float dataRange = max_db - min_db;
        for (int i=0; i<data_size_X; i++){
            float pixel = (std::clamp<float>(psd_output[i], min_db, max_db) - min_db) / dataRange;
            int id = (int)(pixel * (WATERFALL_RESOLUTION - 1));
            if(id > WATERFALL_RESOLUTION - 1){id = WATERFALL_RESOLUTION - 1;}
            if(id < 0){id = 0;}
            waterfall_buffer[i] = waterfallPallet[id];
            }
        memmove(&waterfall_buffer[data_size_X], waterfall_buffer, (data_size_X * (data_size_Y - 1)) * sizeof(uint32_t));
    }
    
    glBindTexture(GL_TEXTURE_2D, imageID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_size_X , data_size_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)waterfall_buffer);

    total_space.x -= 43;
    ImGui::SetCursorPosX(33);
    ImGui::Image((ImTextureID)(intptr_t)imageID, total_space);
    ImGui::EndChild();

    ImGui::End();
};

void raw_waterfall::updatePallette(float colors[][3], int colorCount) {
    
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
