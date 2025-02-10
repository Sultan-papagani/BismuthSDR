#pragma once

class RtlSdrSource : public drawer_interface
{
    public: 
        RtlSdrSource();
        ~RtlSdrSource();
        void init() override;
        void draw() override;
        void destroy() override;
        const char* get_name() override;
};