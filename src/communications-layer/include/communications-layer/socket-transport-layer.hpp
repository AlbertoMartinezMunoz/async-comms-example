#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_

#include <communications-layer/communications-layer.hpp>

class socket_transport_layer : public communications_layer
{
public:
    socket_transport_layer() {}

    int connect_socket(const char *socket_path);

    int disconnect_socket();

    int send_message(const void *message, size_t size) override;

    ssize_t send_command(__attribute__((unused)) const char *command, __attribute__((unused)) size_t command_size,
                     __attribute__((unused)) char *response, __attribute__((unused)) size_t reponse_size) override
    {
        return 0;
    }

private:
    int sending_socket;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
