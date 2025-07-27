#include <interactive-commands/interactive-commands.hpp>

void printresult(int result, const char *message) {
    switch (result) {
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

shutdown_command::shutdown_command(command_manager *cmd_mngr, socket_transport_layer *socket, interactive_console *cli)
    : cmd_mngr(cmd_mngr), socket(socket), cli(cli) {}

void shutdown_command::execute() const {
    printf("shutdown_command::execute\r\n");
    printresult(cmd_mngr->send_shutdown_cmd(), "shutdown_command::execute:");
    socket->shutdown();
    cli->shutdown();
}

fast_command::fast_command(command_manager *cmd_mngr) : cmd_mngr(cmd_mngr) {}

void fast_command::execute() const {
    printf("fast_command::execute\r\n");
    printresult(cmd_mngr->send_fast_cmd(), "fast_command::execute:");
}

slow_command::slow_command(command_manager *cmd_mngr) : cmd_mngr(cmd_mngr) {}

void slow_command::execute() const {
    printf("slow_command::execute\r\n");
    char hello[] = "Hello World!!!";
    printresult(cmd_mngr->send_slow_cmd(hello, sizeof(hello)), "slow_command::execute:");
}
