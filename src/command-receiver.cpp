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
// #include <socket-comms/socket-comms.hpp>
#include <command-manager/command-manager.hpp>
#include <command-manager/command-observer.hpp>
#include <communications-layer/socket-transport-layer.hpp>

class slow_cmd_processor: public command_observer
{
public:
    int process_command() override {
        printf("********************** Received Slow Command **********************\r\n");
        return 0;
    }
};

class fast_cmd_processor: public command_observer
{
public:
    int process_command() override {
        printf("********************** Received Fast Command **********************\r\n");
        return 0;
    }
};

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    int ret;
    // char buffer[BUFFER_SIZE];
    socket_transport_layer *transport_layer;
    // slip_application_layer *application_layer;
    slow_cmd_processor *slow_cmd;
    fast_cmd_processor *fast_cmd;
    command_manager *cmd_mngr;

    transport_layer = new socket_transport_layer();
    // application_layer = new slip_application_layer();
    // application_layer->set_next_send_layer(transport_layer);
    slow_cmd = new slow_cmd_processor();
    fast_cmd = new fast_cmd_processor();
    // TBD: add after enabling SLIP in the command-receiver executable
    // transport_layer->set_next_recv_layer(application_layer);
    cmd_mngr = new command_manager(transport_layer, transport_layer);
    cmd_mngr->subscribe_slow_cmd(slow_cmd);
    cmd_mngr->subscribe_fast_cmd(fast_cmd);

    ret = transport_layer->listen_connections(SOCKET_NAME, cmd_mngr);
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    transport_layer->disconnect();
    delete cmd_mngr;
    delete transport_layer;
    delete slow_cmd;

    exit(EXIT_SUCCESS);
}
