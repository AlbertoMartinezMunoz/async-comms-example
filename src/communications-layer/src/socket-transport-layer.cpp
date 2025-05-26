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

int socket_transport_layer::disconnect()
{
    close(listening_socket);
    unlink(socket_path);
    free(socket_path);

    return 0;
}

int socket_transport_layer::listen_connections(
    const char *path,
    communications_layer_observer *in_msg_observer)
{
    struct sockaddr_un name;

    socket_path = strdup(path);

    /* Create local socket. */
    listening_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (listening_socket == -1)
    {
        perror("listening_socket");
        exit(EXIT_FAILURE);
    }

    /*
     * For portability clear the whole structure, since some
     * implementations have additional (nonstandard) fields in
     * the structure.
     */
    memset(&name, 0, sizeof(name));

    /* Bind socket to socket name. */
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, socket_path, sizeof(name.sun_path) - 1);

    int ret = bind(listening_socket, (const struct sockaddr *)&name,
                   sizeof(name));
    if (ret == -1)
    {
        perror("bind");
        close(listening_socket);
        unlink(socket_path);
        exit(EXIT_FAILURE);
    }

    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */

    ret = listen(listening_socket, 20);
    if (ret == -1)
    {
        perror("listen");
        close(listening_socket);
        unlink(socket_path);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 5; i++)
    {
        sending_socket = accept(listening_socket, NULL, NULL);
        if (sending_socket == -1)
        {
            perror("accept");
            return -1;
        }

        in_msg_observer->incoming_message();

        sleep(1);
        close(sending_socket);
    }

    printf("Stop Listening\r\n");

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
    int ret = communications_layer::recv(buffer, buffer_size);
    if (ret < 0)
    {
        perror("recv");
        return ret;
    }

    ret = read(sending_socket, buffer, buffer_size);
    if (ret == -1)
    {
        perror("read");
    }
    printf("socket_transport_layer: recv_message '%s'\r\n", buffer);

    return ret;
}
