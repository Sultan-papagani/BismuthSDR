#pragma once
#define standart_size 100000
template <typename T>
class simit_buffer
{
    public:
        simit_buffer(int size = standart_size)
        {
            buffer_size = size;
            buffer = (T*)calloc(size,sizeof(T));
        }

        ~simit_buffer()
        {
            free(buffer);
        }

        void resize(int size)
        {
            buffer_size = size;
            free(buffer);
            buffer = (T*)calloc(size,sizeof(T));
        }

        int start_write()
        {
            _mutex.lock();
            return writer_index;
        }

        void end_write()
        {
            _mutex.unlock();
        }


        void read()
        {

        }

        void flush()
        {

        }

    T* buffer;
    int buffer_size;
    int reader_index;
    int writer_index;
    std::mutex _mutex;
};