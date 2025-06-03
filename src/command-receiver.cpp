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
#include <command-manager/command-manager.hpp>
#include <command-manager/command-observer.hpp>
#include <communications-layer/socket-transport-layer.hpp>
#include <communications-layer/slip-application-layer.hpp>

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

class shutdown_cmd_processor: public command_observer
{
public:
    shutdown_cmd_processor(socket_transport_layer *socket)
        :socket(socket){}

    int process_command() override {
        printf("********************** Received Shutdown Command **********************\r\n");
        socket->stop_listening();
        return 0;
    }
private:
    socket_transport_layer *socket;
};

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    int ret;
    socket_transport_layer *transport_layer;
    slip_application_layer *application_layer;
    slow_cmd_processor *slow_cmd;
    fast_cmd_processor *fast_cmd;
    shutdown_cmd_processor *shutdown_cmd;
    command_manager *cmd_mngr;

    transport_layer = new socket_transport_layer();
    application_layer = new slip_application_layer();
    application_layer->set_next_communications_layer(transport_layer);
    slow_cmd = new slow_cmd_processor();
    fast_cmd = new fast_cmd_processor();
    shutdown_cmd = new shutdown_cmd_processor(transport_layer);
    cmd_mngr = new command_manager(application_layer);
    cmd_mngr->subscribe_slow_cmd(slow_cmd);
    cmd_mngr->subscribe_fast_cmd(fast_cmd);
    cmd_mngr->subscribe_shutdown_cmd(shutdown_cmd);

    ret = transport_layer->listen_connections(SOCKET_NAME, cmd_mngr);
    if (ret == -1)
    {
        exit(EXIT_FAILURE);
    }

    delete cmd_mngr;
    delete transport_layer;
    delete slow_cmd;
    delete shutdown_cmd;
    delete fast_cmd;
    delete slow_cmd;

    exit(EXIT_SUCCESS);
}
