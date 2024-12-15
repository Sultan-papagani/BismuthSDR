#include <malloc.h>
#include <mutex>

class buffer 
{
    #define DEFAULT_SIZE 1000000

    public:

        float* _buffer;
        std::mutex bufferAc;
        std::condition_variable readable;
        std::condition_variable writeable;
        bool dataReady = false;
        int size = 0;

        // constructor
        buffer()
        {
            _buffer = (float*)malloc(sizeof(float) * DEFAULT_SIZE);
        }

        // de-constructor
        ~buffer()
        {
            std::lock_guard<std::mutex> bufferLock(bufferAc);
            free(_buffer);
        }

        void DoneWriting()
        {
            readable.notify_all();
        }

        int Read()
        {
            std::unique_lock<std::mutex> bufferLock(bufferAc);
            readable.wait(bufferLock, [this] { return (dataReady); });
            return size;
        }

};
