#include <communications-layer/slip-transport-layer.hpp>

int slip_transport_layer::send_message(uint8_t *message) {
    printf("slip_transport_layer: send_message '%s'\r\n", message);
    return communications_layer::send_message(message);
}