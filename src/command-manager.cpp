/*
 * File server.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/socket.h>
// #include <sys/un.h>
// #include <unistd.h>
#include <thread>

#include "message-connection.h"
#include <command-manager/command-manager.hpp>
#include <command-manager/command-observer.hpp>
#include <communications-layer/communications-layer.hpp>
#include <communications-layer/socket-transport-layer.hpp>
#include <communications-layer/slip-application-layer.hpp>
#include <interactive-console/interactive-console.hpp>
#include <interactive-console/interactive-console-command.hpp>

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

class shutdown_command : public interactive_console_command
{
public:
    shutdown_command(command_manager *cmd_mngr, socket_transport_layer *socket)
        : cmd_mngr(cmd_mngr), socket(socket) {}

    void execute() const override
    {
        int ret = socket->connect_socket(SOCKET_NAME);
        if (ret == -1)
        {
            exit(EXIT_FAILURE);
        }
        cmd_mngr->send_shutdown_cmd();
        socket->disconnect_socket();
    }

private:
    command_manager *cmd_mngr;
    socket_transport_layer *socket;
};

class fast_command : public interactive_console_command
{
public:
    fast_command(command_manager *cmd_mngr, socket_transport_layer *socket)
        : cmd_mngr(cmd_mngr), socket(socket) {}

    void execute() const override
    {
        int ret = socket->connect_socket(SOCKET_NAME);
        if (ret == -1)
        {
            exit(EXIT_FAILURE);
        }
        cmd_mngr->send_fast_cmd();
        socket->disconnect_socket();
    }

private:
    command_manager *cmd_mngr;
    socket_transport_layer *socket;
};

class slow_command : public interactive_console_command
{
public:
    slow_command(command_manager *cmd_mngr, socket_transport_layer *socket)
        : cmd_mngr(cmd_mngr), socket(socket) {}

    void execute() const override
    {
        int ret = socket->connect_socket(SOCKET_NAME);
        if (ret == -1)
        {
            exit(EXIT_FAILURE);
        }
        char hello[] = "Hello World!!!";
        cmd_mngr->send_slow_cmd(hello, sizeof(hello));
        socket->disconnect_socket();
    }

private:
    command_manager *cmd_mngr;
    socket_transport_layer *socket;
};

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    // int ret;
    socket_transport_layer *transport_layer;
    slip_application_layer *application_layer;
    slow_cmd_processor *slow_cmd;
    fast_cmd_processor *fast_cmd;
    shutdown_cmd_processor *shutdown_cmd;
    command_manager *cmd_mngr;

    interactive_console *console;
    interactive_console_command *shutdown_cli_cmd;
    interactive_console_command *fast_cli_cmd;
    interactive_console_command *slow_cli_cmd;

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


    shutdown_cli_cmd = new shutdown_command(cmd_mngr, transport_layer);
    fast_cli_cmd = new fast_command(cmd_mngr, transport_layer);
    slow_cli_cmd = new slow_command(cmd_mngr, transport_layer);

    console = new interactive_console();
    console->set_shutdown_command(shutdown_cli_cmd);
    console->set_fast_command(fast_cli_cmd);
    console->set_slow_command(slow_cli_cmd);

    std::thread t1(&socket_transport_layer::listen_connections, transport_layer, SOCKET_NAME, cmd_mngr);

    console->listen();

    t1.join(); 

    delete cmd_mngr;
    delete transport_layer;
    delete slow_cmd;
    delete shutdown_cmd;
    delete fast_cmd;
    delete console;
    delete shutdown_cli_cmd;
    delete fast_cli_cmd;
    delete slow_cli_cmd;


    exit(EXIT_SUCCESS);
}
