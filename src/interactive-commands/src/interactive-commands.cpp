#include <interactive-commands/interactive-commands.hpp>

shutdown_command::shutdown_command(command_manager *cmd_mngr, socket_transport_layer *socket, const char *socket_path)
    : cmd_mngr(cmd_mngr), socket(socket), socket_path(socket_path) {}

void shutdown_command::execute() const
{
    int ret = socket->connect_socket(socket_path);
    if (ret == -1)
    {
        perror("shutdown_command::execute: connect_socket");
        return;
    }
    cmd_mngr->send_shutdown_cmd();
    socket->disconnect_socket();
    socket->stop_listening();
}

fast_command::fast_command(command_manager *cmd_mngr, socket_transport_layer *socket, const char *socket_path)
    : cmd_mngr(cmd_mngr), socket(socket), socket_path(socket_path) {}

void fast_command::execute() const
{
    int ret = socket->connect_socket(socket_path);
    if (ret == -1)
    {
        perror("fast_command::execute: connect_socket");
        return;
    }
    cmd_mngr->send_fast_cmd();
    socket->disconnect_socket();
}

slow_command::slow_command(command_manager *cmd_mngr, socket_transport_layer *socket, const char *socket_path)
    : cmd_mngr(cmd_mngr), socket(socket), socket_path(socket_path) {}

void slow_command::execute() const
{
    int ret = socket->connect_socket(socket_path);
    if (ret == -1)
    {
        perror("slow_command::execute: connect_socket");
        return;
    }
    char hello[] = "Hello World!!!";
    cmd_mngr->send_slow_cmd(hello, sizeof(hello));
    socket->disconnect_socket();
}
