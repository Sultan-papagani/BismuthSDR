# 0 "C:/Users/sultan/Desktop/BismuthSDR/src/sdr_interface.cpp"
# 1 "C:\\Users\\sultan\\Desktop\\BismuthSDR\\build//"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "C:/Users/sultan/Desktop/BismuthSDR/src/sdr_interface.cpp"
# 1 "C:/Users/sultan/Desktop/BismuthSDR/src/sdr_interface.h" 1
       

enum device_mode
{
    Receive,
    Transmit,
    BOTH
};

class sdr_interface
{
    public:
        ~sdr_interface();


        virtual void init();
        virtual const char* get_name();
        virtual int get_id();
        virtual double get_samplerate();
        virtual double get_bandwidth();
        virtual double get_center_frequency();
        virtual device_mode get_mode();



        virtual bool set_name(const char* _name);
        virtual bool set_id(int _id);
        virtual bool set_samplerate(double _samplerate);
        virtual bool set_bandwidth(double _bandwidth);
        virtual bool set_center_frequency(double _center_frequency);
        virtual bool set_mode(device_mode _mode);


    private:
        const char* name;
        int ID;
        double samplerate;
        double bandwidth;
        double center_frequency;
        device_mode mode;

};
# 2 "C:/Users/sultan/Desktop/BismuthSDR/src/sdr_interface.cpp" 2

sdr_interface::sdr_interface(const char* _name, int _ID, double _samplerate, double _bandwidth, double _center_frequency, device_mode _mode)
{
    name = _name;
    samplerate = _samplerate;
    bandwidth = _bandwidth;
    center_frequency = _center_frequency;
    mode = _mode;
    ID = _ID;
}

sdr_interface::~sdr_interface()
{

}



const char* sdr_interface::get_name() {return name;}
int sdr_interface::get_id() {return ID;}
double sdr_interface::get_samplerate() {return samplerate;}
double sdr_interface::get_bandwidth() {return bandwidth;}
double sdr_interface::get_center_frequency() {return center_frequency;}
device_mode sdr_interface::get_mode() {return mode;}



bool sdr_interface::set_name(const char* _name) {name = _name; return true;}
bool sdr_interface::set_id(int _id) {ID = _id; return true;}
bool sdr_interface::set_samplerate(double _samplerate) {samplerate = _samplerate; return true;}
bool sdr_interface::set_bandwidth(double _bandwidth) {bandwidth = _bandwidth; return true;}
bool sdr_interface::set_center_frequency(double _center_frequency) {center_frequency = _center_frequency; return true;}
bool sdr_interface::set_mode(device_mode _mode) {mode = _mode; return true;}
