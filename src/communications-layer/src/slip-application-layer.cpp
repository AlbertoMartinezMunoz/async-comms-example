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
    uint8_t *message_iterator = (uint8_t *)buffer;
    for (size_t i = 0; i < buffer_size; ++i)
    {
        if ((message_iterator[i] == END) || (message_iterator[i] == ESC))
        {
            ++slip_size;
        }
    }

    this->message = (char *)realloc(this->message, slip_size);
    if (this->message == nullptr)
    {
        perror("malloc");
        return -1;
    }

    size_t slip_index = 0;
    for (size_t i = 0; i < buffer_size; ++i)
    {
        if (message_iterator[i] == END)
        {
            this->message[slip_index++] = ESC;
            this->message[slip_index++] = ESC_END;
        }
        else if (message_iterator[i] == ESC)
        {
            this->message[slip_index++] = ESC;
            this->message[slip_index++] = ESC_ESC;
        }
        else
            this->message[slip_index++] = message_iterator[i];
    }
    this->message[slip_size - 1] = END;

    return communications_layer::send(this->message, slip_size);
}

ssize_t slip_application_layer::recv(char *buffer, size_t buffer_size)
{
    uint8_t *message_iterator = (uint8_t *)buffer;
    size_t slip_size = buffer_size;
    for (size_t i = 0; i < buffer_size; ++i)
    {
        if ((message_iterator[i] == END) || (message_iterator[i] == ESC))
        {
            --slip_size;
        }
    }
    this->message = (char *)realloc(this->message, slip_size);
    size_t j = 0;
    for (size_t i = 0; i < buffer_size; i++)
    {
        if (message_iterator[i] == END)
            return communications_layer::recv(this->message, slip_size);
        else if (message_iterator[i] == ESC)
        {
            i = i + 1;
            if (message_iterator[i] == ESC_ESC)
                this->message[j++] = ESC;
            else if (message_iterator[i] == ESC_END)
                this->message[j++] = END;
            else
                return -1;
        }
        else
            this->message[j++] = message_iterator[i];
    }
    return -1;
}

slip_application_layer::~slip_application_layer()
{
    free(this->message);
    this->message = nullptr;
}
