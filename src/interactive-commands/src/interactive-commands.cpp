#include <interactive-commands/interactive-commands.hpp>
#include <signal.h>

void printresult(int result, const char *message)
{
    switch (result)
    {
    case 0:
        printf("%s ACK received\r\n", message);
        break;
    case 1:
        printf("%s NACK received\r\n", message);
        break;
    default:
        printf("%s Internal error\r\n", message);
        break;
    }
}

shutdown_command::shutdown_command(command_manager *cmd_mngr, socket_transport_layer *socket, const char *socket_path)
    : cmd_mngr(cmd_mngr), socket(socket), socket_path(socket_path) {}

void shutdown_command::execute() const
{
    int ret = socket->connect_socket(socket_path);
    if (ret == -1)
    {
        perror("shutdown_command::execute: connect_socket");
        kill(getpid(), SIGUSR1);
        return;
    }

    printresult(cmd_mngr->send_shutdown_cmd(), "shutdown_command::execute:");
    socket->disconnect_socket();
    kill(getpid(), SIGUSR1);
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
    printresult(cmd_mngr->send_fast_cmd(), "fast_command::execute:");
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
    printresult(cmd_mngr->send_slow_cmd(hello, sizeof(hello)), "slow_command::execute:");
    socket->disconnect_socket();
}
