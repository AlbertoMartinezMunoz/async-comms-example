#include <communications-layer/slip-application-layer.hpp>

#include <cstdlib>
#include <cstring>

const uint8_t slip_application_layer::END = 0xC0;
const uint8_t slip_application_layer::ESC = 0xDB;
const uint8_t slip_application_layer::ESC_END = 0xDC;
const uint8_t slip_application_layer::ESC_ESC = 0xDD;

int slip_application_layer::send_message(uint8_t *message, size_t size) {
    size_t slip_size = size + 1;
    for(size_t i = 0; i < size; ++i)
    {
        if((message[i] == 0xC0) || (message[i] == 0xDB))
            ++slip_size;
    }

    this->message = (uint8_t *)realloc(this->message, slip_size);
    if(this->message == nullptr)
    {
        perror("malloc");
        return -1;
    }

    size_t slip_index = 0;
    for(size_t i = 0; i < size; ++i)
    {
        if(message[i] == END) {
            this->message[slip_index++] = ESC;
            this->message[slip_index++] = ESC_END;
        } else if(message[i] == ESC) {
            this->message[slip_index++] = ESC;
            this->message[slip_index++] = ESC_ESC;
        }
        else
            this->message[slip_index++] = message[i];
    }
    this->message[slip_size - 1] = END;

    return communications_layer::send_message(this->message, slip_size);
}

slip_application_layer::~slip_application_layer(){
    free(this->message);
    this->message = nullptr;
}