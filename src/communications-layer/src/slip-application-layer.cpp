#include <communications-layer/slip-application-layer.hpp>

#include <cstdlib>
#include <cstring>

const uint8_t slip_application_layer::END = 0xC0;
const uint8_t slip_application_layer::ESC = 0xDB;
const uint8_t slip_application_layer::ESC_END = 0xDC;
const uint8_t slip_application_layer::ESC_ESC = 0xDD;

ssize_t slip_application_layer::send(const char *buffer, size_t buffer_size)
{
    size_t slip_size = buffer_size + 1;
    for (size_t i = 0; i < buffer_size; ++i)
    {
        if ((buffer[i] == (char)END) || (buffer[i] == (char)ESC))
        {
            ++slip_size;
        }
    }

    this->message = (char *)realloc(this->message, slip_size);
    if (this->message == nullptr)
    {
        printf("slip_application_layer::send error\r\n");
        perror("malloc");
        return -1;
    }

    size_t slip_index = 0;
    for (size_t i = 0; i < buffer_size; ++i)
    {
        if (buffer[i] == (char)END)
        {
            this->message[slip_index++] = ESC;
            this->message[slip_index++] = ESC_END;
        }
        else if (buffer[i] == (char)ESC)
        {
            this->message[slip_index++] = ESC;
            this->message[slip_index++] = ESC_ESC;
        }
        else
            this->message[slip_index++] = buffer[i];
    }
    this->message[slip_size - 1] = END;
    printf("slip_application_layer::send '%s' [%zu / %zu]\r\n", this->message, slip_size, buffer_size);
    return communications_layer::send(this->message, slip_size);
}

ssize_t slip_application_layer::recv(char *buffer, size_t buffer_size)
{
    int ret = communications_layer::recv(buffer, buffer_size);
    if (ret < 0)
    {
        perror("recv");
        return ret;
    }

    size_t j = 0;
    for (size_t i = 0; i < buffer_size; i++)
    {
        if (buffer[i] == (char)END)
        {
            printf("slip_application_layer::recv '%s' [%zu / %d / %zu ]\r\n", buffer, j, ret, buffer_size);
            return j;
        }
        else if (buffer[i] == (char)ESC)
        {
            i = i + 1;
            if (buffer[i] == (char)ESC_ESC)
                buffer[j++] = ESC;
            else if (buffer[i] == (char)ESC_END)
                buffer[j++] = END;
            else
            {
                printf("slip_application_layer::recv encoding error\r\n");
                return -1;
            }
        }
        else
            buffer[j++] = buffer[i];
    }
    printf("slip_application_layer::recv missing END error\r\n");
    return -1;
}

slip_application_layer::~slip_application_layer()
{
    free(this->message);
    this->message = nullptr;
}
