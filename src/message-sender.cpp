#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "message-connection.h"
#include <socket-comms/socket-comms.hpp>

int main(int argc, char *argv[])
{
    int ret;
    int data_socket = 0;
    char buffer[BUFFER_SIZE];
    socket_transport_layer *socket_layer;

    socket_layer = new socket_transport_layer(data_socket);

    ret = socket_layer->connect_remote(SOCKET_NAME);
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    /* Send arguments. */

    for (int i = 1; i < argc; ++i)
    {
        ret = socket_layer->send(argv[i], strlen(argv[i]) + 1);
        if (ret == -1)
        {
            break;
        }
    }

    /* Request result. */

    strcpy(buffer, "END");
    ret = socket_layer->send(buffer, strlen(buffer) + 1);
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    /* Receive result. */

    ret = socket_layer->receive(buffer, sizeof(buffer));
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    /* Ensure buffer is 0-terminated. */

    buffer[sizeof(buffer) - 1] = 0;

    printf("Result = %s\n", buffer);

    /* Close socket. */
    socket_layer->close_connection();
    delete socket_layer;

    exit(EXIT_SUCCESS);
}
