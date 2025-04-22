#pragma once
#include <math.h>
#include "dsp/double_buffer.h"
#include "rtl-sdr.h"
#include "sdr_interface.h"
#include "logger.h"
#include "sdr_manager.h"

const double sample_rate_list[] = {
    250000,
    1024000,
    1536000,
    1792000,
    1920000,
    2048000,
    2160000,
    2400000,
    2560000,
    2880000,
    3200000
};

const char* sample_rate_text = "250KHz\0 1.024MHz\0 1.536MHz\0 1.792MHz\0 1.92MHz\0 2.048MHz\0 2.16MHz\0 2.4MHz\0 2.56MHz\0 2.88MHz\0 3.2MHz\0";


class rtlsdr_interface : public sdr_interface{
    public:
        rtlsdr_interface(){
            
        }

        ~rtlsdr_interface(){

        }

        bool init(int _device_id, const std::string& _name, int _virtual_id, double _samplerate, double _center_frequency, device_mode _mode) override{
            device_id = _device_id;
            name = _name;
            virtual_id = _virtual_id;
            samplerate = _samplerate;

            center_frequency = _center_frequency;
            mode = _mode;

            int status = rtlsdr_open(&openDev, device_id);
            if (status < 0)
            {
                running = false;
                Logger::log(LogL::DANGER, "[rtlsdr] device cant found id:",status);
                return false;
            }

            rtlsdr_get_tuner_gains(openDev, gains);
            rtlsdr_set_tuner_gain_mode(openDev, 1);
            rtlsdr_set_tuner_gain(openDev, gains[7]);
            rtlsdr_set_sample_rate(openDev, samplerate);
            rtlsdr_set_center_freq(openDev, center_frequency);
        
            sample_count = 11776;//(int)roundf(samplerate / (200 * 512)) * 512; // 11776 2.4mhz için.

            sdr_manager::register_sdr(std::move(this));
            streams.push_back(&buffer);

            freq = 90000000;
            writer = 0;
            rtlsdr_set_center_freq(openDev, freq);

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
            if (running_async){
                cancel_async();
                rtlsdr_set_sample_rate(openDev, samplerate);
                start();
            }else{
                rtlsdr_set_sample_rate(openDev, samplerate);
            }
        }

        void set_samplerate(){
            samplerate = sample_rate_list[sample_rate_id];
            //sample_count = (int)roundf(samplerate / (200 * 512)) * 512;
            if (running_async){
                cancel_async();
                rtlsdr_set_sample_rate(openDev, samplerate);
                start();
            }else{
                rtlsdr_set_sample_rate(openDev, samplerate);
            }
        }

        void set_center_frequency(double _center_frequency) override{
            center_frequency = _center_frequency;
            rtlsdr_set_center_freq(openDev, center_frequency);
        }

        void set_mode(device_mode _mode) override{
            mode = _mode;
        }

        void configure_gain(){
            max_gain_id = rtlsdr_get_tuner_gains(openDev, gains);
        }

        void set_gain(){
            rtlsdr_set_tuner_gain(openDev, gains[gain_id]);
        }

        void set_gain(int index){
            gain_id = index;
            rtlsdr_set_tuner_gain(openDev, gains[index]);
        }

        void set_tuner_agc(bool state){
            tuner_agc = state;
            rtlsdr_set_tuner_gain_mode(openDev, !tuner_agc); 
        }

        void set_tuner_agc(){
            rtlsdr_set_tuner_gain_mode(openDev, !tuner_agc);
        }

        void set_rtl_agc(bool state){
            bullshit_agc = state;
            rtlsdr_set_agc_mode(openDev, bullshit_agc);
        }

        void set_rtl_agc(){
            rtlsdr_set_agc_mode(openDev, bullshit_agc);
        }

        #pragma endregion

        void start() override{
            this->workerThread = std::thread(&rtlsdr_interface::setup_async, this);
        }

        bool getCompleteFrame() override
        {
            
            if (freq < 104000000){
                if (rtlsdr_get_center_freq(openDev) != freq){return false;}
                unsigned char buff[sample_count + 1];
                int nread;
                rtlsdr_read_sync(openDev, &buff, sample_count, &nread);
                for (int i = writer; i < sample_count / 2; i++) {
                    buffer.input[i].re = ((float)buff[i * 2] - 127.4) / 128.0f;
                    buffer.input[i].im = ((float)buff[(i * 2) + 1] - 127.4) / 128.0f;
                }
                freq += samplerate;
                rtlsdr_set_center_freq(openDev, freq);
                writer += sample_count;
                std::cout<<"writer: "<<writer<<" freq: "<<freq/1000000.0<<std::endl;
                return false;
            }else
            {
                writer = 0;
                freq = 90000000;
                rtlsdr_set_center_freq(openDev, freq);
                buffer.swap(writer);
                return true;
            }
            
        }

        void stop() override{
            running_async = false;
            rtlsdr_cancel_async(openDev);
            if (workerThread.joinable()) {workerThread.join(); }
        }

        void destroy() override{
            running_async = false;
            rtlsdr_cancel_async(openDev); 
            rtlsdr_close(openDev);
            if (workerThread.joinable()) {workerThread.join(); }
        }

        void cancel_async(){
            running_async = false;
            rtlsdr_cancel_async(openDev);
            if (workerThread.joinable()) {workerThread.join(); }
        }

        void setup_async()
        {
            running_async = true;
            rtlsdr_reset_buffer(openDev);
            rtlsdr_read_async(openDev, asyncHandler, this, 0, sample_count);
        }

        static void asyncHandler(unsigned char* buf, uint32_t len, void* ctx) {
            rtlsdr_interface* _self = (rtlsdr_interface*)ctx;
            _self->actualHandler(buf, len);
        }

        void actualHandler(unsigned char* buf, uint32_t len) {
            int sampCount = len / 2;
            for (int i = 0; i < sampCount; i++) {
                buffer.input[i].re = ((float)buf[i * 2] - 127.4) / 128.0f;
                buffer.input[i].im = ((float)buf[(i * 2) + 1] - 127.4) / 128.0f;
            }
            buffer.swap(sampCount);
        }

        // testing
        double freq;
        int writer;

        int gains[256];
        bool running_async = false;
        int max_gain_id = 0;
        int sample_rate_id = 7;
        int gain_id = 0;
        bool tuner_agc = false;
        bool bullshit_agc = false;
        rtlsdr_dev_t* openDev;
        int sample_count = 0;
        std::thread workerThread;
        double_buffer<complex> buffer{"rtlsdr", 200000}; // bunu streame kaydetmeden önce rename yapmam lazım
        
};
