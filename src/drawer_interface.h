#pragma once

class drawer_interface{
    public:

        ~drawer_interface() = default;
        virtual void init() = 0;
        virtual void draw() = 0;
        virtual void destroy() = 0;
        virtual const char* get_name() = 0;
        
};