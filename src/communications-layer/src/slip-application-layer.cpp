#include <communications-layer/slip-application-layer.hpp>

#include <cstdlib>
#include <cstring>

const uint8_t slip_application_layer::END = 0xC0;
const uint8_t slip_application_layer::ESC = 0xDB;
const uint8_t slip_application_layer::ESC_END = 0xDC;
const uint8_t slip_application_layer::ESC_ESC = 0xDD;

ssize_t slip_application_layer::send(const char *buffer, size_t buffer_size) {
    char *slip_buffer = nullptr;

    size_t slip_size = buffer_size + 1;
    for (size_t i = 0; i < buffer_size; ++i) {
        if ((buffer[i] == (char)END) || (buffer[i] == (char)ESC)) {
            ++slip_size;
        }
    }

    slip_buffer = (char *)malloc(slip_size);
    if (slip_buffer == nullptr) {
        perror("slip_application_layer::send error: malloc");
        return -1;
    }

    size_t slip_index = 0;
    for (size_t i = 0; i < buffer_size; ++i) {
        if (buffer[i] == (char)END) {
            slip_buffer[slip_index++] = ESC;
            slip_buffer[slip_index++] = ESC_END;
        } else if (buffer[i] == (char)ESC) {
            slip_buffer[slip_index++] = ESC;
            slip_buffer[slip_index++] = ESC_ESC;
        } else
            slip_buffer[slip_index++] = buffer[i];
    }
    slip_buffer[slip_size - 1] = END;
    printf("slip_application_layer::send '%s' [%zu / %zu]\r\n", slip_buffer, slip_size, buffer_size);
    ssize_t ret = communications_layer::send(slip_buffer, slip_size);
    free(slip_buffer);
    return ret;
}

ssize_t slip_application_layer::recv(char *buffer, size_t buffer_size) {
    int ret = communications_layer::recv(buffer, buffer_size);
    if (ret < 0) {
        perror("slip_application_layer::recv");
        return ret;
    }

    size_t j = 0;
    for (size_t i = 0; i < buffer_size; i++) {
        if (buffer[i] == (char)END) {
            printf("slip_application_layer::recv '%s' [%zu / %d / %zu ]\r\n", buffer, j, ret, buffer_size);
            return j;
        } else if (buffer[i] == (char)ESC) {
            i = i + 1;
            if (buffer[i] == (char)ESC_ESC)
                buffer[j++] = ESC;
            else if (buffer[i] == (char)ESC_END)
                buffer[j++] = END;
            else {
                printf("slip_application_layer::recv encoding error\r\n");
                return -1;
            }
        } else
            buffer[j++] = buffer[i];
    }
    printf("slip_application_layer::recv missing END error\r\n");
    return -1;
}

int slip_application_layer::shutdown() { return communications_layer::shutdown(); }
