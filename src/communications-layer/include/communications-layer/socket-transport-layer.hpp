#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_

#include <communications-layer/communications-layer.hpp>
#include <communications-layer/communications-observer.hpp>

class socket_transport_layer : public communications_layer
{
public:
    socket_transport_layer();

    ~socket_transport_layer();
    
    int connect_socket(const char *socket_path);

    int disconnect_socket();

    int listen_connections(const char *socket_path, communications_layer_observer *in_msg_observer);

    void stop_listening();

    ssize_t send(const char *buffer, size_t buffer_size) override;

    ssize_t recv(char *buffer, size_t buffer_size) override;

private:
    int sending_socket;
    int listening_socket;
    bool is_listening;
    char *socket_path;

    int wakeuppfd[2];
};

#endif  // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
