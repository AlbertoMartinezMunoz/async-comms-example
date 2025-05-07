#include <socket-comms/socket-comms.hpp>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

socket_transport_layer::socket_transport_layer(int socket)
    : data_socket(socket) {}

int socket_transport_layer::connect_remote(const char *server)
{
    struct sockaddr_un addr;
    int ret;

    data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (data_socket == -1)
    {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, server, sizeof(addr.sun_path) - 1);
    ret = connect(data_socket, (const struct sockaddr *)&addr,
                  sizeof(addr));
    if (ret == -1)
    {
        perror("The server is down.\n");
        return -1;
    }
    return 0;
}

void socket_transport_layer::close_connection()
{
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
