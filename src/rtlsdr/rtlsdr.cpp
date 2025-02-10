#include <iostream>
#include <vector>
#include "logger.h"
#include "imgui.h"
#include "drawer_interface.h"
#include "rtlsdr.h"
#include "rtlsdr_interface.h"

int device_count;
std::vector<std::unique_ptr<rtlsdr_interface>> rtlsdr_list;

RtlSdrSource::RtlSdrSource(){

}

RtlSdrSource::~RtlSdrSource(){

}

void RtlSdrSource::init(){

    device_count = rtlsdr_get_device_count();

    destroy();

    rtlsdr_list.clear();

    for (int i=0; i<device_count; i++)
    {
        const char* sdr_name = rtlsdr_get_device_name(i);
        auto sdr = std::make_unique<rtlsdr_interface>();
        std::string device_name = "[" + std::to_string(i) + "]" + sdr_name;
        if (sdr->init(i, device_name, i, 2400000, 92000000, device_mode::Receive)){
            Logger::log(LogL::DEBUG, device_name.c_str(), "is added");
        }else{
            Logger::log(LogL::DEBUG, device_name.c_str(), "cant be added");
            sdr.reset();
            return;
        }
        rtlsdr_list.push_back(std::move(sdr));
    }
}

void RtlSdrSource::draw(){
    if (ImGui::CollapsingHeader("RTL-SDR Source")){
        if (ImGui::Button("Refresh")){
            init();
        }
        for (const auto& sdr : rtlsdr_list) {
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
                    if (ImGui::Combo("Samplerate", &sdr->sample_rate_id, sample_rate_text)){
                        sdr->set_samplerate();
                    }
                }
                if(ImGui::Checkbox("Tuner AGC", &sdr->tuner_agc)){
                    sdr->set_tuner_agc();
                }
                if(ImGui::Checkbox("RTL AGC", &sdr->bullshit_agc)){
                    sdr->set_rtl_agc();
                }
                if (ImGui::SliderInt("Gain", &sdr->gain_id, 0, 15, "%d'th gain")){ 
                    sdr->set_gain();
                }
            }
            ImGui::PopID();
        }
    }
}

void RtlSdrSource::destroy(){
    for (const auto& sdr : rtlsdr_list) {
        Logger::log(LogL::DEBUG, sdr->get_name(), "is destroyed");
        sdr->destroy();
    }
}

const char* RtlSdrSource::get_name(){
    return "rtl-sdr";
}

        
