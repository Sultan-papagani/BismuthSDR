#pragma once 

#include <malloc.h>
#include <mutex>
#include "dsp/complex.h"

class buffer 
{
    #define DEFAULT_SIZE 1000000

    public:

        complex* _input;
        complex* _output;
        std::mutex bufferAc;

        int size = 0;
        bool outputReady = false;
        
        std::condition_variable condReader;
        std::condition_variable condWriter;

        buffer(int buffer_size = 0)
        {
            if (buffer_size == 0){buffer_size = DEFAULT_SIZE;}
            size = sizeof(complex) * buffer_size;
            _input = (complex*)malloc(sizeof(complex) * buffer_size);
            _output = (complex*)malloc(sizeof(complex) * buffer_size);
        }

        ~buffer()
        {
            std::unique_lock<std::mutex> bufferLock(bufferAc);

            free(_input);
            free(_output);
        }

        void swap(int _size)
        {
            if (outputReady){
                return;
            }

            //std::unique_lock<std::mutex> bufferLock(bufferAc);
            size = _size;
            outputReady = true;
            complex* temp = _input;
            _input = _output;
            _output = temp;
            condReader.notify_all();
        }

        int read()
        {
            std::unique_lock<std::mutex> bufferLock(bufferAc);
            condReader.wait(bufferLock, [this]{return outputReady;});
            return size;
        }

        void imDone()
        {
            std::unique_lock<std::mutex> bufferLock(bufferAc);
            outputReady = false;
        }

};
