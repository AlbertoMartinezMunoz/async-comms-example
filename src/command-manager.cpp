/*
 * File server.c
 */

#include <argp.h>
#include <memory>
#include <stdio.h>
#include <thread>

#include <arguments-parser/arguments-parser.hpp>
#include <command-manager/command-local-handler.hpp>
#include <command-manager/command-remote-handler.hpp>
#include <commands/commands-implementations.hpp>
#include <communications-layer/communications-layer.hpp>
#include <communications-layer/local-tcp-socket-client-transport-layer.hpp>
#include <communications-layer/local-tcp-socket-listener-transport-layer.hpp>
#include <communications-layer/slip-application-layer.hpp>
#include <communications-layer/socket-transport-layer.hpp>
#include <interactive-console/interactive-console.hpp>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
    std::unique_ptr<local_tcp_socket_listener_transport_layer> remote_listener;
    std::unique_ptr<slip_application_layer> remote_comms;

    std::unique_ptr<local_tcp_socket_client_transport_layer> client;
    std::unique_ptr<slip_application_layer> client_comms;

    socket_transport_layer *transport_layer;
    slip_application_layer *application_layer;
    remote_slow_command *remote_slow_cmd;
    remote_fast_command *remote_fast_cmd;
    remote_shutdown_command *remote_shutdown_cmd;
    command_remote_handler *remote_cmd_handler;
    command_local_handler *local_cmd_handler;

    interactive_console *console;
    local_shutdown_command *shutdown_local_cmd;
    local_fast_command *fast_local_cmd;
    local_slow_command *slow_local_cmd;

    arguments_parser *argparser;

    argparser = new arguments_parser();

    argparser->parse(argc, argv);

    console = interactive_console::get_instance();

    remote_listener = std::make_unique<local_tcp_socket_listener_transport_layer>(argparser->get_local_path());
    remote_comms = std::make_unique<slip_application_layer>();
    remote_comms->set_next_communications_layer(remote_listener.get());

    client = std::make_unique<local_tcp_socket_client_transport_layer>(argparser->get_remote_path());
    client_comms = std::make_unique<slip_application_layer>();
    client_comms->set_next_communications_layer(client.get());

    transport_layer = new socket_transport_layer(argparser->get_remote_path());
    application_layer = new slip_application_layer();
    application_layer->set_next_communications_layer(transport_layer);

    remote_slow_cmd = new remote_slow_command();
    remote_fast_cmd = new remote_fast_command();
    remote_shutdown_cmd = new remote_shutdown_command(remote_listener.get(), console);

    remote_cmd_handler = new command_remote_handler(remote_comms.get());
    remote_cmd_handler->subscribe_slow_cmd(remote_slow_cmd);
    remote_cmd_handler->subscribe_fast_cmd(remote_fast_cmd);
    remote_cmd_handler->subscribe_shutdown_cmd(remote_shutdown_cmd);

    shutdown_local_cmd = new local_shutdown_command(client_comms.get(), client.get(), remote_listener.get(), console);
    fast_local_cmd = new local_fast_command(client_comms.get(), client.get());
    slow_local_cmd = new local_slow_command(client_comms.get(), client.get());

    local_cmd_handler = new command_local_handler(console);
    local_cmd_handler->subscribe_fast_cmd(fast_local_cmd);
    local_cmd_handler->subscribe_slow_cmd(slow_local_cmd);
    local_cmd_handler->subscribe_shutdown_cmd(shutdown_local_cmd);

    std::thread t1(&interactive_console::listen, console, local_cmd_handler);
    if (remote_listener->listen(remote_cmd_handler) != 0)
        console->shutdown();
    t1.join();

    delete transport_layer;
    delete remote_slow_cmd;
    delete remote_shutdown_cmd;
    delete remote_fast_cmd;
    delete console;
    delete shutdown_local_cmd;
    delete fast_local_cmd;
    delete slow_local_cmd;
    delete argparser;

    exit(EXIT_SUCCESS);
}
