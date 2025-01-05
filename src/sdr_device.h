#pragma once

enum device_mode
{
    Receive,
    Transmit,
    BOTH
};

class sdr_device
{
    public:
        sdr_device(const char* _name, int _ID, double _samplerate, double _bandwidth, double _center_frequency, device_mode _mode);
        ~sdr_device();

        // get
        const char* get_name();
        int get_id();
        double get_samplerate();
        double get_bandwidth();
        double get_center_frequency();
        device_mode get_mode();


        // set
        bool set_name(const char* _name);
        bool set_id(int _id);
        bool set_samplerate(double _samplerate);
        bool set_bandwidth(double _bandwidth);
        bool set_center_frequency(double _center_frequency);
        bool set_mode(device_mode _mode);
        

    private:
        const char* name;
        int ID;
        double samplerate;
        double bandwidth;
        double center_frequency;
        device_mode mode;

};
