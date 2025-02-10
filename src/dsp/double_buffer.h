#pragma once
#include "types.h"
#include <mutex>
#define standart_size 150000

// sıkıntı reader lock yapıp veriyi alıyo ama en son bufferi bıraktığı gibi geri alıyor reader yazma fırsatı olmadan

/*template <typename T>
class double_buffer
{
    public:
        double_buffer(const char* _name, int size = standart_size){
            name = _name;
            buffer_size = size;
            input = (T*)calloc(size,sizeof(T));
            output = (T*)calloc(size, sizeof(T));
        }

        ~double_buffer(){
            free(input);
            free(output);
        }

        void resize(int size = standart_size){
            buffer_size = size;
            input = (T*)_recalloc(input,size,sizeof(T));
            output = (T*)_recalloc(output,size,sizeof(T));
        }

        void free_all(){
            free(input);
            free(output);
        }

        bool swap(int size = standart_size){
            std::unique_lock<std::mutex> lock(canSwapMutex);
            test.wait(lock, [this]{return !data_ready;});
            data_ready = true;
            buffer_size = size;
            T* temp = input;
            input = output;
            output = temp;
            return true;
            if (canSwapMutex.try_lock()){
                buffer_size = size;
                T* temp = input;
                input = output;
                output = temp;
                canSwapMutex.unlock();
                return true;
            }
            return false;
        }

        int read(){
            canSwapMutex.lock();
            return buffer_size;
        }

        void flush(int len = 0){
            if (len != 0){
            buffer_size = len;
            }
            test.notify_all();
            data_ready = false;
            canSwapMutex.unlock();
        }

    const char* name;
    int buffer_size;
    std::mutex canSwapMutex;
    std::condition_variable test;
    T* input;
    T* output;
    bool data_ready = false;
};*/

template <typename T>
class double_buffer
{
    public:
        double_buffer(const char* _name, int size = standart_size){
            name = _name;
            buffer_size = size;
            input = (T*)calloc(size,sizeof(T));
            output = (T*)calloc(size, sizeof(T));
        }

        ~double_buffer(){
            free(input);
            free(output);
        }

        void resize(int size = standart_size){
            buffer_size = size;
            input = (T*)_recalloc(input,size,sizeof(T));
            output = (T*)_recalloc(output,size,sizeof(T));
        }

        void free_all(){
            free(input);
            free(output);
        }

        bool swap(int size = standart_size){
            if (canSwapMutex.try_lock()){
                buffer_size = size;
                T* temp = input;
                input = output;
                output = temp;
                data_ready = true;
                canSwapMutex.unlock();
                return true;
            }
            return false;
        }

        int read(){
            canSwapMutex.lock();
            return buffer_size;
        }

        void flush(int len = 0){
            if (len != 0){
            buffer_size = len;
            }
            //data_ready = false;
            canSwapMutex.unlock();
        }

    const char* name;
    int buffer_size;
    T* input;
    T* output;
    bool data_ready = false;
    std::mutex canSwapMutex;
};