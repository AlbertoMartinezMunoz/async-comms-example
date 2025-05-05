#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_

#include <communications-layer/communications-layer.hpp>

class socket_transport_layer : public communications_layer
{
public:
    socket_transport_layer() {}

    int connect_socket(const char *socket_path);

    int disconnect_socket();

    int send_message(uint8_t *message, size_t size) override;

private:
    int sending_socket;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
