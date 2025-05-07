#ifndef IOT_MICRO_FIRMWARE_SRC_SOCKET_COMMS_INCLUDE_SOCKET_COMMS_SOCKET_COMMS_H_
#define IOT_MICRO_FIRMWARE_SRC_SOCKET_COMMS_INCLUDE_SOCKET_COMMS_SOCKET_COMMS_H_

#include <stddef.h>

class socket_transport_layer
{
public:
    socket_transport_layer() {};

    ~socket_transport_layer() {};

    int send(int socket, const void *buf, size_t size);

    int receive(int socket, void *buf, size_t size);
};

#endif // IOT_MICRO_FIRMWARE_SRC_SOCKET_COMMS_INCLUDE_SOCKET_COMMS_SOCKET_COMMS_H_
