#include <chrono>
#include <cstring>
#include "waterfall.h"
#include "implot.h"
#include "sdr_manager.h"
#include "global_helper.h"
#include "main_page.h"
#include "asset_manager.h"

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

waterfall::waterfall()
{
    data_size_X = 16384;
    data_size_Y = 500;
    min_db = -140;
    max_db = 0;

    strcpy(name, generate_random_string(10));

    window_name.reserve(window_name.size() + 11);
    window_name.append(name);

    updatePallette(DEFAULT_COLOR_MAP, 13);
    glGenTextures(1, &imageID);

    selected_mode_text = modes_array[selected_mode];

    selected_fft_text = fft_sizes_text[selected_fft_index];

    psd_output = (float*)calloc(data_size_X, sizeof(float));
    waterfall_buffer = (uint32_t*)calloc(data_size_X * data_size_Y, sizeof(uint32_t));

    psd_converter = new complex_to_psd(selected_source, psd_output, data_size_X);
}

void waterfall::init() {

}

void waterfall::destroy() {
    
}

const char* waterfall::get_name() {
    return name;
}

waterfall::~waterfall()
{
    free(psd_output);
    free(waterfall_buffer);
};

void waterfall::change_data_size(int new_x, int new_y)
{
    if (new_y == 0){
        new_y = data_size_Y;
    }else{
        data_size_Y = new_y;
    }
    data_size_X = new_x;
    //free(waterfall_buffer);
    free(psd_output);
    waterfall_buffer = (uint32_t*)_recalloc(waterfall_buffer,new_x * new_y, sizeof(uint32_t));
    psd_output = (float*)calloc(new_x, sizeof(float));

    psd_converter->change_output(psd_output);
    psd_converter->resize_converter(new_x);

};

void waterfall::change_mode()
{
    selected_mode_text = modes_array[selected_mode];
}

void waterfall::draw()
{

    if (ImGui::CollapsingHeader(window_name.c_str())){
        ImGui::PushID(name);
        ImGui::Checkbox("Reverse FFT", &psd_converter->reverse);
        ImGui::Checkbox("Smooth FFT", &psd_converter->smooth_fft);
        if (psd_converter->smooth_fft)
        {
            ImGui::Text("Smoothing");ImGui::SameLine();
            if (ImGui::SliderFloat("##", &psd_converter->fftSmoothingSpeed, 0.0, 1.0))
            {
                psd_converter->change_smoothing();
            }
        }
        if (ImGui::Button("Destory")){
            remove_drawer_later(name);
        }
        ImGui::Text("Noise Floor: %f", noise_floor);
        ImGui::Text("Baseband dbFS: %f", max_value);
        if (ImGui::SliderFloat("size", &size, 0, 6))
        {

        }
        ImGui::PopID();
    }

    ImGui::Begin(window_name.c_str(), NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) 
    {
        if (ImGui::BeginMenu("Streams"))
        {
            if (ImGui::MenuItem("Refresh List"))
            {
                sdr_list = sdr_manager::get_sdr_names();
            }
            int index = 0;
            for (const auto& name : sdr_list) 
            {
                if (ImGui::BeginMenu(name.c_str())) 
                {
                    std::vector<std::string> stream_names = sdr_manager::get_stream_names_by_index(index);
                    int index_stream = 0;
                    for (const auto& stream_name : stream_names) 
                    {
                        if (ImGui::MenuItem(stream_name.c_str())) 
                        {
                            running = true;
                            selected_source = sdr_manager::sdr_list[index]->streams[index_stream];
                            psd_converter->change_input(selected_source);
                        }
                        index_stream++;
                    }
                    ImGui::EndMenu();
                } 
                index++;
            }
        ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(selected_fft_text))
        {
            if (ImGui::MenuItem("1024")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[0]; change_data_size(1024);}
            if (ImGui::MenuItem("2048")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[1]; change_data_size(2048);}
            if (ImGui::MenuItem("4096")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[2]; change_data_size(4096);}
            if (ImGui::MenuItem("8192")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[3]; change_data_size(8192);}
            if (ImGui::MenuItem("16384")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[4]; change_data_size(16384);}
            if (ImGui::MenuItem("32768")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[5]; change_data_size(32768);}
            if (ImGui::MenuItem("65536")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[6]; change_data_size(65536);}
            if (ImGui::MenuItem("131072")){selected_fft_index = 0;selected_fft_text=fft_sizes_text[7]; change_data_size(131072);}
            ImGui::EndMenu();
        }

        if (ImGui::SmallButton("Nuttal"))
        {
            
        }

        if (ImGui::BeginMenu(selected_mode_text))
        {
            if(ImGui::MenuItem("HF")){selected_mode = 0;selected_mode_text = modes_array[0];}
            if(ImGui::MenuItem("VHF")){selected_mode = 1;selected_mode_text = modes_array[1];}
            if(ImGui::MenuItem("UHF")){selected_mode = 2;selected_mode_text = modes_array[2];}
            if(ImGui::MenuItem("SHF")){selected_mode = 3;selected_mode_text = modes_array[3];}
            ImGui::EndMenu();
        }

        for (int i=0; i<mode_bands_len[selected_mode]; i++){
            if (ImGui::BeginMenu(modes_bands[selected_mode][i])){
                ImGui::EndMenu();
            }
        }


        if (ImGui::SmallButton(running ? "|| Pause": "> Start"))
        {
            if ( selected_source != nullptr){
                running = !running;
            }
        }
        
        ImGui::EndMenuBar();
    }

    if (running){
        psd_converter->run();
    }

    /*ImGui::BeginChild("##zoombar", ImVec2(0,0), ImGuiChildFlags_ResizeX);
    ImGui::Text("Zoom");
    ImGui::VSliderFloat("##zoom",ImVec2(25,130), &zoom, -120.0f, 0.0f);
    ImGui::Text("Min DB");
    ImGui::VSliderFloat("##mindb",ImVec2(25,130), &min_db, -120.0f, 0.0f);
    ImGui::Text("Max DB");
    ImGui::VSliderFloat("##maxdb",ImVec2(25,130), &max_db, -120.0f, 0.0f);
    ImGui::EndChild();
    ImGui::SameLine();*/

    //ImPlot::ShowDemoWindow();


    ImGui::BeginChild("##psd_display", ImVec2(0,0), ImGuiChildFlags_ResizeY);

    // AMONGUS
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    ImVec2 space_max = ImGui::GetContentRegionAvail();
    ImVec2 window_pos = ImGui::GetWindowPos();
    window_pos.x += space_max.x - 60;
    window_pos.y += 140;


    unsigned int target;
    volk_32f_index_max_32u(&target, psd_output, data_size_X);
    max_value = psd_output[target];
    volk_32f_s32f_calc_spectral_noise_floor_32f(&noise_floor, psd_output, 0, data_size_X);

    draw_list->PushClipRectFullScreen();
    draw_list->PathArcTo(ImVec2(window_pos.x + 40*0.5f*size, window_pos.y + 12*0.5f*size), 214*0.5f*size, 3.141592f, 3.141592f * 1.5f);
    draw_list->PathStroke(IM_COL32(255,255,255,255), ImDrawFlags_None, 3);
    draw_list->AddLine(ImVec2(window_pos.x + 40*0.5f*size, window_pos.y + 12*0.5f*size), ImVec2(window_pos.x + size*40*0.5f - 30, window_pos.y-120*size + 12*0.5f*size), IM_COL32(255,0,10,255));
    draw_list->PopClipRect();

    // AMONGUS

    ImGui::SetCursorPosY(10);
    ImVec2 fft_space = ImGui::GetContentRegionAvail();
    fft_space.x -= 10;
    if (ImPlot::BeginPlot("##waterfall", fft_space, ImPlotFlags_CanvasOnly | ImPlotFlags_NoFrame))
    {
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::SetupAxesLimits(0,data_size_X,min_db,max_db, ImPlotCond_Always);
        ImPlot::PlotLine("##", psd_output, data_size_X);
        ImPlot::PlotShaded("##", psd_output, data_size_X, min_db);
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
    ImGui::EndChild();

    // draw freq
    //ImVec2 tempPos = ImGui::GetCursorPos();

    /*ImGui::SetCursorPos(beginPosForFreq);
    ImGui::PushFont(asset_manager::font_big);
    ImGui::Text("92.000.000.000Hz");
    ImGui::PopFont();asas
    ImGui::SetCursorPos(tempPos);*/

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

void waterfall::updatePallette(float colors[][3], int colorCount) {
    
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
