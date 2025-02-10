#pragma once
#include <vector>
#include <memory>
#include <cstring>
#include "sdr_interface.h"

namespace sdr_manager
{
    // device
    bool register_sdr(sdr_interface* sdr);
    bool unregister_sdr(std::string device_name);
    sdr_interface* get_first_sdr();
    int get_sdr_count();
    std::vector<std::string> get_sdr_names();

    // stream 
    int get_stream_count();
    std::vector<std::string> get_stream_names_by_index(int i);

    extern std::vector<sdr_interface*> sdr_list;
    extern std::vector<std::pair<void(*)(void*),void*>> new_sdr_added_event;
}