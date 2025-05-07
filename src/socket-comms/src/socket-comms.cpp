#include <socket-comms/socket-comms.hpp>
#include <unistd.h>
#include <stdio.h>

int socket_transport_layer::send(int socket, const void *buf, size_t size)
{
    int ret = write(socket, buf, size);
    if (ret == -1)
    {
        perror("write");
        return ret;
    }
    return 0;
}