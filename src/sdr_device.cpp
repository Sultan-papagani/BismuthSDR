#include "sdr_device.h"

sdr_device::sdr_device(const char* _name, int _ID, double _samplerate, double _bandwidth, double _center_frequency, device_mode _mode)
{
    name = _name;
    samplerate = _samplerate;
    bandwidth = _bandwidth;
    center_frequency = _center_frequency;
    mode = _mode;
    ID = _ID;
}

sdr_device::~sdr_device()
{
    
}

// get stuff 
// data type         function name                 what variable it returns 
const char*     sdr_device::get_name()              {return name;}
int             sdr_device::get_id()                {return ID;}
double          sdr_device::get_samplerate()        {return samplerate;}
double          sdr_device::get_bandwidth()         {return bandwidth;}
double          sdr_device::get_center_frequency()  {return center_frequency;}
device_mode     sdr_device::get_mode()              {return mode;}

// set stuff
// data type         function name                                   what variable it sets & returns 
bool     sdr_device::set_name(const char* _name)                     {name = _name;                         return true;}
bool     sdr_device::set_id(int _id)                                 {ID = _id;                             return true;}
bool     sdr_device::set_samplerate(double _samplerate)              {samplerate = _samplerate;             return true;}
bool     sdr_device::set_bandwidth(double _bandwidth)                {bandwidth = _bandwidth;               return true;}
bool     sdr_device::set_center_frequency(double _center_frequency)  {center_frequency = _center_frequency; return true;}
bool     sdr_device::set_mode(device_mode _mode)                     {mode = _mode;                         return true;}