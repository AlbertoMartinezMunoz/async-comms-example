#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "message-connection.h"
#include <command-manager/command-manager.hpp>
#include <communications-layer/communications-layer.hpp>
#include <communications-layer/socket-transport-layer.hpp>
#include <communications-layer/slip-application-layer.hpp>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    int ret;
    char buffer[BUFFER_SIZE];
    socket_transport_layer *transport_layer;
    slip_application_layer *application_layer;
    command_manager *cmd_mngr;

    transport_layer = new socket_transport_layer();
    application_layer = new slip_application_layer();
    application_layer->set_next_send_layer(transport_layer);
    // TBD: add after enabling SLIP in the command-receiver executable
    // transport_layer->set_next_recv_layer(application_layer);
    cmd_mngr = new command_manager(application_layer, transport_layer);

    ret = transport_layer->connect_socket(SOCKET_NAME);
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    ret = cmd_mngr->send_slow_cmd(buffer, sizeof(buffer));
    printf("send_slow_cmd: %d. '%s'\r\n", ret, buffer);

    transport_layer->disconnect_socket();

    delete cmd_mngr;
    delete transport_layer;
    delete application_layer;

    exit(EXIT_SUCCESS);
}
