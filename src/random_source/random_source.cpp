#include <iostream>
#include <vector>
#include "logger.h"
#include "imgui.h"
#include "drawer_interface.h"
#include "random_source.h"

int device_count_random;
std::vector<std::unique_ptr<random_interface>> rtlsdr_list_random;

RandomSource::RandomSource(){

}

RandomSource::~RandomSource(){

}

void RandomSource::init(){

    device_count_random = 1;

    destroy();

    rtlsdr_list_random.clear();

    for (int i=0; i<device_count_random; i++)
    {
        const char* sdr_name = "Random Generator";
        auto sdr = std::make_unique<random_interface>();
        std::string device_name = "[" + std::to_string(i) + "]" + sdr_name;
        if (sdr->init(i, device_name, i, 2400000, 92000000, device_mode::Receive)){
            Logger::log(LogL::DEBUG, device_name.c_str(), "is added");
        }else{
            Logger::log(LogL::DEBUG, device_name.c_str(), "cant be added");
            sdr.reset();
            return;
        }
        rtlsdr_list_random.push_back(std::move(sdr));
    }
}

void RandomSource::draw(){
    if (ImGui::CollapsingHeader("Random Source")){
        if (ImGui::Button("Refresh")){
            init();
        }
        for (const auto& sdr : rtlsdr_list_random) {
            const std::string& sdr_name = sdr->get_std_name();
            ImGui::PushID(sdr->get_virtual_id());
            if (ImGui::CollapsingHeader(sdr_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
                if(ImGui::Checkbox("Running", &sdr->running_async)){
                    if (sdr->running_async){
                        sdr->start();
                    }else{
                        sdr->stop();
                    }
                }
                if (!sdr->running_async){
                    if (ImGui::SliderInt("Samplerate", &sdr->sample_count, 50, 20000)){
                        sdr->set_samplerate();
                    }
                }
                if(ImGui::Checkbox("Tuner AGC", &sdr->tuner_agc)){
                    sdr->set_tuner_agc();
                }
                if(ImGui::Checkbox("RTL AGC", &sdr->bullshit_agc)){
                    //sdr->set_rtl_agc();
                }
                if (ImGui::SliderInt("Gain", &sdr->gain_id, 0, 15, "%d'th gain")){ 
                    sdr->set_gain();
                }
            }
            ImGui::PopID();
        }
    }
}

void RandomSource::destroy(){
    for (const auto& sdr : rtlsdr_list_random) {
        Logger::log(LogL::DEBUG, sdr->get_name(), "is destroyed");
        sdr->destroy();
    }
}

const char* RandomSource::get_name(){
    return "random-source";
}

        
