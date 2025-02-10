#pragma once
#include <stdint.h>
#include <vector>
#include "dsp/double_buffer.h"

enum device_mode
{
    Receive,
    Transmit,
    BOTH
};

class sdr_interface
{
    public:
        ~sdr_interface() = default;

        // get
        virtual bool init(int _device_id, const std::string& _name, int _virtual_id, double _samplerate, double _center_frequency, device_mode _mode) = 0;
        virtual const char* get_name() = 0;
        virtual const std::string& get_std_name() = 0;
        virtual int get_device_id() = 0;
        virtual int get_virtual_id() = 0;
        virtual double get_samplerate() = 0;
        virtual double get_center_frequency() = 0;
        virtual device_mode get_mode() = 0;

        // set
        virtual void set_virtual_id(int _virtual_id) = 0;
        virtual void set_name(const char* _name) = 0;
        virtual void set_samplerate(double _samplerate) = 0;
        virtual void set_center_frequency(double _center_frequency) = 0;
        virtual void set_mode(device_mode _mode) = 0;

        //toggle
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void destroy() = 0;

        int device_id;
        int virtual_id;
        std::string name;
        double samplerate;
        double center_frequency;
        device_mode mode;
        bool running = false;
        std::vector<double_buffer<complex>*> streams;
};
