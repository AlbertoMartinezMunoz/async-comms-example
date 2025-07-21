/*
 * File server.c
 */

#include <stdio.h>
#include <thread>
#include <argp.h>

#include <arguments-parser/arguments-parser.hpp>
#include <command-manager/command-manager.hpp>
#include <command-manager/command-observer.hpp>
#include <communications-layer/communications-layer.hpp>
#include <communications-layer/socket-transport-layer.hpp>
#include <communications-layer/slip-application-layer.hpp>
#include <interactive-commands/interactive-commands.hpp>
#include <interactive-console/interactive-console.hpp>
#include <interactive-console/interactive-console-command.hpp>

class slow_cmd_processor : public command_observer
{
public:
    int process_command() override
    {
        printf("********************** Received Slow Command **********************\r\n");
        return 0;
    }
};

class fast_cmd_processor : public command_observer
{
public:
    int process_command() override
    {
        printf("********************** Received Fast Command **********************\r\n");
        return 0;
    }
};

class shutdown_cmd_processor : public command_observer
{
public:
    shutdown_cmd_processor(socket_transport_layer *socket, interactive_console *console)
        : socket(socket), console(console) {}

    int process_command() override
    {
        printf("********************** Received Shutdown Command **********************\r\n");
        socket->stop_listening();
        console->stop();
        return 0;
    }

private:
    socket_transport_layer *socket;
    interactive_console *console;
};

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    socket_transport_layer *transport_layer;
    slip_application_layer *application_layer;
    slow_cmd_processor *slow_cmd;
    fast_cmd_processor *fast_cmd;
    shutdown_cmd_processor *shutdown_cmd;
    command_manager *cmd_mngr;

    interactive_console *console;
    interactive_console_command *shutdown_local_cmd;
    interactive_console_command *fast_local_cmd;
    interactive_console_command *slow_local_cmd;

    arguments_parser *argparser;

    argparser = new arguments_parser();

    argparser->parse(argc, argv);

    console = interactive_console::get_instance();

    transport_layer = new socket_transport_layer();
    application_layer = new slip_application_layer();
    application_layer->set_next_communications_layer(transport_layer);

    slow_cmd = new slow_cmd_processor();
    fast_cmd = new fast_cmd_processor();
    shutdown_cmd = new shutdown_cmd_processor(transport_layer, console);

    cmd_mngr = new command_manager(application_layer, transport_layer, argparser->get_remote_path());
    cmd_mngr->subscribe_slow_cmd(slow_cmd);
    cmd_mngr->subscribe_fast_cmd(fast_cmd);
    cmd_mngr->subscribe_shutdown_cmd(shutdown_cmd);

    shutdown_local_cmd = new shutdown_command(cmd_mngr, transport_layer);
    fast_local_cmd = new fast_command(cmd_mngr);
    slow_local_cmd = new slow_command(cmd_mngr);

    console->set_shutdown_command(shutdown_local_cmd);
    console->set_fast_command(fast_local_cmd);
    console->set_slow_command(slow_local_cmd);

    std::thread t1(&interactive_console::listen, console);
    if (transport_layer->listen_connections(argparser->get_local_path(), cmd_mngr) != 0)
        console->stop();
    t1.join();

    delete cmd_mngr;
    delete transport_layer;
    delete slow_cmd;
    delete shutdown_cmd;
    delete fast_cmd;
    delete console;
    delete shutdown_local_cmd;
    delete fast_local_cmd;
    delete slow_local_cmd;
    delete argparser;

    exit(EXIT_SUCCESS);
}
