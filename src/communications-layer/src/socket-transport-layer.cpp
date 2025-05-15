#include <communications-layer/socket-transport-layer.hpp>

#include <cstdio>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdlib>
#include <unistd.h>

int socket_transport_layer::connect_socket(const char *socket_path)
{
    struct sockaddr_un addr;

    sending_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (sending_socket == -1)
    {
        perror("socket");
        return (-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    int ret = connect(sending_socket, (const struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        fprintf(stderr, "The server is down.\n");
        return (-1);
    }

    return 0;
}

int socket_transport_layer::disconnect_socket()
{
    close(sending_socket);

    return 0;
}

ssize_t socket_transport_layer::send(const char *buffer, size_t buffer_size)
{
    printf("socket_transport_layer: send_message '%s'\r\n", buffer);
    int ret = write(sending_socket, buffer, buffer_size);
    if (ret == -1)
    {
        perror("write");
        return (-1);
    }
    return communications_layer::send(buffer, ret);
}

ssize_t socket_transport_layer::recv(char *buffer, size_t buffer_size)
{
    int ret = read(sending_socket, buffer, buffer_size);
    if (ret == -1)
    {
        perror("read");
        return (-1);
    }
    printf("socket_transport_layer: recv_message '%s'\r\n", buffer);
    return communications_layer::recv(buffer, ret);
}
