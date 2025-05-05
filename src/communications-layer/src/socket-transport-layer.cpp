#include <communications-layer/socket-transport-layer.hpp>

#include <cstdio>

int socket_transport_layer::send_message(uint8_t *message, size_t size) {
    printf("socket_transport_layer: send_message '%s'\r\n", message);
    return communications_layer::send_message(message, size);
}