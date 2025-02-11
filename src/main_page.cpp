#include <vector>
#include <memory>
#include <cstring>
#include "logger.h"
#include "main_page.h"
#include "imgui.h"
#include "drawer_interface.h"
#include "rtlsdr/rtlsdr.h"

#include "waterfall.h"
#include "implot.h"
#include <random>

#include "raw_waterfall.h"
#include "sdr_manager.h"
#include "radio.h"

#include "random_source/random_source.h"

std::vector<std::unique_ptr<drawer_interface>> drawers;

const char* delete_drawer_name = nullptr;

raw_waterfall waterfall_test;
radio _radio;

void init_main_page(){
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

    // DEBUG için

    auto source = std::make_unique<RtlSdrSource>();
    source->init();
    drawers.push_back(std::move(source));

    /*auto source_2 = std::make_unique<waterfall>();
    source_2->init();
    drawers.push_back(std::move(source_2));*/

    //waterfall_test.selected_source = sdr_manager::get_first_sdr()->streams[0];

    _radio.init();
}

bool drawer_exists(const char* name){
    for (const auto& drawer_ptr : drawers) {
        if (std::strcmp(drawer_ptr->get_name(), name) == 0){return true;}
    }
    return false;
}

void remove_drawer_later(const char* name)
{
    delete_drawer_name = name;
}

void remove_drawer(const char* name){
    int index = 0;
    bool found = false;
    for (const auto& drawer_ptr : drawers) {
        if (std::strcmp(drawer_ptr->get_name(), name) == 0){
            // bu.
            found = true;
            break;
        }
        index++;
    }
    if (found){
        std::cout<<"drawer silindi: "<<name<<" at index: "<<index<<std::endl;
        drawers.erase(drawers.begin() + index); 
    }
}

template <typename T>
void button_behaviour(const char* name, bool multiple = false){
    if(ImGui::MenuItem(name)){
        if (!multiple && drawer_exists(name)){
            Logger::log(LogL::DEBUG, name, "source already exists");
        }else{
            Logger::log(LogL::DEBUG, name, "source added");
            auto source = std::make_unique<T>();
            source->init();
            drawers.push_back(std::move(source));
        }
    }
}

void render_main_page(int w, int h){
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),  ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::Begin("Modules", NULL, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()){
        if (ImGui::BeginMenu("Add Source")){
            button_behaviour<RtlSdrSource>("rtl-sdr");
            button_behaviour<RandomSource>("random-source");
            ImGui::MenuItem("Airspy");
            ImGui::MenuItem("Hackrf");
            ImGui::MenuItem("LimeSDR");
            ImGui::MenuItem("Serial Input");
            ImGui::MenuItem("Audio Source");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add Module")){
            button_behaviour<waterfall>("waterfall", true);
            button_behaviour<radio>("radio", false);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    _radio.draw();

    if (delete_drawer_name != nullptr){
        remove_drawer(delete_drawer_name);
        delete_drawer_name = nullptr;
    }
    
    for (const auto& drawer_ptr : drawers) {
        drawer_ptr->draw();
    }



    //waterfall_test.draw();

    ImGui::End();// modül kutusu bitişi
}

void exit_main_page(){
    //destroy all drawers
    for (const auto& drawer_ptr : drawers) {
        drawer_ptr->destroy();
    }
}

