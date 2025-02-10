#include "sdr_manager.h"
#include <bits/algorithmfwd.h>

namespace sdr_manager
{
    std::vector<sdr_interface*> sdr_list;
    std::vector<std::pair<void(*)(void*),void*>> new_sdr_added_event;

    // set
    bool register_sdr(sdr_interface* sdr){
        sdr_list.push_back(sdr);
        // call the event listeners
        for (const auto& listeners : new_sdr_added_event){
            listeners.first(listeners.second);
        }
        return true;
    }

    bool unregister_sdr(std::string device_name){
        int index = 0;
        bool status = false;
        for (const auto& sdr : sdr_list){
            if(std::strcmp(sdr->get_name(), device_name.c_str()) == 0){
                status = true;
            }
            if (status){
                sdr_list.erase(sdr_list.begin() + index);
                return status;
            }
            index++;
        }
        return status;
    }

    sdr_interface* get_first_sdr(){
        return sdr_list[0];
    }

    int get_sdr_count(){
        return sdr_list.size();
    }

    std::vector<std::string> get_sdr_names(){
        std::vector<std::string> sdr_names;
        for (const auto& sdr : sdr_list){
            sdr_names.push_back(std::string(sdr->get_name()));
        }
        return sdr_names;
    }

    std::vector<std::string> get_stream_names_by_index(int i){
        std::vector<std::string> stream_names;
        for (const auto& stream : sdr_list[i]->streams){
            stream_names.push_back(std::string(stream->name));
        }
        return stream_names;
    }

    int get_stream_count(){
        int count = 0;
        for (const auto& sdr : sdr_list){
            for (const auto& sdr : sdr->streams){
                count++;
            }
        }

        return count;
    }
}