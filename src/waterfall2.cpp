#include "waterfall2.h"

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

waterfallYeni::waterfallYeni(const char* _name, int _ownerID, float* _waterfall_data, int _data_size_X, int _data_size_Y, int _min_db, int _max_db)
{
    name = _name;
    ownerID = _ownerID;
    waterfall_data = _waterfall_data;
    data_size_X = _data_size_X;
    data_size_Y = _data_size_Y;
    data_size_Y_UI = _data_size_Y;
    min_db = _min_db;
    max_db = _max_db;

    updatePallette(DEFAULT_COLOR_MAP, 13);
    glGenTextures(1, &imageID);

    waterfall_buffer = (uint32_t*)malloc(sizeof(uint32_t*) * data_size_X * data_size_Y);
};

waterfallYeni::~waterfallYeni()
{
    free(waterfall_buffer);
};

void waterfallYeni::change_data_size(int new_x, int new_y)
{
    free(waterfall_buffer);
    waterfall_buffer = (uint32_t*)malloc(sizeof(uint32_t) * new_x * new_y);
    memset(waterfall_buffer, 90, sizeof(uint32_t) * new_x * new_y);
};

void waterfallYeni::render(int size_x, int _min_db, int _max_db)
{
    min_db = _min_db;
    max_db = _max_db;

    ImGui::Begin(name, NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
    ImVec2 total_space = ImGui::GetContentRegionAvail();
    if (ImGui::BeginMenuBar()){
        if (ImGui::BeginMenu("Settings")){
            ImGui::Checkbox("Height same as window", &window_fill);
            if (window_fill){
                data_size_Y_UI = total_space.y;
            }else{
                ImGui::InputInt("Height: ", &data_size_Y_UI, 100);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (size_x != data_size_X || data_size_Y_UI != data_size_Y){
        data_size_X = size_x;
        data_size_Y = data_size_Y_UI;
        change_data_size(size_x, data_size_Y);
    }

    //waterfalla veri ekle
    float dataRange = max_db - min_db;
    for (int i=0; i<data_size_X; i++){
        float pixel = (std::clamp<float>(waterfall_data[i], min_db, max_db) - min_db) / dataRange;
        int id = (int)(pixel * (WATERFALL_RESOLUTION - 1));
        if(id > WATERFALL_RESOLUTION - 1){id = WATERFALL_RESOLUTION - 1;}
        if(id < 0){id = 0;}
        waterfall_buffer[i] = waterfallPallet[id];
    }
    
    // ekranı kaydır
    memmove(&waterfall_buffer[data_size_X], waterfall_buffer, (data_size_X * (data_size_Y - 1)) * sizeof(uint32_t));
    //glGenTextures(1, &imageID); // ilki 1 idi
    glBindTexture(GL_TEXTURE_2D, imageID);
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_size_X , data_size_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)waterfall_buffer);

    ImGui::Image((ImTextureID)(intptr_t)imageID, total_space);
    ImGui::End();
};

void waterfallYeni::updatePallette(float colors[][3], int colorCount) {
    
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