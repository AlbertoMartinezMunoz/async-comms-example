/*
 * File server.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "message-connection.h"
#include <socket-comms/socket-comms.hpp>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    struct sockaddr_un name;
    int ret;
    int connection_socket;
    char buffer[BUFFER_SIZE];
    socket_transport_layer *socket_layer;

    /* Create local socket. */

    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1)
    {
        perror("socket");
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
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    ret = bind(connection_socket, (const struct sockaddr *)&name,
               sizeof(name));
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */

    ret = listen(connection_socket, 20);
    if (ret == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* This is the main loop for handling connections. */
    socket_layer = new socket_transport_layer(0);
    if (socket_layer->listen_connections(connection_socket, SOCKET_NAME) == -1)
    {
        exit(EXIT_FAILURE);
    }

    /* Wait for next data packet. */
    ret = socket_layer->receive(buffer, sizeof(buffer));
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    buffer[sizeof(buffer) - 1] = 0;
    printf("Receiver: %s", buffer);

    sprintf(buffer, "ACK");
    ret = socket_layer->send(buffer, sizeof(buffer));
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    sleep(1);
    /* Close socket. */
    socket_layer->close_connection();
    delete socket_layer;

    close(connection_socket);

    /* Unlink the socket. */

    unlink(SOCKET_NAME);

    exit(EXIT_SUCCESS);
}
