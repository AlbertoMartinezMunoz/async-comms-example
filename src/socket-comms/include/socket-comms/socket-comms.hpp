#ifndef IOT_MICRO_FIRMWARE_SRC_SOCKET_COMMS_INCLUDE_SOCKET_COMMS_SOCKET_COMMS_H_
#define IOT_MICRO_FIRMWARE_SRC_SOCKET_COMMS_INCLUDE_SOCKET_COMMS_SOCKET_COMMS_H_

#include <stddef.h>

class socket_transport_layer
{
public:
    socket_transport_layer(int socket);

    ~socket_transport_layer() {};

    int connect_remote(const char *server);

    int listen_connections(int connection_socket, const char *server);

    void close_connection();

    int send(const void *buf, size_t size);

    int receive(void *buf, size_t size);

private:
    int data_socket;
};

#endif // IOT_MICRO_FIRMWARE_SRC_SOCKET_COMMS_INCLUDE_SOCKET_COMMS_SOCKET_COMMS_H_
