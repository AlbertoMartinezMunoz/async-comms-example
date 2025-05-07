#include <socket-comms/socket-comms.hpp>
#include <unistd.h>
#include <stdio.h>

socket_transport_layer::socket_transport_layer(int socket)
    : data_socket(socket) {}

void socket_transport_layer::close_connection() {
    close(data_socket);
}

int socket_transport_layer::send(const void *buf, size_t size)
{
    int ret = write(data_socket, buf, size);
    if (ret == -1)
    {
        perror("write");
        return ret;
    }
    return 0;
}

int socket_transport_layer::receive(void *buf, size_t size)
{
    int ret = read(data_socket, buf, size);
    if (ret == -1)
    {
        perror("read");
        return ret;
    }
    return ret;
}