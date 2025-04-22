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

        int write(T* buf, int len)
        {
            int write_clamped = 0;
            int write_len = len;
            if (writer_index + len > buffer_size)
            {
                write_len = buffer_size - writer_index;
                write_clamped = abs(write_len - len);
            }

            // normal yaz.
            memcpy(&buffer[writer_index], buf, write_len);

            writer_index += write_len;

            // kalanı yaz
            if (write_clamped)
            {
                writer_index = write_clamped;
                memcpy(buffer, &buf[write_len], write_clamped);
            }
        }

        void read(T* buf, int len)
        {
            int read_clamped = 0;
            int read_len = len;

            // overflow şeysi
            if (read_len + len > buffer_size)
            {
                read_len = buffer_size - reader_index;
                read_clamped = abs(read_len - len);
            }

            // writer'ın arkasındamıyız kontrolü
            
        }

        void flush()
        {

        }

    T* buffer;
    int buffer_size;
    int reader_index;
    int writer_index;
};