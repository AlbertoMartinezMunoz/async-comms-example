#include <communications-layer/socket-application-layer.hpp>

#include <cstdio>

int socket_application_layer::send_message(uint8_t *message, size_t size) {
    printf("socket_application_layer: send_message '%s'\r\n", message);
    return communications_layer::send_message(message, size);
}