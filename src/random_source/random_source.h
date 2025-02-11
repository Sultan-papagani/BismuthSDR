#pragma once
#include "sdr_interface.h"
#include "sdr_manager.h"
#include <cstdlib>


class RandomSource : public drawer_interface
{
    public: 
        RandomSource();
        ~RandomSource();
        void init() override;
        void draw() override;
        void destroy() override;
        const char* get_name() override;
};

class random_interface : public sdr_interface{
    public:
        random_interface(){
            
        }

        ~random_interface(){

        }

        bool init(int _device_id, const std::string& _name, int _virtual_id, double _samplerate, double _center_frequency, device_mode _mode) override{
            device_id = _device_id;
            name = _name;
            virtual_id = _virtual_id;
            samplerate = _samplerate;

            center_frequency = _center_frequency;
            mode = _mode;
        
            sample_count = 11776; //(int)roundf(samplerate / (200 * 512)) * 512; // 11776 2.4mhz için.

            sdr_manager::register_sdr(std::move(this));
            streams.push_back(&buffer);

            return true;
        }

        #pragma region Getters
        const char* get_name() override{
            return name.c_str();
        }

        const std::string& get_std_name() override{
            return name;
        }

        int get_device_id() override{
            return device_id;
        }

        int get_virtual_id() override{
            return virtual_id;
        }

        double get_samplerate() override{
            return samplerate;
        }

        double get_center_frequency() override{
            return center_frequency;
        }

        device_mode get_mode() override{
            return mode;
        }

        int get_gain(){
            return this->gains[gain_id];
        }

        #pragma endregion

        #pragma region Setters

        void set_virtual_id(int _virtual_id) override{
            virtual_id = _virtual_id;
        }

        void set_name(const char* _name) override{
            name = _name;
        }

        void set_samplerate(double _samplerate) override{
            samplerate = _samplerate;
            sample_count = (int)roundf(samplerate / (200 * 512)) * 512;
        }


        void set_center_frequency(double _center_frequency) override{
            center_frequency = _center_frequency;
        }

        void set_mode(device_mode _mode) override{
            mode = _mode;
        }

        void configure_gain(){
        }

        void set_gain(){
            
        }

        void set_gain(int index){
            gain_id = index;
           
        }

        void set_tuner_agc(bool state){
            tuner_agc = state;
            
        }

        void set_tuner_agc(){
           
        }

        #pragma endregion

        void start() override{
            this->workerThread = std::thread(&random_interface::setup_async, this);
        }

        void stop() override{
            running_async = false;
            if (workerThread.joinable()) {workerThread.join(); }
        }

        void destroy() override{
            running_async = false;
            if (workerThread.joinable()) {workerThread.join(); }
        }

        void cancel_async(){
            running_async = false;
            if (workerThread.joinable()) {workerThread.join(); }
        }

        void setup_async()
        {
            running_async = true;
            while (running_async)
            {
                unsigned char buffer[sample_count];
                for (int i=0; i<sample_count; i++){
                    buffer[i] = rand() % 255;
                }
                actualHandler(buffer, sample_count);
            }
        }

        void actualHandler(unsigned char* buf, uint32_t len) {
            int sampCount = len / 2;
            for (int i = 0; i < sampCount; i++) {
                buffer.input[i].re = ((float)buf[i * 2] - 127.4) / 128.0f;
                buffer.input[i].im = ((float)buf[(i * 2) + 1] - 127.4) / 128.0f;
            }
            buffer.swap(sampCount);
        }

        int gains[256];
        bool running_async = false;
        int max_gain_id = 0;
        int sample_rate_id = 7;
        int gain_id = 0;
        bool tuner_agc = false;
        bool bullshit_agc = false;
        int sample_count = 0;
        std::thread workerThread;
        double_buffer<complex> buffer{"random"}; // bunu streame kaydetmeden önce rename yapmam lazım
};