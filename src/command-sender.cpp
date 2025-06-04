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
#include <interactive-console/interactive-console.hpp>
#include <interactive-console/interactive-console-command.hpp>
#include <interactive-commands/interactive-commands.hpp>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    socket_transport_layer *transport_layer;
    slip_application_layer *application_layer;
    command_manager *cmd_mngr;

    interactive_console *console;
    interactive_console_command *shutdown_cmd;
    interactive_console_command *fast_cmd;
    interactive_console_command *slow_cmd;

    transport_layer = new socket_transport_layer();
    application_layer = new slip_application_layer();
    application_layer->set_next_communications_layer(transport_layer);
    cmd_mngr = new command_manager(application_layer);

    shutdown_cmd = new shutdown_command(cmd_mngr, transport_layer, SOCKET_NAME);
    fast_cmd = new fast_command(cmd_mngr, transport_layer, SOCKET_NAME);
    slow_cmd = new slow_command(cmd_mngr, transport_layer, SOCKET_NAME);

    console = new interactive_console();
    console->set_shutdown_command(shutdown_cmd);
    console->set_fast_command(fast_cmd);
    console->set_slow_command(slow_cmd);

    console->listen();

    delete console;
    delete cmd_mngr;
    delete transport_layer;
    delete application_layer;
    delete shutdown_cmd;
    delete fast_cmd;
    delete slow_cmd;

    exit(EXIT_SUCCESS);
}
